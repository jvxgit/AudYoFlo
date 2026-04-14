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
};

MyPixelBufferTexture *
my_pixel_buffer_texture_new(guint32 width, guint32 height,
                            FlTextureRegistrar *texture_registrar);

#endif // FLUTTER_PLUGIN_MY_PIXEL_BUFFER_TEXTURE_H_
