#include "CjvxGenericWrapperDevice.h"
#include "CjvxGenericWrapperTechnology.h"
#include "CjvxGenericWrapper_common.h"
#include "jvx_dsp_base.h"

jvxErrorType
CjvxGenericWrapperDevice::prepare()
{
	jvxSize i;
	jvxErrorType res = JVX_MY_BASE_CLASS_D::_prepare_lock();
	jvxErrorType resL = JVX_NO_ERROR;

	if(res == JVX_NO_ERROR)
	{
		//jvxSize i;
		//bufferField = NULL;

		CjvxProperties::_lock_properties_local();

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genGenericWrapper_device::properties_active.nonBlockingAsync);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genGenericWrapper_device::properties_active.setupAsync);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genGenericWrapper_device::properties_active.setupResampler);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genGenericWrapper_device::properties_active.qualityResamplerIn);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genGenericWrapper_device::properties_active.qualityResamplerOut);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genGenericWrapper_device::properties_active.controlThreads);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genGenericWrapper_device::properties_active.autoUseHwParams);

		CjvxProperties::_unlock_properties_local();

		// =====================================================================
		// Start allocation state machine
		// =====================================================================

		processingControl.stateInPreparation = 0;
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_PREPARE_STATUS_INIT_BF;
		
		// =====================================================================
		// Pass the current selection of channels to wrapped hardware
		// =====================================================================

		pass_channelsetup_nolock();

		// Update active channel mappings
		this->updateChannelMapper_ActiveSelection_noLock();

		// Update buffersizes and modes for the last time before start
		/*
		this->updateSWSamplerateAndBuffersize_nolock();
		*/

		// =====================================================================
		// Start thread controller if desired
		// =====================================================================

		if (runtime.threadCtrl.coreHdl.hdl)
		{
			runtime.threadCtrl.asyncAudioThreadAssoc = false;
			if (resL == JVX_NO_ERROR)
			{
				if (processingControl.computedParameters.timebasechange_required)
				{
					resL = runtime.threadCtrl.coreHdl.hdl->register_tc_thread(&runtime.threadCtrl.asyncAudioThreadId);
					if (resL != JVX_NO_ERROR)
					{
						this->_report_text_message("Failed to control async threads, no thread control used.", JVX_REPORT_PRIORITY_WARNING);
					}
				}
			}
			else
			{
				this->_report_text_message("Failed to control sync threads, no thread control used.", JVX_REPORT_PRIORITY_WARNING);
			}
		}

		// =====================================================================
		// Some performance variables
		// =====================================================================

		processingControl.measureLoad.deltaT_theory = (jvxData)processingControl.computedParameters.bSize_sw / processingControl.computedParameters.sRate_sw_real * 1000000; // in us
		processingControl.measureLoad.deltaT_max = 0;
		processingControl.measureLoad.deltaT_avrg = 0;

		processingControl.inProc.timeStamp_prev_hw = -1;
		processingControl.inProc.timeStamp_prev_sw = -1;
		processingControl.inProc.deltaT_hw_avrg = 1e6 / processingControl.computedParameters.sRate_hw;// delta T in us
		processingControl.inProc.deltaT_sw_avrg = 1e6 / processingControl.computedParameters.sRate_sw_real;// delta T in us
		genGenericWrapper_device::properties_active_higher.current_rate_hw.value = processingControl.computedParameters.sRate_hw;
		genGenericWrapper_device::properties_active_higher.current_rate_sw.value = processingControl.computedParameters.sRate_sw_real;

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// Final count of number of channels
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		
		// ==========================================================================
		// We evaluate the parameters but will keep them until processing is done!!!
		// The variable <processingControl.inProc.params_fixed_runtime> is set here and will be
		// unchanged while processing is active! It will be release in the call to postprocess!!
		// ==========================================================================
		
		populate_countchannels_datatype(processingControl.inProc.params_fixed_runtime);
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_CHANNELS_COUNT_COMPLETE_BF;

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// Associate levels etc..
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		JVX_DSP_SAFE_ALLOCATE_FIELD(this->processingControl.levels.input_channel_map, jvxSize,
			processingControl.inProc.params_fixed_runtime.chans_in);
		for (i = 0; i < processingControl.inProc.params_fixed_runtime.chans_in; i++)
		{
			this->processingControl.levels.input_channel_map[i] = JVX_SIZE_UNSELECTED;
		}

		this->processingControl.levels.num_entries_input = runtime.channelMappings.inputChannelMapper.size();
		
		if (this->processingControl.levels.num_entries_input > 0)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD(this->processingControl.levels.input_avrg, jvxData, 
				this->processingControl.levels.num_entries_input);
			JVX_DSP_SAFE_ALLOCATE_FIELD(this->processingControl.levels.input_max, jvxData, 
				this->processingControl.levels.num_entries_input);
			JVX_DSP_SAFE_ALLOCATE_FIELD(this->processingControl.levels.input_gain, jvxData, 
				this->processingControl.levels.num_entries_input);
			JVX_DSP_SAFE_ALLOCATE_FIELD(this->processingControl.levels.input_clip, jvxCBool,
				this->processingControl.levels.num_entries_input);
			JVX_DSP_SAFE_ALLOCATE_FIELD(this->processingControl.levels.direct, jvxInt16,
				this->processingControl.levels.num_entries_input);
			
			jvxSize cnt = 0;
			for (i = 0; i < this->processingControl.levels.num_entries_input; i++)
			{
				this->processingControl.levels.input_avrg[i] = 0.0;
				this->processingControl.levels.input_max[i] = 0.0;
				this->processingControl.levels.input_gain[i] = runtime.channelMappings.inputChannelMapper[i].gain;
				this->processingControl.levels.input_clip[i] = false;
				this->processingControl.levels.direct[i] = runtime.channelMappings.inputChannelMapper[i].directIn;
				if (runtime.channelMappings.inputChannelMapper[i].isSelected)
				{
					this->processingControl.levels.input_channel_map[cnt] = i;
					cnt++;
				}
			}
			assert(cnt == processingControl.inProc.params_fixed_runtime.chans_in);

		}

		JVX_DSP_SAFE_ALLOCATE_FIELD(this->processingControl.levels.output_channel_map, jvxSize,
			processingControl.inProc.params_fixed_runtime.chans_out);
		for (i = 0; i < processingControl.inProc.params_fixed_runtime.chans_out; i++)
		{
			this->processingControl.levels.output_channel_map[i] = JVX_SIZE_UNSELECTED;
		}

		this->processingControl.levels.num_entries_output = runtime.channelMappings.outputChannelMapper.size();
		if (this->processingControl.levels.num_entries_output > 0)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD(this->processingControl.levels.output_avrg, jvxData, 
				this->processingControl.levels.num_entries_output);
			JVX_DSP_SAFE_ALLOCATE_FIELD(this->processingControl.levels.output_max, jvxData, 
				this->processingControl.levels.num_entries_output);
			JVX_DSP_SAFE_ALLOCATE_FIELD(this->processingControl.levels.output_gain, jvxData, 
				this->processingControl.levels.num_entries_output);
			JVX_DSP_SAFE_ALLOCATE_FIELD(this->processingControl.levels.output_clip, jvxCBool,
				this->processingControl.levels.num_entries_output);

			jvxSize cnt = 0;
			for (i = 0; i < this->processingControl.levels.num_entries_output; i++)
			{
				this->processingControl.levels.output_avrg[i] = 0.0;
				this->processingControl.levels.output_max[i] = 0.0;
				this->processingControl.levels.output_gain[i] = runtime.channelMappings.outputChannelMapper[i].gain;
				this->processingControl.levels.output_clip[i] = false;
				if (runtime.channelMappings.outputChannelMapper[i].isSelected)
				{
					this->processingControl.levels.output_channel_map[cnt] = i;
					cnt++;
				}
			}
			assert(cnt == processingControl.inProc.params_fixed_runtime.chans_out);
		}

		// Associate input/output levels, gains etc
		associate__properties_active_higher(
			static_cast<CjvxProperties*>(this),
			this->processingControl.levels.input_avrg, 
			this->processingControl.levels.num_entries_input,
			this->processingControl.levels.output_avrg, 
			this->processingControl.levels.num_entries_output,
			this->processingControl.levels.input_max, 
			this->processingControl.levels.num_entries_input,
			this->processingControl.levels.output_max, 
			this->processingControl.levels.num_entries_output);

		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_LEVELS_ASSOCIATED_BF;

		// =======================================================================
		// Start slave device if there is one
		// =======================================================================
		if (onInit.connectedDevice)
		{
			res = onInit.connectedDevice->prepare();
		}

		onInit.parentRef->on_device_caps_changed(static_cast<IjvxDevice*>(this));
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess()
{
	jvxErrorType res = JVX_MY_BASE_CLASS_D::_postprocess_lock();
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{
		// =======================================================================
		// Stop slave device if there is one
		// =======================================================================
		if (onInit.connectedDevice)
		{
			res = onInit.connectedDevice->postprocess();
		}

		if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_LEVELS_ASSOCIATED_BF)
		{
			processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_LEVELS_ASSOCIATED_BF;
			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			// Dessociate levels etc..
			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			if (this->processingControl.levels.num_entries_input > 0)
			{
				JVX_DSP_SAFE_DELETE_FIELD(this->processingControl.levels.input_avrg);
				JVX_DSP_SAFE_DELETE_FIELD(this->processingControl.levels.input_max);
				JVX_DSP_SAFE_DELETE_FIELD(this->processingControl.levels.input_gain);
				JVX_DSP_SAFE_DELETE_FIELD(this->processingControl.levels.input_clip);
				JVX_DSP_SAFE_DELETE_FIELD(this->processingControl.levels.direct);
			}
			if (this->processingControl.levels.num_entries_output > 0)
			{
				JVX_DSP_SAFE_DELETE_FIELD(this->processingControl.levels.output_avrg);
				JVX_DSP_SAFE_DELETE_FIELD(this->processingControl.levels.output_max);
				JVX_DSP_SAFE_DELETE_FIELD(this->processingControl.levels.output_gain);
				JVX_DSP_SAFE_DELETE_FIELD(this->processingControl.levels.output_clip);
			}

			// Associate input/output levels, gains etc
			deassociate__properties_active_higher(static_cast<CjvxProperties*>(this));

			if (processingControl.inProc.params_fixed_runtime.chans_in)
			{
				JVX_DSP_SAFE_DELETE_FIELD(this->processingControl.levels.input_channel_map);
			}
			if (processingControl.inProc.params_fixed_runtime.chans_out)
			{
				JVX_DSP_SAFE_DELETE_FIELD(this->processingControl.levels.output_channel_map);
			}
		}

		if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_CHANNELS_COUNT_COMPLETE_BF)
		{
			processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_CHANNELS_COUNT_COMPLETE_BF;

			// =================================================================================
			// Very important part: The variable <processingControl.inProc.params_fixed_runtime>
			// had been fixed during processing and can be released at this point!!
			// =================================================================================
			release_countchannels_datatype(processingControl.inProc.params_fixed_runtime);
		}

		if (runtime.threadCtrl.coreHdl.hdl)
		{

			if (runtime.threadCtrl.asyncAudioThreadId >= 0)
			{
				resL = runtime.threadCtrl.coreHdl.hdl->unregister_tc_thread(runtime.threadCtrl.asyncAudioThreadId);
				runtime.threadCtrl.asyncAudioThreadAssoc = false;
				runtime.threadCtrl.asyncAudioThreadId = -1;
			}
			resL = this->onInit.parentRef->runtime.theToolsHost->return_reference_tool(JVX_COMPONENT_THREADCONTROLLER, runtime.threadCtrl.coreHdl.object);
			runtime.threadCtrl.coreHdl.object = NULL;
			runtime.threadCtrl.coreHdl.hdl = NULL;
		}		

		// Reset the last flag
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_PREPARE_STATUS_INIT_BF;
		// ================================================================================

		CjvxProperties::_lock_properties_local();

		CjvxProperties::_undo_update_property_access_type(
			genGenericWrapper_device::properties_active.nonBlockingAsync);

		CjvxProperties::_undo_update_property_access_type(
			genGenericWrapper_device::properties_active.setupAsync);

		CjvxProperties::_undo_update_property_access_type(
			genGenericWrapper_device::properties_active.setupResampler);

		CjvxProperties::_undo_update_property_access_type(
			genGenericWrapper_device::properties_active.qualityResamplerIn);

		CjvxProperties::_undo_update_property_access_type(
			genGenericWrapper_device::properties_active.qualityResamplerOut);

		CjvxProperties::_undo_update_property_access_type(
			genGenericWrapper_device::properties_active.controlThreads);

		CjvxProperties::_undo_update_property_access_type(
			genGenericWrapper_device::properties_active.autoUseHwParams);


		CjvxProperties::_unlock_properties_local();

		unlock_state();

		onInit.parentRef->on_device_caps_changed(static_cast<IjvxDevice*>(this));
