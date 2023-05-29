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


virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
{
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE);

	return _transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
};



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

#ifndef JVX_CONNECTOR_NOT_DERIVED_FROM_OBJECT
	if (_common_set.theUniqueId)
	{
		_common_set.theUniqueId->obtain_unique_id(&idRuntime, _common_set.theDescriptor.c_str());
	}
#endif

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	forward = false;
#endif
	return _start_connect_icon(forward, idRuntime JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
};

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
	std::cout << __FUNCTION__ << "::" << __FILE__ << ": " << __LINE__ << ": Input IDX=" << _common_set_icon.theData_in->con_pipeline.idx_stage << "; Output IDX=" << _common_set_icon.theData_in->con_compat.idx_receiver_to_sender << std::endl;
#endif
	// Connect output buffer index to buffer index in processing function
	// This is always the primary stage, therefore, we just copy the index
	if (_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr)
	{
		_common_set_icon.theData_in->con_compat.idx_receiver_to_sender = *_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr;
	}
	else
	{
		// Option with no output
		_common_set_icon.theData_in->con_compat.idx_receiver_to_sender = JVX_SIZE_UNSELECTED;
	}

#ifdef VERBOSE_BUFFERING_REPORT
	jvxSize i;
	jvxApiString str;
	jvxSize cs = JVX_SIZE_UNSELECTED;
	std::cout << "Node input <-> output, bidx = " << _common_set_icon.theData_in->con_pipeline.idx_stage << "/"
		<< _common_set_icon.theData_in->con_data.number_buffers 
		<< "<->" << _common_set_icon.theData_in->con_compat.idx_receiver_to_sender <<  "/"
		<< _common_set_icon.theData_in->con_compat.number_buffers << std::endl;
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

	// Override pipeline stage if specified
	jvxSize run_idx_stage = *_common_set_icon.theData_in->con_pipeline.idx_stage_ptr;
	if (JVX_CHECK_SIZE_SELECTED(idx_stage))
	{
		run_idx_stage = idx_stage;
	}
	res = _common_set_ldslave.data_processor->process_st(_common_set_icon.theData_in,
		run_idx_stage,
		_common_set_icon.theData_in->con_compat.idx_receiver_to_sender);

	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	if (_common_set_ldslave.copy_timestamp_inout)
	{
		res = jvx_copyDataLinkDescriptorSync(_common_set_icon.theData_in, run_idx_stage,
			&_common_set_ocon.theData_out, _common_set_icon.theData_in->con_compat.idx_receiver_to_sender);
		assert(res == JVX_NO_ERROR);
	}

#endif

	

	return _process_buffers_icon( mt_mask,  idx_stage);

#endif
// #ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER

	
};

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

#ifndef JVX_CONNECTOR_NOT_DERIVED_FROM_OBJECT
	if (_common_set.theUniqueId)
	{
		_common_set.theUniqueId->release_unique_id(idRuntime);
	}
#endif

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

virtual jvxErrorType associated_connection_icon(IjvxDataConnectionCommon** ref) override
{
	return _associated_connection_icon(ref);
}

#ifndef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AVAILABLE
virtual jvxErrorType available_to_connect_icon() override
{
	return _available_to_connect_icon();
}
#endif

virtual IjvxInputConnector* reference_icon() override
{
	return this;
}

virtual jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_icon(
	jvxDataFormatGroup format_group,
	jvxDataflow data_flow)override
{
	return _supports_connector_class_icon(format_group, data_flow);
}

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

virtual jvxErrorType connected_ocon(IjvxOutputConnector** ocon) override
{
	return _connected_ocon(ocon);
}

#ifndef JVX_INPUT_OUTPUT_SUPPRESS_TRIGGER_CONNECTOR
virtual jvxErrorType request_trigger_otcon(IjvxTriggerOutputConnector** otcon) override
{
	return JVX_ERROR_UNSUPPORTED;
}

virtual jvxErrorType return_trigger_otcon(IjvxTriggerOutputConnector* otcon) override
{
	return JVX_ERROR_UNSUPPORTED;
}
#endif
