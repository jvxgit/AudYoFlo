#include <stdint.h>

#include "include/my_pixel_buffer_texture.h"

G_DEFINE_TYPE(MyPixelBufferTexture, my_pixel_buffer_texture,
              fl_pixel_buffer_texture_get_type())

static gboolean my_texture_copy_pixels(FlPixelBufferTexture *texture,
                                       const uint8_t **out_buffer,
                                       uint32_t *width, uint32_t *height,
                                       GError **error) {
  MyPixelBufferTexture *self = MY_PIXEL_BUFFER_TEXTURE(texture);
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
  return object;
}

static void my_pixel_buffer_texture_dispose(GObject *object) {
  MyPixelBufferTexture *self = MY_PIXEL_BUFFER_TEXTURE(object);
  if (self->queue)
    g_async_queue_unref(self->queue);
  free(self->buffer1);
  free(self->buffer2);
  G_OBJECT_CLASS(my_pixel_buffer_texture_parent_class)->dispose(object);
}

static void
my_pixel_buffer_texture_class_init(MyPixelBufferTextureClass *klass) {
  FL_PIXEL_BUFFER_TEXTURE_CLASS(klass)->copy_pixels = my_texture_copy_pixels;
  G_OBJECT_CLASS(klass)->dispose = my_pixel_buffer_texture_dispose;
}

static void my_pixel_buffer_texture_init(MyPixelBufferTexture *self) {}
