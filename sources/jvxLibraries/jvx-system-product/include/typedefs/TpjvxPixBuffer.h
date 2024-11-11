#ifndef __TPJVX_PIXBUFFER_H__
#define __TPJVX_PIXBUFFER_H__

typedef bool (*PixelBufferTexturePluginFrameAvailableCallback)(int64_t texture_id, bool reqSwap);
typedef void (*PixelBufferTexturePluginNotifyCallback)(int64_t texture_id);
typedef void (*PixelBufferTexturePluginCreateRendererCallback)(
    int64_t texture_id,
    uint8_t **buffer, uint32_t width, uint32_t height,
    const char *id, const char* ctxt, const char *arg,
    PixelBufferTexturePluginFrameAvailableCallback frame_available_cb,
    PixelBufferTexturePluginNotifyCallback notify_cb, void* priv_data);
typedef void (*PixelBufferTexturePluginDestroyRendererCallback)(int64_t texture_id, void* priv_data);

// Entry function as provided by the system
typedef void (*pixbuffer_render_set_callback)(
    PixelBufferTexturePluginCreateRendererCallback create_cb,
    PixelBufferTexturePluginDestroyRendererCallback destroy_cb, void* privdata);

typedef void (*pixbuffer_render_reset_callback)();

#endif