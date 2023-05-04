#include "jvx.h"
#include "common/CjvxSingleInputConnector.h"


jvxErrorType
CjvxSingleInputTriggerConnector::trigger(jvxTriggerConnectorPurpose purp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (purp)
	{
	case jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_BACKWARD:
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}


// =====================================================================================

CjvxSingleInputConnector::CjvxSingleInputConnector(jvxBool withTriggerConnectorArg):
	CjvxConnector<CjvxInputConnectorLink, CjvxSingleInputTriggerConnector>(withTriggerConnectorArg)
{
	if (withTriggerConnector)
	{
		JVX_SAFE_ALLOCATE_OBJECT(trig_con, CjvxSingleInputTriggerConnector);
		trig_con->bwdRef = this;
	}
}

CjvxSingleInputConnector::~CjvxSingleInputConnector()
{
	assert(_common_set_icon.conn_in == nullptr);
	assert(_common_set_icon.theCommon_to == nullptr);	
	if (withTriggerConnector)
	{
		JVX_SAFE_DELETE_OBJECT(trig_con);
	}
}

jvxErrorType 
CjvxSingleInputConnector::activate(IjvxObject* theObj, 
	IjvxConnectorFactory* conFac, const std::string& nm,
	CjvxSingleConnector_report<CjvxSingleInputConnector>* reportArg,
	jvxSize conIdArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxInputOutputConnectorCore::activate(theObj, conFac, nullptr, nm);
	assert(res == JVX_NO_ERROR);

	res = CjvxInputConnectorCore::activate(this, this);
	assert(res == JVX_NO_ERROR);

	this->report = reportArg;
	conId = conIdArg;
	return res;
}

jvxErrorType
CjvxSingleInputConnector::deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxInputOutputConnectorCore::deactivate();
	assert(res == JVX_NO_ERROR);

	res = CjvxInputConnectorCore::deactivate();
	assert(res == JVX_NO_ERROR);

	return res;
}

jvxErrorType
CjvxSingleInputConnector::connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	_connect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb), false);

	// If we have set a trigger forward, follow it but ONLY with the link data type for linkage!!
	if (
		trig_con && 
		trig_con->linked_ref)
	{
		jvxChainConnectArguments args;
		args.theMaster = theData->con_link.master;
		args.uIdConnection = theData->con_link.uIdConn;
		res = trig_con->linked_ref->trigger(jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_CONNECT, &args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
};

jvxErrorType
CjvxSingleInputConnector::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// If we have set a trigger forward, follow it but ONLY with the link data type for linkage!!
	if (
		trig_con &&
		trig_con->linked_ref)
	{
		jvxChainConnectArguments args;
		args.theMaster = theData->con_link.master;
		args.uIdConnection = theData->con_link.uIdConn;
		res = trig_con->linked_ref->trigger(jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_DISCONNECT, &args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}

	_disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb), false);

	return res;
}

// ================================================================================================

