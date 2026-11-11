#include "CjvxSpNMultiplexer.h"

CjvxSpNMultiplexer::CjvxSpNMultiplexer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_SIGNAL_PROCESSING_NODE);
	_common_set.theComponentSubTypeDescriptor = "signal_processing_node";

	// Requests of the successors regarding processing parameters are passed to the previous component
	forward_complain = true;

	// The connector "default" on the output side is not in use. Zero copy is realized
	// in this class by linking the buffers of the connectors out#<idx>
	_common_set_ldslave.zeroCopyBuffering_cfg = false;
}

CjvxSpNMultiplexer::~CjvxSpNMultiplexer()
{
	assert(outputConnectors.size() == 0);
}

// =====================================================================================

jvxErrorType
CjvxSpNMultiplexer::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		genMultiplexer_node::init_all();
		genMultiplexer_node::allocate_all();
		genMultiplexer_node::register_all(this);
		genMultiplexer_node::register_callbacks(this, set_config, this);

		// The output connector "default" of the base class is not exposed: all connections
		// on the output side involve the connectors out#<idx>. The connector remains in
		// operation as an unconnected (terminated) output of the base class
		_common_set_conn_factory.output_connectors.erase(static_cast<IjvxOutputConnectorSelect*>(this));

		res = update_output_connectors(genMultiplexer_node::config.number_output_connectors.value);
		if (res != JVX_NO_ERROR)
		{
			genMultiplexer_node::config.number_output_connectors.value = outputConnectors.size();
			res = JVX_NO_ERROR;
		}
		update_monitor();
	}
	return res;
}

jvxErrorType
CjvxSpNMultiplexer::deactivate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		// Remove all output connectors
		for (auto& elm : outputConnectors)
		{
			_common_set_conn_factory.output_connectors.erase(static_cast<IjvxOutputConnectorSelect*>(elm));
			elm->deactivate();
			JVX_SAFE_DELETE_OBJECT(elm);
		}
		outputConnectors.clear();

		genMultiplexer_node::unregister_callbacks(this);
		genMultiplexer_node::unregister_all(this);
		genMultiplexer_node::deallocate_all();

		res = CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

// =====================================================================================

jvxErrorType
CjvxSpNMultiplexer::check_output_connector_available(CjvxSpNMultiplexerOutputConnector* oconn, IjvxDataConnectionCommon* ass_connection_common)
{
	// Connectors can only be involved in a connection as long as nothing is in operation
	if (_common_set_min.theState != JVX_STATE_ACTIVE)
	{
		return JVX_ERROR_COMPONENT_BUSY;
	}

	// Once the chain has been connected, the unconnected output connectors can no longer be connected
	if (_common_set_icon.theData_in != nullptr)
	{
		return JVX_ERROR_COMPONENT_BUSY;
	}

	// Input and output connectors must be part of the same connection
	if (ass_connection_common && _common_set_icon.theCommon_to)
	{
		if (ass_connection_common != _common_set_icon.theCommon_to)
		{
			return JVX_ERROR_INVALID_SETTING;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNMultiplexer::update_output_connectors(jvxSize numConnectorsNew)
{
	jvxSize i;
	if (numConnectorsNew == outputConnectors.size())
	{
		return JVX_NO_ERROR;
	}

	// The connectors can only be modified in state ACTIVE and only if the chain is not connected
	if (_common_set_min.theState != JVX_STATE_ACTIVE)
	{
		return JVX_ERROR_WRONG_STATE;
	}
	if (_common_set_icon.theData_in != nullptr)
	{
		return JVX_ERROR_COMPONENT_BUSY;
	}
	if (numConnectorsNew > JVX_SPN_MULTIPLEXER_MAX_CONNECTORS)
	{
		return JVX_ERROR_INVALID_SETTING;
	}

	// Connectors to be removed must not be in use in any bridge
	for (i = numConnectorsNew; i < outputConnectors.size(); i++)
	{
		if (
			(outputConnectors[i]->_common_set_ocon.conn_out != nullptr) ||
			(outputConnectors[i]->_common_set_ocon.theCommon_from != nullptr))
		{
			return JVX_ERROR_COMPONENT_BUSY;
		}
	}

	while (outputConnectors.size() > numConnectorsNew)
	{
		CjvxSpNMultiplexerOutputConnector* oconn = outputConnectors.back();
		outputConnectors.pop_back();

		_common_set_conn_factory.output_connectors.erase(static_cast<IjvxOutputConnectorSelect*>(oconn));
		oconn->deactivate();
		JVX_SAFE_DELETE_OBJECT(oconn);
	}

	while (outputConnectors.size() < numConnectorsNew)
	{
		jvxSize idx = outputConnectors.size();
		CjvxSpNMultiplexerOutputConnector* oconn = nullptr;
		JVX_SAFE_ALLOCATE_OBJECT(oconn, CjvxSpNMultiplexerOutputConnector(this, idx));

		oconn->activate(static_cast<IjvxObject*>(this), static_cast<IjvxConnectorFactory*>(this),
			"out#" + jvx_size2String(idx), this, idx);
		oconn->report_trans = this;

		oneOutputConnectorElement elmOut;
		elmOut.theConnector = oconn;
		_common_set_conn_factory.output_connectors[elmOut.theConnector] = elmOut;

		outputConnectors.push_back(oconn);
	}
	return JVX_NO_ERROR;
}

void
CjvxSpNMultiplexer::update_monitor()
{
	jvxSize numConnected = 0;
	jvxSize numLinked = 0;
	for (auto& elm : outputConnectors)
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
	genMultiplexer_node::monitor.number_connectors_connected.value = numConnected;
	genMultiplexer_node::monitor.number_connectors_zerocopy.value = numLinked;
}

// =====================================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMultiplexer, set_config)
{
	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genMultiplexer_node::config.number_output_connectors))
	{
		jvxErrorType res = update_output_connectors(genMultiplexer_node::config.number_output_connectors.value);
		if (res != JVX_NO_ERROR)
		{
			// Not possible in the current state, fall back to what is currently there
			genMultiplexer_node::config.number_output_connectors.value = outputConnectors.size();
			CjvxProperties::add_property_report_collect(genMultiplexer_node::config.number_output_connectors.descriptor.std_str(), false);
		}
	}

	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genMultiplexer_node::config.channels_per_connector))
	{
		// New channel assignment, run the test function
		this->inform_chain_test();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNMultiplexer::put_configuration(jvxCallManagerConfiguration* callMan,
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
			genMultiplexer_node::put_configuration_all(callMan, processor, sectionToContainAllSubsectionsForMe, &warns);
			JVX_PROPERTY_PUT_CONFIGRATION_WARNINGS_STD_COUT(warns);

			// Create the connectors before the connections are established
			if (update_output_connectors(genMultiplexer_node::config.number_output_connectors.value) != JVX_NO_ERROR)
			{
				genMultiplexer_node::config.number_output_connectors.value = outputConnectors.size();
			}
		}
	}
	return res;
}

jvxErrorType
CjvxSpNMultiplexer::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::get_configuration(callMan,
		processor, sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		genMultiplexer_node::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	}
	return res;
}
