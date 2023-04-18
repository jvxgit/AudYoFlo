#include "common/CjvxOutputConnectorCore.h"

CjvxOutputConnectorCore::CjvxOutputConnectorCore()
{
	_common_set_ocon.conn_out = NULL;
	_common_set_ocon.theCommon_from = NULL;
	_common_set_ocon.allows_termination = true;
	_common_set_ocon.setup_for_termination = false;
	_common_set_ocon.ocon = NULL;

	jvx_initDataLinkDescriptor(&_common_set_ocon.theData_out);
}

jvxErrorType 
CjvxOutputConnectorCore::activate(IjvxOutputConnector* ocon)
{
	_common_set_ocon.ocon = ocon;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxOutputConnectorCore::deactivate()
{
	_common_set_ocon.ocon = nullptr;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxOutputConnectorCore::_associated_common_ocon(IjvxDataConnectionCommon** ref)
{
	if (ref)
	{
		*ref = _common_set_ocon.theCommon_from;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxOutputConnectorCore::_supports_connector_class_ocon(
	jvxDataFormatGroup format_group,
	jvxDataflow data_flow)
{
	if (_common_set_ocon.caps_out.format_group != JVX_DATAFORMAT_GROUP_NONE)
	{
		if (_common_set_ocon.caps_out.format_group != format_group)
		{
			return JVX_ERROR_UNSUPPORTED;
		}
	}
	if (_common_set_ocon.caps_out.data_flow != JVX_DATAFLOW_NONE)
	{
		if (_common_set_ocon.caps_out.data_flow != data_flow)
		{
			return JVX_ERROR_UNSUPPORTED;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxOutputConnectorCore::_select_connect_ocon(IjvxConnectorBridge* bri, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common, IjvxOutputConnector** replace_connector)
{
	// To and from conector must be associated in the same process/group
	if (_common_set_ocon.theCommon_from != NULL)
	{
		return JVX_ERROR_ALREADY_IN_USE;
	}

	jvxErrorType res = _check_common_ocon(ass_connection_common, master);
	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	// If no bridge was set before
	if (_common_set_ocon.conn_out == NULL)
	{
		_common_set_ocon.conn_out = bri;
		_common_set_ocon.theCommon_from = ass_connection_common;

		// What else to do here?
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_COMPONENT_BUSY;
}

jvxErrorType
CjvxOutputConnectorCore::_unselect_connect_ocon(IjvxConnectorBridge* bri, IjvxOutputConnector* replace_connector)
{
	if (_common_set_ocon.conn_out)
	{
		_common_set_ocon.conn_out = NULL;
		_common_set_ocon.theCommon_from = NULL;

		// What else to do here?
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

// ===============================================================================

jvxErrorType
CjvxOutputConnectorCore::_connect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb), jvxDataProcessorHintDescriptor* add_me)
{
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
	if (_common_set_ocon.conn_out)
	{
		jvx_initDataLinkDescriptor(&_common_set_ocon.theData_out);
		assert(_common_set_ocon.theData_out.con_link.connect_to == NULL);
		_common_set_ocon.conn_out->reference_connect_to(&_common_set_ocon.theData_out.con_link.connect_to);
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			_common_set_ocon.theData_out.con_link.master = args.theMaster;
			_common_set_ocon.theData_out.con_link.uIdConn = args.uIdConnection;
			_common_set_ocon.theData_out.con_link.connect_from = _common_set_ocon.ocon;
			_common_set_ocon.theData_out.con_user.chain_spec_user_hints = add_me;

			res = _common_set_ocon.theData_out.con_link.connect_to->connect_connect_icon(&_common_set_ocon.theData_out 
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (res != JVX_NO_ERROR)
			{
				_common_set_ocon.conn_out->return_reference_connect_to(_common_set_ocon.theData_out.con_link.connect_to);
				_common_set_ocon.theData_out.con_link.connect_to = NULL;
				jvx_terminateDataLinkDescriptor(&_common_set_ocon.theData_out);
			}
		}
		else
		{
			assert(0);
			if (!_common_set_ocon.allows_termination)
			{
				std::cout << "This output connector does not allow termination but is not connected to a connected bridge (no input connector in bridge)." << std::endl;
				res = JVX_ERROR_NOT_READY;
			}
		}
	}
	else
	{
		if (!_common_set_ocon.allows_termination)
		{
			std::cout << "This output connector does not allow termination but is not connected to a connected bridge (no input connector in bridge)." << std::endl;
			res = JVX_ERROR_NOT_READY;
		}
	}
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
	return res;
}

jvxErrorType
CjvxOutputConnectorCore::_disconnect_connect_ocon(const jvxChainConnectArguments& args, JVX_CONNECTION_FEEDBACK_TYPE_F(fdb) jvxDataProcessorHintDescriptor** pointer_remove)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ocon.conn_out)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->disconnect_connect_icon(&_common_set_ocon.theData_out JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (pointer_remove)
		{
			*pointer_remove = _common_set_ocon.theData_out.con_user.chain_spec_user_hints;
		}
		_common_set_ocon.conn_out->return_reference_connect_to(_common_set_ocon.theData_out.con_link.connect_to);
		_common_set_ocon.theData_out.con_link.connect_to = nullptr;
		jvx_terminateDataLinkDescriptor(&_common_set_ocon.theData_out);
	}
	else
	{
		std::cout << "Not connected" << std::endl;
	}
	return res;
}

// ===============================================================================

jvxErrorType
CjvxOutputConnectorCore::_prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Content of link descriptor has been set before...
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	else
	{
		if (_common_set_ocon.allows_termination)
		{
			// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
			// Accepted as chain terminator
		}
		else
		{
			std::cout << "This output connector is not connected to any bridge." << std::endl;
			res = JVX_ERROR_UNEXPECTED;
		}
	}
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif

	return res;
};

jvxErrorType
CjvxOutputConnectorCore::_postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Content of link descriptor has been set before...
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	else
	{
		if (_common_set_ocon.allows_termination)
		{
			// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
			// Accepted as chain terminator
		}
		else
		{
			std::cout << "This output connector is not connected to any bridge." << std::endl;
			res = JVX_ERROR_UNEXPECTED;
		}
	}

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif

	return res;
}

// ===============================================================================

jvxErrorType
CjvxOutputConnectorCore::_start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Content of link descriptor has been set before...
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	else
	{
		if (_common_set_ocon.allows_termination)
		{
			// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
			// Accepted as chain terminator
		}
		else
		{
			std::cout << "This output connector is not connected to any bridge." << std::endl;
			res = JVX_ERROR_UNEXPECTED;
		}
	}

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif

	return res;
}

jvxErrorType
CjvxOutputConnectorCore::_stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Content of link descriptor has been set before...
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	else
	{
		if (_common_set_ocon.allows_termination)
		{
			// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
			// Accepted as chain terminator
		}
		else
		{
			std::cout << "This output connector is not connected to any bridge." << std::endl;
			res = JVX_ERROR_UNEXPECTED;
		}
	}
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
	std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif

	return res;
}
// ===============================================================================

jvxErrorType
CjvxOutputConnectorCore::_connected_icon(IjvxInputConnector** ocon)
{
	if (ocon)
	{
		*ocon = nullptr;
		*ocon = _common_set_ocon.theData_out.con_link.connect_to;
	}
	return JVX_NO_ERROR;
}




