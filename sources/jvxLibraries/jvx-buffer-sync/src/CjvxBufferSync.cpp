#include "CjvxBufferSync.h"

#define ALPHA_SMOOTH 0.7

CjvxBufferSync::CjvxBufferSync()
{
	jvxDspBaseErrorType resL = jvx_audio_stack_sample_dispenser_cont_initCfg(&myAudioDispenser);
	assert(resL == JVX_DSP_NO_ERROR);
	JVX_INITIALIZE_MUTEX(safeAccess_lock);
}

CjvxBufferSync::~CjvxBufferSync()
{
	JVX_INITIALIZE_MUTEX(safeAccess_lock);
}

// =================================================

jvxErrorType
CjvxBufferSync::startSync(
	jvxSize buffersize, 
	jvxSize srate, 
	jvxDataFormat form, 
	jvxSize numChannelsFromHW, 
	jvxSize numChannelsToHW,
	jvxBufferSync_config* config_arg,
	jvxBufferSync_runtime* runtime_arg,
	jvxBufferSync_fillheight* fheight_args)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	
	runtime = runtime_arg;
	runtime->number_aborts = 0;
	runtime->number_overflows = 0;
	runtime->number_underflows = 0;
	runtime->jitter_buffer_size = 0;

	config = config_arg;

	numberEventsConsidered_perMinMaxSection = JVX_DATA2INT16((jvxData)srate / (jvxData)buffersize / (jvxData)fheight_args->num_MinMaxSections);

	resL = jvx_estimate_buffer_fillheight_init(
		&fHeightEstimator,
		JVX_SIZE_INT32(numberEventsConsidered_perMinMaxSection),
		JVX_SIZE_INT32(fheight_args->num_MinMaxSections),
		fheight_args->recSmoothFactor,
		JVX_SIZE_INT32(fheight_args->numOperations));

	jvx_estimate_buffer_fillheight_restart(fHeightEstimator);

	myAudioDispenser.buffersize = config->number_buffers * buffersize;
	myAudioDispenser.form = form;
	myAudioDispenser.numChannelsFromExternal = numChannelsFromHW;
	myAudioDispenser.numChannelsToExternal = numChannelsToHW;

	myAudioDispenser.callbacks.prv_callbacks = reinterpret_cast<jvxHandle*>(this);
	myAudioDispenser.callbacks.lock_callback = lock_sync_threads_cb;
	myAudioDispenser.callbacks.unlock_callback = unlock_sync_threads_cb;

	myAudioDispenser.start_threshold = config->number_buffers / 2 * buffersize;

	work_buffers_from_hw = NULL;
	if (myAudioDispenser.numChannelsFromExternal)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(work_buffers_from_hw, jvxHandle*,
			myAudioDispenser.numChannelsFromExternal);
		for (i = 0; i < myAudioDispenser.numChannelsFromExternal; i++)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(work_buffers_from_hw[i],
				jvxData, myAudioDispenser.buffersize);
		}
	}

	work_buffers_to_hw = NULL;
	if (myAudioDispenser.numChannelsToExternal)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(work_buffers_to_hw, jvxHandle*,
			myAudioDispenser.numChannelsToExternal);
		for (i = 0; i < myAudioDispenser.numChannelsToExternal; i++)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(work_buffers_to_hw[i],
				jvxData, myAudioDispenser.buffersize);
		}
	}

	resL = jvx_audio_stack_sample_dispenser_cont_init(&myAudioDispenser,
		work_buffers_from_hw, work_buffers_to_hw,
		NULL, NULL);
	assert(resL == JVX_DSP_NO_ERROR);

	jvx_audio_stack_sample_dispenser_cont_prepare(&myAudioDispenser, fHeightEstimator);
	assert(resL == JVX_DSP_NO_ERROR);
	
	
	runtime->jitter_buffer_size = myAudioDispenser.buffersize;

	//inProcessing.crossThreadTransfer.stateFlags = 0;
	// JVX_MUTEX_LOCK(inProcessing.crossThreadTransfer.saf)
	return res;
}

