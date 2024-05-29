#ifndef FLUTTER_PLUGIN_AYFCOREPACK_PLUGIN_H_
#define FLUTTER_PLUGIN_AYFCOREPACK_PLUGIN_H_

#include <flutter_linux/flutter_linux.h>
#include <string>

G_BEGIN_DECLS

#ifdef FLUTTER_PLUGIN_IMPL
#define FLUTTER_PLUGIN_EXPORT __attribute__((visibility("default")))
#else
#define FLUTTER_PLUGIN_EXPORT
#endif

std::string fileNameModule = "not-set";

typedef struct _AyfcorepackPlugin AyfcorepackPlugin;
typedef struct {
  GObjectClass parent_class;
} AyfcorepackPluginClass;

FLUTTER_PLUGIN_EXPORT GType ayfcorepack_plugin_get_type();

FLUTTER_PLUGIN_EXPORT void ayfcorepack_plugin_register_with_registrar(
    FlPluginRegistrar* registrar);

G_END_DECLS

#endif  // FLUTTER_PLUGIN_AYFCOREPACK_PLUGIN_H_