#if 0
		returnLinkDataSlaveRef();
		_common_set_audio_device.referene_audio_node = NULL;
		_common_set_audio_device.link_audio_node = NULL;
#endif
		// Check that start and stop are inverse operations!!
		assert(processingControl.stateInPreparation == 0);
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i,j,k;
	jvxErrorType res = JVX_NO_ERROR;

	jvxSize idxToSw;
	jvxSize idxToHwFile;

	// Take over the number of buffers from device
	processingControl.numBuffers_sender_to_receiver_hw = 1;
	if(theData)
	{
		processingControl.numBuffers_sender_to_receiver_hw = theData->con_data.number_buffers;
	}

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Detect operation mode(s)
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	res = prepare_detectmode();
	if(res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_PROCESSING_MODES_SET_BF;
	}

	// If we have an intermediate buffer, we need to allocate only one channelset,
	// otherwise, we should allocate as many as the hardware desires
	processingControl.numBuffers_sender_to_receiver_sw = 1;
	if(proc_fields.seq_operation_in == PROC)
	{
		processingControl.numBuffers_sender_to_receiver_sw = processingControl.numBuffers_sender_to_receiver_hw;
	}

	processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_NUMBER_BUFFERS_SET_BF;

	// Prepare all this what was in "prepare" before
	if(res == JVX_NO_ERROR)
	{
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// Prepare link data type
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// MODIFICATION: the link data type is already there on connect, no need to allocate
		// an extra one
		//res = jvx_initDataLinkDescriptor(&processingControl.inProc.theData_out_to_queue);
		//assert(res == JVX_NO_ERROR);

		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_PREPARE_STATUS_LINK_TYPE_PREPARED_BF;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Prepare the linkdata type and allocate memory in node
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	res = prepare_linkdata(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_LINK_DATA_SENDER_TO_RECEIVER_ALLOCATED_BF;
	}

	res = prepare_preplinkmapper_input(&_common_set_ocon.theData_out);
	if (res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_SHORTCUTS_LINKMAPPER_INPUT_COMPLETE_BF;
	}
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Prepare the input files
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	res = prepare_inputfiles();
	if(res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUTFILES_COMPLETE_BF;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Prepare the input reference
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	res = prepare_inputreferences();
	if(res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUTREFERENCES_COMPLETE_BF;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Prepare the input dummies
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	res = prepare_inputDummies();
	if (res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUTDUMMIES_COMPLETE_BF;
	}


	// =========================================================================================
	// File handling principally complete here..
	// =========================================================================================

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Prepare all buffers and components on the input side
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	res = prepare_inputprocessing();
	if(res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUT_BUFFERS_COMPLETE_BF;
	}

	// Let us create the pointer shortcuts
	for(i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
	{
		if(runtime.channelMappings.inputChannelMapper[i].isSelected)
		{
			switch(runtime.channelMappings.inputChannelMapper[i].theChannelType)
			{
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE:


				assert(theData != NULL);

				// Need to create shortcut here!!
				switch(proc_fields.seq_operation_in)
				{
				case NOTHING:

					// Only file input. File input has been completely handled before
					break;
				case PROC:

					idxToSw = runtime.channelMappings.inputChannelMapper[i].toSw.pointsToId_inclFiles;
					idxToHwFile = runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdSelectionThisDevice;

					// The number of software buffers is identical to the number of input hardware buffers in this case
					// We therefore forward the buffer pointer
					assert(processingControl.numBuffers_sender_to_receiver_sw == theRelocator._common_set_icon.theData_in->con_data.number_buffers);
					assert(processingControl.numBuffers_sender_to_receiver_sw == _common_set_ocon.theData_out.con_data.number_buffers);

					for(k = 0; k < processingControl.numBuffers_sender_to_receiver_sw; k++)
					{
						theRelocator._common_set_icon.theData_in->con_data.bExt.raw[k][idxToHwFile] =
							_common_set_ocon.theData_out.con_data.bExt.raw[k][idxToSw];
						theRelocator._common_set_icon.theData_in->con_data.bExt.offset[k][idxToHwFile] =
							_common_set_ocon.theData_out.con_data.bExt.offset[k][idxToSw];
						theRelocator._common_set_icon.theData_in->con_data.buffers[k][idxToHwFile] =
							_common_set_ocon.theData_out.con_data.buffers[k][idxToSw];
						theRelocator._common_set_icon.theData_in->con_data.bExt.sz = 
							_common_set_ocon.theData_out.con_data.bExt.sz;
					}
					break;

				case TB_PROC:
				case RES_PROC:
				case RES_TB_PROC:
				case DACON_PROC:
				case DACON_TB_PROC:
				case RES_DACON_PROC:
				case RES_DACON_TB_PROC:

					idxToHwFile = runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdSelectionThisDevice;

					// We must have as many input fields (ptrA_in) as we have hardware input buffers
					assert(processingControl.numBuffers_sender_to_receiver_hw == theRelocator._common_set_icon.theData_in->con_data.number_buffers);

					for(k = 0; k < theRelocator._common_set_icon.theData_in->con_data.number_buffers; k++)
					{
						theRelocator._common_set_icon.theData_in->con_data.buffers[k][idxToHwFile] = 
							proc_fields.ptrA_in_net[k][idxToHwFile];
						theRelocator._common_set_icon.theData_in->con_data.bExt.raw[k][idxToHwFile] =
							proc_fields.ptrA_in_raw[k][idxToHwFile];
						theRelocator._common_set_icon.theData_in->con_data.bExt.offset[k][idxToHwFile] =
							proc_fields.ptrA_in_off[k][idxToHwFile];
						theRelocator._common_set_icon.theData_in->con_data.bExt.sz = proc_fields.ptrA_in_sz;
					}
					break;

				default:
					assert(0);
				}
				break;
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:

				idxToSw = runtime.channelMappings.inputChannelMapper[i].toSw.pointsToId_inclFiles;
				idxToHwFile = runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdAllThisDevice;

				for(j = 0; j < onInit.theFilesRef->theInputFiles.size(); j++)
				{
					if(runtime.channelMappings.inputChannelMapper[i].refHint ==
						onInit.theFilesRef->theInputFiles[j].common.uniqueIdHandles)
					{
						// There should be as many input buffers as there are software buffers since the following object 
						// knows nothing about the type of the channel
						assert(processingControl.numBuffers_sender_to_receiver_sw == _common_set_ocon.theData_out.con_data.number_buffers);

						for(k = 0; k < processingControl.numBuffers_sender_to_receiver_sw; k++)
						{
							onInit.theFilesRef->theInputFiles[j].common.processing.fieldSamplesSW[k][idxToHwFile] =
								reinterpret_cast<jvxByte*>(_common_set_ocon.theData_out.con_data.buffers[k][idxToSw]);
						}
					}
				}
				break;

			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL:

				idxToSw = runtime.channelMappings.inputChannelMapper[i].toSw.pointsToId_inclFiles;
				idxToHwFile = runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdAllThisDevice;

				for(j = 0; j < onInit.theExtRef->theInputReferences.size(); j++)
				{
					if(runtime.channelMappings.inputChannelMapper[i].refHint ==
						onInit.theExtRef->theInputReferences[j].uniqueIdHandles)
					{
						// There should be as many input buffers as there are software buffers since the following object 
						// knows nothing about the type of the channel
						assert(processingControl.numBuffers_sender_to_receiver_sw == _common_set_ocon.theData_out.con_data.number_buffers);

						for(k = 0; k < processingControl.numBuffers_sender_to_receiver_sw; k++)
						{
							onInit.theExtRef->theInputReferences[j].processing.fieldSamplesSW[k][idxToHwFile] =
								reinterpret_cast<jvxByte*>(_common_set_ocon.theData_out.con_data.buffers[k][idxToSw]);
						}
					}
				}
				break;

			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY:

				idxToSw = runtime.channelMappings.inputChannelMapper[i].toSw.pointsToId_inclFiles;

				for (j = 0; j < onInit.theDummyRef->inputs.size(); j++)
				{
					if (runtime.channelMappings.inputChannelMapper[i].refHint == onInit.theDummyRef->inputs[j].uniqueId)
					{
						assert(onInit.theDummyRef->inputs[j].wasStarted);
						
						// There should be as many input buffers as there are software buffers since the following object 
						// knows nothing about the type of the channel
						assert(processingControl.numBuffers_sender_to_receiver_sw == _common_set_ocon.theData_out.con_data.number_buffers);

						for (k = 0; k < processingControl.numBuffers_sender_to_receiver_sw; k++)
						{
							onInit.theDummyRef->inputs[j].buf[k] =
								reinterpret_cast<jvxByte*>(_common_set_ocon.theData_out.con_data.buffers[k][idxToSw]);
						}
						break;
					}
				}
				break;
			default:
				assert(0);
			}
		}
	}
	processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUT_BUFFERS_LINKED_BF;

	//}
	// Indicate that linkage is done
	processingControl.inProc.fieldsLinkedInput = true;

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Prepare the timestamp buffer
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	prepare_timestamps();
	if (res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_TIMESTAMPS_PREPARED_BF;
	}
	return(JVX_NO_ERROR);
}


jvxErrorType
CjvxGenericWrapperDevice::reconfigured_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	assert(0);
	return(res);
}


jvxErrorType
CjvxGenericWrapperDevice::prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i,j,k;
	jvxSize idxChanInProc = 0;
	jvxSize idxChanHw = 0;

	jvxSize idxToSw;
	jvxSize idxToHwFile;

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Create id-shortcuts in link mapper
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	res = prepare_preplinkmapper_output(theData);
	if (res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_SHORTCUTS_LINKMAPPER_OUTPUT_COMPLETE_BF;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Prepare the output files
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	prepare_outputfiles();

	if(res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUTFILES_COMPLETE_BF;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Prepare the output files
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	prepare_outputreferences();

	if(res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUTREFERENCES_COMPLETE_BF;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Prepare the output dummies
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	prepare_outputDummies();

	if (res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUTDUMMIES_COMPLETE_BF;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Prepare all buffers and components on the output side
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	res = prepare_outputprocessing(_common_set_icon.theData_in->con_data.bExt.prepend,
		_common_set_icon.theData_in->con_data.bExt.append,
		_common_set_icon.theData_in->con_data.bExt.align);

	if(res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUT_BUFFERS_COMPLETE_BF;
	}


	// Let us create the pointer shortcuts
	for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
	{
		if(runtime.channelMappings.outputChannelMapper[i].isSelected)
		{
			idxChanInProc = runtime.channelMappings.outputChannelMapper[i].proc.idInChannelStruct;
			switch(runtime.channelMappings.outputChannelMapper[i].theChannelType)
			{
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE:

				// Need to create shortcut here!!
				assert(theData != NULL);


				switch(proc_fields.seq_operation_out)
				{
				case NOTHING:

					// Only file input. File input has been completely handled before
					break;
				case PROC:

					idxToSw = runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_inclFiles;
					idxToHwFile = runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdSelectionThisDevice;

					// We must have as many hw output buffers as there are hardware buffers in the device
					//assert(processingControl.numBuffers_receiver_to_sender_sw == theData->con_data.number_buffers);
					assert(processingControl.numBuffers_receiver_to_sender_sw == theRelocator._common_set_ocon.theData_out.con_data.number_buffers);

					for(k = 0; k < processingControl.numBuffers_receiver_to_sender_sw; k++)
					{
						theData->con_data.buffers[k][idxToSw] =
							theRelocator._common_set_ocon.theData_out.con_data.buffers[k][idxToHwFile];
					}
					break;

				case TB_PROC:
				case RES_PROC:
				case RES_TB_PROC:
				case DACON_PROC:
				case DACON_TB_PROC:
				case RES_DACON_PROC:
				case RES_DACON_TB_PROC:

					idxToSw = runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_inclFiles;
					idxToHwFile = runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdSelectionThisDevice;

					// If we are here, there is no direct contact to the hardware - hence, there is only one output buffer
					for (k = 0; k < processingControl.numBuffers_receiver_to_sender_sw; k++)
					{
						theData->con_data.buffers[k][idxToSw] =
							proc_fields.ptrA_out_net[k][idxToHwFile];
					}
					break;

				default:
					assert(0);
				}
				break;
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:

				idxToSw = runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_inclFiles;
				idxToHwFile = runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdAllThisDevice;

				for(j = 0; j < onInit.theFilesRef->theOutputFiles.size(); j++)
				{
					if(runtime.channelMappings.outputChannelMapper[i].refHint ==
						onInit.theFilesRef->theOutputFiles[j].common.uniqueIdHandles)
					{
						// Yes, we might work with only the same buffer here. If the software requires two outputs since
						// other channels are connected to the hardware, we MIGHT use the same buffer on both entries in the
						// data link descrptor. However, it works as it is and hence will be left like this
						for(k = 0; k < processingControl.numBuffers_receiver_to_sender_sw; k++)
						{
							theData->con_data.buffers[k][idxToSw] =
								onInit.theFilesRef->theOutputFiles[j].common.processing.fieldSamplesSW[k][idxToHwFile];
						}
						break;

					}
				}
				break;

			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL:

				idxToSw = runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_inclFiles;
				idxToHwFile = runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdAllThisDevice;

				for(j = 0; j < onInit.theExtRef->theOutputReferences.size(); j++)
				{
					if(runtime.channelMappings.outputChannelMapper[i].refHint ==
						onInit.theExtRef->theOutputReferences[j].uniqueIdHandles)
					{
						// Yes, we might work with only the same buffer here. If the software requires two outputs since
						// other channels are connected to the hardware, we MIGHT use the same buffer on both entries in the
						// data link descrptor. However, it works as it is and hence will be left like this
						for(k = 0; k < processingControl.numBuffers_receiver_to_sender_sw; k++)
						{
							theData->con_data.buffers[k][idxToSw] =
								onInit.theExtRef->theOutputReferences[j].processing.fieldSamplesSW[k][idxToHwFile];
						}
						break;

					}
				}
				break;

			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY:

				idxToSw = runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_inclFiles;

				for (j = 0; j < onInit.theDummyRef->outputs.size(); j++)
				{
					if (runtime.channelMappings.outputChannelMapper[i].refHint == onInit.theDummyRef->outputs[j].uniqueId)
					{
						assert(onInit.theDummyRef->outputs[j].wasStarted);

						// Yes, we might work with only the same buffer here. If the software requires two outputs since
						// other channels are connected to the hardware, we MIGHT use the same buffer on both entries in the
						// data link descrptor. However, it works as it is and hence will be left like this
						for (k = 0; k < processingControl.numBuffers_receiver_to_sender_sw; k++)
						{
							theData->con_data.buffers[k][idxToSw] =
								onInit.theDummyRef->outputs[j].buf[k];
						}
						break;

					}
				}
				break;
			}
		}
	}
	processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUT_BUFFERS_LINKED_BF;
	processingControl.inProc.fieldsLinkedOutput = true;

	/*
	runtime.channelMappings.outputChannelMapper_hwrev.resize(processingControl.inProc.params.chanshw_out);
	for (i = 0; i < runtime.channelMappings.outputChannelMapper_hwrev.size(); i++)
	{
		runtime.channelMappings.outputChannelMapper_hwrev[i] = JVX_SIZE_UNSELECTED;

		for (j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
		{
			if (runtime.channelMappings.outputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
			{
				runtime.channelMappings.outputChannelMapper_hwrev[i] = j;
			}
		}
	}*/

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxSize i, j, k;
	jvxSize idxChanInProc = 0;
	jvxSize idxChanHw = 0;
	jvxSize idxToSw;
	jvxSize idxToHwFile;

	jvxErrorType res = JVX_NO_ERROR;

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUT_BUFFERS_LINKED_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUT_BUFFERS_LINKED_BF;
		// Nothing to do
		// Let us create the pointer shortcuts
		for (i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
		{
			if (runtime.channelMappings.outputChannelMapper[i].isSelected)
			{
				idxChanInProc = runtime.channelMappings.outputChannelMapper[i].proc.idInChannelStruct;
				switch (runtime.channelMappings.outputChannelMapper[i].theChannelType)
				{
				case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE:

					// Need to create shortcut here!!
					assert(theData != NULL);


					switch (proc_fields.seq_operation_out)
					{
					case NOTHING:

						// Only file input. File input has been completely handled before
						break;
					case PROC:

						idxToSw = runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_inclFiles;
						idxToHwFile = runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdSelectionThisDevice;

						for (k = 0; k < theData->con_data.number_buffers; k++)
						{
							theData->con_data.buffers[k][idxToSw] = NULL;
						}
						break;

					case TB_PROC:
					case RES_PROC:
					case RES_TB_PROC:
					case DACON_PROC:
					case DACON_TB_PROC:
					case RES_DACON_PROC:
					case RES_DACON_TB_PROC:

						idxToSw = runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_inclFiles;
						idxToHwFile = runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdSelectionThisDevice;

						for (k = 0; k < theData->con_data.number_buffers; k++)
						{
							theData->con_data.buffers[k][idxToSw] = NULL;
						}
						break;

					default:
						assert(0);
					}
					break;
				case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:

					idxToSw = runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_inclFiles;
					idxToHwFile = runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdAllThisDevice;

					for (j = 0; j < onInit.theFilesRef->theOutputFiles.size(); j++)
					{
						if (runtime.channelMappings.outputChannelMapper[i].refHint ==
							onInit.theFilesRef->theOutputFiles[j].common.uniqueIdHandles)
						{
							for (k = 0; k < processingControl.numBuffers_receiver_to_sender_sw; k++)
							{
								theData->con_data.buffers[k][idxToSw] = NULL;
							}
							break;

						}
					}
					break;

				case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL:

					idxToSw = runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_inclFiles;
					idxToHwFile = runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdAllThisDevice;

					for (j = 0; j < onInit.theExtRef->theOutputReferences.size(); j++)
					{
						if (runtime.channelMappings.outputChannelMapper[i].refHint ==
							onInit.theExtRef->theOutputReferences[j].uniqueIdHandles)
						{
							for (k = 0; k < processingControl.numBuffers_receiver_to_sender_sw; k++)
							{
								theData->con_data.buffers[k][idxToSw] = NULL;
							}
							break;

						}
					}
					break;

				case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY:

					idxToSw = runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_inclFiles;

					for (j = 0; j < onInit.theDummyRef->outputs.size(); j++)
					{
						if (runtime.channelMappings.outputChannelMapper[i].refHint == onInit.theDummyRef->outputs[j].uniqueId)
						{
							assert(onInit.theDummyRef->outputs[j].wasStarted);
							for (k = 0; k < processingControl.numBuffers_receiver_to_sender_sw; k++)
							{
								theData->con_data.buffers[k][idxToSw] = NULL;
							}
							break;

						}
					}
					break;
				}
			}
		}
	}
	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUT_BUFFERS_COMPLETE_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUT_BUFFERS_COMPLETE_BF;
		postprocess_outputprocessing();
	}
	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUTDUMMIES_COMPLETE_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUTDUMMIES_COMPLETE_BF;
		postprocess_outputDummies();
	}

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUTREFERENCES_COMPLETE_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUTREFERENCES_COMPLETE_BF;
		postprocess_outputreferences();
	}
	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUTFILES_COMPLETE_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_OUTPUTFILES_COMPLETE_BF;
		postprocess_outputfiles();
	}

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_SHORTCUTS_LINKMAPPER_OUTPUT_COMPLETE_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_SHORTCUTS_LINKMAPPER_OUTPUT_COMPLETE_BF;
		postprocess_preplinkmapper_output();
	}

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i, j, k;
	jvxSize idxChanInProc = 0;
	jvxSize idxChanHw = 0;
	jvxSize idxToSw;
	jvxSize idxToHwFile;

	// JVX_GENERIC_WRAPPER_SUBDEVICE_PREPARED
	// and
	// JVX_GENERIC_WRAPPER_SETUP_COMPLETE
	// are unused state
	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_TIMESTAMPS_PREPARED_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_TIMESTAMPS_PREPARED_BF;
		postprocess_timestamps();
	}

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUT_BUFFERS_LINKED_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUT_BUFFERS_LINKED_BF;
		for (i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
		{
			if (runtime.channelMappings.inputChannelMapper[i].isSelected)
			{
				switch (runtime.channelMappings.inputChannelMapper[i].theChannelType)
				{
				case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE:


					assert(theData != NULL);

					// Need to create shortcut here!!
					switch (proc_fields.seq_operation_in)
					{
					case NOTHING:

						// Only file input. File input has been completely handled before
						break;
					case PROC:

						idxToSw = runtime.channelMappings.inputChannelMapper[i].toSw.pointsToId_inclFiles;
						idxToHwFile = runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdSelectionThisDevice;

						for (k = 0; k < theData->con_data.number_buffers; k++)
						{
							theData->con_data.buffers[k][idxToHwFile] = NULL;
						}
						break;

					case TB_PROC:
					case RES_PROC:
					case RES_TB_PROC:
					case DACON_PROC:
					case DACON_TB_PROC:
					case RES_DACON_PROC:
					case RES_DACON_TB_PROC:

						idxToHwFile = runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdSelectionThisDevice;

						for (k = 0; k < theData->con_data.number_buffers; k++)
						{
							theData->con_data.buffers[k][idxToHwFile] = NULL;
						}
						break;

					default:
						assert(0);
					}
					break;
				case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:

					idxToSw = runtime.channelMappings.inputChannelMapper[i].toSw.pointsToId_inclFiles;
					idxToHwFile = runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdAllThisDevice;

					for (j = 0; j < onInit.theFilesRef->theInputFiles.size(); j++)
					{
						if (runtime.channelMappings.inputChannelMapper[i].refHint ==
							onInit.theFilesRef->theInputFiles[j].common.uniqueIdHandles)
						{
							for (k = 0; k < processingControl.numBuffers_sender_to_receiver_sw; k++)
							{
								onInit.theFilesRef->theInputFiles[j].common.processing.fieldSamplesSW[k][idxToHwFile] = NULL;
							}
						}
					}
					break;

				case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL:

					idxToSw = runtime.channelMappings.inputChannelMapper[i].toSw.pointsToId_inclFiles;
					idxToHwFile = runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdAllThisDevice;

					for (j = 0; j < onInit.theExtRef->theInputReferences.size(); j++)
					{
						if (runtime.channelMappings.inputChannelMapper[i].refHint ==
							onInit.theExtRef->theInputReferences[j].uniqueIdHandles)
						{
							for (k = 0; k < processingControl.numBuffers_sender_to_receiver_sw; k++)
							{
								onInit.theExtRef->theInputReferences[j].processing.fieldSamplesSW[k][idxToHwFile] = NULL;
							}
						}
					}
					break;

				case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY:

					idxToSw = runtime.channelMappings.inputChannelMapper[i].toSw.pointsToId_inclFiles;

					for (j = 0; j < onInit.theDummyRef->inputs.size(); j++)
					{
						if (runtime.channelMappings.inputChannelMapper[i].refHint == onInit.theDummyRef->inputs[j].uniqueId)
						{
							assert(onInit.theDummyRef->inputs[j].wasStarted);
							for (k = 0; k < processingControl.numBuffers_sender_to_receiver_sw; k++)
							{
								onInit.theDummyRef->inputs[j].buf[k] = NULL;
							}
							break;
						}
					}
					break;
				default:
					assert(0);
				}
			}
		}
	}

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUT_BUFFERS_COMPLETE_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUT_BUFFERS_COMPLETE_BF;
		postprocess_inputprocessing();
	}

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUTDUMMIES_COMPLETE_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUTDUMMIES_COMPLETE_BF;
		postprocess_inputDummies();
	}

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUTREFERENCES_COMPLETE_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUTREFERENCES_COMPLETE_BF;
		postprocess_inputreferences();
	}

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUTFILES_COMPLETE_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_INPUTFILES_COMPLETE_BF;
		postprocess_inputfiles();
	}

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_SHORTCUTS_LINKMAPPER_INPUT_COMPLETE_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_SHORTCUTS_LINKMAPPER_INPUT_COMPLETE_BF;
		postprocess_preplinkmapper_input();
	}

	processingControl.inProc.fieldsLinkedOutput = false;

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_LINK_DATA_SENDER_TO_RECEIVER_ALLOCATED_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_LINK_DATA_SENDER_TO_RECEIVER_ALLOCATED_BF;

		// This will connect through all connected objects
		postprocess_linkdata(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	// ========================================================================================================
	// ========================================================================================================

	processingControl.inProc.fieldsLinkedInput = false;

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_PREPARE_STATUS_LINK_TYPE_PREPARED_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_PREPARE_STATUS_LINK_TYPE_PREPARED_BF;

		// set the slots
		jvxSize numDeallocated = 0;

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// Prepare link data type
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	}


	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_NUMBER_BUFFERS_SET_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_NUMBER_BUFFERS_SET_BF;
		processingControl.numBuffers_sender_to_receiver_sw = 1;
	}

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_PROCESSING_MODES_SET_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_PROCESSING_MODES_SET_BF;
		postprocess_detectmode();
	}

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxSize i;

	resL = prepare_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	processingControl.inProc.fieldsLinkedInput = false;
	processingControl.inProc.fieldsLinkedOutput = false;

	res = _prepare_chain_master_no_link();
	if (res != JVX_NO_ERROR)
	{
		goto leave_error;
	}

