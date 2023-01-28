#include "CjvxVideoDevice.h"
#include "CjvxVideoTechnology.h"

#include <ROApi.h>

#include <shlwapi.h> // QITAB and friends
#include <objbase.h> // IID_PPV_ARGS and friends
#include <dshow.h> // IAMVideoProcAmp and friends

CjvxVideoDevice::CjvxVideoDevice()
{
	parent = NULL;
	name = "iamunset";
	JVX_INITIALIZE_MUTEX(safeAccess);

	thisisme = NULL;
	srcPtr = NULL;
	readPtr = NULL;

	mRefCount = 0;
	//std::vector<jvxVideoSetting> lstModes;
	idModeSelect = JVX_SIZE_UNSELECTED;

	runtime.width = 0;
	runtime.height = 0;
	runtime.stride = 0; 
	runtime.form = JVX_VIDEO_FORMAT_UNKNOWN;
	runtime.fldRaw = NULL;
	runtime.szElement = 0;
	runtime.szLine = 0;
	runtime.szRaw = 0;
	runtime.curRate = 0;
	runtime.lastTime = 0;
	runtime.avrgT = 0;
	runtime.status = JVX_STATE_NONE;

	//JVX_MUTEX_HANDLE safeAccess;
	//jvxTimeStampData tStamp;

}
	
CjvxVideoDevice::~CjvxVideoDevice()
{
	JVX_TERMINATE_MUTEX(safeAccess);
}
jvxErrorType 
CjvxVideoDevice::init(CjvxVideoTechnology* par, std::string nm, IMFActivate* inPtr)
{
	parent = par;
	name = nm;
	thisisme = inPtr;

	runtime.status = JVX_STATE_INIT;

	return JVX_NO_ERROR;
}
	
