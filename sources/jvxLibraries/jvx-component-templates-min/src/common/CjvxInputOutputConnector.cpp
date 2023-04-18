#include "common/CjvxInputOutputConnector.h"

CjvxInputOutputConnector::CjvxInputOutputConnector()
	{
		_common_set_ldslave.data_processor = NULL;
		_common_set_ldslave.num_min_buffers_in = 1;
		_common_set_ldslave.num_min_buffers_out = 1;
		_common_set_ldslave.num_additional_pipleline_stages = 0;
		_common_set_ldslave.zeroCopyBuffering_cfg = false;		
		_common_set_ldslave.copy_timestamp_inout = true;

		_common_set_ldslave.detectLoop = false;

		// _common_set_ldslave.caps_in with default settings
		// _common_set_ldslave.caps_out with default settings

#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		_common_set_ldslave.dbg_hint = NULL;
#endif
}

CjvxInputOutputConnector::~CjvxInputOutputConnector()
	{
	}

	jvxErrorType 
		CjvxInputOutputConnector::CjvxInputOutputConnector::_available_connector(jvxBool* isAvail)
	{
		if (isAvail)
		{
			*isAvail = false;
			if (
				(_common_set_icon.conn_in == NULL) &&
				(_common_set_ocon.conn_out == NULL)) // <-
			{
				*isAvail = true;
			}
		}
		return JVX_NO_ERROR;
	}
	
	

	jvxErrorType 
		CjvxInputOutputConnector::lds_activate(IjvxDataProcessor* theDataProc, IjvxObject* theObjRef,
		IjvxConnectorFactory* myPar, IjvxConnectionMaster* master, std::string descror,
		IjvxInputConnector* icon, IjvxOutputConnector* ocon
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		, const char* dbg_hint
#endif
		)
	{
		jvxErrorType res = JVX_NO_ERROR;
		_common_set_ldslave.data_processor = theDataProc;
		_common_set_ldslave.detectLoop = false;

		CjvxInputOutputConnectorCore::activate(theObjRef,
			myPar, master, descror);

		CjvxInputConnectorCore::activate(icon);
		CjvxOutputConnectorCore::activate(ocon);		

#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		_common_set_ldslave.dbg_hint = dbg_hint;
#endif
		return res;
	}

	/*
	jvxErrorType
		CjvxInputOutputConnector::_outputs_data_format_group(jvxDataFormatGroup grp)
	{
		if (grp == _common_set_ldslave.format_group_out)
		{
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_UNSUPPORTED;
	}
	*/

	jvxErrorType 
		CjvxInputOutputConnector::lds_deactivate()
	{
		jvxErrorType res = JVX_NO_ERROR;
		_common_set_ldslave.data_processor = NULL;
		
		CjvxInputOutputConnectorCore::deactivate();
		CjvxInputConnectorCore::deactivate();
		CjvxOutputConnectorCore::deactivate();
		
		return res;
	}	

	// ==============================================================
	// SELECT CHECK SELECT CHECK SELECT CHECK SELECT CHECK SELECT CHECK
	// ==============================================================	

	jvxErrorType 
		CjvxInputOutputConnector::_check_common_icon(IjvxDataConnectionCommon* ass_connection_common, IjvxConnectionMaster* master)
	{
		// Cross check
		if (_common_set_ocon.theCommon_from != NULL)
		{
			if (ass_connection_common != _common_set_ocon.theCommon_from)
			{
				return JVX_ERROR_INVALID_SETTING;
			}
		}

		return _check_common(ass_connection_common, master);
	}

	jvxErrorType
		CjvxInputOutputConnector::_check_common_ocon(IjvxDataConnectionCommon* ass_connection_common, IjvxConnectionMaster* master)
	{
		if (_common_set_icon.theCommon_to != NULL)
		{
			if (ass_connection_common != _common_set_icon.theCommon_to)
			{
				return JVX_ERROR_INVALID_SETTING;
			}
		}
		
		return _check_common(ass_connection_common, master);
	}
	
	jvxErrorType
		CjvxInputOutputConnector::_check_common(IjvxDataConnectionCommon* ass_connection_common, IjvxConnectionMaster* master)
	{
		// If we are linked with a master since we are a master with connectors, passed master must match
		if (master && _common_set_io_common.theMaster)
		{
			if (master != _common_set_io_common.theMaster)
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
		}

		return JVX_NO_ERROR;
	}

	// ==============================================================
	// CONNECT CONNECT CONNECT CONNECT CONNECT CONNECT CONNECT CONNECT
	// ==============================================================

	jvxErrorType
		CjvxInputOutputConnector::_connect_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		assert(_common_set_ldslave.detectLoop == false);

		if (_common_set_ocon.ocon)
		{
			_common_set_ldslave.detectLoop = true;
			res = _common_set_ocon.ocon->connect_connect_ocon(
				jvxChainConnectArguments(_common_set_icon.theData_in->con_link.uIdConn, 
					_common_set_icon.theData_in->con_link.master) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			_common_set_ldslave.detectLoop = false;
			if (res != JVX_NO_ERROR)
			{
				_common_set_icon.theData_in = NULL;
			}
		}
		else
		{
			std::cout << "Not connected" << std::endl;
		}
		return res;
	}	

	// ==============================================================
	// DISCONNECT DISCONNECT DISCONNECT DISCONNECT DISCONNECT DISCONNECT
	// ==============================================================

	jvxErrorType
		CjvxInputOutputConnector::_disconnect_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (_common_set_ocon.ocon)
		{
			res = _common_set_ocon.ocon->disconnect_connect_ocon(jvxChainConnectArguments(_common_set_icon.theData_in->con_link.uIdConn),
				JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		else
		{
			std::cout << "Not connected" << std::endl;
		}
		return res;
	}
	
	// ==============================================================
	// PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE
	// ==============================================================

	jvxErrorType
		CjvxInputOutputConnector::_prepare_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		assert(_common_set_ldslave.detectLoop == false);
		if (_common_set_ocon.ocon)
		{
			_common_set_ldslave.detectLoop = true;
			res = _common_set_ocon.ocon->prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			_common_set_ldslave.detectLoop = false;
		}
		else
		{
			std::cout << "Not connected" << std::endl;
		}
		return res;
	}

	// ==============================================================
	// POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS
	// ==============================================================

	jvxErrorType
		CjvxInputOutputConnector::_postprocess_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		assert(_common_set_ldslave.detectLoop == false);
		if (_common_set_ocon.ocon)
		{
			_common_set_ldslave.detectLoop = true;
			_common_set_ocon.ocon->postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			_common_set_ldslave.detectLoop = false;
		}
		else
		{
			std::cout << "Not connected" << std::endl;
		}
		return res;
	}

	// ==============================================================
	// START START START START START START START START START START START
	// ==============================================================

	void
	CjvxInputOutputConnector::_start_connect_common(jvxSize myUniquePipelineId)
	{
		_common_set_io_common.myRuntimeId = myUniquePipelineId;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_start_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;

		assert(_common_set_ldslave.detectLoop == false);
		if (_common_set_ocon.ocon)
		{
			_common_set_ldslave.detectLoop = true;
			_common_set_ocon.ocon->start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			_common_set_ldslave.detectLoop = false;
		}
		else
		{
			std::cout << "Not connected" << std::endl;
		}

		return res;
	}

	// ==============================================================
	// STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP
	// ==============================================================
	
	jvxErrorType 
		CjvxInputOutputConnector::_stop_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		assert(_common_set_ldslave.detectLoop == false);
		if (_common_set_ocon.ocon)
		{
			_common_set_ldslave.detectLoop = true;
			res = _common_set_ocon.ocon->stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			_common_set_ldslave.detectLoop = false;
		}
		else
		{
			std::cout << "Not connected" << std::endl;
		}
		return res;
	}

	void 
	CjvxInputOutputConnector::_stop_connect_common(jvxSize* uId)
	{
		if (uId)
			*uId = _common_set_io_common.myRuntimeId;

		_common_set_io_common.myRuntimeId = JVX_SIZE_UNSELECTED;
	}

	// =========================================================================
	// =========================================================================
	// =========================================================================
	// =========================================================================

	// ==============================================================
	// PROCESSSTART PROCESSSTART PROCESSSTART PROCESSSTART PROCESSSTART
	// ==============================================================
	jvxErrorType 
		CjvxInputOutputConnector::_process_start_icon(jvxSize pipeline_offset , jvxSize* idx_stage, jvxSize tobeAccessedByStage,
			callback_process_start_in_lock clbk, jvxHandle* priv_ptr)
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Propagate to next object
		if (_common_set_ocon.ocon)
		{
			// Input / output connector implementations are never zerocopy
			// We start a regular output connector here
			res = _common_set_ocon.ocon->process_start_ocon(0, NULL, 0, NULL, NULL);
		}

		if (res == JVX_NO_ERROR)
		{
			res = shift_buffer_pipeline_idx_on_start(pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
		}
		return res;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_process_start_ocon(jvxSize pipeline_offset, jvxSize* idx_stage, jvxSize tobeAccessedByStage,
			callback_process_start_in_lock clbk,
			jvxHandle* priv_ptr )
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Propagate to next object
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon(pipeline_offset, idx_stage, tobeAccessedByStage,
				clbk, priv_ptr);
		}
		else
		{
			if (_common_set_ocon.allows_termination)
			{
				// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
				// Accepted as chain terminator
			}
			else
			{
				std::cout << "This output connector is not connected to any bridge." << std::endl;
				res = JVX_ERROR_UNEXPECTED;
			}
		}

		return res;
	}

	// ==============================================================
	// PROCESSBUFFER PROCESSBUFFER PROCESSBUFFER PROCESSBUFFER PROCESSBUFFER
	// ==============================================================
	jvxErrorType 
		CjvxInputOutputConnector::_process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
	{
		jvxErrorType res = JVX_NO_ERROR;

		if (_common_set_ocon.ocon)
		{
			// Copy the admin data
			_common_set_ocon.theData_out.admin = _common_set_icon.theData_in->admin;
			_common_set_ocon.ocon->process_buffers_ocon(mt_mask, idx_stage);

			//theData->link.descriptor_out_linked->admin = theData->admin;
			//res = theData->link.next->process_buffers_icon();
		}
		return res;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_process_buffers_ocon(jvxSize mt_mask, jvxSize idx_stage)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon(mt_mask, idx_stage);
		}
		else
		{
			if (_common_set_ocon.allows_termination)
			{
				// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
				// Accepted as chain terminator
			}
			else
			{
				std::cout << "This output connector is not connected to any bridge." << std::endl;
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		return res;
	}

	// ==============================================================
	// PROCESSSTOP PROCESSSTOP PROCESSSTOP PROCESSSTOP PROCESSSTOP PROCESSSTOP
	// ==============================================================
	jvxErrorType 
		CjvxInputOutputConnector::_process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
			jvxSize tobeAccessedByStage,
			callback_process_stop_in_lock clbk,
			jvxHandle* priv_ptr)
	{
		jvxErrorType res = JVX_NO_ERROR;


		if (_common_set_ocon.ocon)
		{
			// Input / output connector implementations are never zerocopy
			// We start a regular output connector here
			res = _common_set_ocon.ocon->process_stop_ocon(JVX_SIZE_UNSELECTED, true, tobeAccessedByStage , clbk, priv_ptr);
		}

		if (res == JVX_NO_ERROR)
		{
			res = shift_buffer_pipeline_idx_on_stop(idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);
		}
		return res;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_process_stop_ocon(jvxSize idx_stage, jvxBool shift_fwd,
			jvxSize tobeAccessedByStage ,
			callback_process_stop_in_lock clbk,
			jvxHandle* priv_ptr)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->process_stop_icon(
				idx_stage, shift_fwd, 
				tobeAccessedByStage,
				clbk, priv_ptr);
		}
		else
		{
			if (_common_set_ocon.allows_termination)
			{
				// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
				// Accepted as chain terminator
			}
			else
			{
				std::cout << "This output connector is not connected to any bridge." << std::endl;
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		return res;
	}

	// ============================================================================
	// ============================================================================
	// ============================================================================
	// ============================================================================

	// ==============================================================
	// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::_test_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(_common_set_io_common.object),
			_common_set_io_common.descriptor.c_str(), "Entering single default CjvxInputOutputConnector input connector");

		JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_I(fdb, _common_set_icon.theData_in);
		if (forward)
		{
			if (_common_set_ocon.ocon)
			{
				return _common_set_ocon.ocon->test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			return JVX_ERROR_UNSUPPORTED;
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(_common_set_io_common.object),
			_common_set_io_common.descriptor.c_str(), "Leaving single default CjvxInputOutputConnector output connector");
		JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_O(fdb, (&_common_set_ocon.theData_out));

		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			_common_set_ocon.setup_for_termination = false;
			return _common_set_ocon.theData_out.con_link.connect_to->test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}

		if (_common_set_ocon.allows_termination)
		{
			JVX_CONNECTION_FEEDBACK_SET_COMMENT_STRING(fdb, "Input / output connector with allowed termination.");
			_common_set_ocon.setup_for_termination = true;
			return JVX_NO_ERROR;
		}
		
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "No output connection.", JVX_ERROR_NOT_READY);
		return JVX_ERROR_NOT_READY;
	}

	// ==============================================================
	// TRANSFER FORWARD
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION _transfer_forward_icon(jvxBool forward, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
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
			if (_common_set_icon.icon)
			{
				locTxt += "object input connector " + JVX_DISPLAY_CONNECTOR(_common_set_icon.icon);
			}
			if (forward)
			{
				if (_common_set_ocon.ocon)
				{
					locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
					locTxt += "-<int>->";
					locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
					str->assign(locTxt);
					res = _common_set_ocon.ocon->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					locTxt = str->std_str();
					locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
					locTxt += "<-<tni>-";
					locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				}
				else
				{
					locTxt += "-<ncon>";
				}
			}
			else
			{
				locTxt += "<end>";
			}
			str->assign(locTxt);
			return res;
			break;

		case JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON:

			if (_common_set_icon.icon)
			{
				jsonElm.makeAssignmentString("icon", JVX_DISPLAY_CONNECTOR(_common_set_icon.icon));
				jsonLst->addConsumeElement(jsonElm);
			}
			if (forward)
			{
				if (_common_set_ocon.ocon)
				{
					CjvxJsonElementList jsonLstRet;
					res = _common_set_ocon.ocon->transfer_forward_ocon(tp, &jsonLstRet JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (res == JVX_NO_ERROR)
					{
						jsonElm.makeSection("connects_from", jsonLstRet);
					}
					else
					{
						jsonElm.makeAssignmentString("connects_error", jvxErrorType_txt(res));
					}
					jsonLst->addConsumeElement(jsonElm);
				}
			}
			else
			{
				CjvxJsonElementList jsonLstRet;
				jsonElm.makeAssignmentString("connects_end", "here");
				jsonLst->addConsumeElement(jsonElm);
			}
			return JVX_NO_ERROR;
			break;

		case JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY:

			// This is supported
			if (forward)
			{
				if (_common_set_ocon.ocon)
				{
					res = _common_set_ocon.ocon->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
			}
			return res;
			break;

		default:
			if (forward)
			{
				if (_common_set_ocon.ocon)
				{
					return _common_set_ocon.ocon->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
				return JVX_ERROR_UNSUPPORTED;
			}
			break;
		}
		return JVX_ERROR_UNSUPPORTED;
	}

	jvxErrorType 
	CjvxInputOutputConnector::JVX_CALLINGCONVENTION _transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{

		jvxErrorType res = JVX_NO_ERROR;
		std::string locTxt;
		jvxApiString* str = (jvxApiString*)data;
		CjvxJsonElementList* jsonLst = (CjvxJsonElementList*)data;
		CjvxJsonElement jsonElm;

		switch (tp)
		{
		case JVX_LINKDATA_TRANSFER_COLLECT_LINK_STRING:

			if (!str)
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
			locTxt = str->std_str();
			if (_common_set_ocon.ocon)
			{
				locTxt += "object output connector " + JVX_DISPLAY_CONNECTOR(_common_set_ocon.ocon);
			}
			if (_common_set_ocon.theData_out.con_link.connect_to)
			{
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
				locTxt += "-<ext>->";
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
				str->assign(locTxt);
				res = _common_set_ocon.theData_out.con_link.connect_to->transfer_forward_icon(tp, str JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				locTxt = str->std_str();
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				locTxt += "<-<txe>-";
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				str->assign(locTxt);
				return res;
			}
			locTxt += "-<not_connected>";
			str->assign(locTxt);
			return JVX_NO_ERROR;
			break;
		case JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON:

			if (!jsonLst)
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
			if (_common_set_ocon.ocon)
			{
				jsonElm.makeAssignmentString("ocon", JVX_DISPLAY_CONNECTOR(_common_set_ocon.ocon));
				jsonLst->addConsumeElement(jsonElm);
				if (_common_set_ocon.theData_out.con_link.connect_to)
				{
					CjvxJsonElementList jsonLstRet;
					res = _common_set_ocon.theData_out.con_link.connect_to->transfer_forward_icon(tp, &jsonLstRet JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (res == JVX_NO_ERROR)
					{
						jsonElm.makeSection("connects_to", jsonLstRet);
					}
					else
					{
						jsonElm.makeAssignmentString("connects_error", jvxErrorType_txt(res));
					}
					jsonLst->addConsumeElement(jsonElm);
				}
			}
			return JVX_NO_ERROR;
			break;
		default:
			if (_common_set_ocon.theData_out.con_link.connect_to)
			{
				return _common_set_ocon.theData_out.con_link.connect_to->transfer_forward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_UNSUPPORTED;
	}

	// ==============================================================
	// TRANSFER FORWARD
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION _transfer_backward_ocon(jvxBool forward, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, _common_set_io_common.object,
			_common_set_io_common.descriptor.c_str(), "Entering transfer_backward on output onnector");

		if (tp == JVX_LINKDATA_TRANSFER_REQUEST_CHAIN_SPECIFIC_DATA_HINT)
		{
			if (_common_set_ocon.theData_out.con_user.chain_spec_user_hints)
			{
				jvxDataProcessorHintDescriptorSearch* ptr = (jvxDataProcessorHintDescriptorSearch*)data;
				if (data)
				{
					jvxErrorType res = jvx_hintDesriptor_find(_common_set_ocon.theData_out.con_user.chain_spec_user_hints,
						&ptr->reference, ptr->hintDescriptorId, ptr->assocHint);
					if (res == JVX_NO_ERROR)
					{
						return res;
					}
				}
			}
		}

		if (forward)
		{
			if (_common_set_icon.icon)
			{
				return _common_set_icon.icon->transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			return JVX_ERROR_UNSUPPORTED;
		}
		if (tp == JVX_LINKDATA_TRANSFER_REQUEST_CHAIN_SPECIFIC_DATA_HINT)
		{
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION _transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, _common_set_io_common.object,
			_common_set_io_common.descriptor.c_str(), "Entering transfer_backward on input onnector");
		if (_common_set_icon.theData_in->con_link.connect_from)
		{
			return _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		return JVX_ERROR_UNSUPPORTED;
	}

	// ==============================================================

	jvxErrorType
		CjvxInputOutputConnector::_number_next(jvxSize* num)
	{
		if (num)
		{
			*num = 1;
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType
		CjvxInputOutputConnector::_reference_next(jvxSize idx, IjvxConnectionIterator** next)
	{
		if(idx == 0)
		{
			if (next)
			{
				*next = nullptr;
				IjvxInputConnector* icon = nullptr;
				this->_connected_icon(&icon);
				if (icon)
				{
					*next = static_cast<IjvxConnectionIterator*>(icon);
				}
			}
			return JVX_NO_ERROR;

		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}

	jvxErrorType
		CjvxInputOutputConnector::_reference_component(
			jvxComponentIdentification* cpTp, 
			jvxApiString* modName,
			jvxApiString* lContext)
	{
		jvxComponentIdentification cpId;
		IjvxObject* objRef = nullptr;
		if (_common_set_io_common.myParent)
		{
			_common_set_io_common.myParent->request_reference_object(&objRef);
			if (objRef)
			{
				objRef->request_specialization(nullptr, &cpId, nullptr);
				objRef->module_reference(modName, nullptr);
				_common_set_io_common.myParent->return_reference_object(objRef);
				if (lContext)
					lContext->assign(_common_set_io_common.descriptor);
			}
		}
		
		if (cpTp)
		{
			*cpTp = cpId;
		}
		return JVX_NO_ERROR;
	}
	

	// ==============================================================
	// HELPER FUNCTIONS
	// ==============================================================
	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION shift_buffer_pipeline_idx_on_start(jvxSize pipeline_offset,
			jvxSize* idx_stage,
			jvxSize tobeAccessedByStage,
			callback_process_start_in_lock clbk = NULL,
			jvxHandle* priv_ptr = NULL)
	{
		return  jvx_shift_buffer_pipeline_idx_on_start(_common_set_icon.theData_in, _common_set_io_common.myRuntimeId,
			pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	}

	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION shift_buffer_pipeline_idx_on_stop(jvxSize idx_stage, 
			jvxBool shift_fwd, jvxSize tobeAccessed,
			callback_process_stop_in_lock clbk, jvxHandle* priv_ptr)
	{
		return jvx_shift_buffer_pipeline_idx_on_stop(
			_common_set_icon.theData_in, idx_stage,
			shift_fwd, tobeAccessed, 
			clbk, priv_ptr);
	}

	jvxErrorType CjvxInputOutputConnector::JVX_CALLINGCONVENTION allocate_pipeline_and_buffers_prepare_to()
	{ 

		return jvx_allocate_pipeline_and_buffers_prepare_to(_common_set_icon.theData_in
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		, _common_set_ldslave.dbg_hint
#endif
		);
	};

	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION allocate_pipeline_and_buffers_prepare_to_zerocopy()
	{
		return jvx_allocate_pipeline_and_buffers_prepare_to_zerocopy(_common_set_icon.theData_in, &_common_set_ocon.theData_out);
	}

	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION deallocate_pipeline_and_buffers_postprocess_to()
	{ 
		return jvx_deallocate_pipeline_and_buffers_postprocess_to(_common_set_icon.theData_in);
	}

	jvxErrorType 
		JVX_CALLINGCONVENTION CjvxInputOutputConnector::deallocate_pipeline_and_buffers_postprocess_to_zerocopy()
	{
		return jvx_deallocate_pipeline_and_buffers_postprocess_to_zerocopy(_common_set_icon.theData_in);
	}

	void
		CjvxInputOutputConnector::request_test_chain(IjvxReport* theRep)
	{
		jvxCBitField req = 0;
		IjvxDataConnectionCommon* ctxt = NULL;
		jvxSize idConn = JVX_SIZE_UNSELECTED;

		jvx_bitSet(req, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT);
		if (_common_set_icon.theData_in)
		{
			if (_common_set_icon.theData_in->con_link.master)
			{
				_common_set_icon.theData_in->con_link.master->connection_context(&ctxt);
				if (ctxt)
				{
					ctxt->unique_id_connections(&idConn);
					intptr_t idMaster = (intptr_t)idConn;
					theRep->report_command_request(req, (jvxHandle*)idMaster, 0);
				}
			}
		}
	}

jvxErrorType
CjvxInputOutputConnector::request_process_id_chain(jvxSize* procId)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	IjvxDataConnectionProcess* theProc = NULL;
	if (_common_set_icon.theData_in)
	{
		if (_common_set_icon.theData_in->con_link.master)
		{
			_common_set_icon.theData_in->con_link.master->associated_process(&theProc);
			if (theProc)
			{
				theProc->unique_id_connections(procId);
				res = JVX_NO_ERROR;
			}
		}
	}
	return res;
}

	