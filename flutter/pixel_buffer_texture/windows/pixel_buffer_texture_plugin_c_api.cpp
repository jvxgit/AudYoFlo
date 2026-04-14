#define JVX_COMPONENT_ACCESS_CALLING_CONVENTION 
#include "jvx_sdk.h"
#include "jvx.h"
#include "typedefs/TpjvxPixBuffer.h"


#include "include/pixel_buffer_texture/pixel_buffer_texture_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "pixel_buffer_texture_plugin.h"
#include "pixel_buffer_local.h"

void PixelBufferTexturePluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  pixel_buffer_texture::PixelBufferTexturePlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

void PixelBufferTexturePluginCApiSetRendererCallbacks(
    PixelBufferTexturePluginCreateRendererCallback create_cb,
    PixelBufferTexturePluginDestroyRendererCallback destroy_cb, 
    void* privdata) {
  pixel_buffer_texture::PixelBufferTexturePlugin::SetRendererCallbacks(create_cb, destroy_cb, privdata);
}

void PixelBufferTexturePluginCApiResetRendererCallbacks()
{
  pixel_buffer_texture::PixelBufferTexturePlugin::ResetRendererCallbacks();
}
