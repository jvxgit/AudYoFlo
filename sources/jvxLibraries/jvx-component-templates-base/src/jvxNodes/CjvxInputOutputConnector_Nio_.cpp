#include "CjvxInputOutputConnector_Nio.h"
#include "CjvxJson.h"

CjvxCommonNio::CjvxCommonNio(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, CjvxCommonNio_report* report, IjvxConnectionMaster* aM)
{
	_common_set_comnio.detectLoop = false;
	_common_set_comnio.object = NULL;
	_common_set_comnio.descriptor = descr;
	_common_set_comnio.myParent = par;
	_common_set_comnio.myRuntimeId = JVX_SIZE_UNSELECTED;
	_common_set_comnio.theCommon = NULL;
	_common_set_comnio.theMaster = NULL;
	_common_set_comnio.conBridge = NULL;
	_common_set_comnio.ctxtId = ctxId;
	_common_set_comnio.cbRef = report;
	associatedMaster = aM;
}

CjvxCommonNio::~CjvxCommonNio()
{
};

 jvxErrorType 
	 CjvxCommonNio::_descriptor_connector(jvxApiString* str)
 {
	 if (str)
	 {
		 str->assign(_common_set_comnio.descriptor);
	 }
	 return JVX_NO_ERROR;
 }

 jvxErrorType
CjvxCommonNio::_parent_factory(IjvxConnectorFactory** my_parent)
 {
	 if (my_parent)
	 {
		 *my_parent = _common_set_comnio.myParent;
	 }
	 return JVX_NO_ERROR;
 }

 CjvxInputConnectorNio::CjvxInputConnectorNio(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, CjvxCommonNio_report* report, 
	 IjvxConnectionMaster* aM):
	 CjvxCommonNio(ctxId, descr, par, report, aM)
 {
	 //std::vector<IjvxOutputConnector*> ocons;
	 _common_set_icon_nio.theData_in = NULL;
	 _common_set_icon_nio.num_min_buffers_in = 1;
	 _common_set_icon_nio.num_additional_pipleline_stages = 0;
 }
	
 CjvxInputConnectorNio::~CjvxInputConnectorNio()
{
 }

jvxErrorType 
CjvxInputConnectorNio::add_reference(CjvxOutputConnectorNio*ref)
{
	jvxBool duplicateEntry = false;
	jvxSize i;
	for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
	{
		if (ref == _common_set_icon_nio.ocons[i])
		{
			duplicateEntry = true;
			break;
		}
	}
	if (duplicateEntry)
		return JVX_ERROR_DUPLICATE_ENTRY;

	_common_set_icon_nio.ocons.push_back(ref);
	return JVX_NO_ERROR;
}
	
// =======================================================================================================
	

jvxErrorType
CjvxInputConnectorNio::descriptor_connector(jvxApiString* str)
 {
	 return _descriptor_connector(str);
 }

jvxErrorType 
CjvxInputConnectorNio::parent_factory(IjvxConnectorFactory** my_parent)
{
	return _parent_factory(my_parent);
}

	// =======================================================================================================

