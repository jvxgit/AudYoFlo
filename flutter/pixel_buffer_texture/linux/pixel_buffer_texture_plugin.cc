#include <flutter_linux/flutter_linux.h>

#include <cstring>
#include <dlfcn.h>

#include "include/my_pixel_buffer_texture.h"
#include "include/pixel_buffer_texture/pixel_buffer_texture_plugin.h"

PixelBufferTexturePluginCreateRendererCallback create_texture_renderer_cb =
    NULL;
PixelBufferTexturePluginDestroyRendererCallback destroy_texture_renderer_cb =
    NULL;
gpointer private_data = NULL;

GHashTable *texture_set;

#define PIXEL_BUFFER_TEXTURE_PLUGIN(obj)                                       \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), pixel_buffer_texture_plugin_get_type(),   \
                              PixelBufferTexturePlugin))

// NOTE taken from jvx-system-base/include/typedefs/TjvxTypes.h
#define JVX_MAXSTRING 256

// NOTE taken from jvx-system-base/src/platform/linux/jvx_system_specific.cpp
void JVX_GET_CURRENT_MODULE_PATH(gpointer ptrIdentify, char* buf, gsize buf_size)
{
  /*
  https://man7.org/linux/man-pages/man3/dladdr.3.html
  */
  Dl_info info;
  if (dladdr(ptrIdentify, &info))
  {
    // Loaded from DLL
    strncpy(buf, info.dli_fname, buf_size);
  }
  else
  {
    // I am an executable
    char arg1[20] = { 0 };
    sprintf(arg1, "/proc/%d/exe", getpid());
    memset(buf, 0, buf_size);
    readlink(arg1, buf, buf_size);
  }
}

typedef struct _PixelBufferTexturePlugin PixelBufferTexturePlugin;
struct _PixelBufferTexturePlugin {
  GObject parent_instance;
  FlTextureRegistrar *texture_registrar = NULL;
};

typedef struct {
  GObjectClass parent_class;
} PixelBufferTexturePluginClass;

G_DEFINE_TYPE(PixelBufferTexturePlugin, pixel_buffer_texture_plugin,
              g_object_get_type())

static gboolean pixel_buffer_texture_frame_available(
    MyPixelBufferTexture *my_pixel_buffer_texture, gboolean req_swap) {
  my_pixel_buffer_texture->activate_swap = req_swap;
  gboolean res = fl_texture_registrar_mark_texture_frame_available(
      my_pixel_buffer_texture->texture_registrar,
      FL_TEXTURE(my_pixel_buffer_texture));
  g_async_queue_pop(my_pixel_buffer_texture->queue);
  return res;
}

static void pixel_buffer_texture_notify(
    MyPixelBufferTexture *my_pixel_buffer_texture) {
  g_async_queue_push(my_pixel_buffer_texture->queue, GINT_TO_POINTER(1));
}