jvxErrorType 
CjvxSingleInputConnector::number_next(jvxSize* num) 
{
	*num = 0;
	if (trig_con)
	{
		*num = 1;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleInputConnector::reference_next(jvxSize idx, IjvxConnectionIterator** onReturn) 
{
	*onReturn = nullptr;
	if (trig_con)
	{
		trig_con->trigger(jvxTriggerConnectorPurpose::JVX_CONNECTOR_TRIGGER_ITERATOR_NEXT, onReturn JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleInputConnector::reference_component(jvxComponentIdentification* cpId, jvxApiString* modName, jvxApiString* description, jvxApiString* linkName)
{
	return _reference_component(cpId, modName, description, linkName);
}

jvxErrorType 
CjvxSingleInputConnector::updateFixedProcessingArgs(const jvxLinkDataDescriptor_con_params& params, jvxBool requesTestChain)
{
	// We only accept ONE setting!!
	neg_input._update_parameters_fixed(params.number_channels,
		params.buffersize, params.rate,
		params.format, params.format_group,
		JVX_DATAFLOW_PUSH_ON_PULL, nullptr);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSingleInputConnector::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = neg_input._negotiate_connect_icon(_common_set_icon.theData_in, nullptr, "none" JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Yes, we forward but this will end up in test_connect_forward where nothing really happens
		res = _test_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

// ===============================================================================================

jvxErrorType
CjvxSingleInputConnector::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = _prepare_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		res = jvx_allocate_pipeline_and_buffers_prepare_to(_common_set_icon.theData_in
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
			, _common_set_ldslave.descriptor.c_str()
#endif
		);
	}
	return res;
}

jvxErrorType
CjvxSingleInputConnector::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (res == JVX_NO_ERROR)
	{
		_postprocess_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		res = jvx_deallocate_pipeline_and_buffers_postprocess_to(_common_set_icon.theData_in);
	}
	return res;
}

// ===============================================================================================

jvxErrorType
CjvxSingleInputConnector::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Request a unique pipeline id
	if (report)
	{
		report->request_unique_id_start(this, &_common_set_io_common_ptr->_common_set_io_common.myRuntimeId);
	}
	
	jvxErrorType res = _start_connect_icon(true, _common_set_io_common_ptr->_common_set_io_common.myRuntimeId JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
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
CjvxSingleInputConnector::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (res == JVX_NO_ERROR)
	{
		res = _stop_connect_icon(true, &_common_set_io_common_ptr->_common_set_io_common.myRuntimeId JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		if (report)
		{
			report->report_stopped_connector(this);
			report->release_unique_id_stop(this, _common_set_io_common_ptr->_common_set_io_common.myRuntimeId);
		}
	}
	return res;
}

jvxErrorType 
CjvxSingleInputConnector::trigger_get_data()
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (_common_set_icon.theData_in)
	{
		if (_common_set_icon.theData_in->con_link.connect_from)
		{
			res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_REQUEST_DATA, nullptr JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
		}
	}
	return res;
}

jvxErrorType 
CjvxSingleInputConnector::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxHandle** buffers_in = nullptr; 
	jvxSize idx_stage_local = idx_stage;
	if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_local))
	{
		idx_stage_local = *_common_set_icon.theData_in->con_pipeline.idx_stage_ptr;
	}
	buffers_in = _common_set_icon.theData_in->con_data.buffers[idx_stage_local];
	// jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);

	if(report)
	{
		report->report_process_buffers(this, buffers_in, _common_set_icon.theData_in->con_params);
	}

	// Forward chaining, will end up immediately in _process_buffers_icon anyway
	return _process_buffers_icon(mt_mask, idx_stage);
}

jvxErrorType 
CjvxSingleInputConnector::request_trigger_otcon(IjvxTriggerOutputConnector** otcon) 
{
	if (trig_con)
	{
		*otcon = trig_con;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxSingleInputConnector::return_trigger_otcon(IjvxTriggerOutputConnector* otcon) 
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

// ===============================================================================================
// ===============================================================================================


jvxErrorType 
CjvxSingleInputConnectorMulti::select_connect_icon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common, IjvxInputConnector** replace_connector) 
{
	CjvxSingleInputConnector* newConnector = nullptr;
	if (numConnectorsInUse < (acceptNumberConnectors - 1))
	{
		JVX_SAFE_ALLOCATE_OBJECT(newConnector, CjvxSingleInputConnector(withTriggerConnector));
		newConnector->activate(_common_set_io_common_ptr->_common_set_io_common.object,
			_common_set_io_common_ptr->_common_set_io_common.myParent,
			_common_set_io_common_ptr->_common_set_io_common.descriptor,
			report, allocatedConnectors.size());
	}

	else if (numConnectorsInUse < acceptNumberConnectors)
	{
		IjvxInputConnector* retLoc = this;
		_select_connect_icon(obj, master, ass_connection_common, &retLoc);
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
CjvxSingleInputConnectorMulti::unselect_connect_icon(IjvxConnectorBridge* obj,
	IjvxInputConnector* replace_connector) 
{
	jvxErrorType res = JVX_NO_ERROR;
	CjvxSingleInputConnector* remConnector = nullptr;

	auto elm = allocatedConnectors.find(replace_connector);
	if (elm != allocatedConnectors.end())
	{
		if (report)
		{
			report->report_unselected_connector(elm->second);
		}

		if (replace_connector == static_cast<IjvxInputConnector*>(this))
		{
			// If this is the connector, just unselect it
			_unselect_connect_icon(obj, replace_connector);
		}
		else
		{
			elm->second->deactivate();

			// If it is one of the additional connectors, unselect and delete
			res = elm->second->unselect_connect_icon(obj, replace_connector);
			JVX_SAFE_DELETE_OBJ(elm->second);
		}

		allocatedConnectors.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}