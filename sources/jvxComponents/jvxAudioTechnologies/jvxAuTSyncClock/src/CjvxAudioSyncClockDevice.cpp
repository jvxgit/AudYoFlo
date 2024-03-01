#include "CjvxAudioSyncClockDevice.h"
#include "CjvxAudioSyncClockTechnology.h"
#include "jvx-helpers-cpp.h"


CjvxAudioSyncClockDevice::CjvxAudioSyncClockDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set_properties.moduleReference = _common_set.theModuleName;

	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_16BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_32BIT_LE);

	
}


CjvxAudioSyncClockDevice::~CjvxAudioSyncClockDevice()
{
}

// ============================================================================

void 
CjvxAudioSyncClockDevice::init(CjvxAudioSyncClockTechnology* ptr)
{
	parentTech = ptr;
	numInChannelsMax = ptr->genSyncClock_technology::config_select.init_number_in_channels_max.value;
	numOutChannelsMax = ptr->genSyncClock_technology::config_select.init_number_out_channels_max.value;

}

jvxErrorType 
CjvxAudioSyncClockDevice::activate()
{
	jvxSize i;
	jvxBitField bf;
	jvxErrorType res = CjvxAudioDevice::activate();
	if (res == JVX_NO_ERROR)
	{
		// Should all be zeroed
		jvx_bitFClear(bf);
		for (i = 0; i < 2; i++)
		{
			std::string nmToken = "Input Channel #" + jvx_size2String(i);
			CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back(nmToken.c_str());
			jvx_bitSet(bf, i);
		}
		CjvxAudioDevice::properties_active.inputchannelselection.value.selection() = bf;
		
		jvx_bitFClear(bf);
		for (i = 0; i < 2; i++)
		{
			std::string nmToken = "Output Channel #" + jvx_size2String(i);
			CjvxAudioDevice::properties_active.outputchannelselection.value.entries.push_back(nmToken.c_str());
			jvx_bitSet(bf, i);
		}
		CjvxAudioDevice::properties_active.outputchannelselection.value.selection() = bf;

		CjvxAudioDevice::properties_active.samplerate.value = 48000;
		CjvxAudioDevice::properties_active.buffersize.value = 1024;

		threads = reqInstTool<IjvxThreads>(_common_set.theToolsHost, JVX_COMPONENT_THREADS);
		assert(threads.cpPtr);

		return JVX_NO_ERROR;
	}
	return res;
}

jvxErrorType
CjvxAudioSyncClockDevice::deactivate()
{
	jvxErrorType res = CjvxAudioDevice::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		retInstTool<IjvxThreads>(_common_set.theToolsHost, JVX_COMPONENT_THREADS, threads);
		threads.cpPtr = nullptr;
		threads.objPtr = nullptr;

		CjvxAudioDevice::properties_active.buffersize.value = 1024;
		CjvxAudioDevice::properties_active.samplerate.value = 48000;
		CjvxAudioDevice::properties_active.inputchannelselection.value.entries.clear();
		CjvxAudioDevice::properties_active.inputchannelselection.value.selection() = 0;
		CjvxAudioDevice::properties_active.inputchannelselection.value.exclusive = 0;
		CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
		CjvxAudioDevice::properties_active.outputchannelselection.value.selection() = 0;
		CjvxAudioDevice::properties_active.outputchannelselection.value.exclusive = 0;

		res = CjvxAudioDevice::deactivate();
	}
	return res;
}

// ==========================================================================

jvxErrorType 
CjvxAudioSyncClockDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res =  CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_ERROR_REQUEST_CALL_AGAIN)
	{
		res = CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
CjvxAudioSyncClockDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
    jvxErrorType res = CjvxAudioDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		threads.cpPtr->initialize(this);
	}

#if 0
	// Auto-run the prepare function
	res = prepare_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	// Prepare processing parameters
	_common_set_ocon.theData_out.con_data.number_buffers = 1;
	jvx_bitZSet(_common_set_ocon.theData_out.con_data.alloc_flags, 
		(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);

	jvx_bitSet(_common_set_ocon.theData_out.con_data.alloc_flags,
		(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);

	// New type of connection by propagating through linked elements
    res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{

		// All parameters were set before, only very few need update
		_common_set_ocon.theData_out.con_data.number_buffers = 1;

		// START HERE!!
	}
	else
	{
		_postprocess_chain_master(NULL);		
		goto leave_error;
    }
    
    return(res);
leave_error:
#endif	
    return res;
}

jvxErrorType
CjvxAudioSyncClockDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxAudioDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		threads.cpPtr->terminate();
	}

#if 0
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL;


	// STOP HERE
	
	_postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	// If the chain is postprocessed, the object itself should also be postprocessed if not done so before
	resL = postprocess_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);
#endif
	return res;

}

jvxErrorType
CjvxAudioSyncClockDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxAudioDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		jvxData timeOutBuffer_secs = (jvxData)_common_set_ocon.theData_out.con_params.buffersize / (jvxData)_common_set_ocon.theData_out.con_params.rate;
		timeOutBuffer_msecs_exact = timeOutBuffer_secs * 1e3;
		progress_msecs_accumulated_exact = 0;
		progress_msecs_measured = -1;
		tStampLastFrame = -1;
		jvxSize timeOutBuffer_msecs_floor = floor(timeOutBuffer_msecs_exact);
		JVX_GET_TICKCOUNT_US_SETREF(&tStamp);
		threads.cpPtr->start(timeOutBuffer_msecs_floor, true, true);
	}
