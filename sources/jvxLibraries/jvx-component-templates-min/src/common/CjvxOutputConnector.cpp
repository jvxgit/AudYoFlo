#include "common/CjvxOutputConnector.h"

CjvxOutputConnectorCore::CjvxOutputConnectorCore()
{
	_common_set_ocon.conn_out = NULL;
	_common_set_ocon.theCommon_from = NULL;
	jvx_initDataLinkDescriptor(&_common_set_ocon.theData_out);
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
CjvxOutputConnectorCore::_connected_icon(IjvxInputConnector** ocon)
{
	if (ocon)
	{
		*ocon = nullptr;
		*ocon = _common_set_ocon.theData_out.con_link.connect_to;
	}
	return JVX_NO_ERROR;
}




