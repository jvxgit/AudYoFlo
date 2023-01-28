#include "CjvxGenericWrapperDevice.h"
#include "CjvxGenericWrapperTechnology.h"
#include "CjvxGenericWrapper_common.h"
#include "CjvxGenericWrapper_common.h"

#define HANDLE_LEVELS
#define CLIP_VALUE 0.99
#define SMOOTH_LEVELS 0.995

// #define VERBOSE_BUFFERING_REPORT

/*
#define CNTMAX 100
static int cnt = 0;
jvxInt64 tStamps[CNTMAX];
jvxInt32 numsIn[CNTMAX];
*/
jvxErrorType
CjvxGenericWrapperDevice::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver,
		jvxSize idx_receiver_to_sender)
{
	jvxErrorType res = JVX_NO_ERROR;
	switch(proc_fields.doSelection)
	{
		case SELECTION_ASYNC_MODE:
			/*
			res = process_async();
			assert(res == JVX_NO_ERROR);
			*/
			assert(0); // This code is no longer active
			break;
		case SELECTION_SYNC_MODE:
			res = process_sync(JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED
			/*theData, idx_sender_to_receiver, idx_receiver_to_sender*/);
			assert(res == JVX_NO_ERROR);
			break;
		default:
			assert(0);
			break;
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::process_sync(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i, j, k;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool fileInvolved_in = false;
	jvxBool fileInvolved_out = false;
	jvxBool fileRunning = false;
	jvxBool autostop_enabled = genGenericWrapper_device::properties_active.auto_stop.value;

	jvxTick timeStart = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);
	//jvxSize idx_sender_to_receiver_local = 0;
	//jvxSize idx_receiver_to_sender_local = 0;

	//jvxLinkDataDescriptor* theData, 
	jvxSize idx_device2genwrapper = *theRelocator._common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;
	jvxSize idx_genwrapper2queue = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;

	jvxSize idx_queue2genwrapper = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;
	jvxSize idx_genwrapper2device = *theRelocator._common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;

	// The following is due to the direct linkage:
	//assert(_common_set_ldslave.theData_out.pipeline.idx_stage == idx_sender_to_receiver);
	//assert(_common_set_ldslave.theData_in->pipeline.idx_stage == idx_receiver_to_sender);

	if (processingControl.inProc.timeStamp_prev_hw > 0)
	{
		jvxData dd = (jvxData)(timeStart - processingControl.inProc.timeStamp_prev_hw);
		jvxData ddhw = dd / processingControl.computedParameters.bSize_hw;
		jvxData ddsw = dd / processingControl.computedParameters.bSize_sw;
		processingControl.inProc.deltaT_hw_avrg = processingControl.inProc.deltaT_hw_avrg * (JVX_GW_ALPHA_SMOOTH_DELTAT)+ddhw * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);
		processingControl.inProc.deltaT_sw_avrg = processingControl.inProc.deltaT_sw_avrg * (JVX_GW_ALPHA_SMOOTH_DELTAT)+ddsw * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);
		genGenericWrapper_device::properties_active_higher.current_rate_hw.value = 1e6 / processingControl.inProc.deltaT_hw_avrg;// delta T in us
		genGenericWrapper_device::properties_active_higher.current_rate_sw.value = 1e6 / processingControl.inProc.deltaT_sw_avrg;// delta T in us
	}
	processingControl.inProc.timeStamp_prev_hw = timeStart;

	// =============================================================================================================
	// HANDLE MULTIPLE BUFFERS: TAKE VALUE FROM FRIVING HARDWARE
	// =============================================================================================================

	// In one case we need to siwtch also the buffers
	/*
	if (proc_fields.seq_operation_in == PROC) 
	{
		// Align buffer indices to hardware
		idx_sender_to_receiver_local = theRelocator._common_set_ldslave.theData_in->pipeline.idx_stage;
	}

	if (proc_fields.seq_operation_out == PROC)
	{
		// Align buffer indices to hardware
		idx_receiver_to_sender_local = theRelocator._common_set_ldslave.theData_out.pipeline.idx_stage;
	}
	*/
	// =============================================================================================================
	// =============================================================================================================

	if (!autostop_enabled)
	{
		fileRunning = true;
	}

	for (i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
	{

		jvxTick timeStart_loc = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);

		if (onInit.theFilesRef->theInputFiles[i].common.processing.selected)
		{
			fileInvolved_in = true;

			if(onInit.theFilesRef->theInputFiles[i].flag_bwd)
			{
				onInit.theFilesRef->theInputFiles[i].flag_bwd = 0;
				onInit.theFilesRef->theInputFiles[i].theReader->wind_backward(
					onInit.theFilesRef->theInputFiles[i].common.samplerate * JVX_GW_WIND_SECONDS);
			}
			if (onInit.theFilesRef->theInputFiles[i].flag_fwd)
			{
				onInit.theFilesRef->theInputFiles[i].flag_fwd = 0;
				onInit.theFilesRef->theInputFiles[i].theReader->wind_forward(
					onInit.theFilesRef->theInputFiles[i].common.samplerate * JVX_GW_WIND_SECONDS);
			}
			if (onInit.theFilesRef->theInputFiles[i].flag_restart)
			{
				onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS;
				onInit.theFilesRef->theInputFiles[i].flag_restart = 0;
				onInit.theFilesRef->theInputFiles[i].theReader->restart(0);
				if (onInit.theFilesRef->theInputFiles[i].common.pauseOnStart)
				{
					onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS_PAUSED;
				}
			}
			if (onInit.theFilesRef->theInputFiles[i].flag_pause)
			{
				onInit.theFilesRef->theInputFiles[i].flag_pause = 0;
				onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS_PAUSED;
			}
			if (onInit.theFilesRef->theInputFiles[i].flag_play)
			{
				onInit.theFilesRef->theInputFiles[i].flag_play = 0;
				onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS;
			}

			// ---------------------------

			switch (onInit.theFilesRef->theInputFiles[i].common.processing.theStatus)
			{
			case FILE_ACCESS:

				fileRunning = true;
				if (onInit.theFilesRef->theInputFiles[i].common.processing.resampling)
				{

					resL = onInit.theFilesRef->theInputFiles[i].theReader->get_audio_buffer(
						reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS),
						onInit.theFilesRef->theInputFiles[i].common.number_channels,
						onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs,
						false, &onInit.theFilesRef->theInputFiles[i].progress);

					for (j = 0; j < (jvxSize)onInit.theFilesRef->theInputFiles[i].common.number_channels; j++)
					{
						onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theHdl->process(
							onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS[j],
							onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[idx_genwrapper2queue][j]);
					}
				}
				else
				{
					resL = onInit.theFilesRef->theInputFiles[i].theReader->get_audio_buffer(
						reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[idx_genwrapper2queue]),
						onInit.theFilesRef->theInputFiles[i].common.number_channels,
						onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs,
						false, &onInit.theFilesRef->theInputFiles[i].progress);
				}
				if (resL != JVX_NO_ERROR)
				{
					switch (resL)
					{
					case JVX_ERROR_BUFFER_UNDERRUN:
						// Do kind of a report....
						onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS_DROPOUT;
						_report_text_message(("Buffer underrun in file " + onInit.theFilesRef->theInputFiles[i].common.name).c_str(), JVX_REPORT_PRIORITY_WARNING);
						onInit.theFilesRef->theInputFiles[i].common.processing.lostFrames = 1;

						break;
					case JVX_ERROR_COMPONENT_BUSY:
						break;
					default:
						onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS_COMPLETED;
					}
				}
				break;

			case FILE_ACCESS_DROPOUT:

				/* We need this state to avoid endless message postings */
				fileRunning = true;
				if (onInit.theFilesRef->theInputFiles[i].common.processing.resampling)
				{

					resL = onInit.theFilesRef->theInputFiles[i].theReader->get_audio_buffer(
						reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS),
						onInit.theFilesRef->theInputFiles[i].common.number_channels,
						onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs,
						false, &onInit.theFilesRef->theInputFiles[i].progress);

					for (j = 0; j < (jvxSize)onInit.theFilesRef->theInputFiles[i].common.number_channels; j++)
					{
						onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theHdl->process(
							onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS[j],
							onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[idx_genwrapper2queue][j]);
					}
				}
				else
				{
					resL = onInit.theFilesRef->theInputFiles[i].theReader->get_audio_buffer(
						reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[idx_genwrapper2queue]),
						onInit.theFilesRef->theInputFiles[i].common.number_channels,
						onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs,
						false, &onInit.theFilesRef->theInputFiles[i].progress);
				}
				if (resL != JVX_NO_ERROR)
				{
					switch (resL)
					{
					case JVX_ERROR_BUFFER_UNDERRUN:
						onInit.theFilesRef->theInputFiles[i].common.processing.lostFrames++;
						break;
					default:
						if (autostop_enabled)
						{
							onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS_COMPLETED;
						}
					}
				}
				else
				{
					_report_text_message(("Lost " + jvx_size2String(onInit.theFilesRef->theInputFiles[i].common.processing.lostFrames) + " frames in file " + onInit.theFilesRef->theInputFiles[i].common.name).c_str(), JVX_REPORT_PRIORITY_WARNING);
					onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS;
				}
				break;
			case FILE_ACCESS_PAUSED:

				fileRunning = true;

			case FILE_ACCESS_COMPLETED:
				for (j = 0; j < (jvxSize)onInit.theFilesRef->theInputFiles[i].common.number_channels; j++)
				{
					memset(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[idx_genwrapper2queue][j],
						0, proc_fields.szElementSW * processingControl.computedParameters.bSize_sw);
				}
				break;
			}
		}
		jvxTick timeStop_loc = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);
		jvxTick deltaT_loc = timeStop_loc - timeStart_loc;

		onInit.theFilesRef->theInputFiles[i].common.processing.deltaT_avrg = onInit.theFilesRef->theInputFiles[i].common.processing.deltaT_avrg* (JVX_GW_ALPHA_SMOOTH_DELTAT)+deltaT_loc * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);

	}

	for (i = 0; i < onInit.theExtRef->theInputReferences.size(); i++)
	{

		//jvxInt64 timeStart_loc = JVX_GET_TICKCOUNT_US_GET(processingControl.inProc.tStampRef);

		if (onInit.theExtRef->theInputReferences[i].processing.selected)
		{
			fileInvolved_in = true;
			switch (onInit.theExtRef->theInputReferences[i].processing.theStatus)
			{
			case FILE_ACCESS:

				if (onInit.theExtRef->theInputReferences[i].processing.resampling)
				{
					resL = onInit.theExtRef->theInputReferences[i].theCallback->get_one_frame(
						onInit.theExtRef->theInputReferences[i].uniqueIdHandles,
						reinterpret_cast<jvxHandle**>(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS));

					for (j = 0; j < (jvxSize)onInit.theExtRef->theInputReferences[i].number_channels; j++)
					{
						onInit.theExtRef->theInputReferences[i].processing.resampler[j].theHdl->process(
							onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS[j],
							onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW[idx_genwrapper2queue][j]);
					}
				}
				else
				{
					resL = onInit.theExtRef->theInputReferences[i].theCallback->get_one_frame(
						onInit.theExtRef->theInputReferences[i].uniqueIdHandles,
						reinterpret_cast<jvxHandle**>(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW[idx_genwrapper2queue]));
				}
				if (resL == JVX_NO_ERROR)
				{
					fileRunning = true;
				}
				else
				{
					if (resL == JVX_ERROR_END_OF_FILE)
					{
						onInit.theExtRef->theInputReferences[i].processing.theStatus = FILE_ACCESS_COMPLETED;
					}
					else
					{
						fileRunning = true;
						onInit.theExtRef->theInputReferences[i].processing.numberLostFrames++;
					}
				}
				break;

			default:
				for (j = 0; j < (jvxSize)onInit.theExtRef->theInputReferences[i].number_channels; j++)
				{
					memset(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW[idx_genwrapper2queue][j],
						0, proc_fields.szElementSW * processingControl.computedParameters.bSize_sw);
				}
				break;
			}
		}
		/*
		jvxInt64 timeStop_loc = JVX_GET_TICKCOUNT_US_GET(processingControl.inProc.tStampRef);
		jvxInt64 deltaT_loc = timeStop_loc - timeStart_loc;

		onInit.theFilesRef->theInputFiles[i].common.processing.deltaT_avrg = onInit.theFilesRef->theInputFiles[i].common.processing.deltaT_avrg* (JVX_GW_ALPHA_SMOOTH_DELTAT) + deltaT_loc * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);
		*/
	}

	// Preset dummy buffers to zero
	for (i = 0; i < onInit.theDummyRef->inputs.size(); i++)
	{
		if (onInit.theDummyRef->inputs[i].wasStarted)
		{
			memset(onInit.theDummyRef->inputs[i].buf[idx_genwrapper2queue], 0, proc_fields.szElementSW * processingControl.computedParameters.bSize_sw);
		}
	}

	/*
	for (i = 0; i < onInit.theDummyRef->outputs.size(); i++)
	{
		if (onInit.theDummyRef->outputs[i].wasStarted)
		{
			memset(onInit.theDummyRef->outputs[i].buf[idx_receiver_to_sender], 0,
				proc_fields.szElementSW * processingControl.computedParameters.bSize_sw);
		}
	}*/

	// Stop if file has been completed!
	if (fileInvolved_in && !fileRunning)
	{
		// Send end signal only once
		if (proc_fields.threads.endTriggered == false)
		{
			_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_PROCESS_SHIFT);
			proc_fields.threads.endTriggered = true;
		}
	}

	switch (proc_fields.seq_operation_in)
	{
	case NOTHING:
	case PROC:
		// nothing to be done, all managed by channel routings
		break;
	case RES_PROC:

#ifdef VERBOSE_BUFFERING_REPORT
		std::cout << "input side conversion: Map idx " << idx_device2genwrapper << "/"
			<< theRelocator._common_set_ldslave.theData_in->con_data.number_buffers
			<< " to " << idx_genwrapper2queue << "/"
			<< _common_set_ldslave.theData_out.con_data.number_buffers
			<< std::endl;
#endif

		for (i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
		{
			//idxOut = proc_fields.mapToOutput_in[i];
			jvxSize idxIn = runtime.channelMappings.inputChannelMapper_hwrev[i];
			proc_fields.resamplersIn_hdl[i]->process(
				proc_fields.ptrA_in_net[idx_device2genwrapper][i], 
				_common_set_ldslave.theData_out.con_data.buffers[idx_genwrapper2queue][idxIn]);
		}
		break;

	case DACON_PROC:

#ifdef VERBOSE_BUFFERING_REPORT
		std::cout << "input side conversion: Map idx " << idx_device2genwrapper << "/"
			<< theRelocator._common_set_ldslave.theData_in->con_data.number_buffers
			<< " to " << idx_genwrapper2queue << "/"
			<< _common_set_ldslave.theData_out.con_data.number_buffers
			<< std::endl;
#endif


		for (i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
		{
			//idxOut = proc_fields.mapToOutput_in[i];
			jvxSize idxIn = runtime.channelMappings.inputChannelMapper_hwrev[i];
			proc_fields.convertersIn_hdl[i]->process(
				proc_fields.ptrA_in_net[idx_device2genwrapper][i], 
				_common_set_ldslave.theData_out.con_data.buffers[idx_genwrapper2queue][idxIn],
				processingControl.computedParameters.bSize_sw);
		}
		break;
	case RES_DACON_PROC:

#ifdef VERBOSE_BUFFERING_REPORT
		std::cout << "input side conversion: Map idx " << idx_device2genwrapper << "/" 
			<< theRelocator._common_set_ldslave.theData_in->con_data.number_buffers 
			<< " to " << idx_genwrapper2queue << "/" 
			<< _common_set_ldslave.theData_out.con_data.number_buffers 
			<< std::endl;
#endif

		for (i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
		{
			jvxSize idxIn = runtime.channelMappings.inputChannelMapper_hwrev[i];
			//idxOut = proc_fields.mapToOutput_in[i];
			proc_fields.resamplersIn_hdl[i]->process(
				proc_fields.ptrA_in_net[idx_device2genwrapper][i], proc_fields.ptrB_in[i]);
			proc_fields.convertersIn_hdl[i]->process(
				proc_fields.ptrB_in[i],
				_common_set_ldslave.theData_out.con_data.buffers[idx_genwrapper2queue][idxIn],
				processingControl.computedParameters.bSize_sw);
		}
		break;
	default:
		// Not yet implemented
		assert(0);
		break;
	}


	if (jvx_bitTest(properties_active_higher.levelHandlingInput.value.selection(), 0))
	{		
		assert(processingControl.inProc.params_fixed_runtime.chans_in == _common_set_ldslave.theData_out.con_params.number_channels);
		jvx_fieldLevelGainClip(
			_common_set_ldslave.theData_out.con_data.buffers[idx_genwrapper2queue],
			_common_set_ldslave.theData_out.con_params.number_channels,
			_common_set_ldslave.theData_out.con_params.buffersize,
			_common_set_ldslave.theData_out.con_params.format,
			processingControl.levels.input_avrg,
			processingControl.levels.input_max,
			processingControl.levels.input_gain,
			processingControl.levels.input_clip,
			processingControl.levels.input_channel_map,
			CLIP_VALUE, SMOOTH_LEVELS);
	}

	// =======================================================================
	// Forward data to next connected object
	// =======================================================================
	_common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();

	for(i = 0; i < _common_set_ldslave.theData_out.con_params.number_channels; i++)
	{
		jvxSize mapi = processingControl.levels.input_channel_map[i];
		if(processingControl.levels.direct[mapi] == 1)
		{
			for(k = 0; k < processingControl.levels.num_entries_output; k++)
			{
				jvx_fieldMix(_common_set_ldslave.theData_out.con_data.buffers[idx_genwrapper2queue][i],
					_common_set_ldslave.theData_in->con_data.buffers[idx_queue2genwrapper][k],
					_common_set_ldslave.theData_in->con_params.buffersize, _common_set_ldslave.theData_in->con_params.format);
			}
		}
	}


	if(jvx_bitTest(properties_active_higher.levelHandlingOutput.value.selection(),0))
	{
		assert(processingControl.inProc.params_fixed_runtime.chans_out == _common_set_ldslave.theData_in->con_params.number_channels);
		jvx_fieldLevelGainClip(
			_common_set_ldslave.theData_in->con_data.buffers[idx_queue2genwrapper],
			_common_set_ldslave.theData_in->con_params.number_channels,
			_common_set_ldslave.theData_in->con_params.buffersize,
			_common_set_ldslave.theData_in->con_params.format,
			processingControl.levels.output_avrg,
			processingControl.levels.output_max,
			processingControl.levels.output_gain,
			processingControl.levels.output_clip,
			processingControl.levels.output_channel_map,
			CLIP_VALUE, SMOOTH_LEVELS);
	}
	
	// =======================================================================
	// Output side
	// =======================================================================
	switch(proc_fields.seq_operation_out)
	{
	case NOTHING:
	case PROC:
		// nothing to be done, all managed by channel routings
		break;
	case RES_PROC:

#ifdef VERBOSE_BUFFERING_REPORT
		std::cout << "output side conversion: Map idx " << idx_queue2genwrapper << "/"
			<< _common_set_ldslave.theData_in->con_data.number_buffers
			<< " to " << idx_genwrapper2device << "/"
			<< theRelocator._common_set_ldslave.theData_out.con_data.number_buffers
			<< std::endl;
#endif

		for (i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
		{
			jvxSize idxOut = i; // runtime.channelMappings.outputChannelMapper_hwrev[i];
			proc_fields.resamplersOut_hdl[i]->process(
				proc_fields.ptrA_out_net[idx_queue2genwrapper][i],
				theRelocator._common_set_ldslave.theData_out.con_data.buffers[idx_genwrapper2device][idxOut]);
		}
		break;

	case DACON_PROC:

#ifdef VERBOSE_BUFFERING_REPORT
		std::cout << "output side conversion: Map idx " << idx_queue2genwrapper << "/"
			<< _common_set_ldslave.theData_in->con_data.number_buffers
			<< " to " << idx_genwrapper2device << "/"
			<< theRelocator._common_set_ldslave.theData_out.con_data.number_buffers
			<< std::endl;
#endif

		for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
		{
			jvxSize idxOut = i; // runtime.channelMappings.outputChannelMapper_hwrev[i];
			proc_fields.convertersOut_hdl[i]->process(
				proc_fields.ptrA_out_net[*_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr][i],
				theRelocator._common_set_ldslave.theData_out.con_data.buffers[idx_genwrapper2device][idxOut],
				processingControl.computedParameters.bSize_sw);
		}
		break;
	case RES_DACON_PROC:

#ifdef VERBOSE_BUFFERING_REPORT
		std::cout << "output side conversion: Map idx " << idx_queue2genwrapper << "/"
			<< _common_set_ldslave.theData_in->con_data.number_buffers
			<< " to " << idx_genwrapper2device << "/"
			<< theRelocator._common_set_ldslave.theData_out.con_data.number_buffers
			<< std::endl;
#endif

		for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
		{
			jvxSize idxOut = i; // runtime.channelMappings.outputChannelMapper_hwrev[i];
			proc_fields.convertersOut_hdl[i]->process(
				proc_fields.ptrA_out_net[*_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr][i],
				proc_fields.ptrB_out[i],
				processingControl.computedParameters.bSize_sw);
			proc_fields.resamplersOut_hdl[i]->process(
				proc_fields.ptrB_out[i], 
				theRelocator._common_set_ldslave.theData_out.con_data.buffers[idx_genwrapper2device][idxOut]);
		}
		break;
	default:
		// Not yet implemented
		assert(0);
		break;
	}

	jvxTick timeStop = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);
	jvxTick deltaT = timeStop - timeStart;


	processingControl.measureLoad.deltaT_avrg = processingControl.measureLoad.deltaT_avrg  * (JVX_GW_ALPHA_SMOOTH_DELTAT) + deltaT * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);
	if(deltaT > processingControl.measureLoad.deltaT_max)
	{
		processingControl.measureLoad.deltaT_max  = JVX_INT64_DATA(deltaT);
	}
	else
	{
		processingControl.measureLoad.deltaT_max *= JVX_GW_ALPHA_RELEASE_DELTAT_MAX;
	}

	genGenericWrapper_device::properties_active_higher.loadpercent.value = processingControl.measureLoad.deltaT_avrg / processingControl.measureLoad.deltaT_theory * 100.0;

	fileInvolved_out = false;
	fileRunning = false;
	for(i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
	{
		jvxTick timeStart_loc = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);

		if(onInit.theFilesRef->theOutputFiles[i].common.processing.selected)
		{
			fileInvolved_out = true;
			switch(onInit.theFilesRef->theOutputFiles[i].common.processing.theStatus)
			{
			case FILE_ACCESS:

				fileRunning = true;
				if(onInit.theFilesRef->theOutputFiles[i].common.processing.resampling)
				{
					for(j = 0; j < (jvxSize)onInit.theFilesRef->theOutputFiles[i].common.number_channels; j++)
					{
						onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theHdl->process(
							onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[idx_queue2genwrapper][j],
							onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS[j]);
					}

					resL = onInit.theFilesRef->theOutputFiles[i].theWriter->add_audio_buffer(
						reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS),
						onInit.theFilesRef->theOutputFiles[i].common.number_channels,
						onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs,
						false);

				}
				else
				{
					resL = onInit.theFilesRef->theOutputFiles[i].theWriter->add_audio_buffer(
						reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[idx_queue2genwrapper]),
						onInit.theFilesRef->theOutputFiles[i].common.number_channels,
						onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs,
						false);
				}
				if(resL != JVX_NO_ERROR)
				{
					switch(resL)
					{
					case JVX_ERROR_BUFFER_OVERFLOW:
					case JVX_ERROR_POSTPONE:
						// Do kind of a report....
						onInit.theFilesRef->theOutputFiles[i].common.processing.theStatus = FILE_ACCESS_DROPOUT;
						_report_text_message(("Buffer overrun in file " + onInit.theFilesRef->theOutputFiles[i].common.name).c_str(), JVX_REPORT_PRIORITY_WARNING);
						onInit.theFilesRef->theOutputFiles[i].common.processing.lostFrames = 1;

						break;
					case JVX_ERROR_END_OF_FILE:

						onInit.theFilesRef->theOutputFiles[i].common.processing.theStatus = FILE_ACCESS_COMPLETED;
						_report_text_message(("Buffer overrun in file " + onInit.theFilesRef->theOutputFiles[i].common.name).c_str(), JVX_REPORT_PRIORITY_WARNING);
						onInit.theFilesRef->theOutputFiles[i].common.processing.lostFrames = 1;
						
					default:

						assert(0);
					}
				}
				break;
			case FILE_ACCESS_DROPOUT:

				fileRunning = true;
				if(onInit.theFilesRef->theOutputFiles[i].common.processing.resampling)
				{
					for(j = 0; j < (jvxSize)onInit.theFilesRef->theOutputFiles[i].common.number_channels; j++)
					{
						onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theHdl->process(
							onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[idx_queue2genwrapper][j],
							onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS[j]);
					}

					resL = onInit.theFilesRef->theOutputFiles[i].theWriter->add_audio_buffer(
						reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS),
						onInit.theFilesRef->theOutputFiles[i].common.number_channels,
						onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs,
						false);

				}
				else
				{
					resL = onInit.theFilesRef->theOutputFiles[i].theWriter->add_audio_buffer(
						reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[idx_queue2genwrapper]),
						onInit.theFilesRef->theOutputFiles[i].common.number_channels,
						onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs,
						false);
				}
				if(resL != JVX_NO_ERROR)
				{
					switch(resL)
					{
					case JVX_ERROR_BUFFER_OVERFLOW:
					case JVX_ERROR_POSTPONE:
					case JVX_ERROR_END_OF_FILE:
						onInit.theFilesRef->theOutputFiles[i].common.processing.lostFrames++;
						break;
					default:
						assert(0);
					}
				}
				else
				{
					_report_text_message(("Lost " + jvx_size2String(onInit.theFilesRef->theOutputFiles[i].common.processing.lostFrames) + " frames in file " + onInit.theFilesRef->theOutputFiles[i].common.name).c_str(), JVX_REPORT_PRIORITY_WARNING);
					onInit.theFilesRef->theOutputFiles[i].common.processing.theStatus = FILE_ACCESS;
				}
				break;
			}
		}
		jvxTick timeStop_loc = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);
		jvxTick deltaT_loc = timeStop_loc - timeStart_loc;

		onInit.theFilesRef->theOutputFiles[i].common.processing.deltaT_avrg = onInit.theFilesRef->theOutputFiles[i].common.processing.deltaT_avrg* (JVX_GW_ALPHA_SMOOTH_DELTAT) + deltaT_loc * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);
	}




	for(i = 0; i < onInit.theExtRef->theOutputReferences.size(); i++)
	{

		//jvxInt64 timeStart_loc = JVX_GET_TICKCOUNT_US_GET(processingControl.inProc.tStampRef);

		if(onInit.theExtRef->theOutputReferences[i].processing.selected)
		{
			fileInvolved_out = true;
			switch(onInit.theExtRef->theOutputReferences[i].processing.theStatus)
			{
			case FILE_ACCESS:
				fileRunning = true;
				if(onInit.theExtRef->theOutputReferences[i].processing.resampling)
				{
					for (j = 0; j < (jvxSize)onInit.theExtRef->theOutputReferences[i].number_channels; j++)
					{
						onInit.theExtRef->theOutputReferences[i].processing.resampler[j].theHdl->process(
							onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW[idx_queue2genwrapper][j],
							onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesRS[j]
							);
					}

					resL = onInit.theExtRef->theOutputReferences[i].theCallback->put_one_frame(
						onInit.theExtRef->theOutputReferences[i].uniqueIdHandles,
						reinterpret_cast<jvxHandle**>(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesRS));

				}
				else
				{
					resL = onInit.theExtRef->theOutputReferences[i].theCallback->put_one_frame(
						onInit.theExtRef->theOutputReferences[i].uniqueIdHandles,
						reinterpret_cast<jvxHandle**>(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW[idx_queue2genwrapper]));
				}
				if(resL != JVX_NO_ERROR)
				{
					onInit.theExtRef->theOutputReferences[i].processing.numberLostFrames++;
				}
				break;

			default:
				for(j = 0; j < (jvxSize)onInit.theExtRef->theOutputReferences[i].number_channels; j++)
				{
					memset(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW[idx_queue2genwrapper][j],
						0, proc_fields.szElementSW * processingControl.computedParameters.bSize_sw);
				}
				break;
			}
		}
	}

	// Stop if file has been completed!
	if (fileInvolved_out && !fileRunning)
	{
		// Send end signal only once
		if (proc_fields.threads.endTriggered == false)
		{
			_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_PROCESS_SHIFT);
			proc_fields.threads.endTriggered = true;
		}
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::process_async(/*jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver,
	jvxSize idx_receiver_to_sender*/
	jvxSize mt_mask, jvxSize idx_stage)
{
	// _common_set_ldslave.theData_in, _common_set_ldslave.theData_in->pipeline.idx_stage, _common_set_ldslave.theData_out.pipeline.idx_stage
	jvxSize i;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxTick timeStart = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);

	jvxInt32 numIn = processingControl.computedParameters.bSize_hw_ds;
	jvxInt32 spaceOut = processingControl.computedParameters.bSize_hw_ds;

	jvxSize idx_sender_to_receiver = *theRelocator._common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;
	jvxSize idx_receiver_to_sender = *theRelocator._common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;

