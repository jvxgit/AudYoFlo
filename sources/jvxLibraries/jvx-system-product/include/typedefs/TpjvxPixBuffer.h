#ifndef __TPJVX_PIXBUFFER_H__
#define __TPJVX_PIXBUFFER_H__

/*
 * Connection between a pixel buffer texture plugin (UI side, e.g. flutter/pixel_buffer_texture) and
 * the backend (jvxLibHost, jvxLibHost_pixbuf.cpp).
 *
 * A texture is created on the UI side and passed to the backend via the create callback. The
 * argument <arg> addresses the rendering target in component <id>:
 *
 *   "<install target property>[:<key>=<value>]*"
 *
 * Keys sx, sy, f, fg, om, nb name the properties to read the buffer size, format, subformat,
 * operation mode and number of buffers from. The key "mode" selects the renderer in the backend:
 *
 *   mode=copy (default)  2D full buffer is copied (and scaled) into the texture buffer <*buffer>,
 *                        the buffer is installed when the texture is created (e.g. video).
 *   mode=plot_inc        2D incremental buffer (e.g. spectrogram) rendered with jvxVideoRenderCore
 *   mode=plot_full       2D full buffer rendered with jvxVideoRenderCore
 *                        plot buffers are installed when processing starts and removed when it
 *                        stops (as CjvxMyGlWidget does), the images are passed via the publish
 *                        callback of the extension (or copied into <*buffer> if the size matches).
 */

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

// ==============================================================================================
// Extension for renderers that produce complete images (mode=plot_*). Optional: the entry symbol
// is the third entry in the list of entry symbols ("<set>;<reset>;<set ext>").
// ==============================================================================================

// UI -> backend: runtime configuration of a renderer, called in the UI thread. Keys:
//   "size"   "<width>:<height>:<scale>": size of the texture in physical pixels, scale is the
//            device pixel ratio (used for the overlay text size)
//   "config" renderer specific configuration. Plot renderers: overlay description, lines of
//            "<key>=<value>" separated by '\n', in <value> '\' escapes ("\n": newline, "\\": '\').
//            Keys: active, ndx, ndy, grid (0/1), lx, ly (one per tick label), alx, aly, title,
//            fs, fst (font sizes), legpos (jvxRenderLegendPosition), leg ("<AARRGGBB hex>:<text>",
//            one per entry)
typedef void (*PixelBufferTexturePluginConfigureRendererCallback)(int64_t texture_id,
    const char* key, const char* value, void* priv_data);

// Backend -> UI: hand over a complete RGBA image (top row first) for the texture. The size may
// differ from the size at creation. Does not block, may be called from any thread.
typedef bool (*PixelBufferTexturePluginPublishFrameCallback)(int64_t texture_id,
    const uint8_t* rgba, uint32_t width, uint32_t height);

// Entry function of the extension: the backend registers its configure callback (private data as
// passed to pixbuffer_render_set_callback) and receives the publish function of the UI side.
typedef PixelBufferTexturePluginPublishFrameCallback (*pixbuffer_render_set_callback_ext)(
    PixelBufferTexturePluginConfigureRendererCallback configure_cb);

#endif
