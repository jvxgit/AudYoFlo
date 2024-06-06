#include "CjvxSpNSynchronize_sec.h"
#include "CjvxSpNSynchronize.h"

#define ALPHA_SMOOTH 0.95

jvxErrorType
CjvxSpNSynchronize_sec::lock_sync_threads_cb(jvxHandle* prv)
{
	if (prv)
	{
		CjvxSpNSynchronize_sec* this_p = (CjvxSpNSynchronize_sec*)prv;
		return this_p->lock_sync_threads_ic();
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
CjvxSpNSynchronize_sec::lock_sync_threads_ic()
{
	safeAccessConnectionBuffering.lock();
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
CjvxSpNSynchronize_sec::unlock_sync_threads_cb(jvxHandle* prv)
{
	if (prv)
	{
		CjvxSpNSynchronize_sec* this_p = (CjvxSpNSynchronize_sec*)prv;
		return this_p->unlock_sync_threads_ic();
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}
jvxDspBaseErrorType
CjvxSpNSynchronize_sec::unlock_sync_threads_ic()
{
	safeAccessConnectionBuffering.unlock();
	return JVX_DSP_NO_ERROR;
}

// ====================================================================================

jvxErrorType
CjvxSpNSynchronize_sec::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvx_presetMasterOnPrepare(_common_set_ocon.theData_out);
	return CjvxConnectionMaster::_prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxSpNSynchronize_sec::prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxSingleOutputConnector::prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
	}
	return res;
}

jvxErrorType
CjvxSpNSynchronize_sec::postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxSingleOutputConnector::postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{

	}
	return res;

}

jvxErrorType
CjvxSpNSynchronize_sec::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	auto res = CjvxSingleInputConnector::process_buffers_icon(mt_mask, idx_stage);
	return res;
}

jvxErrorType
CjvxSpNSynchronize_sec::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	auto res = CjvxSingleOutputConnector::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		switch (tp)
		{
		case JVX_LINKDATA_TRANSFER_REQUEST_DATA:

			// Make sure the access to the secondary link is fully setup before actually using it
			safeAccessRWConnectionStatus.lock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_SHARED); // or better: try_lock?
			if (safeAccessRWConnectionStatus.v == 0xF)
			{
				if (_common_set_ocon.theData_out.con_link.connect_to)
				{
					_common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
					jvxSize numLostNow = referencePtr->genSynchronize::rt_info.output.number_aborts.value +
						referencePtr->genSynchronize::rt_info.output.number_overflows.value +
						referencePtr->genSynchronize::rt_info.output.number_underflows.value;
					if (numLostNow != inProcessing.cntLost)
					{
						jvxLinkDataAttachedLostFrames* newPtr = NULL;
						if (jvxLinkDataAttachedLostFrames_updatePrepare(
							inProcessing.attachedData,
							numLostNow,
							newPtr))
						{
							inProcessing.cntLost = numLostNow;

							// Note: theData_in->con_data.attached_buffer_single might be nullptr!!
							if (_common_set_ocon.theData_out.con_data.attached_buffer_single)
							{
								_common_set_ocon.theData_out.con_data.attached_buffer_single[*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr] = newPtr;
							}
						}
					}

					_common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();

					jvxHandle** buf_to_secondary = NULL;
					jvxHandle** buf_from_secondary = NULL;
					if (_common_set_ocon.theData_out.con_data.buffers)
					{
						buf_to_secondary = _common_set_ocon.theData_out.con_data.buffers[*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr];
					}
					if (_common_set_icon.theData_in->con_data.buffers)
					{
						buf_from_secondary = _common_set_icon.theData_in->con_data.buffers[
							*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr];
					}

					res = audio_secondary(_common_set_icon.theData_in, 
						*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr,
						&_common_set_ocon.theData_out);
						
					_common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
				}
			}
			safeAccessRWConnectionStatus.unlock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_SHARED);
			break;
		default:
			// What to do here?
			break;
		}		
	}
	return res;
}

