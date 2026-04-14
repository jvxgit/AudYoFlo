#include <iostream>
#include <unordered_map>
#include <memory>
#include <thread>
#include <math.h>

#include <flutter/dart_project.h>
#include <flutter/flutter_view_controller.h>
#include <windows.h>

#include "flutter_window.h"
#include "utils.h"

#include <pixel_buffer_texture/pixel_buffer_texture_plugin_c_api.h>

// We need to declare the callbacks for this example. We can not include the 
// file <jvx_flutter_native_pixbuf.h> here since we do not want to add a dependency to the
// AYF-SDK here.
// #include "jvx_flutter_native_pixbuf.h"

typedef bool (*PixelBufferTexturePluginFrameAvailableCallback)(int64_t texture_id, void* priv_data);
typedef bool (*PixelBufferTexturePluginNotifyCallback)(int64_t texture_id, void* priv_data);
typedef void (*PixelBufferTexturePluginCreateRendererCallback)(
    int64_t texture_id,
    uint8_t** buffer, uint32_t width, uint32_t height,
    const char* id, const char* arg,
    PixelBufferTexturePluginFrameAvailableCallback frame_available_cb,
    PixelBufferTexturePluginNotifyCallback notify_cb, void* priv_data);
typedef void (*PixelBufferTexturePluginDestroyRendererCallback)(int64_t texture_id, void* priv_data);

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef FLUTTER_PLUGIN_IMPL
#define FLUTTER_PLUGIN_EXPORT __declspec(dllexport)
#else
#define FLUTTER_PLUGIN_EXPORT __declspec(dllimport)
#endif

    FLUTTER_PLUGIN_EXPORT void PixelBufferTexturePluginCApiSetRendererCallbacks(
        PixelBufferTexturePluginCreateRendererCallback create_cb,
        PixelBufferTexturePluginDestroyRendererCallback destroy_cb, void* privdata);

#if defined(__cplusplus)
}  // extern "C"
#endif


// ============================================================================================

struct threaddata {
  int64_t texture_id;
  uint8_t **buffer;
  uint32_t width;
  uint32_t height;
  PixelBufferTexturePluginFrameAvailableCallback frame_available_cb;
  PixelBufferTexturePluginNotifyCallback notify_cb;
  bool running;
  std::unique_ptr<std::thread> thread;
};

std::unordered_map<int64_t, std::unique_ptr<struct threaddata>> threads;

void f1(struct threaddata *data) {
  for (uint32_t i = 0; data->running; i++) {
      for (uint32_t j = 0; j < data->width * data->height * 4; j += 4) {
        (*data->buffer)[j] = (uint8_t)(127.f * (1 + sin(0.1f * i)));
        (*data->buffer)[j + 1] = (uint8_t)(127.f * (1.f + sin(0.2f * i)));
        (*data->buffer)[j + 2] = (uint8_t)(127.f * (1.f + sin(0.3f * i)));
        (*data->buffer)[j + 3] = 255;
      }
      data->frame_available_cb(data->texture_id, nullptr);
  }
}

void PixelBufferTextureCreateRenderer(
    int64_t texture_id,
    uint8_t **buffer, uint32_t width, uint32_t height,
    const char *id, const char *arg,
    PixelBufferTexturePluginFrameAvailableCallback frame_available_cb,
    PixelBufferTexturePluginNotifyCallback notify_cb, void* priv_data) {
  auto tdg = std::make_unique<struct threaddata>();
  tdg->texture_id = texture_id;
  tdg->buffer = buffer;
  tdg->width = width;
  tdg->height = height;
  tdg->frame_available_cb = frame_available_cb;
  tdg->notify_cb = notify_cb;

  tdg->running = true;
  tdg->thread = std::make_unique<std::thread>(f1, tdg.get());
  threads.emplace(texture_id, std::move(tdg));
}

void PixelBufferTextureDestroyRenderer(int64_t texture_id, void* priv_data) {
  auto tdg = threads[texture_id].get();
  tdg->running = false;
  tdg->notify_cb(tdg->texture_id, priv_data);
  tdg->thread->join();
  threads.erase(texture_id);
}

int APIENTRY wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev,
                      _In_ wchar_t *command_line, _In_ int show_command) {
  PixelBufferTexturePluginCApiSetRendererCallbacks(PixelBufferTextureCreateRenderer, PixelBufferTextureDestroyRenderer, nullptr);

  // Attach to console when present (e.g., 'flutter run') or create a
  // new console when running with a debugger.
  if (!::AttachConsole(ATTACH_PARENT_PROCESS) && ::IsDebuggerPresent()) {
    CreateAndAttachConsole();
  }

  // Initialize COM, so that it is available for use in the library and/or
  // plugins.
  ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  flutter::DartProject project(L"data");

  std::vector<std::string> command_line_arguments =
      GetCommandLineArguments();

  project.set_dart_entrypoint_arguments(std::move(command_line_arguments));

  FlutterWindow window(project);
  Win32Window::Point origin(10, 10);
  Win32Window::Size size(1280, 720);
  if (!window.Create(L"pixel_buffer_texture_example", origin, size)) {
    return EXIT_FAILURE;
  }
  window.SetQuitOnClose(true);

  ::MSG msg;
  while (::GetMessage(&msg, nullptr, 0, 0)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }

  ::CoUninitialize();
  return EXIT_SUCCESS;
}
