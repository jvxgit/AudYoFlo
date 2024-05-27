#ifndef FLUTTER_PLUGIN_AYFCOREPACK_PLUGIN_H_
#define FLUTTER_PLUGIN_AYFCOREPACK_PLUGIN_H_

#include "../common/ayf-sdk-local-headers.h"

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace ayfcorepack {

class AyfcorepackPlugin : public flutter::Plugin {
 public:
static std::string fileNameModule;


  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  AyfcorepackPlugin();

  virtual ~AyfcorepackPlugin();

  // Disallow copy and assign.
  AyfcorepackPlugin(const AyfcorepackPlugin&) = delete;
  AyfcorepackPlugin& operator=(const AyfcorepackPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace ayfcorepack

#endif  // FLUTTER_PLUGIN_AYFCOREPACK_PLUGIN_H_