#if defined( JVX_ASYNC_BUFFER_DEBUG_CNT)
	jvxTick tt = 0;
#endif

#if defined( JVX_ASYNC_BUFFER_DEBUG_CNT)
	tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.dbg_tstamp);
	JVX_LOCK_MUTEX(runtime.dbg_safeAccess);
	runtime.dbg_events[runtime.dbg_events_cnt].tStamp = tt;
	runtime.dbg_events[runtime.dbg_events_cnt].event = 0;
	runtime.dbg_events_cnt = (runtime.dbg_events_cnt + 1) % JVX_ASYNC_BUFFER_DEBUG_CNT;
	JVX_UNLOCK_MUTEX(runtime.dbg_safeAccess);
#endif

#ifdef VERBOSE_BUFFERING_REPORT
	std::cout << "async input side, map bidx " << idx_sender_to_receiver << "/"
		<< theRelocator._common_set_ldslave.theData_in->con_data.number_buffers 
		<< " to synchronizer" << std::endl;
	std::cout << "async input side, map synchronizer to bidx " << idx_receiver_to_sender << "/"
		<< theRelocator._common_set_ldslave.theData_out.con_data.number_buffers << std::endl;
#endif

	if(processingControl.inProc.timeStamp_prev_hw > 0)
	{
		jvxData dd = (jvxData)(timeStart - processingControl.inProc.timeStamp_prev_hw);
		dd /= processingControl.computedParameters.bSize_hw;
		processingControl.inProc.deltaT_hw_avrg = processingControl.inProc.deltaT_hw_avrg * (JVX_GW_ALPHA_SMOOTH_DELTAT) + dd * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);
		genGenericWrapper_device::properties_active_higher.current_rate_hw.value = 1e6 /processingControl.inProc.deltaT_hw_avrg;// delta T in us
	}
	processingControl.inProc.timeStamp_prev_hw = timeStart;

	// Before processing, check that there are enough input samples and space in output buffer
	if(processingControl.inProc.params_fixed_runtime.chanshw_in > 0)
	{
		// Check the space in circular buffer for write
		jvxInt32 spaceLoc = spaceOut;
		for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
		{
			resL = proc_fields.circularBuffersIn[i]->spaceInBuffer(&spaceLoc);
			spaceOut = JVX_MIN(spaceOut, spaceLoc);
		}
	}
	if(processingControl.inProc.params_fixed_runtime.chanshw_out > 0)
	{
		// Check the number samples in circular buffer for read
		jvxInt32 numLoc = numIn;
		for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
		{
			resL = proc_fields.circularBuffersOut[i]->elementsInBuffer(&numIn);
			numIn = JVX_MIN(numIn, numLoc);
		}
	}
