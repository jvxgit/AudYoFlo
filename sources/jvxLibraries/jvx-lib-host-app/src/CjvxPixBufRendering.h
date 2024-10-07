#ifndef __CJVXPIXBUFRENDERING_H__
#define __CJVXPIXBUFRENDERING_H__

#include "jvxLibHost.h"
#include "CjvxPixBufRendering.h"

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

class CjvxPixBufRendering : public IjvxThreads_report
{
public:
	jvxInt64 texture_id = JVX_SIZE_UNSELECTED;
	jvxUInt8** buffer = nullptr;
	jvxUInt32 width = 0;
	jvxUInt32 height = 0;
	jvxSize szBuf = 0;
	jvxSize frameRate = 0;
	std::string addressArg;
	std::string installHint;
	std::string propInstall;
	jvxComponentIdentification cpId;
	PixelBufferTexturePluginFrameAvailableCallback frame_available_cb = nullptr;
	PixelBufferTexturePluginNotifyCallback notify_cb = nullptr;

	refComp< IjvxThreads> ref;

	jvxLockWithVariable<jvxOneThreadBufferCombo>* lockHandle = nullptr;
	jvxTimeStampData tStamp;
	jvxTick tCountLast = -1;
	jvxData deltaTAverageBuf2Buf = 0;
	jvxData deltaTAverageConvert = 0;
	jvxData deltaTAverageWait = 0;

	static void lockf(jvxHandle* priv);
	static jvxErrorType trylockf(jvxHandle* priv);
	static void unlockf(jvxHandle* priv);

	// ==========================================================================================================================

	jvxErrorType startup(jvxInt64 timestamp_us) override;
	
	jvxErrorType expired(jvxInt64 timestamp_us, jvxSize* delta_ms) override;

	jvxErrorType wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	
	void buffer_proc_core();

	jvxErrorType stopped(jvxInt64 timestamp_us) override;

	// ==========================================================================================================================
	static jvxErrorType  static_report_bufferswitch_trigger(jvxHandle* priv);
	jvxErrorType report_bufferswitch_trigger();
};

#endif
