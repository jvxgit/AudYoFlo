#include "jvxLibHost.h"
#include <thread>
#include <opencv2/imgproc.hpp>

class jvxOneThreadBufferCombo
{
public:
	jvxSize width = 0;
	jvxSize height = 0;
	jvxSize bsize = 0;
	jvxSize numBuffers = 0;
	jvxDataFormat form = JVX_DATAFORMAT_BYTE;
	jvxDataFormatGroup sform = JVX_DATAFORMAT_GROUP_VIDEO_RGBA32;
	jvxExternalBuffer* swb = nullptr;
	jvxBool swbValid = false;
};

class threaddata 
{
public:
	jvxInt64 texture_id = JVX_SIZE_UNSELECTED;
  jvxUInt8 **buffer = nullptr;
  jvxUInt32 width = 0;
  jvxUInt32 height = 0;
  jvxSize szBuf = 0;
  std::string addressArg;
  std::string installHint;
  std::string propInstall;
  jvxComponentIdentification cpId;   
  PixelBufferTexturePluginFrameAvailableCallback frame_available_cb = nullptr;
  PixelBufferTexturePluginNotifyCallback notify_cb = nullptr;
  bool running = false;
  std::unique_ptr<std::thread> thread;

  jvxLockWithVariable<jvxOneThreadBufferCombo>* lockHandle = nullptr;

  static void lockf(jvxHandle* priv)
  {
	  jvxLockWithVariable<jvxOneThreadBufferCombo>* lockHandle = reinterpret_cast<jvxLockWithVariable<jvxOneThreadBufferCombo>*>(priv);
	  assert(lockHandle);
	  lockHandle->lock();
  }

  static jvxErrorType trylockf(jvxHandle* priv)
  {
	  jvxLockWithVariable< jvxOneThreadBufferCombo>* lockHandle = reinterpret_cast<jvxLockWithVariable<jvxOneThreadBufferCombo>*>(priv);
	  assert(lockHandle);
	  if (lockHandle->try_lock())
	  {
		  return JVX_NO_ERROR;
	  }
	  return JVX_ERROR_COMPONENT_BUSY;
  }

  static void unlockf(jvxHandle* priv)
  {
	  jvxLockWithVariable<jvxOneThreadBufferCombo>* lockHandle = reinterpret_cast<jvxLockWithVariable<jvxOneThreadBufferCombo>*>(priv);
	  assert(lockHandle);
	  lockHandle->unlock();
  }
};

std::unordered_map<int64_t, std::unique_ptr<struct threaddata>> threads;

