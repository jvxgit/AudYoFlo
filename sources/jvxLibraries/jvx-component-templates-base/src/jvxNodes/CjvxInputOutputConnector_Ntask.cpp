#include "jvxNodes/CjvxInputOutputConnector_Ntask.h"
#include "CjvxJson.h"


// =======================================================================
// =======================================================================

CjvxCommonNtask::CjvxCommonNtask(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, IjvxInputOutputConnectorNtask* report,
	IjvxConnectionMaster* aM,
	IjvxObject* objRef,
	jvxSize ctxtSubId) : CjvxCommonNVtask(ctxId,
		descr,
		par,
		aM,
		objRef,
		ctxtSubId)
{
	_common_set_comntask.cbRef = report;
}

// ==========================================================================================================
// ==========================================================================================================
// ==========================================================================================================
	
CjvxInputConnectorNtask::CjvxInputConnectorNtask(jvxSize ctxId,
	const char* descr,
	IjvxConnectorFactory* par,
	IjvxInputOutputConnectorNtask* report,
	IjvxConnectionMaster* aM,
	IjvxObject* objRef,
	jvxSize ctxtSubId) :
	CjvxInputConnectorNVtask(),
	common_ntask(ctxId, descr, par, report, aM, objRef, ctxtSubId)
{
	//std::vector<IjvxOutputConnector*> ocons;
	_common_set_icon_ntask.ocon = NULL;
	_common_nvtask = static_cast<CjvxCommonNVtask*>(&common_ntask);
	JVX_INITIALIZE_MUTEX(safeAccessConnection);
}

CjvxInputConnectorNtask::~CjvxInputConnectorNtask()
{
	 JVX_TERMINATE_MUTEX(safeAccessConnection);
}

