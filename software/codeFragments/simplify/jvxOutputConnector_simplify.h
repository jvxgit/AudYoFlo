#ifndef JVX_CONNECTION_MASTER_SKIP_CONNECT_CONNECT_OCON
virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ocon(const jvxChainConnectArguments& args  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
{
	// Forward user hints
	return _connect_connect_ocon(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}
#endif

// ===============================================================================

#ifndef JVX_CONNECTION_MASTER_SKIP_DISCONNECT_CONNECT_OCON
virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
{
	return _disconnect_connect_ocon(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}
#endif

// ===============================================================================

#ifndef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_OCON
virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
{
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE);

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_SET_OUTPUT_PARAMETERS
	this->test_set_output_parameters();
#endif

	JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_O(fdb, (&_common_set_ocon.theData_out));
	return _test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

#endif

// ===============================================================================

virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
{
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE);

	return _transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
};

// ===============================================================================

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

virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
{
	return _prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

// ===============================================================================

virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
{
	return _postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

// ===============================================================================

#ifndef JVX_INPUT_OUTPUT_SUPPRESS_START_STOP
virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
{
	return _start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};
#endif

// ===============================================================================


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



virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_ocon(jvxSize mt_mask, jvxSize idx_stage)override
{
	return _process_buffers_ocon(mt_mask, idx_stage);
};

// ===============================================================================


virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_ocon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage ,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr )override
{
	return _process_stop_ocon(idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr );
};

// ===============================================================================

#ifndef JVX_INPUT_OUTPUT_SUPPRESS_START_STOP
virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
{
	return _stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};
#endif

// ==============================================================================

virtual jvxErrorType associated_connection_ocon(IjvxDataConnectionCommon** ref) override
{
	return _associated_connection_ocon(ref);
}

#ifndef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AVAILABLE
virtual jvxErrorType available_to_connect_ocon() override
{
	return _available_to_connect_ocon();
}
#endif

// ==============================================================================


virtual IjvxOutputConnector* reference_ocon() override
{
	return this;
}

// ==============================================================================


virtual jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_ocon(
	jvxDataFormatGroup format_group,
	jvxDataflow data_flow)override
{
	return _supports_connector_class_ocon(format_group, data_flow);
}

// ==============================================================================

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

virtual jvxErrorType JVX_CALLINGCONVENTION connected_icon(IjvxInputConnector** icon) override
{
	return _connected_icon(icon);
}

#ifndef JVX_INPUT_OUTPUT_SUPPRESS_TRIGGER_CONNECTOR
virtual jvxErrorType request_trigger_itcon(IjvxTriggerInputConnector** itcon) override
{
	return JVX_ERROR_UNSUPPORTED;
}

virtual jvxErrorType return_trigger_itcon(IjvxTriggerInputConnector* itcon) override
{
	return JVX_ERROR_UNSUPPORTED;
}
#endif


