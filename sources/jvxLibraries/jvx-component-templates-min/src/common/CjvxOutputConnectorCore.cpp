#include "common/CjvxOutputConnectorCore.h"
#include "CjvxJson.h"
#include "common/CjvxInputOutputConnectorCore.h"

CjvxOutputConnectorCore::CjvxOutputConnectorCore()
{
	_common_set_ocon.conn_out = NULL;
	_common_set_ocon.theCommon_from = NULL;
	_common_set_ocon.allows_termination = true;
	_common_set_ocon.setup_for_termination = false;
	_common_set_ocon.ocon = NULL;

	jvx_initDataLinkDescriptor(&_common_set_ocon.theData_out);
}

jvxErrorType 
CjvxOutputConnectorCore::activate(CjvxInputOutputConnectorCore* commRef, IjvxOutputConnector* ocon)
{
	_common_set_io_common_ptr = commRef;
	_common_set_ocon.ocon = ocon;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxOutputConnectorCore::deactivate()
{
	_common_set_ocon.ocon = nullptr;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxOutputConnectorCore::_associated_common_ocon(IjvxDataConnectionCommon** ref)
{
	if (ref)
	{
		*ref = _common_set_ocon.theCommon_from;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxOutputConnectorCore::_supports_connector_class_ocon(
	jvxDataFormatGroup format_group,
	jvxDataflow data_flow)
{
	if (_common_set_ocon.caps_out.format_group != JVX_DATAFORMAT_GROUP_NONE)
	{
		if (_common_set_ocon.caps_out.format_group != format_group)
		{
			return JVX_ERROR_UNSUPPORTED;
		}
	}
	if (_common_set_ocon.caps_out.data_flow != JVX_DATAFLOW_NONE)
	{
		if (_common_set_ocon.caps_out.data_flow != data_flow)
		{
			return JVX_ERROR_UNSUPPORTED;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxOutputConnectorCore::_select_connect_ocon(IjvxConnectorBridge* bri, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common, IjvxOutputConnector** replace_connector)
{
	// To and from conector must be associated in the same process/group
	if (_common_set_ocon.theCommon_from != NULL)
	{
		return JVX_ERROR_ALREADY_IN_USE;
	}

	jvxErrorType res = _check_common_ocon(ass_connection_common, master);
	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	// If no bridge was set before
	if (_common_set_ocon.conn_out == NULL)
	{
		_common_set_ocon.conn_out = bri;
		_common_set_ocon.theCommon_from = ass_connection_common;

		// What else to do here?
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_COMPONENT_BUSY;
}

jvxErrorType
CjvxOutputConnectorCore::_unselect_connect_ocon(IjvxConnectorBridge* bri, IjvxOutputConnector* replace_connector)
{
	if (_common_set_ocon.conn_out)
	{
		_common_set_ocon.conn_out = NULL;
		_common_set_ocon.theCommon_from = NULL;

		// What else to do here?
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

// ===============================================================================

jvxErrorType
CjvxOutputConnectorCore::_connect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb), jvxDataProcessorHintDescriptor* add_me)
{
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
	if (_common_set_ocon.conn_out)
	{
		jvx_initDataLinkDescriptor(&_common_set_ocon.theData_out);
		assert(_common_set_ocon.theData_out.con_link.connect_to == NULL);
		_common_set_ocon.conn_out->reference_connect_to(&_common_set_ocon.theData_out.con_link.connect_to);
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			_common_set_ocon.theData_out.con_link.master = args.theMaster;
			_common_set_ocon.theData_out.con_link.uIdConn = args.uIdConnection;
			_common_set_ocon.theData_out.con_link.connect_from = _common_set_ocon.ocon;
			_common_set_ocon.theData_out.con_user.chain_spec_user_hints = add_me;

			res = _common_set_ocon.theData_out.con_link.connect_to->connect_connect_icon(&_common_set_ocon.theData_out 
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (res != JVX_NO_ERROR)
			{
				_common_set_ocon.conn_out->return_reference_connect_to(_common_set_ocon.theData_out.con_link.connect_to);
				_common_set_ocon.theData_out.con_link.connect_to = NULL;
				jvx_terminateDataLinkDescriptor(&_common_set_ocon.theData_out);
			}
		}
		else
		{
			assert(0);
			if (!_common_set_ocon.allows_termination)
			{
				std::cout << "This output connector does not allow termination but is not connected to a connected bridge (no input connector in bridge)." << std::endl;
				res = JVX_ERROR_NOT_READY;
			}
		}
	}
	else
	{
		if (!_common_set_ocon.allows_termination)
		{
			std::cout << "This output connector does not allow termination but is not connected to a connected bridge (no input connector in bridge)." << std::endl;
			res = JVX_ERROR_NOT_READY;
		}
	}
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
	return res;
}

jvxErrorType
CjvxOutputConnectorCore::_disconnect_connect_ocon(const jvxChainConnectArguments& args, JVX_CONNECTION_FEEDBACK_TYPE_F(fdb) jvxDataProcessorHintDescriptor** pointer_remove)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ocon.conn_out)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->disconnect_connect_icon(&_common_set_ocon.theData_out JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (pointer_remove)
		{
			*pointer_remove = _common_set_ocon.theData_out.con_user.chain_spec_user_hints;
		}
		_common_set_ocon.conn_out->return_reference_connect_to(_common_set_ocon.theData_out.con_link.connect_to);
		_common_set_ocon.theData_out.con_link.connect_to = nullptr;
		jvx_terminateDataLinkDescriptor(&_common_set_ocon.theData_out);
	}
	else
	{
		std::cout << "Not connected" << std::endl;
	}
	return res;
}

// ===============================================================================

jvxErrorType
CjvxOutputConnectorCore::_test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(_common_set_io_common_ptr->_common_set_io_common.object),
		_common_set_io_common_ptr->_common_set_io_common.descriptor.c_str(), "Leaving single default CjvxInputOutputConnector output connector");
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

// ===============================================================================

jvxErrorType
CjvxOutputConnectorCore::_transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
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

jvxErrorType
CjvxOutputConnectorCore::JVX_CALLINGCONVENTION _transfer_backward_ocon(jvxBool forward, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, _common_set_io_common_ptr->_common_set_io_common.object,
		_common_set_io_common_ptr->_common_set_io_common.descriptor.c_str(), "Entering transfer_backward on output onnector");

	// Check if the requested operation can be fulfilled
	if (tp == JVX_LINKDATA_TRANSFER_REQUEST_CHAIN_SPECIFIC_DATA_HINT)
	{
		if (_common_set_ocon.theData_out.con_user.chain_spec_user_hints)
		{
			jvxDataProcessorHintDescriptorSearch* ptr = (jvxDataProcessorHintDescriptorSearch*)data;
			if (data)
			{
				res = jvx_hintDesriptor_find(_common_set_ocon.theData_out.con_user.chain_spec_user_hints,
					&ptr->reference, ptr->hintDescriptorId, ptr->assocHint);
				if (res == JVX_NO_ERROR)
				{
					return res;
				}
			}
		}
	}

	// If it can not be fulfilled, we try it towards previous element
	if (forward)
	{
		// We return immediately
		res = _transfer_backward_backward(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		return res;
	}

	// Only if there was no previous element and we could not find the requested transfer we return an error
	if (tp == JVX_LINKDATA_TRANSFER_REQUEST_CHAIN_SPECIFIC_DATA_HINT)
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return res;
}


jvxErrorType
CjvxOutputConnectorCore::_prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Content of link descriptor has been set before...
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
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
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif

	return res;
};

jvxErrorType
CjvxOutputConnectorCore::_postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Content of link descriptor has been set before...
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
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

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif

	return res;
}

// ===============================================================================

jvxErrorType
CjvxOutputConnectorCore::_start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Content of link descriptor has been set before...
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
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

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif

	return res;
}

jvxErrorType
CjvxOutputConnectorCore::_stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Content of link descriptor has been set before...
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
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
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif

	return res;
}

// ===============================================================================

jvxErrorType
CjvxOutputConnectorCore::_process_start_ocon(jvxSize pipeline_offset, jvxSize* idx_stage, jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
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

jvxErrorType
CjvxOutputConnectorCore::_process_buffers_ocon(jvxSize mt_mask, jvxSize idx_stage)
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

jvxErrorType
CjvxOutputConnectorCore::_process_stop_ocon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
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

// ===============================================================================

jvxErrorType
CjvxOutputConnectorCore::_connected_icon(IjvxInputConnector** ocon)
{
	if (ocon)
	{
		*ocon = nullptr;
		*ocon = _common_set_ocon.theData_out.con_link.connect_to;
	}
	return JVX_NO_ERROR;
}




