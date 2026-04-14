#include "include/my_pixel_buffer_texture.h"
#include <iostream>

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

void MyPixelBufferTexture::notify() {
    {
        std::lock_guard lock(this->mutex);
        this->ready = true;
    }
    this->condition.notify_all();
}