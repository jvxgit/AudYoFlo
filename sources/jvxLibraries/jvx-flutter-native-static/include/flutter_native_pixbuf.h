#ifndef _JVX_FLUTTER_NATIVE_PIXBUF___
#define _JVX_FLUTTER_NATIVE_PIXBUF___

typedef bool (*PixelBufferTexturePluginFrameAvailableCallback)(int64_t texture_id, void* priv_data);
typedef bool (*PixelBufferTexturePluginNotifyCallback)(int64_t texture_id, void* priv_data);
typedef void (*PixelBufferTexturePluginCreateRendererCallback)(
    int64_t texture_id,
    uint8_t **buffer, uint32_t width, uint32_t height,
    const char *id, const char *arg,
    PixelBufferTexturePluginFrameAvailableCallback frame_available_cb,
    PixelBufferTexturePluginNotifyCallback notify_cb, void* priv_data);
typedef void (*PixelBufferTexturePluginDestroyRendererCallback)(int64_t texture_id, void* priv_data);

// Entry function as provided by the system
typedef void (*pixbuffer_render_callback)(
    PixelBufferTexturePluginCreateRendererCallback create_cb,
    PixelBufferTexturePluginDestroyRendererCallback destroy_cb, void* privdata);

#endif
