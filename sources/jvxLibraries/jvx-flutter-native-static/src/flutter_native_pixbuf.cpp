#include "flutter_native_local.h"
#include "flutter_native_pixbuf.h"

#include <unordered_map>
#include <memory>
#include <thread>

extern "C"
{

	struct threaddata 
	{
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

	void f1(struct threaddata *data) 
	{
	  for (uint32_t i = 0; data->running; i++) 
	  {
		  for (uint32_t j = 0; j < data->width * data->height * 4; j += 4)
		  {
			(*data->buffer)[j] = (uint8_t)(127.f * (1 + sin(0.1f * i)));
			(*data->buffer)[j + 1] = (uint8_t)(127.f * (1.f + sin(0.2f * i)));
			(*data->buffer)[j + 2] = (uint8_t)(127.f * (1.f + sin(0.3f * i)));
			(*data->buffer)[j + 3] = 255;
		  }

		  // This function is synchronized with flutter, hence the maximum unblocked rate is 60 Hz!!
		  data->frame_available_cb(data->texture_id, nullptr);
	  }
	}

	void pixbuffer_local_create_cb(
		int64_t texture_id,
		uint8_t **buffer, uint32_t width, uint32_t height,
		const char *id, const char *arg,
		PixelBufferTexturePluginFrameAvailableCallback frame_available_cb,
		PixelBufferTexturePluginNotifyCallback notify_cb, void* priv_data)
	{
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
	  std::cout << "Calling function " << __FUNCTION__ << std::endl;
	}

	void pixbuffer_local_destroy_cb(int64_t texture_id, void* priv_data)
	{
	  auto tdg = threads[texture_id].get();
	  tdg->running = false;
	  tdg->notify_cb(tdg->texture_id, priv_data);
	  tdg->thread->join();
	  threads.erase(texture_id);
	}
};