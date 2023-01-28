#include "CjvxSPAsyncIo.h"
#define ALPHA_SMOOTH 0.7

jvxErrorType
JVX_ASYNCIO_CLASSNAME::startAudioSync_nolock(jvxSize buffersize, jvxSize srate, jvxDataFormat form, jvxSize numChannelsFromHW, jvxSize numChannelsToHW)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;

	genSPAsyncio_node::rt_info.output.number_aborts.value = 0;
	genSPAsyncio_node::rt_info.output.number_overflows.value = 0;
	genSPAsyncio_node::rt_info.output.number_underflows.value = 0;
	genSPAsyncio_node::general.cross_threads.jitter_buffer_size.value = 0;

	inProcessing.fHeight.numberEventsConsidered_perMinMaxSection = JVX_DATA2INT16(srate / buffersize / inProcessing.fHeight.num_MinMaxSections);

	JVX_LOCK_MUTEX(inProcessing.crossThreadTransfer.safeAccessSecLink);

	resL = jvx_estimate_buffer_fillheight_init(
		&inProcessing.fHeight.fHeightEstimator,
		JVX_SIZE_INT32(inProcessing.fHeight.numberEventsConsidered_perMinMaxSection),
		JVX_SIZE_INT32(inProcessing.fHeight.num_MinMaxSections),
		inProcessing.fHeight.recSmoothFactor,
		JVX_SIZE_INT32(inProcessing.fHeight.numOperations));

	jvx_estimate_buffer_fillheight_restart(inProcessing.fHeight.fHeightEstimator);

	inProcessing.crossThreadTransfer.myAudioDispenser.buffersize = genSPAsyncio_node::general.cross_threads.number_buffers.value * buffersize;
	inProcessing.crossThreadTransfer.myAudioDispenser.form = form;
	inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsFromExternal = numChannelsFromHW;
	inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsToExternal = numChannelsToHW;

	inProcessing.crossThreadTransfer.myAudioDispenser.callbacks.prv_callbacks = reinterpret_cast<jvxHandle*>(this);
	inProcessing.crossThreadTransfer.myAudioDispenser.callbacks.lock_callback = lock_sync_threads_cb;
	inProcessing.crossThreadTransfer.myAudioDispenser.callbacks.unlock_callback = unlock_sync_threads_cb;

	inProcessing.crossThreadTransfer.myAudioDispenser.start_threshold = genSPAsyncio_node::general.cross_threads.number_buffers.value / 2 * buffersize;

	inProcessing.crossThreadTransfer.work_buffers_from_hw = NULL;
	if (inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsFromExternal)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(inProcessing.crossThreadTransfer.work_buffers_from_hw, jvxHandle*,
			inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsFromExternal);
		for (i = 0; i < inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsFromExternal; i++)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(inProcessing.crossThreadTransfer.work_buffers_from_hw[i],
				jvxData, inProcessing.crossThreadTransfer.myAudioDispenser.buffersize);
		}
	}

	inProcessing.crossThreadTransfer.work_buffers_to_hw = NULL;
	if (inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsToExternal)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(inProcessing.crossThreadTransfer.work_buffers_to_hw, jvxHandle*,
			inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsToExternal);
		for (i = 0; i < inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsToExternal; i++)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(inProcessing.crossThreadTransfer.work_buffers_to_hw[i],
				jvxData, inProcessing.crossThreadTransfer.myAudioDispenser.buffersize);
		}
	}

	resL = jvx_audio_stack_sample_dispenser_cont_init(&inProcessing.crossThreadTransfer.myAudioDispenser,
		inProcessing.crossThreadTransfer.work_buffers_from_hw, inProcessing.crossThreadTransfer.work_buffers_to_hw,
		NULL, NULL);
	assert(resL == JVX_DSP_NO_ERROR);

	jvx_audio_stack_sample_dispenser_cont_prepare(&inProcessing.crossThreadTransfer.myAudioDispenser, inProcessing.fHeight.fHeightEstimator);
	assert(resL == JVX_DSP_NO_ERROR);
	inProcessing.crossThreadTransfer.secLinkRead = true;
	JVX_UNLOCK_MUTEX(inProcessing.crossThreadTransfer.safeAccessSecLink);
	
	genSPAsyncio_node::general.cross_threads.jitter_buffer_size.value = inProcessing.crossThreadTransfer.myAudioDispenser.buffersize;

	//inProcessing.crossThreadTransfer.stateFlags = 0;
	// JVX_MUTEX_LOCK(inProcessing.crossThreadTransfer.saf)
	return res;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::stopAudioSync_nolock()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;

	JVX_LOCK_MUTEX(inProcessing.crossThreadTransfer.safeAccessSecLink);
	
	resL = jvx_audio_stack_sample_dispenser_cont_postprocess(&inProcessing.crossThreadTransfer.myAudioDispenser);
	assert(resL == JVX_DSP_NO_ERROR);

	// First remove buffers, than terminate since terminate will set default values and overwrite important information
	for (i = 0; i < inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsFromExternal; i++)
	{
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(inProcessing.crossThreadTransfer.work_buffers_from_hw[i], jvxData);
	}
	JVX_DSP_SAFE_DELETE_FIELD(inProcessing.crossThreadTransfer.work_buffers_from_hw);
	inProcessing.crossThreadTransfer.work_buffers_from_hw = NULL;
	inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsFromExternal = 0;

	for (i = 0; i < inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsToExternal; i++)
	{
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(inProcessing.crossThreadTransfer.work_buffers_to_hw[i], jvxData);
	}
	JVX_DSP_SAFE_DELETE_FIELD(inProcessing.crossThreadTransfer.work_buffers_to_hw);
	inProcessing.crossThreadTransfer.work_buffers_to_hw = NULL;
	inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsToExternal = 0;

	resL = jvx_audio_stack_sample_dispenser_cont_terminate(&inProcessing.crossThreadTransfer.myAudioDispenser);
	assert(resL == JVX_DSP_NO_ERROR);

	resL = jvx_estimate_buffer_fillheight_terminate(inProcessing.fHeight.fHeightEstimator);
	assert(resL == JVX_DSP_NO_ERROR);

	inProcessing.crossThreadTransfer.secLinkRead = false;
	JVX_UNLOCK_MUTEX(inProcessing.crossThreadTransfer.safeAccessSecLink);
	return res;
}

