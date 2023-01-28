#ifdef JVX_WITH_PCAP

#include "CjvxSocketsClientInterfacePcap.h"

CjvxSocketsClientInterfacePcap::CjvxSocketsClientInterfacePcap() :
	CjvxSocketsClientInterface()
{
	memset(mac, 0, sizeof(mac));
}

jvxErrorType
CjvxSocketsClientInterfacePcap::pre_start_socket()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsClientInterfacePcap::start_socket()
{
	int pcapsflags = PCAP_OPEN_FLAGS_NORMAL;
	char errbuf[PCAP_ERRBUF_SIZE];

	pcapsflags = PCAP_OPEN_FLAGS_NO_SELF_CAPTURE;
#ifdef JVX_OS_WINDOWS
	thePcapSocket = pcap_open(name.c_str(),
		PCAP_SNAP_LEN /* snaplen */,
		pcapsflags /* flags */,
		PCAP_READ_TIMEOUT,
		NULL,
		errbuf);
#else // JVX_OS_WINDOWS
	thePcapSocket = pcap_open_live(pcap_name.c_str(),
		PCAP_SNAP_LEN /* snaplen */,
		pcapsflags /* flags */,
		PCAP_READ_TIMEOUT,
		errbuf);
#endif // JVX_OS_WINDOWS

	if (thePcapSocket == nullptr)
	{
		last_error = errbuf;
		return JVX_ERROR_INTERNAL;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsClientInterfacePcap::start_connection_loop()
{
	// Create thread:
	// jvx_connect_loop_pcap(theHandleId);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsClientInterfacePcap::set_opts_socket()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsClientInterfacePcap::connect_socket()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsClientInterfacePcap::stop_connection_loop()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsClientInterfacePcap::stop_socket()
{
	return JVX_NO_ERROR;
}

// ====================================================================
jvxErrorType 
CjvxSocketsConnectionPcap::configure(CjvxSocketsClientInterfacePcap* par)
{
	parent = par;
	return JVX_NO_ERROR;
}

void 
CjvxSocketsConnectionPcap::main_loop() 
{
}

#endif