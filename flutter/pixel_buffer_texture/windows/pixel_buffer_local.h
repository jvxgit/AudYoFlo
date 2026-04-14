#if defined(__cplusplus)
extern "C" {
#endif

#ifdef FLUTTER_PLUGIN_IMPL
#define FLUTTER_PLUGIN_EXPORT __declspec(dllexport)
#else
#define FLUTTER_PLUGIN_EXPORT __declspec(dllimport)
#endif

FLUTTER_PLUGIN_EXPORT void PixelBufferTexturePluginCApiSetRendererCallbacks(
    PixelBufferTexturePluginCreateRendererCallback create_cb,
    PixelBufferTexturePluginDestroyRendererCallback destroy_cb, void* privdata);

FLUTTER_PLUGIN_EXPORT void PixelBufferTexturePluginCApiResetRendererCallbacks();

#if defined(__cplusplus)
}  // extern "C"
#endif