jvxErrorType
CjvxBufferSync::stopSync()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;

	resL = jvx_audio_stack_sample_dispenser_cont_postprocess(&myAudioDispenser);
	assert(resL == JVX_DSP_NO_ERROR);

	// First remove buffers, than terminate since terminate will set default values and overwrite important information
	for (i = 0; i < myAudioDispenser.numChannelsFromExternal; i++)
	{
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(work_buffers_from_hw[i], jvxData);
	}
	JVX_DSP_SAFE_DELETE_FIELD(work_buffers_from_hw);
	work_buffers_from_hw = NULL;
	myAudioDispenser.numChannelsFromExternal = 0;

	for (i = 0; i < myAudioDispenser.numChannelsToExternal; i++)
	{
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(work_buffers_to_hw[i], jvxData);
	}
	JVX_DSP_SAFE_DELETE_FIELD(work_buffers_to_hw);
	work_buffers_to_hw = NULL;
	myAudioDispenser.numChannelsToExternal = 0;

	resL = jvx_audio_stack_sample_dispenser_cont_terminate(&myAudioDispenser);
	assert(resL == JVX_DSP_NO_ERROR);

	resL = jvx_estimate_buffer_fillheight_terminate(fHeightEstimator);
	assert(resL == JVX_DSP_NO_ERROR);

	runtime = nullptr;
	config = nullptr;

	return res;
}

jvxErrorType
CjvxBufferSync::io_hardware(jvxHandle** bufs_from_hw, jvxHandle** bufs_to_hw, jvxSize bsize)
{
#ifdef JVX_NTASK_ENABLE_LOGFILE
	jvxErrorType resL = JVX_NO_ERROR;
#endif
	jvxErrorType res = JVX_ERROR_INTERNAL;
	jvxSize posi = 0;
	jvxDspBaseErrorType resl = JVX_DSP_NO_ERROR;
	jvxData out_average = 0;
	jvxData out_min = 0;
	jvxData out_max = 0;	
	jvxSize failReason = 0;
	jvxAudioStackDispenserProfile* prof = NULL;
	jvxCBool logTiming = config->log_timing;

	if (logTiming)
	{
		prof = &prof_sec;
	}
	resl = jvx_audio_stack_sample_dispenser_cont_external_copy(
		&myAudioDispenser,
		bufs_from_hw, 0, bufs_to_hw, 0, bsize, 0, prof, NULL, NULL, NULL);

	switch (resl)
	{
	case JVX_DSP_NO_ERROR:
		jvx_audio_stack_sample_dispenser_update_fillheight(
			&myAudioDispenser, 0,  &out_average,
			&out_min,&out_max, NULL, NULL, NULL);

		runtime->fillheight_avrg = runtime->fillheight_avrg * ALPHA_SMOOTH + out_average * (1 - ALPHA_SMOOTH);
		runtime->fillheight_min = out_min;
		runtime->fillheight_max = out_max;
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_ABORT:
		failReason = 1;
		runtime->number_aborts++;
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_BUFFER_OVERFLOW:
		failReason = 2;
		runtime->number_overflows++;
		res = JVX_NO_ERROR;
		break;
	default:
		break;
	}

#ifdef JVX_NTASK_ENABLE_LOGFILE
	if (res == JVX_NO_ERROR)
	{
		if (prof)
		{
			jvxHandle* ptr = entries_timing;
			entries_timing[0] = prof->fHeightOnEnter;
			entries_timing[1] = prof->fHeightOnLeave;
			entries_timing[2] = prof->state_onEnter;
			entries_timing[3] = prof->state_onLeave;
			entries_timing[4] = (jvxSize)prof->tStamp;
			entries_timing[5] = JVX_ID_SEC_AUDIO;
			entries_timing[6] = JVX_DATA2SIZE(genSPAsyncio_node::rt_info.output.fillheight_avrg.value);
			entries_timing[7] = failReason;

			resL = logWriter_timing.lfc_write_all_channels(
				(jvxHandle**)&ptr,
				1,
				JVX_NUMBER_ENTRIES_TIMING, JVX_DATAFORMAT_SIZE,
				idTiming);
		}
	}
#endif
	return res;
}