#ifdef JVX_ASYNC_BUFFER_DEBUG_CNT
	memset(runtime.dbg_events, 0, sizeof(runtime.dbg_events));
	JVX_INITIALIZE_MUTEX(runtime.dbg_safeAccess);
	JVX_GET_TICKCOUNT_US_SETREF(&runtime.dbg_tstamp);
	runtime.dbg_events_cnt = 0;
#endif

	// Browse through all connections
	if (onInit.connectedDevice)
	{
#if 0
		switch (proc_fields.doSelection)
		{
		case CjvxGenericWrapperDevice::SELECTION_ASYNC_MODE:
		case CjvxGenericWrapperDevice::SELECTION_SYNC_MODE:
#endif
			res = theRelocator.x_prepare_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res != JVX_NO_ERROR)
			{
				resL = theRelocator.x_postprocess_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));

				// This is new: if the prepare fails we need to do some additional cleanup
				// to undo 
				// 1) _prepare_chain_master_no_link
				// 2) prepare_chain_master_autostate
				// 
				goto exit_error_I;
			}
#if 0
			break;
		case CjvxGenericWrapperDevice::SELECTION_FILEIO_MODE:
			res = this->prepare_sender_to_receiver(NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			assert(res == JVX_NO_ERROR);
			if (res != JVX_NO_ERROR)
			{
				res = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				assert(res == JVX_NO_ERROR);
				goto leave_error;
			}
			break;
		default:
			assert(0);
			break;
		}
