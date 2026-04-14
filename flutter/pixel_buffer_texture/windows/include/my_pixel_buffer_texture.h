#ifndef FLUTTER_PLUGIN_MY_PIXEL_BUFFER_TEXTURE_H_
#define FLUTTER_PLUGIN_MY_PIXEL_BUFFER_TEXTURE_H_

#include <windows.h>
#include <stdint.h>
#include <memory>
#include <memory>
#include <mutex>
#include <condition_variable>

#include <flutter/plugin_registrar_windows.h>

class MyPixelBufferTexture
{
public:
    MyPixelBufferTexture(uint32_t width, uint32_t height,
        flutter::TextureRegistrar *texture_registrar);

    ~MyPixelBufferTexture();

    void notify();

    std::unique_ptr<uint8_t> buffer1;
    std::unique_ptr<uint8_t> buffer2;
    uint8_t *buffer;
    FlutterDesktopPixelBuffer flutter_pixel_buffer1 = {0};
    FlutterDesktopPixelBuffer flutter_pixel_buffer2 = {0};

    uint32_t width;
    uint32_t height;
    flutter::TextureRegistrar *texture_registrar;
    std::unique_ptr<flutter::TextureVariant> texture;
    int64_t texture_id;

    std::mutex mutex;
    std::condition_variable condition;
    bool ready = false;

    // Flag to indicate if buffer swapping is required or not
    bool activateSwap = false;
};

#endif // FLUTTER_PLUGIN_MY_PIXEL_BUFFER_TEXTURE_H_
