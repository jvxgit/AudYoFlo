#include "CjvxAuNForwardBuffer.h"
#include "jvx-helpers-cpp.h"

CjvxAuNForwardBuffer::CjvxAuNForwardBuffer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_NODE_TYPE_SPECIFIER_TYPE);
	_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;

	jvxDspBaseErrorType resL = jvx_audio_stack_sample_dispenser_cont_initCfg(&myAudioDispenser);
	assert(resL == JVX_DSP_NO_ERROR);
}

CjvxAuNForwardBuffer::~CjvxAuNForwardBuffer()
{

}

// ===================================================================

/*
jvxErrorType
CjvxAuNForwardBuffer::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::select(owner);
	if (res == JVX_NO_ERROR)
	{
		genForwardBuffer_node::init__config_select();
		genForwardBuffer_node::allocate__config_select();
		genForwardBuffer_node::register__config_select(static_cast<CjvxProperties*>(this));
	}
	return res;
}

jvxErrorType
CjvxAuNForwardBuffer::unselect()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::unselect();
	if (res == JVX_NO_ERROR)
	{
		genForwardBuffer_node::unregister__config_select(static_cast<CjvxProperties*>(this));
		genForwardBuffer_node::deallocate__config_select();
	}
	return res;
}
*/

// ===================================================================

jvxErrorType
CjvxAuNForwardBuffer::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		genForwardBuffer_node::init_all();
		genForwardBuffer_node::allocate_all();
		genForwardBuffer_node::register_all(static_cast<CjvxProperties*>(this));

		genForwardBuffer_node::register_callbacks(static_cast<CjvxProperties*>(this),
			set_bypass_buffer, get_processing_monitor, this, nullptr);
		// Obtain the thread handle here
		refThreads = reqInstTool<IjvxThreads>(
			_common_set.theToolsHost,
			JVX_COMPONENT_THREADS);
		assert(refThreads.cpPtr);
	}
	return res;
}

jvxErrorType
CjvxAuNForwardBuffer::deactivate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		// There may be some allocated parameters allocated on "test_connect_icon" that need to be removed
		if (reRouting.numChannels)
		{
			genForwardBuffer_node::deassociate__convert(static_cast<CjvxProperties*>(this));
			JVX_DSP_SAFE_DELETE_FIELD(reRouting.selChannels);
			reRouting.selChannels = nullptr;
			reRouting.numChannels = 0;
		}

		retInstTool<IjvxThreads>(
			_common_set.theToolsHost,
			JVX_COMPONENT_THREADS,
			refThreads);

		genForwardBuffer_node::deallocate_all();
		genForwardBuffer_node::unregister_all(static_cast<CjvxProperties*>(this));

		CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

