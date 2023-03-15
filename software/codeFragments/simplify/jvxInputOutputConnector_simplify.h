#ifndef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
protected:
	jvxCommonSetNodeAutoStart _common_set_autostart;
#endif

public: 

#ifndef JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE
#define JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE static_cast<IjvxObject*>(this)
#define JVX_INPUTOUTPUT_CONNECTOR_UNDEFINE_OBJECT_REFERENCE
#endif

#ifndef JVX_CONNECTION_MASTER_SKIP_CONNECT_CONNECT_ICON
virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
{
	jvxBool forward = true;

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	forward = false;
#endif
	return _connect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb), forward);
};
#endif

#ifndef JVX_CONNECTION_MASTER_SKIP_CONNECT_CONNECT_OCON
virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ocon(const jvxChainConnectArguments& args  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
{
	// Forward user hints
	return _connect_connect_ocon(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}
#endif

// ===============================================================================

#ifndef JVX_CONNECTION_MASTER_SKIP_DISCONNECT_CONNECT_ICON
virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
{
	jvxBool forward = true;
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	forward = false;
#endif
	return _disconnect_connect_icon(theData, forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

};
#endif

#ifndef JVX_CONNECTION_MASTER_SKIP_DISCONNECT_CONNECT_OCON
virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
{
	return _disconnect_connect_ocon(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}
#endif

// ===============================================================================
#ifndef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
{
	jvxBool forward = true;
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	forward = false;
#endif
	return _test_connect_icon(forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
};
#endif

#ifndef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_OCON
virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
{
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE);

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_SET_OUTPUT_PARAMETERS
	this->test_set_output_parameters();
#endif

	JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_O(fdb, (&_common_set_ldslave.theData_out));
	return _test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

#endif

// ===============================================================================

#ifndef JVX_CONNECTION_MASTER_SKIP_TRANSFER_FORWARD_ICON
virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
{
	jvxBool forward = true;
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	forward = false;
#endif
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE);

#ifndef JVX_SUPPRESS_AUTO_READY_CHECK_ICON
	if (tp == JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY)
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxBool isReady = true;
		jvxApiString* astr = (jvxApiString*)data;
		res = this->is_ready(&isReady, astr);
		assert(res == JVX_NO_ERROR);
		if (!isReady)
		{
			return JVX_ERROR_NOT_READY;
		}
	}
#endif
	return _transfer_forward_icon(forward, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
};
#endif

virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
{
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE);

	return _transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
};

// ===============================================================================

virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
{
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE);

	return _transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
};

#ifndef JVX_CONNECTION_MASTER_SKIP_TRANSFER_BACKWARD_OCON
virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
{
	jvxBool forward = true;

	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE);

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	forward = false;
#endif
	return _transfer_backward_ocon(forward, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
};
#endif

// ===============================================================================

#ifndef JVX_INPUT_OUTPUT_SUPPRESS_PREPARE_POSTPROCESS_TO
virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool forward = true;

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	forward = false;
#endif

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);
	jvxErrorType resF = JVX_NO_ERROR;
#endif

#ifndef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART

	_common_set_autostart.impPrepareOnChainPrepare = false;
	if (_common_set_autostart.prepareOnChainPrepare)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			// Call of implicit prepare
			this->prepare();
			_common_set_autostart.impPrepareOnChainPrepare = true;
		}
	}
#endif
	if (_common_set_min.theState < JVX_STATE_PREPARED)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	_common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages = JVX_MAX(
		_common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages,
		_common_set_ldslave.num_additional_pipleline_stages);
	_common_set_ldslave.theData_in->con_data.number_buffers = JVX_MAX(
		_common_set_ldslave.theData_in->con_data.number_buffers,
		_common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages + 1);
	_common_set_ldslave.theData_in->con_data.number_buffers = JVX_MAX(
		_common_set_ldslave.theData_in->con_data.number_buffers,
		_common_set_ldslave.num_min_buffers_in);