jvxErrorType 
CjvxVideoDevice::terminate()
{
	if (runtime.status == JVX_STATE_COMPLETE)
	{
		return JVX_ERROR_COMPONENT_BUSY;
	}
	
	if (runtime.status == JVX_STATE_PROCESSING)
	{
		this->stop();
	}
	if (runtime.status == JVX_STATE_PREPARED)
	{
		this->postprocess();
	}
	if (runtime.status == JVX_STATE_ACTIVE)
	{
		this->deactivate();
	}

	parent = NULL;
	name = "iamunset"; 
	thisisme = NULL;

	runtime.status = JVX_STATE_NONE;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxVideoDevice::activate()
{
	if (runtime.status == JVX_STATE_INIT)
	{
		IMFAttributes *attributes = NULL;

		// Activate the selected device
		thisisme->ActivateObject(__uuidof(IMFMediaSource), (void**)&srcPtr);

		// Allocate attributes for configuration
		HRESULT hr = MFCreateAttributes(&attributes, 3);
		JVX_ASSERT_X("MFCreateAttributes failed", 0, (hr == S_OK));

		// Expect raw data from devices - no implicit conversion
		hr = attributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, TRUE);
		JVX_ASSERT_X("attributes->SetUINT32 failed", 0, (hr == S_OK));

		// Provide this class as callback object
		hr = attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this);
		JVX_ASSERT_X("MFStartup failed", 0, (hr == S_OK));

		// Transform Media source into a source reader
		hr = MFCreateSourceReaderFromMediaSource(srcPtr, attributes, &readPtr);
		JVX_ASSERT_X("MFCreateSourceReaderFromMediaSource failed", 0, (hr == S_OK));

		idModeSelect = JVX_SIZE_UNSELECTED;
		scanProperties(readPtr);
		if (lstModes.size() > 0)
		{
			idModeSelect = 0;
		}

		runtime.status = JVX_STATE_ACTIVE;
		attributes->Release();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxVideoDevice::deactivate()
{
	if (runtime.status == JVX_STATE_ACTIVE)
	{
		idModeSelect = JVX_SIZE_UNSELECTED;
		lstModes.clear();

		readPtr->Release();
		readPtr = NULL;

		srcPtr->Shutdown();
		srcPtr->Release();
		srcPtr = NULL;
		runtime.status = JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxVideoDevice::prepare(jvxByte** ptrExt, jvxSize num, jvxSize szExt)
{
	jvxSize i;
	if (runtime.status == JVX_STATE_ACTIVE)
	{
		IMFMediaType* type = NULL;
		JVX_ASSERT_X("Check for at least 1 mode", 0, JVX_CHECK_SIZE_SELECTED(idModeSelect));

		HRESULT hr = readPtr->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, (DWORD)lstModes[idModeSelect].id, &type);
		JVX_ASSERT_X("MFCreateSourceReaderFromMediaSource failed", 0, (hr == S_OK));

		hr = readPtr->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, type);
		JVX_ASSERT_X("MFCreateSourceReaderFromMediaSource failed", 0, (hr == S_OK));

		// Get native subid to extract FOURCC code (first DWORD of GUID)
		GUID nativeSubGuid = { 0 };
		hr = type->GetGUID(MF_MT_SUBTYPE, &nativeSubGuid);

		runtime.width = lstModes[idModeSelect].width;
		runtime.height = lstModes[idModeSelect].height;
		runtime.stride = 0;
		runtime.form = lstModes[idModeSelect].form;

		LONG stride = 0;
		hr = MFGetStrideForBitmapInfoHeader(nativeSubGuid.Data1, (DWORD)runtime.width, &stride);
		JVX_ASSERT_X("MFGetStrideForBitmapInfoHeader failed", 0, (hr == S_OK));
		runtime.stride = stride;
		runtime.szElement = jvxVideoSetting_size[runtime.form];
		runtime.szLine = runtime.width * runtime.szElement;
		runtime.szRaw = runtime.height * runtime.szLine;
		runtime.numBufs = num;
		JVX_DSP_SAFE_ALLOCATE_FIELD(runtime.fldBufStat, jvxState, runtime.numBufs);
		if (ptrExt && (runtime.szRaw == szExt))
		{
			runtime.fldRaw = ptrExt;
			runtime.fldExtPtr = true;
		}
		else
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(runtime.fldRaw, jvxByte*, runtime.numBufs);
			for (i = 0; i < runtime.numBufs; i++)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(runtime.fldRaw[i], jvxByte, runtime.szRaw);
			}
			runtime.fldExtPtr = false;
		}
		for (i = 0; i < runtime.numBufs; i++)
		{
			runtime.fldBufStat[i] = JVX_STATE_INIT;
		}
		runtime.bufIdx = 0;
		runtime.bufFHeight = 0;
		runtime.status = JVX_STATE_PREPARED;
		type->Release();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxVideoDevice::postprocess()
{
	if (runtime.status == JVX_STATE_PREPARED)
	{
		if (!runtime.fldExtPtr)
		{
			JVX_DSP_SAFE_DELETE_FIELD(runtime.fldRaw);
		}
		runtime.fldRaw = NULL;
		runtime.fldExtPtr = false;

		runtime.stride = 0;
		runtime.szElement = 0;
		runtime.szLine = 0;
		runtime.szRaw = 0;
		runtime.form = JVX_VIDEO_FORMAT_UNKNOWN;

		runtime.status = JVX_STATE_ACTIVE;
		return JVX_NO_ERROR;

	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxVideoDevice::start()
{
	if (runtime.status == JVX_STATE_PREPARED)
	{
		JVX_GET_TICKCOUNT_US_SETREF(&tStamp);
		runtime.lastTime = -1;
		runtime.avrgT = 0;
		runtime.status = JVX_STATE_PROCESSING;
		runtime.inputOverrun = 0;
		runtime.numFrames = 0;
		HRESULT hr = readPtr->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);
		JVX_ASSERT_X("readPtr->ReadSample failed", 0, (hr == S_OK));

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxVideoDevice::stop()
{
	if (runtime.status == JVX_STATE_PROCESSING)
	{
		runtime.status = JVX_STATE_COMPLETE;

		// Wait for video to stop
		while (runtime.status == JVX_STATE_COMPLETE)
		{
			JVX_SLEEP_MS(100);
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxVideoDevice::access_property(jvxVideoDevicePropertyType tp, jvxBool do_set,
	long* param1, long* param2, long* param3, long* param4, long* param5)
{
	HRESULT hr = S_OK;
	IAMVideoProcAmp *procAmp = NULL;
	IAMCameraControl *control = NULL;

	if (do_set)
	{
		if (!(param1 && param2))
			return JVX_ERROR_INVALID_ARGUMENT;
	}
	else
	{
		if (!(param1 && param2 && param3 && param4 && param5))
			return JVX_ERROR_INVALID_ARGUMENT;
	}

	if (runtime.status >= JVX_STATE_ACTIVE)
	{
		switch (tp)
		{
		case JVX_VIDEO_DATA_PROPS_BRIGHTNESS:
		case JVX_VIDEO_DATA_PROPS_CONTRAST:
		case JVX_VIDEO_DATA_PROPS_HUE:
		case JVX_VIDEO_DATA_PROPS_SATURATION:
		case JVX_VIDEO_DATA_PROPS_SHARPNESS:
		case JVX_VIDEO_DATA_PROPS_GAMMA:
		case JVX_VIDEO_DATA_PROPS_COLORENABLE:
		case JVX_VIDEO_DATA_PROPS_WHITEBALANCE:
		case JVX_VIDEO_DATA_PROPS_BACKLIGHTCOMPENSATION:
		case JVX_VIDEO_DATA_PROPS_GAIN:
			hr = srcPtr->QueryInterface(IID_PPV_ARGS(&procAmp));
			if (SUCCEEDED(hr))
			{
				switch (tp)
				{
				case JVX_VIDEO_DATA_PROPS_BRIGHTNESS:
					if (do_set)
					{

						hr = procAmp->Set(VideoProcAmp_Brightness, *param1, *param2);
					}
					else
					{
						hr = procAmp->GetRange(VideoProcAmp_Brightness, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_DATA_PROPS_CONTRAST:
					if (do_set)
					{

						hr = procAmp->Set(VideoProcAmp_Contrast, *param1, *param2);
					}
					else
					{
						hr = procAmp->GetRange(VideoProcAmp_Contrast, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_DATA_PROPS_HUE:
					if (do_set)
					{

						hr = procAmp->Set(VideoProcAmp_Hue, *param1, *param2);
					}
					else
					{
						hr = procAmp->GetRange(VideoProcAmp_Hue, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_DATA_PROPS_SATURATION:
					if (do_set)
					{

						hr = procAmp->Set(VideoProcAmp_Saturation, *param1, *param2);
					}
					else
					{
						hr = procAmp->GetRange(VideoProcAmp_Saturation, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_DATA_PROPS_SHARPNESS:
					if (do_set)
					{

						hr = procAmp->Set(VideoProcAmp_Sharpness, *param1, *param2);
					}
					else
					{
						hr = procAmp->GetRange(VideoProcAmp_Sharpness, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_DATA_PROPS_GAMMA:
					if (do_set)
					{

						hr = procAmp->Set(VideoProcAmp_Gamma, *param1, *param2);
					}
					else
					{
						hr = procAmp->GetRange(VideoProcAmp_Gamma, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_DATA_PROPS_COLORENABLE:
					if (do_set)
					{

						hr = procAmp->Set(VideoProcAmp_ColorEnable, *param1, *param2);
					}
					else
					{
						hr = procAmp->GetRange(VideoProcAmp_ColorEnable, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_DATA_PROPS_WHITEBALANCE:
					if (do_set)
					{

						hr = procAmp->Set(VideoProcAmp_WhiteBalance, *param1, *param2);
					}
					else
					{
						hr = procAmp->GetRange(VideoProcAmp_WhiteBalance, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_DATA_PROPS_BACKLIGHTCOMPENSATION:
					if (do_set)
					{

						hr = procAmp->Set(VideoProcAmp_BacklightCompensation, *param1, *param2);
					}
					else
					{
						hr = procAmp->GetRange(VideoProcAmp_BacklightCompensation, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_DATA_PROPS_GAIN:
					if (do_set)
					{

						hr = procAmp->Set(VideoProcAmp_Gain, *param1, *param2);
					}
					else
					{
						hr = procAmp->GetRange(VideoProcAmp_Gain, param1, param2, param3, param4, param5);
					}
					break;
				default:
					assert(0);
				}
				procAmp->Release();
			}
			break;
		case JVX_VIDEO_CONTROL_PROPS_PAN:
		case JVX_VIDEO_CONTROL_PROPS_TILT:
		case JVX_VIDEO_CONTROL_PROPS_ROLL:
		case JVX_VIDEO_CONTROL_PROPS_ZOOM:
		case JVX_VIDEO_CONTROL_PROPS_EXPOSURE:
		case JVX_VIDEO_CONTROL_PROPS_IRIS:
		case JVX_VIDEO_CONTROL_PROPS_FOCUS:
			hr = srcPtr->QueryInterface(IID_PPV_ARGS(&control));
			if (SUCCEEDED(hr))
			{
				switch (tp)
				{
				case JVX_VIDEO_CONTROL_PROPS_PAN:
					if (do_set)
					{

						hr = control->Set(CameraControl_Pan, *param1, *param2);
					}
					else
					{
						hr = control->GetRange(CameraControl_Pan, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_CONTROL_PROPS_TILT:
					if (do_set)
					{

						hr = control->Set(CameraControl_Tilt, *param1, *param2);
					}
					else
					{
						hr = control->GetRange(CameraControl_Tilt, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_CONTROL_PROPS_ROLL:
					if (do_set)
					{

						hr = control->Set(CameraControl_Roll, *param1, *param2);
					}
					else
					{
						hr = control->GetRange(CameraControl_Roll, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_CONTROL_PROPS_ZOOM:
					if (do_set)
					{

						hr = control->Set(CameraControl_Zoom, *param1, *param2);
					}
					else
					{
						hr = control->GetRange(CameraControl_Zoom, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_CONTROL_PROPS_EXPOSURE:
					if (do_set)
					{

						hr = control->Set(CameraControl_Exposure, *param1, *param2);
					}
					else
					{
						hr = control->GetRange(CameraControl_Exposure, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_CONTROL_PROPS_IRIS:
					if (do_set)
					{

						hr = control->Set(CameraControl_Iris, *param1, *param2);
					}
					else
					{
						hr = control->GetRange(CameraControl_Iris, param1, param2, param3, param4, param5);
					}
					break;
				case JVX_VIDEO_CONTROL_PROPS_FOCUS:
					if (do_set)
					{

						hr = control->Set(CameraControl_Focus, *param1, *param2);
					}
					else
					{
						hr = control->GetRange(CameraControl_Focus, param1, param2, param3, param4, param5);
					}
					break;

				default:
					assert(0);
				}
				control->Release();
			}
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxVideoDevice::number_modes(jvxSize* sz)
{
	HRESULT hr = S_OK;

	if (runtime.status >= JVX_STATE_ACTIVE)
	{
		if (sz)
			*sz = lstModes.size();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxVideoDevice::settings_modes(jvxSize id, jvxVideoSetting* set)
{
	if (runtime.status >= JVX_STATE_ACTIVE)
	{
		if (id < lstModes.size())
		{
			if (set)
				*set = lstModes[id];
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxVideoDevice::activate_mode(jvxSize id)
{
	if (runtime.status >= JVX_STATE_ACTIVE)
	{
		if (id < lstModes.size())
		{
			idModeSelect = id;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxVideoDevice::next_buffer_ready_lock(jvxSize* idBuf)
{
	*idBuf = JVX_SIZE_UNSELECTED;
	if (runtime.status == JVX_STATE_PROCESSING)
	{
		//JVX_LOCK_MUTEX(safeAccess);
		jvxSize idx = runtime.bufIdx;
		jvxState stat = runtime.fldBufStat[idx];
		//JVX_UNLOCK_MUTEX(safeAccess);

		if (stat == JVX_STATE_ACTIVE)
		{
			*idBuf = idx;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_NOT_READY;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxVideoDevice::buffer_ready_unlock(jvxSize idBuf)
{
	if (runtime.status == JVX_STATE_PROCESSING)
	{
		if (idBuf == runtime.bufIdx)
		{
			jvxState stat = runtime.fldBufStat[idBuf];
			if (stat == JVX_STATE_ACTIVE)
			{
				JVX_LOCK_MUTEX(safeAccess);
				runtime.bufIdx = (runtime.bufIdx + 1) % runtime.numBufs;
				runtime.bufFHeight--;
				runtime.fldBufStat[idBuf] = JVX_STATE_INIT;
				JVX_UNLOCK_MUTEX(safeAccess);
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_NOT_READY;
		}
		return JVX_ERROR_INVALID_ARGUMENT;
	}
	return JVX_ERROR_WRONG_STATE;
}