// ===================================================================
jvxErrorType
CjvxAuNForwardBuffer::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	std::string token;
	std::string tokenDecoder;
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		if (
			node_output._common_set_node_params_a_1io.number_channels
			!= reRouting.numChannels)
		{
			jvxSize newNumChannels = 0;
			jvxBitField* newSelChannels = nullptr;

			if (node_output._common_set_node_params_a_1io.number_channels)
			{
				newNumChannels = node_output._common_set_node_params_a_1io.number_channels;
				JVX_DSP_SAFE_ALLOCATE_FIELD(newSelChannels, jvxBitField, node_output._common_set_node_params_a_1io.number_channels);
			}

			// We have N channels but allow an extraposition to allow all input channels and "silence"
			for (i = 0; i < node_output._common_set_node_params_a_1io.number_channels; i++)
			{
				// Circular routing of input to output
				jvxSize idxIn = i % node_inout._common_set_node_params_a_1io.number_channels;
				jvx_bitZSet(newSelChannels[i], idxIn);

				// Here we try to copy the previous channel idx
				if (i < reRouting.numChannels)
				{
					jvxSize idSel = jvx_bitfieldSelection2Id(reRouting.selChannels[i], reRouting.numChannels + 1);
					if (idSel < node_inout._common_set_node_params_a_1io.number_channels)
					{
						// Select previous channel selection
						jvx_bitZSet(newSelChannels[i], idSel);
					}
					else
					{
						// Select silence also
						jvx_bitZSet(newSelChannels[i], node_inout._common_set_node_params_a_1io.number_channels);
					}
				}
			}

			genForwardBuffer_node::convert.channel_selection.value.entries.clear();
			for (i = 0; i < node_output._common_set_node_params_a_1io.number_channels; i++)
			{
				genForwardBuffer_node::convert.channel_selection.value.entries.push_back("Channel #" + jvx_size2String(i));
			}
			genForwardBuffer_node::convert.channel_selection.value.entries.push_back("No Channel");

			if (reRouting.numChannels)
			{
				genForwardBuffer_node::deassociate__convert(static_cast<CjvxProperties*>(this));
				JVX_DSP_SAFE_DELETE_FIELD(reRouting.selChannels);
				reRouting.selChannels = nullptr;
				reRouting.numChannels = 0;
			}

			if (newNumChannels)
			{
				reRouting.numChannels = newNumChannels;
				reRouting.selChannels = newSelChannels;
				genForwardBuffer_node::associate__convert(static_cast<CjvxProperties*>(this), reRouting.selChannels, reRouting.numChannels);
			}
		}
	}
	return res;
}

void
CjvxAuNForwardBuffer::from_input_to_output()
{
	// Copy all output parameters from input parameters -except for the buffersize
	node_output._common_set_node_params_a_1io.samplerate = node_inout._common_set_node_params_a_1io.samplerate;
	node_output._common_set_node_params_a_1io.format = node_inout._common_set_node_params_a_1io.format;
	node_output._common_set_node_params_a_1io.subformat = node_inout._common_set_node_params_a_1io.subformat;
	node_output._common_set_node_params_a_1io.number_channels = node_inout._common_set_node_params_a_1io.number_channels;
	node_output._common_set_node_params_a_1io.segmentation.y = node_inout._common_set_node_params_a_1io.segmentation.y;

	if (genForwardBuffer_node::config_select.bypass_buffer.value)
	{
		// Input buffer: Output identical to input only for bypass-buffer-mode

		// We may also see buffers with zero buffersize here. The output is set from the end
		node_output._common_set_node_params_a_1io.buffersize = node_inout._common_set_node_params_a_1io.buffersize;
		node_output._common_set_node_params_a_1io.segmentation.x = node_inout._common_set_node_params_a_1io.segmentation.x;
	}

	/*
	 * Here is a little bit the trick of the module: we pass the new params only towards the output if a fundamental change happened.
	 * With this we make sure that the output has control of the buffersize unless input has changed.
	 */
	if (newParamsOnTestInput)
	{
		node_output._common_set_node_params_a_1io.buffersize = node_inout._common_set_node_params_a_1io.buffersize;
		node_output._common_set_node_params_a_1io.segmentation.x = node_inout._common_set_node_params_a_1io.segmentation.x;
	}

	update_output_params();
}

void
CjvxAuNForwardBuffer::test_set_output_parameters()
{
	jvxSize bsize = JVX_SIZE_UNSELECTED; // Leave the buffersize open and accept next component proposal
	if (genForwardBuffer_node::config_select.bypass_buffer.value)
	{
		bsize = node_output._common_set_node_params_a_1io.buffersize; // Here, we need to set the proper buffersize
	}
	neg_output._update_parameters_fixed(node_output._common_set_node_params_a_1io.number_channels,
		bsize, node_output._common_set_node_params_a_1io.samplerate,
		(jvxDataFormat)node_output._common_set_node_params_a_1io.format,
		(jvxDataFormatGroup)node_output._common_set_node_params_a_1io.subformat,
		(jvxDataflow)node_output._common_set_node_params_a_1io.data_flow);

	// Use the default implementation
	CjvxBareNode1ioRearrange::test_set_output_parameters();

	// Create the link between input and output channel number
	// node_output._common_set_node_params_a_1io.number_channels = node_inout._common_set_node_params_a_1io.number_channels;
}
// ===================================================================