void f1(threaddata *data) 
{
	// This function is in a dedicated thread outside the main thread and the visual data processing thread
	for (uint32_t i = 0; data->running; i++)
	{
		bool favailRep = false;
#ifdef JVX_INIT_EXAMPLE
		for (uint32_t j = 0; j < data->width * data->height * 4; j += 4)
		{
			(*data->buffer)[j] = (uint8_t)(127.f * (1 + sin(0.1f * i)));
			(*data->buffer)[j + 1] = (uint8_t)(127.f * (1.f + sin(0.2f * i)));
			(*data->buffer)[j + 2] = (uint8_t)(127.f * (1.f + sin(0.3f * i)));
			(*data->buffer)[j + 3] = 255;
		}

		// This function is synchronized with flutter, hence the maximum unblocked rate is 60 Hz!!
		data->frame_available_cb(data->texture_id, nullptr);
#else
		if (data->buffer)
		{
			if (data->lockHandle)
			{
				if (data->lockHandle->v.swb) 
				{
					if (data->lockHandle->v.swb->fill_height > 0)
					{
						// Check buffer match size!!
						// assert(data->lockHandle->v.swb->specific.the2DFieldBuffer_full.common.szFldOneBuf == data->szBuf);
						
						//data->lockHandle->v.swb->safe_access.lockf(data->lockHandle->v.swb->safe_access.priv);
						jvxSize idx_read = data->lockHandle->v.swb->idx_read;
						//data->lockHandle->v.swb->safe_access.unlockf(data->lockHandle->v.swb->safe_access.priv);
						jvxByte* ptrRead = data->lockHandle->v.swb->ptrFld;
						ptrRead += idx_read * data->lockHandle->v.swb->numElmFldOneBuf;

						if (data->lockHandle->v.swb->specific.the2DFieldBuffer_full.common.szFldOneBuf == data->szBuf)
						{
							memcpy(*data->buffer, ptrRead, data->lockHandle->v.swb->specific.the2DFieldBuffer_full.common.szFldOneBuf);
						}
						else
						{
							
							// Convert from data->lockHandle->v.swb to data->szBuf
							cv::Mat imageFrom(data->lockHandle->v.swb->specific.the2DFieldBuffer_full.common.seg_y, 
								data->lockHandle->v.swb->specific.the2DFieldBuffer_full.common.seg_x, CV_8UC4, ptrRead);
							cv::Mat resize_down;
							cv::resize(imageFrom, resize_down, cv::Size(data->width, data->height), cv::INTER_LINEAR);
							memcpy(*data->buffer, resize_down.data, data->szBuf);

							// https://learnopencv.com/image-resizing-with-opencv/#resize-by-wdith-height

							
						}
						
						data->lockHandle->v.swb->safe_access.lockf(data->lockHandle->v.swb->safe_access.priv);
						data->lockHandle->v.swb->idx_read = (data->lockHandle->v.swb->idx_read + 1) % data->lockHandle->v.swb->specific.the2DFieldBuffer_full.common.number_buffers;
						data->lockHandle->v.swb->fill_height--;
						data->lockHandle->v.swb->safe_access.unlockf(data->lockHandle->v.swb->safe_access.priv);
						//data->lockHandle->v.swb->safe_access.unlockf(data->lockHandle->v.swb->safe_access.priv);

						favailRep = true;
						data->frame_available_cb(data->texture_id, true);
					}
				}
			}
		}
		if (!favailRep)
		{
			data->frame_available_cb(data->texture_id, false);
		}
#endif
	}
}

