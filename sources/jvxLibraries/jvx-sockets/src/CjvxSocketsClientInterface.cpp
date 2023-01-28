#include "CjvxSockets.h"

// =========================================================================
// =========================================================================

CjvxSocketsClientInterface::CjvxSocketsClientInterface(jvxSocketsConnectionType socketTypeArg)
{
	socketType = socketTypeArg;

	// Connection must be protected: on "stop", the connection may have been removed from the other side at the same time!
	JVX_INITIALIZE_MUTEX(safeAccessConnection);
}

CjvxSocketsClientInterface::~CjvxSocketsClientInterface()
{
	JVX_TERMINATE_MUTEX(safeAccessConnection);
}

jvxErrorType
CjvxSocketsClientInterface::configure()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(
		(theState == jvxSocketsConnectionState::JVX_STATE_CONNECTION_INIT)||
		(theState == jvxSocketsConnectionState::JVX_STATE_CONNECTION_STARTED))
	{
		idMainThread = JVX_GET_CURRENT_THREAD_ID();
		switch (socketType)
		{
		case jvxSocketsConnectionType::JVX_SOCKET_TYPE_TCP:

			// Args disableNagle + connection info
			break;

		case jvxSocketsConnectionType::JVX_SOCKET_TYPE_UDP:

			// Args connection info
			break;

#ifdef JVX_WITH_PCAP
		case jvxSocketsConnectionType::JVX_SOCKET_TYPE_PCAP:
			break;

#endif
#ifdef OS_SUPPORTS_BTH
		case jvxSocketType::JVX_SOCKET_TYPE_BTH:
			break;
#endif
		default:
			assert(0);
		}

#if 0
		case JVX_CONNECT_SOCKET_TYPE_TCP:
			assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_TCP);
			theIf->config.modeSpecific.forTcpUdp = *((jvx_connect_spec_tcp*)config_specific);
			break;
		case JVX_CONNECT_SOCKET_TYPE_UDP:
			assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_UDP);
			theIf->config.modeSpecific.forTcpUdp.connect = *((jvx_connect_spec_udp*)config_specific);
			theIf->config.modeSpecific.forTcpUdp.disableNagleAlgorithmTcp = false;

			if (!theIf->config.modeSpecific.forTcpUdp.connect.targetName.empty())
			{
				jvx_connect_client_prepare_udp_target_st__core(theIf->config.modeSpecific.forTcpUdp.connect.targetName.c_str(),
					&theIf->runtime.targetServer, theIf->config.modeSpecific.forTcpUdp.connect.connectToPort, 
					theIf->config.modeSpecific.forTcpUdp.connect.family);
				theIf->runtime.targetServerSet = true;
			}
			break;
#ifdef JVX_WITH_PCAP
		case JVX_CONNECT_SOCKET_TYPE_PCAP:
			assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_PCAP);
			theIf->config.modeSpecific.forPcap = *((jvx_connect_spec_pcap*)config_specific);
			break;
#endif
#ifdef OS_SUPPORTS_BTH
		case JVX_CONNECT_SOCKET_TYPE_BTH:
			theIf->config.modeSpecific.forBth = ((jvx_connect_spec_bth*)config_specific);
			break;
#endif
#endif
	
		theState = jvxSocketsConnectionState::JVX_STATE_CONNECTION_STARTED;
		return JVX_NO_ERROR;
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxSocketsClientInterface::start(IjvxSocketsClient_report* theReportClientArg)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;

#ifdef OS_SUPPORTS_BTH
	SOCKADDR_BTH RemoteBthAddr = { 0 };
#endif
	int flag = 0;
	int errCode = 0;
	// int family_int = 0;

	if (theState == jvxSocketsConnectionState::JVX_STATE_CONNECTION_STARTED)
	{
		if (idMainThread != JVX_GET_CURRENT_THREAD_ID())
		{
			std::cout << __FUNCTION__ << "Warning: Call from non-main thread detected." << std::endl;
		}
		theReportClient = theReportClientArg;

		// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
		// Create socket
		// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
		res = pre_start_socket();
		if (res != JVX_NO_ERROR)
		{
			return res;
		}

		res = start_socket();
		if (res != JVX_NO_ERROR)
		{
			goto exit_fail_II;
		}

		res = set_opts_socket();
		if (res != JVX_NO_ERROR)
		{
			goto exit_fail_I;
		}

		res = connect_socket();
		if (res == JVX_NO_ERROR)
		{
			theState = jvxSocketsConnectionState::JVX_STATE_CONNECTION_CONNECTED;
			res = start_connection_loop();
		}
		else
		{
			goto exit_fail_I;
		}
	}

	return res;

	// Error cases
exit_fail_I:

	stop_socket();
	theReportClient = NULL;
	theState = jvxSocketsConnectionState::JVX_STATE_CONNECTION_STARTED;

exit_fail_II:
	return res;
}

jvxErrorType
CjvxSocketsClientInterface::stop()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (
		(theState == jvxSocketsConnectionState::JVX_STATE_CONNECTION_CONNECTED) ||
		(theState == jvxSocketsConnectionState::JVX_STATE_CONNECTION_DISCONNECTED))
	{
		if (idMainThread != JVX_GET_CURRENT_THREAD_ID())
		{
			std::cout << __FUNCTION__ << "Warning: Call from non-main thread detected." << std::endl;
		}

		// Stop connect loop waits until the status is disconnected
		stop_connection_loop();
		assert(theState == jvxSocketsConnectionState::JVX_STATE_CONNECTION_DISCONNECTED);

		stop_socket();
		theReportClient = NULL;
		theState = jvxSocketsConnectionState::JVX_STATE_CONNECTION_STARTED;
	}
	return res;
}

// ===========================================================================================


// ###############################################################################

#ifdef OS_SUPPORTS_BTH

BTH SOCKET


case CjvxSocketsClient::jvxSocketClientType::JVX_SOCKET_CLIENT_BTH:
	theSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	break;
default:
	assert(0);
}

jvxErrorType
CjvxSocketClientInterfacePcap::connect_socket()
{
	if (NameToBthAddr((LPSTR)theHandle->config.target.c_str(), &RemoteBthAddr) != 0)
	{
		assert(0);
	}

	RemoteBthAddr.addressFamily = AF_BTH;
	RemoteBthAddr.serviceClassId = g_guidServiceClass;
	RemoteBthAddr.port = 0;
	errCode = connect_timeout(theSocket, (const sockaddr*)&RemoteBthAddr, sizeof(SOCKADDR_BTH), theHandle->config.timeout_ms);
	return JVX_NO_ERROR;
}

#endif
