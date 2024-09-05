#include "common/CjvxSingleOutputConnector.h"

jvxErrorType
CjvxSingleOutputTriggerConnector::trigger(jvxTriggerConnectorPurpose purp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	const jvxChainConnectArguments* args = (const jvxChainConnectArguments*)data;
	IjvxConnectionIterator** itReturn = (IjvxConnectionIterator**)data;
	switch (purp)
	{
	case jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_CONNECT:
		if (bwdRef)
		{
			res = bwdRef->_connect_connect_ocon(*args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		break;
	case jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_DISCONNECT:
		if (bwdRef)
		{
			res = bwdRef->_disconnect_connect_ocon(*args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		break;
	case jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_ITERATOR_NEXT:
		if (itReturn)
		{
			*itReturn = static_cast<IjvxConnectionIterator*>(bwdRef);
		}
		break;
	case jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_TEST:
		if (bwdRef)
		{			
			res = bwdRef->test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		break;
	case jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_PREPARE:
		if (bwdRef)
		{						
			res = bwdRef->prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		break;
	case jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_POSTPROCESS:
		if (bwdRef)
		{
			res = bwdRef->postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		break;
	case jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_START:
		if (bwdRef)
		{
			res = bwdRef->start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		break;
	case jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_STOP:
		if (bwdRef)
		{
			res = bwdRef->stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		break;
	case jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_FORWARD_:

		// not in use!!
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

CjvxSingleOutputConnector::CjvxSingleOutputConnector(jvxBool withTriggerConnectorArg) :
	CjvxConnector<CjvxOutputConnectorLink, CjvxSingleOutputTriggerConnector>(withTriggerConnectorArg)
{
	if (withTriggerConnector)
	{
		JVX_SAFE_ALLOCATE_OBJECT(trig_con, CjvxSingleOutputTriggerConnector);
		trig_con->bwdRef = this;
	}
}

CjvxSingleOutputConnector::~CjvxSingleOutputConnector()
{
	assert(_common_set_ocon.conn_out == nullptr);
	assert(_common_set_ocon.theCommon_from == nullptr);
	if (withTriggerConnector)
	{
		JVX_SAFE_DELETE_OBJECT(trig_con);
	}
}

jvxErrorType 
CjvxSingleOutputConnector::number_next(jvxSize* num)
{
	*num = 1;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleOutputConnector::reference_next(jvxSize idx, IjvxConnectionIterator** outReturn)
{
	*outReturn = nullptr;
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		*outReturn = _common_set_ocon.theData_out.con_link.connect_to;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleOutputConnector::reference_component(jvxComponentIdentification* cpId, jvxApiString* modName, jvxApiString* description, jvxApiString* linkName)
{
	return _reference_component(cpId, modName, description, linkName);
}

jvxErrorType
CjvxSingleOutputConnector::activate(IjvxObject* theObj,
	IjvxConnectorFactory* conFac, const std::string& nm,
	CjvxSingleConnector_report<CjvxSingleOutputConnector>* reportArg,
	jvxSize conIdArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxInputOutputConnectorCore::activate(theObj, conFac, nullptr, nm);
	assert(res == JVX_NO_ERROR);

	res = CjvxOutputConnectorCore::activate(this, this);
	assert(res == JVX_NO_ERROR);

	this->report = reportArg;
	conId = conIdArg;
	return res;
}



jvxErrorType
CjvxSingleOutputConnector::deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxInputOutputConnectorCore::deactivate();
	assert(res == JVX_NO_ERROR);

	res = CjvxOutputConnectorCore::deactivate();
	assert(res == JVX_NO_ERROR);

	return res;
}

jvxErrorType 
CjvxSingleOutputConnector::test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) 
{
	// JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, this);
	if (report)
	{
		report->report_test_connector(this  JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		jvxSize chanMax = chanSetting.channel_num;
		if (JVX_CHECK_SIZE_SELECTED(chanMax))
		{
			chanMax = JVX_MIN(chanMax, channelWidthMax - chanSetting.idxOffset);
			neg_output.preferred.number_channels.max = chanMax;
			neg_output.preferred.number_channels.min = chanMax;
		}
	}

	neg_output._constrain_ldesc(&_common_set_ocon.theData_out);

	// this->test_set_output_parameters();

	JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_O(fdb, (&_common_set_ocon.theData_out));
	return _test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

jvxErrorType 
CjvxSingleOutputConnector::start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Request a unique pipeline id
	if (report)
	{
		report->request_unique_id_start(this, &_common_set_io_common_ptr->_common_set_io_common.myRuntimeId);
	}

	jvxErrorType res = _start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		if (report)
		{
			report->report_started_connector(this);
		}
	}
	return res;
}

jvxErrorType 
CjvxSingleOutputConnector::stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = _stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	if (report)
	{
		report->report_stopped_connector(this);
		report->release_unique_id_stop(this, _common_set_io_common_ptr->_common_set_io_common.myRuntimeId);
	}
	return res;
}

jvxErrorType
CjvxSingleOutputConnector::updateFixedProcessingArgs(const jvxLinkDataDescriptor_con_params& params)
{
	// We only accept ONE setting!! Dataflow is forward
	neg_output._update_parameters_fixed(params.number_channels,
		params.buffersize, params.rate,
		params.format, params.format_group,
		JVX_DATAFLOW_PUSH_ACTIVE, nullptr);
	
	// Not activated on the output side, otherwise, we will only see 0 channels!!
	// It always outputs the maximim number of channels!!
	// neg_output.preferred.number_channels.min = 0;
	channelWidthMax = params.number_channels;	
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleOutputConnector::trigger_put_data()
{
	jvxErrorType res = _common_set_ocon.ocon->process_start_ocon(0, NULL, 0, NULL, NULL);
	if (res == JVX_NO_ERROR)
	{		
		if (report)
		{
			report->report_process_buffers(this, _common_set_ocon.theData_out, JVX_SIZE_UNSELECTED);
		}

		_common_set_ocon.ocon->process_buffers_ocon(JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
		_common_set_ocon.ocon->process_stop_ocon(0, NULL, 0, NULL, NULL);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSingleOutputConnector::request_trigger_itcon(IjvxTriggerInputConnector** otcon)
{
	if (trig_con)
	{
		*otcon = trig_con;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxSingleOutputConnector::return_trigger_itcon(IjvxTriggerInputConnector* otcon)
{
	if (trig_con)
	{
		if (otcon == trig_con)
		{
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxSingleOutputConnector::available_to_connect_ocon()
{
	if (allowSingleConnect)
	{
		if (_common_set_ocon.theCommon_from == NULL)
		{
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ALREADY_IN_USE;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxSingleOutputConnector::resultFromInputConnector()
{
	jvxErrorType res = JVX_ERROR_UNKNOWN;
	if (trig_con)
	{
		if (trig_con->linked_ref)
		{
			res = trig_con->linked_ref->latest_result_data();
		}
	}
	return res;
}

jvxErrorType
CjvxSingleOutputConnector::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (report_trans)
	{
		return report_trans->report_transfer_connector(this, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return JVX_ERROR_UNSUPPORTED;
}

// =================================================================================

jvxErrorType
CjvxSingleOutputConnectorMulti::available_to_connect_ocon()
{
	if (numConnectorsInUse < acceptNumberConnectors)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CjvxSingleOutputConnectorMulti::select_connect_ocon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common, IjvxOutputConnector** replace_connector)
{
	CjvxSingleOutputConnector* newConnector = nullptr;
	if (numConnectorsInUse < acceptNumberConnectors)
	{
		JVX_SAFE_ALLOCATE_OBJECT(newConnector, CjvxSingleOutputConnector(withTriggerConnector));

		// replace_connector is overridden here but will be modified later
		newConnector->select_connect_ocon(obj, master, ass_connection_common, replace_connector);

		newConnector->activate(_common_set_io_common_ptr->_common_set_io_common.object,
			_common_set_io_common_ptr->_common_set_io_common.myParent,
			_common_set_io_common_ptr->_common_set_io_common.descriptor,
			report, allocatedConnectors.size());
	}
	else
	{
		return JVX_ERROR_ALREADY_IN_USE;
	}

	if (newConnector)
	{
		*replace_connector = newConnector;
		allocatedConnectors[*replace_connector] = newConnector;

		if (report)
		{
			report->report_selected_connector(newConnector);
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxSingleOutputConnectorMulti::unselect_connect_ocon(IjvxConnectorBridge* obj,
	IjvxOutputConnector* replace_connector)
{
	jvxErrorType res = JVX_NO_ERROR;
	CjvxSingleOutputConnector* remConnector = nullptr;

	auto elm = allocatedConnectors.find(replace_connector);
	if (elm != allocatedConnectors.end())
	{
		if (report)
		{
			report->report_unselected_connector(elm->second);
		}

		assert(replace_connector != static_cast<IjvxOutputConnector*>(this));
		
		elm->second->deactivate();

		// If it is one of the additional connectors, unselect and delete
		res = elm->second->unselect_connect_ocon(obj, replace_connector);
		JVX_SAFE_DELETE_OBJ(elm->second);	

		allocatedConnectors.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

