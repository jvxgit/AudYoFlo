#include "jvxNodes/CjvxInputOutputConnector_Vtask.h"

CjvxCommonVtask::CjvxCommonVtask(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, IjvxInputOutputConnectorVtask* report,
	IjvxConnectionMaster* aM, IjvxObject* objRef, jvxSize ctxtSubId,
	CjvxCommonVtask::vTaskConnectiorRole role ):
	CjvxCommonNVtask(ctxId, descr, par, aM, 
		objRef, ctxtSubId)
{
	_common_set_comvtask.myRole = role;
	_common_set_comvtask.cbRef = report;
}

CjvxCommonVtask::~CjvxCommonVtask()
{
}

// =======================================================================

CjvxInputConnectorVtask::CjvxInputConnectorVtask(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, IjvxInputOutputConnectorVtask* report,
	IjvxConnectionMaster* aM, IjvxObject* objRef, jvxSize ctxtSubId,
	CjvxCommonVtask::vTaskConnectiorRole role) :
	common_vtask(ctxId, descr, par, report,
		aM, objRef, ctxtSubId, role)
{
	_common_nvtask = static_cast<CjvxCommonNVtask*>(&common_vtask);
	in_proc.id = JVX_SIZE_UNSELECTED;
}

CjvxInputConnectorVtask::CjvxInputConnectorVtask(CjvxInputConnectorVtask* copyThis, jvxSize ctxtIdx,
	CjvxCommonVtask::vTaskConnectiorRole role) :
	common_vtask(
		ctxtIdx,
		copyThis->_common_nvtask->_common_set_comnvtask.descriptor.c_str(),
		copyThis->_common_nvtask->_common_set_comnvtask.myParent,
		copyThis->common_vtask._common_set_comvtask.cbRef,
		copyThis->_common_nvtask->_common_set_comnvtask.associatedMaster,
		copyThis->_common_nvtask->_common_set_comnvtask.object,
		copyThis->_common_nvtask->_common_set_comnvtask.ctxtSubId,
		role)
{
	_common_nvtask = static_cast<CjvxCommonNVtask*>(&common_vtask);
	in_proc.id = JVX_SIZE_UNSELECTED;
}

CjvxInputConnectorVtask::~CjvxInputConnectorVtask()
{
}

/*
 * This function is called on the "expose" connector and returns a "use" connector.
 * From here on, the "use" connector will be used until the unselct - which again 
 * involves the "expose" connector
 *
 */
jvxErrorType
CjvxInputConnectorVtask::select_connect_icon(
	IjvxConnectorBridge* bri, 
	IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common,
	IjvxInputConnector** replace_connector)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	if (common_vtask._common_set_comvtask.cbRef)
	{
		switch (common_vtask._common_set_comvtask.myRole)
		{
		case CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_EXPOSE:
			res = common_vtask._common_set_comvtask.cbRef->report_select_input_vtask(
				_common_nvtask->_common_set_comnvtask.ctxtId,
				_common_nvtask->_common_set_comnvtask.ctxtSubId,
				bri, master,
				ass_connection_common,
				static_cast<IjvxInputConnector*>(this),
				replace_connector);
			break;
		case CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_USE:
			assert(_common_nvtask->_common_set_comnvtask.theMaster == NULL);
			assert(_common_nvtask->_common_set_comnvtask.conBridge == NULL);
			_common_nvtask->_common_set_comnvtask.theMaster = master;
			_common_nvtask->_common_set_comnvtask.conBridge = bri;
			_common_nvtask->_common_set_comnvtask.theCommon = ass_connection_common;
			break;
		default:
			assert(0);
		}
	}	

	return res;
}

jvxErrorType
CjvxInputConnectorVtask::unselect_connect_icon(IjvxConnectorBridge* bri,
	IjvxInputConnector* replace_connector)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	if (common_vtask._common_set_comvtask.cbRef)
	{
		switch (common_vtask._common_set_comvtask.myRole)
		{
		case CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_EXPOSE:
			res = common_vtask._common_set_comvtask.cbRef->report_unselect_input_vtask(
				_common_nvtask->_common_set_comnvtask.ctxtId,
				_common_nvtask->_common_set_comnvtask.ctxtSubId,
				static_cast<IjvxInputConnector*>(this),
				replace_connector);
			break;
		case CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_USE:
			break;
		default:
			assert(0);
		}
	}

	return res;
}