#endif
	}
	else
	{
		/*
		_common_set_ocon.theData_out.con_params.buffersize = processingControl.computedParameters.bSize_hw;
		_common_set_ocon.theData_out.con_params.format = processingControl.computedParameters.form_hw;
		_common_set_ocon.theData_out.con_data.buffers = NULL;
		_common_set_ocon.theData_out.con_data.number_buffers = 1;
		_common_set_ocon.theData_out.con_params.number_channels = 0;
		_common_set_ocon.theData_out.con_params.rate = processingControl.computedParameters.sRate_hw;
		_common_set_ocon.theData_out.link.descriptor_out_linked->con_data.user_hints = NULL;
		*/
		res = this->prepare_sender_to_receiver(NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		assert(res == JVX_NO_ERROR);
		if (res != JVX_NO_ERROR)
		{
			res = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			assert(res == JVX_NO_ERROR);
			goto leave_error;
		}
	}

	//if(_common_set_ld_master.state == JVX_STATE_ACTIVE)
	JVX_GET_TICKCOUNT_US_SETREF(&this->processingControl.inProc.tStampRef);

	// =====================================================================
	// Start thread controller
	// =====================================================================

	if (JVX_EVALUATE_BITFIELD(genGenericWrapper_device::properties_active.controlThreads.value.selection() & 0x2))
	{
		if (this->onInit.parentRef->runtime.theToolsHost)
		{
			resL = this->onInit.parentRef->runtime.theToolsHost->reference_tool(JVX_COMPONENT_THREADCONTROLLER, &runtime.threadCtrl.coreHdl.object, 0, NULL);
			if ((resL == JVX_NO_ERROR) && (runtime.threadCtrl.coreHdl.object))
			{
				resL = runtime.threadCtrl.coreHdl.object->request_specialization(reinterpret_cast<jvxHandle**>(&runtime.threadCtrl.coreHdl.hdl), NULL, NULL);
				if ((resL == JVX_NO_ERROR) && runtime.threadCtrl.coreHdl.hdl)
				{
					jvxState stat = JVX_STATE_NONE;
					runtime.threadCtrl.coreHdl.hdl->state(&stat);
					if (stat < JVX_STATE_ACTIVE)
					{
						this->onInit.parentRef->runtime.theToolsHost->return_reference_tool(JVX_COMPONENT_THREADCONTROLLER, runtime.threadCtrl.coreHdl.object);
						runtime.threadCtrl.coreHdl.hdl = NULL;
						runtime.threadCtrl.coreHdl.object = NULL;
					}
				}
			}
		}
	}

	if (processingControl.inProc.fieldsLinkedInput == false)
	{
		assert(0);
		// In case the prepare_sender_to_receiver was not called before, do it here manually (e.g. pure file io)
		res = this->prepare_sender_to_receiver(NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		processingControl.inProc.fieldsLinkedInput = true;
	}
	if (processingControl.inProc.fieldsLinkedOutput == false)
	{
		assert(0);
		// In case the prepare_sender_to_receiver was not called before, do it here manually (e.g. pure file io)
		this->prepare_complete_receiver_to_sender(NULL);
		processingControl.inProc.fieldsLinkedOutput = true;
	}

	if (res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_SUBDEVICE_PREPARED_BF;
	}

	if (res == JVX_NO_ERROR)
	{
		processingControl.stateInPreparation |= (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_SETUP_COMPLETE_BF;
	}

	//unlock_state();
	
	return res;

exit_error_I:

	// Check the location at which we jum here for comment
	_postprocess_chain_master_no_link();
	postprocess_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));

leave_error:

	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize numDeallocated = 0;

	jvxBool resB = _check_postprocess_chain_master();
	assert(resB);

	processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_SETUP_COMPLETE_BF;

	if (processingControl.stateInPreparation & (jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_SUBDEVICE_PREPARED_BF)
	{
		processingControl.stateInPreparation &= ~(jvxUInt32)jvxStatePrepapration::JVX_GENERIC_WRAPPER_SUBDEVICE_PREPARED_BF;
		if (onInit.connectedDevice)
		{
			// This will come back in the appropriate function
			res = theRelocator.x_postprocess_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		else
		{
			// This will propagate forward
			res = this->before_postprocess_receiver_to_sender(NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			assert(res == JVX_NO_ERROR);
		}

		if (processingControl.inProc.fieldsLinkedOutput)
		{
			assert(0);
			// If data link functions have not been called, do it manually
			this->before_postprocess_receiver_to_sender(NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			processingControl.inProc.fieldsLinkedOutput = false;
		}
		if (processingControl.inProc.fieldsLinkedInput)
		{
			assert(0);
			// If data link functions have not been called, do it manually
			this->postprocess_sender_to_receiver(NULL);
			processingControl.inProc.fieldsLinkedInput = false;
		}		
	}
	_postprocess_chain_master_no_link();

	assert(_common_set_min.theState == JVX_STATE_PREPARED);

	resL = postprocess_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return res;
};

// =================================================================================

jvxErrorType 
CjvxGenericWrapperDevice::prepare_connect_icon_x(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// Create container in which to place buffer references lateron
	// HK: This allocation has been postponed. The idea is that
	// if the output is pipelined, the input needs to adapt the number of buffers!!
	//res = jvx_allocateDataLinkDescriptor(theData, false);
	//assert(res == JVX_NO_ERROR);

	// Prepare sender to receiver will use a different linkData container
	res = this->prepare_sender_to_receiver(theRelocator._common_set_icon.theData_in JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	assert(res == JVX_NO_ERROR);

	// Check this here
	return res;
};



jvxErrorType 
CjvxGenericWrapperDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// Terminate this link here. This is the place where we reach from the software processing chain.
	// The buffers must be set lateron depending on quality of the output - file dummy or hardware
	CjvxInputOutputConnector::_prepare_connect_icon(false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	// Correct the given parameters. The algorithm chain should reach over the same values as it would correct
	/*
	_common_set_icon.theData_in->con_params.buffersize = processingControl.inProc.params.buffersize;
	_common_set_icon.theData_in->con_params.format = processingControl.inProc.params.format;
	_common_set_icon.theData_in->con_data.buffers = NULL;
	_common_set_icon.theData_in->con_data.number_buffers = 1;// Assume 1 buffer on output side.
	_common_set_icon.theData_in->con_params.number_channels = processingControl.inProc.params.chans_out;
	_common_set_icon.theData_in->con_params.rate = processingControl.inProc.params.samplerate;
	*/
	_common_set_icon.theData_in->con_params.buffersize = processingControl.computedParameters.bSize_sw;
	_common_set_icon.theData_in->con_params.format = processingControl.computedParameters.form_sw;
	_common_set_icon.theData_in->con_data.buffers = NULL;
	_common_set_icon.theData_in->con_data.number_buffers = JVX_MAX(1, _common_set_icon.theData_in->con_data.number_buffers);// Thre might be more than one buffer on the input side
	_common_set_icon.theData_in->con_params.number_channels = processingControl.inProc.params_fixed_runtime.chans_out;
	_common_set_icon.theData_in->con_params.rate = processingControl.computedParameters.sRate_sw;

	// If a device is involved, pass chaining frowad to involved hardware device
	if (onInit.connectedDevice)
	{
		// Change over to host relocator
		theRelocator._common_set_ocon.theData_out.con_params.buffersize = processingControl.computedParameters.bSize_hw;
		theRelocator._common_set_ocon.theData_out.con_params.format = processingControl.computedParameters.form_hw;
		theRelocator._common_set_ocon.theData_out.con_data.buffers = NULL;
		theRelocator._common_set_ocon.theData_out.con_data.number_buffers = 1;

		if (proc_fields.seq_operation_out == PROC)
		{
			theRelocator._common_set_ocon.theData_out.con_data.number_buffers = JVX_MAX(1, _common_set_icon.theData_in->con_data.number_buffers);
		}
		theRelocator._common_set_ocon.theData_out.con_params.number_channels = processingControl.inProc.params_fixed_runtime.chanshw_out;
		theRelocator._common_set_ocon.theData_out.con_params.rate = processingControl.computedParameters.sRate_hw;
		
		// Forward user hints from the hw to relocator link (not the user hints from the sw connection!)
		//theRelocator._common_set_ocon.theData_out.con_data.user_hints =
		//	theRelocator._common_set_icon.theData_in->con_data.user_hints;

		// Move over to relocator (HW) side 
		res = theRelocator.prepare_connect_icon_x(_common_set_icon.theData_in JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		// Store number of buffers used by hardware output
		if (res == JVX_NO_ERROR)
		{
			processingControl.numBuffers_receiver_to_sender_hw = theRelocator._common_set_ocon.theData_out.con_data.number_buffers;
		}
	}

	// If we have an intermediate buffer, we need to allocate only one channelset,
	// otherwise, we should allocate as many as the hardware desires
	processingControl.numBuffers_receiver_to_sender_sw = _common_set_icon.theData_in->con_data.number_buffers;
	if (proc_fields.seq_operation_out == PROC)
	{
		processingControl.numBuffers_receiver_to_sender_sw = JVX_MAX(processingControl.numBuffers_receiver_to_sender_hw, processingControl.numBuffers_receiver_to_sender_sw);
	}

	// Override the specification from the audio node output side
	_common_set_icon.theData_in->con_data.number_buffers = processingControl.numBuffers_receiver_to_sender_sw;

	// Correct what was passed by algorithm chain in case we have multiple buffers in pipeline
	// TODO!!! Use different number of buffers for input as well as output side
	//_common_set_icon.theData_in->con_data.number_buffers = JVX_MAX(processingControl.numBuffers_receiver_to_sender_sw,
	//	_common_set_icon.theData_in->con_data.number_buffers);

	// Create empty link descriptor - to be filled lateron
	jvx_allocateDataLinkDescriptor(_common_set_icon.theData_in, false);
	assert(res == JVX_NO_ERROR);

	if (proc_fields.seq_operation_out == PROC)
	{
		_common_set_icon.theData_in->con_pipeline = theRelocator._common_set_ocon.theData_out.con_pipeline;
	}
	else
	{
		jvx_allocateDataLinkPipelineControl(_common_set_icon.theData_in);
		
		// Prepopulate pipeline buffer
		*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr = 0;
		_common_set_icon.theData_in->con_pipeline.reserve_buffer_pipeline_stage[
			*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr].idProcess =
			_common_set_io_common.myRuntimeId;
	}

	// Cross link the output buffers and complete output side
	this->prepare_complete_receiver_to_sender(_common_set_icon.theData_in);
	assert(res == JVX_NO_ERROR);

	// Cross copy to receiver_to_sender side
	jvx_ccopyDataLinkDescriptor(_common_set_icon.theData_in, &_common_set_ocon.theData_out);
	_common_set_icon.theData_in->con_compat.from_receiver_buffer_allocated_by_sender = _common_set_ocon.theData_out.con_data.buffers;
	_common_set_icon.theData_in->con_compat.user_hints = _common_set_ocon.theData_out.con_compat.user_hints;

	return res;
};

// ===========================================================================
jvxErrorType
CjvxGenericWrapperDevice::postprocess_connect_icon_x(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// This will propagate forward
	res = this->before_postprocess_receiver_to_sender(theRelocator._common_set_icon.theData_in JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	assert(res == JVX_NO_ERROR);

	return res;
}

jvxErrorType 
CjvxGenericWrapperDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	_common_set_icon.theData_in->con_compat.user_hints = NULL;
	_common_set_icon.theData_in->con_compat.from_receiver_buffer_allocated_by_sender = NULL;
	jvx_cinitDataLinkDescriptor(_common_set_icon.theData_in);

	// This function will forward the postprocess.. call to the concatenated blocks 
	res = this->postprocess_sender_to_receiver(_common_set_icon.theData_in);
	assert(res == JVX_NO_ERROR);

	// Handle pipeline control struct
	if (proc_fields.seq_operation_out != PROC)
	{
		jvx_deallocateDataLinkPipelineControl(_common_set_icon.theData_in);
	}
	jvx_neutralDataLinkDescriptor_pipeline(&_common_set_icon.theData_in->con_pipeline);

	res = jvx_deallocateDataLinkDescriptor(_common_set_icon.theData_in, false);
	assert(res == JVX_NO_ERROR);

	// Rest number of buffer
	processingControl.numBuffers_receiver_to_sender_sw = 1;

	// If we are here, this is the connect homing to be forwarded to the "real" device
	if (onInit.connectedDevice)
	{
		res = theRelocator.postprocess_connect_icon_x(_common_set_icon.theData_in JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		assert(res == JVX_NO_ERROR);

		// Change over to host relocator
		theRelocator._common_set_ocon.theData_out.con_params.buffersize = 0;
		theRelocator._common_set_ocon.theData_out.con_params.format = JVX_DATAFORMAT_NONE;
		theRelocator._common_set_ocon.theData_out.con_data.buffers = NULL;
		theRelocator._common_set_ocon.theData_out.con_data.number_buffers = 0; 
		theRelocator._common_set_ocon.theData_out.con_params.number_channels = 0;
		theRelocator._common_set_ocon.theData_out.con_params.rate = 0;
	}
	_postprocess_connect_icon(false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return res;
};

jvxErrorType 
CjvxGenericWrapperDevice::test_connect_icon_x(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	IjvxDataConnectionCommon* conCom = NULL;
	IjvxDataConnections* conCon = NULL;
	jvxApiString str;
	jvxBool validUnDesc = false;
	if (_common_set_ld_master.refProc)
	{
		_common_set_ld_master.refProc->connection_context(&conCon);
		if (conCon)
		{
			conCon->unique_descriptor(&str);
			validUnDesc = true;
		}
	}
	if (validUnDesc)
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_CONTEXT(fdb, static_cast<IjvxObject*>(this), str.c_str());
	}
	else
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));
	}

	return CjvxInputOutputConnector::_test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};



