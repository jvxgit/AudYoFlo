#include "jvx.h"
#include "jvxAudioNodes/CjvxBareAudioNode.h"


CjvxBareAudioNode::CjvxBareAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioNodeBase(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set_ldslave.zeroCopyBuffering_cfg = true;
}

jvxErrorType
CjvxBareAudioNode::activate()
{
	jvxErrorType res = CjvxAudioNodeBase::activate();

	if (res == JVX_NO_ERROR)
	{
		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this), "default", NULL, _common_set.theModuleName.c_str());
	}
	return(res);
};

jvxErrorType
CjvxBareAudioNode::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);
	jvxErrorType resF = JVX_NO_ERROR;
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_PREPARED)
	{
		// Accepts and modify some parameters
		_common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages = _common_set_ldslave.num_additional_pipleline_stages;
		_common_set_ldslave.theData_in->con_data.number_buffers = JVX_MAX(
			_common_set_ldslave.theData_in->con_data.number_buffers + _common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages,
			_common_set_ldslave.num_min_buffers_in);

		// Do the processing checks and allocate buffers etc
		// Setup has been verified in the test chain functions before - this is only for simpler access during processing
		_common_set_node_params_1io.processing.buffersize = _common_set_ldslave.theData_in->con_params.buffersize;
		_common_set_node_params_1io.processing.number_input_channels = _common_set_ldslave.theData_in->con_params.number_channels;
		_common_set_node_params_1io.processing.number_output_channels = _common_set_ldslave.theData_out.con_params.number_channels;
		_common_set_node_params_1io.processing.samplerate = _common_set_ldslave.theData_in->con_params.rate;
		_common_set_node_params_1io.processing.format = _common_set_ldslave.theData_in->con_params.format;

		// Prepare next processing stage processing
		// The only deviation from the input side may be the number of output channels - which is taken from the node parameter set
		assert(_common_set_ldslave.theData_out.con_params.buffersize == _common_set_node_params_1io.processing.buffersize);
		assert(_common_set_ldslave.theData_out.con_params.format == _common_set_node_params_1io.processing.format);
		assert(_common_set_ldslave.theData_out.con_data.buffers == NULL);
		assert(_common_set_ldslave.theData_out.con_params.number_channels == _common_set_node_params_1io.processing.number_output_channels);
		assert(_common_set_ldslave.theData_out.con_params.rate == _common_set_node_params_1io.processing.samplerate);

		zeroCopyBuffering_rt = _common_set_ldslave.zeroCopyBuffering_cfg;
		if (
			(jvx_bitTest(_common_set_ldslave.theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT) )||
			(jvx_bitTest(_common_set_ldslave.theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT)))
		{
			zeroCopyBuffering_rt = false;
		}

		if (!
			(
			(_common_set_ldslave.theData_out.con_params.number_channels == _common_set_ldslave.theData_in->con_params.number_channels) &&
				(_common_set_ldslave.theData_out.con_params.rate == _common_set_ldslave.theData_in->con_params.rate) &&
				(_common_set_ldslave.theData_out.con_params.buffersize == _common_set_ldslave.theData_in->con_params.buffersize) &&
				(_common_set_ldslave.theData_out.con_params.format == _common_set_ldslave.theData_in->con_params.format)))
		{
			zeroCopyBuffering_rt = false;
		}

		if (zeroCopyBuffering_rt)
		{
			// Set the number of buffers on the output identical to the input side
			_common_set_ldslave.theData_out.con_data.number_buffers = _common_set_ldslave.theData_in->con_data.number_buffers;
			_common_set_ldslave.theData_out.con_data.alloc_flags = _common_set_ldslave.theData_in->con_data.alloc_flags;
			///jvx_bitClear(_common_set_ldslave.theData_out.con_data.alloc_flags, JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT)
		}
		else
		{
			// Set the number of buffers as desired
			_common_set_ldslave.theData_out.con_data.number_buffers = _common_set_ldslave.num_min_buffers_out;
			_common_set_ldslave.theData_out.con_data.alloc_flags = _common_set_ldslave.theData_in->con_data.alloc_flags;
			jvx_bitClear(_common_set_ldslave.theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT);
			jvx_bitClear(_common_set_ldslave.theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT);
		}

		// We might attach user hints here!!!
		//_common_set_ldslave.theData_out.con_data.user_hints = _common_set_ldslave.theData_in->con_data.user_hints;

		res = _prepare_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		
		if (res == JVX_NO_ERROR)
		{
			if (zeroCopyBuffering_rt)
			{
				// Link the buffers directly to output
				res = allocate_pipeline_and_buffers_prepare_to_zerocopy();
				assert(res == JVX_NO_ERROR);
				if (jvx_bitTest(_common_set_ldslave.theData_out.con_data.alloc_flags, 
					(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT))
				{
					jvx_bitSet(_common_set_ldslave.theData_in->con_data.alloc_flags, 
						(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);
				}
			}
			else
			{
				res = allocate_pipeline_and_buffers_prepare_to();
				assert(res == JVX_NO_ERROR);
				jvx_bitClear(_common_set_ldslave.theData_in->con_data.alloc_flags, 
					(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);
			}
		}
	}

	return res;
};

jvxErrorType
CjvxBareAudioNode::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool forward = true;
	if (_common_set_min.theState == JVX_STATE_PREPARED)
	{
		res = _postprocess_connect_icon(forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		// We might remove user hints here!!!
		//_common_set_ldslave.theData_out.con_data.user_hints = NULL;

		// The only deviation may be the number of output channels - which is taken from the node parameter set
		_common_set_ldslave.theData_out.con_data.number_buffers = 0;

		if (zeroCopyBuffering_rt)
		{
			res = deallocate_pipeline_and_buffers_postprocess_to_zerocopy();
		}
		else
		{
			res = deallocate_pipeline_and_buffers_postprocess_to();
		}

		_common_set_ldslave.theData_in->con_data.number_buffers = 0;
		_common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages = 0;

		_common_set_node_params_1io.processing.buffersize = JVX_SIZE_UNSELECTED;
		_common_set_node_params_1io.processing.number_input_channels = JVX_SIZE_UNSELECTED;
		_common_set_node_params_1io.processing.number_output_channels = JVX_SIZE_UNSELECTED;
		_common_set_node_params_1io.processing.samplerate = JVX_SIZE_UNSELECTED;
		_common_set_node_params_1io.processing.format = JVX_DATAFORMAT_NONE;

		zeroCopyBuffering_rt = false;
	}

	return res;
}

jvxErrorType 
CjvxBareAudioNode::process_start_icon(
	jvxSize pipeline_offset, 
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	if (zeroCopyBuffering_rt)
	{
		if (_common_set_ldslave.ocon)
		{
			return _common_set_ldslave.ocon->process_start_ocon(pipeline_offset,  idx_stage, tobeAccessedByStage, clbk, 
				priv_ptr);
		}
		return JVX_ERROR_NOT_READY;
	}
	return _process_start_icon( pipeline_offset,  idx_stage, tobeAccessedByStage, clbk, priv_ptr);
}

jvxErrorType
CjvxBareAudioNode::process_stop_icon(jvxSize idx_stage, 
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	if (zeroCopyBuffering_rt)
	{
		if (_common_set_ldslave.ocon)
		{
			return _common_set_ldslave.ocon->process_stop_ocon(
				idx_stage, shift_fwd, 
				tobeAccessedByStage, clbk, priv_ptr);
		}
		return JVX_ERROR_NOT_READY;
	}
	return _process_stop_icon( idx_stage, shift_fwd);
}

jvxErrorType 
CjvxBareAudioNode::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

#ifdef VERBOSE_BUFFERING_REPORT
	jvxSize i;
	jvxApiString str;
	jvxSize cs = JVX_SIZE_UNSELECTED;
	std::cout << "Node input <-> output, bidx = " << _common_set_ldslave.theData_in->pipeline.idx_stage << "/"
		<< _common_set_ldslave.theData_in->con_data.number_buffers
		<< "<->" << _common_set_ldslave.theData_out.pipeline.idx_stage << "/"
		<< _common_set_ldslave.theData_out.con_data.number_buffers << std::endl;
	std::cout << "Input pipeline:" << std::endl;
	for (i = 0; i < _common_set_ldslave.theData_in->con_data.number_buffers; i++)
	{
		cs = _common_set_ldslave.theData_in->con_pipeline.reserve_buffer_pipeline_stage[i];
		if (JVX_CHECK_SIZE_SELECTED(cs))
		{
			_common_set.theToolsHost->descriptor_unique_id(cs, &str);
			std::cout << "[is#" << i << "->" << str.std_str() << "]" << std::flush;
		}
		else
		{
			std::cout << "[is#" << i << "->*]" << std::flush;
		}
	}
	std::cout << std::endl;
	std::cout << "Output pipeline:" << std::endl;
	for (i = 0; i < _common_set_ldslave.theData_out.con_data.number_buffers; i++)
	{
		cs = _common_set_ldslave.theData_out.con_pipeline.reserve_buffer_pipeline_stage[i];
		if (JVX_CHECK_SIZE_SELECTED(cs))
		{
			_common_set.theToolsHost->descriptor_unique_id(cs, &str);
			std::cout << "[os#" << i << "->" << str.std_str() << "]" << std::flush;
		}
		else
		{
			std::cout << "[os#" << i << "->*]" << std::flush;
		}
	}
	std::cout << std::endl;
#endif

	if (!zeroCopyBuffering_rt)
	{

		// Talkthrough
		jvxSize ii;
		jvxSize maxChans = JVX_MAX(_common_set_ldslave.theData_in->con_params.number_channels, _common_set_ldslave.theData_out.con_params.number_channels);
		jvxSize minChans = JVX_MIN(_common_set_ldslave.theData_in->con_params.number_channels, _common_set_ldslave.theData_out.con_params.number_channels);

		// This default function does not tolerate a lot of unexpected settings
		assert(_common_set_ldslave.theData_in->con_params.format == _common_set_ldslave.theData_out.con_params.format);
		assert(_common_set_ldslave.theData_in->con_params.buffersize == _common_set_ldslave.theData_out.con_params.buffersize);

		if (minChans)
		{
			for (ii = 0; ii < maxChans; ii++)
			{
				jvxSize idxIn = ii % _common_set_ldslave.theData_in->con_params.number_channels;
				jvxSize idxOut = ii % _common_set_ldslave.theData_out.con_params.number_channels;

				assert(_common_set_ldslave.theData_in->con_params.format == _common_set_ldslave.theData_out.con_params.format);
				jvx_convertSamples_memcpy(
					_common_set_ldslave.theData_in->con_data.buffers[
						*_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr][idxIn],
					_common_set_ldslave.theData_out.con_data.buffers[
						*_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr][idxOut],
					jvxDataFormat_size[_common_set_ldslave.theData_in->con_params.format],
					_common_set_ldslave.theData_in->con_params.buffersize);
			}
		}

		if (res != JVX_NO_ERROR)
		{
			return res;
		}
	}
	// Forward to next chain
	return _process_buffers_icon( mt_mask,  idx_stage);
};