// ################################################################
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API

#ifdef JVX_INPUTOUTPUTCONNECTOR_VERBOSE
	std::cout << __FUNCTION__ << "::" << __FILE__ << ": " << __LINE__ << ": Input Number buffers = " << _common_set_ldslave.theData_in->con_data.number_buffers << std::endl;
#endif

	res = _common_set_ldslave.data_processor->prepare_sender_to_receiver(_common_set_ldslave.theData_in);
#else

	res = jvx_allocate_pipeline_and_buffers_prepare_to(_common_set_ldslave.theData_in
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		, _common_set_ldslave.descriptor.c_str()
#endif
	);

#endif
// ################################################################

	if (res == JVX_NO_ERROR)
	{

		// The only deviation may be the number of output channels - which is taken from the node parameter set
		_common_set_ldslave.theData_out.con_data.number_buffers = _common_set_ldslave.num_min_buffers_out;
		_common_set_ldslave.theData_out.con_pipeline.num_additional_pipleline_stages = 0;
		_common_set_ldslave.theData_out.con_sync.type_timestamp = _common_set_ldslave.theData_in->con_sync.type_timestamp;

		// Forward the "forward allocation flags" - JVX_LINKDATA_ALLOCATION_MASK_FORWARD_ELEMENT_TO_ELEMENT is declared in TjvxDataLinkDescriptor.h
		// We can input additional flags by setting _common_set_ldslave.theData_out.con_data.alloc_flags in advance
		_common_set_ldslave.theData_out.con_data.alloc_flags |= (_common_set_ldslave.theData_in->con_data.alloc_flags & _common_set_ldslave.fwd_alloc_flags);

		res = _prepare_connect_icon(forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

// ################################################################
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API
		if (res == JVX_NO_ERROR)
		{
#ifdef JVX_INPUTOUTPUTCONNECTOR_VERBOSE
			std::cout << __FUNCTION__ << "::" << __FILE__ << ": " << __LINE__ << ": Output Number buffers = " << _common_set_ldslave.theData_out.con_data.number_buffers << std::endl;
#endif

			// This is for convenience: support old type of processing principle
			_common_set_ldslave.theData_in->con_compat.buffersize =
				_common_set_ldslave.theData_out.con_params.buffersize;
			_common_set_ldslave.theData_in->con_compat.format =
				_common_set_ldslave.theData_out.con_params.format;
			_common_set_ldslave.theData_in->con_compat.from_receiver_buffer_allocated_by_sender =
				_common_set_ldslave.theData_out.con_data.buffers;
			_common_set_ldslave.theData_in->con_compat.number_buffers =
				_common_set_ldslave.theData_out.con_data.number_buffers;
			_common_set_ldslave.theData_in->con_compat.number_channels =
				_common_set_ldslave.theData_out.con_params.number_channels;
			_common_set_ldslave.theData_in->con_compat.rate =
				_common_set_ldslave.theData_out.con_params.rate;
			
			_common_set_ldslave.theData_in->con_compat.ext.segmentation_x =
				_common_set_ldslave.theData_out.con_params.segmentation.x;
			_common_set_ldslave.theData_in->con_compat.ext.segmentation_y =
				_common_set_ldslave.theData_out.con_params.segmentation.y;
			_common_set_ldslave.theData_in->con_compat.ext.subformat =
				_common_set_ldslave.theData_out.con_params.format_group;
			_common_set_ldslave.theData_in->con_compat.ext.hints =
				_common_set_ldslave.theData_out.con_params.hints;

			// Here would be the right placce to enque a backward user hint

			// Now, create forwarding of backward oriented user hints
			_common_set_ldslave.theData_in->con_compat.user_hints = _common_set_ldslave.theData_out.con_compat.user_hints;

			res = _common_set_ldslave.data_processor->prepare_complete_receiver_to_sender(_common_set_ldslave.theData_in);
			if (res != JVX_NO_ERROR)
			{
				goto exit_error_1;
			}
		}
		else
		{
			goto exit_error_0;
		}
#endif
// ################################################################

	}

	return res;

// ################################################################
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API

	// Error cases
exit_error_1:

	resF = _postprocess_connect_icon(forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb_loc));
	jvx_neutralDataLinkDescriptor(_common_set_ldslave.theData_in, false);

