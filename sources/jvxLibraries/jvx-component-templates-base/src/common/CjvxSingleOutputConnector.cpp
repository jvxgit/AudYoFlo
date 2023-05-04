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
CjvxSingleOutputConnector::updateFixedProcessingArgs(const jvxLinkDataDescriptor_con_params& params, jvxBool requesTestChain)
{
	// We only accept ONE setting!!
	neg_output._update_parameters_fixed(params.number_channels,
		params.buffersize, params.rate,
		params.format, params.format_group,
		JVX_DATAFLOW_PUSH_ON_PULL, nullptr);
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

// =================================================================================

jvxErrorType
CjvxSingleOutputConnectorMulti::select_connect_ocon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common, IjvxOutputConnector** replace_connector)
{
	CjvxSingleOutputConnector* newConnector = nullptr;
	if (numConnectorsInUse < (acceptNumberConnectors - 1))
	{
		JVX_SAFE_ALLOCATE_OBJECT(newConnector, CjvxSingleOutputConnector(withTriggerConnector));
		newConnector->activate(_common_set_io_common_ptr->_common_set_io_common.object,
			_common_set_io_common_ptr->_common_set_io_common.myParent,
			_common_set_io_common_ptr->_common_set_io_common.descriptor,
			report, allocatedConnectors.size());
	}

	else if (numConnectorsInUse < acceptNumberConnectors)
	{
		IjvxOutputConnector* retLoc = this;
		_select_connect_ocon(obj, master, ass_connection_common, &retLoc);
		newConnector = this;
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

		if (replace_connector == static_cast<IjvxOutputConnector*>(this))
		{
			// If this is the connector, just unselect it
			_unselect_connect_ocon(obj, replace_connector);
		}
		else
		{
			elm->second->deactivate();

			// If it is one of the additional connectors, unselect and delete
			res = elm->second->unselect_connect_ocon(obj, replace_connector);
			JVX_SAFE_DELETE_OBJ(elm->second);
		}

		allocatedConnectors.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}