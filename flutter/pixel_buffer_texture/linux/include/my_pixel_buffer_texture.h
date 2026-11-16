#ifndef FLUTTER_PLUGIN_MY_PIXEL_BUFFER_TEXTURE_H_
#define FLUTTER_PLUGIN_MY_PIXEL_BUFFER_TEXTURE_H_

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>

#include "pixel_buffer_texture/pixel_buffer_texture_plugin.h"

#define MY_PIXEL_BUFFER_TEXTURE(obj)                                           \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), my_pixel_buffer_texture_get_type(),       \
                              MyPixelBufferTexture))

G_DECLARE_FINAL_TYPE(MyPixelBufferTexture, my_pixel_buffer_texture, MY,
                     MY_PIXEL_BUFFER_TEXTURE, FlPixelBufferTexture)

struct _MyPixelBufferTexture {
  FlPixelBufferTexture parent_instance;
  guint8 *buffer1;
  guint8 *buffer2;
  guint8 *buffer;
  guint32 width;
  guint32 height;
  FlTextureRegistrar *texture_registrar;
  GAsyncQueue *queue;
  // Flag to indicate if buffer swapping is required or not
  gboolean activate_swap;

  // Renderers that produce complete images (e.g. plots) hand them over via
  // my_pixel_buffer_texture_publish. The size may differ from the creation size.
  gint published;
  GMutex publish_mutex;
  guint8 *publish_front;
  guint8 *publish_back;
  gsize publish_front_size;
  gsize publish_back_size;
  guint32 publish_front_width;
  guint32 publish_front_height;
  guint32 publish_back_width;
  guint32 publish_back_height;
  gboolean publish_back_valid;
};

MyPixelBufferTexture *
my_pixel_buffer_texture_new(guint32 width, guint32 height,
                            FlTextureRegistrar *texture_registrar);

// Called from the render thread of the backend, does not block
void my_pixel_buffer_texture_publish(MyPixelBufferTexture *self,
                                     const guint8 *rgba, guint32 width,
                                     guint32 height);

#endif // FLUTTER_PLUGIN_MY_PIXEL_BUFFER_TEXTURE_H_
