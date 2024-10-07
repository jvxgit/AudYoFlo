#include "CjvxPixBufRendering.h"

#include <thread>
#include <opencv2/imgproc.hpp>

#define JVX_ALPHA_SMOOTH 0.9

//std::unordered_map<int64_t, std::unique_ptr<struct threaddata>> threads;
std::unordered_map<int64_t, CjvxPixBufRendering*> threads;

void 
jvxLibHost::pixbuffer_local_create_cb(
	int64_t texture_id,
	uint8_t **buffer, uint32_t width, uint32_t height,
	const char *id, const char *arg,
	PixelBufferTexturePluginFrameAvailableCallback frame_available_cb,
	PixelBufferTexturePluginNotifyCallback notify_cb, void* priv_data)
{
	// This function enters the backend within the main thread!!!
	//auto tdg = std::make_unique<struct threaddata>();
	CjvxPixBufRendering* tdg = nullptr;
	JVX_SAFE_ALLOCATE_OBJECT(tdg, CjvxPixBufRendering);

	tdg->texture_id = texture_id;
	tdg->buffer = buffer;
	tdg->width = width;
	tdg->height = height;
	tdg->frame_available_cb = frame_available_cb;
	tdg->notify_cb = notify_cb;
	tdg->szBuf = tdg->width * tdg->height * jvxDataFormat_getsize(JVX_DATAFORMAT_BYTE) * jvxDataFormatGroup_getsize_mult(JVX_DATAFORMAT_GROUP_VIDEO_RGBA32);
	tdg->szBuf /= jvxDataFormatGroup_getsize_div(JVX_DATAFORMAT_GROUP_VIDEO_RGBA32);

	jvxLibHost* hostRef = reinterpret_cast<jvxLibHost*>(priv_data);
	IjvxToolsHost* tools = reqInterface<IjvxToolsHost>(hostRef->involvedHost.hHost);

	if (tools)
	{
		 tdg->ref = reqInstTool<IjvxThreads>(tools, JVX_COMPONENT_THREADS);

	}

	retInterface<IjvxToolsHost>(hostRef->involvedHost.hHost, tools);

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
						CjvxPixBufRendering::lockf, CjvxPixBufRendering::trylockf, CjvxPixBufRendering::unlockf,
						&szBuf, tdg->lockHandle->v.numBuffers, tdg->lockHandle->v.width, 
						tdg->lockHandle->v.height, tdg->lockHandle);

					tdg->lockHandle->v.swb->associationHint = tdg->installHint.c_str();
					tdg->lockHandle->v.swbValid = true;
					// Check settings!

					
					tdg->lockHandle->v.swb->specific.the2DFieldBuffer_full.report_triggerf = CjvxPixBufRendering::static_report_bufferswitch_trigger;
					tdg->lockHandle->v.swb->specific.the2DFieldBuffer_full.report_trigger_priv = reinterpret_cast<jvxHandle*>(tdg);
					
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

	JVX_GET_TICKCOUNT_US_SETREF(&tdg->tStamp);

	if (tdg->ref.objPtr)
	{
		tdg->ref.cpPtr->initialize(tdg);
		tdg->ref.cpPtr->start(JVX_SIZE_UNSELECTED, true);
	}

	//tdg->thread = std::make_unique<std::thread>(f1, tdg.get());
	//threads.emplace(texture_id, std::move(tdg));
	threads[texture_id] = tdg;
	std::cout << "Calling function " << __FUNCTION__ << std::endl;
}

void 
jvxLibHost::pixbuffer_local_destroy_cb(int64_t texture_id, void* priv_data)
{
	// This function enters the backend within the main thread!!!
	auto it = threads.find(texture_id);
	assert(it!= threads.end());

	auto tdg = it->second;

	tdg->notify_cb(tdg->texture_id);
	
	//tdg->thread->join();
	tdg->ref.cpPtr->stop();
	tdg->ref.cpPtr->terminate();

	jvxLibHost* hostRef = reinterpret_cast<jvxLibHost*>(priv_data);
	IjvxToolsHost* tools = reqInterface<IjvxToolsHost>(hostRef->involvedHost.hHost);

	if (tools)
	{
		retInstTool<IjvxThreads>(tools, JVX_COMPONENT_THREADS, tdg->ref);

	}
	retInterface<IjvxToolsHost>(hostRef->involvedHost.hHost, tools);


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

	threads.erase(it);
	
	JVX_SAFE_DELETE_OBJECT(tdg);
}
