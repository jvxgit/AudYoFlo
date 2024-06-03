#include "ayfcorepack_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

namespace ayfcorepack {

// static
void AyfcorepackPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "ayfcorepack",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<AyfcorepackPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

AyfcorepackPlugin::AyfcorepackPlugin() {}

AyfcorepackPlugin::~AyfcorepackPlugin() {}

void AyfcorepackPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  } 
    else if (method_call.method_name().compare("getEntryPoints") == 0)
  {
		flutter::EncodableMap retMap;

    std::string externalAccessModuleName = JVX_GET_CURRENT_MODULE_PATH((void*)flutter_config_open);
		retMap[flutter::EncodableValue("loadedModule")] = flutter::EncodableValue(externalAccessModuleName);		
		/*
         * Do not activatze this part since it requires extra libs to be included
        retMap[flutter::EncodableValue("loadedModuleShort")] = flutter::EncodableValue(
			jvx_extractFileFromFilePath(ayfcorepack::AyfcorepackPlugin::fileNameModule));
         */
		retMap[flutter::EncodableValue("moduleEntryAddress")] = flutter::EncodableValue((intptr_t)flutter_config_open);
		retMap[flutter::EncodableValue("moduleEntrySymbol")] = flutter::EncodableValue("flutter_config_open");
		result->Success(retMap);
  }
  else
	{
    result->NotImplemented();
  }
}

}  // namespace ayfcorepack


//------------------------------------------------------------------------
// Can only pre-populate as follows 
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-errors-2/compiler-error-c2864?view=msvc-170
//
// -----------------------------------------------------------------------
