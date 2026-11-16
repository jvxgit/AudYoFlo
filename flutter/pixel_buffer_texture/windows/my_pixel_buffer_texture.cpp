#include "include/my_pixel_buffer_texture.h"
#include <iostream>
#include <cstring>

MyPixelBufferTexture::MyPixelBufferTexture(uint32_t width, uint32_t height,
    flutter::TextureRegistrar *texture_registrar)
    :  width(width), height(height), texture_registrar(texture_registrar) {
    const auto size = width * height * 4; // RGBA
    this->buffer1.reset(new uint8_t[size]);
    this->buffer2.reset(new uint8_t[size]);
    this->buffer = this->buffer1.get();

    this->flutter_pixel_buffer1.buffer = this->buffer1.get();
    this->flutter_pixel_buffer1.width = width;
    this->flutter_pixel_buffer1.height = height;

    this->flutter_pixel_buffer2.buffer = this->buffer2.get();
    this->flutter_pixel_buffer2.width = width;
    this->flutter_pixel_buffer2.height = height;

    this->texture = std::make_unique<flutter::TextureVariant>(
        flutter::PixelBufferTexture(
            [=](size_t width, size_t height) -> const FlutterDesktopPixelBuffer *
            {
                
                if (this->published)
                {
                    return this->copy_published();
                }

                const FlutterDesktopPixelBuffer* res;

                // We may return from texture fill without any new data. 
                // Then, we will not swap the buffers to retain the image constant
                if(activateSwap)
                {
                    if (this->buffer == this->buffer1.get()) {                    
                        this->buffer = this->buffer2.get();
                        res = &this->flutter_pixel_buffer1;
                    } else {
                        this->buffer = this->buffer1.get();
                        res = &this->flutter_pixel_buffer2;
                    }
                }
                else
                {
                    if (this->buffer == this->buffer1.get()) {                    
                        res = &this->flutter_pixel_buffer2;
                    } else {
                        res = &this->flutter_pixel_buffer1;
                    }
                }   
                this->notify();
                return res;
            }));
}

MyPixelBufferTexture::~MyPixelBufferTexture() {
    this->notify();
}

void MyPixelBufferTexture::publish(const uint8_t* rgba, uint32_t w, uint32_t h) {
    // Called from the render thread of the backend
    {
        std::lock_guard lock(this->publishMutex);
        size_t sz = (size_t)w * h * 4;
        this->publishBack.resize(sz);
        memcpy(this->publishBack.data(), rgba, sz);
        this->publishBackWidth = w;
        this->publishBackHeight = h;
        this->publishBackValid = true;
        this->published = true;
    }
    this->texture_registrar->MarkTextureFrameAvailable(this->texture_id);
}

const FlutterDesktopPixelBuffer* MyPixelBufferTexture::copy_published() {
    // Called from the raster thread. Flutter copies the returned buffer before the next call,
    // hence, the front buffer is only modified within this function.
    std::lock_guard lock(this->publishMutex);
    if (this->publishBackValid) {
        std::swap(this->publishFront, this->publishBack);
        this->publishFrontWidth = this->publishBackWidth;
        this->publishFrontHeight = this->publishBackHeight;
        this->publishBackValid = false;
    }
    if (this->publishFront.empty()) {
        return nullptr;
    }
    this->flutter_pixel_buffer_published.buffer = this->publishFront.data();
    this->flutter_pixel_buffer_published.width = this->publishFrontWidth;
    this->flutter_pixel_buffer_published.height = this->publishFrontHeight;
    return &this->flutter_pixel_buffer_published;
}

void MyPixelBufferTexture::notify() {
    {
        std::lock_guard lock(this->mutex);
        this->ready = true;
    }
    this->condition.notify_all();
}