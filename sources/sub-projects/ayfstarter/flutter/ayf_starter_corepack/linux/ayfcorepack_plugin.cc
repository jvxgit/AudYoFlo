#include "ayf-sdk-local-headers.h"

#include "include/ayfcorepack/ayfcorepack_plugin.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#include <cstring>

#include <string>
extern std::string fileNameModule;

#define AYFCOREPACK_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), ayfcorepack_plugin_get_type(), \
                              AyfcorepackPlugin))

struct _AyfcorepackPlugin {
  GObject parent_instance;
};

G_DEFINE_TYPE(AyfcorepackPlugin, ayfcorepack_plugin, g_object_get_type())

// Called when a method call is received from Flutter.
static void ayfcorepack_plugin_handle_method_call(
    AyfcorepackPlugin* self,
    FlMethodCall* method_call) {
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar* method = fl_method_call_get_name(method_call);

  if (strcmp(method, "getPlatformVersion") == 0) {
    struct utsname uname_data = {};
    uname(&uname_data);
    g_autofree gchar *version = g_strdup_printf("Linux %s", uname_data.version);
    g_autoptr(FlValue) result = fl_value_new_string(version);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  } 
  else if (strcmp(method, "getEntryPoints") == 0)
  {
    // https://levelup.gitconnected.com/writing-plugins-for-linux-in-flutter-38e46bd7872f
    // https://api.flutter.dev/linux-embedder/fl__value_8h.html

    g_autoptr(FlValue) mapReturn = fl_value_new_map();
		//flutter::EncodableMap retMap;    
		
    std::string fNameModule = fileNameModule;

    fl_value_set(mapReturn, fl_value_new_string("loadedModule"),
                   fl_value_new_string("123"));//ayfcorepack::AyfcorepackPlugin::fileNameModule.c_str()));
    // retMap[flutter::EncodableValue("loadedModule")] = flutter::EncodableValue(ayfcorepack::AyfcorepackPlugin::fileNameModule);		

    fl_value_set(mapReturn, fl_value_new_string("moduleEntryAddress"), fl_value_new_int((intptr_t)flutter_config_open)); 
		//retMap[flutter::EncodableValue("moduleEntryAddress")] = flutter::EncodableValue((intptr_t)flutter_config_open);

    fl_value_set(mapReturn, fl_value_new_string("moduleEntrySymbol"), fl_value_new_string("flutter_config_open")); 
		// retMap[flutter::EncodableValue("moduleEntrySymbol")] = flutter::EncodableValue("flutter_config_open");
		
    //result->Success(retMap);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(mapReturn));
    }
    else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  fl_method_call_respond(method_call, response, nullptr);
}

static void ayfcorepack_plugin_dispose(GObject* object) {
  G_OBJECT_CLASS(ayfcorepack_plugin_parent_class)->dispose(object);
}

static void ayfcorepack_plugin_class_init(AyfcorepackPluginClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = ayfcorepack_plugin_dispose;
}

static void ayfcorepack_plugin_init(AyfcorepackPlugin* self) {}

static void method_call_cb(FlMethodChannel* channel, FlMethodCall* method_call,
                           gpointer user_data) {
  AyfcorepackPlugin* plugin = AYFCOREPACK_PLUGIN(user_data);
  ayfcorepack_plugin_handle_method_call(plugin, method_call);
}

void ayfcorepack_plugin_register_with_registrar(FlPluginRegistrar* registrar) {
  AyfcorepackPlugin* plugin = AYFCOREPACK_PLUGIN(
      g_object_new(ayfcorepack_plugin_get_type(), nullptr));

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "ayfcorepack",
                            FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(channel, method_call_cb,
                                            g_object_ref(plugin),
                                            g_object_unref);

  g_object_unref(plugin);
}