jvxErrorType
JVX_ASYNCIO_CLASSNAME::audio_io_hardware(jvxHandle** bufs_from_hw, jvxHandle** bufs_to_hw, jvxSize bsize)
{
#ifdef JVX_NTASK_ENABLE_LOGFILE
	jvxErrorType resL = JVX_NO_ERROR;
#endif
	jvxErrorType res = JVX_ERROR_INTERNAL;
	jvxSize i;
	jvxSize posi = 0;
	jvxDspBaseErrorType resl = JVX_DSP_NO_ERROR;
	jvxData out_average = 0;
	jvxData out_min = 0;
	jvxData out_max = 0;	
	jvxSize failReason = 0;
	jvxAudioStackDispenserProfile* prof = NULL;
	jvxCBool logTiming = genSPAsyncio_node::general.cross_threads.log_timing.value;

	if (logTiming)
	{
		prof = &inProcessing.prof_sec;
	}
	resl = jvx_audio_stack_sample_dispenser_cont_external_copy(
		&inProcessing.crossThreadTransfer.myAudioDispenser,
		bufs_from_hw, 0, bufs_to_hw, 0, bsize, 0, prof, NULL, NULL, NULL);

	switch (resl)
	{
	case JVX_DSP_NO_ERROR:
		jvx_audio_stack_sample_dispenser_update_fillheight(
			&inProcessing.crossThreadTransfer.myAudioDispenser, 0,  &out_average,
			&out_min,&out_max, NULL, NULL, NULL);

		genSPAsyncio_node::rt_info.output.fillheight_avrg.value = genSPAsyncio_node::rt_info.output.fillheight_avrg.value* ALPHA_SMOOTH + out_average * (1 - ALPHA_SMOOTH);
		genSPAsyncio_node::rt_info.output.fillheight_min.value = out_min;
		genSPAsyncio_node::rt_info.output.fillheight_max.value = out_max;
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_ABORT:
		failReason = 1;
		genSPAsyncio_node::rt_info.output.number_aborts.value++;
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_BUFFER_OVERFLOW:
		failReason = 2;
		genSPAsyncio_node::rt_info.output.number_overflows.value++;
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
JVX_ASYNCIO_CLASSNAME::audio_io_software(jvxHandle** bufs_to_sw, jvxHandle** bufs_from_sw, jvxSize bsize)
{
	jvxSize i;
	jvxSize posi = 0;
#ifdef JVX_NTASK_ENABLE_LOGFILE
	jvxErrorType resL = JVX_NO_ERROR;
#endif
	jvxErrorType res = JVX_ERROR_INTERNAL;
	jvxDspBaseErrorType resl = JVX_DSP_NO_ERROR;
	jvxAudioStackDispenserProfile* prof = NULL;
	jvxSize failReason = 0;
	jvxCBool logTiming = genSPAsyncio_node::general.cross_threads.log_timing.value;
	jvxSize delay = 0;
	if (logTiming)
	{
		prof = &inProcessing.prof_prim;
	}
	resl = jvx_audio_stack_sample_dispenser_cont_internal_copy(&inProcessing.crossThreadTransfer.myAudioDispenser, 
		bufs_to_sw, 0, bufs_from_sw, 0, bsize, 0, prof, NULL, NULL, NULL);
	switch(resl)
	{
	case JVX_DSP_NO_ERROR:
		
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_ABORT:
		failReason = 1;
		genSPAsyncio_node::rt_info.output.number_aborts.value++;
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_BUFFER_OVERFLOW:
		failReason = 2;
		genSPAsyncio_node::rt_info.output.number_underflows.value++;
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
JVX_ASYNCIO_CLASSNAME::lock_sync_threads_cb(jvxHandle* prv)
{
	if (prv)
	{
		JVX_ASYNCIO_CLASSNAME* this_p = (JVX_ASYNCIO_CLASSNAME*)prv;
		return this_p->lock_sync_threads_ic();
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
JVX_ASYNCIO_CLASSNAME::lock_sync_threads_ic()
{
	JVX_LOCK_MUTEX(inProcessing.crossThreadTransfer.safeAccess_lock);
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
JVX_ASYNCIO_CLASSNAME::unlock_sync_threads_cb(jvxHandle* prv)
{
	if (prv)
	{
		JVX_ASYNCIO_CLASSNAME* this_p = (JVX_ASYNCIO_CLASSNAME*)prv;
		return this_p->unlock_sync_threads_ic();
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}
jvxDspBaseErrorType
JVX_ASYNCIO_CLASSNAME::unlock_sync_threads_ic()
{
	JVX_UNLOCK_MUTEX(inProcessing.crossThreadTransfer.safeAccess_lock);
	return JVX_DSP_NO_ERROR;
}