/*
	if(cnt < CNTMAX)
	{
		tStamps[cnt] = theData->tStamp_us;
		numsIn[cnt] = numIn;
		cnt++;
	}
	else
	{
		cnt = 0;
	}
		*/

	// If we have an underrun, report it
	if((numIn < processingControl.computedParameters.bSize_hw_ds) || (spaceOut < processingControl.computedParameters.bSize_hw_ds))
	{
#if defined( JVX_ASYNC_BUFFER_DEBUG_CNT)
		tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.dbg_tstamp);
		JVX_LOCK_MUTEX(runtime.dbg_safeAccess);
		runtime.dbg_events[runtime.dbg_events_cnt].tStamp = tt;
		runtime.dbg_events[runtime.dbg_events_cnt].event = 1;
		runtime.dbg_events_cnt = (runtime.dbg_events_cnt + 1) % JVX_ASYNC_BUFFER_DEBUG_CNT;
		JVX_UNLOCK_MUTEX(runtime.dbg_safeAccess);
#endif

		if((numIn < processingControl.computedParameters.bSize_hw_ds))
		{
			//_report_text_message("Circular buffer underrun in collecting thread.", JVX_REPORT_PRIORITY_WARNING);
		}
		if((spaceOut < processingControl.computedParameters.bSize_hw_ds))
		{
			//_report_text_message("Circular buffer overrun in collecting thread.", JVX_REPORT_PRIORITY_WARNING);
		}
		// Output 0 values instead
		for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
		{
			memset(theRelocator._common_set_ldslave.theData_out.con_data.buffers[idx_receiver_to_sender][i], 0, jvxDataFormat_size[processingControl.computedParameters.form_hw]);
		}
		//this->processingControl.performance.numAudioThreadFailed++; // <-report number of lost frames
		CjvxAudioDevice_genpcg::properties_active.lostBuffers.value++;
	}
	else
	{
		switch(proc_fields.seq_operation_in)
		{
		case NOTHING:
			break;

		case TB_PROC:

			for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				proc_fields.circularBuffersIn[i]->write(proc_fields.ptrA_in_net[idx_sender_to_receiver][i], processingControl.computedParameters.bSize_hw_ds,0);
			}
			break;

		case RES_TB_PROC:
			for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				proc_fields.resamplersIn_hdl[i]->process(
					proc_fields.ptrA_in_net[idx_sender_to_receiver][i], proc_fields.ptrB_in[i]);
				resL = proc_fields.circularBuffersIn[i]->write(proc_fields.ptrB_in[i], processingControl.computedParameters.bSize_hw_ds,0);
				if(resL != JVX_NO_ERROR)
				{
					_report_text_message("Circular buffer write failed in collecting thread.", JVX_REPORT_PRIORITY_WARNING);
				}

			}
			break;

		case DACON_TB_PROC:
			for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				proc_fields.convertersIn_hdl[i]->process(
					proc_fields.ptrA_in_net[idx_sender_to_receiver][i], proc_fields.ptrB_in[i], processingControl.computedParameters.bSize_hw);
				resL = proc_fields.circularBuffersIn[i]->write(proc_fields.ptrB_in[i], processingControl.computedParameters.bSize_hw_ds,0);
				if(resL != JVX_NO_ERROR)
				{
					_report_text_message("Circular buffer write failed in collecting thread.", JVX_REPORT_PRIORITY_WARNING);
				}
			}
			break;
		case RES_DACON_TB_PROC:

			for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				proc_fields.resamplersIn_hdl[i]->process(
					proc_fields.ptrA_in_net[idx_sender_to_receiver][i], proc_fields.ptrB_in[i]);
				proc_fields.convertersIn_hdl[i]->process(
					proc_fields.ptrB_in[i],
					proc_fields.ptrC_in[i],
					processingControl.computedParameters.bSize_hw_ds);
				resL = proc_fields.circularBuffersIn[i]->write(proc_fields.ptrC_in[i], processingControl.computedParameters.bSize_hw_ds,0);
				if(resL != JVX_NO_ERROR)
				{
					_report_text_message("Circular buffer write failed in collecting thread.", JVX_REPORT_PRIORITY_WARNING);
				}
			}
			break;
		default:
			// Not yet implemented
			break;
		}

		// =======================================================================
		// Output side
		// =======================================================================
		switch(proc_fields.seq_operation_out)
		{
		case NOTHING:
			break;

		case TB_PROC:

			for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				resL = proc_fields.circularBuffersOut[i]->read((jvxByte*)
					theRelocator._common_set_ldslave.theData_out.con_data.buffers[idx_receiver_to_sender][i], processingControl.computedParameters.bSize_hw);
				if(resL != JVX_NO_ERROR)
				{
					_report_text_message("Circular buffer read failed in collecting thread.", JVX_REPORT_PRIORITY_WARNING);
				}
			}
			break;
			// nothing to be done, all managed by channel routings

		case RES_TB_PROC:
			for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				resL = proc_fields.circularBuffersOut[i]->read(proc_fields.ptrB_out[i], processingControl.computedParameters.bSize_hw_ds);
				if(resL != JVX_NO_ERROR)
				{
					_report_text_message("Circular buffer read failed in collecting thread.", JVX_REPORT_PRIORITY_WARNING);
				}
				proc_fields.resamplersOut_hdl[i]->process(
					proc_fields.ptrB_out[i], theRelocator._common_set_ldslave.theData_out.con_data.buffers[idx_receiver_to_sender][i]);
			}
			break;

		case DACON_TB_PROC:
			for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				resL = proc_fields.circularBuffersOut[i]->read(proc_fields.ptrB_out[i], processingControl.computedParameters.bSize_hw_ds);
				if(resL != JVX_NO_ERROR)
				{
					_report_text_message("Circular buffer read failed in collecting thread.", JVX_REPORT_PRIORITY_WARNING);
				}
				proc_fields.convertersOut_hdl[i]->process(
					proc_fields.ptrB_out[i], theRelocator._common_set_ldslave.theData_out.con_data.buffers[idx_receiver_to_sender][i],
					processingControl.computedParameters.bSize_hw_ds);
			}
			break;
		case RES_DACON_TB_PROC:

			for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				resL = proc_fields.circularBuffersOut[i]->read(proc_fields.ptrB_out[i], processingControl.computedParameters.bSize_hw_ds);
				if(resL != JVX_NO_ERROR)
				{
					_report_text_message("Circular buffer read failed in collecting thread.", JVX_REPORT_PRIORITY_WARNING);
				}
				proc_fields.convertersOut_hdl[i]->process(
					proc_fields.ptrB_out[i],
					proc_fields.ptrC_out[i],
					processingControl.computedParameters.bSize_hw_ds);
				proc_fields.resamplersOut_hdl[i]->process(
					proc_fields.ptrC_out[i], theRelocator._common_set_ldslave.theData_out.con_data.buffers[idx_receiver_to_sender][i]);
			}
			break;
		default:
			// Not yet implemented
			assert(0);
			break;
		}
	}

