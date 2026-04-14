#ifndef FLUTTER_PLUGIN_PIXEL_BUFFER_TEXTURE_PLUGIN_H_
#define FLUTTER_PLUGIN_PIXEL_BUFFER_TEXTURE_PLUGIN_H_

//#define JVX_COMPONENT_ACCESS_CALLING_CONVENTION
//#include "jvx_sdk.h"
//#include "jvx.h"
#define JVX_COMPONENT_ACCESS_CALLING_CONVENTION 
#include "jvx_sdk.h"
#include "jvx.h"
#include "typedefs/TpjvxPixBuffer.h"

#include <flutter_linux/flutter_linux.h>

G_BEGIN_DECLS

#ifdef FLUTTER_PLUGIN_IMPL
#define FLUTTER_PLUGIN_EXPORT __attribute__((visibility("default")))
#else
#define FLUTTER_PLUGIN_EXPORT
#endif

FLUTTER_PLUGIN_EXPORT GType pixel_buffer_texture_plugin_get_type();

FLUTTER_PLUGIN_EXPORT void pixel_buffer_texture_plugin_register_with_registrar(
    FlPluginRegistrar *registrar);

/*
typedef gboolean (*PixelBufferTexturePluginFrameAvailableCallback)(
    gint64 texture_id, gboolean reqSwap);
typedef void (*PixelBufferTexturePluginNotifyCallback)(
    gint64 texture_id);
typedef void (*PixelBufferTexturePluginCreateRendererCallback)(
    gint64 texture_id, guint8 **buffer, guint32 width, guint32 height,
    const char *id, const char *arg,
    PixelBufferTexturePluginFrameAvailableCallback frame_available_cb,
    PixelBufferTexturePluginNotifyCallback notify_cb,
    gpointer priv_data);
typedef void (*PixelBufferTexturePluginDestroyRendererCallback)(
    gint64 texture_id, gpointer priv_data);
*/
FLUTTER_PLUGIN_EXPORT void pixel_buffer_texture_plugin_set_renderer_callbacks(
    PixelBufferTexturePluginCreateRendererCallback create_cb,
    PixelBufferTexturePluginDestroyRendererCallback destroy_cb,
    gpointer priv_data);
FLUTTER_PLUGIN_EXPORT void pixel_buffer_texture_plugin_reset_renderer_callbacks();

G_END_DECLS

#endif // FLUTTER_PLUGIN_PIXEL_BUFFER_TEXTURE_PLUGIN_H_
