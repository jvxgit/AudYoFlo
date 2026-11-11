#include "CjvxSpNMultiplexer.h"

CjvxSpNMultiplexerOutputConnector::CjvxSpNMultiplexerOutputConnector(CjvxSpNMultiplexer* parentArg, jvxSize idxArg) :
	CjvxSingleOutputConnector(false), parent(parentArg), idx(idxArg)
{
	// Each connector can be connected exactly once
	allowSingleConnect = true;
}

jvxErrorType
CjvxSpNMultiplexerOutputConnector::available_to_connect_ocon()
{
	jvxErrorType res = parent->check_output_connector_available(this, nullptr);
	if (res == JVX_NO_ERROR)
	{
		res = CjvxSingleOutputConnector::available_to_connect_ocon();
	}
	return res;
}

jvxErrorType
CjvxSpNMultiplexerOutputConnector::select_connect_ocon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common, IjvxOutputConnector** replace_connector)
{
	jvxErrorType res = parent->check_output_connector_available(this, ass_connection_common);
	if (res == JVX_NO_ERROR)
	{
		res = CjvxSingleOutputConnector::select_connect_ocon(obj, master, ass_connection_common, replace_connector);
	}
	return res;
}
