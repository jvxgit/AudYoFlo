#include "jvx.h"
#include "common/CjvxSingleInputConnector.h"

CjvxSingleInputConnector::CjvxSingleInputConnector() 
{
}

CjvxSingleInputConnector::~CjvxSingleInputConnector()
{
	assert(_common_set_icon.conn_in == nullptr);
	assert(_common_set_icon.theCommon_to == nullptr);
}

jvxErrorType 
CjvxSingleInputConnector::activate(IjvxObject* theObj, 
	IjvxConnectorFactory* conFac, const std::string& nm,
	CjvxSingleConnector_report<CjvxSingleInputConnector>* reportArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxInputOutputConnectorCore::activate(theObj, conFac, nullptr, nm);
	assert(res == JVX_NO_ERROR);

	res = CjvxInputConnectorCore::activate(this, this);
	assert(res == JVX_NO_ERROR);

	this->report = reportArg;
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
CjvxSingleInputConnector::number_next(jvxSize*) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleInputConnector::reference_next(jvxSize, IjvxConnectionIterator**) 
	{
		return JVX_NO_ERROR;
	}

jvxErrorType 
CjvxSingleInputConnector::reference_component(jvxComponentIdentification*, jvxApiString*, jvxApiString*)
	{
		return JVX_NO_ERROR;
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
// ===============================================================================================
// ===============================================================================================

CjvxSingleInputConnectorMulti::CjvxSingleInputConnectorMulti()
{
}

CjvxSingleInputConnectorMulti::~CjvxSingleInputConnectorMulti()
{
	assert(allocatedConnectors.size() == 0);
}

jvxErrorType 
CjvxSingleInputConnectorMulti::select_connect_icon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common, IjvxInputConnector** replace_connector)
{
	CjvxSingleInputConnector* newConnector = nullptr;
	if (numConnectorsInUse < (acceptNumberConnectors - 1))
	{
		JVX_SAFE_ALLOCATE_OBJECT(newConnector, CjvxSingleInputConnector);
	}
	else if (numConnectorsInUse < acceptNumberConnectors)
	{
		IjvxInputConnector* retLoc = this;
		_select_connect_icon(obj, master, ass_connection_common, &retLoc);
		newConnector = this;
	}

	if(newConnector)
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
	if(elm != allocatedConnectors.end())
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
			// If it is one of the additional connectors, unselect and delete
			res = elm->second->unselect_connect_icon(obj, replace_connector);
			JVX_SAFE_DELETE_OBJ(elm->second);
		}

		allocatedConnectors.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