jvxErrorType
CjvxInputConnectorVtask::connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	assert(common_vtask._common_set_comvtask.myRole == CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_USE);

	_common_set_icon_nvtask.theData_in = theData;

	// Detect connection loops here
	if (_common_nvtask->_common_set_comnvtask.detectLoop == true)
	{
		res = JVX_ERROR_INVALID_SETTING;
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, "Loop detected in chain connection", res);
	}
	else
	{
		if (!_common_nvtask->_common_set_comnvtask.associatedMaster) // If this pointer is non-zero, the icon is the end of a chain.
		{
			if (common_vtask._common_set_comvtask.cbRef)
			{				
				_common_nvtask->_common_set_comnvtask.detectLoop = true;
				res = common_vtask._common_set_comvtask.cbRef->connect_connect_icon_vtask(
					_common_nvtask->_common_set_comnvtask.ctxtId,
					_common_nvtask->_common_set_comnvtask.ctxtSubId					
					JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				_common_nvtask->_common_set_comnvtask.detectLoop = false;
			}
		}
	}

	return res;
}

jvxErrorType
CjvxInputConnectorVtask::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (!_common_nvtask->_common_set_comnvtask.associatedMaster) // If this pointer is non-zero, the icon is the end of a chain.
	{
		if (common_vtask._common_set_comvtask.cbRef)
		{
			_common_nvtask->_common_set_comnvtask.detectLoop = true;
			res = common_vtask._common_set_comvtask.cbRef->disconnect_connect_icon_vtask(
				_common_nvtask->_common_set_comnvtask.ctxtId,
				_common_nvtask->_common_set_comnvtask.ctxtSubId
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			_common_nvtask->_common_set_comnvtask.detectLoop = false;
		}
	}

	_common_set_icon_nvtask.theData_in = NULL;
	return res;
}