#if defined( JVX_ASYNC_BUFFER_DEBUG_CNT)
	tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.dbg_tstamp);
	JVX_LOCK_MUTEX(runtime.dbg_safeAccess);
	runtime.dbg_events[runtime.dbg_events_cnt].tStamp = tt;
	runtime.dbg_events[runtime.dbg_events_cnt].event = 2;
	runtime.dbg_events_cnt = (runtime.dbg_events_cnt + 1) % JVX_ASYNC_BUFFER_DEBUG_CNT;
	JVX_UNLOCK_MUTEX(runtime.dbg_safeAccess);
#endif

	// Indicate that next buffer is ready for processing
	jvx_thread_wakeup(proc_fields.threads.thread_handle, true);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxGenericWrapperDevice::static_process_async(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	if(privateData_thread)
	{
		CjvxGenericWrapperDevice* this_pointer = reinterpret_cast<CjvxGenericWrapperDevice*>(privateData_thread);
		return this_pointer->iclass_process_async(timestamp_us);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxGenericWrapperDevice::iclass_process_async(jvxInt64 timestamp_us)
{
	jvxSize i,j,k;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType resL_start = JVX_NO_ERROR;
	jvxErrorType resL_stop = JVX_NO_ERROR;
	jvxErrorType resL_proc = JVX_NO_ERROR;
	jvxBool fileInvolved_in = false;
	jvxBool fileInvolved_out = false;
	jvxBool fileRunning = false;
	jvxBool triggerEnd = false;
	jvxBool noInputOutput = false;

	jvxTick timeStart = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);
	jvxInt32 numIn = processingControl.computedParameters.bSize_sw;
	jvxInt32 spaceOut = processingControl.computedParameters.bSize_sw;
	jvxBool keepOnProcessing = true;

#if defined( JVX_ASYNC_BUFFER_DEBUG_CNT)
	jvxTick tt = 0;
#endif

#if defined( JVX_ASYNC_BUFFER_DEBUG_CNT)
	tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.dbg_tstamp);
	JVX_LOCK_MUTEX(runtime.dbg_safeAccess);
	runtime.dbg_events[runtime.dbg_events_cnt].tStamp = tt;
	runtime.dbg_events[runtime.dbg_events_cnt].event = 3;
	runtime.dbg_events_cnt = (runtime.dbg_events_cnt + 1) % JVX_ASYNC_BUFFER_DEBUG_CNT;
	JVX_UNLOCK_MUTEX(runtime.dbg_safeAccess);
#endif

	if(runtime.threadCtrl.coreHdl.hdl)
	{
		if(!runtime.threadCtrl.asyncAudioThreadAssoc)
		{
			runtime.threadCtrl.coreHdl.hdl->associate_tc_thread(JVX_GET_CURRENT_THREAD_ID(), runtime.threadCtrl.asyncAudioThreadId);
			runtime.threadCtrl.asyncAudioThreadAssoc = true;
		}
	}
	// Before processing, check that there are enough input samples and space in output buffer
	if(processingControl.inProc.params_fixed_runtime.chanshw_in > 0)
	{
		jvxInt32 numLoc = numIn;
		for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
		{
			resL = proc_fields.circularBuffersIn[i]->elementsInBuffer(&numLoc);
			numIn = JVX_MIN(numIn, numLoc);
		}
	}
	if(processingControl.inProc.params_fixed_runtime.chanshw_out > 0)
	{
		jvxInt32 spaceLoc = spaceOut;
		for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
		{
			resL = proc_fields.circularBuffersOut[i]->spaceInBuffer(&spaceLoc);
			spaceOut = JVX_MIN(spaceOut, spaceLoc);
		}
	}
	if((numIn < processingControl.computedParameters.bSize_sw) || (spaceOut < processingControl.computedParameters.bSize_sw))
	{
#if defined( JVX_ASYNC_BUFFER_DEBUG_CNT)
		tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.dbg_tstamp);
		JVX_LOCK_MUTEX(runtime.dbg_safeAccess);
		runtime.dbg_events[runtime.dbg_events_cnt].tStamp = tt;
		runtime.dbg_events[runtime.dbg_events_cnt].event = 4;
		runtime.dbg_events_cnt = (runtime.dbg_events_cnt + 1) % JVX_ASYNC_BUFFER_DEBUG_CNT;
		JVX_UNLOCK_MUTEX(runtime.dbg_safeAccess);
#endif
		// Nothing to do here..
		res = JVX_NO_ERROR;
	}
	else
	{
		resL = process_start_icon_x(0, NULL, 0); // regular pipeline behavior
		if (resL_start != JVX_NO_ERROR)
		{
			std::cout << __FUNCTION__ << "Process start function failed, error reason: " << jvxErrorType_descr(resL_start) << "." << std::endl;
		}


		jvxSize idxBufferIn = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
		jvxSize idxBufferOut = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;

#ifdef VERBOSE_BUFFERING_REPORT
		std::cout << "async processing side to queue, map synchronizer to bidx " << idxBufferIn << "/"
			<< _common_set_ldslave.theData_out.con_data.number_buffers << std::endl;
		std::cout << "async processing side to queue, map bidx  " << idxBufferOut << "/"
			<< _common_set_ldslave.theData_in->con_data.number_buffers << " to synchronizer" << std::endl;
#endif
		if(processingControl.inProc.timeStamp_prev_sw > 0)
		{
			jvxData dd = (jvxData)(timeStart - processingControl.inProc.timeStamp_prev_sw);
			dd /= processingControl.computedParameters.bSize_sw;
			processingControl.inProc.deltaT_sw_avrg = processingControl.inProc.deltaT_sw_avrg * (JVX_GW_ALPHA_SMOOTH_DELTAT) + dd * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);
			genGenericWrapper_device::properties_active_higher.current_rate_sw.value = 1e6 /processingControl.inProc.deltaT_sw_avrg;// delta T in us
		}
		processingControl.inProc.timeStamp_prev_sw = timeStart;

		for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
		{
			jvxTick timeStart_loc = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);

			if(onInit.theFilesRef->theInputFiles[i].common.processing.selected)
			{
				fileInvolved_in = true;

				// ---------------------------
				if(onInit.theFilesRef->theInputFiles[i].flag_bwd)
				{
					onInit.theFilesRef->theInputFiles[i].flag_bwd = 0;
					onInit.theFilesRef->theInputFiles[i].theReader->wind_backward(
						onInit.theFilesRef->theInputFiles[i].common.samplerate * JVX_GW_WIND_SECONDS);
				}
				if (onInit.theFilesRef->theInputFiles[i].flag_fwd)
				{
					onInit.theFilesRef->theInputFiles[i].flag_fwd = 0;
					onInit.theFilesRef->theInputFiles[i].theReader->wind_forward(
						onInit.theFilesRef->theInputFiles[i].common.samplerate * JVX_GW_WIND_SECONDS);
				}
				if (onInit.theFilesRef->theInputFiles[i].flag_restart)
				{
					onInit.theFilesRef->theInputFiles[i].flag_restart = 0;
					onInit.theFilesRef->theInputFiles[i].theReader->restart(0);
					if (onInit.theFilesRef->theInputFiles[i].common.pauseOnStart)
					{
						onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS_PAUSED;
					}
				}
				if (onInit.theFilesRef->theInputFiles[i].flag_pause)
				{
					onInit.theFilesRef->theInputFiles[i].flag_pause = 0;
					onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS_PAUSED;
				}
				if (onInit.theFilesRef->theInputFiles[i].flag_play)
				{
					onInit.theFilesRef->theInputFiles[i].flag_play = 0;
					onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS;
				}

				// ---------------------------

				switch(onInit.theFilesRef->theInputFiles[i].common.processing.theStatus)
				{
				case FILE_ACCESS:

					fileRunning = true;
					if(onInit.theFilesRef->theInputFiles[i].common.processing.resampling)
					{

						resL = onInit.theFilesRef->theInputFiles[i].theReader->get_audio_buffer(
							reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS),
							onInit.theFilesRef->theInputFiles[i].common.number_channels,
							onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs,
							false, &onInit.theFilesRef->theInputFiles[i].progress);

						for(j = 0; j < (jvxSize)onInit.theFilesRef->theInputFiles[i].common.number_channels; j++)
						{
							onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theHdl->process(
								onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS[j],
								onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[idxBufferIn][j]);
						}
					}
					else
					{
						resL = onInit.theFilesRef->theInputFiles[i].theReader->get_audio_buffer(
							reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[idxBufferIn]),
							onInit.theFilesRef->theInputFiles[i].common.number_channels,
							processingControl.inProc.params_fixed_runtime.buffersize,
							false, &onInit.theFilesRef->theInputFiles[i].progress);
					}
					if(resL != JVX_NO_ERROR)
					{
						switch(resL)
						{
						case JVX_ERROR_BUFFER_UNDERRUN:
							// Do kind of a report....
							onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS_DROPOUT;
							_report_text_message(("Buffer underrun in file " + onInit.theFilesRef->theInputFiles[i].common.name).c_str(), JVX_REPORT_PRIORITY_WARNING);
							onInit.theFilesRef->theInputFiles[i].common.processing.lostFrames = 1;

							break;
						default:
							onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS_COMPLETED;
						}
					}
					break;

				case FILE_ACCESS_DROPOUT:

					/* We need this state to avoid endless message postings */
					fileRunning = true;
					if(onInit.theFilesRef->theInputFiles[i].common.processing.resampling)
					{

						resL = onInit.theFilesRef->theInputFiles[i].theReader->get_audio_buffer(
							reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS),
							onInit.theFilesRef->theInputFiles[i].common.number_channels,
							onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs,
							false, &onInit.theFilesRef->theInputFiles[i].progress);

						for(j = 0; j < (jvxSize)onInit.theFilesRef->theInputFiles[i].common.number_channels; j++)
						{
							onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theHdl->process(
								onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS[j],
								onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[idxBufferIn][j]);
						}
					}
					else
					{
						resL = onInit.theFilesRef->theInputFiles[i].theReader->get_audio_buffer(
							reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[idxBufferIn]),
							onInit.theFilesRef->theInputFiles[i].common.number_channels,
							onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs,
							false, &onInit.theFilesRef->theInputFiles[i].progress);
					}
					if(resL != JVX_NO_ERROR)
					{
						switch(resL)
						{
						case JVX_ERROR_BUFFER_UNDERRUN:
							onInit.theFilesRef->theInputFiles[i].common.processing.lostFrames++;
							break;
						default:
							onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS_COMPLETED;
						}
					}
					else
					{
						_report_text_message(("Lost " + jvx_size2String(onInit.theFilesRef->theInputFiles[i].common.processing.lostFrames) + " frames in file " + onInit.theFilesRef->theInputFiles[i].common.name).c_str(), JVX_REPORT_PRIORITY_WARNING);
						onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS;
					}
					break;
				case FILE_ACCESS_PAUSED:
					fileRunning = true;

				case FILE_ACCESS_COMPLETED:
					for(j = 0; j < (jvxSize)onInit.theFilesRef->theInputFiles[i].common.number_channels; j++)
					{
						memset(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[
							*_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr][j], 0, proc_fields.szElementSW * processingControl.computedParameters.bSize_sw);
					}
					break;
				}
			}
			jvxTick timeStop_loc = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);
			jvxTick deltaT_loc = timeStop_loc - timeStart_loc;

			onInit.theFilesRef->theInputFiles[i].common.processing.deltaT_avrg = onInit.theFilesRef->theInputFiles[i].common.processing.deltaT_avrg* (JVX_GW_ALPHA_SMOOTH_DELTAT) + deltaT_loc * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);
		}

		for(i = 0; i < onInit.theExtRef->theInputReferences.size(); i++)
		{

			//jvxInt64 timeStart_loc = JVX_GET_TICKCOUNT_US_GET(processingControl.inProc.tStampRef);

			if(onInit.theExtRef->theInputReferences[i].processing.selected)
			{
				fileInvolved_in = true;
				switch(onInit.theExtRef->theInputReferences[i].processing.theStatus)
				{
				case FILE_ACCESS:

					if(onInit.theExtRef->theInputReferences[i].processing.resampling)
					{
						resL = onInit.theExtRef->theInputReferences[i].theCallback->get_one_frame(
							onInit.theExtRef->theInputReferences[i].uniqueIdHandles,
							reinterpret_cast<jvxHandle**>(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS));

						for(j = 0; j < (jvxSize)onInit.theExtRef->theInputReferences[i].number_channels; j++)
						{
							onInit.theExtRef->theInputReferences[i].processing.resampler[j].theHdl->process(
								onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS[j],
								onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW[idxBufferIn][j]);
						}
					}
					else
					{
						resL = onInit.theExtRef->theInputReferences[i].theCallback->get_one_frame(
							onInit.theExtRef->theInputReferences[i].uniqueIdHandles,
							reinterpret_cast<jvxHandle**>(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW[idxBufferIn]));
					}
					if(resL == JVX_NO_ERROR)
					{
						fileRunning = true;
					}
					else
					{
						if(resL == JVX_ERROR_END_OF_FILE)
						{
							onInit.theExtRef->theInputReferences[i].processing.theStatus = FILE_ACCESS_COMPLETED;
						}
						else
						{
							fileRunning = true;
							onInit.theExtRef->theInputReferences[i].processing.numberLostFrames++;
						}
					}
					break;

				default:
					for(j = 0; j < (jvxSize)onInit.theExtRef->theInputReferences[i].number_channels; j++)
					{
						memset(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW[idxBufferIn][j], 0, proc_fields.szElementSW * processingControl.computedParameters.bSize_sw);
					}
					break;
				}
			}
			/*
			jvxInt64 timeStop_loc = JVX_GET_TICKCOUNT_US_GET(processingControl.inProc.tStampRef);
			jvxInt64 deltaT_loc = timeStop_loc - timeStart_loc;

			onInit.theFilesRef->theInputFiles[i].common.processing.deltaT_avrg = onInit.theFilesRef->theInputFiles[i].common.processing.deltaT_avrg* (JVX_GW_ALPHA_SMOOTH_DELTAT) + deltaT_loc * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);
			*/
		}

		// Preset dummy buffers to zero
		for (i = 0; i < onInit.theDummyRef->inputs.size(); i++)
		{
			if (onInit.theDummyRef->inputs[i].wasStarted)
			{
				memset(onInit.theDummyRef->inputs[i].buf[idxBufferIn], 0, proc_fields.szElementSW * processingControl.computedParameters.bSize_sw);
			}
		}

		/*
		for (i = 0; i < onInit.theDummyRef->outputs.size(); i++)
		{
			if (onInit.theDummyRef->outputs[i].wasStarted)
			{
				memset(onInit.theDummyRef->outputs[i].buf[_common_set_ldslave.theData_out.pipeline.idx_stage], 0, proc_fields.szElementSW * processingControl.computedParameters.bSize_sw);
			}
		}
		*/

		// Stop if file has been completed!
		if(fileInvolved_in && !fileRunning) 
		{
			triggerEnd = true;
		}


		// ==================================================================================
		// ==================================================================================

		switch(proc_fields.seq_operation_in)
		{
		case TB_PROC:
		case RES_TB_PROC:
		case DACON_TB_PROC:
		case RES_DACON_TB_PROC:

			for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				//idxOut = proc_fields.mapToOutput_in[i];
				jvxSize idxIn = runtime.channelMappings.inputChannelMapper_hwrev[i];
				resL = proc_fields.circularBuffersIn[i]->read(reinterpret_cast<jvxByte*>(
					_common_set_ldslave.theData_out.con_data.buffers[idxBufferIn][idxIn]),
					processingControl.computedParameters.bSize_sw);
				if(resL != JVX_NO_ERROR)
				{
					_report_text_message("Circular buffer read failed in processing thread.", JVX_REPORT_PRIORITY_WARNING);
				}
			}
			break;
		case NOTHING:

			break;
		default:
			// Not yet implemented
			assert(0);
			break;
		}

		if(triggerEnd)
		{
			if((proc_fields.seq_operation_out == NOTHING) && (proc_fields.seq_operation_in == NOTHING))
			{
				// If HW is not involved and we process from file/buffer to file/buffer, no more output should be produced!
				noInputOutput = true;
			}
		}
		// No input available is only possible in mode without any HW involved. In that case we do not produce any output for no input
		if(!noInputOutput)
		{
			if(JVX_EVALUATE_BITFIELD(properties_active_higher.levelHandlingInput.value.selection() & 0x1))
			{
				assert(processingControl.inProc.params_fixed_runtime.chans_in == _common_set_ldslave.theData_out.con_params.number_channels);
				jvx_fieldLevelGainClip(
					_common_set_ldslave.theData_out.con_data.buffers[idxBufferIn],
					_common_set_ldslave.theData_out.con_params.number_channels,
					_common_set_ldslave.theData_out.con_params.buffersize,
					_common_set_ldslave.theData_out.con_params.format,
					processingControl.levels.input_avrg,
					processingControl.levels.input_max,
					processingControl.levels.input_gain,
					processingControl.levels.input_clip,
					processingControl.levels.input_channel_map,
					CLIP_VALUE, SMOOTH_LEVELS);
			}

			timeStart = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);
			// =======================================================================

			// We will be able to do this in a better way...
			_common_set_ldslave.theData_out.admin.tStamp_us = 0;
			_common_set_ldslave.theData_out.admin.frames_lost_since_last_time = 0;
			
			// =============================================================================
			// Main processing
			// =============================================================================
			resL_proc = process_buffers_icon_x(JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
			if (resL_proc != JVX_NO_ERROR)
			{
				std::cout << __FUNCTION__ << "Process buffer function failed, error reason: " << jvxErrorType_descr(resL_proc) << "." << std::endl;
			}


			// =============================================================================
			// =============================================================================

			jvxTick timeStop = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);
			jvxInt32 modFHeight = processingControl.deltaWrite;
			modFHeight = JVX_MAX(-processingControl.computedParameters.bSize_sw, modFHeight);

			// Mix in input signal for direct listening
			for(i = 0; i < _common_set_ldslave.theData_out.con_params.number_channels; i++)
			{
				if(processingControl.levels.direct[i] == 1)
				{
					for(k = 0; k < processingControl.levels.num_entries_output; k++)
					{
						jvx_fieldMix(_common_set_ldslave.theData_out.con_data.buffers[idxBufferIn][i],
							_common_set_ldslave.theData_in->con_data.buffers[idxBufferOut][k],
							_common_set_ldslave.theData_in->con_params.buffersize, _common_set_ldslave.theData_in->con_params.format);
					}
				}
			}

			if(JVX_EVALUATE_BITFIELD(properties_active_higher.levelHandlingOutput.value.selection() & 0x1))
			{
				assert(processingControl.inProc.params_fixed_runtime.chans_out == _common_set_ldslave.theData_in->con_params.number_channels);
				jvx_fieldLevelGainClip(
					_common_set_ldslave.theData_in->con_data.buffers[idxBufferOut],
					_common_set_ldslave.theData_in->con_params.number_channels,
					_common_set_ldslave.theData_in->con_params.buffersize,
					_common_set_ldslave.theData_in->con_params.format,
					processingControl.levels.output_avrg,
					processingControl.levels.output_max,
					processingControl.levels.output_gain,
					processingControl.levels.output_clip,
					processingControl.levels.output_channel_map,
					CLIP_VALUE, SMOOTH_LEVELS);
			}


			// =======================================================================
			// Output side
			// =======================================================================
			switch(proc_fields.seq_operation_out)
			{
			case TB_PROC:
			case RES_TB_PROC:
			case DACON_TB_PROC:
			case RES_DACON_TB_PROC:

				for(i = 0; i < (jvxSize)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
				{
					jvxSize idxOut = runtime.channelMappings.outputChannelMapper_hwrev[i];
					assert(JVX_CHECK_SIZE_SELECTED(idxOut));
					resL = proc_fields.circularBuffersOut[i]->write(
						(const char*)_common_set_ldslave.theData_in->con_data.buffers[idxBufferOut][idxOut],
						//proc_fields.ptrA_out_net[_common_set_ldslave.theData_in->pipeline.idx_stage][i], 
						processingControl.computedParameters.bSize_sw, modFHeight);
					if(resL != JVX_NO_ERROR)
					{
						_report_text_message("Circular buffer write failed in processing thread.", JVX_REPORT_PRIORITY_WARNING);
					}

				}
				break;

			case NOTHING:

				break;
			default:
				// Not yet implemented
				assert(0);
				break;
			}
			processingControl.deltaWrite = 0;

			jvxTick deltaT = timeStop - timeStart;

			processingControl.measureLoad.deltaT_avrg = processingControl.measureLoad.deltaT_avrg  * (JVX_GW_ALPHA_SMOOTH_DELTAT) + deltaT * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);
			if(deltaT > processingControl.measureLoad.deltaT_max)
			{
				processingControl.measureLoad.deltaT_max  = JVX_INT64_DATA(deltaT);
			}
			else
			{
				processingControl.measureLoad.deltaT_max *= JVX_GW_ALPHA_RELEASE_DELTAT_MAX;
			}

			genGenericWrapper_device::properties_active_higher.loadpercent.value = processingControl.measureLoad.deltaT_avrg / processingControl.measureLoad.deltaT_theory * 100.0;

			for(i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
			{
				jvxTick timeStart_loc = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);
				if(onInit.theFilesRef->theOutputFiles[i].common.processing.selected)
				{
					fileInvolved_out = true;
					switch(onInit.theFilesRef->theOutputFiles[i].common.processing.theStatus)
					{
					case FILE_ACCESS:
						fileRunning = true;
						if(onInit.theFilesRef->theOutputFiles[i].common.processing.resampling)
						{
							for(j = 0; j < (jvxSize)onInit.theFilesRef->theOutputFiles[i].common.number_channels; j++)
							{
								onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theHdl->process(
									onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[idxBufferOut][j],
									onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS[j]);
							}

							resL = onInit.theFilesRef->theOutputFiles[i].theWriter->add_audio_buffer(
								reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS),
								onInit.theFilesRef->theOutputFiles[i].common.number_channels,
								onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs,
								false);

						}
						else
						{
							resL = onInit.theFilesRef->theOutputFiles[i].theWriter->add_audio_buffer(
								reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[idxBufferOut]),
								onInit.theFilesRef->theOutputFiles[i].common.number_channels,
								processingControl.inProc.params_fixed_runtime.buffersize,
								false);
						}
						if(resL != JVX_NO_ERROR)
						{
							switch(resL)
							{
							case JVX_ERROR_BUFFER_OVERFLOW:
								// Do kind of a report....
								onInit.theFilesRef->theOutputFiles[i].common.processing.theStatus = FILE_ACCESS_DROPOUT;
								_report_text_message(("Buffer overrun in file " + onInit.theFilesRef->theOutputFiles[i].common.name).c_str(), JVX_REPORT_PRIORITY_WARNING);
								onInit.theFilesRef->theOutputFiles[i].common.processing.lostFrames = 1;

								break;
							case JVX_ERROR_END_OF_FILE:
								
								onInit.theFilesRef->theOutputFiles[i].common.processing.theStatus = FILE_ACCESS_COMPLETED;
								_report_text_message(("End of file in file " + onInit.theFilesRef->theOutputFiles[i].common.name).c_str(), 
									JVX_REPORT_PRIORITY_WARNING);
								onInit.theFilesRef->theOutputFiles[i].common.processing.lostFrames = 1;
								triggerEnd = true;
								break;
							default:

								assert(0);
							}
						}
						break;
					case FILE_ACCESS_DROPOUT:

						if(onInit.theFilesRef->theOutputFiles[i].common.processing.resampling)
						{
							for(j = 0; j < (jvxSize)onInit.theFilesRef->theOutputFiles[i].common.number_channels; j++)
							{
								onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theHdl->process(
									onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[idxBufferOut][j],
									onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS[j]);
							}

							resL = onInit.theFilesRef->theOutputFiles[i].theWriter->add_audio_buffer(
								reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS),
								onInit.theFilesRef->theOutputFiles[i].common.number_channels,
								onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs,
								false);

						}
						else
						{
							resL = onInit.theFilesRef->theOutputFiles[i].theWriter->add_audio_buffer(
								reinterpret_cast<jvxHandle**>(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[idxBufferOut]),
								onInit.theFilesRef->theOutputFiles[i].common.number_channels,
								onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs,
								false);
						}
						if(resL != JVX_NO_ERROR)
						{
							switch(resL)
							{
							case JVX_ERROR_BUFFER_OVERFLOW:
							case JVX_ERROR_POSTPONE:
							case JVX_ERROR_END_OF_FILE:
								onInit.theFilesRef->theOutputFiles[i].common.processing.lostFrames++;
								break;
							default:
								assert(0);
							}
						}
						else
						{
							_report_text_message(("Lost " + jvx_size2String(onInit.theFilesRef->theOutputFiles[i].common.processing.lostFrames) + " frames in file " + onInit.theFilesRef->theOutputFiles[i].common.name).c_str(), JVX_REPORT_PRIORITY_WARNING);
							onInit.theFilesRef->theOutputFiles[i].common.processing.theStatus = FILE_ACCESS;
						}
						break;
					}
				}
				jvxTick timeStop_loc = JVX_GET_TICKCOUNT_US_GET_CURRENT(&processingControl.inProc.tStampRef);
				jvxTick deltaT_loc = timeStop_loc - timeStart_loc;

				onInit.theFilesRef->theOutputFiles[i].common.processing.deltaT_avrg = onInit.theFilesRef->theOutputFiles[i].common.processing.deltaT_avrg* (JVX_GW_ALPHA_SMOOTH_DELTAT) + deltaT_loc * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);
			}

			for(i = 0; i < onInit.theExtRef->theOutputReferences.size(); i++)
			{

				//jvxInt64 timeStart_loc = JVX_GET_TICKCOUNT_US_GET(processingControl.inProc.tStampRef);

				if(onInit.theExtRef->theOutputReferences[i].processing.selected)
				{
					fileInvolved_out = true;
					switch(onInit.theExtRef->theOutputReferences[i].processing.theStatus)
					{
					case FILE_ACCESS:

						if (onInit.theExtRef->theOutputReferences[i].processing.resampling)
						{
							for (j = 0; j < (jvxSize)onInit.theExtRef->theOutputReferences[i].number_channels; j++)
							{
								onInit.theExtRef->theOutputReferences[i].processing.resampler[j].theHdl->process(
									onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW[idxBufferOut][j],
									onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS[j]);
							}
						
							resL = onInit.theExtRef->theOutputReferences[i].theCallback->put_one_frame(
								onInit.theExtRef->theOutputReferences[i].uniqueIdHandles,
								reinterpret_cast<jvxHandle**>(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesRS));

						}
						else
						{
							resL = onInit.theExtRef->theOutputReferences[i].theCallback->put_one_frame(
								onInit.theExtRef->theOutputReferences[i].uniqueIdHandles,
								reinterpret_cast<jvxHandle**>(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW[idxBufferOut]));
						}
						if(resL != JVX_NO_ERROR)
						{
							onInit.theExtRef->theOutputReferences[i].processing.numberLostFrames++;
						}
						break;

					default:
						for(j = 0; j < (jvxSize)onInit.theExtRef->theOutputReferences[i].number_channels; j++)
						{
							memset(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW[idxBufferOut][j], 0, proc_fields.szElementSW * processingControl.computedParameters.bSize_sw);
						}
						break;
					}
				}
				/*
				jvxInt64 timeStop_loc = JVX_GET_TICKCOUNT_US_GET(processingControl.inProc.tStampRef);
				jvxInt64 deltaT_loc = timeStop_loc - timeStart_loc;

				onInit.theFilesRef->theInputFiles[i].common.processing.deltaT_avrg = onInit.theFilesRef->theInputFiles[i].common.processing.deltaT_avrg* (JVX_GW_ALPHA_SMOOTH_DELTAT) + deltaT_loc * (1 - JVX_GW_ALPHA_SMOOTH_DELTAT);
				*/
			}
		}

		resL_stop = process_stop_icon_x(JVX_SIZE_UNSELECTED, true, 0, NULL, NULL);
		if (resL_stop != JVX_NO_ERROR)
		{
			std::cout << __FUNCTION__ << "Process stop function failed, error reason: " << jvxErrorType_descr(resL_stop) << "." << std::endl;
		}
		if ((resL_stop != JVX_NO_ERROR) || (resL_start != JVX_NO_ERROR) || (resL_proc != JVX_NO_ERROR))
		{
			triggerEnd = true;
		}

		if(
			(!fileInvolved_in) && (!fileInvolved_out) && (proc_fields.seq_operation_in == NOTHING) && (proc_fields.seq_operation_out == NOTHING))
		{
			triggerEnd = true;
		}


		if(triggerEnd)
		{
			// Send end signal only once
			if(proc_fields.threads.endTriggered == false)
			{
				_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_PROCESS_SHIFT);
				proc_fields.threads.endTriggered = true;
			}
		}
		res = JVX_ERROR_THREAD_ONCE_MORE;
	}