jvxErrorType 
CjvxInputConnectorNtask::set_reference(CjvxOutputConnectorNtask*ref)
{
	jvxBool duplicateEntry = false;
	if(_common_set_icon_ntask.ocon == NULL)
	{
		_common_set_icon_ntask.ocon = ref;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}
	
// =======================================================================================================
// =======================================================================================================

jvxErrorType
CjvxInputConnectorNtask::select_connect_icon(IjvxConnectorBridge* bri, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common,
	IjvxInputConnector** replace_connector )
{
	jvxBool detectedProblem = false;
	// To and from conector must be associated in the same process/group
	if (_common_nvtask->_common_set_comnvtask.theCommon != NULL)
	{
		return JVX_ERROR_ALREADY_IN_USE;
	}

	// Make sure, all connections are associated to the same process or group
	if (_common_set_icon_ntask.ocon->_common_nvtask->_common_set_comnvtask.theCommon != NULL)
	{
		if (ass_connection_common != _common_set_icon_ntask.ocon->common_ntask._common_set_comnvtask.theCommon)
		{
			detectedProblem = true;
		}
	}
	if (detectedProblem)
	{
		return JVX_ERROR_INVALID_SETTING;
	}

	// If we are linked with a master since we are a master with connectors, passed master must match
	if (master && _common_nvtask->_common_set_comnvtask.theMaster)
	{
		if (master != _common_nvtask->_common_set_comnvtask.theMaster)
		{
			return JVX_ERROR_INVALID_ARGUMENT;
		}
	}

	if (_common_nvtask->_common_set_comnvtask.conBridge == NULL)
	{
		_common_nvtask->_common_set_comnvtask.conBridge = bri;
		_common_nvtask->_common_set_comnvtask.theCommon = ass_connection_common;

		// What else to do here?
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_COMPONENT_BUSY;
}

jvxErrorType
CjvxInputConnectorNtask::unselect_connect_icon(IjvxConnectorBridge* bri,
	IjvxInputConnector* replace_connector)
{
	if (_common_nvtask->_common_set_comnvtask.conBridge == bri)
	{
		_common_nvtask->_common_set_comnvtask.conBridge = NULL;
		_common_nvtask->_common_set_comnvtask.theCommon = NULL;

		// What else to do here?
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxInputConnectorNtask::connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool runDisconnect = false;

	// In the NTask module the connection may be connected while already in operation. In that case we need to make sure there
	// is no race condition
	JVX_LOCK_MUTEX(safeAccessConnection);

	_common_set_icon_nvtask.theData_in = theData;

	// Detect connection loops here
	if (_common_nvtask->_common_set_comnvtask.detectLoop == true)
	{
		res = JVX_ERROR_INVALID_SETTING;
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, "Loop detected in chain connection", res);
	}
	else
	{
		// If there is an associated master this input connector is the last element in the chain
		// If the pointer is null, elements may follow in chain
		if (!_common_nvtask->_common_set_comnvtask.associatedMaster) 
		{
			_common_nvtask->_common_set_comnvtask.detectLoop = true;
			res = _common_set_icon_ntask.ocon->connect_connect_ocon(
				jvxChainConnectArguments(
					_common_set_icon_nvtask.theData_in->con_link.uIdConn,
				_common_set_icon_nvtask.theData_in->con_link.master) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			_common_nvtask->_common_set_comnvtask.detectLoop = false;
		}
	}

	if (res == JVX_NO_ERROR)
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			if (_common_nvtask->_common_set_comnvtask.associatedMaster)
			{
				 res = common_ntask._common_set_comntask.cbRef->connect_connect_ntask(
					//_common_set_icon_nvtask.theData_in->con_link.master, 
					 _common_set_icon_nvtask.theData_in,
					 nullptr,
					_common_nvtask->_common_set_comnvtask.ctxtId);
			}
			else
			{				
				res = common_ntask._common_set_comntask.cbRef->connect_connect_ntask(
					//_common_set_icon_nvtask.theData_in->con_link.master, 
					_common_set_icon_nvtask.theData_in,
					&_common_set_icon_ntask.ocon->_common_set_ocon_nvtask.theData_out,
					_common_nvtask->_common_set_comnvtask.ctxtId);
			}

			if (res != JVX_NO_ERROR)
			{
				runDisconnect = true;
			}
		}
	}
	else
	{
		// First part failed..
		_common_set_icon_nvtask.theData_in = nullptr;
	}
	JVX_UNLOCK_MUTEX(safeAccessConnection);

	if (runDisconnect)
	{
		// Undo all following connections (if established)
		disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType
CjvxInputConnectorNtask::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// In the NTask module the connection may be connected while already in operation. In that case we need to make sure there
	// is no race condition
	JVX_LOCK_MUTEX(safeAccessConnection);

	if (res == JVX_NO_ERROR)
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			if (_common_nvtask->_common_set_comnvtask.associatedMaster)
			{
				common_ntask._common_set_comntask.cbRef->disconnect_connect_ntask(
					_common_set_icon_nvtask.theData_in,
					nullptr,
					_common_nvtask->_common_set_comnvtask.ctxtId);
			}
			else
			{
				common_ntask._common_set_comntask.cbRef->disconnect_connect_ntask(
					_common_set_icon_nvtask.theData_in,
					&_common_set_icon_ntask.ocon->_common_set_ocon_nvtask.theData_out,
					_common_nvtask->_common_set_comnvtask.ctxtId);
			}
		}
	}

	if (!_common_nvtask->_common_set_comnvtask.associatedMaster)
	{
		res = _common_set_icon_ntask.ocon->disconnect_connect_ocon(jvxChainConnectArguments(
			_common_set_icon_nvtask.theData_in->con_link.uIdConn,
			_common_set_icon_nvtask.theData_in->con_link.master) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	_common_set_icon_nvtask.theData_in = NULL;

	JVX_UNLOCK_MUTEX(safeAccessConnection);
	
	return res;
}

jvxErrorType
CjvxInputConnectorNtask::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, 
		_common_nvtask->_common_set_comnvtask.object,
		_common_nvtask->_common_set_comnvtask.descriptor.c_str(), 
		"Entering input ntask connector.");

	if (_common_nvtask->_common_set_comnvtask.associatedMaster)
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->test_connect_icon_ntask(
				_common_set_icon_nvtask.theData_in, NULL, _common_nvtask->_common_set_comnvtask.ctxtId, NULL
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->test_connect_icon_ntask(
				_common_set_icon_nvtask.theData_in,
				&_common_set_icon_ntask.ocon->_common_set_ocon_nvtask.theData_out,
				_common_nvtask->_common_set_comnvtask.ctxtId,
				_common_set_icon_ntask.ocon
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		else
		{
			res = _common_set_icon_ntask.ocon->test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
	}

	return res;
}

jvxErrorType
CjvxInputConnectorNtask::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_nvtask->_common_set_comnvtask.associatedMaster)
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->prepare_connect_icon_ntask(_common_set_icon_nvtask.theData_in,
				NULL, _common_nvtask->_common_set_comnvtask.ctxtId, NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->prepare_connect_icon_ntask(_common_set_icon_nvtask.theData_in,
				&_common_set_icon_ntask.ocon->_common_set_ocon_nvtask.theData_out,
				_common_nvtask->_common_set_comnvtask.ctxtId,
				_common_set_icon_ntask.ocon
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		else
		{
			res = _common_set_icon_ntask.ocon->prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
	}

	return res;
}

jvxErrorType
CjvxInputConnectorNtask::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_nvtask->_common_set_comnvtask.associatedMaster)
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->start_connect_icon_ntask(_common_set_icon_nvtask.theData_in,
				NULL, _common_nvtask->_common_set_comnvtask.ctxtId, NULL	JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->start_connect_icon_ntask(_common_set_icon_nvtask.theData_in,
				&_common_set_icon_ntask.ocon->_common_set_ocon_nvtask.theData_out,
				_common_nvtask->_common_set_comnvtask.ctxtId,
				_common_set_icon_ntask.ocon
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		else
		{
			res = _common_set_icon_ntask.ocon->start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
	}
	return res;
}

jvxErrorType
CjvxInputConnectorNtask::process_start_icon(
	jvxSize pipeline_offset, 
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (!_common_nvtask->_common_set_comnvtask.associatedMaster)
	{
		res = _common_set_icon_ntask.ocon->process_start_ocon(pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	}

	if (res == JVX_NO_ERROR)
	{
		res = jvx_shift_buffer_pipeline_idx_on_start(_common_set_icon_nvtask.theData_in, _common_nvtask->_common_set_comnvtask.myRuntimeId,
			pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	}
	return res;
}

jvxErrorType
CjvxInputConnectorNtask::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_nvtask->_common_set_comnvtask.associatedMaster)
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->process_buffers_icon_ntask(_common_set_icon_nvtask.theData_in,
				NULL, _common_nvtask->_common_set_comnvtask.ctxtId, NULL, mt_mask, idx_stage
			);
		}
	}
	else
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->process_buffers_icon_ntask(_common_set_icon_nvtask.theData_in,
				&_common_set_icon_ntask.ocon->_common_set_ocon_nvtask.theData_out,
				_common_nvtask->_common_set_comnvtask.ctxtId,
				_common_set_icon_ntask.ocon,
				mt_mask, idx_stage
			);
		}
		else
		{
			res = _common_set_icon_ntask.ocon->process_buffers_ocon(mt_mask, idx_stage);
		}
	}

	return res;
}