void 
CjvxSpNSynchronize_sec::try_allocate_runtime_sync_data(jvxSize posId)
{
	jvxSize i;
	jvxBool finalizeSetup = false;
	safeAccessRWConnectionStatus.lock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE);
	jvx_bitSet(safeAccessRWConnectionStatus.v, posId);
	if (safeAccessRWConnectionStatus.v == 0x7)
	{
		finalizeSetup = true;
	}
	safeAccessRWConnectionStatus.unlock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE);

	if (finalizeSetup)
	{
		safeAccessRWConnectionStatus.lock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE);

		referencePtr->genSynchronize::rt_info.output.number_aborts.value = 0;
		referencePtr->genSynchronize::rt_info.output.number_overflows.value = 0;
		referencePtr->genSynchronize::rt_info.output.number_underflows.value = 0;

		referencePtr->genSynchronize::info.output.jitter_buffer_size.value = 0;

		inProcessing.fHeight.numberEventsConsidered_perMinMaxSection = JVX_DATA2INT16(referencePtr->node_inout._common_set_node_params_a_1io.samplerate / 
			referencePtr->node_inout._common_set_node_params_a_1io.buffersize / 
			inProcessing.fHeight.num_MinMaxSections);

		// Allocate the requied buffers
		auto resL = jvx_estimate_buffer_fillheight_init(
			&inProcessing.fHeight.fHeightEstimator,
			JVX_SIZE_INT32(inProcessing.fHeight.numberEventsConsidered_perMinMaxSection),
			JVX_SIZE_INT32(inProcessing.fHeight.num_MinMaxSections),
			inProcessing.fHeight.recSmoothFactor,
			JVX_SIZE_INT32(inProcessing.fHeight.numOperations));
		jvx_estimate_buffer_fillheight_restart(inProcessing.fHeight.fHeightEstimator);

		inProcessing.crossThreadTransfer.myAudioDispenser.buffersize = referencePtr->genSynchronize::synchronization.config.number_buffers.value * referencePtr->node_inout._common_set_node_params_a_1io.buffersize;
		inProcessing.crossThreadTransfer.myAudioDispenser.form = (jvxDataFormat)referencePtr->node_inout._common_set_node_params_a_1io.format;
		inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsFromExternal = referencePtr->node_inout._common_set_node_params_a_1io.number_channels;
		inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsToExternal = referencePtr->node_output._common_set_node_params_a_1io.number_channels;

		inProcessing.crossThreadTransfer.myAudioDispenser.callbacks.prv_callbacks = reinterpret_cast<jvxHandle*>(this);
		inProcessing.crossThreadTransfer.myAudioDispenser.callbacks.lock_callback = lock_sync_threads_cb;
		inProcessing.crossThreadTransfer.myAudioDispenser.callbacks.unlock_callback = unlock_sync_threads_cb;

		inProcessing.crossThreadTransfer.myAudioDispenser.start_threshold = referencePtr->genSynchronize::synchronization.config.number_buffers.value / 2 * referencePtr->node_inout._common_set_node_params_a_1io.buffersize;

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
		
		referencePtr->genSynchronize::info.output.jitter_buffer_size.value = inProcessing.crossThreadTransfer.myAudioDispenser.buffersize;

		// Finalize setup
		jvx_bitSet(safeAccessRWConnectionStatus.v, 3);
		safeAccessRWConnectionStatus.unlock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE);
	}
}

void 
CjvxSpNSynchronize_sec::try_deallocate_runtime_sync_data(jvxSize posId)
{
	jvxSize i;
	jvxBool deconstructSetup = false;
	safeAccessRWConnectionStatus.lock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE);
	if (safeAccessRWConnectionStatus.v == 0xF)
	{
		// This will stop the input output processing immediately
		deconstructSetup = true;
		jvx_bitClear(safeAccessRWConnectionStatus.v, 3);
	}
	safeAccessRWConnectionStatus.unlock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE);

	if (deconstructSetup)
	{
		safeAccessRWConnectionStatus.lock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE);

		// Deallocate the requied buffers
		auto resL = jvx_audio_stack_sample_dispenser_cont_postprocess(&inProcessing.crossThreadTransfer.myAudioDispenser);
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

		// Reset the target bit flag
		jvx_bitClear(safeAccessRWConnectionStatus.v, posId);
		safeAccessRWConnectionStatus.unlock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE);
	}
}

