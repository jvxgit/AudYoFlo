#include "CjvxSocket.h"


jvxErrorType
CjvxSocket::scallback_report_denied_incoming(jvxHandle* privateData, const char* description, jvxSize ifidx)
{
	if (privateData)
	{
		CjvxSocket* myPtr = (CjvxSocket*)privateData;
		return myPtr->ic_scallback_report_denied_incoming(description, ifidx);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxSocket::scallback_report_connect_incoming(jvxHandle* privateData, jvxSize channelId, const char* description, jvxSize ifidx)
{
	if (privateData)
	{
		CjvxSocket* myPtr = (CjvxSocket*)privateData;
		return myPtr->ic_scallback_report_connect_incoming(channelId, description, ifidx);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxSocket::scallback_report_disconnect_incoming(jvxHandle* privateData, jvxSize channelId, jvxSize ifidx)
{
	if (privateData)
	{
		CjvxSocket* myPtr = (CjvxSocket*)privateData;
		return myPtr->ic_scallback_report_disconnect_incoming(channelId, ifidx);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}


// =================================================================

jvxErrorType
CjvxSocket::ic_scallback_report_denied_incoming(const char* description, jvxSize ifidx)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnection_report* theReport = NULL;
	theReport = runtime.server.ifaces[ifidx].theReport;
	if (theReport)
	{
		jvxBitField mask;
		jvx_bitSet(mask, JVX_SOCKET_SERVER_REPORT_DENIED_INCOMING_SHFT);
		res = theReport->report_event(mask, ifidx, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);

	}
	return res;
}

jvxErrorType
CjvxSocket::ic_scallback_report_connect_incoming(jvxSize channelId, const char* description, jvxSize ifidx)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnection_report* theReport = NULL;
	jvx_socket_connection_tcp_private myConnData;
	theReport = runtime.server.ifaces[ifidx].theReport;
	if (theReport)
	{
		jvxBitField mask;
		jvx_bitSet(mask, JVX_SOCKET_SERVER_REPORT_CONNECT_INCOMING_SHFT);
		myConnData.origin = description;
		myConnData.idConnection = channelId;
		res = theReport->report_event(mask, ifidx, (jvxHandle*)&myConnData, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONNECTION_TCP);

	}
	return res;
}

jvxErrorType
CjvxSocket::ic_scallback_report_disconnect_incoming(jvxSize channelId, jvxSize ifidx)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnection_report* theReport = NULL;
	theReport = runtime.server.ifaces[ifidx].theReport;
	if (theReport)
	{
		jvxBitField mask;
		jvx_bitSet(mask, JVX_SOCKET_SERVER_REPORT_DISCONNECT_INCOMING_SHFT);
		res = theReport->report_event(mask, ifidx, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);

	}
	return res;
}

