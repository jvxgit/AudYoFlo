#include "include/ayf_pack/ayf_pack_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "ayf_pack_plugin.h"

void AyfPackPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  ayf_pack::AyfPackPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