jvxErrorType
CjvxInputConnectorVtask::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, _common_nvtask->_common_set_comnvtask.object,
		(_common_nvtask->_common_set_comnvtask.descriptor + "<" +
			jvx_size2String(_common_nvtask->_common_set_comnvtask.ctxtId) + "," +
			jvx_size2String(_common_nvtask->_common_set_comnvtask.ctxtSubId) + ">").c_str(),
		"Entering CjvxInputConnectorVtask.");

	assert(common_vtask._common_set_comvtask.myRole == CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_USE);

	if (_common_nvtask->_common_set_comnvtask.associatedMaster)
	{
		if (common_vtask._common_set_comvtask.cbRef)
		{
			res = common_vtask._common_set_comvtask.cbRef->test_connect_icon_vtask(
				_common_nvtask->_common_set_comnvtask.ctxtId,
				_common_nvtask->_common_set_comnvtask.ctxtSubId
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		if (common_vtask._common_set_comvtask.cbRef)
		{
			res = common_vtask._common_set_comvtask.cbRef->test_connect_icon_vtask(
				_common_nvtask->_common_set_comnvtask.ctxtId,
				_common_nvtask->_common_set_comnvtask.ctxtSubId
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		else
		{
			return JVX_ERROR_UNSUPPORTED;
		}
	}
	return res;
}

// ==========================================================================================================

jvxErrorType
CjvxInputConnectorVtask::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (common_vtask._common_set_comvtask.cbRef)
	{
		res = common_vtask._common_set_comvtask.cbRef->prepare_connect_icon_vtask(
			_common_nvtask->_common_set_comnvtask.ctxtId,
			_common_nvtask->_common_set_comnvtask.ctxtSubId
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		return res;
	}
	return JVX_ERROR_NOT_READY;
}

jvxErrorType
CjvxInputConnectorVtask::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (common_vtask._common_set_comvtask.cbRef)
	{
		res = common_vtask._common_set_comvtask.cbRef->start_connect_icon_vtask(
			_common_nvtask->_common_set_comnvtask.ctxtId,
			_common_nvtask->_common_set_comnvtask.ctxtSubId
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		return res;
	}
	return JVX_ERROR_NOT_READY;
}

jvxErrorType
CjvxInputConnectorVtask::process_start_icon(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
		if (common_vtask._common_set_comvtask.cbRef)
		{
			res = common_vtask._common_set_comvtask.cbRef->process_start_icon_vtask(
				pipeline_offset,
				idx_stage,
				tobeAccessedByStage,
				clbk,
				priv_ptr,
				in_proc.id);
		}
	}
	return res;
}

jvxErrorType
CjvxInputConnectorVtask::process_stop_icon(
	jvxSize idx_stage,
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr )
{
	jvxErrorType res = JVX_NO_ERROR;

	if (common_vtask._common_set_comvtask.cbRef)
	{
		res = common_vtask._common_set_comvtask.cbRef->process_stop_icon_vtask(
			idx_stage,
			shift_fwd,
			tobeAccessedByStage = 0,
			clbk,
			priv_ptr,
			in_proc.id);
		return res;
	}
	return JVX_ERROR_NOT_READY;
}

jvxErrorType
CjvxInputConnectorVtask::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (common_vtask._common_set_comvtask.cbRef)
	{

		res = common_vtask._common_set_comvtask.cbRef->process_buffers_icon_vtask(			
			mt_mask,
			idx_stage,
			in_proc.id);
	}
	return res;
}

jvxErrorType
CjvxInputConnectorVtask::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (common_vtask._common_set_comvtask.cbRef)
	{
		res = common_vtask._common_set_comvtask.cbRef->stop_connect_icon_vtask(
			_common_nvtask->_common_set_comnvtask.ctxtId,
			_common_nvtask->_common_set_comnvtask.ctxtSubId
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		return res;
	}
	return JVX_ERROR_NOT_READY;
}

jvxErrorType
CjvxInputConnectorVtask::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (common_vtask._common_set_comvtask.cbRef)
	{
		res = common_vtask._common_set_comvtask.cbRef->postprocess_connect_icon_vtask(
			_common_nvtask->_common_set_comnvtask.ctxtId,
			_common_nvtask->_common_set_comnvtask.ctxtSubId
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		return res;
	}
	return JVX_ERROR_NOT_READY;
}

jvxErrorType
CjvxInputConnectorVtask::transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* data  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_icon_nvtask.theData_in)
	{
		if (_common_set_icon_nvtask.theData_in->con_link.connect_from)
		{
			res = _common_set_icon_nvtask.theData_in->con_link.connect_from->transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	return res;
}

jvxErrorType
CjvxInputConnectorVtask::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	assert(common_vtask._common_set_comvtask.myRole == CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_USE);

	if (_common_nvtask->_common_set_comnvtask.associatedMaster)
	{
		if (common_vtask._common_set_comvtask.cbRef)
		{
			res = common_vtask._common_set_comvtask.cbRef->transfer_forward_icon_vtask(
				tp,
				data,
				_common_nvtask->_common_set_comnvtask.ctxtId,
				_common_nvtask->_common_set_comnvtask.ctxtSubId
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			return res;
		}
	}
	else
	{
		if (common_vtask._common_set_comvtask.cbRef)
		{
			res = common_vtask._common_set_comvtask.cbRef->transfer_forward_icon_vtask(
				tp,
				data,
				_common_nvtask->_common_set_comnvtask.ctxtId,
				_common_nvtask->_common_set_comnvtask.ctxtSubId
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			return res;
		}
		else
		{
			return JVX_ERROR_UNSUPPORTED;
		}
	}
	return res;
}

jvxErrorType
CjvxInputConnectorVtask::number_next(jvxSize* num)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (common_vtask._common_set_comvtask.cbRef)
	{
		res = common_vtask._common_set_comvtask.cbRef->number_next_icon_vtask(
			num,
			_common_nvtask->_common_set_comnvtask.ctxtId,
			_common_nvtask->_common_set_comnvtask.ctxtSubId);
	}
	return res;

}

jvxErrorType
CjvxInputConnectorVtask::reference_next(jvxSize idx, IjvxConnectionIterator** next)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (common_vtask._common_set_comvtask.cbRef)
	{
		res = common_vtask._common_set_comvtask.cbRef->reference_next_icon_vtask(
			idx, next,
			_common_nvtask->_common_set_comnvtask.ctxtId,
			_common_nvtask->_common_set_comnvtask.ctxtSubId);
	}
	return res;
}

jvxErrorType
CjvxInputConnectorVtask::reference_component(
	jvxComponentIdentification* cpTp, 
	jvxApiString* modName, 
	jvxApiString* description,
	jvxApiString* lContext)
{
	jvxErrorType res = common_vtask._common_set_comnvtask.object->request_specialization(nullptr, cpTp, nullptr);
	res = common_vtask._common_set_comnvtask.object->module_reference(modName, nullptr);
	res = common_vtask._common_set_comnvtask.object->description(description);
	if (lContext)
	{
		lContext->assign("VTask<" +
			jvx_size2String(common_vtask._common_set_comnvtask.ctxtId) +
			"><" +
			jvx_size2String(common_vtask._common_set_comnvtask.ctxtSubId) +
			">");
	}
	
	return res;
}
// ================================================================================

