#include "CjvxSockets.h"

CjvxSocketsConnection_transfer::CjvxSocketsConnection_transfer(jvxByte* bufSendArg, jvxSize bufSizeArg)
{
	bufSend = bufSendArg;
	bufSize = bufSizeArg;
}

CjvxSocketsConnection_transfer::CjvxSocketsConnection_transfer()
{

}

void 
CjvxSocketsConnection_transfer::set_buffer(jvxByte* bufSendArg, jvxSize bufSizeArg)
{
	bufSend = bufSendArg;
	bufSize = bufSizeArg;
}

jvxErrorType 
CjvxSocketsConnection_transfer::get_data(jvxByte** buf_send, jvxSize* numElements) 
{
	if (buf_send)
	{
		*buf_send = bufSend;
	}
	if (numElements)
	{
		*numElements = bufSize;
	}
	return JVX_NO_ERROR;
}

jvxSocketsConnectionTransfer 
CjvxSocketsConnection_transfer::type() 
{
	return jvxSocketsConnectionTransfer::JVX_SOCKET_TRANSFER_NORMAL;
}

jvxErrorType 
CjvxSocketsConnection_transfer::specialization(jvxHandle** ptr, jvxSocketsConnectionTransfer tp) 
{
	return JVX_ERROR_UNSUPPORTED;
}

// ================================================================
jvxErrorType
CjvxSocketsConnection::register_report(IjvxSocketsConnection_report* report)
{
	if (this->theReportConnection == NULL)
	{
		theReportConnection = report;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CjvxSocketsConnection::unregister_report(IjvxSocketsConnection_report* report)
{
	if (this->theReportConnection == report)
	{
		theReportConnection = NULL;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}