// Called when a method call is received from Flutter.
static void
pixel_buffer_texture_plugin_handle_method_call(PixelBufferTexturePlugin *self,
                                               FlMethodCall *method_call) {
  g_autoptr(FlMethodResponse) response = NULL;

  const gchar *method = fl_method_call_get_name(method_call);
  FlValue *args = fl_method_call_get_args(method_call);

  if (strcmp(method, "createTextureRenderer") == 0) {
    int width = 0;
    int height = 0;
    const char *renderer_id = NULL;
    const char *renderer_arg = NULL;
    const char *renderer_ctxt = NULL;
    FlValue *w = fl_value_lookup_string(args, "width");
    FlValue *h = fl_value_lookup_string(args, "height");
    FlValue *r_id = fl_value_lookup_string(args, "rendererId");
    FlValue *r_ctxt = fl_value_lookup_string(args, "rendererCtxt");
    FlValue *r_arg = fl_value_lookup_string(args, "rendererArg");
    if (w != NULL)
      width = fl_value_get_int(w);
    if (h != NULL)
      height = fl_value_get_int(h);
    if (r_id != NULL)
      renderer_id = fl_value_get_string(r_id);
    if (r_ctxt != NULL)
      renderer_ctxt = fl_value_get_string(r_ctxt);
    if (r_arg != NULL)
      renderer_arg = fl_value_get_string(r_arg);
    if (width == 0 || height == 0) {
      response = FL_METHOD_RESPONSE(fl_method_error_response_new(
          "100",
          "MethodCall createTextureRenderer() called without passing width and "
          "height parameters!",
          NULL));
    } else {
      if (create_texture_renderer_cb == NULL ||
          destroy_texture_renderer_cb == NULL) {
        response = FL_METHOD_RESPONSE(fl_method_error_response_new(
            "100",
            "MethodCall createTextureRenderer() called without renderer "
            "callbacks being registered!",
            NULL));
      } else {
        MyPixelBufferTexture *my_pixel_buffer_texture =
            my_pixel_buffer_texture_new(width, height, self->texture_registrar);
        if (!fl_texture_registrar_register_texture(
                self->texture_registrar, FL_TEXTURE(my_pixel_buffer_texture))) {
          response = FL_METHOD_RESPONSE(fl_method_error_response_new(
              "100",
              "MethodCall createTextureRenderer() failed to register with "
              "texture registrar!",
              NULL));
        }
        // NOTE on newer flutter: fl_texture_get_id(FL_TEXTURE(self->texture))
        gint64 texture_id = reinterpret_cast<int64_t>(my_pixel_buffer_texture);
	g_hash_table_add(texture_set, &texture_id);

        create_texture_renderer_cb(
            texture_id, &my_pixel_buffer_texture->buffer, width, height,
            renderer_id, renderer_ctxt, renderer_arg,
            (PixelBufferTexturePluginFrameAvailableCallback)
                pixel_buffer_texture_frame_available,
            (PixelBufferTexturePluginNotifyCallback)
                pixel_buffer_texture_notify,
	    private_data);

        g_autoptr(FlValue) result = fl_value_new_int(texture_id);
        response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
      }
    }
  } else if (strcmp(method, "destroyTextureRenderer") == 0) {
    gint64 texture_id = 0;
    FlValue *t = fl_value_lookup_string(args, "textureId");
    if (t != NULL)
      texture_id = fl_value_get_int(t);
    if (texture_id == 0) {
      response = FL_METHOD_RESPONSE(fl_method_error_response_new(
          "100",
          "MethodCall destroyTextureRenderer() called without passing a valid "
          "texture id parameter!",
          NULL));
    } else {
      MyPixelBufferTexture *my_pixel_buffer_texture =
          reinterpret_cast<MyPixelBufferTexture *>(texture_id);
      destroy_texture_renderer_cb((gint64)texture_id, private_data);
      fl_texture_registrar_unregister_texture(
          self->texture_registrar, FL_TEXTURE(my_pixel_buffer_texture));
      g_hash_table_remove(texture_set, &texture_id);
      g_clear_object(&my_pixel_buffer_texture);
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(NULL));
    }
  } else if (strcmp(method, "getEntryPoints") == 0) {
    char external_access_module_name[JVX_MAXSTRING + 1];
    JVX_GET_CURRENT_MODULE_PATH((void*)pixel_buffer_texture_plugin_set_renderer_callbacks, external_access_module_name, JVX_MAXSTRING);

    g_autoptr(FlValue) ret_map = fl_value_new_map();
    fl_value_set_string_take(ret_map, "loadedModule", fl_value_new_string(external_access_module_name));
    fl_value_set_string_take(ret_map, "moduleEntryAddressSet", fl_value_new_int((intptr_t)pixel_buffer_texture_plugin_set_renderer_callbacks));
    fl_value_set_string_take(ret_map, "moduleEntryAddressReset", fl_value_new_int((intptr_t)pixel_buffer_texture_plugin_reset_renderer_callbacks));
    fl_value_set_string_take(ret_map, "moduleEntrySymbol", fl_value_new_string("pixel_buffer_texture_plugin_set_renderer_callbacks;pixel_buffer_texture_plugin_reset_renderer_callbacks"));
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(ret_map));
  } else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  fl_method_call_respond(method_call, response, NULL);
}

static void pixel_buffer_texture_plugin_dispose(GObject *object) {
  G_OBJECT_CLASS(pixel_buffer_texture_plugin_parent_class)->dispose(object);
  g_hash_table_destroy(texture_set);
}

static void
pixel_buffer_texture_plugin_class_init(PixelBufferTexturePluginClass *klass) {
  texture_set = g_hash_table_new(g_int64_hash, g_int64_equal);
  G_OBJECT_CLASS(klass)->dispose = pixel_buffer_texture_plugin_dispose;
}

static void pixel_buffer_texture_plugin_init(PixelBufferTexturePlugin *self) {}

static void method_call_cb(FlMethodChannel *channel, FlMethodCall *method_call,
                           gpointer user_data) {
  PixelBufferTexturePlugin *plugin = PIXEL_BUFFER_TEXTURE_PLUGIN(user_data);
  pixel_buffer_texture_plugin_handle_method_call(plugin, method_call);
}

void pixel_buffer_texture_plugin_register_with_registrar(
    FlPluginRegistrar *registrar) {
  PixelBufferTexturePlugin *plugin = PIXEL_BUFFER_TEXTURE_PLUGIN(
      g_object_new(pixel_buffer_texture_plugin_get_type(), NULL));

  plugin->texture_registrar =
      fl_plugin_registrar_get_texture_registrar(registrar);

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "pixel_buffer_texture", FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(
      channel, method_call_cb, g_object_ref(plugin), g_object_unref);

  g_object_unref(plugin);
}

void pixel_buffer_texture_plugin_set_renderer_callbacks(
    PixelBufferTexturePluginCreateRendererCallback create_cb,
    PixelBufferTexturePluginDestroyRendererCallback destroy_cb,
    gpointer priv_data) {
  create_texture_renderer_cb = create_cb;
  destroy_texture_renderer_cb = destroy_cb;
  private_data = priv_data;
}

void destroy_texture(gpointer key, gpointer value, gpointer user_data) {
  destroy_texture_renderer_cb(*((gint64*)key), private_data);
}

void pixel_buffer_texture_plugin_reset_renderer_callbacks() {
  create_texture_renderer_cb = NULL;
  destroy_texture_renderer_cb = NULL;
  private_data = NULL;
  g_hash_table_foreach(texture_set, destroy_texture, NULL);
  g_hash_table_destroy(texture_set);
  texture_set = g_hash_table_new(g_int64_hash, g_int64_equal);
}
