#include "CjvxSpNDemultiplexer.h"

CjvxSpNDemultiplexer::CjvxSpNDemultiplexer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_SIGNAL_PROCESSING_NODE);
	_common_set.theComponentSubTypeDescriptor = "signal_processing_node";

	// Requests of the successor regarding processing parameters are passed to the previous components
	forward_complain = true;

	// The connector "default" on the input side is not in use. Zero copy is realized
	// in this class by linking the output buffers to the connectors in#<idx>
	_common_set_ldslave.zeroCopyBuffering_cfg = false;
}

CjvxSpNDemultiplexer::~CjvxSpNDemultiplexer()
{
	assert(inputConnectors.size() == 0);
}

// =====================================================================================

jvxErrorType
CjvxSpNDemultiplexer::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		genDemultiplexer_node::init_all();
		genDemultiplexer_node::allocate_all();
		genDemultiplexer_node::register_all(this);
		genDemultiplexer_node::register_callbacks(this, set_config, this);

		// The input connector "default" of the base class is not exposed: all connections
		// on the input side involve the connectors in#<idx>
		_common_set_conn_factory.input_connectors.erase(static_cast<IjvxInputConnectorSelect*>(this));

		res = update_input_connectors(genDemultiplexer_node::config.number_input_connectors.value);
		if (res != JVX_NO_ERROR)
		{
			genDemultiplexer_node::config.number_input_connectors.value = inputConnectors.size();
			res = JVX_NO_ERROR;
		}
		update_monitor();
	}
	return res;
}

jvxErrorType
CjvxSpNDemultiplexer::deactivate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		// Remove all input connectors
		for (auto& elm : inputConnectors)
		{
			_common_set_conn_factory.input_connectors.erase(static_cast<IjvxInputConnectorSelect*>(elm));
			elm->deactivate();
			JVX_SAFE_DELETE_OBJECT(elm);
		}
		inputConnectors.clear();

		genDemultiplexer_node::unregister_callbacks(this);
		genDemultiplexer_node::unregister_all(this);
		genDemultiplexer_node::deallocate_all();

		res = CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

// =====================================================================================

jvxErrorType
CjvxSpNDemultiplexer::check_input_connector_available(CjvxSpNDemultiplexerInputConnector* iconn, IjvxDataConnectionCommon* ass_connection_common)
{
	// Connectors can only be involved in a connection as long as nothing is in operation
	if (_common_set_min.theState != JVX_STATE_ACTIVE)
	{
		return JVX_ERROR_COMPONENT_BUSY;
	}

	// Once the chain has been connected, the unconnected input connectors can no longer be connected
	if (output.isConnected)
	{
		return JVX_ERROR_COMPONENT_BUSY;
	}

	// Output and all input connectors must be part of the same connection
	if (ass_connection_common)
	{
		if (_common_set_ocon.theCommon_from)
		{
			if (ass_connection_common != _common_set_ocon.theCommon_from)
			{
				return JVX_ERROR_INVALID_SETTING;
			}
		}
		for (auto& elm : inputConnectors)
		{
			if (elm->_common_set_icon.theCommon_to)
			{
				if (ass_connection_common != elm->_common_set_icon.theCommon_to)
				{
					return JVX_ERROR_INVALID_SETTING;
				}
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNDemultiplexer::update_input_connectors(jvxSize numConnectorsNew)
{
	jvxSize i;
	if (numConnectorsNew == inputConnectors.size())
	{
		return JVX_NO_ERROR;
	}

	// The connectors can only be modified in state ACTIVE and only if the chain is not connected
	if (_common_set_min.theState != JVX_STATE_ACTIVE)
	{
		return JVX_ERROR_WRONG_STATE;
	}
	if (output.isConnected)
	{
		return JVX_ERROR_COMPONENT_BUSY;
	}
	if (numConnectorsNew > JVX_SPN_DEMULTIPLEXER_MAX_CONNECTORS)
	{
		return JVX_ERROR_INVALID_SETTING;
	}

	// Connectors to be removed must not be in use in any bridge
	for (i = numConnectorsNew; i < inputConnectors.size(); i++)
	{
		if (
			(inputConnectors[i]->_common_set_icon.conn_in != nullptr) ||
			(inputConnectors[i]->_common_set_icon.theCommon_to != nullptr))
		{
			return JVX_ERROR_COMPONENT_BUSY;
		}
	}

	while (inputConnectors.size() > numConnectorsNew)
	{
		CjvxSpNDemultiplexerInputConnector* iconn = inputConnectors.back();
		inputConnectors.pop_back();

		_common_set_conn_factory.input_connectors.erase(static_cast<IjvxInputConnectorSelect*>(iconn));
		iconn->deactivate();
		JVX_SAFE_DELETE_OBJECT(iconn);
	}

	while (inputConnectors.size() < numConnectorsNew)
	{
		jvxSize idx = inputConnectors.size();
		CjvxSpNDemultiplexerInputConnector* iconn = nullptr;
		JVX_SAFE_ALLOCATE_OBJECT(iconn, CjvxSpNDemultiplexerInputConnector(this, idx));

		iconn->activate(static_cast<IjvxObject*>(this), static_cast<IjvxConnectorFactory*>(this),
			"in#" + jvx_size2String(idx), this, idx);
		iconn->report_trans = this;

		oneInputConnectorElement elmIn;
		elmIn.theConnector = iconn;
		_common_set_conn_factory.input_connectors[elmIn.theConnector] = elmIn;

		inputConnectors.push_back(iconn);
	}
	return JVX_NO_ERROR;
}

void
CjvxSpNDemultiplexer::update_monitor()
{
	jvxSize numConnected = 0;
	jvxSize numLinked = 0;
	for (auto& elm : inputConnectors)
	{
		if (elm->inChain)
		{
			numConnected++;
		}
		if (elm->isLinked)
		{
			numLinked++;
		}
	}
	genDemultiplexer_node::monitor.number_connectors_connected.value = numConnected;
	genDemultiplexer_node::monitor.number_connectors_zerocopy.value = numLinked;
}

// =====================================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNDemultiplexer, set_config)
{
	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genDemultiplexer_node::config.number_input_connectors))
	{
		jvxErrorType res = update_input_connectors(genDemultiplexer_node::config.number_input_connectors.value);
		if (res != JVX_NO_ERROR)
		{
			// Not possible in the current state, fall back to what is currently there
			genDemultiplexer_node::config.number_input_connectors.value = inputConnectors.size();
			CjvxProperties::add_property_report_collect(genDemultiplexer_node::config.number_input_connectors.descriptor.std_str(), false);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNDemultiplexer::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::put_configuration(callMan,
		processor, sectionToContainAllSubsectionsForMe,
		filename, lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			std::vector<std::string> warns;
			genDemultiplexer_node::put_configuration_all(callMan, processor, sectionToContainAllSubsectionsForMe, &warns);
			JVX_PROPERTY_PUT_CONFIGRATION_WARNINGS_STD_COUT(warns);

			// Create the connectors before the connections are established
			if (update_input_connectors(genDemultiplexer_node::config.number_input_connectors.value) != JVX_NO_ERROR)
			{
				genDemultiplexer_node::config.number_input_connectors.value = inputConnectors.size();
			}
		}
	}
	return res;
}

jvxErrorType
CjvxSpNDemultiplexer::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::get_configuration(callMan,
		processor, sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		genDemultiplexer_node::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	}
	return res;
}