jvxErrorType
CjvxInputConnectorNio::associated_common_icon(IjvxDataConnectionCommon** ref)
{
	if (ref)
	{
		*ref = _common_set_comnio.theCommon;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxInputConnectorNio::select_connect_icon(IjvxConnectorBridge* bri, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common)
{
	jvxSize i;
	jvxBool detectedProblem = false;
	// To and from conector must be associated in the same process/group
	if (_common_set_comnio.theCommon != NULL)
	{
		return JVX_ERROR_ALREADY_IN_USE;
	}

	// Make sure, all connections are associated to the same process or group
	for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
	{
		if (_common_set_icon_nio.ocons[i]->_common_set_comnio.theCommon != NULL)
		{
			if (ass_connection_common != _common_set_icon_nio.ocons[i]->_common_set_comnio.theCommon)
			{
				detectedProblem = true;
				break;
			}
		}
	}
	if (detectedProblem)
	{
		return JVX_ERROR_INVALID_SETTING;
	}

	// If we are linked with a master since we are a master with connectors, passed master must match
	if (master && _common_set_comnio.theMaster)
	{
		if (master != _common_set_comnio.theMaster)
		{
			return JVX_ERROR_INVALID_ARGUMENT;
		}
	}

	if (_common_set_comnio.conBridge == NULL)
	{
		_common_set_comnio.conBridge = bri;
		_common_set_comnio.theCommon = ass_connection_common;

		// What else to do here?
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_COMPONENT_BUSY;
}

jvxErrorType
CjvxInputConnectorNio::unselect_connect_icon(IjvxConnectorBridge* bri)
{
	if (_common_set_comnio.conBridge == bri)
	{
		_common_set_comnio.conBridge = NULL;
		_common_set_comnio.theCommon = NULL;

		// What else to do here?
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxInputConnectorNio::connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	_common_set_icon_nio.theData_in = theData;

	assert(_common_set_comnio.detectLoop == false);
	_common_set_comnio.detectLoop = true;

	if (!associatedMaster)
	{
		for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
		{
			jvxErrorType resL = _common_set_icon_nio.ocons[i]->connect_connect_ocon(_common_set_icon_nio.theData_in->con_link.master JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (resL != JVX_NO_ERROR)
			{
				res = resL;
				break;
			}
		}
		_common_set_comnio.detectLoop = false;
	}
	return res;
}

jvxErrorType
CjvxInputConnectorNio::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (!associatedMaster)
	{
		for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
		{
			jvxErrorType resL = _common_set_icon_nio.ocons[i]->disconnect_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
	}
	_common_set_icon_nio.theData_in = NULL;
	return res;
}

jvxErrorType
CjvxInputConnectorNio::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	if (associatedMaster)
	{
		if (_common_set_comnio.cbRef)
		{
			res = _common_set_comnio.cbRef->test_connect_icon_id(
				_common_set_icon_nio.theData_in, NULL, _common_set_comnio.ctxtId, JVX_SIZE_UNSELECTED, NULL
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			if (_common_set_comnio.cbRef)
			{
				resL = _common_set_comnio.cbRef->test_connect_icon_id(
					_common_set_icon_nio.theData_in,
					&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
					_common_set_comnio.ctxtId,
					_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
					_common_set_icon_nio.ocons[i]
					JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			else
			{
				resL = _common_set_icon_nio.ocons[i]->test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			if (resL != JVX_NO_ERROR)
			{
				res = resL;
				break;
			}
		}
	}
	
	return res;
}

// ==========================================================================================================

jvxErrorType
CjvxInputConnectorNio::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	int i;
	jvxErrorType res = JVX_NO_ERROR;

	if (associatedMaster)
	{
		if (_common_set_comnio.cbRef)
		{
			res = _common_set_comnio.cbRef->prepare_connect_icon_id(_common_set_icon_nio.theData_in,
				NULL, _common_set_comnio.ctxtId, JVX_SIZE_UNSELECTED, NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			if (_common_set_comnio.cbRef)
			{
				res = _common_set_comnio.cbRef->prepare_connect_icon_id(_common_set_icon_nio.theData_in,
					&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
					_common_set_comnio.ctxtId,
					_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
					_common_set_icon_nio.ocons[i]
					JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			else
			{
				resL = _common_set_icon_nio.ocons[i]->prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			if (resL != JVX_NO_ERROR)
			{
				// Undo prepare
				i--;
				for (; i > 0; i--)
				{
					res = _common_set_comnio.cbRef->postprocess_connect_icon_id(_common_set_icon_nio.theData_in,
						&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
						_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]
						JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
				res = resL;
				break;
			}
		}
	}

	return res;
}

jvxErrorType
CjvxInputConnectorNio::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (associatedMaster)
	{
		if (_common_set_comnio.cbRef)
		{
			res = _common_set_comnio.cbRef->start_connect_icon_id(_common_set_icon_nio.theData_in,
				NULL, _common_set_comnio.ctxtId, JVX_SIZE_UNSELECTED, NULL
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			if (_common_set_comnio.cbRef)
			{
				res = _common_set_comnio.cbRef->start_connect_icon_id(_common_set_icon_nio.theData_in,
					&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
					_common_set_comnio.ctxtId,
					_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
					_common_set_icon_nio.ocons[i]
					JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			else
			{
				resL = _common_set_icon_nio.ocons[i]->start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			if (resL != JVX_NO_ERROR)
			{
				// Undo prepare
				i--;
				for (; i > 0; i--)
				{
					res = _common_set_comnio.cbRef->stop_connect_icon_id(_common_set_icon_nio.theData_in,
						&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
						_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]
						JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
				res = resL;
				break;
			}
		}
	}
	return res;
}

jvxErrorType
CjvxInputConnectorNio::process_start_icon()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (!associatedMaster)
	{
		for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			_common_set_icon_nio.ocons[i]->process_start_ocon();
			if (resL != JVX_NO_ERROR)
			{
				res = resL;
				break;
			}
		}
	}

	if (res == JVX_NO_ERROR)
	{
		res = jvx_shift_buffer_pipeline_idx_on_start(_common_set_icon_nio.theData_in, _common_set_comnio.myRuntimeId);
	}
	return res;
}

jvxErrorType
CjvxInputConnectorNio::process_buffers_icon(jvxSize mt_mask )
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (associatedMaster)
	{
		if (_common_set_comnio.cbRef)
		{
			res = _common_set_comnio.cbRef->process_buffers_icon_id(_common_set_icon_nio.theData_in,
				NULL, _common_set_comnio.ctxtId, JVX_SIZE_UNSELECTED, NULL
			);
		}
	}
	else
	{
		for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
		{

			jvxErrorType resL = JVX_NO_ERROR;
			if (_common_set_comnio.cbRef)
			{
				resL = _common_set_comnio.cbRef->process_buffers_icon_id(_common_set_icon_nio.theData_in,
					&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
					_common_set_comnio.ctxtId,
					_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
					_common_set_icon_nio.ocons[i]
				);
			}
			else
			{
				resL = _common_set_icon_nio.ocons[i]->process_buffers_ocon();
			}
			if (resL != JVX_NO_ERROR)
			{
				res = resL;
				break;
			}
		}
	}

	return res;
}

jvxErrorType
CjvxInputConnectorNio::process_stop_icon()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (!associatedMaster)
	{
		for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
		{
			jvxErrorType resL = _common_set_icon_nio.ocons[i]->process_stop_ocon();
			if (resL != JVX_NO_ERROR)
			{
				res = resL;
				break;
			}
		}
	}

	if (res == JVX_NO_ERROR)
	{
		res = jvx_shift_buffer_pipeline_idx_on_stop(_common_set_icon_nio.theData_in);
	}
	return res;
}

jvxErrorType
CjvxInputConnectorNio::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (associatedMaster)
	{
		if (_common_set_comnio.cbRef)
		{
			res = _common_set_comnio.cbRef->stop_connect_icon_id(_common_set_icon_nio.theData_in,
				NULL,_common_set_comnio.ctxtId, JVX_SIZE_UNSELECTED,NULL 
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			if (_common_set_comnio.cbRef)
			{
				res = _common_set_comnio.cbRef->stop_connect_icon_id(_common_set_icon_nio.theData_in,
					&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
					_common_set_comnio.ctxtId,
					_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
					_common_set_icon_nio.ocons[i]
					JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			else
			{
				resL = _common_set_icon_nio.ocons[i]->stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			if (resL != JVX_NO_ERROR)
			{
				// Undo stop makes no sense - what to do here??
				/*
				i--;
				for (; i > 0; i--)
				{
					res = _common_set_comnio.cbRef->stop_connect_icon_id(_common_set_icon_nio.theData_in,
						&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
						_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]
						JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}*/
				res = resL;
				break;
			}
		}
	}

	return res;
}

jvxErrorType
CjvxInputConnectorNio::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (associatedMaster)
	{
		if (_common_set_comnio.cbRef)
		{
			res = _common_set_comnio.cbRef->postprocess_connect_icon_id(_common_set_icon_nio.theData_in,
				NULL, _common_set_comnio.ctxtId, JVX_SIZE_UNSELECTED, NULL
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			if (_common_set_comnio.cbRef)
			{
				res = _common_set_comnio.cbRef->postprocess_connect_icon_id(_common_set_icon_nio.theData_in,
					&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
					_common_set_comnio.ctxtId,
					_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
					_common_set_icon_nio.ocons[i]
					JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			else
			{
				resL = _common_set_icon_nio.ocons[i]->postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			if (resL != JVX_NO_ERROR)
			{
				// Undo postprocess makes no sense - what to do here??
				/*
				i--;
				for (; i > 0; i--)
				{
					res = _common_set_comnio.cbRef->stop_connect_icon_id(_common_set_icon_nio.theData_in,
						&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
						_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]
						JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}*/
				res = resL;
				break;
			}
		}
	}
	return res;
}

jvxErrorType
CjvxInputConnectorNio::transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* data  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_icon_nio.theData_in->con_link.connect_from)
	{
		return _common_set_icon_nio.theData_in->con_link.connect_from->transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}

	return res;
}

jvxErrorType
CjvxInputConnectorNio::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle*  data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i;
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

		if (associatedMaster)
		{
			locTxt += "<end>";
		}
		else
		{
			for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
			{
				jvxErrorType resL = JVX_NO_ERROR;

				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
				locTxt += "-<int>->";
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
				str->assign(locTxt);

				if (_common_set_comnio.cbRef)
				{
					res = _common_set_comnio.cbRef->transfer_forward_icon_id(tp, data,
						_common_set_icon_nio.theData_in,
						&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
						_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]
						JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
				else
				{
					resL = _common_set_icon_nio.ocons[i]->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}

				locTxt = str->std_str();
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				locTxt += "<-<tni>-";
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;

				if (resL != JVX_NO_ERROR)
				{
					res = resL;
					break;
				}
			}
		}
		str->assign(locTxt);
		break;

	case JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON:

		jsonElm.makeAssignmentString("icon", JVX_DISPLAY_CONNECTOR(this));
		jsonLst->addConsumeElement(jsonElm);

		if (associatedMaster)
		{
			CjvxJsonElementList jsonLstRet;
			jsonElm.makeAssignmentString("connects_end", "here");
			jsonLst->addConsumeElement(jsonElm);
		}
		else
		{
			for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
			{
				jvxErrorType resL = JVX_NO_ERROR;
				CjvxJsonElementList jsonLstRet;

				if (_common_set_comnio.cbRef)
				{
					res = _common_set_comnio.cbRef->transfer_forward_icon_id(tp, &jsonLstRet,
						_common_set_icon_nio.theData_in,
						&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
						_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]
						JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
				else
				{
					resL = _common_set_icon_nio.ocons[i]->transfer_forward_ocon(tp, &jsonLstRet JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}

				if (resL == JVX_NO_ERROR)
				{
					jsonElm.makeSection(("connects_from_" + jvx_size2String(i)).c_str(), jsonLstRet);
				}
				else
				{
					jsonElm.makeAssignmentString("connects_error", jvxErrorType_txt(res));
					res = resL;
					break;
				}
				jsonLst->addConsumeElement(jsonElm);
			}
		}
		break;
	default:

		if (associatedMaster)
		{
			if (_common_set_comnio.cbRef)
			{
				res = _common_set_comnio.cbRef->transfer_forward_icon_id(tp, data,
					_common_set_icon_nio.theData_in, NULL, _common_set_comnio.ctxtId,
					JVX_SIZE_UNSELECTED, NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
		}
		else
		{
			for (i = 0; i < _common_set_icon_nio.ocons.size(); i++)
			{
				jvxErrorType resL = JVX_NO_ERROR;
				if (_common_set_comnio.cbRef)
				{
					res = _common_set_comnio.cbRef->transfer_forward_icon_id(tp, data,
						_common_set_icon_nio.theData_in,
						&_common_set_icon_nio.ocons[i]->_common_set_ocon_nio.theData_out,
						_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]->_common_set_comnio.ctxtId,
						_common_set_icon_nio.ocons[i]
						JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
				else
				{
					resL = _common_set_icon_nio.ocons[i]->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
				if (resL != JVX_NO_ERROR)
				{
					res = resL;
					break;
				}
			}
		}
		break;
	}

	return res;
}

// ==========================================================================================================

CjvxOutputConnectorNio::CjvxOutputConnectorNio(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, 
	CjvxCommonNio_report* report, IjvxConnectionMaster* aM) :
	CjvxCommonNio(ctxId, descr, par, report, aM)
{
	//std::vector<IjvxOutputConnector*> icons;
	//_common_set_icon_nio.theData_out = NULL;
	_common_set_ocon_nio.icon_connect = NULL;
	jvx_initDataLinkDescriptor(&_common_set_ocon_nio.theData_out);
}

jvxErrorType
CjvxOutputConnectorNio::add_reference(CjvxInputConnectorNio*ref)
{
	jvxBool duplicateEntry = false;
	jvxSize i;
	for (i = 0; i < _common_set_ocon_nio.icons.size(); i++)
	{
		if (ref == _common_set_ocon_nio.icons[i])
		{
			duplicateEntry = true;
			break;
		}
	}
	if (duplicateEntry)
		return JVX_ERROR_DUPLICATE_ENTRY;

	_common_set_ocon_nio.icons.push_back(ref);
	return JVX_NO_ERROR;
}
// =======================================================================================================

jvxErrorType
CjvxOutputConnectorNio::descriptor_connector(jvxApiString* str)
{
	return _descriptor_connector(str);
}

jvxErrorType
CjvxOutputConnectorNio::parent_factory(IjvxConnectorFactory** my_parent)
	{
		return _parent_factory(my_parent);
	};

	// =======================================================================================================

jvxErrorType
CjvxOutputConnectorNio::associated_common_ocon(IjvxDataConnectionCommon** ref)
	{
		if (ref)
		{
			*ref = _common_set_comnio.theCommon;
		}
		return JVX_NO_ERROR;
	}

jvxErrorType
CjvxOutputConnectorNio::select_connect_ocon(IjvxConnectorBridge* bri, IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common)
	{
		jvxBool detectedProblem = false;
		jvxSize i;

		// To and from conector must be associated in the same process/group
		if (_common_set_comnio.theCommon != NULL)
		{
			return JVX_ERROR_ALREADY_IN_USE;
		}

		// Make sure, all connections are associated to the same process or group
		for(i = 0; i < _common_set_ocon_nio.icons.size(); i++)
		{
			if (_common_set_ocon_nio.icons[i]->_common_set_comnio.theCommon != NULL)
			{
				if (ass_connection_common != _common_set_ocon_nio.icons[i]->_common_set_comnio.theCommon)
				{
					detectedProblem = true;
					break;
				}
			}
		}
		if (detectedProblem)
		{
			return JVX_ERROR_INVALID_SETTING;
		}

		// If we are linked with a master since we are a master with connectors, passed master must match
		if (master && _common_set_comnio.theMaster)
		{
			if (master != _common_set_comnio.theMaster)
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		if (_common_set_comnio.conBridge == NULL)
		{
			_common_set_comnio.conBridge = bri;
			_common_set_comnio.theCommon = ass_connection_common;

			// What else to do here?
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_COMPONENT_BUSY;
	}

jvxErrorType
CjvxOutputConnectorNio::unselect_connect_ocon(/*IjvxConnectorBridge* bri*/)
	{
		//if (_common_set_comnio.conBridge == bri)
		//{
			_common_set_comnio.conBridge = NULL;
			_common_set_comnio.theCommon = NULL;

			// What else to do here?
			return JVX_NO_ERROR;
		//}
		return JVX_ERROR_INVALID_ARGUMENT;
	}

jvxErrorType
CjvxOutputConnectorNio::connect_connect_ocon(IjvxConnectionMaster* theMaster JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	// Check that at least one of the associated input connectors has been connected before. Otherwise,
	// there is an error!
	for (i = 0; i < _common_set_ocon_nio.icons.size(); i++)
	{
		// Only on first connect request - we may have reached here from different input connectors
		if(_common_set_ocon_nio.theData_out.con_link.connect_to == NULL)
		{ 
			// Find the connector where we have come from
			//_common_set_ocon_nio.theData_out.con_link.connect_from = _common_set_ocon_nio.icons[i];
			if (_common_set_ocon_nio.icons[i]->_common_set_icon_nio.theData_in->con_link.connect_from ==
				static_cast<IjvxOutputConnector*>(this))
			{
				// Store the input connector at which we connected
				_common_set_ocon_nio.icon_connect = _common_set_ocon_nio.icons[i];

				jvx_initDataLinkDescriptor(&_common_set_ocon_nio.theData_out);

				_common_set_comnio.conBridge->reference_connect_to(&_common_set_ocon_nio.theData_out.con_link.connect_to);
				if (_common_set_ocon_nio.theData_out.con_link.connect_to)
				{
					_common_set_ocon_nio.theData_out.con_link.master = theMaster;
					_common_set_ocon_nio.theData_out.con_link.connect_from = this;
					jvxErrorType resL = _common_set_ocon_nio.theData_out.con_link.connect_to->connect_connect_icon(&_common_set_ocon_nio.theData_out JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (resL != JVX_NO_ERROR)
					{
						res = resL;
						break;
					}
				}
				else
				{
					std::string txt = "This output connector is not connected to a connection bridge (no associated input connector in bridge element).";
					JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt);
					res = JVX_ERROR_NOT_READY;
					break;
				}
			}
		}
		else
		{
			std::string txt = "This output connector has been connected before.";
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt);
			// This is not necessarily an error!
		}
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNio::disconnect_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	// Check that at least one of the associated input connectors has been connected before. Otherwise,
	// there is an error!
	for (i = 0; i < _common_set_ocon_nio.icons.size(); i++)
	{
		// Only on first connect request - we may have reached here from different input connectors
		if (_common_set_ocon_nio.theData_out.con_link.connect_to)
		{
			// Find the connector where we have come from
			if (_common_set_ocon_nio.icons[i]->_common_set_icon_nio.theData_in->con_link.connect_from ==
				static_cast<IjvxOutputConnector*>(this))
			{
				if (_common_set_ocon_nio.icons[i] == _common_set_ocon_nio.icon_connect)
				{
					// Identify the connector from which we connected
					assert(_common_set_comnio.conBridge);

					if (_common_set_ocon_nio.theData_out.con_link.connect_to)
					{
						jvxErrorType resL = _common_set_ocon_nio.theData_out.con_link.connect_to->disconnect_connect_icon(
							&_common_set_ocon_nio.theData_out JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					}
					_common_set_comnio.conBridge->return_reference_connect_to(_common_set_ocon_nio.theData_out.con_link.connect_to);
					jvx_terminateDataLinkDescriptor(&_common_set_ocon_nio.theData_out);

					_common_set_ocon_nio.icon_connect = NULL;
					break;
				}
				else
				{
					// This is not the "connected" icon
				}
			}
			else
			{
				// ??
			}
		}
		else
		{
			// ??
		}
	}
	return res;
}

jvxErrorType
CjvxOutputConnectorNio::test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ocon_nio.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nio.theData_out.con_link.connect_to->test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
CjvxOutputConnectorNio::prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ocon_nio.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nio.theData_out.con_link.connect_to->prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNio::postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ocon_nio.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nio.theData_out.con_link.connect_to->postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNio::start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ocon_nio.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nio.theData_out.con_link.connect_to->start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNio::stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ocon_nio.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nio.theData_out.con_link.connect_to->stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNio::process_start_ocon()
{
	jvxErrorType res = JVX_NO_ERROR;

	// Propagate to next object
	if (_common_set_ocon_nio.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nio.theData_out.con_link.connect_to->process_start_icon();
	}

	return res;
}

jvxErrorType
CjvxOutputConnectorNio::process_buffers_ocon()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize mt_mask = JVX_SIZE_UNSELECTED;

	if (_common_set_ocon_nio.theData_out.con_link.connect_to)
	{
		if (_common_set_comnio.cbRef)
		{
			res = _common_set_comnio.cbRef->process_buffers_ocon_id(_common_set_comnio.ctxtId, &mt_mask);
		}

		res = _common_set_ocon_nio.theData_out.con_link.connect_to->process_buffers_icon(mt_mask);
	}
	return res;
}

jvxErrorType
CjvxOutputConnectorNio::process_stop_ocon()
{
	jvxErrorType res = JVX_NO_ERROR;

	// Propagate to next object
	if (_common_set_ocon_nio.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon_nio.theData_out.con_link.connect_to->process_stop_icon();
	}

	return res;
}


jvxErrorType
CjvxOutputConnectorNio::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (associatedMaster)
	{
		if (_common_set_comnio.cbRef)
		{
			res = _common_set_comnio.cbRef->transfer_backward_ocon_id(tp, data,
				&_common_set_ocon_nio.theData_out, NULL, _common_set_comnio.ctxtId,
				JVX_SIZE_UNSELECTED, NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	else
	{
		for (i = 0; i < _common_set_ocon_nio.icons.size(); i++)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			if (_common_set_comnio.cbRef)
			{
				res = _common_set_comnio.cbRef->transfer_backward_ocon_id(tp, data,
					&_common_set_ocon_nio.theData_out,
					_common_set_ocon_nio.icons[i]->_common_set_icon_nio.theData_in,
					_common_set_comnio.ctxtId,
					_common_set_ocon_nio.icons[i]->_common_set_comnio.ctxtId,
					_common_set_ocon_nio.icons[i]
					JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			else
			{
				resL = _common_set_ocon_nio.icons[i]->transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			if (resL != JVX_NO_ERROR)
			{
				res = resL;
				break;
			}
		}
	}
	return res;
}

jvxErrorType
CjvxOutputConnectorNio::transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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

		if (_common_set_ocon_nio.theData_out.con_link.connect_to)
		{
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
			locTxt += "-<ext>->";
			locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
			str->assign(locTxt);

			res = _common_set_ocon_nio.theData_out.con_link.connect_to->transfer_forward_icon(tp,
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

		if (_common_set_ocon_nio.theData_out.con_link.connect_to)
		{
			CjvxJsonElementList jsonLstRet;
			res = _common_set_ocon_nio.theData_out.con_link.connect_to->transfer_forward_icon(tp,
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
		if (_common_set_ocon_nio.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon_nio.theData_out.con_link.connect_to->transfer_forward_icon(tp,
				data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		break;
	}

	return res;
}