jvxErrorType
CjvxAuNForwardBuffer::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	if (!(
		(_common_set_min.theState == JVX_STATE_ACTIVE) ||
		(_common_set_min.theState == JVX_STATE_PREPARED)))
	{
		return JVX_ERROR_WRONG_STATE;
	}

	// Fix the bypass_buffer_mode selection - it should not be changed during processing!
	bypass_buffer_mode = genForwardBuffer_node::config_select.bypass_buffer.value;
	CjvxProperties::_update_property_access_type(
		JVX_PROPERTY_ACCESS_READ_ONLY,
		genForwardBuffer_node::config_select.bypass_buffer.category,
		genForwardBuffer_node::config_select.bypass_buffer.globalIdx);
	if (!bypass_buffer_mode)
	{
		checkInputOutputMostlyIdentical = false; // This prevents also the zerocopy mode
	}

	// ================================================================

	buffermode = genForwardBuffer_node::translate__config__buffer_location_from();
	// ================================================================

	if (bypass_buffer_mode)
	{
		// Bypass buffer in bypass-buffer-mode
		_common_set_ldslave.zeroCopyBuffering_cfg = true;
		res = CjvxBareNode1ioRearrange::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	else
	{
		prepare_autostart();

		if (_common_set_min.theState == JVX_STATE_PREPARED)
		{
			// The number of pipeline stages may be increased from element to element
			_common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages = JVX_MAX(
				_common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages,
				_common_set_ldslave.num_additional_pipleline_stages);

			// The number of buffers is always lower bounded by the add pipeline stages
			_common_set_ldslave.theData_in->con_data.number_buffers = JVX_MAX(
				_common_set_ldslave.theData_in->con_data.number_buffers,
				1 + _common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages);

			// We might specify another additional lower limit for the buffers
			_common_set_ldslave.theData_in->con_data.number_buffers = JVX_MAX(
				_common_set_ldslave.theData_in->con_data.number_buffers,
				_common_set_ldslave.num_min_buffers_in);

			// Set the number of buffers as desired
			_common_set_ldslave.theData_out.con_data.number_buffers = _common_set_ldslave.num_min_buffers_out;
			_common_set_ldslave.theData_out.con_data.alloc_flags = _common_set_ldslave.theData_in->con_data.alloc_flags;
			jvx_bitClear(_common_set_ldslave.theData_in->con_data.alloc_flags,
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT);
			jvx_bitClear(_common_set_ldslave.theData_in->con_data.alloc_flags,
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT);
			_common_set_ldslave.theData_out.con_pipeline.num_additional_pipleline_stages = 0;

			// Have to review this lateron, currently: no timestamp
			_common_set_ldslave.theData_out.con_sync.type_timestamp = 0;

			res = _prepare_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

			if (res == JVX_NO_ERROR)
			{
				// Do the processing checks and allocate buffers etc
			// Setup has been verified in the test chain functions before - this is only for simpler access during processing
				/*
				input.buffersize = _common_set_ldslave.theData_in->con_params.buffersize;
				input.number_channels = _common_set_ldslave.theData_in->con_params.number_channels;
				input.samplerate = _common_set_ldslave.theData_in->con_params.rate;
				input.format = _common_set_ldslave.theData_in->con_params.format;
				*/
				res = allocate_pipeline_and_buffers_prepare_to();
				assert(res == JVX_NO_ERROR);
				jvx_bitClear(_common_set_ldslave.theData_in->con_data.alloc_flags,
					(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);
			}

			jvxBool withStartThreshold = false;
			if (buffermode == jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_INPUT)
			{
				withStartThreshold = true;
			}

			jvxSize bs = node_output._common_set_node_params_a_1io.samplerate * genForwardBuffer_node::config.buffersize_msecs.value * 0.001;
			bs = JVX_MAX(
				JVX_MAX(node_inout._common_set_node_params_a_1io.buffersize, node_output._common_set_node_params_a_1io.buffersize) * 4, bs);
			start_audiostack(
				bs,
				node_output._common_set_node_params_a_1io.number_channels,
				(jvxDataFormat)node_output._common_set_node_params_a_1io.format,
				false, withStartThreshold);

			assert(node_inout._common_set_node_params_a_1io.buffersize == _common_set_ldslave.theData_in->con_params.buffersize);
			assert(node_output._common_set_node_params_a_1io.buffersize == _common_set_ldslave.theData_out.con_params.buffersize);

			// ========================================================================================================
			// Allocate pre-sorting channel buffer and zero input buffer
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(reRouting.bufReroute, jvxData*, node_output._common_set_node_params_a_1io.number_channels);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(reRouting.zbuf, jvxData, node_inout._common_set_node_params_a_1io.buffersize);
			// ========================================================================================================

			if (buffermode == jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_INPUT)
			{
				if (_common_set_ldslave.theData_in->con_params.data_flow == jvxDataflow::JVX_DATAFLOW_PUSH_ON_PULL)
				{
					// Start the secondary thread
					resL = refThreads.cpPtr->initialize(
						static_cast<IjvxThreads_report*>(this));
					assert(resL == JVX_NO_ERROR);
				}
			}
			else
			{
				// Start the secondary thread
				resL = refThreads.cpPtr->initialize(
					static_cast<IjvxThreads_report*>(this));
				assert(resL == JVX_NO_ERROR);
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	}
	return res;
}

jvxErrorType
CjvxAuNForwardBuffer::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	if (bypass_buffer_mode)
	{
		res = CjvxBareNode1ioRearrange::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	else
	{
		if (buffermode == jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_INPUT)
		{
			if (_common_set_ldslave.theData_in->con_params.data_flow == jvxDataflow::JVX_DATAFLOW_PUSH_ON_PULL)
			{
				refThreads.cpPtr->terminate();
				assert(resL == JVX_NO_ERROR);
			}
		}
		else
		{
			refThreads.cpPtr->terminate();
			assert(resL == JVX_NO_ERROR);
		}

		// ========================================================================================================
		// Deallocate pre-sorting channel buffer and zero input buffer
		JVX_DSP_SAFE_DELETE_FIELD(reRouting.bufReroute);
		reRouting.bufReroute = nullptr;
		JVX_DSP_SAFE_DELETE_FIELD(reRouting.zbuf);
		reRouting.zbuf = nullptr;
		// ========================================================================================================

		stop_audiostack();

		res = _postprocess_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		// The only deviation may be the number of output channels - which is taken from the node parameter set
		_common_set_ldslave.theData_out.con_data.number_buffers = 0;

		res = deallocate_pipeline_and_buffers_postprocess_to();

		_common_set_ldslave.theData_in->con_data.number_buffers = 0;
		_common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages = 0;

		postprocess_autostart();

	}

	CjvxProperties::_undo_update_property_access_type(
		genForwardBuffer_node::config_select.bypass_buffer.category,
		genForwardBuffer_node::config_select.bypass_buffer.globalIdx);

	return res;
}

jvxErrorType
CjvxAuNForwardBuffer::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	if (bypass_buffer_mode)
	{
		res = CjvxBareNode1ioRearrange::start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	else
	{
		res = CjvxBareNode1ioRearrange::start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
			if (buffermode == jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_INPUT)
			{
				if (_common_set_ldslave.theData_in->con_params.data_flow == jvxDataflow::JVX_DATAFLOW_PUSH_ON_PULL)
				{
					// Start the secondary thread
					resL = refThreads.cpPtr->start();
					assert(resL == JVX_NO_ERROR);
				}
			}
			else
			{
				// Start the secondary thread
				resL = refThreads.cpPtr->start();
				assert(resL == JVX_NO_ERROR);
			}
		}
	}
	return res;
}

jvxErrorType
CjvxAuNForwardBuffer::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	if (bypass_buffer_mode)
	{
		res = CjvxBareNode1ioRearrange::stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	else
	{
		if (buffermode == jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_INPUT)
		{
			if (_common_set_ldslave.theData_in->con_params.data_flow == jvxDataflow::JVX_DATAFLOW_PUSH_ON_PULL)
			{
				// Start the secondary thread
				resL = refThreads.cpPtr->stop(5000);
				assert(resL == JVX_NO_ERROR);
			}
		}
		else
		{
			// Start the secondary thread
			resL = refThreads.cpPtr->stop(5000);
			assert(resL == JVX_NO_ERROR);
		}
		res = CjvxBareNode1ioRearrange::stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

// ===========================================================================

jvxErrorType
CjvxAuNForwardBuffer::process_start_icon(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (bypass_buffer_mode)
	{
		res = CjvxBareNode1ioRearrange::process_start_icon(
			pipeline_offset,
			idx_stage,
			tobeAccessedByStage,
			clbk,
			priv_ptr);
	}
	else
	{
		res = shift_buffer_pipeline_idx_on_start(pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	}
	return res;
}

jvxErrorType
CjvxAuNForwardBuffer::process_stop_icon(
	jvxSize idx_stage,
	jvxBool operate_first_call,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock cb,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (bypass_buffer_mode)
	{
		res = CjvxBareNode1ioRearrange::process_stop_icon(
			idx_stage,
			operate_first_call,
			tobeAccessedByStage,
			cb,
			priv_ptr);
	}
	else
	{
		res = shift_buffer_pipeline_idx_on_stop(
			idx_stage,
			operate_first_call,
			tobeAccessedByStage,
			cb,
			priv_ptr);
	}
	return res;
}

jvxErrorType
CjvxAuNForwardBuffer::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	// Run the decode microconnection
	jvxErrorType res = JVX_NO_ERROR;
	if (bypass_buffer_mode)
	{
		res = CjvxBareNode1ioRearrange::process_buffers_icon(mt_mask, idx_stage);
	}
	else
	{
		jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_ldslave.theData_in, idx_stage);

		// ========================================================================================================
		// Filter out the right amount of channels and run the channel selection. We sort the input buffers in a
		// different order in a dedicated buffer prior to passing them to the audio sync buffer.
		// ========================================================================================================
		for (int i = 0; i < node_output._common_set_node_params_a_1io.number_channels; i++)
		{
			jvxSize idx = jvx_bitfieldSelection2Id(reRouting.selChannels[i], 
				node_output._common_set_node_params_a_1io.number_channels);
			if (idx < node_output._common_set_node_params_a_1io.number_channels)
			{
				reRouting.bufReroute[i] = bufsIn[idx];
			}
			else
			{
				reRouting.bufReroute[i] = reRouting.zbuf;
			}
		}

		// ========================================================================================================
		// Pass data to the sync buffer
		// ========================================================================================================
		res = jvx_audio_stack_sample_dispenser_cont_external_copy(
			&myAudioDispenser,
			//(jvxHandle**)bufsIn, 0,
			(jvxHandle**)reRouting.bufReroute, 0,
			nullptr, 0, node_inout._common_set_node_params_a_1io.buffersize,
			0, nullptr, nullptr,
			nullptr, nullptr);

		// ========================================================================================================

		// If we are in OUTPUT_BUFFER MODE we will trigger the output thread
		if (buffermode == jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_OUTPUT)
		{
			if (res == JVX_NO_ERROR)
			{
				refThreads.cpPtr->trigger_wakeup();
			}
		}
	}
	return res;
}

jvxErrorType 
CjvxAuNForwardBuffer::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
		
		if (genForwardBuffer_node::config_select.bypass_buffer.value)
		{
			accept_input_parameters_stop(JVX_ERROR_UNSUPPORTED);
			if(_common_set_ldslave.theData_in->con_link.connect_from)
			{ 
				res = _common_set_ldslave.theData_in->con_link.connect_from->transfer_backward_ocon(
					tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res == JVX_NO_ERROR)
				{
					accept_input_parameters_start(JVX_CONNECTION_FEEDBACK_CALL(fdb));
					/*
					res = accept_negotiate_input(tp, ld JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

					if (res == JVX_NO_ERROR)
					{
						// There is only room to negotiate 
						res = neg_output._negotiate_transfer_backward_ocon(ld,
							&_common_set_ldslave.theData_out, static_cast<IjvxObject*>(this), NULL
							JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					}
					*/
					test_set_output_parameters();
				}
			}
		}
		else
		{
			// To do at this position
			res = CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (res == JVX_NO_ERROR)
			{
				// If new parameters were set, forward those to the current status variable for output
				update_output_params();
			}
		}
		return res;
		break;

	case JVX_LINKDATA_TRANSFER_REQUEST_DATA:
		if (genForwardBuffer_node::config_select.bypass_buffer.value)
		{
			if (_common_set_ldslave.theData_in->con_link.connect_from)
			{
				return CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
		}
		else
		{
			if (buffermode == jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_INPUT)
			{
				if (_common_set_ldslave.theData_out.con_link.connect_to)
				{
					res = _common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
					if (res == JVX_NO_ERROR)
					{
						assert(node_output._common_set_node_params_a_1io.number_channels == 
							_common_set_ldslave.theData_out.con_params.number_channels);
						assert(node_output._common_set_node_params_a_1io.buffersize ==
							_common_set_ldslave.theData_out.con_params.buffersize);

						// Get new audio data here
						jvxData** bufsOut = jvx_process_icon_extract_output_buffers<jvxData>(&_common_set_ldslave.theData_out);
						jvxSize fHeight = 0;
						jvxErrorType resL = jvx_audio_stack_sample_dispenser_cont_internal_copy(
							&myAudioDispenser,
							(jvxHandle**)bufsOut, 0,
							nullptr, 0, _common_set_ldslave.theData_out.con_params.buffersize, 0, nullptr, NULL, NULL, NULL);

						switch (resL)
						{
						case JVX_DSP_NO_ERROR:
							jvx_audio_stack_sample_dispenser_buffer_status(
								&myAudioDispenser,
								&fHeight, nullptr);
							if (fHeight < myAudioDispenser.start_threshold)
							{
								refThreads.cpPtr->trigger_wakeup();
							}

							res = JVX_NO_ERROR;
							break;
						case JVX_DSP_ERROR_ABORT:
							res = JVX_NO_ERROR;
							break;
						case JVX_DSP_ERROR_BUFFER_OVERFLOW:
							res = JVX_NO_ERROR;
							break;
						default:
							break;
						}

						_common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();
						_common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
					}
				}
			}
		}
		break;
	default:
		res = CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

// ===================================================================
// ===================================================================

jvxErrorType
CjvxAuNForwardBuffer::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = JVX_NO_ERROR;
	
	return res;
}

jvxErrorType
CjvxAuNForwardBuffer::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	std::vector<std::string> warns;
	jvxErrorType res = JVX_NO_ERROR;
	
	return res;
}

// ================================================================================

jvxErrorType
CjvxAuNForwardBuffer::startup(jvxInt64 timestamp_us)
{
	if (buffermode == jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_INPUT)
	{
		read_samples_to_buffer();
	}
	
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNForwardBuffer::expired(jvxInt64 timestamp_us, jvxSize* delta_ms) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNForwardBuffer::wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms) 
{
	if (buffermode == jvxOperationMode::JVX_FORWARDBUFFER_BUFFER_INPUT)
	{
		read_samples_to_buffer();
	}
	else
	{
		write_samples_to_buffer();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNForwardBuffer::stopped(jvxInt64 timestamp_us) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuNForwardBuffer::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_INTERFACE_MANIPULATE:
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxManipulate*>(this));
		break;
	default:
		res = CjvxBareNode1ioRearrange::request_hidden_interface(tp, hdl);
	}
	return res;
}

jvxErrorType 
CjvxAuNForwardBuffer::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_INTERFACE_MANIPULATE:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxManipulate*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	default:
		res = CjvxBareNode1ioRearrange::return_hidden_interface(tp, hdl);
	}
	return res;
}

