#include <stdint.h>
#include <string.h>

#include "include/my_pixel_buffer_texture.h"

G_DEFINE_TYPE(MyPixelBufferTexture, my_pixel_buffer_texture,
              fl_pixel_buffer_texture_get_type())

static gboolean my_texture_copy_pixels(FlPixelBufferTexture *texture,
                                       const uint8_t **out_buffer,
                                       uint32_t *width, uint32_t *height,
                                       GError **error) {
  MyPixelBufferTexture *self = MY_PIXEL_BUFFER_TEXTURE(texture);
  if (g_atomic_int_get(&self->published)) {
    // Flutter copies the returned buffer before the next call, hence, the
    // front buffer is only modified within this function.
    g_mutex_lock(&self->publish_mutex);
    if (self->publish_back_valid) {
      guint8 *tmp = self->publish_front;
      gsize tmp_size = self->publish_front_size;
      self->publish_front = self->publish_back;
      self->publish_front_size = self->publish_back_size;
      self->publish_front_width = self->publish_back_width;
      self->publish_front_height = self->publish_back_height;
      self->publish_back = tmp;
      self->publish_back_size = tmp_size;
      self->publish_back_valid = FALSE;
    }
    *out_buffer = self->publish_front;
    *width = self->publish_front_width;
    *height = self->publish_front_height;
    g_mutex_unlock(&self->publish_mutex);
    return (*out_buffer != NULL);
  }

  *width = self->width;
  *height = self->height;
  if (self->activate_swap) {
    if (self->buffer == self->buffer1) {
      self->buffer = self->buffer2;
      *out_buffer = self->buffer1;
    } else {
      self->buffer = self->buffer1;
      *out_buffer = self->buffer2;
    }
  } else {
    if (self->buffer == self->buffer1) {
      *out_buffer = self->buffer2;
    } else {
      *out_buffer = self->buffer1;
    }
  }
  g_async_queue_push(self->queue, GINT_TO_POINTER(1));
  return TRUE; // TODO maybe return FALSE if !self->activate_swap
}

MyPixelBufferTexture *
my_pixel_buffer_texture_new(guint32 width, guint32 height,
                            FlTextureRegistrar *texture_registrar) {
  MyPixelBufferTexture *object = MY_PIXEL_BUFFER_TEXTURE(
      g_object_new(my_pixel_buffer_texture_get_type(), NULL));

  object->buffer1 =
      (guint8 *)malloc(width * height * 4 * sizeof(guint8)); // RGBA
  object->buffer2 =
      (guint8 *)malloc(width * height * 4 * sizeof(guint8)); // RGBA
  object->buffer = object->buffer1;
  object->width = width;
  object->height = height;
  object->texture_registrar = texture_registrar;
  object->queue = g_async_queue_new();
  object->activate_swap = FALSE;

  object->published = 0;
  g_mutex_init(&object->publish_mutex);
  object->publish_front = NULL;
  object->publish_back = NULL;
  object->publish_front_size = 0;
  object->publish_back_size = 0;
  object->publish_front_width = 0;
  object->publish_front_height = 0;
  object->publish_back_width = 0;
  object->publish_back_height = 0;
  object->publish_back_valid = FALSE;
  return object;
}

void my_pixel_buffer_texture_publish(MyPixelBufferTexture *self,
                                     const guint8 *rgba, guint32 width,
                                     guint32 height) {
  gsize sz = (gsize)width * height * 4;
  g_mutex_lock(&self->publish_mutex);
  if (self->publish_back_size != sz) {
    self->publish_back = (guint8 *)g_realloc(self->publish_back, sz);
    self->publish_back_size = sz;
  }
  memcpy(self->publish_back, rgba, sz);
  self->publish_back_width = width;
  self->publish_back_height = height;
  self->publish_back_valid = TRUE;
  g_mutex_unlock(&self->publish_mutex);
  g_atomic_int_set(&self->published, 1);
  fl_texture_registrar_mark_texture_frame_available(self->texture_registrar,
                                                    FL_TEXTURE(self));
}

static void my_pixel_buffer_texture_dispose(GObject *object) {
  MyPixelBufferTexture *self = MY_PIXEL_BUFFER_TEXTURE(object);
  if (self->queue)
    g_async_queue_unref(self->queue);
  free(self->buffer1);
  free(self->buffer2);
  self->buffer1 = NULL;
  self->buffer2 = NULL;
  g_free(self->publish_front);
  g_free(self->publish_back);
  self->publish_front = NULL;
  self->publish_back = NULL;
  G_OBJECT_CLASS(my_pixel_buffer_texture_parent_class)->dispose(object);
}

static void
my_pixel_buffer_texture_class_init(MyPixelBufferTextureClass *klass) {
  FL_PIXEL_BUFFER_TEXTURE_CLASS(klass)->copy_pixels = my_texture_copy_pixels;
  G_OBJECT_CLASS(klass)->dispose = my_pixel_buffer_texture_dispose;
  G_OBJECT_CLASS(klass)->finalize = my_pixel_buffer_texture_finalize;
}

static void my_pixel_buffer_texture_finalize(GObject *object) {
  MyPixelBufferTexture *self = MY_PIXEL_BUFFER_TEXTURE(object);
  g_mutex_clear(&self->publish_mutex);
  G_OBJECT_CLASS(my_pixel_buffer_texture_parent_class)->finalize(object);
}

static void my_pixel_buffer_texture_init(MyPixelBufferTexture *self) {}