#if defined( JVX_ASYNC_BUFFER_DEBUG_CNT)
	tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.dbg_tstamp);
	JVX_LOCK_MUTEX(runtime.dbg_safeAccess);
	runtime.dbg_events[runtime.dbg_events_cnt].tStamp = tt;
	runtime.dbg_events[runtime.dbg_events_cnt].event = 5;
	runtime.dbg_events_cnt = (runtime.dbg_events_cnt + 1) % JVX_ASYNC_BUFFER_DEBUG_CNT;
	JVX_UNLOCK_MUTEX(runtime.dbg_safeAccess);
#endif
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (onInit.connectedDevice)
	{
		switch (proc_fields.doSelection)
		{
		case CjvxGenericWrapperDevice::SELECTION_ASYNC_MODE:

			// The processing chain ends here
			break;

		case CjvxGenericWrapperDevice::SELECTION_SYNC_MODE:
			theRelocator.process_buffers_icon_x(mt_mask, idx_stage);
			break;
		}
	}
	else
	{
		// Do nothing..
	}
	return res;
#if 0
	res = this->process_st(_common_set_ldslave.theData_in, 
		_common_set_ldslave.theData_in->pipeline.idx_stage, 
		_common_set_ldslave.theData_in->con_compat.idx_receiver_to_sender);

	// Take over the buffer index
	/*
		theRelocator._common_set_ldslave.theData_in->con_compat.idx_receiver_to_sender =
		processingControl.inProc.theData_out_to_device.pipeline.idx_stage;
		*/

	// If there is no terminal data, then there is no device involved
	assert(0);
