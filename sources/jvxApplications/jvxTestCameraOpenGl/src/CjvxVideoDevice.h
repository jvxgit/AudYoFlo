#ifndef __CJVXVIDEODEVICE_H__
#define __CJVXVIDEODEVICE_H__

#include "jvx.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#define VERBOSE_1
#define VERBOSE_2
#define ALPHA 0.95


class CjvxVideoTechnology;

typedef enum
{
	JVX_VIDEO_DATA_PROPS_NONE,
	JVX_VIDEO_DATA_PROPS_BRIGHTNESS,
	JVX_VIDEO_DATA_PROPS_CONTRAST,
	JVX_VIDEO_DATA_PROPS_HUE,
	JVX_VIDEO_DATA_PROPS_SATURATION,
	JVX_VIDEO_DATA_PROPS_SHARPNESS,
	JVX_VIDEO_DATA_PROPS_GAMMA,
	JVX_VIDEO_DATA_PROPS_COLORENABLE,
	JVX_VIDEO_DATA_PROPS_WHITEBALANCE,
	JVX_VIDEO_DATA_PROPS_BACKLIGHTCOMPENSATION,
	JVX_VIDEO_DATA_PROPS_GAIN,

	JVX_VIDEO_CONTROL_PROPS_PAN,
	JVX_VIDEO_CONTROL_PROPS_TILT,
	JVX_VIDEO_CONTROL_PROPS_ROLL,
	JVX_VIDEO_CONTROL_PROPS_ZOOM,
	JVX_VIDEO_CONTROL_PROPS_EXPOSURE,
	JVX_VIDEO_CONTROL_PROPS_IRIS,
	JVX_VIDEO_CONTROL_PROPS_FOCUS
} jvxVideoDevicePropertyType;

typedef enum
{
	JVX_VIDEO_FORMAT_UNKNOWN,
	JVX_VIDEO_FORMAT_RGB_24,
	JVX_VIDEO_FORMAT_LIMIT
} jvxVideoFormat;

static const char* jvxVideoSetting_str[JVX_VIDEO_FORMAT_LIMIT] =
{
	"unknown",
	"rgb24"
};

static jvxSize jvxVideoSetting_size[JVX_VIDEO_FORMAT_LIMIT] =
{
	JVX_SIZE_UNSELECTED, (jvxSize)3
};

typedef struct
{
	int width;
	int height;
	jvxSize id;
	jvxVideoFormat form;
	jvxData fps_min;
	jvxData fps_max;
	jvxData fps;
}jvxVideoSetting;

// ==========================================================================

class CjvxVideoDevice: public IMFSourceReaderCallback
{
private:
	CjvxVideoTechnology* parent;
	std::string name;
	IMFActivate* thisisme;
	IMFMediaSource* srcPtr;
	IMFSourceReader* readPtr;
	
	long mRefCount;
	std::vector<jvxVideoSetting> lstModes;
	jvxSize idModeSelect;

	struct
	{
		jvxSize width;
		jvxSize height;
		jvxInt32 stride; // Stride may be negative!!
		jvxVideoFormat form;
		jvxByte** fldRaw;
		jvxState* fldBufStat;
		jvxSize numBufs;
		jvxSize bufIdx;
		jvxSize bufFHeight;
		jvxSize szElement;
		jvxSize szLine;
		jvxSize szRaw;
		jvxData curRate;
		jvxInt64 lastTime;
		jvxData avrgT;
		jvxState status;
		jvxBool fldExtPtr;
		jvxSize inputOverrun;
		jvxSize numFrames;
	} runtime;

	JVX_MUTEX_HANDLE safeAccess;
	jvxTimeStampData tStamp;

public:
	CjvxVideoDevice();
	~CjvxVideoDevice();

	std::string get_name() { return name; };
	jvxErrorType init(CjvxVideoTechnology* par, std::string name, IMFActivate* inPtr);
	jvxErrorType terminate();

	jvxErrorType activate();
	jvxErrorType deactivate();

	jvxErrorType prepare(jvxByte** ptrExt, jvxSize num, jvxSize szExt);
	jvxErrorType postprocess();

	jvxErrorType start();
	jvxErrorType stop();

	jvxErrorType access_property(jvxVideoDevicePropertyType tp, jvxBool do_set,
		long* param1, long* param2, long* param3, long* param4, long* param5);

	jvxErrorType number_modes(jvxSize* sz);
	jvxErrorType settings_modes(jvxSize id, jvxVideoSetting* set);
	jvxErrorType activate_mode(jvxSize id);

	jvxErrorType next_buffer_ready_lock(jvxSize* idBuf);
	jvxErrorType buffer_ready_unlock(jvxSize idBuf);

	// ====================================================================
	STDMETHODIMP QueryInterface(REFIID aRiid, void** aPpv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP OnReadSample(
		HRESULT aStatus,
		DWORD aStreamIndex,
		DWORD aStreamFlags,
		LONGLONG aTimestamp,
		IMFSample *aSample
	);
	STDMETHODIMP OnEvent(DWORD, IMFMediaEvent *);
	STDMETHODIMP OnFlush(DWORD);

	void scanProperties(IMFSourceReader* reader);
};	

#endif