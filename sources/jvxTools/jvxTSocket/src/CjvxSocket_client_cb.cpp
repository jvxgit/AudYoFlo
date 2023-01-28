#include "CjvxSocket.h"
	
jvxErrorType 
CjvxSocket::ccallback_report_unsuccesful_outgoing(jvxHandle* privateData, jvx_connect_fail_reason reason, jvxSize ifidx)
{
	if(privateData)
	{
		CjvxSocket* myPtr = (CjvxSocket*)privateData;
		return myPtr->ic_ccallback_report_unsuccesful_outgoing(reason, ifidx);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType 
CjvxSocket::ccallback_report_connect_outgoing(jvxHandle* privateData, jvxSize ifidx)
{
	if(privateData)
	{
		CjvxSocket* myPtr = (CjvxSocket*)privateData;
		return myPtr->ic_ccallback_report_connect_outgoing(ifidx);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType 
CjvxSocket::ccallback_report_disconnect_outgoing(jvxHandle* privateData, jvxSize ifidx)
{
	if(privateData)
	{
		CjvxSocket* myPtr = (CjvxSocket*)privateData;
		return myPtr->ic_ccallback_report_disconnect_outgoing(ifidx);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

// =============================================================================================


jvxErrorType 
CjvxSocket::ic_ccallback_report_unsuccesful_outgoing(jvx_connect_fail_reason reason, jvxSize ifidx)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnection_report* theReport = NULL;
	theReport = runtime.client.ifaces[ifidx].theReport;
	if (theReport)
	{
		jvxBitField mask;
		jvx_bitSet(mask, JVX_SOCKET_CLIENT_REPORT_UNSUCCESSFUL_OUTGOING_SHFT);
		res = theReport->report_event(mask, ifidx, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);

	}
	return res;
}
	
jvxErrorType 
CjvxSocket::ic_ccallback_report_connect_outgoing(jvxSize ifidx)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnection_report* theReport = NULL;
	theReport = runtime.client.ifaces[ifidx].theReport;
	if (theReport)
	{
		jvxBitField mask;
		jvx_bitSet(mask, JVX_SOCKET_CLIENT_REPORT_CONNECT_OUTGOING_SHFT);
		res = theReport->report_event(mask, ifidx, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);

	}
	return res;
}

jvxErrorType 
CjvxSocket::ic_ccallback_report_disconnect_outgoing(jvxSize ifidx)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnection_report* theReport = NULL;
	theReport = runtime.client.ifaces[ifidx].theReport;
	if (theReport)
	{
		jvxBitField mask;
		jvx_bitSet(mask, JVX_SOCKET_CLIENT_REPORT_DISCONNECT_OUTGOING_SHFT);
		res = theReport->report_event(mask, ifidx, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);

	}
	return res;
}


