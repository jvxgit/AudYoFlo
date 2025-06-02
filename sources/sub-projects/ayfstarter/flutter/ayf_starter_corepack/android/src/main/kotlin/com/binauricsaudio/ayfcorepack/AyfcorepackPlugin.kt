package com.binauricsaudio.ayfcorepack

import androidx.annotation.NonNull

import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result

/** AyfcorepackPlugin */
class AyfcorepackPlugin: FlutterPlugin, MethodCallHandler {
  /// The MethodChannel that will the communication between Flutter and native Android
  ///
  /// This local reference serves to register the plugin with the Flutter Engine and unregister it
  /// when the Flutter Engine is detached from the Activity
  private lateinit var channel : MethodChannel

  override fun onAttachedToEngine(@NonNull flutterPluginBinding: FlutterPlugin.FlutterPluginBinding) {
    channel = MethodChannel(flutterPluginBinding.binaryMessenger, "ayfcorepack")
    channel.setMethodCallHandler(this)
  }

  override fun onMethodCall(@NonNull call: MethodCall, @NonNull result: Result) {
    if (call.method == "getPlatformVersion") {
      result.success("Android ${android.os.Build.VERSION.RELEASE}")
    } 
    else if (call.method == "getEntryPoints") {
      val entryPoints: Map<String, Any> = getEntryPoints()
      result.success(entryPoints)      
    }
    else {
      result.notImplemented()
    }
  }

  override fun onDetachedFromEngine(@NonNull binding: FlutterPlugin.FlutterPluginBinding) {
    channel.setMethodCallHandler(null)
  }

  private fun getEntryPoints(): Map<String, Any> {
        val map = HashMap<String, Any>()
        map["loadedModule"] = "libayfstarter-native-config_import.so"
        map["moduleEntryAddress"] = -1
        map["moduleEntrySymbol"] = "flutter_config_open"
        map["requiredOperation"] = "jni_load_dll"
        // map[anotherkey] = listOf("item1", "item2")
        // map["key4"] = mapOf("nestedKey" to "nestedValue")
        return map
  }
}
