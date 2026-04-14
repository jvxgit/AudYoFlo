#include "my_application.h"

#include <glib.h>
#include <math.h>
#include <pixel_buffer_texture/pixel_buffer_texture_plugin.h>

struct threaddata {
  gint64 texture_id;
  guint8 **buffer;
  guint32 width;
  guint32 height;
  PixelBufferTexturePluginFrameAvailableCallback frame_available_cb;
  PixelBufferTexturePluginNotifyCallback notify_cb;
  gpointer priv_data;
  gboolean running;
  GThread *thread;
};

gpointer f1(gpointer data) {
  struct threaddata *td = (struct threaddata *)data;
  unsigned i = 0;
  while (td->running) {
    for (int j = 0; j < td->width * td->height * 4; j += 4) {
      (*td->buffer)[j] = (guint8)(127.f * (1.f + sin((float)0.1f * i)));
      (*td->buffer)[j + 1] = (guint8)(127.f * (1.f + sin((float)0.2f * i)));
      (*td->buffer)[j + 2] = (guint8)(127.f * (1.f + sin((float)0.3f * i)));
      (*td->buffer)[j + 3] = 255;
    }
    td->frame_available_cb(td->texture_id, true);
    i += 1;
  }
  return NULL;
}

GHashTable *threads = NULL;

void create_renderer_cb(
    gint64 texture_id, guint8 **buffer, guint32 width, guint32 height,
    const char *id, const char* ctxt, const char *arg,
    PixelBufferTexturePluginFrameAvailableCallback frame_available_cb,
    PixelBufferTexturePluginNotifyCallback notify_cb,
    gpointer priv_data) {
  struct threaddata *tdg = g_new(struct threaddata, 1);
  tdg->texture_id = texture_id;
  tdg->buffer = buffer;
  tdg->width = width;
  tdg->height = height;
  tdg->frame_available_cb = frame_available_cb;
  tdg->notify_cb = notify_cb;
  tdg->priv_data = priv_data;

  tdg->running = TRUE;
  tdg->thread = g_thread_new(NULL, f1, (gpointer)tdg);
  if (!threads) {
    threads =
        g_hash_table_new_full(g_int64_hash, g_int64_equal, g_free, g_free);
  }
  gint64 *key = g_new(gint64, 1);
  *key = texture_id;
  g_hash_table_insert(threads, key, tdg->thread);
}

void destroy_renderer_cb(gint64 texture_id, gpointer priv_data) {
  struct threaddata *tdg =
      (struct threaddata *)g_hash_table_lookup(threads, &texture_id);
  tdg->running = FALSE;
  tdg->notify_cb(texture_id);
  g_thread_join(tdg->thread);
  g_hash_table_remove(threads, &texture_id);
}

int main(int argc, char **argv) {
  pixel_buffer_texture_plugin_set_renderer_callbacks(create_renderer_cb,
                                                     destroy_renderer_cb,
                                                     nullptr);
  g_autoptr(MyApplication) app = my_application_new();
  return g_application_run(G_APPLICATION(app), argc, argv);
}