#endif
#if 0
	if (theData == theRelocator._common_set_ldslave.theData_in)
	{
		res = this->process_st(theData, theData->pipeline.idx_stage, theData->con_compat.idx_receiver_to_sender);

		// Take over the buffer index
		theRelocator._common_set_ldslave.theData_in->con_compat.idx_receiver_to_sender =
			processingControl.inProc.theData_out_to_device.pipeline.idx_stage;
	}
	else
	{
		if (proc_fields.doSelection == SELECTION_SYNC_MODE)
		{
			// Only if we are in sync mode we will pass the processed data to the device.
			// In async mode, we have to stop before since the other thread will reconnect to 
			// device (other case up)
			if (theRelocator._common_set_ldslave.theData_in->link.next)
			{
				// Output to the initial device
				processingControl.inProc.theData_out_to_device.admin = theData->admin;
				res = theRelocator._common_set_ldslave.theData_in->link.next->process_buffers_icon(
					&processingControl.inProc.theData_out_to_device);
			}
		}
	}
#endif
	return res;
};

jvxErrorType
CjvxGenericWrapperDevice::process_start_icon(
	jvxSize pipeline_offset, 
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool acquire = false;

	// This function may only be called if we have a hardware involved
	// We end up here on the processing chain related to the involved device
	if (onInit.connectedDevice)
	{
		switch (proc_fields.doSelection)
		{
		case CjvxGenericWrapperDevice::SELECTION_ASYNC_MODE:

			//JVX_SHIFT_BUFFER_PIPELINE_IDX_ON_START;
			break;

		case CjvxGenericWrapperDevice::SELECTION_SYNC_MODE:
			res = theRelocator.process_start_icon_x(pipeline_offset, idx_stage, 0);

			if (proc_fields.seq_operation_out == CjvxGenericWrapperDevice::PROC)
			{
				// The right position in the pipeline reservation buffer is alreadey set anyway
				/*
				_common_set_ldslave.theData_in->con_pipeline.do_lock(_common_set_ldslave.theData_in->con_pipeline.lock_hdl); 
				_common_set_ldslave.theData_in->pipeline.idx_stage =
					theRelocator._common_set_ldslave.theData_out.pipeline.idx_stage;
				_common_set_ldslave.theData_in->con_pipeline.do_unlock(_common_set_ldslave.theData_in->con_pipeline.lock_hdl);
				*/
			}
			else
			{
				res = shift_buffer_pipeline_idx_on_start(pipeline_offset, idx_stage,
					tobeAccessedByStage,
					clbk,
					priv_ptr);
			}
			break;
		case  CjvxGenericWrapperDevice::SELECTION_FILEIO_MODE:
			res = shift_buffer_pipeline_idx_on_start(pipeline_offset, idx_stage,
				tobeAccessedByStage, clbk, priv_ptr);
			break;

		default:
			assert(0);
		}
	}
	else
	{
		res = shift_buffer_pipeline_idx_on_start(pipeline_offset, idx_stage, 0, clbk, priv_ptr);
	}
	return res;
#if 0
	// This is a shortcut: go to the device directly. All 
	// subnodes will be triggered from within the internal process function
	if (_common_set_ldslave.theData_out.con_link.connect_to)
	{
		_common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
		// This gives us the output buffer index in the device in _common_set_ldslave.theData_Out
		// NO LONGER NEEDED: if (proc_fields.seq_operation_out == PROC)
		// NO LONGER NEEDED: {
		// NO LONGER NEEDED: _common_set_ldslave.theData_in->pipeline.idx_stage = theRelocator._common_set_ldslave.theData_out.pipeline.idx_stage;
		// NO LONGER NEEDED: }
		// NO LONGER NEEDED: else
		// NO LONGER NEEDED: {
		// NO LONGER NEEDED: // In all other cases, the output is only a single buffer
		// NO LONGER NEEDED: _common_set_ldslave.theData_in->pipeline.idx_stage = 0;
		// NO LONGER NEEDED: }
	}
#endif

#if 0
	// If there is no terminal data, then there is no device involved
	if (theData == theRelocator._common_set_ldslave.theData_in)
	{
		// Contact through to queue
		_common_set_audio_device.link_audio_node->process_start_icon(&_common_set_ldslave.theData_out);

		// When returning from queue, prepare the buffer processing index
		// This is for the device if using the old style processing method
		theData->con_compat.idx_receiver_to_sender = processingControl.inProc.theData_out_to_device.pipeline.idx_stage;
	}
	else
	{
		if (theRelocator._common_set_ldslave.theData_in->link.next)
		{
			// Output to the initial device
			res = theRelocator._common_set_ldslave.theData_in->link.next->process_start_icon(&processingControl.inProc.theData_out_to_device);

			if (proc_fields.seq_operation_out == PROC)
			{
				// If we use multiple buffers directly, we need to also adapt the buffer switch index
				// This is for the prpevios object to address the right input buffer index
				theData->pipeline.idx_stage = processingControl.inProc.theData_out_to_device.pipeline.idx_stage;
			}
			else
			{
				// In all other cases, the output is only a single buffer
				theData->pipeline.idx_stage = 0;
			}
		}
	}
#endif
};

