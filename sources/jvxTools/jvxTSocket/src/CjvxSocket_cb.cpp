#include "CjvxSocket.h"

// =================================================================

jvxErrorType
CjvxSocket::sccallback_report_startup_complete(jvxHandle* privateData, jvxSize ifidx)
{
	if (privateData)
	{
		CjvxSocket* myPtr = (CjvxSocket*)privateData;
		return myPtr->ic_sccallback_report_startup_complete(ifidx);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxSocket::sccallback_report_shutdow_complete(jvxHandle* privateData, jvxSize ifidx)
{
	if (privateData)
	{
		CjvxSocket* myPtr = (CjvxSocket*)privateData;
		return myPtr->ic_sccallback_report_shutdow_complete(ifidx);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxSocket::sccallback_report_error(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx)
{
	if (privateData)
	{
		CjvxSocket* myPtr = (CjvxSocket*)privateData;
		return myPtr->ic_sccallback_report_error(errCode, errDescription, ifidx);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxSocket::sccallback_provide_data_and_length(jvxHandle* privateData, jvxSize channelId, char** fld, jvxSize*sz, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	if (privateData)
	{
		CjvxSocket* myPtr = (CjvxSocket*)privateData;
		return myPtr->ic_sccallback_provide_data_and_length(channelId, fld, sz, offset, idIf, additionalInfo, whatsthis);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxSocket::sccallback_report_data_and_read(jvxHandle* privateData, jvxSize channelId, char* fld, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	if (privateData)
	{
		CjvxSocket* myPtr = (CjvxSocket*)privateData;
		return myPtr->ic_sccallback_report_data_and_read(channelId, fld, sz, offset, idIf, additionalInfo, whatsthis);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}
// =================================================================

jvxErrorType
CjvxSocket::ic_sccallback_report_startup_complete(jvxSize ifidx)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnection_report* theReport = NULL;
	switch (config.whatToAddress)
	{
	case JVX_SOCKET_ADDRESS_SERVER:
		theReport = runtime.server.ifaces[ifidx].theReport;
		break;
	case JVX_SOCKET_ADDRESS_CLIENT:
		theReport = runtime.client.ifaces[ifidx].theReport;
		break;
	default:
		assert(0);
	}

	if (theReport)
	{
		jvxBitField mask;
		jvx_bitSet(mask, JVX_CONNECTION_REPORT_STARTUP_COMPLETE_SHFT);
		res = theReport->report_event(mask, ifidx, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
	}
	return res;
}

jvxErrorType
CjvxSocket::ic_sccallback_report_shutdow_complete(jvxSize ifidx)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnection_report* theReport = NULL;
	switch (config.whatToAddress)
	{
	case JVX_SOCKET_ADDRESS_SERVER:
		theReport = runtime.server.ifaces[ifidx].theReport;
		break;
	case JVX_SOCKET_ADDRESS_CLIENT:
		theReport = runtime.client.ifaces[ifidx].theReport;
		break;
	default:
		assert(0);
	}
	if (theReport)
	{
		jvxBitField mask;
		jvx_bitSet(mask, JVX_CONNECTION_REPORT_SHUTDOWN_COMPLETE_SHFT);
		res = theReport->report_event(mask, ifidx, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);

	}
	return res;
}

jvxErrorType
CjvxSocket::ic_sccallback_report_error(jvxInt32 errCode, const char* errDescription, jvxSize ifidx)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnection_report* theReport = NULL;
	switch (config.whatToAddress)
	{
	case JVX_SOCKET_ADDRESS_SERVER:
		theReport = runtime.server.ifaces[ifidx].theReport;
		break;
	case JVX_SOCKET_ADDRESS_CLIENT:
		theReport = runtime.client.ifaces[ifidx].theReport;
		break;
	default:
		assert(0);
	}
	if (theReport)
	{
		jvxBitField mask;
		jvx_bitSet(mask, JVX_CONNECTION_REPORT_ERROR_SHFT);
		jvx_socket_error_private myError;
		myError.errCode = errCode;
		myError.errDescr = errDescription;
		res = theReport->report_event(mask, ifidx, &myError, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_ERROR);

	}
	return res;

	std::cout << "Callback " << __FUNCTION__ << std::endl;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocket::ic_sccallback_report_data_and_read(jvxSize channelId, char* ptr, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnection_report* theReport = NULL;
	switch (config.whatToAddress)
	{
	case JVX_SOCKET_ADDRESS_SERVER:
		theReport = runtime.server.ifaces[idIf].theReport;
		/*
		switch (config.addressViaWhatProtocol)
		{
		case JVX_SOCKET_OPERATE_VIA_UDP:
			break;
		case JVX_SOCKET_OPERATE_VIA_PCAP:
			assert(0);
			break;
		}*/
		break;
	case JVX_SOCKET_ADDRESS_CLIENT:
		theReport = runtime.client.ifaces[idIf].theReport;
		break;
	default:
		assert(0);
	}
	if (theReport)
	{
		res = theReport->report_data_and_read(ptr, sz, offset, idIf, additionalInfo, whatsthis);
	}
	return res;
}

jvxErrorType
CjvxSocket::ic_sccallback_provide_data_and_length(jvxSize channelId, char** ptr, jvxSize* sz, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnection_report* theReport = NULL;
	switch (config.whatToAddress)
	{
	case JVX_SOCKET_ADDRESS_SERVER:
		theReport = runtime.server.ifaces[idIf].theReport;
		break;
	case JVX_SOCKET_ADDRESS_CLIENT:
		theReport = runtime.client.ifaces[idIf].theReport;
		break;
	default:
		assert(0);
	}
	if (theReport)
	{
		res = theReport->provide_data_and_length(ptr, sz, offset, idIf, additionalInfo, whatsthis);
	}
	return res;
}
