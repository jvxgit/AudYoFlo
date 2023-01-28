#ifndef _CJVXBUFFERSYNC_H__
#define _CJVXBUFFERSYNC_H__

#include "jvx.h"

extern "C"
{
#include "jvx_audio_stack_sample_dispenser_cont.h"
#include "jvx_buffer_estimation/jvx_estimate_buffer_fillheight.h"
}

class jvxBufferSync_config
{
public:
	jvxSize number_buffers = 4;
	jvxCBool log_timing = c_false;
	void reset()
	{
		number_buffers = 4;
		log_timing = c_false;
	};
};

class jvxBufferSync_runtime
{
public:
	jvxSize number_aborts = 0;
	jvxSize number_overflows = 0;
	jvxSize number_underflows = 0;
	jvxSize jitter_buffer_size = 0;
	jvxData fillheight_avrg = 0;
	jvxData fillheight_min = 0;
	jvxData fillheight_max = 0;
	void reset()
	{
		number_aborts = 0;
		number_overflows = 0;
		number_underflows = 0;
		jitter_buffer_size = 0;
		fillheight_avrg = 0;
		fillheight_min = 0;
		fillheight_max = 0;
	};
};

class jvxBufferSync_fillheight
{
public:
	jvxSize num_MinMaxSections = 5;
	jvxData recSmoothFactor = 0.95;
	jvxSize numOperations = 0;
	/*
	inProcessing.fHeight.numberEventsConsidered_perMinMaxSection = 10;
	inProcessing.fHeight.num_MinMaxSections = 5;
	inProcessing.fHeight.recSmoothFactor = 0.95;
	inProcessing.fHeight.numOperations = 0;
	*/
	void reset()
	{
		num_MinMaxSections = 5;
		recSmoothFactor = 0.95;
		numOperations = 10;
	};
};

JVX_INTERFACE IjvxBufferSync_convert
{
	virtual ~IjvxBufferSync_convert() {};
	virtual void convert_buffer(jvxHandle* buf_from, jvxHandle* buffer_to, jvxSize num_elms) = 0;
};

class CjvxBufferSync
{
public:

	JVX_MUTEX_HANDLE safeAccess_lock;

	jvxBufferSync_runtime* runtime = nullptr;

	jvxBufferSync_config* config = nullptr;

	// ==============================================
	
	jvxAudioStackDispenserCont myAudioDispenser;
	jvxHandle** work_buffers_from_hw = nullptr;
	jvxHandle** work_buffers_to_hw = nullptr;

	// ==============================================
	
	jvxHandle* fHeightEstimator = nullptr;
	jvxSize numberEventsConsidered_perMinMaxSection = JVX_SIZE_UNSELECTED;	

	// ==============================================

	jvxAudioStackDispenserProfile prof_sec;
	jvxAudioStackDispenserProfile prof_prim;

public:
	CjvxBufferSync();
	~CjvxBufferSync();
	jvxErrorType startSync(
		jvxSize buffersize, 
		jvxSize srate, 
		jvxDataFormat form, 
		jvxSize numChannelsFromHW, 
		jvxSize numChannelsToHW,
		jvxBufferSync_config* rt_config,
		jvxBufferSync_runtime* rt_params,
		jvxBufferSync_fillheight* fh_params);

	jvxErrorType stopSync();

	jvxErrorType io_hardware(jvxHandle** bufs_from_hw, jvxHandle** bufs_to_hw, jvxSize bsize);
	jvxErrorType io_software(jvxHandle** bufs_to_sw, jvxHandle** bufs_from_sw, jvxSize bsize);

	static jvxDspBaseErrorType lock_sync_threads_cb(jvxHandle* prv);
	jvxDspBaseErrorType lock_sync_threads_ic();

	static jvxDspBaseErrorType unlock_sync_threads_cb(jvxHandle* prv);
	jvxDspBaseErrorType unlock_sync_threads_ic();
};

#endif