jvxErrorType
CjvxSpNSynchronize_sec::audio_primary(jvxLinkDataDescriptor* datIn, jvxSize idx_stage, jvxLinkDataDescriptor* datOut)
{
//#ifdef JVX_NTASK_ENABLE_LOGFILE
//	jvxErrorType resL = JVX_NO_ERROR;
//#endif
	jvxData** bufsInput = jvx_process_icon_extract_input_buffers<jvxData>(datIn, idx_stage);
	jvxData** bufsOutput = jvx_process_icon_extract_output_buffers<jvxData>(*datOut);

	jvxErrorType res = JVX_ERROR_INTERNAL;
	jvxSize i;
	jvxSize posi = 0;
	jvxDspBaseErrorType resl = JVX_DSP_NO_ERROR;
	jvxData out_average = 0;
	jvxData out_min = 0;
	jvxData out_max = 0;
	jvxSize failReason = 0;
	jvxAudioStackDispenserProfile* prof = NULL;
	//jvxCBool logTiming = genSPAsyncio_node::general.cross_threads.log_timing.value;

	/*
	if (logTiming)
	{
		prof = &inProcessing.prof_sec;
	}
	*/
	// Lock the buffering instance
	safeAccessRWConnectionStatus.lock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_SHARED);
	if (safeAccessRWConnectionStatus.v == 0xF)
	{
		// Here, we can be sure that the buffer is allocated
		assert(datIn->con_params.buffersize == datOut->con_params.buffersize);
		assert(datIn->con_params.rate == datOut->con_params.rate);
		assert(datIn->con_params.format == datOut->con_params.format);

		assert(datIn->con_params.number_channels == inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsFromExternal);
		assert(datOut->con_params.number_channels == inProcessing.crossThreadTransfer.myAudioDispenser.numChannelsToExternal);

		resl = jvx_audio_stack_sample_dispenser_cont_external_copy(
			&inProcessing.crossThreadTransfer.myAudioDispenser,
			(jvxHandle**)bufsInput, 0, (jvxHandle**)bufsOutput, 0,
			datIn->con_params.buffersize, 0, prof, NULL, NULL, NULL);

		switch (resl)
		{
		case JVX_DSP_NO_ERROR:
			jvx_audio_stack_sample_dispenser_update_fillheight(
				&inProcessing.crossThreadTransfer.myAudioDispenser, 0, &out_average,
				&out_min, &out_max, NULL, NULL, NULL);

			referencePtr->genSynchronize::rt_info.output.fillheight_avrg.value = referencePtr->genSynchronize::rt_info.output.fillheight_avrg.value * ALPHA_SMOOTH + out_average * (1 - ALPHA_SMOOTH);
			referencePtr->genSynchronize::rt_info.output.fillheight_min.value = out_min;
			referencePtr->genSynchronize::rt_info.output.fillheight_max.value = out_max;
			res = JVX_NO_ERROR;
			break;
		case JVX_DSP_ERROR_ABORT:
			failReason = 1;
			referencePtr->genSynchronize::rt_info.output.number_aborts.value++;
			res = JVX_NO_ERROR;
			break;
		case JVX_DSP_ERROR_BUFFER_OVERFLOW:
			failReason = 2;
			referencePtr->genSynchronize::rt_info.output.number_overflows.value++;
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
	} // if (safeAccessRWConnectionStatus.v == 0x15)
	safeAccessRWConnectionStatus.unlock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_SHARED);
	return res;
}

jvxErrorType
CjvxSpNSynchronize_sec::audio_secondary(jvxLinkDataDescriptor* datIn, jvxSize idx_stage, jvxLinkDataDescriptor* datOut)
{
	jvxSize i;
	jvxSize posi = 0;
#ifdef JVX_NTASK_ENABLE_LOGFILE
	jvxErrorType resL = JVX_NO_ERROR;
#endif
	jvxData** bufsInput = jvx_process_icon_extract_input_buffers<jvxData>(datIn, idx_stage);
	jvxData** bufsOutput = jvx_process_icon_extract_output_buffers<jvxData>(*datOut);

	jvxErrorType res = JVX_ERROR_INTERNAL;
	jvxDspBaseErrorType resl = JVX_DSP_NO_ERROR;
	jvxAudioStackDispenserProfile* prof = NULL;
	jvxSize failReason = 0;
	//jvxCBool logTiming = referencePtr->genSynchronize::general.cross_threads.log_timing.value;
	jvxSize delay = 0;
	//if (logTiming)
	//{
	//	prof = &inProcessing.prof_prim;
	//}
	safeAccessRWConnectionStatus.lock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_SHARED);
	if (safeAccessRWConnectionStatus.v == 0xF)
	{

		resl = jvx_audio_stack_sample_dispenser_cont_internal_copy(&inProcessing.crossThreadTransfer.myAudioDispenser,
			(jvxHandle**)bufsOutput, 0, (jvxHandle**)bufsInput, 0, datOut->con_params.buffersize, 0, prof, NULL, NULL, NULL);
		switch (resl)
		{
		case JVX_DSP_NO_ERROR:

			res = JVX_NO_ERROR;
			break;
		case JVX_DSP_ERROR_ABORT:
			failReason = 1;
			referencePtr->genSynchronize::rt_info.output.number_aborts.value++;
			res = JVX_NO_ERROR;
			break;
		case JVX_DSP_ERROR_BUFFER_OVERFLOW:
			failReason = 2;
			referencePtr->genSynchronize::rt_info.output.number_underflows.value++;
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
	}
	safeAccessRWConnectionStatus.unlock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_SHARED);
	return res;
}


