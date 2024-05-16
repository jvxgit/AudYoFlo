#ifndef FLUTTER_PLUGIN_AYF_PACK_PLUGIN_H_
#define FLUTTER_PLUGIN_AYF_PACK_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace ayf_pack {

class AyfPackPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  AyfPackPlugin();

  virtual ~AyfPackPlugin();

  // Disallow copy and assign.
  AyfPackPlugin(const AyfPackPlugin&) = delete;
  AyfPackPlugin& operator=(const AyfPackPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace ayf_pack

#endif  // FLUTTER_PLUGIN_AYF_PACK_PLUGIN_H_
