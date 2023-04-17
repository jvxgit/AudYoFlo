	jvxErrorType connect_connect_icon_x(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
#ifdef JVX_CONNECT_X_FORWARD_MASTER
		assert(_common_set_icon.theData_in);
		return CjvxInputOutputConnector::_connect_connect_ocon(
			jvxChainConnectArguments(_common_set_icon.theData_in->con_link.uIdConn, _common_set_icon.theData_in->con_link.master)
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
#else
		return CjvxInputOutputConnector::_connect_connect_ocon(
			jvxChainConnectArguments(uIdMain, static_cast<IjvxConnectionMaster*>(this)) 
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
#endif
	};

	jvxErrorType disconnect_connect_icon_x(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
#ifdef JVX_CONNECT_X_FORWARD_MASTER
		assert(_common_set_icon.theData_in);
		return CjvxInputOutputConnector::_disconnect_connect_ocon(
			jvxChainConnectArguments(_common_set_icon.theData_in->con_link.uIdConn, _common_set_icon.theData_in->con_link.master)
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
#else
		return CjvxInputOutputConnector::_disconnect_connect_ocon(
			jvxChainConnectArguments(_common_set_ocon.theData_out.con_link.uIdConn, static_cast<IjvxConnectionMaster*>(this))
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
#endif
	};

	/*
	jvxErrorType specific_connect_ocon_x(jvxHandle* data, jvxDataConnectionsSpecificType tp)
	{
		return CjvxInputOutputConnector::_specific_connect_icon(data, tp, true);
	};
	*/

	jvxErrorType transfer_forward_icon_x(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{

		jvxErrorType res = JVX_NO_ERROR;
		std::string locTxt;
		jvxApiString* str = (jvxApiString*)data;
		CjvxJsonElementList* jsonLst = (CjvxJsonElementList*)data;
		CjvxJsonElement jsonElm;

		switch (tp)
		{
		case JVX_LINKDATA_TRANSFER_COLLECT_LINK_STRING:

			locTxt = str->std_str();
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
			locTxt += "-<xcon>->";
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
			str->assign(locTxt);
			break;
		case JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON:
			jsonElm.makeAssignmentString("connects_spec_type", "chain_crossing");
			jsonLst->addConsumeElement(jsonElm);
			break;
		}

		res = CjvxInputOutputConnector::_transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		switch (tp)
		{
		case JVX_LINKDATA_TRANSFER_COLLECT_LINK_STRING:

			locTxt = str->std_str();
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
			locTxt += "<-<nocx>-";
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
			str->assign(locTxt);
			break;
		}
		return res;
	}

	jvxErrorType transfer_backward_ocon_x(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		return CjvxInputOutputConnector::_transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	/*
	jvxErrorType test_connect_ocon_x()
	{
		return CjvxInputOutputConnector::_test_connect_icon(true);
	}
	*/

	jvxErrorType start_connect_icon_x(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr = 0;
		_common_set_ocon.theData_out.con_compat.idx_receiver_to_sender = 0;
		return CjvxInputOutputConnector::_start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	};

	jvxErrorType stop_connect_icon_x(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		return CjvxInputOutputConnector::_stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	};

	jvxErrorType process_start_icon_x(jvxSize pipeline_offset , jvxSize* idx_stage,
		jvxSize tobeAccessedByStage)
	{
		return CjvxInputOutputConnector::_process_start_ocon(pipeline_offset , idx_stage,
			 tobeAccessedByStage);
	};

	jvxErrorType process_buffers_icon_x(jvxSize mt_mask, jvxSize idx_stage)
	{
		return CjvxInputOutputConnector::_process_buffers_ocon( mt_mask,  idx_stage);
	};

	jvxErrorType process_stop_icon_x(jvxSize idx_stage , jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr)
	{
		return CjvxInputOutputConnector::_process_stop_ocon(idx_stage, shift_fwd,
			tobeAccessedByStage,
			clbk,
			priv_ptr);
	};
