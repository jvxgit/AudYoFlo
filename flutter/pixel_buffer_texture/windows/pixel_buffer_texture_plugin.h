#ifndef FLUTTER_PLUGIN_PIXEL_BUFFER_TEXTURE_PLUGIN_H_
#define FLUTTER_PLUGIN_PIXEL_BUFFER_TEXTURE_PLUGIN_H_

#define JVX_COMPONENT_ACCESS_CALLING_CONVENTION 
#include "jvx_sdk.h"
#include "jvx.h"
#include "typedefs/TpjvxPixBuffer.h"

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>
#include <unordered_map>

#include "include/my_pixel_buffer_texture.h"
#include "include/pixel_buffer_texture/pixel_buffer_texture_plugin_c_api.h"

namespace pixel_buffer_texture {

class PixelBufferTexturePlugin : public flutter::Plugin {
 public:
  static bool TextureFrameAvailable(int64_t texture_id, bool reqSwap);
  static void Notify(int64_t texture_id);

  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);
  static void SetRendererCallbacks(PixelBufferTexturePluginCreateRendererCallback create_cb,
                                   PixelBufferTexturePluginDestroyRendererCallback destroy_cb, 
                                    void* priv_data);
  static void ResetRendererCallbacks();

  PixelBufferTexturePlugin(flutter::TextureRegistrar* texture_registrar);

  virtual ~PixelBufferTexturePlugin();

  // Disallow copy and assign.
  PixelBufferTexturePlugin(const PixelBufferTexturePlugin&) = delete;
  PixelBufferTexturePlugin& operator=(const PixelBufferTexturePlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

  flutter::TextureRegistrar* texture_registrar_;
  static std::unordered_map<int64_t, std::unique_ptr<MyPixelBufferTexture>> textures_;

  static void* private_data_renderer_cb;
  static PixelBufferTexturePluginCreateRendererCallback create_texture_renderer_cb;
  static PixelBufferTexturePluginDestroyRendererCallback destroy_texture_renderer_cb;
};

}  // namespace pixel_buffer_texture

#endif  // FLUTTER_PLUGIN_PIXEL_BUFFER_TEXTURE_PLUGIN_H_