void
CjvxAuNForwardBuffer::read_samples_to_buffer()
{
	jvxSize space = 0;
	while (1)
	{
		jvx_audio_stack_sample_dispenser_buffer_status(&myAudioDispenser,
			nullptr,
			&space);
		if (space >= node_output._common_set_node_params_a_1io.buffersize)
		{
			_common_set_ldslave.theData_in->con_link.connect_from->transfer_backward_ocon(
				JVX_LINKDATA_TRANSFER_REQUEST_DATA, nullptr JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
		}
		else
		{
			break;
		}
	}
}

void
CjvxAuNForwardBuffer::write_samples_to_buffer()
{
	if (_common_set_ldslave.theData_out.con_link.connect_to)
	{
		while(1)
		{
			jvxSize fHeight = 0;
			jvx_audio_stack_sample_dispenser_buffer_status(
				&myAudioDispenser,
				&fHeight, nullptr);
			if (fHeight >= _common_set_ldslave.theData_out.con_params.buffersize)
			{
				jvxErrorType res = _common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
				if (res == JVX_NO_ERROR)
				{
					// Get new audio data here
					jvxData** bufsOut = jvx_process_icon_extract_output_buffers<jvxData>(&_common_set_ldslave.theData_out);

					assert(node_output._common_set_node_params_a_1io.buffersize == 
						_common_set_ldslave.theData_out.con_params.buffersize);

					jvxErrorType resL = jvx_audio_stack_sample_dispenser_cont_internal_copy(
						&myAudioDispenser,
						(jvxHandle**)bufsOut, 0,
						nullptr, 0, _common_set_ldslave.theData_out.con_params.buffersize, 0, nullptr, NULL, NULL, NULL);
					assert(resL == JVX_NO_ERROR);

					_common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();
					_common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
}

jvxErrorType
CjvxAuNForwardBuffer::set_manipulate_value(jvxSize id, jvxVariant* varray)
{
	jvxApiString* ptrString = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	if (!varray)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	switch (id)
	{
	case JVX_MANIPULATE_DESCRIPTION:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			_common_set_min.theDescription = ptrString->std_str();
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	case JVX_MANIPULATE_DESCRIPTOR:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			_common_set_min.theDescription = ptrString->std_str();
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

jvxErrorType
CjvxAuNForwardBuffer::get_manipulate_value(jvxSize id, jvxVariant* varray)
{
	jvxApiString* ptrString = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	if (!varray)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	switch (id)
	{
	case JVX_MANIPULATE_DESCRIPTION:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			ptrString->assign(_common_set_min.theDescription);
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	case JVX_MANIPULATE_DESCRIPTOR:
		varray->getApiString(&ptrString);
		if (ptrString)
		{
			ptrString->assign(_common_set_min.theDescription);
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

void 
CjvxAuNForwardBuffer::update_output_params()
{
	/*
	output.buffersize = node_output._common_set_node_params_a_1io.buffersize;
	output.samplerate = node_output._common_set_node_params_a_1io.samplerate;
	output.number_channels = node_output._common_set_node_params_a_1io.number_channels;
	output.format = (jvxDataFormat)node_output._common_set_node_params_a_1io.format;
	*/
}

// =================================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNForwardBuffer, get_processing_monitor)
{
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNForwardBuffer, set_bypass_buffer)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genForwardBuffer_node::config_select.bypass_buffer))
	{
		inform_chain_test(false);
	}
	return JVX_NO_ERROR;
}

// =================================================================================

CjvxAudioStackBuffer::CjvxAudioStackBuffer()
{
	JVX_INITIALIZE_MUTEX(safeAccess_lock);
}

CjvxAudioStackBuffer::~CjvxAudioStackBuffer()
{
	JVX_TERMINATE_MUTEX(safeAccess_lock);
}

jvxDspBaseErrorType
CjvxAudioStackBuffer::lock_sync_threads_cb(jvxHandle* prv)
{
	if (prv)
	{
		CjvxAudioStackBuffer* this_p = (CjvxAudioStackBuffer*)prv;
		return this_p->lock_sync_threads_ic();
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
CjvxAudioStackBuffer::lock_sync_threads_ic()
{
	JVX_LOCK_MUTEX(safeAccess_lock);
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
CjvxAudioStackBuffer::unlock_sync_threads_cb(jvxHandle* prv)
{
	if (prv)
	{
		CjvxAudioStackBuffer* this_p = (CjvxAudioStackBuffer*)prv;
		return this_p->unlock_sync_threads_ic();
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}
jvxDspBaseErrorType
CjvxAudioStackBuffer::unlock_sync_threads_ic()
{
	JVX_UNLOCK_MUTEX(safeAccess_lock);
	return JVX_DSP_NO_ERROR;
}

void
CjvxAudioStackBuffer::start_audiostack(
	jvxSize num_samples_buffer,
	jvxSize number_input_channels,
	jvxDataFormat format,
	jvxBool withFHeightEstimator, 
	jvxBool withStartThreshold)
{
	jvxSize i;
	jvxErrorType resL = JVX_NO_ERROR;
	myAudioDispenser.buffersize = num_samples_buffer;
	myAudioDispenser.form = format;
	myAudioDispenser.numChannelsFromExternal = number_input_channels;
	myAudioDispenser.numChannelsToExternal = 0;

	myAudioDispenser.imm_start = c_true;
	myAudioDispenser.switch_on_buffer_full = c_false;

	myAudioDispenser.callbacks.prv_callbacks = reinterpret_cast<jvxHandle*>(this);
	myAudioDispenser.callbacks.lock_callback = lock_sync_threads_cb;
	myAudioDispenser.callbacks.unlock_callback = unlock_sync_threads_cb;

	myAudioDispenser.start_threshold = 0;
	if (withStartThreshold)
	{
		myAudioDispenser.start_threshold = myAudioDispenser.buffersize / 2;
	}

	if (withFHeightEstimator)
	{
		resL = jvx_estimate_buffer_fillheight_init(
			&fHeightEstimator,
			JVX_SIZE_INT32(numberEventsConsidered_perMinMaxSection),
			JVX_SIZE_INT32(num_MinMaxSections),
			recSmoothFactor,
			JVX_SIZE_INT32(numOperations));

		jvx_estimate_buffer_fillheight_restart(fHeightEstimator);
	}
	
	work_buffers_from_hw = nullptr;
	if (myAudioDispenser.numChannelsFromExternal)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(work_buffers_from_hw, jvxHandle*, myAudioDispenser.numChannelsFromExternal);
		for (i = 0; i < myAudioDispenser.numChannelsFromExternal; i++)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(work_buffers_from_hw[i], jvxData, myAudioDispenser.buffersize);
		}
	}

	work_buffers_to_hw = nullptr;
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

	jvx_audio_stack_sample_dispenser_cont_prepare(&myAudioDispenser, nullptr);
	assert(resL == JVX_DSP_NO_ERROR);
}

void
CjvxAudioStackBuffer::stop_audiostack()
{
	jvxSize i;
	jvxErrorType resL = jvx_audio_stack_sample_dispenser_cont_postprocess(&myAudioDispenser);
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

	if (fHeightEstimator)
	{
		resL = jvx_estimate_buffer_fillheight_terminate(fHeightEstimator);
		assert(resL == JVX_DSP_NO_ERROR);
	}
}

