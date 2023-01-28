#include "mySocketReport.h"

mySocketReport::mySocketReport()
{
	szFld = 0;
	fld = NULL;
	JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_ASSIGN(
			theCallbacks, mySocketReport,
			startup_complete, shutdown_complete, provide_data_and_length, data_and_read, report_error, specific, 
			connect_incoming, disconnect_incoming, denied_incoming);
};

mySocketReport::~mySocketReport()
{
}

JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_SIMPLE(mySocketReport, startup_complete)
{
	std::cout << "Startup complete!" << std::endl;
	return JVX_NO_ERROR;
}

JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_SIMPLE(mySocketReport, shutdown_complete)
{
	std::cout << "Shutdown complete!" << std::endl;
	return JVX_NO_ERROR;
}

JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_PROVIDE_DATA(mySocketReport, provide_data_and_length)
{
	// jvxHandle* privateData, jvxSize channelId, char** bufptr, jvxSize* szbuf, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis
	if (szFld < *szbuf)
	{
		if (fld)
		{
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(fld, jvxByte);
		}
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, jvxByte, *szbuf);
	}

	*bufptr = fld;
	*offset = 0;

	return JVX_NO_ERROR;
}

JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_DATA_READ(mySocketReport, data_and_read)
{
	jvxByte* load = NULL;
	jvxSize numBytes = 0;
	jvxSize i;
	// jvxHandle* privateData, jvxSize channelId, char* buf, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis
	jvxRawStreamHeader* ptrStream = (jvxRawStreamHeader*)buf;
	std::cout << "New packet" << std::endl;
	std::cout << "--> Paket size:" << ptrStream->loc_header.paketsize << std::endl;
	load = buf + sizeof(jvxRawStreamHeader);
	numBytes = ptrStream->loc_header.paketsize - sizeof(jvxRawStreamHeader);
	std::cout << "[" << std::flush;
	for (i = 0; i < numBytes; i++)
	{
		if (i != 0)
			std::cout << ", " << std::flush;
		std::cout << jvx_int2String(load[i]) << std::flush;
	}
	std::cout << "]" << std::endl;

	return JVX_NO_ERROR;
}

JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_ERROR(mySocketReport, report_error)
{
	std::cout << "Error: " << errDescription << std::endl;
	return JVX_NO_ERROR;
}

JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_SPECIFIC(mySocketReport, specific)
{
	std::cout << "Specific callback" << std::endl;
	return JVX_NO_ERROR;
}

JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_DENIED_IN(mySocketReport, denied_incoming)
{
	std::cout << "Denied in callback" << std::endl;
	return JVX_NO_ERROR;
}

JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_CONNECT_IN(mySocketReport, connect_incoming)
{
	std::cout << "Connect in callback" << std::endl;
	return JVX_NO_ERROR;
}

JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_DISCONNECT_IN(mySocketReport, disconnect_incoming)
{
	std::cout << "Disconnect in callback" << std::endl;
	return JVX_NO_ERROR;
}


