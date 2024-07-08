#include "jvx.h"
#include "jvxNodes/CjvxBareNode1io.h"

#define JVX_NODE_TYPE_SPECIFIER_TYPE_DEFAULT JVX_COMPONENT_AUDIO_NODE
#define JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR_DEFAULT "signal_processing_node/audio_node"

CjvxBareNode1io::CjvxBareNode1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxNodeBase1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	if (_common_set.theComponentType.tp == JVX_COMPONENT_UNKNOWN)
	{
		_common_set.theComponentType.unselected(JVX_NODE_TYPE_SPECIFIER_TYPE_DEFAULT);
	}

	if (_common_set.theComponentSubTypeDescriptor.empty())
	{
		_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR_DEFAULT;
	}
	_common_set_ldslave.zeroCopyBuffering_cfg = true;

	_common_set_node_base_1io.prepareOnChainPrepare = true;
	_common_set_node_base_1io.startOnChainStart = true;
	impPrepareOnChainPrepare = false;
	impStartOnChainStart = false;

	allowZeroCopyOnCondition = true;
}

jvxErrorType
CjvxBareNode1io::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);
	jvxErrorType resF = JVX_NO_ERROR;
	jvxErrorType res = JVX_ERROR_WRONG_STATE;

	prepare_autostart();

	if (_common_set_min.theState == JVX_STATE_PREPARED)
	{
		// The number of pipeline stages may be increased from element to element
		_common_set_icon.theData_in->con_pipeline.num_additional_pipleline_stages = JVX_MAX(
			_common_set_icon.theData_in->con_pipeline.num_additional_pipleline_stages,
			_common_set_ldslave.num_additional_pipleline_stages);

		// The number of buffers is always lower bounded by the add pipeline stages
		_common_set_icon.theData_in->con_data.number_buffers = JVX_MAX(
			_common_set_icon.theData_in->con_data.number_buffers,
			1 + _common_set_icon.theData_in->con_pipeline.num_additional_pipleline_stages);

		// We might specify another additional lower limit for the buffersHallo Frau 
		_common_set_icon.theData_in->con_data.number_buffers = JVX_MAX(
			_common_set_icon.theData_in->con_data.number_buffers,
			_common_set_ldslave.num_min_buffers_in);

		// Do the processing checks and allocate buffers etc
		// Setup has been verified in the test chain functions before - this is only for simpler access during processing
		assert(node_inout._common_set_node_params_a_1io.buffersize == _common_set_icon.theData_in->con_params.buffersize);
		assert(node_inout._common_set_node_params_a_1io.number_channels == _common_set_icon.theData_in->con_params.number_channels);
		assert(node_inout._common_set_node_params_a_1io.samplerate == _common_set_icon.theData_in->con_params.rate);
		assert(node_inout._common_set_node_params_a_1io.format == _common_set_icon.theData_in->con_params.format);
		assert(_common_set_ocon.theData_out.con_data.buffers == NULL);

		
		if (allowZeroCopyOnCondition)
		{
			if (check_positive_zero_copy())
			{
				zeroCopyBuffering_rt = _common_set_ldslave.zeroCopyBuffering_cfg;
			}
			else
			{
				zeroCopyBuffering_rt = false;
			}
		}
		else
		{
			zeroCopyBuffering_rt = false;
		}

		if (
			(jvx_bitTest(_common_set_icon.theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT)) ||
			(jvx_bitTest(_common_set_icon.theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT)))
		{
			zeroCopyBuffering_rt = false;
		}

		if (zeroCopyBuffering_rt)
		{
			// If we have a termination here we need to allocate buffers
			if (_common_set_ocon.setup_for_termination)
			{
				zeroCopyBuffering_rt = false;
			}
		}

		if (zeroCopyBuffering_rt)
		{
			// Set the number of buffers on the output identical to the input side
			_common_set_ocon.theData_out.con_data.number_buffers = _common_set_icon.theData_in->con_data.number_buffers;
			_common_set_ocon.theData_out.con_pipeline.num_additional_pipleline_stages = _common_set_icon.theData_in->con_pipeline.num_additional_pipleline_stages;
			_common_set_ocon.theData_out.con_data.alloc_flags = _common_set_icon.theData_in->con_data.alloc_flags;
			_common_set_ocon.theData_out.con_sync.type_timestamp = _common_set_icon.theData_in->con_sync.type_timestamp;

			///jvx_bitClear(_common_set_ocon.theData_out.con_data.alloc_flags, JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT)
		}
		else
		{
			// Set the number of buffers as desired
			_common_set_ocon.theData_out.con_data.number_buffers = _common_set_ldslave.num_min_buffers_out;
			_common_set_ocon.theData_out.con_data.alloc_flags = _common_set_icon.theData_in->con_data.alloc_flags;
			jvx_bitClear(_common_set_icon.theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT);
			jvx_bitClear(_common_set_icon.theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT);
			_common_set_ocon.theData_out.con_pipeline.num_additional_pipleline_stages = 0;
			_common_set_ocon.theData_out.con_sync.type_timestamp = _common_set_icon.theData_in->con_sync.type_timestamp;
		}

		// We might attach user hints here!!!
		// _common_set_ocon.theData_out.con_data.chain_spec_user_hints = _common_set_icon.theData_in->con_data.user_hints;

		res = _prepare_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		if (res == JVX_NO_ERROR)
		{
			if (zeroCopyBuffering_rt)
			{
				// Link the buffers directly to output
#if 0
				res = allocate_pipeline_and_buffers_prepare_to_zerocopy();
				assert(res == JVX_NO_ERROR);
#else
				_common_set_icon.theData_in->con_data = _common_set_ocon.theData_out.con_data;
				_common_set_icon.theData_in->con_pipeline = _common_set_ocon.theData_out.con_pipeline;
				_common_set_icon.theData_in->con_sync = _common_set_ocon.theData_out.con_sync;

#endif
				if (jvx_bitTest(_common_set_ocon.theData_out.con_data.alloc_flags, 
					(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT))
				{
					jvx_bitSet(_common_set_icon.theData_in->con_data.alloc_flags, 
						(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);
				}
			}
			else
			{
				res = allocate_pipeline_and_buffers_prepare_to();
				assert(res == JVX_NO_ERROR);
				jvx_bitClear(_common_set_icon.theData_in->con_data.alloc_flags, 
					(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);
			}
		}
		else
		{
			goto exit_error;
		}
	}
	
	return res;
exit_error:
	if (impPrepareOnChainPrepare)
	{
		if (_common_set_min.theState == JVX_STATE_PREPARED)
		{
			// Call of implicit prepare
			this->_postprocess();
		}
	}
	impPrepareOnChainPrepare = false;
	return res;

};

jvxErrorType
CjvxBareNode1io::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idRuntime = JVX_SIZE_UNSELECTED;
	if (_common_set.theUniqueId)
	{
		_common_set.theUniqueId->obtain_unique_id(&idRuntime, _common_set.theDescriptor.c_str());
	}

	start_autostart();

	res = _start_connect_icon(true, idRuntime JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res != JVX_NO_ERROR)
	{
		if (impStartOnChainStart)
		{
			if (_common_set_min.theState == JVX_STATE_PROCESSING)
			{
				this->_stop();
			}
		}
		impStartOnChainStart = false;
	}
	
	return res;
};

jvxErrorType
CjvxBareNode1io::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idRuntime = JVX_SIZE_UNSELECTED;
	res = _stop_connect_icon(true, &idRuntime JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	if (_common_set.theUniqueId)
	{
		_common_set.theUniqueId->release_unique_id(idRuntime);
	}
	stop_autostart();
	return res;
};

jvxErrorType
CjvxBareNode1io::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool forward = true;
	if (_common_set_min.theState == JVX_STATE_PREPARED)
	{
		res = _postprocess_connect_icon(forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		// We might remove user hints here!!!
		// _common_set_ocon.theData_out.con_data.chain_spec_user_hints = NULL;

		// The only deviation may be the number of output channels - which is taken from the node parameter set
		_common_set_ocon.theData_out.con_data.number_buffers = 0;

		if (zeroCopyBuffering_rt)
		{
			jvx_neutralDataLinkDescriptor_data(&_common_set_icon.theData_in->con_data);
			jvx_neutralDataLinkDescriptor_pipeline(&_common_set_icon.theData_in->con_pipeline);
			jvx_neutralDataLinkDescriptor_sync(&_common_set_icon.theData_in->con_sync);
#if 0
			res = deallocate_pipeline_and_buffers_postprocess_to_zerocopy();
#endif
		}
		else
		{
			res = deallocate_pipeline_and_buffers_postprocess_to();
		}

		_common_set_icon.theData_in->con_data.number_buffers = 0;
		_common_set_icon.theData_in->con_pipeline.num_additional_pipleline_stages = 0;

		zeroCopyBuffering_rt = false;

		postprocess_autostart();

	}

	return res;
}

jvxErrorType 
CjvxBareNode1io::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (zeroCopyBuffering_rt)
	{
		if (_common_set_ocon.ocon)
		{
			res = _common_set_ocon.ocon->process_start_ocon(pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
			return res;
		}
		return JVX_ERROR_NOT_READY;
	}
	return _process_start_icon( pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
}

jvxErrorType
CjvxBareNode1io::process_stop_icon(
	jvxSize idx_stage, 
	jvxBool operate_first_call,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr )
{
	jvxErrorType res = JVX_NO_ERROR;
	if (zeroCopyBuffering_rt)
	{
		if (_common_set_ocon.ocon)
		{
			res = _common_set_ocon.ocon->process_stop_ocon(idx_stage, operate_first_call, tobeAccessedByStage,
				clbk, priv_ptr);
			return res;
		}
		return JVX_ERROR_NOT_READY;
	}
	return _process_stop_icon(idx_stage, operate_first_call, tobeAccessedByStage, clbk, priv_ptr);
}

jvxErrorType 
CjvxBareNode1io::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

#ifdef VERBOSE_BUFFERING_REPORT
	jvxSize i;
	jvxApiString str;
	jvxSize cs = JVX_SIZE_UNSELECTED;
	std::cout << "Node input <-> output, bidx = " << _common_set_icon.theData_in->pipeline.idx_stage << "/"
		<< _common_set_icon.theData_in->con_data.number_buffers
		<< "<->" << _common_set_ocon.theData_out.pipeline.idx_stage << "/"
		<< _common_set_ocon.theData_out.con_data.number_buffers << std::endl;
	std::cout << "Input pipeline:" << std::endl;
	for (i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
	{
		cs = _common_set_icon.theData_in->con_pipeline.reserve_buffer_pipeline_stage[i];
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
	for (i = 0; i < _common_set_ocon.theData_out.con_data.number_buffers; i++)
	{
		cs = _common_set_ocon.theData_out.con_pipeline.reserve_buffer_pipeline_stage[i];
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
		if (!_common_set_ocon.setup_for_termination)
		{
			// Talkthrough
			jvxSize ii;
			jvxSize maxChans = JVX_MAX(_common_set_icon.theData_in->con_params.number_channels, _common_set_ocon.theData_out.con_params.number_channels);
			jvxSize minChans = JVX_MIN(_common_set_icon.theData_in->con_params.number_channels, _common_set_ocon.theData_out.con_params.number_channels);

			// This default function does not tolerate a lot of unexpected settings
			assert(_common_set_icon.theData_in->con_params.format == _common_set_ocon.theData_out.con_params.format);
			assert(_common_set_icon.theData_in->con_params.buffersize == _common_set_ocon.theData_out.con_params.buffersize);

			if (minChans)
			{
				for (ii = 0; ii < maxChans; ii++)
				{
					jvxSize idxIn = ii % _common_set_icon.theData_in->con_params.number_channels;
					jvxSize idxOut = ii % _common_set_ocon.theData_out.con_params.number_channels;

					assert(_common_set_icon.theData_in->con_params.format == _common_set_ocon.theData_out.con_params.format);
					jvx_convertSamples_memcpy(
						_common_set_icon.theData_in->con_data.buffers[
							*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][idxIn],
						_common_set_ocon.theData_out.con_data.buffers[
							*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][idxOut],
						jvxDataFormat_size[_common_set_icon.theData_in->con_params.format],
						_common_set_icon.theData_in->con_params.buffersize);
				}
			}

			if (res != JVX_NO_ERROR)
			{
				return res;
			}
		}
		else
		{
			// No output buffers exist due to chain termination
		}
	}
	// Forward to next chain
	return _process_buffers_icon(mt_mask, idx_stage);
};

void 
CjvxBareNode1io::prepare_autostart()
{
	impPrepareOnChainPrepare = false;
	if (_common_set_node_base_1io.prepareOnChainPrepare)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			if (_common_set_node_base_1io.involveStateSwitchHost)
			{
				IjvxHost* hh = reqInterface<IjvxHost>(_common_set_min.theHostRef);
				if (hh)
				{
					hh->switch_state_component(_common_set.theComponentType, JVX_STATE_SWITCH_PREPARE);
				}
				else
				{
					prepare();
				}
			}
			else
			{
				// Call of implicit prepare - simple
				this->prepare();
			}
			impPrepareOnChainPrepare = true;
		}
	}
}

void
CjvxBareNode1io::postprocess_autostart()
{
	if (impPrepareOnChainPrepare)
	{
		if (_common_set_min.theState == JVX_STATE_PREPARED)
		{
			if (_common_set_node_base_1io.involveStateSwitchHost)
			{
				IjvxHost* hh = reqInterface<IjvxHost>(_common_set_min.theHostRef);
				if (hh)
				{
					hh->switch_state_component(_common_set.theComponentType, JVX_STATE_SWITCH_POSTPROCESS);
				}
				else
				{
					postprocess();
				}
			}
			else
			{
				// Call of implicit postprocess
				this->postprocess();
			}
		}
	}
	impPrepareOnChainPrepare = false;
}

void
CjvxBareNode1io::start_autostart()
{
	impStartOnChainStart = false;
	if (_common_set_node_base_1io.startOnChainStart)
	{
		if (_common_set_min.theState == JVX_STATE_PREPARED)
		{
			if (_common_set_node_base_1io.involveStateSwitchHost)
			{
				IjvxHost* hh = reqInterface<IjvxHost>(_common_set_min.theHostRef);
				if (hh)
				{
					hh->switch_state_component(_common_set.theComponentType, JVX_STATE_SWITCH_START);
				}
				else
				{
					start();
				}
			}
			else
			{
				// Call of implicit start
				this->start();
			}
			impStartOnChainStart = true;
		}
	}
}

void
CjvxBareNode1io::stop_autostart()
{
	if (impStartOnChainStart)
	{
		if (_common_set_min.theState == JVX_STATE_PROCESSING)
		{
			if (_common_set_node_base_1io.involveStateSwitchHost)
			{
				IjvxHost* hh = reqInterface<IjvxHost>(_common_set_min.theHostRef);
				if (hh)
				{
					hh->switch_state_component(_common_set.theComponentType, JVX_STATE_SWITCH_STOP);
				}
				else
				{
					stop();
				}
			}
			else
			{
				this->stop();
			}
		}
	}
	impStartOnChainStart = false;
}

jvxErrorType 
CjvxBareNode1io::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) 
{
	jvxErrorType res = _disconnect_connect_icon(theData, true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		node_inout.reset();
	}
	return res;
};

// We check if zerocopy mode is allowed! Typically, input and output must be identical.
// ORIGINAL COMMENT: The only deviation from the input side may be the number of output channels - which is taken from the node parameter set
// EXTENSION HK, 07.06.2024: I can not remember why a different number of output channels was meant to be accepted before. 		
// Components may re-define this check if they want to do whatever
bool CjvxBareNode1io::check_positive_zero_copy()
{
	return (
		(_common_set_ocon.theData_out.con_params.buffersize == node_inout._common_set_node_params_a_1io.buffersize) &&
		(_common_set_ocon.theData_out.con_params.format == node_inout._common_set_node_params_a_1io.format) &&
		(_common_set_ocon.theData_out.con_params.rate == node_inout._common_set_node_params_a_1io.samplerate) &&
		(_common_set_ocon.theData_out.con_params.number_channels == node_inout._common_set_node_params_a_1io.number_channels));
}