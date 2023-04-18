#include "common/CjvxInputConnector.h"

CjvxInputConnectorCore::CjvxInputConnectorCore()
{
	_common_set_icon.conn_in = NULL;
	_common_set_icon.theCommon_to = NULL;
	_common_set_icon.theData_in = NULL;
}

jvxErrorType
CjvxInputConnectorCore::_associated_common_icon(IjvxDataConnectionCommon** ref)
{
	if (ref)
	{
		*ref = _common_set_icon.theCommon_to;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxInputConnectorCore::_supports_connector_class_icon(
	jvxDataFormatGroup format_group,
	jvxDataflow data_flow)
{
	if (_common_set_icon.caps_in.format_group != JVX_DATAFORMAT_GROUP_NONE)
	{
		if (_common_set_icon.caps_in.format_group != format_group)
		{
			return JVX_ERROR_UNSUPPORTED;
		}
	}
	if (_common_set_icon.caps_in.data_flow != JVX_DATAFLOW_NONE)
	{
		if (_common_set_icon.caps_in.data_flow != data_flow)
		{
			return JVX_ERROR_UNSUPPORTED;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxInputConnectorCore::_select_connect_icon(IjvxConnectorBridge* bri, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common,
	IjvxInputConnector** replace_connector)
{
	// To and from conector must be associated in the same process/group
	if (_common_set_icon.theCommon_to != NULL)
	{
		return JVX_ERROR_ALREADY_IN_USE;
	}

	// Here, we can check more generally the status before connect
	jvxErrorType res = _check_common_icon(ass_connection_common, master);
	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	if (_common_set_icon.conn_in == NULL)
	{
		_common_set_icon.conn_in = bri;
		_common_set_icon.theCommon_to = ass_connection_common;

		// What else to do here?
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_COMPONENT_BUSY;
}

jvxErrorType
CjvxInputConnectorCore::_unselect_connect_icon(IjvxConnectorBridge* bri, IjvxInputConnector* replace_connector)
{
	if (_common_set_icon.conn_in == bri)
	{
		_common_set_icon.conn_in = NULL;
		_common_set_icon.theCommon_to = NULL;

		// What else to do here?
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxInputConnectorCore::_connected_ocon(IjvxOutputConnector** ocon)
{
	if (ocon)
	{
		*ocon = nullptr;
		if (_common_set_icon.theData_in)
		{
			*ocon = _common_set_icon.theData_in->con_link.connect_from;
		}
	}
	return JVX_NO_ERROR;
}