jvxErrorType
CjvxGenericWrapperDevice::process_stop_icon(
	jvxSize idx_stage, 
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	// CjvxInputOutputConnector::_start_connect_icon(false); <- not needed

	// This function may only be called if we have a hardware involved
	// We end up here on the processing chain related to the involved device

	if (onInit.connectedDevice)
	{
		switch (proc_fields.doSelection)
		{
		case CjvxGenericWrapperDevice::SELECTION_ASYNC_MODE:

			// The processing chain ends here

			// Move buffer index forward
			res = shift_buffer_pipeline_idx_on_stop(idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);
			break;

		case CjvxGenericWrapperDevice::SELECTION_SYNC_MODE:
			res = theRelocator.process_stop_icon_x( idx_stage, shift_fwd,
				tobeAccessedByStage,
				clbk,
				 priv_ptr);

			if (proc_fields.seq_operation_out == PROC)
			{
				// Let us copy the output index from the folowing object to the local object			
				_common_set_ldslave.theData_in->con_pipeline.do_lock(_common_set_ldslave.theData_in->con_pipeline.lock_hdl);
				*_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr =
					*theRelocator._common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
				_common_set_ldslave.theData_in->con_pipeline.do_unlock(_common_set_ldslave.theData_in->con_pipeline.lock_hdl);
			}
			else
			{
				res = shift_buffer_pipeline_idx_on_stop( idx_stage,  shift_fwd, tobeAccessedByStage,
					clbk, priv_ptr);
			}
			break;
		case CjvxGenericWrapperDevice::SELECTION_FILEIO_MODE:
			// Here, we are the endpoint. This will move the output bufer pointer forward
			res = shift_buffer_pipeline_idx_on_stop( idx_stage,  shift_fwd,
				tobeAccessedByStage,
				clbk,
				priv_ptr);
			break;

		default:
			assert(0);
		}
	}
	else
	{
		// Here, we are the endpoint. This will move the output bufer pointer forward
		res = shift_buffer_pipeline_idx_on_stop( idx_stage, shift_fwd,
			tobeAccessedByStage,
			clbk,
			priv_ptr);
	}

#if 0
	// This is a shortcut: go to the device directly. All 
	// subnodes will be triggered from within the internal process function
	if (_common_set_ldslave.theData_out.con_link.connect_to)
	{
		_common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
	}
#endif
	return res;
};