void 
jvxLibHost::pixbuffer_local_create_cb(
	int64_t texture_id,
	uint8_t **buffer, uint32_t width, uint32_t height,
	const char *id, const char *arg,
	PixelBufferTexturePluginFrameAvailableCallback frame_available_cb,
	PixelBufferTexturePluginNotifyCallback notify_cb, void* priv_data)
{
	// This function enters the backend within the main thread!!!
	auto tdg = std::make_unique<struct threaddata>();
	tdg->texture_id = texture_id;
	tdg->buffer = buffer;
	tdg->width = width;
	tdg->height = height;
	tdg->frame_available_cb = frame_available_cb;
	tdg->notify_cb = notify_cb;
	tdg->szBuf = tdg->width * tdg->height * jvxDataFormat_getsize(JVX_DATAFORMAT_BYTE) * jvxDataFormatGroup_getsize_mult(JVX_DATAFORMAT_GROUP_VIDEO_RGBA32);
	tdg->szBuf /= jvxDataFormatGroup_getsize_div(JVX_DATAFORMAT_GROUP_VIDEO_RGBA32);

	jvxLibHost* hostRef = reinterpret_cast<jvxLibHost*>(priv_data);

	tdg->addressArg = arg;
	std::vector<std::string> args;
	jvx::helper::parseStringListIntoTokens(tdg->addressArg, args, ':');

	if (args.size() >= 2)
	{
		tdg->installHint = args[0];
		tdg->propInstall = args[1];
		
		std::map<std::string, std::string> mapArgs;
		for (jvxSize i = 2; i < args.size(); i++)
		{
			std::vector<std::string> oneArg;
			jvx::helper::parseStringListIntoTokens(args[i], oneArg, '=');
			if (oneArg.size() == 2)
			{
				mapArgs[oneArg[0]] = oneArg[1];
			}
		}
	
		if (jvxComponentIdentification_decode(tdg->cpId, id) == JVX_NO_ERROR)
		{
			IjvxObject* theObj = nullptr;
			jPAD ident;
			jvxCallManagerProperties callGate;


			hostRef->involvedHost.hHost->request_object_selected_component(tdg->cpId, &theObj);
			if (theObj)
			{
				IjvxProperties* props = reqInterfaceObj<IjvxProperties>(theObj);
				if (props)
				{
					jvxInt32 val = 0;
					JVX_SAFE_ALLOCATE_OBJECT(tdg->lockHandle, jvxLockWithVariable<jvxOneThreadBufferCombo>);
					
					tdg->lockHandle->v.sform = jvxDataFormatGroup::JVX_DATAFORMAT_GROUP_VIDEO_RGBA32; // fg
					auto elm = mapArgs.find("fg");
					if (elm != mapArgs.end())
					{
						// Get the property from module
						ident.reset(elm->second.c_str());						
						jvxErrorType res = props->get_property(callGate, jPRIO<jvxInt32>(val), ident);
						if (res == JVX_NO_ERROR)
						{
							tdg->lockHandle->v.sform = (jvxDataFormatGroup)val;
						}
						else
						{
							std::cout << __FUNCTION__ << ", error: could not read property <" << elm->second << ">." << std::endl;
						}

					}
					
					tdg->lockHandle->v.form = jvxDataFormat::JVX_DATAFORMAT_BYTE; // f
					elm = mapArgs.find("f");
					if (elm != mapArgs.end())
					{
						// Get the property from module
						ident.reset(elm->second.c_str());
						jvxErrorType res = props->get_property(callGate, jPRIO<jvxInt32>(val), ident);
						if (res == JVX_NO_ERROR)
						{
							tdg->lockHandle->v.form = (jvxDataFormat)val;
						}
						else
						{
							std::cout << __FUNCTION__ << ", error: could not read property <" << elm->second << ">." << std::endl;
						}

					}

					tdg->lockHandle->v.numBuffers = 2; // nb
					elm = mapArgs.find("nb");
					if (elm != mapArgs.end())
					{
						// Get the property from module
						ident.reset(elm->second.c_str());
						jvxErrorType res = props->get_property(callGate, jPRIO<jvxInt32>(val), ident);
						if (res == JVX_NO_ERROR)
						{
							tdg->lockHandle->v.numBuffers = val;
						}
						else
						{
							std::cout << __FUNCTION__ << ", error: could not read property <" << elm->second << ">." << std::endl;
						}

					}
					tdg->lockHandle->v.width = tdg->width; // sx
					elm = mapArgs.find("sx");
					if (elm != mapArgs.end())
					{
						// Get the property from module
						ident.reset(elm->second.c_str());
						jvxErrorType res = props->get_property(callGate, jPRIO<jvxInt32>(val), ident);
						if (res == JVX_NO_ERROR)
						{
							tdg->lockHandle->v.width = val;
						}
						else
						{
							std::cout << __FUNCTION__ << ", error: could not read property <" << elm->second << ">." << std::endl;
						}

					}

					tdg->lockHandle->v.height = tdg->height; // sy
					elm = mapArgs.find("sy");
					if (elm != mapArgs.end())
					{
						// Get the property from module
						ident.reset(elm->second.c_str());
						jvxErrorType res = props->get_property(callGate, jPRIO<jvxInt32>(val), ident);
						if (res == JVX_NO_ERROR)
						{
							tdg->lockHandle->v.height = val;
						}
						else
						{
							std::cout << __FUNCTION__ << ", error: could not read property <" << elm->second << ">." << std::endl;
						}
					}

					jvxSize szBuf = 0;

					tdg->lockHandle->v.swb = jvx_allocate2DFieldExternalBuffer_full(
						(tdg->lockHandle->v.width * tdg->lockHandle->v.height), 1, 
						tdg->lockHandle->v.form, tdg->lockHandle->v.sform,
						threaddata::lockf, threaddata::trylockf, threaddata::unlockf, 
						&szBuf, tdg->lockHandle->v.numBuffers, tdg->lockHandle->v.width, 
						tdg->lockHandle->v.height, tdg->lockHandle);

					tdg->lockHandle->v.swb->associationHint = tdg->installHint.c_str();
					tdg->lockHandle->v.swbValid = true;
					// Check settings!

					/*
					tdg->lockHandle->v->specific.the2DFieldBuffer_full.report_triggerf = static_report_bufferswitch_trigger;
					tdg->lockHandle->v->specific.the2DFieldBuffer_full.report_trigger_priv = reinterpret_cast<jvxHandle*>(this);
					*/
					jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(tdg->lockHandle->v.swb, &tdg->lockHandle->v.swbValid,
						tdg->lockHandle->v.swb->formFld);
					jvxCallManagerProperties callGate;
					std::string propStr = tdg->propInstall.c_str();
					jPAD ident(propStr.c_str());
					jvxErrorType res = props->install_property_reference(callGate, ptrRaw, ident);
					JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, propStr);

					retInterfaceObj<IjvxProperties>(theObj, props);
				} // if (props)
				else
				{
					std::cout << "Error: Installing a pixbuf external buffer requires to address a valid <install target property>." << std::endl;
				}
			} // if (theObj)
			else
			{
				std::cout << "Error: Installing a pixbuf external buffer requires to address a selected <install target component>." << std::endl;
			}
		} // if (jvxComponentIdentification_decode(tdg->cpId, id) == JVX_NO_ERROR)
		else
		{
			std::cout << "Error: Installing a pixbuf external buffer requires a valid <install target component>, which <" << id << "> is not!" << std::endl;
		}
	}
	else // if (args.size() == 2)
	{
		std::cout << "Error: Installing a pixbuf external buffer requires an <installation hint> and a <install target property>, both separated by \":\"" << std::endl;
	}
	tdg->running = true;
	tdg->thread = std::make_unique<std::thread>(f1, tdg.get());
	threads.emplace(texture_id, std::move(tdg));
	std::cout << "Calling function " << __FUNCTION__ << std::endl;
}