exit_error_0:

	resF = _common_set_ldslave.data_processor->postprocess_sender_to_receiver(_common_set_ldslave.theData_in);
	jvx_neutralDataLinkDescriptor(&_common_set_ldslave.theData_out, true);

#endif
	// ################################################################

	return res;
};
#endif

virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
{
	return _prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

// ===============================================================================
#ifndef JVX_INPUT_OUTPUT_SUPPRESS_PREPARE_POSTPROCESS_TO
virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool forward = true;
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	forward = false;
#endif

	assert(_common_set_min.theState >= JVX_STATE_PREPARED);

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API
	res = _common_set_ldslave.data_processor->before_postprocess_receiver_to_sender(_common_set_ldslave.theData_in);
	assert(res == JVX_NO_ERROR);

	_common_set_ldslave.theData_in->con_compat.user_hints = NULL;

	_common_set_ldslave.theData_in->con_compat.buffersize = 0;
	_common_set_ldslave.theData_in->con_compat.format = JVX_DATAFORMAT_NONE;
	_common_set_ldslave.theData_in->con_compat.from_receiver_buffer_allocated_by_sender = NULL;
	_common_set_ldslave.theData_in->con_compat.number_buffers = 0;
	_common_set_ldslave.theData_in->con_compat.number_channels = 0;
	_common_set_ldslave.theData_in->con_compat.rate = 0;
	_common_set_ldslave.theData_in->con_compat.ext.hints = 0;
	_common_set_ldslave.theData_in->con_compat.ext.segmentation_x = 0;
	_common_set_ldslave.theData_in->con_compat.ext.segmentation_y = 0;
	_common_set_ldslave.theData_in->con_compat.ext.subformat = JVX_DATAFORMAT_GROUP_NONE;

#endif

	res = _postprocess_connect_icon(forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	assert(res == JVX_NO_ERROR);

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API
	
	// Deallocate buffers in theData_in
	res = _common_set_ldslave.data_processor->postprocess_sender_to_receiver(_common_set_ldslave.theData_in);

#else

	res = jvx_deallocate_pipeline_and_buffers_postprocess_to(_common_set_ldslave.theData_in);
#endif
	assert(res == JVX_NO_ERROR);

#ifndef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
	if (_common_set_autostart.impPrepareOnChainPrepare)
	{
		if (_common_set_min.theState == JVX_STATE_PREPARED)
		{
			// Call of implicit prepare
			this->postprocess();
		}
	}
	_common_set_autostart.impPrepareOnChainPrepare = false;
#endif

	return res;
};
#endif

virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
{
	return _postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

// ===============================================================================
#ifdef JVX_INPUT_OUTPUT_SUPPRESS_START_STOP_TO
virtual jvxErrorType JVX_CALLINGCONVENTION __start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
#else
virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
#endif
{
	jvxBool forward = true;
	jvxSize idRuntime = JVX_SIZE_UNSELECTED;

#ifndef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
	_common_set_autostart.impStartOnChainStart = false;
	if (_common_set_autostart.startOnChainStart)
	{
		if (_common_set_min.theState == JVX_STATE_PREPARED)
		{
			// Call of implicit prepare
			this->start();
			_common_set_autostart.impStartOnChainStart = true;
		}
	}
#endif

	if (_common_set.theUniqueId)
	{
		_common_set.theUniqueId->obtain_unique_id(&idRuntime, _common_set.theDescriptor.c_str());
	}

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	forward = false;
#endif
	return _start_connect_icon(forward, idRuntime JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
};

virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
{
	return _start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

// ===============================================================================
#ifdef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_STARTSTOP_TO
virtual jvxErrorType JVX_CALLINGCONVENTION __process_start_icon(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk = NULL,
	jvxHandle* priv_ptr = NULL) 
#else
virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
	jvxSize pipeline_offset, 
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk = NULL,
	jvxHandle* priv_ptr = NULL) override
#endif
{
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	return shift_buffer_pipeline_idx_on_start(pipeline_offset, 
		idx_stage, 
		tobeAccessedByStage, 
		clbk,
		priv_ptr);

#else
	return _process_start_icon(
		pipeline_offset, 
		idx_stage, 
		tobeAccessedByStage,
		clbk, 
		priv_ptr);
#endif
};

virtual jvxErrorType JVX_CALLINGCONVENTION process_start_ocon(
	jvxSize pipeline_offset , 
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)override
{
	return _process_start_ocon(pipeline_offset, 
		idx_stage, tobeAccessedByStage, 
		clbk, priv_ptr);
};

// ===============================================================================

#ifdef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO
virtual jvxErrorType JVX_CALLINGCONVENTION __process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
#else
virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override
#endif
{
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	return JVX_NO_ERROR;
#else
//#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API
	jvxErrorType res = JVX_NO_ERROR;
	 
#ifdef JVX_INPUTOUTPUTCONNECTOR_VERBOSE
	std::cout << __FUNCTION__ << "::" << __FILE__ << ": " << __LINE__ << ": Input IDX=" << _common_set_ldslave.theData_in->con_pipeline.idx_stage << "; Output IDX=" << _common_set_ldslave.theData_in->con_compat.idx_receiver_to_sender << std::endl;
#endif
	// Connect output buffer index to buffer index in processing function
	// This is always the primary stage, therefore, we just copy the index
	if (_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr)
	{
		_common_set_ldslave.theData_in->con_compat.idx_receiver_to_sender = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
	}
	else
	{
		// Option with no output
		_common_set_ldslave.theData_in->con_compat.idx_receiver_to_sender = JVX_SIZE_UNSELECTED;
	}

#ifdef VERBOSE_BUFFERING_REPORT
	jvxSize i;
	jvxApiString str;
	jvxSize cs = JVX_SIZE_UNSELECTED;
	std::cout << "Node input <-> output, bidx = " << _common_set_ldslave.theData_in->con_pipeline.idx_stage << "/"
		<< _common_set_ldslave.theData_in->con_data.number_buffers 
		<< "<->" << _common_set_ldslave.theData_in->con_compat.idx_receiver_to_sender <<  "/"
		<< _common_set_ldslave.theData_in->con_compat.number_buffers << std::endl;
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

	// Override pipeline stage if specified
	jvxSize run_idx_stage = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;
	if (JVX_CHECK_SIZE_SELECTED(idx_stage))
	{
		run_idx_stage = idx_stage;
	}
	res = _common_set_ldslave.data_processor->process_st(_common_set_ldslave.theData_in,
		run_idx_stage,
		_common_set_ldslave.theData_in->con_compat.idx_receiver_to_sender);

	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	if (_common_set_ldslave.copy_timestamp_inout)
	{
		res = jvx_copyDataLinkDescriptorSync(_common_set_ldslave.theData_in, run_idx_stage,
			&_common_set_ldslave.theData_out, _common_set_ldslave.theData_in->con_compat.idx_receiver_to_sender);
		assert(res == JVX_NO_ERROR);
	}

#endif

	

	return _process_buffers_icon( mt_mask,  idx_stage);

#endif
// #ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER

	
};

virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_ocon(jvxSize mt_mask, jvxSize idx_stage)override
{
	return _process_buffers_ocon(mt_mask, idx_stage);
};

// ===============================================================================
#ifdef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_STARTSTOP_TO
virtual jvxErrorType JVX_CALLINGCONVENTION __process_stop_icon(jvxSize idx_stage,
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
#else
virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(jvxSize idx_stage,
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)override
#endif
{
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	return shift_buffer_pipeline_idx_on_stop(idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);
#else
	return _process_stop_icon(idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);
#endif
};


virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_ocon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage ,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr )override
{
	return _process_stop_ocon(idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr );
};

// ===============================================================================
#ifdef JVX_INPUT_OUTPUT_SUPPRESS_START_STOP_TO
virtual jvxErrorType JVX_CALLINGCONVENTION __stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
#else
virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
#endif
{
	jvxBool forward = true;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idRuntime = JVX_SIZE_UNSELECTED;

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	forward = false;
#endif
	res = _stop_connect_icon(forward, &idRuntime JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	if (_common_set.theUniqueId)
	{
		_common_set.theUniqueId->release_unique_id(idRuntime);
	}

#ifndef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
	if (_common_set_autostart.impStartOnChainStart)
	{
		if (_common_set_min.theState == JVX_STATE_PROCESSING)
		{
			this->stop();
		}
	}
	_common_set_autostart.impStartOnChainStart = false;
#endif

	return res;
};

virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
{
	return _stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

// ==============================================================================

virtual jvxErrorType associated_common_icon(IjvxDataConnectionCommon** ref) override
{
	return _associated_common_icon(ref);
}

virtual jvxErrorType associated_common_ocon(IjvxDataConnectionCommon** ref) override
{
	return _associated_common_ocon(ref);
}

// ==============================================================================

virtual jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_icon(
	jvxDataFormatGroup format_group,
	jvxDataflow data_flow)override
{
	return _supports_connector_class_icon(format_group, data_flow);
}

virtual jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_ocon(
	jvxDataFormatGroup format_group,
	jvxDataflow data_flow)override
{
	return _supports_connector_class_ocon(format_group, data_flow);
}

// ==============================================================================

virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_icon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master, 
	IjvxDataConnectionCommon* ass_connection_common, IjvxInputConnector** replace_connector) override
{
	return _select_connect_icon(obj, master, ass_connection_common, replace_connector);
};

virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_icon(IjvxConnectorBridge* obj, 
	IjvxInputConnector* replace_connector) override
{
	return _unselect_connect_icon(obj, replace_connector);
};

virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_ocon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master, 
	IjvxDataConnectionCommon* ass_connection_common, 
	IjvxOutputConnector** replace_connector) override
{
	return _select_connect_ocon(obj, master, ass_connection_common, replace_connector);
};

virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_ocon(IjvxConnectorBridge* obj,
	IjvxOutputConnector* replace_connector) override
{
	return _unselect_connect_ocon(obj, replace_connector);
};

virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_connector(jvxApiString* str) override
{
	return _descriptor_connector(str);
}

virtual jvxErrorType JVX_CALLINGCONVENTION parent_factory(IjvxConnectorFactory** my_parent)override
{
	return _parent_factory(my_parent);
}

virtual jvxErrorType JVX_CALLINGCONVENTION number_next(jvxSize* num) override
{
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	if (num)
		*num = 0;
	return JVX_NO_ERROR;
#else
	return _number_next(num);
#endif
}

virtual jvxErrorType JVX_CALLINGCONVENTION reference_next(jvxSize idx, IjvxConnectionIterator** next) override
{
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	if (next)
	{
		*next = nullptr;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
#else
	return _reference_next(idx, next);
#endif

}

virtual jvxErrorType JVX_CALLINGCONVENTION reference_component(
	jvxComponentIdentification* cpTp, 
	jvxApiString* modName,
	jvxApiString* lContext) override
{
	return _reference_component(cpTp, modName, lContext);
}

virtual jvxErrorType JVX_CALLINGCONVENTION connected_icon(IjvxInputConnector** icon) override
{
	return _connected_icon(icon);
}

#ifdef JVX_INPUTOUTPUT_CONNECTOR_UNDEFINE_OBJECT_REFERENCE
#undef JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE
#endif