#if 0
	jvxErrorType res = JVX_NO_ERROR;

	// If the chain is started, the object itself should also be started if not done so before
	jvxErrorType resL = start_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	res = _start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res != JVX_NO_ERROR)
	{
	    goto leave_error;
	}

	// Start core function

	if (res != JVX_NO_ERROR)
	{
	    _stop_chain_master(NULL);
	    goto leave_error;
	}

	return res;
leave_error:
#endif

	return res;
}

jvxErrorType
CjvxAudioSyncClockDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxAudioDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		threads.cpPtr->stop();
	}
#if 0
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL;

	res = _stop_chain_master(NULL);
	assert(res == JVX_NO_ERROR);

	// If the chain is stopped, the object itself should also be stopped if not done so before
	resL = stop_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);
#endif
	return res;
}

jvxErrorType
CjvxAudioSyncClockDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxAudioDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	
	#if 0
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	_common_set_icon.theData_in->con_params.buffersize = _inproc.buffersize;
	_common_set_icon.theData_in->con_params.format = _inproc.format;
	_common_set_icon.theData_in->con_data.buffers = NULL;
	_common_set_icon.theData_in->con_data.number_buffers = JVX_MAX(_common_set_icon.theData_in->con_data.number_buffers, 1);
	_common_set_icon.theData_in->con_params.number_channels = _inproc.numOutputs;
	_common_set_icon.theData_in->con_params.rate = _inproc.samplerate;

	// Connect the output side and start this link
	res = allocate_pipeline_and_buffers_prepare_to();

	// Do not attach any user hint into backward direction
	_common_set_icon.theData_in->con_compat.user_hints = NULL;
	// _common_set_icon.theData_in->pipeline.idx_stage = 0;
	return res;
#endif
}

jvxErrorType
CjvxAudioSyncClockDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxAudioDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
#if 0
	jvxErrorType res = JVX_NO_ERROR;

	res = deallocate_pipeline_and_buffers_postprocess_to();

	return res;
#endif
}

jvxErrorType
CjvxAudioSyncClockDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	return CjvxAudioDevice::process_buffers_icon(mt_mask, idx_stage);
#if 0
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list
	return res;
#endif
}

jvxErrorType
CjvxAudioSyncClockDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
					jvxSize tobeAccessedByStage,
					callback_process_start_in_lock clbk,
					jvxHandle* priv_ptr)
{
	return CjvxAudioDevice::process_start_icon(
		pipeline_offset, idx_stage,
		tobeAccessedByStage, clbk, priv_ptr);

#if 0
	jvxErrorType res = JVX_NO_ERROR;
	res = shift_buffer_pipeline_idx_on_start( pipeline_offset,  idx_stage,
	    tobeAccessedByStage,
	    clbk, priv_ptr);
	return res;
#endif
}

jvxErrorType
CjvxAudioSyncClockDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
    jvxSize tobeAccessedByStage,
	    callback_process_stop_in_lock clbk,
	    jvxHandle* priv_ptr)
{
	return CjvxAudioDevice::process_stop_icon(
		idx_stage, shift_fwd,
		tobeAccessedByStage, clbk, priv_ptr);
#if 0
	jvxErrorType res = JVX_NO_ERROR;

	// Unlock the buffer for this pipeline
	res = shift_buffer_pipeline_idx_on_stop( idx_stage,  shift_fwd,
	    tobeAccessedByStage, clbk, priv_ptr);

	return res;
#endif

}

jvxErrorType 
CjvxAudioSyncClockDevice::startup(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioSyncClockDevice::expired(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxTick tickEnter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
	if (progress_msecs_measured < 0)
	{
		// First frame
		progress_msecs_measured = 0;
		progress_msecs_accumulated_exact = 0;
	}
	else
	{
		// Start time this frame
		progress_msecs_measured += (tickEnter - tStampLastFrame) * 1e-3;
		progress_msecs_accumulated_exact += timeOutBuffer_msecs_exact;
	}
	tStampLastFrame = tickEnter;

	// =================================================================================================
	res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
	assert(res == JVX_NO_ERROR);

	res = _common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
	assert(res == JVX_NO_ERROR);

	res = _common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
	assert(res == JVX_NO_ERROR);

	// =================================================================================================
	
	jvxData progress_msecs_accumulated_expect_next = progress_msecs_accumulated_exact + timeOutBuffer_msecs_exact;
	jvxTick tickNow = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
	progress_msecs_measured += (tickNow - tickEnter) * 1e-3;

	jvxData deltaMsec = progress_msecs_accumulated_expect_next - progress_msecs_measured;
	if (deltaMsec < 0)
	{
		jvxSize numFramesToCompensate = -deltaMsec / timeOutBuffer_msecs_exact;
		if (numFramesToCompensate > 5)
		{
			// If we are here, we have lost too many buffers!!
			numFramesToCompensate -= 5;
			progress_msecs_measured = -1;
			progress_msecs_accumulated_exact = 0;
			deltaMsec = timeOutBuffer_msecs_exact;
		}
	}

	deltaMsec = JVX_MAX(deltaMsec, 0);

	*delta_ms = floor(deltaMsec);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioSyncClockDevice::wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioSyncClockDevice::stopped(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}
