#include "common/CjvxSingleOutputConnector.h"

CjvxSingleOutputConnector::CjvxSingleOutputConnector()
{
}

jvxErrorType 
CjvxSingleOutputConnector::activate(IjvxObject* theObj, IjvxConnectorFactory* conFac, const std::string& nm, 
	CjvxSingleConnector_report<CjvxSingleOutputConnector>* reportArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxInputOutputConnectorCore::activate(theObj, conFac, nullptr, nm);
	assert(res == JVX_NO_ERROR);

	res = CjvxOutputConnectorCore::activate(this, this);
	assert(res == JVX_NO_ERROR);

	report = reportArg;
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

CjvxSingleOutputConnectorMulti::CjvxSingleOutputConnectorMulti()
{
}

CjvxSingleOutputConnectorMulti::~CjvxSingleOutputConnectorMulti()
{
	assert(allocatedConnectors.size() == 0);
}

jvxErrorType
CjvxSingleOutputConnectorMulti::select_connect_ocon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common, IjvxOutputConnector** replace_connector)
{
	CjvxSingleOutputConnector* newConnector = nullptr;
	if (numConnectorsInUse < (acceptNumberConnectors - 1))
	{
		JVX_SAFE_ALLOCATE_OBJECT(newConnector, CjvxSingleOutputConnector);
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
			// If it is one of the additional connectors, unselect and delete
			res = elm->second->unselect_connect_ocon(obj, replace_connector);
			JVX_SAFE_DELETE_OBJ(elm->second);
		}

		allocatedConnectors.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}