jvxErrorType
CjvxSpNSynchronize_sec::audio_secondary_get(jvxLinkDataDescriptor* datIn, jvxSize idx_stage)
{
	jvxSize i;
	jvxSize posi = 0;

	jvxData** bufsInput = jvx_process_icon_extract_input_buffers<jvxData>(datIn, idx_stage);
	// jvxData** bufsOutput = jvx_process_icon_extract_output_buffers<jvxData>(datOut);

#ifdef JVX_NTASK_ENABLE_LOGFILE
	jvxErrorType resL = JVX_NO_ERROR;
#endif
	jvxErrorType res = JVX_ERROR_INTERNAL;
	jvxDspBaseErrorType resl = JVX_DSP_NO_ERROR;
	jvxAudioStackDispenserProfile* prof = NULL;
	jvxSize failReason = 0;
	// jvxCBool logTiming = referencePtr->genSynchronize::general.cross_threads.log_timing.value;
	jvxSize delay = 0;
	/*
	if (logTiming)
	{
		prof = &inProcessing.prof_prim;
	}
	*/
	
	/*
	resl = jvx_audio_stack_sample_dispenser_cont_internal_copy(&inProcessing.crossThreadTransfer.myAudioDispenser,
		(jvxHandle**)bufsOutput, 0, (jvxHandle**)bufsInput, 0, bsize, 0, prof, NULL, NULL, NULL);
		*/
	switch (resl)
	{
	case JVX_DSP_NO_ERROR:

		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_ABORT:
		failReason = 1;
		referencePtr->genSynchronize::rt_info.output.number_aborts.value++;
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_BUFFER_OVERFLOW:
		failReason = 2;
		referencePtr->genSynchronize::rt_info.output.number_underflows.value++;
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

jvxErrorType
CjvxSpNSynchronize_sec::audio_secondary_put(jvxLinkDataDescriptor* datOut)
{
	return JVX_NO_ERROR;
}

void 
CjvxSpNSynchronize_sec::trigger_process_immediate(jvxLinkDataDescriptor* datIn, jvxSize idx_stage, jvxLinkDataDescriptor* datOut)
{
	safeAccessRWConnectionStatus.lock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_SHARED);
	if ((safeAccessRWConnectionStatus.v == 0xF) &&
		(_common_set_ocon.theData_out.con_link.connect_to))
	{
		jvxSize i;
		jvxData** bufsInputFromPri = jvx_process_icon_extract_input_buffers<jvxData>(datIn, idx_stage);
		jvxData** bufsOutputToPri = jvx_process_icon_extract_output_buffers<jvxData>(*datOut);

		_common_set_ocon.theData_out.con_link.connect_to->process_start_icon();

		jvxData** bufsOutputToSec = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);

		for (i = 0; i < datIn->con_params.number_channels; i++)
		{
			memcpy(bufsOutputToSec[i], bufsInputFromPri[i], jvxDataFormat_getsize(datIn->con_params.format) * datIn->con_params.buffersize);
		}

		// Run processor!!
		_common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();

		// Request data from output side of secondary chain
		if (referencePtr->bufferMode == jvxSynchronizeBufferMode::JVX_SYNCHRONIZE_UNBUFFERED_PUSH)
		{
			if (_common_set_icon.theData_in)
			{
				if (_common_set_icon.theData_in->con_link.connect_from)
				{
					_common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(jvxLinkDataTransferType::JVX_LINKDATA_TRANSFER_REQUEST_DATA, nullptr JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
				}
			}
		}

		jvxData** bufsInputFromSec = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
		for (i = 0; i < datOut->con_params.number_channels; i++)
		{
			memcpy(bufsOutputToPri[i], bufsInputFromSec[i], jvxDataFormat_getsize(datIn->con_params.format) * datIn->con_params.buffersize);
		}

		// run the postprocessor
		_common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
	}
	safeAccessRWConnectionStatus.unlock(jvxRWLockWithVariable<jvxCBitField>::jvxRwLockFunction::JVX_RW_LOCK_SHARED);
}

void
CjvxSpNSynchronize_sec::trigger_process_pull()
{
}


jvxErrorType 
CjvxSpNSynchronize_sec::process_start_icon(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk ,
	jvxHandle* priv_ptr ) 
{
	return CjvxSingleInputConnector::process_start_icon(
		pipeline_offset, idx_stage, tobeAccessedByStage,
		clbk, priv_ptr);
}

jvxErrorType 
CjvxSpNSynchronize_sec::process_stop_icon(jvxSize idx_stage,
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	return CjvxSingleInputConnector::process_stop_icon(
		idx_stage, shift_fwd, tobeAccessedByStage,
		clbk, priv_ptr);
}

