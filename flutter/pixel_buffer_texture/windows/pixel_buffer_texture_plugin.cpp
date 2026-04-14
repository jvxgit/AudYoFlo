#include "pixel_buffer_texture_plugin.h"
#include "include/my_pixel_buffer_texture.h"
#include "typedefs/TpjvxPixBuffer.h"

// This must be included before many other Windows headers.
// #include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <iostream>

#include "pixel_buffer_local.h"

namespace pixel_buffer_texture {

// static
bool PixelBufferTexturePlugin::TextureFrameAvailable(int64_t texture_id, bool reqSwap)
{
	bool res = false;
	if (auto it = PixelBufferTexturePlugin::textures_.find(texture_id); it != PixelBufferTexturePlugin::textures_.end())
	{
		auto my_pixel_buffer_texture = it->second.get();
		my_pixel_buffer_texture->activateSwap = reqSwap;
		res = my_pixel_buffer_texture->texture_registrar->MarkTextureFrameAvailable(my_pixel_buffer_texture->texture_id);
		std::unique_lock lock(my_pixel_buffer_texture->mutex);
		my_pixel_buffer_texture->condition.wait(lock, [=] { return my_pixel_buffer_texture->ready; });
		my_pixel_buffer_texture->ready = false;
		lock.unlock();
	}
	return res;
}

void PixelBufferTexturePlugin::Notify(int64_t texture_id) {
	if (auto it = PixelBufferTexturePlugin::textures_.find(texture_id); it != PixelBufferTexturePlugin::textures_.end())
	{
		auto my_pixel_buffer_texture = it->second.get();
		my_pixel_buffer_texture->notify();
	}
}

void PixelBufferTexturePlugin::RegisterWithRegistrar(
	flutter::PluginRegistrarWindows *registrar) {
  auto channel =
	  std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
		  registrar->messenger(), "pixel_buffer_texture",
		  &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<PixelBufferTexturePlugin>(registrar->texture_registrar());

  channel->SetMethodCallHandler(
	  [plugin_pointer = plugin.get()](const auto &call, auto result) {
		plugin_pointer->HandleMethodCall(call, std::move(result));
	  });

  registrar->AddPlugin(std::move(plugin));
}

void
PixelBufferTexturePlugin::SetRendererCallbacks(PixelBufferTexturePluginCreateRendererCallback create_cb,
                                               PixelBufferTexturePluginDestroyRendererCallback destroy_cb, void* hdl_private)
{
  PixelBufferTexturePlugin::create_texture_renderer_cb = create_cb;
  PixelBufferTexturePlugin::destroy_texture_renderer_cb = destroy_cb;
  PixelBufferTexturePlugin::private_data_renderer_cb = hdl_private;
}

void
PixelBufferTexturePlugin::ResetRendererCallbacks()
{
	for (const auto& pair : PixelBufferTexturePlugin::textures_) {
		const auto texture_id = pair.first;
		if (PixelBufferTexturePlugin::destroy_texture_renderer_cb)
		{
			PixelBufferTexturePlugin::destroy_texture_renderer_cb(texture_id, PixelBufferTexturePlugin::private_data_renderer_cb);
		}
	}
	PixelBufferTexturePlugin::create_texture_renderer_cb = nullptr;
	PixelBufferTexturePlugin::destroy_texture_renderer_cb = nullptr;
	PixelBufferTexturePlugin::private_data_renderer_cb = nullptr;
}

PixelBufferTexturePluginCreateRendererCallback PixelBufferTexturePlugin::create_texture_renderer_cb = nullptr;
PixelBufferTexturePluginDestroyRendererCallback PixelBufferTexturePlugin::destroy_texture_renderer_cb = nullptr;
void* PixelBufferTexturePlugin::private_data_renderer_cb = nullptr;
std::unordered_map<int64_t, std::unique_ptr<MyPixelBufferTexture>> PixelBufferTexturePlugin::textures_;

PixelBufferTexturePlugin::PixelBufferTexturePlugin(flutter::TextureRegistrar* texture_registrar)
  : texture_registrar_(texture_registrar) {}

PixelBufferTexturePlugin::~PixelBufferTexturePlugin() {
  while(this->textures_.size())
  {
	  auto pair = this->textures_.begin();
	  const auto texture_id = pair->first;
	  if (PixelBufferTexturePlugin::destroy_texture_renderer_cb)
	  {
		  PixelBufferTexturePlugin::destroy_texture_renderer_cb(texture_id, PixelBufferTexturePlugin::private_data_renderer_cb);
	  }
	  // TODO maybe use future to delay erase
	  this->texture_registrar_->UnregisterTexture(texture_id);
	  PixelBufferTexturePlugin::textures_.erase(texture_id);
	}
}

void PixelBufferTexturePlugin::HandleMethodCall(
	const flutter::MethodCall<flutter::EncodableValue>& method_call,
	std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
{
	if (method_call.method_name().compare("createTextureRenderer") == 0)
	{
		flutter::EncodableMap arguments = std::get<flutter::EncodableMap>(*method_call.arguments());
		auto width = std::get<int>(arguments[flutter::EncodableValue("width")]);
		auto height = std::get<int>(arguments[flutter::EncodableValue("height")]);
		auto rendererId = std::get<std::string>(arguments[flutter::EncodableValue("rendererId")]);
		auto rendererCtxt = std::get<std::string>(arguments[flutter::EncodableValue("rendererCtxt")]);
		auto rendererArg = std::get<std::string>(arguments[flutter::EncodableValue("rendererArg")]);
		if (width == 0 || height == 0)
		{
			result->Error("VALUE", "MethodCall createTextureRenderer() called without passing width and height parameters!");
		}
		else
		{
			if (create_texture_renderer_cb == NULL ||
				destroy_texture_renderer_cb == NULL)
			{
				result->Error("UNINITIALIZED", "MethodCall createTextureRenderer() called without renderer callbacks being registered!");
			}
			else
			{
				auto my_pixel_buffer_texture_uptr = std::make_unique<MyPixelBufferTexture>(width, height, this->texture_registrar_);
				auto texture_id = this->texture_registrar_->RegisterTexture(my_pixel_buffer_texture_uptr->texture.get());
				my_pixel_buffer_texture_uptr->texture_id = texture_id;
				auto [it, inserted] = PixelBufferTexturePlugin::textures_.emplace(texture_id, std::move(my_pixel_buffer_texture_uptr));
				if (!inserted)
				{
					this->texture_registrar_->UnregisterTexture(texture_id);
					result->Error("INTERNAL", "Could'nt store pixel buffer texture object in lookup table.");
				}
				else
				{
					auto my_pixel_buffer_texture_ptr = it->second.get();
					if (PixelBufferTexturePlugin::create_texture_renderer_cb)
					{
						PixelBufferTexturePluginFrameAvailableCallback cbFrameAvailable = PixelBufferTexturePlugin::TextureFrameAvailable;
						PixelBufferTexturePluginNotifyCallback cbNotify = PixelBufferTexturePlugin::Notify;

						PixelBufferTexturePlugin::create_texture_renderer_cb(texture_id,
							&my_pixel_buffer_texture_ptr->buffer, width, height,
							rendererId.c_str(), rendererCtxt.c_str(), rendererArg.c_str(),
							cbFrameAvailable,
							cbNotify,
							PixelBufferTexturePlugin::private_data_renderer_cb);
					}

					result->Success(flutter::EncodableValue(texture_id));
				}
			}
		}
	}
	else if (method_call.method_name().compare("destroyTextureRenderer") == 0)
	{
		flutter::EncodableMap arguments = std::get<flutter::EncodableMap>(*method_call.arguments());
		auto texture_id = std::get<int64_t>(arguments[flutter::EncodableValue("textureId")]);
		if (texture_id == 0)
		{
			result->Error("UNINITIALIZED", "MethodCall createTextureRenderer() called without renderer callbacks being registered!");
		}
		else
		{
			std::cerr << "destroyTextureRenderer called!" << std::endl;
			if (PixelBufferTexturePlugin::destroy_texture_renderer_cb)
			{
				PixelBufferTexturePlugin::destroy_texture_renderer_cb(texture_id, PixelBufferTexturePlugin::private_data_renderer_cb);
			}
			// TODO maybe use future to delay erase
			this->texture_registrar_->UnregisterTexture(texture_id);
			PixelBufferTexturePlugin::textures_.erase(texture_id);
			result->Success();
		}
	}
	else if (method_call.method_name().compare("getEntryPoints") == 0)
	{
		flutter::EncodableMap retMap;

		pixbuffer_render_set_callback cbs = PixelBufferTexturePluginCApiSetRendererCallbacks;
		// pixbuffer_render_reset_callback cbr = PixelBufferTexturePluginCApiResetRendererCallbacks;

		std::string externalAccessModuleName = JVX_GET_CURRENT_MODULE_PATH((void*)cbs);
		retMap[flutter::EncodableValue("loadedModule")] = flutter::EncodableValue(externalAccessModuleName);
		retMap[flutter::EncodableValue("moduleEntryAddressSet")] = flutter::EncodableValue((intptr_t)PixelBufferTexturePluginCApiSetRendererCallbacks);
		retMap[flutter::EncodableValue("moduleEntryAddressReset")] = flutter::EncodableValue((intptr_t)PixelBufferTexturePluginCApiResetRendererCallbacks);
		retMap[flutter::EncodableValue("moduleEntrySymbol")] = flutter::EncodableValue("PixelBufferTexturePluginCApiSetRendererCallbacks;PixelBufferTexturePluginCApiResetRendererCallbacks");
		result->Success(retMap);
	}
	else
	{
		result->NotImplemented();
	}
}

}  // namespace pixel_buffer_texture