void 
jvxLibHost::pixbuffer_local_destroy_cb(int64_t texture_id, void* priv_data)
{
	// This function enters the backend within the main thread!!!
	auto tdg = threads[texture_id].get();
	tdg->running = false;
	tdg->notify_cb(tdg->texture_id);
	tdg->thread->join();

	jvxLibHost* hostRef = reinterpret_cast<jvxLibHost*>(priv_data);

	IjvxObject* theObj = nullptr;
	hostRef->involvedHost.hHost->request_object_selected_component(tdg->cpId, &theObj);
	if (theObj)
	{
		IjvxProperties* props = reqInterfaceObj<IjvxProperties>(theObj);
		if (props)
		{
			jvxCallManagerProperties callGate;
			std::string propStr = tdg->propInstall.c_str();
			jPAD ident(propStr.c_str());
			jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(tdg->lockHandle->v.swb, &tdg->lockHandle->v.swbValid,
				tdg->lockHandle->v.swb->formFld);
			props->uninstall_property_reference(callGate, ptrRaw, ident);
			retInterfaceObj<IjvxProperties>(theObj, props);
		}
		if (tdg->lockHandle)
		{
			if (tdg->lockHandle->v.swb)
			{
				jvx_deallocateExternalBuffer(tdg->lockHandle->v.swb, tdg->lockHandle);
				tdg->lockHandle->v.swb = nullptr;
				tdg->lockHandle->v.swbValid = false;
			}
			JVX_SAFE_DELETE_OBJ(tdg->lockHandle);
		}
		tdg->lockHandle = nullptr;
	}

	threads.erase(texture_id);
}
