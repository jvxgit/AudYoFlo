#include "ayf-sdk-local-headers.h"
#include "include/ayfcorepack/ayfcorepack_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "ayfcorepack_plugin.h"

void AyfcorepackPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  ayfcorepack::AyfcorepackPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
