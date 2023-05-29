#include "common/CjvxInputConnectorCore.h"
#include "CjvxJson.h"
#include "HjvxMisc.h"
#include "common/CjvxInputOutputConnectorCore.h"

CjvxInputConnectorCore::CjvxInputConnectorCore()
{
	_common_set_icon.conn_in = NULL;
	_common_set_icon.theCommon_to = NULL;
	_common_set_icon.theData_in = NULL;
	_common_set_icon.icon = NULL;

}

jvxErrorType 
CjvxInputConnectorCore::activate(CjvxInputOutputConnectorCore* commRef, IjvxInputConnector* icon)
{
	_common_set_io_common_ptr = commRef;
	_common_set_icon.icon = icon;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxInputConnectorCore::deactivate()
{
	_common_set_icon.icon = nullptr;
	return JVX_NO_ERROR;
}

// ===============================================================

jvxErrorType
CjvxInputConnectorCore::_associated_connection_icon(IjvxDataConnectionCommon** ref)
{
	if (ref)
	{
		*ref = _common_set_icon.theCommon_to;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxInputConnectorCore::_available_to_connect_icon()
{
	if (_common_set_icon.theCommon_to == nullptr)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CjvxInputConnectorCore::_supports_connector_class_icon(
	jvxDataFormatGroup format_group,
	jvxDataflow data_flow)
{
	if (_common_set_icon.caps_in.format_group != JVX_DATAFORMAT_GROUP_NONE)
	{
		if (_common_set_icon.caps_in.format_group != format_group)
		{
			return JVX_ERROR_UNSUPPORTED;
		}
	}
	if (_common_set_icon.caps_in.data_flow != JVX_DATAFLOW_NONE)
	{
		if (_common_set_icon.caps_in.data_flow != data_flow)
		{
			return JVX_ERROR_UNSUPPORTED;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxInputConnectorCore::_select_connect_icon(IjvxConnectorBridge* bri, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common,
	IjvxInputConnector** replace_connector)
{
	// To and from conector must be associated in the same process/group
	if (_common_set_icon.theCommon_to != NULL)
	{
		return JVX_ERROR_ALREADY_IN_USE;
	}

	// Here, we can check more generally the status before connect
	jvxErrorType res = _check_common_icon(ass_connection_common, master);
	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	if (_common_set_icon.conn_in == NULL)
	{
		_common_set_icon.conn_in = bri;
		_common_set_icon.theCommon_to = ass_connection_common;

		// What else to do here?
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_COMPONENT_BUSY;
}

jvxErrorType
CjvxInputConnectorCore::_unselect_connect_icon(IjvxConnectorBridge* bri, IjvxInputConnector* replace_connector)
{
	if (_common_set_icon.conn_in == bri)
	{
		_common_set_icon.conn_in = NULL;
		_common_set_icon.theCommon_to = NULL;

		// What else to do here?
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxInputConnectorCore::_connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb), jvxBool forward)
{
	jvxErrorType res = JVX_NO_ERROR;
	_common_set_icon.theData_in = theData;

	if (forward)
	{

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
		// Forward to "internal" output connector or otherwise
		res = _connect_connect_forward(JVX_CONNECTION_FEEDBACK_CALL(fdb));

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
	}
	else
	{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering & Leaving " << __FUNCTION__ << std::endl;
#endif
	}
	return res;
}

jvxErrorType
CjvxInputConnectorCore::_disconnect_connect_icon(jvxLinkDataDescriptor* theData, jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (forward)
	{
		res = _disconnect_connect_forward(JVX_CONNECTION_FEEDBACK_CALL(fdb));		
	}
	_common_set_icon.theData_in = NULL;
	return res;
}

// ===================================================================================

jvxErrorType
CjvxInputConnectorCore::_test_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(_common_set_io_common_ptr->_common_set_io_common.object),
		_common_set_io_common_ptr->_common_set_io_common.descriptor.c_str(), "Entering single default CjvxInputOutputConnector input connector");

	JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_I(fdb, _common_set_icon.theData_in);
	if (forward)
	{
		res = _test_connect_forward(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

// ===================================================================================

jvxErrorType
CjvxInputConnectorCore::_prepare_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (forward)
	{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
		res = _prepare_connect_forward(JVX_CONNECTION_FEEDBACK_CALL(fdb));

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
	}
	else
	{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering & Leaving " << __FUNCTION__ << std::endl;
#endif
	}
	return res;
}

jvxErrorType
CjvxInputConnectorCore::_postprocess_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (forward)
	{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
		res = _postprocess_connect_forward(JVX_CONNECTION_FEEDBACK_CALL(fdb));

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
	}
	else
	{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering & Leaving " << __FUNCTION__ << std::endl;
#endif
	}
	return res;
}

// ==============================================================================
jvxErrorType
CjvxInputConnectorCore::_start_connect_icon(jvxBool forward, jvxSize myUniquePipelineId JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	_start_connect_common(myUniquePipelineId);

	if (forward)
	{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
		res = _start_connect_forward(JVX_CONNECTION_FEEDBACK_CALL(fdb));

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
	}
	else
	{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering & Leaving " << __FUNCTION__ << std::endl;
#endif
	}
	return res;
}

jvxErrorType
CjvxInputConnectorCore::_stop_connect_icon(jvxBool forward, jvxSize* uId JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (forward)
	{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
		res = _stop_connect_forward(JVX_CONNECTION_FEEDBACK_CALL(fdb));

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
	}
	else
	{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering & Leaving " << __FUNCTION__ << std::endl;
#endif
	}

	_stop_connect_common(uId);

	return res;
}

// ===========================================================================

jvxErrorType
CjvxInputConnectorCore::JVX_CALLINGCONVENTION _transfer_forward_icon(jvxBool forward, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
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
			res = _transfer_forward_forward(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		else
		{
			locTxt += "<end>";
		}
		str->assign(locTxt);
		break;

	case JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON:

		if (_common_set_icon.icon)
		{
			jsonElm.makeAssignmentString("icon", JVX_DISPLAY_CONNECTOR(_common_set_icon.icon));
			jsonLst->addConsumeElement(jsonElm);
		}
		if (forward)
		{
			res = _transfer_forward_forward(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));			
		}
		else
		{
			CjvxJsonElementList jsonLstRet;
			jsonElm.makeAssignmentString("connects_end", "here");
			jsonLst->addConsumeElement(jsonElm);
		}
		break;

	case JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY:

		// This is supported
		if (forward)
		{
			res = _transfer_forward_forward(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		break;

	default:
		if (forward)
		{
			res = _transfer_forward_forward(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));			
		}
		break;
	}
	return res;
}

jvxErrorType
CjvxInputConnectorCore::JVX_CALLINGCONVENTION _transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, _common_set_io_common_ptr->_common_set_io_common.object,
		_common_set_io_common_ptr->_common_set_io_common.descriptor.c_str(), "Entering transfer_backward on input onnector");
	if (_common_set_icon.theData_in->con_link.connect_from)
	{
		return _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return JVX_ERROR_UNSUPPORTED;
}

// =============================================================================

jvxErrorType
CjvxInputConnectorCore::_process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage, jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk, jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	// Propagate to next object
	res = _process_start_forward();

	if (res == JVX_NO_ERROR)
	{
		return  jvx_shift_buffer_pipeline_idx_on_start(_common_set_icon.theData_in, 
			_common_set_io_common_ptr->_common_set_io_common.myRuntimeId,
			pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	}
	return res;
}

jvxErrorType
CjvxInputConnectorCore::_process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	res = _process_stop_forward(JVX_SIZE_UNSELECTED, true, tobeAccessedByStage, clbk, priv_ptr);

	if (res == JVX_NO_ERROR)
	{
		res = jvx_shift_buffer_pipeline_idx_on_stop(_common_set_icon.theData_in, idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);
	}
	return res;
}

jvxErrorType
CjvxInputConnectorCore::_process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	res = _process_buffers_forward(mt_mask, idx_stage);
	return res;
}

// =============================================================================

jvxErrorType
CjvxInputConnectorCore::_connected_ocon(IjvxOutputConnector** ocon)
{
	if (ocon)
	{
		*ocon = nullptr;
		if (_common_set_icon.theData_in)
		{
			*ocon = _common_set_icon.theData_in->con_link.connect_from;
		}
	}
	return JVX_NO_ERROR;
}


jvxErrorType
CjvxInputConnectorCore::shift_buffer_pipeline_idx_on_start(jvxSize pipeline_offset,
	jvxSize* idx_stage,	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk, jvxHandle* priv_ptr)
{
	return  jvx_shift_buffer_pipeline_idx_on_start(_common_set_icon.theData_in, _common_set_io_common_ptr->_common_set_io_common.myRuntimeId,
		pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
}

jvxErrorType 
CjvxInputConnectorCore::shift_buffer_pipeline_idx_on_stop(jvxSize idx_stage,
	jvxBool shift_fwd, jvxSize tobeAccessed,
	callback_process_stop_in_lock clbk, jvxHandle* priv_ptr)
{
	return jvx_shift_buffer_pipeline_idx_on_stop(
		_common_set_icon.theData_in, idx_stage,
		shift_fwd, tobeAccessed,
		clbk, priv_ptr);
}

jvxErrorType 
CjvxInputConnectorCore::allocate_pipeline_and_buffers_prepare_to()
{

	return jvx_allocate_pipeline_and_buffers_prepare_to(_common_set_icon.theData_in
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		, _common_set_ldslave.dbg_hint
#endif
	);
};

jvxErrorType
CjvxInputConnectorCore::deallocate_pipeline_and_buffers_postprocess_to()
{
	return jvx_deallocate_pipeline_and_buffers_postprocess_to(_common_set_icon.theData_in);
}

jvxErrorType
CjvxInputConnectorCore::deallocate_pipeline_and_buffers_postprocess_to_zerocopy()
{
	return jvx_deallocate_pipeline_and_buffers_postprocess_to_zerocopy(_common_set_icon.theData_in);
}