jvxErrorType
CjvxInputConnectorNtask::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd, jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk , jvxHandle* priv_ptr )
{
	jvxErrorType res = JVX_NO_ERROR;

	if (!_common_nvtask->_common_set_comnvtask.associatedMaster)
	{
		res = _common_set_icon_ntask.ocon->process_stop_ocon(idx_stage, shift_fwd,
			tobeAccessedByStage, clbk, priv_ptr);
	}

	if (res == JVX_NO_ERROR)
	{
		res = jvx_shift_buffer_pipeline_idx_on_stop(_common_set_icon_nvtask.theData_in,
			idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);
	}
	return res;
}

jvxErrorType
CjvxInputConnectorNtask::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_nvtask->_common_set_comnvtask.associatedMaster)
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->stop_connect_icon_ntask(_common_set_icon_nvtask.theData_in,
				NULL,_common_nvtask->_common_set_comnvtask.ctxtId, NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->stop_connect_icon_ntask(_common_set_icon_nvtask.theData_in,
				&_common_set_icon_ntask.ocon->_common_set_ocon_nvtask.theData_out,
				_common_nvtask->_common_set_comnvtask.ctxtId, _common_set_icon_ntask.ocon JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		else
		{
			res = _common_set_icon_ntask.ocon->stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
	}

	return res;
}

jvxErrorType
CjvxInputConnectorNtask::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_nvtask->_common_set_comnvtask.associatedMaster)
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->postprocess_connect_icon_ntask(_common_set_icon_nvtask.theData_in,
				NULL, _common_nvtask->_common_set_comnvtask.ctxtId, NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->postprocess_connect_icon_ntask(_common_set_icon_nvtask.theData_in,
				&_common_set_icon_ntask.ocon->_common_set_ocon_nvtask.theData_out,
				_common_nvtask->_common_set_comnvtask.ctxtId, _common_set_icon_ntask.ocon JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		else
		{
			res = _common_set_icon_ntask.ocon->postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
	}
	return res;
}

jvxErrorType
CjvxInputConnectorNtask::transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* data  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (tp == JVX_LINKDATA_TRANSFER_REQUEST_DATA)
	{
		// JVX_LINKDATA_TRANSFER_REQUEST_DATA is commonly called from a different thread since it 
		// organizes links between components
		JVX_LOCK_MUTEX(safeAccessConnection);
	}
	if (_common_set_icon_nvtask.theData_in)
	{
		if (_common_set_icon_nvtask.theData_in->con_link.connect_from)
		{
			res = _common_set_icon_nvtask.theData_in->con_link.connect_from->transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	if (tp == JVX_LINKDATA_TRANSFER_REQUEST_DATA)
	{
		JVX_UNLOCK_MUTEX(safeAccessConnection);
	}
	return res;
}

jvxErrorType
CjvxInputConnectorNtask::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle*  data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
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
		locTxt += "object input connector " + JVX_DISPLAY_CONNECTOR(this);

		if (_common_nvtask->_common_set_comnvtask.associatedMaster)
		{
			locTxt += "<end>";
		}
		else
		{
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
			locTxt += "-<int>->";
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
			str->assign(locTxt);

			if (common_ntask._common_set_comntask.cbRef)
			{
				res = common_ntask._common_set_comntask.cbRef->transfer_forward_icon_ntask(tp, data,
					_common_set_icon_nvtask.theData_in,
					&_common_set_icon_ntask.ocon->_common_set_ocon_nvtask.theData_out,
					_common_nvtask->_common_set_comnvtask.ctxtId, _common_set_icon_ntask.ocon
					JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			else
			{
				res = _common_set_icon_ntask.ocon->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}

			locTxt = str->std_str();
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
			locTxt += "<-<tni>-";
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
		}
		str->assign(locTxt);
		break;

	case JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON:

		jsonElm.makeAssignmentString("icon", JVX_DISPLAY_CONNECTOR(this));
		jsonLst->addConsumeElement(jsonElm);

		if (_common_nvtask->_common_set_comnvtask.associatedMaster)
		{
			CjvxJsonElementList jsonLstRet;
			jsonElm.makeAssignmentString("connects_end", "here");
			jsonLst->addConsumeElement(jsonElm);
		}
		else
		{
			CjvxJsonElementList jsonLstRet;

			if (common_ntask._common_set_comntask.cbRef)
			{
				res = common_ntask._common_set_comntask.cbRef->transfer_forward_icon_ntask(tp, &jsonLstRet,
					_common_set_icon_nvtask.theData_in,
					&_common_set_icon_ntask.ocon->_common_set_ocon_nvtask.theData_out,
					_common_nvtask->_common_set_comnvtask.ctxtId, _common_set_icon_ntask.ocon	JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			else
			{
				res = _common_set_icon_ntask.ocon->transfer_forward_ocon(tp, &jsonLstRet JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}

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
		break;
	default:

		if (_common_nvtask->_common_set_comnvtask.associatedMaster)
		{
			if (common_ntask._common_set_comntask.cbRef)
			{
				res = common_ntask._common_set_comntask.cbRef->transfer_forward_icon_ntask(tp, data,
					_common_set_icon_nvtask.theData_in, NULL, _common_nvtask->_common_set_comnvtask.ctxtId,
					NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
		}
		else
		{
			if (common_ntask._common_set_comntask.cbRef)
			{
				res = common_ntask._common_set_comntask.cbRef->transfer_forward_icon_ntask(tp, data,
					_common_set_icon_nvtask.theData_in,
					&_common_set_icon_ntask.ocon->_common_set_ocon_nvtask.theData_out,
					_common_nvtask->_common_set_comnvtask.ctxtId, _common_set_icon_ntask.ocon JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			else
			{
				res = _common_set_icon_ntask.ocon->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}			
		}
		break;
	}

	return res;
}

// ==============================================================================

jvxErrorType 
CjvxInputConnectorNtask::number_next(jvxSize* num)
{
	if (num)
	{
		*num = 0;
		IjvxInputConnector* icon = nullptr;
		if (!_common_nvtask->_common_set_comnvtask.associatedMaster) // If this pointer is non-zero, the icon is the end of a chain.
		{
			if (_common_set_icon_ntask.ocon)
			{
				_common_set_icon_ntask.ocon->connected_icon(&icon);
				if (icon)
				{
					*num = 1;
				}
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxInputConnectorNtask::reference_next(jvxSize idx, IjvxConnectionIterator** next) 
{
	if (idx == 0)
	{
		if (next)
		{
			*next = nullptr;
			if (!_common_nvtask->_common_set_comnvtask.associatedMaster) // If this pointer is non-zero, the icon is the end of a chain.
			{
				IjvxInputConnector* icon = nullptr;
				if (_common_set_icon_ntask.ocon)
				{
					_common_set_icon_ntask.ocon->connected_icon(&icon);
					if (icon)
					{
						*next = static_cast<IjvxConnectionIterator*>(icon);
						return JVX_NO_ERROR;
					}
				}
			}
		}
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType 
CjvxInputConnectorNtask::reference_component(
	jvxComponentIdentification* cpTp, 
	jvxApiString* modName,
	jvxApiString* lContext)
{
	jvxErrorType res = common_ntask._common_set_comnvtask.object->request_specialization(nullptr, cpTp, nullptr);
	common_ntask._common_set_comnvtask.object->module_reference(modName, nullptr);
	if (lContext)
	{
		lContext->assign("NTask<" +
			jvx_size2String(common_ntask._common_set_comnvtask.ctxtId) +
			"><" +
			jvx_size2String(common_ntask._common_set_comnvtask.ctxtSubId) +
			">");
	}
	return res;
}

// ==========================================================================================================
// ==========================================================================================================
// ==========================================================================================================

CjvxOutputConnectorNtask::CjvxOutputConnectorNtask(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par,
	IjvxInputOutputConnectorNtask* report, IjvxConnectionMaster* aM, IjvxObject* objRef,
	jvxSize ctxtSubId) :
	CjvxOutputConnectorNVtask(),
	common_ntask(ctxId, descr, par, report, aM, objRef,  ctxtSubId)
{
	//std::vector<IjvxOutputConnector*> icons;
	//_common_set_icon_ntask.theData_out = NULL;
	jvx_initDataLinkDescriptor(&_common_set_ocon_nvtask.theData_out);
	_common_set_ocon_ntask.icon = NULL;
	_common_nvtask = static_cast<CjvxCommonNVtask*>(&common_ntask);
}

jvxErrorType
CjvxOutputConnectorNtask::set_reference(CjvxInputConnectorNtask*ref)
{
	jvxBool duplicateEntry = false;
	if (_common_set_ocon_ntask.icon == NULL)
	{
		_common_set_ocon_ntask.icon = ref;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}
// =======================================================================================================

jvxErrorType
CjvxOutputConnectorNtask::select_connect_ocon(IjvxConnectorBridge* bri, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common,
	IjvxOutputConnector** replace_connector)
	{
		jvxBool detectedProblem = false;

		// To and from conector must be associated in the same process/group
		if (_common_nvtask->_common_set_comnvtask.theCommon != NULL)
		{
			return JVX_ERROR_ALREADY_IN_USE;
		}

		// Make sure, all connections are associated to the same process or group
		if (_common_set_ocon_ntask.icon->_common_nvtask->_common_set_comnvtask.theCommon != NULL)
		{
			if (ass_connection_common != _common_set_ocon_ntask.icon->_common_nvtask->_common_set_comnvtask.theCommon)
			{
				detectedProblem = true;
			}
		}
		if (detectedProblem)
		{
			return JVX_ERROR_INVALID_SETTING;
		}

		// If we are linked with a master since we are a master with connectors, passed master must match
		if (master && _common_nvtask->_common_set_comnvtask.theMaster)
		{
			if (master != _common_nvtask->_common_set_comnvtask.theMaster)
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		if (_common_nvtask->_common_set_comnvtask.conBridge == NULL)
		{
			_common_nvtask->_common_set_comnvtask.conBridge = bri;
			_common_nvtask->_common_set_comnvtask.theCommon = ass_connection_common;

			// What else to do here?
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_COMPONENT_BUSY;
	}

jvxErrorType
CjvxOutputConnectorNtask::unselect_connect_ocon(IjvxConnectorBridge* bri,
	IjvxOutputConnector* replace_connector)
{
	//if (_common_set_comntask.conBridge == bri)
	//{
	_common_nvtask->_common_set_comnvtask.conBridge = NULL;
	_common_nvtask->_common_set_comnvtask.theCommon = NULL;

	// What else to do here?
	return JVX_NO_ERROR;
	//}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxOutputConnectorNtask::connect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = _connect_connect_ocon(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	/*
	if (res == JVX_NO_ERROR)
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			if (!_common_nvtask->_common_set_comnvtask.associatedMaster)
			{
				common_ntask._common_set_comntask.cbRef->connect_connect_ntask(args.theMaster, _common_nvtask->_common_set_comnvtask.ctxtId);
			}
		}
	}
	*/
	return res;
}

jvxErrorType
CjvxOutputConnectorNtask::disconnect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	/*
	if (res == JVX_NO_ERROR)
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			if (!_common_nvtask->_common_set_comnvtask.associatedMaster)
			{
				common_ntask._common_set_comntask.cbRef->disconnect_connect_ntask(_common_nvtask->_common_set_comnvtask.ctxtId);
			}
		}
	}
	*/
	res = _disconnect_connect_ocon(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return res;
}

jvxErrorType
CjvxOutputConnectorNtask::test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, _common_nvtask->_common_set_comnvtask.object);
	if (common_ntask._common_set_comntask.cbRef)
	{
		res = common_ntask._common_set_comntask.cbRef->test_connect_ocon_ntask(&_common_set_ocon_nvtask.theData_out,
			_common_nvtask->_common_set_comnvtask.ctxtId, _common_set_ocon_nvtask.theData_out.con_link.connect_to JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
	}
	return res;
}




jvxErrorType
CjvxOutputConnectorNtask::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_nvtask->_common_set_comnvtask.associatedMaster)
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->transfer_backward_ocon_ntask(tp, data,
				&_common_set_ocon_nvtask.theData_out, 
				_common_set_ocon_ntask.icon->_common_set_icon_nvtask.theData_in, 
				_common_nvtask->_common_set_comnvtask.ctxtId,
				 NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->transfer_backward_ocon_ntask(tp, data,
				&_common_set_ocon_nvtask.theData_out,
				_common_set_ocon_ntask.icon->_common_set_icon_nvtask.theData_in,
				_common_nvtask->_common_set_comnvtask.ctxtId, _common_set_ocon_ntask.icon
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		else
		{
			res = _common_set_ocon_ntask.icon->transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}	
	}
	return res;
}

/*
jvxErrorType
CjvxOutputConnectorNtask::transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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
*/

jvxErrorType
CjvxOutputConnectorNtask::process_buffers_ocon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ocon_nvtask.theData_out.con_link.connect_to)
	{
		if (common_ntask._common_set_comntask.cbRef)
		{
			res = common_ntask._common_set_comntask.cbRef->process_buffers_ocon_ntask(_common_nvtask->_common_set_comnvtask.ctxtId,
				mt_mask, idx_stage);
		}

		res = _common_set_ocon_nvtask.theData_out.con_link.connect_to->process_buffers_icon(mt_mask);
	}
	return res;
}

jvxErrorType
CjvxOutputConnectorNtask::connected_icon(IjvxInputConnector** icon)
{
	if (icon)
	{
		*icon = _common_set_ocon_nvtask.theData_out.con_link.connect_to;
	}
	return JVX_NO_ERROR;
}