CjvxOutputConnectorVtask::CjvxOutputConnectorVtask(jvxSize ctxId, 
	const char* descr,
	IjvxConnectorFactory* par, 
	IjvxInputOutputConnectorVtask* report,
	IjvxConnectionMaster* aM,
	IjvxObject* objRef,
	jvxSize ctxtSubId ,
	CjvxCommonVtask::vTaskConnectiorRole role) : 
	common_vtask(ctxId, descr, par, report, aM, objRef, ctxtSubId, role)
{
	_common_nvtask = static_cast<CjvxCommonNVtask*>(&common_vtask);
}

CjvxOutputConnectorVtask::CjvxOutputConnectorVtask(CjvxOutputConnectorVtask* copyThis, jvxSize ctxtIdx,
	CjvxCommonVtask::vTaskConnectiorRole role) :
	common_vtask(
		ctxtIdx,
		copyThis->_common_nvtask->_common_set_comnvtask.descriptor.c_str(),
		copyThis->_common_nvtask->_common_set_comnvtask.myParent,
		copyThis->common_vtask._common_set_comvtask.cbRef,
		copyThis->_common_nvtask->_common_set_comnvtask.associatedMaster,
		copyThis->_common_nvtask->_common_set_comnvtask.object,
		copyThis->_common_nvtask->_common_set_comnvtask.ctxtSubId,
		role)
{
	_common_nvtask = static_cast<CjvxCommonNVtask*>(&common_vtask);
}

jvxErrorType
CjvxOutputConnectorVtask::select_connect_ocon(IjvxConnectorBridge* bri, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common,
	IjvxOutputConnector** replace_connector)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	if (common_vtask._common_set_comvtask.cbRef)
	{
		switch (common_vtask._common_set_comvtask.myRole)
		{
		case CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_EXPOSE:
			res = common_vtask._common_set_comvtask.cbRef->report_select_output_vtask(
				_common_nvtask->_common_set_comnvtask.ctxtId,
				_common_nvtask->_common_set_comnvtask.ctxtSubId,
				bri,
				master,
				ass_connection_common,
				static_cast<IjvxOutputConnector*>(this),
				replace_connector);
			break;
		case CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_USE:
			assert(_common_nvtask->_common_set_comnvtask.theMaster == NULL);
			assert(_common_nvtask->_common_set_comnvtask.conBridge == NULL);
			_common_nvtask->_common_set_comnvtask.theMaster = master;
			_common_nvtask->_common_set_comnvtask.conBridge = bri;
			_common_nvtask->_common_set_comnvtask.theCommon = ass_connection_common;
			break;
		default:
			assert(0);
		}
	}
	return res;
}

jvxErrorType
CjvxOutputConnectorVtask::unselect_connect_ocon(
	IjvxConnectorBridge* obj, 
	IjvxOutputConnector* replace_connector)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	if (common_vtask._common_set_comvtask.cbRef)
	{
		res = common_vtask._common_set_comvtask.cbRef->report_unselect_output_vtask(
			_common_nvtask->_common_set_comnvtask.ctxtId,
			_common_nvtask->_common_set_comnvtask.ctxtSubId,
			static_cast<IjvxOutputConnector*>(this),
			replace_connector);
	}
	return res;
}

jvxErrorType
CjvxOutputConnectorVtask::test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, _common_nvtask->_common_set_comnvtask.object,
		(_common_nvtask->_common_set_comnvtask.descriptor + "<" +
			jvx_size2String(_common_nvtask->_common_set_comnvtask.ctxtId) + "," +
			jvx_size2String(_common_nvtask->_common_set_comnvtask.ctxtSubId) + ">").c_str(),
		"Entering CjvxOutputConnectorVtask.");

	if (common_vtask._common_set_comvtask.cbRef)
	{
		res = common_vtask._common_set_comvtask.cbRef->test_connect_ocon_vtask(			
			_common_nvtask->_common_set_comnvtask.ctxtId, 
			_common_nvtask->_common_set_comnvtask.ctxtSubId,
			&this->_common_set_ocon_nvtask.theData_out
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
	}
	return res;
};

jvxErrorType
CjvxOutputConnectorVtask::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, _common_nvtask->_common_set_comnvtask.object);
	if (common_vtask._common_set_comvtask.cbRef)
	{
		res = common_vtask._common_set_comvtask.cbRef->transfer_backward_ocon_vtask(
			tp, data, 
			_common_nvtask->_common_set_comnvtask.ctxtId,
			_common_nvtask->_common_set_comnvtask.ctxtSubId
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}	
	return res;
}