jvxErrorType
CjvxBufferSync::io_software(jvxHandle** bufs_to_sw, jvxHandle** bufs_from_sw, jvxSize bsize)
{
	jvxSize posi = 0;
#ifdef JVX_NTASK_ENABLE_LOGFILE
	jvxErrorType resL = JVX_NO_ERROR;
#endif
	jvxErrorType res = JVX_ERROR_INTERNAL;
	jvxDspBaseErrorType resl = JVX_DSP_NO_ERROR;
	jvxAudioStackDispenserProfile* prof = NULL;
	jvxSize failReason = 0;
	jvxCBool logTiming = config->log_timing;
	jvxSize delay = 0;
	jvxBool setZero = false;

	if (logTiming)
	{
		prof = &prof_prim;
	}
	resl = jvx_audio_stack_sample_dispenser_cont_internal_copy(&myAudioDispenser, 
		bufs_to_sw, 0, bufs_from_sw, 0, bsize, 0, prof, NULL, NULL, NULL);
	switch(resl)
	{
	case JVX_DSP_NO_ERROR:
		
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_ABORT:
		failReason = 1;
		runtime->number_aborts++;
		setZero = true;
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_BUFFER_OVERFLOW:
		failReason = 2;
		setZero = true;
		runtime->number_underflows++;
		res = JVX_NO_ERROR;
		break;
	default:
		break;
	}

	if (setZero)
	{
		jvxSize i;
		jvxSize szElm = jvxDataFormat_getsize(myAudioDispenser.form);
		jvxSize szBuf = szElm * bsize;
		for (i = 0; i < myAudioDispenser.numChannelsFromExternal; i++)
		{
			memset(bufs_to_sw[i], 0, szBuf);
		}
	}

#ifdef JVX_NTASK_ENABLE_LOGFILE
	if (res == JVX_NO_ERROR)
	{
		if (prof)
		{
			jvxHandle* ptr = entries_timing;
			entries_timing[0] = prof->fHeightOnEnter;
			entries_timing[1] = prof->fHeightOnLeave;
			entries_timing[2] = prof->state_onEnter;
			entries_timing[3] = prof->state_onLeave;
			entries_timing[4] = (jvxSize)prof->tStamp;
			entries_timing[5] = JVX_ID_PRIM_AUDIO;
			entries_timing[6] = JVX_DATA2SIZE(genSPAsyncio_node::rt_info.output.fillheight_avrg.value);
			entries_timing[7] = failReason;

			resL = logWriter_timing.lfc_write_all_channels(
				&ptr,
				1,
				JVX_NUMBER_ENTRIES_TIMING, JVX_DATAFORMAT_SIZE,
				idTiming);
		}
	}
#endif
	return res;
}

jvxDspBaseErrorType
CjvxBufferSync::lock_sync_threads_cb(jvxHandle* prv)
{
	if (prv)
	{
		CjvxBufferSync* this_p = (CjvxBufferSync*)prv;
		return this_p->lock_sync_threads_ic();
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
CjvxBufferSync::lock_sync_threads_ic()
{
	JVX_LOCK_MUTEX(safeAccess_lock);
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
CjvxBufferSync::unlock_sync_threads_cb(jvxHandle* prv)
{
	if (prv)
	{
		CjvxBufferSync* this_p = (CjvxBufferSync*)prv;
		return this_p->unlock_sync_threads_ic();
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}
jvxDspBaseErrorType
CjvxBufferSync::unlock_sync_threads_ic()
{
	JVX_UNLOCK_MUTEX(safeAccess_lock);
	return JVX_DSP_NO_ERROR;
}

