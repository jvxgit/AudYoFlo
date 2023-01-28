#include "jvxNodes/CjvxInputOutputConnector_NVtask.h"
#include "CjvxJson.h"

CjvxCommonNVtask::CjvxCommonNVtask(
	jvxSize ctxId, 
	const char* descr, 
	IjvxConnectorFactory* par,
	IjvxConnectionMaster* aM,
	IjvxObject* objRef,
	jvxSize ctxtSubId)
{
	_common_set_comnvtask.detectLoop = false;
	_common_set_comnvtask.object = objRef;
	_common_set_comnvtask.descriptor = descr;
	_common_set_comnvtask.myParent = par;
	_common_set_comnvtask.myRuntimeId = JVX_SIZE_UNSELECTED;
	_common_set_comnvtask.theCommon = NULL;
	_common_set_comnvtask.theMaster = NULL;
	_common_set_comnvtask.conBridge = NULL;
	_common_set_comnvtask.ctxtTypeId = ctxId; 
	_common_set_comnvtask.ctxtId = ctxId;
	_common_set_comnvtask.associatedMaster = aM;
	_common_set_comnvtask.ctxtSubId = ctxtSubId;
}

void
CjvxCommonNVtask::_modify_ctxtid(jvxSize cxtIdArg)
{
	_common_set_comnvtask.ctxtTypeId = cxtIdArg;
}

jvxErrorType
CjvxCommonNVtask::_descriptor_connector(jvxApiString* str)
{
	if (str)
	{
		str->assign(_common_set_comnvtask.descriptor);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxCommonNVtask::_parent_factory(IjvxConnectorFactory** my_parent)
{
	if (my_parent)
	{
		*my_parent = _common_set_comnvtask.myParent;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxCommonNVtask::_associated_common(IjvxDataConnectionCommon** ref)
{
	if (ref)
	{
		*ref = _common_set_comnvtask.theCommon;
	}
	return JVX_NO_ERROR;
}

// ==========================================================

CjvxInputConnectorNVtask::CjvxInputConnectorNVtask()
{
	//std::vector<IjvxOutputConnector*> ocons;
	_common_set_icon_nvtask.theData_in = NULL;
	_common_nvtask = NULL;
}

jvxErrorType
CjvxInputConnectorNVtask::accepts_data_format_group(jvxDataFormatGroup grp)
{
	if (grp == _common_set_icon_nvtask.format_group_in == grp)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

// ==========================================================

CjvxOutputConnectorNVtask::CjvxOutputConnectorNVtask()
{
	_common_nvtask = NULL;
}

jvxErrorType 
CjvxOutputConnectorNVtask::outputs_data_format_group(jvxDataFormatGroup grp)
{
	if (grp == _common_set_ocon_nvtask.format_group_out == grp)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxOutputConnectorNVtask::connected_icon(IjvxInputConnector** icon)
{
	if (icon)
	{
		*icon = _common_set_ocon_nvtask.theData_out.con_link.connect_to;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxOutputConnectorNVtask::_connect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ocon_nvtask.theData_out.con_link.connect_to == NULL)
	{
		if (_common_nvtask->_common_set_comnvtask.conBridge)
		{
			_common_nvtask->_common_set_comnvtask.conBridge->reference_connect_to(&_common_set_ocon_nvtask.theData_out.con_link.connect_to);
			assert(_common_set_ocon_nvtask.theData_out.con_link.connect_to);

			_common_set_ocon_nvtask.theData_out.con_link.uIdConn = args.uIdConnection;
			_common_set_ocon_nvtask.theData_out.con_link.master = args.theMaster;
			_common_set_ocon_nvtask.theData_out.con_link.connect_from = this;
			res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->connect_connect_icon(&_common_set_ocon_nvtask.theData_out JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (res != JVX_NO_ERROR)
			{
				_common_nvtask->_common_set_comnvtask.conBridge->return_reference_connect_to(_common_set_ocon_nvtask.theData_out.con_link.connect_to);
				jvx_terminateDataLinkDescriptor(&_common_set_ocon_nvtask.theData_out);
			}
		}
	}
	else
	{
		std::string txt = "This output connector is not connected to a connection bridge.";
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), JVX_ERROR_NOT_READY);
		res = JVX_ERROR_NOT_READY;
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNVtask::_disconnect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
	{
		// Identify the connector from which we connected
		assert(_common_nvtask->_common_set_comnvtask.conBridge);

		res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->disconnect_connect_icon(
			&_common_set_ocon_nvtask.theData_out JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		_common_nvtask->_common_set_comnvtask.conBridge->return_reference_connect_to(_common_set_ocon_nvtask.theData_out.con_link.connect_to);
		jvx_terminateDataLinkDescriptor(&_common_set_ocon_nvtask.theData_out);
	}
	else
	{
		// No connection established before
	}
	return res;
}

// =================================================================================

jvxErrorType
CjvxOutputConnectorNVtask::_prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNVtask::_postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNVtask::_start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNVtask::_stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNVtask::_process_start_ocon(jvxSize pipeline_offset, jvxSize* idx_stage, jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	// Propagate to next object
	if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->process_start_icon(pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNVtask::_process_stop_ocon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	// Propagate to next object
	if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->process_stop_icon(
			idx_stage, shift_fwd,
			tobeAccessedByStage,
			clbk, priv_ptr);
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNVtask::_process_buffers_ocon(
	jvxSize mt_mask,
	jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	// Propagate to next object
	if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->process_buffers_icon(
			mt_mask,
			idx_stage);
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNVtask::_transfer_forward_ocon(jvxLinkDataTransferType tp, 
	jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
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
		locTxt += "object output connector " + JVX_DISPLAY_CONNECTOR(this);

		if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
		{
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
			locTxt += "-<ext>->";
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
			str->assign(locTxt);

			res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->transfer_forward_icon(tp,
				data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

			locTxt = str->std_str();
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
			locTxt += "<-<txe>-";
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
			str->assign(locTxt);
			return res;
		}
		locTxt += "-<not_connected>";
		str->assign(locTxt);
		break;
	case JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON:

		jsonElm.makeAssignmentString("ocon", JVX_DISPLAY_CONNECTOR(this));
		jsonLst->addConsumeElement(jsonElm);

		if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
		{
			CjvxJsonElementList jsonLstRet;
			res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->transfer_forward_icon(tp,
				&jsonLstRet JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

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
		break;
	default:
		if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->transfer_forward_icon(tp,
				data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		break;
	}

	return res;
}
