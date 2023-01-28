#ifdef _MSC_VER
extern "C"
{
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2bth.h>
}
#endif

#include "jvx_dsp_base.h"
#include "jvx_connect_client_st.h"
#include "jvx_connect_server_st.h"

// include socket stuff
#include <sys/types.h>

#ifdef OS_SUPPORTS_BTH
#include <initguid.h>
#endif

#include "jvx_connect_common_helpers.h"

#define JVX_DEPRECATED_SOCKET_API
#define JVX_MAP_TO_CLIENT_WHERE_POSSIBLE

extern jvxHandle* jvx_connect_client_priv_get_callback_priv(jvx_connect_client_if* hdlClient);

// =============================================================================================
// Typedefs of structs
// =============================================================================================

struct jvx_connect_server_family;

struct jvx_connect_server_if
{
	jvx_connect_server_family* priv;
	jvxSize id;
};

typedef struct
{
	JVX_SOCKET theSocket;
	int idConnection;
	bool inuse;
	/*
		I just decied not to protect a single connection. If we call a single socket in the wrong state, the
		worst that can happen is that the call fails due to an invalid socket. No dedicated memory is part of the 
		open status of the socket.
	*/
	// JVX_RW_MUTEX_HANDLE safeAccessConnection;
#ifdef JVX_WITH_PCAP
	pcap_t* thePcapSocket;

#ifndef JVX_PCAP_DIRECT_SEND
	pcap_send_queue* my_send_queue;
#endif
#endif

} oneServerConnection;

typedef struct
{
	struct
	{
		jvxSize numAllConnections;
		jvx_connect_socket_type socket_type;
		std::string name;
		std::string description;

		struct
		{
			jvx_connect_spec_tcp forTcp;
			jvx_connect_spec_udp forUdp;
#ifdef JVX_WITH_PCAP
			jvx_connect_spec_pcap forPcap;
#endif
#ifdef OS_SUPPORTS_BTH
			jvx_connect_spec_bth forBth;
#endif
		} modeSpecific;
	} config;

	struct
	{
		JVX_THREAD_HANDLE threadHdl;
		JVX_THREAD_ID threadId;
	} thread;

	struct
	{
		JVX_THREAD_HANDLE threadHdl;
		JVX_THREAD_ID threadId;
	} thread_delayedReport;

	struct
	{
		oneServerConnection* spaceForAllConnections;
		int numAllConnections_inuse;

		JVX_SOCKET theSocket;
		jvx_connect_state theState;
		JVX_RW_MUTEX_HANDLE safeAccess;
#ifdef JVX_WITH_PCAP
		char mac[6];
#endif
		jvxTimeStampData tStamp;

	} runtime;

	struct
	{
		jvx_connect_server_callback_type* handlerStruct;
		jvxHandle* privData;
	} callbacks;


	jvx_connect_server_if thisisme;
} oneServerInterface;

struct jvx_connect_server_family
{
	std::vector<oneServerInterface> theIfs;

	struct
	{
		jvx_connect_socket_type socket_type;
	} config;

	struct
	{
		jvx_connect_client_family* fam;
		jvx_connect_client_if* theIf;
		jvxCBool uselink;
		jvx_connect_client_callback_type callbacks;
		jvxHandle* priv;
	} linkToClient;
};



// Static callback wrappers
static jvxErrorType jvx_server_local_connected_outgoing(jvxHandle* privateData, jvxSize ifidx)
{
	jvx_connect_server_if* hdl = (jvx_connect_server_if*)privateData;
	jvx_connect_server_family* hdlFamily = (jvx_connect_server_family*)hdl->priv;
	oneServerInterface* theIf = &hdlFamily->theIfs[hdl->id];
	assert(hdlFamily->linkToClient.uselink);
	return(theIf->callbacks.handlerStruct->callback_report_connect_incoming(theIf->callbacks.privData, 0, "", ifidx));
}

static jvxErrorType jvx_server_local_disconnect_outgoing(jvxHandle* privateData, jvxSize ifidx)
{
	jvx_connect_server_if* hdl = (jvx_connect_server_if*)privateData;
	jvx_connect_server_family* hdlFamily = (jvx_connect_server_family*)hdl->priv;
	oneServerInterface* theIf = &hdlFamily->theIfs[hdl->id];

	assert(hdlFamily->linkToClient.uselink);
	return(theIf->callbacks.handlerStruct->callback_report_disconnect_incoming(theIf->callbacks.privData, 0, ifidx));
}

static jvxErrorType jvx_server_local_unsuccessful_outgoing(jvxHandle* privateData, jvx_connect_fail_reason reason, jvxSize ifidx)
{
	jvx_connect_server_if* hdl = (jvx_connect_server_if*)privateData;
	jvx_connect_server_family* hdlFamily = (jvx_connect_server_family*)hdl->priv;
	oneServerInterface* theIf = &hdlFamily->theIfs[hdl->id];

	assert(hdlFamily->linkToClient.uselink);
	assert(0); // <- this call should never happen in server mode for the client matched technologies
	return(theIf->callbacks.handlerStruct->callback_report_denied_incoming(theIf->callbacks.privData, "Failed", ifidx));
}

jvxErrorType jvx_server_local_startup_complete(jvxHandle* privateData, jvxSize ifidx)
{
	jvx_connect_server_if* hdl = (jvx_connect_server_if*)privateData;
	jvx_connect_server_family* hdlFamily = (jvx_connect_server_family*)hdl->priv;
	oneServerInterface* theIf = &hdlFamily->theIfs[hdl->id];
	assert(hdlFamily->linkToClient.uselink);
	return(theIf->callbacks.handlerStruct->common.callback_report_startup_complete(theIf->callbacks.privData, ifidx));
}

jvxErrorType jvx_server_local_shutdown_complete(jvxHandle* privateData, jvxSize ifidx)
{
	jvx_connect_server_if* hdl = (jvx_connect_server_if*)privateData;
	jvx_connect_server_family* hdlFamily = (jvx_connect_server_family*)hdl->priv;
	oneServerInterface* theIf = &hdlFamily->theIfs[hdl->id];
	assert(hdlFamily->linkToClient.uselink);
	return(theIf->callbacks.handlerStruct->common.callback_report_shutdown_complete(theIf->callbacks.privData, ifidx));
}

jvxErrorType jvx_server_local_provide_data_and_length(jvxHandle* privateData, jvxSize channelId, char** fld, jvxSize* sz, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvx_connect_server_if* hdl = (jvx_connect_server_if*)privateData;
	jvx_connect_server_family* hdlFamily = (jvx_connect_server_family*)hdl->priv;
	oneServerInterface* theIf = &hdlFamily->theIfs[hdl->id];
	assert(hdlFamily->linkToClient.uselink);
	return(theIf->callbacks.handlerStruct->common.callback_provide_data_and_length(theIf->callbacks.privData, channelId, fld, sz, offset, idIf, additionalInfo, whatsthis));
}

jvxErrorType jvx_server_local_report_data_and_read(jvxHandle* privateData, jvxSize channelId, char* fld, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvx_connect_server_if* hdl = (jvx_connect_server_if*)privateData;
	jvx_connect_server_family* hdlFamily = (jvx_connect_server_family*)hdl->priv;
	oneServerInterface* theIf = &hdlFamily->theIfs[hdl->id];
	assert(hdlFamily->linkToClient.uselink);
	return(theIf->callbacks.handlerStruct->common.callback_report_data_and_read(theIf->callbacks.privData, channelId, fld, sz, offset, idIf, additionalInfo, whatsthis));
}

jvxErrorType jvx_server_local_report_error(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx)
{
	jvx_connect_server_if* hdl = (jvx_connect_server_if*)privateData;
	jvx_connect_server_family* hdlFamily = (jvx_connect_server_family*)hdl->priv;
	oneServerInterface* theIf = &hdlFamily->theIfs[hdl->id];
	assert(hdlFamily->linkToClient.uselink);
	return(theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, errCode, errDescription, ifidx));
}

jvxErrorType jvx_server_local_report_specific(jvxHandle* privateData, jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx)
{
	jvx_connect_server_if* hdl = (jvx_connect_server_if*)privateData;
	jvx_connect_server_family* hdlFamily = (jvx_connect_server_family*)hdl->priv;
	oneServerInterface* theIf = &hdlFamily->theIfs[hdl->id];
	assert(hdlFamily->linkToClient.uselink);
	return(theIf->callbacks.handlerStruct->common.callback_report_specific(hdlFamily->linkToClient.priv, idWhat, fld, szFld, ifidx));
}

// ========================================================================================
// Some STATIC helper functions
static void
init_one_connection(oneServerConnection* theCon, jvxSize idx)
{
	theCon->theSocket = JVX_INVALID_SOCKET_HANDLE;
	theCon->idConnection = (jvxInt32)idx;
	theCon->inuse = false;
	//JVX_INITIALIZE_RW_MUTEX(theCon->safeAccessConnection);

#ifdef JVX_WITH_PCAP
	theCon->thePcapSocket = NULL;

#ifndef JVX_PCAP_DIRECT_SEND
	theCon->my_send_queue = NULL;
#endif
#endif

}

static void
terminate_one_connection(oneServerConnection* theCon)
{
	//JVX_TERMINATE_RW_MUTEX(theCon->safeAccessConnection);
	theCon->theSocket = JVX_INVALID_SOCKET_HANDLE;
	theCon->idConnection = -1;
	theCon->inuse = false;

#ifdef JVX_WITH_PCAP
	theCon->thePcapSocket = NULL;

#ifndef JVX_PCAP_DIRECT_SEND
	theCon->my_send_queue = NULL;
#endif
#endif
}

static void
init_server_handle(jvx_connect_server_family* theHandle, jvx_connect_socket_type socket_tp, jvxHandle* config_specific, jvxConnectionPrivateTypeEnum whatsthis)
{
	
#ifdef OS_SUPPORTS_BTH
	std::vector<std::string> namesBluetoothDevs;
#endif

	theHandle->config.socket_type = socket_tp;

	oneServerInterface theNewInterface;

	theNewInterface.config.numAllConnections = 0;
	theNewInterface.config.socket_type= socket_tp;
	theNewInterface.config.name = "Unknown";
	theNewInterface.config.description = "Unknown";

	// Default configurations
	theNewInterface.config.modeSpecific.forTcp.disableNagleAlgorithmTcp = 0;
	theNewInterface.config.modeSpecific.forTcp.connect.family = JVX_IP_V4;
	theNewInterface.config.modeSpecific.forTcp.connect.connectToPort = 12345;
	theNewInterface.config.modeSpecific.forTcp.connect.localPort = 12345;
	theNewInterface.config.modeSpecific.forTcp.connect.targetName = "localhost";

	/*
	 * We come here only for TCP and BTH 
	 *
	theNewInterface.config.modeSpecific.forUdp.family = JVX_IP_V4;
	theNewInterface.config.modeSpecific.forUdp.connectToPort = 12345;
	theNewInterface.config.modeSpecific.forUdp.localPort = 12345;
	theNewInterface.config.modeSpecific.forUdp.targetName = "localhost";
	*/
#ifdef JVX_WITH_PCAP
	theNewInterface.config.modeSpecific.forPcap.timeoutConnect_msec = 1000;
	memset(theNewInterface.config.modeSpecific.forPcap.dest_mac, 0, 6);
	theNewInterface.config.modeSpecific.forPcap.receive_mode = 0;
	theNewInterface.config.modeSpecific.forPcap.send_mode = 0;
#endif

#ifdef OS_SUPPORTS_BTH
	theHandle->config.modeSpecific.forBth.guidPtr = &g_guidServiceClass;
#endif
	theNewInterface.runtime.spaceForAllConnections = NULL;
	theNewInterface.config.numAllConnections = 0;
	theNewInterface.runtime.numAllConnections_inuse = 0;
	theNewInterface.runtime.theSocket = JVX_INVALID_SOCKET_HANDLE;

	theNewInterface.runtime.theState = JVX_CONNECT_STATE_NONE;

	theNewInterface.callbacks.handlerStruct = NULL;
	theNewInterface.callbacks.privData = NULL;

	theNewInterface.thisisme.priv = theHandle;

#ifdef JVX_OS_WINDOWS
	theNewInterface.thread.threadHdl = INVALID_HANDLE_VALUE;
#endif
	theNewInterface.thread.threadId = JVX_NULLTHREAD;

	switch (socket_tp)
	{
	case JVX_CONNECT_SOCKET_TYPE_UDP:
		assert(0);// <- UDP server is handled as a client
		break;

	case JVX_CONNECT_SOCKET_TYPE_TCP:

		theNewInterface.config.name = "TCP Server";
		if (config_specific)
		{
			assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_TCP);
			theNewInterface.config.modeSpecific.forTcp = *((jvx_connect_spec_tcp*)config_specific);
		}
		theNewInterface.config.description = "The Single Interface";		
		theNewInterface.thisisme.id = 0;
		JVX_INITIALIZE_RW_MUTEX(theNewInterface.runtime.safeAccess);

		theHandle->theIfs.push_back(theNewInterface);
		break;

#if defined(OS_SUPPORTS_BTH)
	case JVX_CONNECT_CLIENT_SOCKET_TYPE_BTH:
		// Get the names of all paired bluetooth devices for display
		listNamesBth(namesBluetoothDevs);
		for (i = 0; i < namesBluetoothDevs.size(); i++)
		{
			theNewInterface.config.description = namesBluetoothDevs[i];
	
			theNewInterface.runtime.spaceForAllConnections = new oneConnection[theNewInterface.config.numAllConnections];
			for (i = 0; i < theNewInterface.config.numAllConnections; i++)
			{
				init_one_connection(&theNewInterface.runtime.spaceForAllConnections[i], i);
			}

			theNewInterface.thisisme.id = i;
			JVX_INITIALIZE_RW_MUTEX(theNewInterface.runtime.safeAccess);
			theHandle->theIfs.push_back(theNewInterface);
		}
		break;
#endif

	default:
		assert(0);
	}
}

static void jvx_connect_loop_tcp_poll(jvx_connect_server_if* theHandleId)
{
	int clilen;
	std::string ipaddr;
	std::string errorDescription;
	jvx_socket_connection_tcp_private  localPrivate;
	int max_sd = 0;
	jvxSize i,j;
	int activity = 0;
	char oneChar;
	jvxSize idx = theHandleId->id;
	jvx_connect_server_family* theHandle = theHandleId->priv;
	jvxErrorType resL = JVX_NO_ERROR;
	oneServerInterface * theIf = NULL;
	sockaddr_in remoteAddrv4;
	sockaddr_in6 remoteAddrv6;
	jvxHandle* ptrRemoteAddr = NULL;
	jvxSize szRemoteAddr = 0;
	int myPort = 0;
	if (idx < theHandle->theIfs.size())
	{
		theIf = &theHandle->theIfs[idx];
	}
	else
	{
		assert(0);
	}

	switch (theIf->config.modeSpecific.forTcp.connect.family)
	{
	case JVX_IP_V4:
		ptrRemoteAddr = &remoteAddrv4;
		szRemoteAddr = sizeof(remoteAddrv4);
		break;
	case JVX_IP_V6:
		ptrRemoteAddr = &remoteAddrv6;
		szRemoteAddr = sizeof(remoteAddrv6);
		break;
	default:
		assert(0);
	}

	JVX_POLL_SOCKET_STRUCT* fdarray = new JVX_POLL_SOCKET_STRUCT[theIf->config.numAllConnections+1];

	while(1)
	{
		int numFd = 1;
		fdarray[0].fd = theIf->runtime.theSocket;
		fdarray[0].events = JVX_SOCKET_POLL_INIT_DEFAULT;
		fdarray[0].revents = 0;

		for(i = 0; i < theIf->config.numAllConnections; i++)
		{
			if(theIf->runtime.spaceForAllConnections[i].inuse)
			{
				if(theIf->runtime.spaceForAllConnections[i].theSocket >= 0)
				{
					fdarray[numFd].fd = theIf->runtime.spaceForAllConnections[i].theSocket;
					fdarray[numFd].events = JVX_SOCKET_POLL_INIT_DEFAULT;
					fdarray[numFd].revents = 0;
					numFd ++;
				}
			}
		}

		// Pre populate all other sockets with a -1
		for(i = numFd; i <= theIf->config.numAllConnections; i++)
		{
			fdarray[i].fd = -1;
			fdarray[i].events = JVX_SOCKET_POLL_INIT_DEFAULT;
			fdarray[i].revents = 0;
		}

		// Check socket in blocking manner
		activity = JVX_POLL_SOCKET_INF(fdarray, numFd);

		if (fdarray[0].revents != 0)
		{
			if (
				(fdarray[0].revents == POLLNVAL) ||
				(fdarray[0].revents == POLLERR) ||
				(fdarray[0].revents == POLLHUP))
			{
				break;
			}
			else
			{
				memset(ptrRemoteAddr, 0, szRemoteAddr);
				clilen = JVX_SIZE_INT(szRemoteAddr);

				// New incoming connection or listening stopped
				JVX_SOCKET newSocket = accept(theIf->runtime.theSocket, (struct sockaddr *) ptrRemoteAddr, (socklen_t *)&clilen);// < we can not put this in mutex to avoid blocking

				if (
					(newSocket == INVALID_SOCKET) ||
					(newSocket == SOCKET_ERROR))
				{
					// At first, catch messages in the context of controlled disconnects
#ifdef JVX_OS_WINDOWS
					int errCode = WSAGetLastError();
					if (errCode == WSAENOTSOCK) // <- Regular "stop socket case
#else
					int errCode = errno;
					if (errCode == EBADF)
#endif
					{		
						errorDescription = "Failed to accept new incoming connection.";
						// Report error
						if (theIf->callbacks.handlerStruct)
						{

							if (theIf->callbacks.handlerStruct->common.callback_report_error)
							{
								theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_CREATE_SOCKET_ACCEPT, errorDescription.c_str(), theIf->thisisme.id);
							}
						}

						break; // end of loop
					}
					else
					{
#ifdef JVX_OS_WINDOWS
						switch (errCode)
						{
						case WSANOTINITIALISED:
							errorDescription = "A successful WSAStartup call must occur before using this function.";
							break;
						case WSAENETDOWN:
							errorDescription = "The network subsystem has failed.";
							break;
						case WSAEFAULT:
							errorDescription = "The addrlen parameter is too small or addr is not a valid part of the user address space.";
							break;
						case WSAEINPROGRESS:
							errorDescription = "A blocking Winsock call is in progress, or the service provider is still processing a callback function.";
							break;
						case WSAEINVAL:
							errorDescription = "The listen function was not invoked prior to this function.";
							break;
						case WSAEMFILE:
							errorDescription = "The queue is nonempty on entry to this function, and there are no descriptors available.";
							break;
						case WSAENOBUFS:
							errorDescription = "No buffer space is available.";
							break;
						case WSAEINTR:
							errorDescription = "No buffer space is available.";
							break;
						case WSAENOTSOCK:
							errorDescription = "The descriptor is not a socket.";
							break;
						case WSAEOPNOTSUPP:
							errorDescription = "The referenced socket is not a type that supports connection-oriented service.";
							break;
						case WSAEWOULDBLOCK:
							errorDescription = "The socket is marked as nonblocking and no connections are present to be accepted.";
							break;
						}
#else
						errorDescription = (std::string)"Was not able to listen on socket, Error: " + strerror(errno);
#endif

						// Report error
						if (theIf->callbacks.handlerStruct)
						{

							if (theIf->callbacks.handlerStruct->common.callback_report_error)
							{
								theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_CREATE_SOCKET_ACCEPT, errorDescription.c_str(), theIf->thisisme.id);
							}
						}
						break;
					}
				}
				else // if(newSocket == INVALID_SOCKET or ERROR)
				{
					// Add new incoming connection to connection list
					jvxSize newId = JVX_SIZE_UNSELECTED;
					jvxSize reportId = JVX_SIZE_UNSELECTED;
					std::string ipaddr = sockAddrRecvToHostName(ptrRemoteAddr, theIf->config.modeSpecific.forTcp.connect.family, myPort);

					// New / removed connections always come within this thread, therefore, no safe access required
					// Valid new socket
					for (i = 0; i < theIf->config.numAllConnections; i++)
					{
						if (!theIf->runtime.spaceForAllConnections[i].inuse)
						{
							newId = i;
							break;
						}
					}
					if (JVX_CHECK_SIZE_SELECTED(newId))
					{
						theIf->runtime.spaceForAllConnections[newId].inuse = true;
						theIf->runtime.spaceForAllConnections[newId].theSocket = newSocket;
						reportId = theIf->runtime.spaceForAllConnections[i].idConnection;
					}

					if (JVX_CHECK_SIZE_SELECTED(reportId))
					{
						if (theIf->callbacks.handlerStruct)
						{
							if (theIf->callbacks.handlerStruct->callback_report_connect_incoming)
							{
								theIf->callbacks.handlerStruct->callback_report_connect_incoming(theIf->callbacks.privData, reportId, ipaddr.c_str(), theIf->thisisme.id);
							}
						}

						// If desired, start connection in "disableNagle" mode
						if (theIf->config.modeSpecific.forTcp.disableNagleAlgorithmTcp)
						{
							int flag = 1;
							int errCode = setsockopt(newSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
							assert(errCode == 0);
						}

						// All sockets setup for select in next round of loop!
					}
					else
					{
						JVX_CLOSE_SOCKET(newSocket);
						if (theIf->callbacks.handlerStruct)
						{
							if (theIf->callbacks.handlerStruct->callback_report_denied_incoming)
							{
								theIf->callbacks.handlerStruct->callback_report_denied_incoming(theIf->callbacks.privData, ipaddr.c_str(), theIf->thisisme.id);
							}
						}
					}
				}// if(newSocket == INVALID_SOCKET)
			} // if(FD_ISSET(theHandle->runtime.theSocket, &readfds))
		}
		// All other sockets
		for(i = 1; i <= theIf->config.numAllConnections; i++)
		{
			jvxSize idUse = JVX_SIZE_UNSELECTED;
			JVX_SOCKET_REVENT revents;
			JVX_SOCKET sock_act = fdarray[i].fd;
			if(sock_act >= 0)
			{
				for(j = 0; j < theIf->config.numAllConnections; j++)
				{
					if(theIf->runtime.spaceForAllConnections[j].theSocket == sock_act)
					{
						if(fdarray[i].revents)
						{
							idUse = j;
							revents = fdarray[i].revents;
							break;
						}
					}
				}

				if(JVX_CHECK_SIZE_SELECTED(idUse))
				{
					if (revents & JVX_POLL_SOCKET_RDNORM)
					{
						char* ptrRead;
						jvxSize maxNumCopy = 0;
						jvxSize szRead = 0;
						jvxSize readNum = 0;
						jvxSize offset = 0;

						localPrivate.idConnection = idUse;
						localPrivate.origin = NULL;

						JVX_SOCKET_NUM_BYTES bytes_available = 0;
						JVX_IOCTRL_SOCKET_FIONREAD(theIf->runtime.spaceForAllConnections[idUse].theSocket, &bytes_available);

						while (bytes_available)
						{
							maxNumCopy = bytes_available;
							ptrRead = NULL;
							offset = 0;

							if (theIf->callbacks.handlerStruct->common.callback_provide_data_and_length)
							{
								resL = theIf->callbacks.handlerStruct->common.callback_provide_data_and_length(theIf->callbacks.privData, theIf->runtime.spaceForAllConnections[idUse].idConnection, &ptrRead, &maxNumCopy, &offset, 
									theIf->thisisme.id, &localPrivate, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONNECTION_TCP);
								if (resL != JVX_NO_ERROR)
								{
									std::cout << "Packet from invalid source channel, skipping!" << std::endl;
								}
							}
							else
							{
								maxNumCopy = 0;
							}

							if (maxNumCopy == 0)
							{
								// if application refuses data, just read out and do not pass data to connected class
								ptrRead = &oneChar;
								maxNumCopy = 1;
								offset = 0;
								while (1)
								{
									readNum = recv(theIf->runtime.spaceForAllConnections[idUse].theSocket, ptrRead, 1, 0);
									if (readNum == 0)
									{
										break;
									}
								}
								bytes_available = 0;
								continue;
							}

							// Regular case: Read in data and pass to connected class
							szRead = maxNumCopy;
							assert(szRead > 0);
							readNum = recv(theIf->runtime.spaceForAllConnections[idUse].theSocket, ptrRead + offset, JVX_SIZE_INT(szRead), 0);
							if (readNum > 0)
							{
								if (theIf->callbacks.handlerStruct->common.callback_report_data_and_read)
								{
									theIf->callbacks.handlerStruct->common.callback_report_data_and_read(theIf->callbacks.privData, theIf->runtime.spaceForAllConnections[idUse].idConnection, ptrRead, 
										readNum, offset, theIf->thisisme.id, &localPrivate, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONNECTION_TCP);
								}
								bytes_available -= (JVX_SOCKET_NUM_BYTES)readNum;
							}
							else
							{
								// This lets this socket become inactive
								theIf->runtime.spaceForAllConnections[idUse].inuse = false;
								if (theIf->callbacks.handlerStruct)
								{
									if (theIf->callbacks.handlerStruct->callback_report_disconnect_incoming)
									{
										theIf->callbacks.handlerStruct->callback_report_disconnect_incoming(theIf->callbacks.privData, theIf->runtime.spaceForAllConnections[idUse].idConnection, theIf->thisisme.id);
									}
								}
								break; // while (bytes_available)
							}
						}
					}
					else if(
						(revents & JVX_POLL_SOCKET_ERR) ||
						(revents & JVX_POLL_SOCKET_HUP))
					{
						theIf->runtime.spaceForAllConnections[idUse].inuse = false;

						if(theIf->callbacks.handlerStruct)
						{
							if(theIf->callbacks.handlerStruct->callback_report_disconnect_incoming)
							{
								theIf->callbacks.handlerStruct->callback_report_disconnect_incoming(theIf->callbacks.privData, theIf->runtime.spaceForAllConnections[idUse].idConnection, theIf->thisisme.id);
							}
						}
						JVX_CLOSE_SOCKET(theIf->runtime.spaceForAllConnections[idUse].theSocket);
						theIf->runtime.spaceForAllConnections[idUse].theSocket = JVX_INVALID_SOCKET_HANDLE;
					}
					else if(revents & JVX_POLL_SOCKET_NVAL)
					{
						theIf->runtime.spaceForAllConnections[idUse].inuse = false;

						if(theIf->callbacks.handlerStruct)
						{
							if(theIf->callbacks.handlerStruct->callback_report_disconnect_incoming)
							{
								theIf->callbacks.handlerStruct->callback_report_disconnect_incoming(theIf->callbacks.privData, theIf->runtime.spaceForAllConnections[idUse].idConnection, theIf->thisisme.id);
							}
						}
						JVX_CLOSE_SOCKET(theIf->runtime.spaceForAllConnections[idUse].theSocket);
						theIf->runtime.spaceForAllConnections[idUse].theSocket = JVX_INVALID_SOCKET_HANDLE;
					}
					else
					{
						assert(0);
					}
				} // if(idUse >= 0)
			} // if(sock_act >= 0)
		}	// for(i = 1; i <= theHandle->config.numAllConnections; i++)
	} // while(1)

	// Switch state to STARTED in parallel to client behavior
	JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
	theIf->runtime.theState = JVX_CONNECT_STATE_STARTED;
	JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);

	delete[](fdarray);
}

JVX_THREAD_ENTRY_FUNCTION(jvx_connect_server_thread_report, lp)
{
	std::string errorDescription;
	jvx_connect_server_if* theHandleId = (jvx_connect_server_if*)lp;
	jvx_connect_server_family* theHandle = theHandleId->priv;
	jvxSize idx = theHandleId->id;

	oneServerInterface * theIf = NULL;
	if (idx < theHandle->theIfs.size())
	{
		theIf = &theHandle->theIfs[idx];
	}
	else
	{
		assert(0);
	}
#ifdef JVX_WITH_PCAP
	JVX_SLEEP_MS(theIf->config.modeSpecific.forPcap.timeoutConnect_msec);
#endif
	// Report successful connection -- but wait before reporting
	if (theIf->callbacks.handlerStruct)
	{

		if (theIf->callbacks.handlerStruct->common.callback_report_startup_complete)
		{
			theIf->callbacks.handlerStruct->common.callback_report_startup_complete(theIf->callbacks.privData, theIf->thisisme.id);
		}
	}
	return(0);
}

JVX_THREAD_ENTRY_FUNCTION(jvx_connect_server_thread_entry, lp)
{
	std::string errorDescription;
	jvx_connect_server_if* theHandleId = (jvx_connect_server_if*)lp;
	jvx_connect_server_family* theHandle = theHandleId->priv;
	jvxSize idx = theHandleId->id;
	struct sockaddr_in my_addr_v4;
	struct sockaddr_in6 my_addr_v6;

	oneServerInterface * theIf = NULL;
	if (idx < theHandle->theIfs.size())
	{
		theIf = &theHandle->theIfs[idx];
	}
	else
	{
		assert(0);
	}
	bool errorDetected = false;
	jvxBool waitForConnectionThread = false;
	int errCode = 0;
	int flag = 0;
	jvxBool reportDisconnect = false;
#ifdef JVX_OS_WINDOWS
	char reuseaddress = 0;
#else
	int reuseaddress = 0;
#endif
	// Init the socket

	int family = 0;

	assert(theIf->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP);

	switch (theHandle->config.socket_type)
	{
	case JVX_CONNECT_SOCKET_TYPE_TCP:
		switch (theIf->config.modeSpecific.forTcp.connect.family)
		{
		case JVX_IP_V4:
			family = AF_INET;
			break;
		case JVX_IP_V6:
			family = AF_INET6;
			break;
		case JVX_IP_UNIX:
			family = AF_UNIX;
			break;
		default:
			assert(0);
		}
		break;
	case JVX_CONNECT_SOCKET_TYPE_UDP:
		switch (theIf->config.modeSpecific.forUdp.family)
		{
		case JVX_IP_V4:
			family = AF_INET;
			break;
		case JVX_IP_V6:
			family = AF_INET6;
			break;
		default:
			assert(0);
		}
		break;
	}

	// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
	// Create socket
	// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
	switch(theIf->config.socket_type)
	{
	case JVX_CONNECT_SOCKET_TYPE_TCP:
		theIf->runtime.theSocket = socket(family, SOCK_STREAM, IPPROTO_TCP);//IPPROTO_TCP
		break;
	case JVX_CONNECT_SOCKET_TYPE_UDP:
#ifdef JVX_WITH_PCAP
	case JVX_CONNECT_SOCKET_TYPE_PCAP:
		assert(0);
		break;
#endif // JVX_WITH_PCAP
#ifdef OS_SUPPORTS_BTH
	case JVX_CONNECT_SOCKET_TYPE_BTH:
		theIf->runtime.theSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		break;
#endif
	default:
		assert(0);
	}

	if (theIf->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP)
	{
		if ((theIf->runtime.theSocket == INVALID_SOCKET) ||
			(theIf->runtime.theSocket == SOCKET_ERROR))
		{
			errorDetected = true;
#ifdef JVX_OS_WINDOWS
		        errCode = WSAGetLastError();
			switch (errCode)
			{
			case WSANOTINITIALISED:
				errorDescription = "A successful WSAStartup call must occur before using this function.";
				break;
			case WSAENETDOWN:
				errorDescription = "The network subsystem or the associated service provider has failed.";
				break;
			case WSAEAFNOSUPPORT:
				errorDescription = "The specified address family is not supported. For example, an application tried to create a socket for the AF_IRDA address family but an infrared adapter and device driver is not installed on the local computer.";
				break;
			case WSAEINPROGRESS:
				errorDescription = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
				break;
			case WSAEMFILE:
				errorDescription = "No more socket descriptors are available.";
				break;
			case WSAEINVAL:
				errorDescription = "An invalid argument was supplied. This error is returned if the af parameter is set to AF_UNSPEC and the type and currentConfiguration.socketProtocolType parameter are unspecified.";
				break;
			case WSAENOBUFS:
				errorDescription = "No buffer space is available. The socket cannot be created.";
				break;
			case WSAEPROTONOSUPPORT:
				errorDescription = "The specified currentConfiguration.socketProtocolType is not supported.";
				break;
			case WSAEPROTOTYPE:
				errorDescription = "The specified currentConfiguration.socketProtocolType is the wrong type for this socket.";
				break;
			case WSAESOCKTNOSUPPORT:
				errorDescription = "The specified socket type is not supported in this address family.";
				break;
			}

#else
			errorDescription = (std::string)"Was not able to create socket, error: " + strerror(errno);
#endif

			errorDetected = true;
		}
	}

	if(errorDetected)
	{

		// Report error
		if(theIf->callbacks.handlerStruct)
		{
			if(theIf->callbacks.handlerStruct->common.callback_report_error)
			{
				theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_CREATE_SOCKET_FAILED, errorDescription.c_str(), theIf->thisisme.id);
			}
		}
		goto leave_securely;
	}

	// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
	// Set socket options
	// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
	if(theIf->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP)
	{
#ifdef JVX_OS_WINDOWS

		// Set some parameters for the socket
		reuseaddress = 1;

		errCode = setsockopt(theIf->runtime.theSocket, SOL_SOCKET, SO_REUSEADDR, &reuseaddress, sizeof(char));

		if(errCode != 0)
		{
			errCode = WSAGetLastError();
			switch(errCode)
			{
			case WSANOTINITIALISED:
				errorDescription = "A successful WSAStartup call must occur before using this function.";
				break;
			case WSAENETDOWN:
				errorDescription = "The network subsystem has failed.";
				break;
			case WSAEFAULT:
				errorDescription = "The buffer pointed to by the optval parameter is not in a valid part of the process address space or the optlen parameter is too small.";
				break;
			case WSAEINPROGRESS:
				errorDescription = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
				break;
			case WSAEINVAL:
				errorDescription = "The level parameter is not valid, or the information in the buffer pointed to by the optval parameter is not valid.";
				break;
			case WSAENETRESET:
				errorDescription = "The connection has timed out when SO_KEEPALIVE is set.";
				break;
			case WSAENOPROTOOPT:
				errorDescription = "The option is unknown or unsupported for the specified provider or socket (see SO_GROUP_PRIORITY limitations).";
				break;
			case WSAENOTCONN:
				errorDescription = "The connection has been reset when SO_KEEPALIVE is set.";
				break;
			case WSAENOTSOCK:
				errorDescription = "The descriptor is not a socket.";
				break;
			}
			errorDetected = true;
		}
#else

		// TODO: REWRITE THIS FOR MAC/LINUX
		// Set some parameters for the socket
		reuseaddress = 1;
		errCode = setsockopt(theIf->runtime.theSocket, SOL_SOCKET, SO_REUSEADDR, &reuseaddress, sizeof(int));
		if (errCode < 0)
		{
			errorDescription = (std::string)"Was not able to set socket options, Error: " + strerror(errno);
			errorDetected = true;
		}
#endif

		if(errorDetected)
		{
			// Report error
			if(theIf->callbacks.handlerStruct)
			{
				if(theIf->callbacks.handlerStruct->common.callback_report_error)
				{
					theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_CREATE_SOCKET_SETOPT_I, errorDescription.c_str(), theIf->thisisme.id);
				}
			}
			goto leave_securely;
		}
	}

	if(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP)
	{
		if(theIf->config.modeSpecific.forTcp.disableNagleAlgorithmTcp)
		{
#ifdef JVX_OS_WINDOWS

			int flag = 1;
			int errCode = setsockopt(theIf->runtime.theSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
			if(errCode != 0)
			{
				errCode = WSAGetLastError();
				switch(errCode)
				{
				case WSANOTINITIALISED:
					errorDescription = "A successful WSAStartup call must occur before using this function.";
					break;
				case WSAENETDOWN:
					errorDescription = "The network subsystem has failed.";
					break;
				case WSAEFAULT:
					errorDescription = "The buffer pointed to by the optval parameter is not in a valid part of the process address space or the optlen parameter is too small.";
					break;
				case WSAEINPROGRESS:
					errorDescription = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
					break;
				case WSAEINVAL:
					errorDescription = "The level parameter is not valid, or the information in the buffer pointed to by the optval parameter is not valid.";
					break;
				case WSAENETRESET:
					errorDescription = "The connection has timed out when SO_KEEPALIVE is set.";
					break;
				case WSAENOPROTOOPT:
					errorDescription = "The option is unknown or unsupported for the specified provider or socket (see SO_GROUP_PRIORITY limitations).";
					break;
				case WSAENOTCONN:
					errorDescription = "The connection has been reset when SO_KEEPALIVE is set.";
					break;
				case WSAENOTSOCK:
					errorDescription = "The descriptor is not a socket.";
					break;
				}
				errorDetected = true;
			}

#else
			flag = 1;
			errCode = setsockopt(theIf->runtime.theSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));    /* length of option value */
			if (errCode < 0)
			{
				errorDescription = (std::string)"Was not able to set socket options, Error: " + strerror(errno);
				errorDetected = true;
			}


#endif

			if(errorDetected)
			{
				// Report error
				if(theIf->callbacks.handlerStruct)
				{

					if(theIf->callbacks.handlerStruct->common.callback_report_error)
					{
						theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_CREATE_SOCKET_SETOPT_II, errorDescription.c_str(), theIf->thisisme.id);
					}
				}
				goto leave_securely;
			}
		}
	} // if(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP)


	// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
	// Bind
	// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

	if(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP)
	{
		switch (theIf->config.modeSpecific.forTcp.connect.family)
		{
		case JVX_IP_V4:
			memset((char *)&my_addr_v4, 0, sizeof(my_addr_v4));

			// Specify listening port
			my_addr_v4.sin_family = family;
			my_addr_v4.sin_addr.s_addr = htonl(INADDR_ANY);
			my_addr_v4.sin_port = htons(theIf->config.modeSpecific.forTcp.connect.localPort);

			// ==================================================================
			// Bind the new socket
			// ==================================================================
			errCode = bind(theIf->runtime.theSocket, (struct sockaddr *)&my_addr_v4, sizeof(my_addr_v4));
			break;
		case JVX_IP_V6:
			memset((char *)&my_addr_v6, 0, sizeof(my_addr_v6));

			// Specify listening port
			my_addr_v6.sin6_family = family;
			my_addr_v6.sin6_addr = in6addr_any;
			my_addr_v6.sin6_port = htons(theIf->config.modeSpecific.forTcp.connect.localPort);

			// ==================================================================
			// Bind the new socket
			// ==================================================================
			errCode = bind(theIf->runtime.theSocket, (struct sockaddr *)&my_addr_v6, sizeof(my_addr_v6));
			break;
		}		
		
		if(errCode != 0)
		{
#ifdef JVX_OS_WINDOWS
			errCode = WSAGetLastError();
			switch(errCode)
			{
			case WSANOTINITIALISED:
				errorDescription = "Note  A successful WSAStartup call must occur before using this function.";
				break;
			case WSAENETDOWN:
				errorDescription = "The network subsystem has failed.";
				break;
			case WSAEACCES:
				errorDescription = "An attempt to bind a datagram socket to the broadcast address failed because the setsockopt option SO_BROADCAST is not enabled.";
				break;
			case WSAEADDRINUSE:
				errorDescription = "A process on the computer is already bound to the same fully-qualified address and the socket has not been marked to allow address reuse with SO_REUSEADDR. For example, the IP address and port specified in the name parameter are already bound to another socket being used by another application. For more information, see the SO_REUSEADDR socket option in the SOL_SOCKET Socket Options reference, Using SO_REUSEADDR and SO_EXCLUSIVEADDRUSE, and SO_EXCLUSIVEADDRUSE.";
				break;
			case WSAEADDRNOTAVAIL:
				errorDescription = "The specified address pointed to by the name parameter is not a valid local IP address on this computer.";
				break;
			case WSAEFAULT:
				errorDescription = "The name or namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the name parameter contains an incorrect address format for the associated address family, or the first two bytes of the memory block specified by name does not match the address family associated with the socket descriptor s.";
				break;
			case WSAEINPROGRESS:
				errorDescription = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
				break;
			case WSAEINVAL:
				errorDescription = "The socket is already bound to an address.";
				break;
			case WSAENOBUFS:
				errorDescription = "Not enough buffers are available, too many connections.";
				break;
			case WSAENOTSOCK:
				errorDescription = "The descriptor in the s parameter is not a socket.";
				break;
			}
#else
			errorDescription = (std::string)"Was not able to bind socket, Error: " + strerror(errno);
#endif

			if(errorDetected)
			{
				// Report error
				if(theIf->callbacks.handlerStruct)
				{

					if(theIf->callbacks.handlerStruct->common.callback_report_error)
					{
						theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_CREATE_SOCKET_BIND, errorDescription.c_str(), theIf->thisisme.id);
					}
				}
				goto leave_securely;
			}
		}
	}
	else if(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_BTH)
	{
		// Currently not supported!!
		assert(0);
	}

	// Start to listen to incoming connections
	if(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP)
	{
		int errCode = listen(theIf->runtime.theSocket, SOMAXCONN);// <- we can not safe this by a mutex since otherwise we would block everything
		if(errCode != 0)
		{
#ifdef JVX_OS_WINDOWS
			errCode = WSAGetLastError();
			switch(errCode)
			{
			case WSANOTINITIALISED:
				errorDescription = "A successful WSAStartup call must occur before using this function.";
				break;
			case WSAENETDOWN:
				errorDescription = "The network subsystem has failed.";
				break;
			case WSAEADDRINUSE:
				errorDescription = "The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs during execution of the bind function, but could be delayed until this function if the bind was to a partially wildcard address (involving ADDR_ANY) and if a specific address needs to be committed at the time of this function.";
				break;
			case WSAEINPROGRESS:
				errorDescription = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
				break;
			case WSAEINVAL:
				errorDescription = "The socket has not been bound with bind.";
				break;
			case WSAEISCONN:
				errorDescription = "The socket is already connected.";
				break;
			case WSAEMFILE:
				errorDescription = "No more socket descriptors are available.";
				break;
			case WSAENOBUFS:
				errorDescription = "No buffer space is available.";
				break;
			case WSAENOTSOCK:
				errorDescription = "The descriptor is not a socket.";
				break;
			case WSAEOPNOTSUPP:
				errorDescription = "The referenced socket is not of a type that supports the listen operation.";
				break;
			}
#else
			if (errCode < 0)
			{
				errorDescription = (std::string)"Was not able to listen on socket, Error: " + strerror(errno);
			}
#endif		

			// Report error
			if(theIf->callbacks.handlerStruct)
			{

				if(theIf->callbacks.handlerStruct->common.callback_report_error)
				{
					theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_CREATE_SOCKET_LISTEN, errorDescription.c_str(), theIf->thisisme.id);
				}
			}

			goto leave_securely;
		}
	}

	waitForConnectionThread = false;

	JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
	theIf->runtime.theState = JVX_CONNECT_STATE_LISTENING;
	JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);

	if (theIf->callbacks.handlerStruct)
	{

		if (theIf->callbacks.handlerStruct->common.callback_report_startup_complete)
		{
			theIf->callbacks.handlerStruct->common.callback_report_startup_complete(theIf->callbacks.privData, theIf->thisisme.id);
		}
	}

	// ===================================================================================================
	// At this point, endless loop until server is closed...
	// ===================================================================================================
	switch(theHandle->config.socket_type)
	{
	case JVX_CONNECT_SOCKET_TYPE_TCP:
		jvx_connect_loop_tcp_poll(theHandleId);
		break;
	default:
		assert(0);
	}

	if (waitForConnectionThread)
	{
		JVX_WAIT_FOR_THREAD_TERMINATE_MS(theIf->thread_delayedReport.threadHdl, 5000);
	}

leave_securely:

	// Reset socket value
	theIf->runtime.theSocket = JVX_INVALID_SOCKET_HANDLE;

	if (theIf->callbacks.handlerStruct)
	{
		if (theIf->callbacks.handlerStruct->common.callback_report_shutdown_complete)
		{
			theIf->callbacks.handlerStruct->common.callback_report_shutdown_complete(theIf->callbacks.privData, theIf->thisisme.id);
		}
	}
	return(0);
}

// ========================================================================================
// Main API functions
// ========================================================================================

jvxErrorType jvx_connect_server_family_initialize_st(jvx_connect_server_family** hdlFamilyReturn,
	jvx_connect_socket_type socket_type, 
	jvxHandle* config_specific, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxSize i;
	jvxSize num = 0;
	jvxErrorType res = JVX_NO_ERROR;
	if (hdlFamilyReturn)
	{
		jvx_connect_server_family* theHandle = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(theHandle, jvx_connect_server_family);

		theHandle->config.socket_type = socket_type;
		
		theHandle->linkToClient.uselink = false;
		theHandle->linkToClient.fam = NULL;
		theHandle->linkToClient.theIf = NULL;

#ifdef JVX_WITH_PCAP
		if (
			(socket_type == JVX_CONNECT_SOCKET_TYPE_UDP) ||
			(socket_type == JVX_CONNECT_SOCKET_TYPE_PCAP))
#else
		if (socket_type == JVX_CONNECT_SOCKET_TYPE_UDP)
#endif
		{
			theHandle->linkToClient.uselink = true;
			res = jvx_connect_client_family_initialize_st(&theHandle->linkToClient.fam, theHandle->config.socket_type);
			jvx_connect_client_family_number_interfaces_st(theHandle->linkToClient.fam, &num);
			for (i = 0; i < num; i++)
			{
				oneServerInterface theNewInterface;
				theNewInterface.config.numAllConnections = 0;
				theNewInterface.config.socket_type = socket_type;
				theNewInterface.config.name = "Unknown";
				theNewInterface.config.description = "Unknown";
				theNewInterface.config.modeSpecific.forTcp.disableNagleAlgorithmTcp = 0;
				theNewInterface.runtime.spaceForAllConnections = NULL;
				theNewInterface.config.numAllConnections = 0;
				theNewInterface.runtime.numAllConnections_inuse = 0;
				theNewInterface.runtime.theSocket = JVX_INVALID_SOCKET_HANDLE;
				theNewInterface.runtime.theState = JVX_CONNECT_STATE_NONE;
				theNewInterface.callbacks.handlerStruct = NULL;
				theNewInterface.callbacks.privData = NULL;
				theNewInterface.config.description = "This should not be visible!";
				theNewInterface.thisisme.id = i;
				theNewInterface.thisisme.priv = theHandle;

				theNewInterface.config.modeSpecific.forUdp.family = JVX_IP_V4;
				theNewInterface.config.modeSpecific.forUdp.connectToPort = 12345;
				theNewInterface.config.modeSpecific.forUdp.localPort = 12345;
				theNewInterface.config.modeSpecific.forUdp.targetName = "localhost";

				if (config_specific)
				{
					switch (theNewInterface.config.socket_type)
					{
					case JVX_CONNECT_SOCKET_TYPE_UDP:
						assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_UDP);
						theNewInterface.config.modeSpecific.forUdp = *((jvx_connect_spec_udp*)config_specific);
						break;
#ifdef JVX_WITH_PCAP
					case JVX_CONNECT_SOCKET_TYPE_PCAP:
						assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_PCAP);
						theNewInterface.config.modeSpecific.forPcap = *((jvx_connect_spec_pcap*)config_specific);
						break;
#endif
					}
				}
				theHandle->theIfs.push_back(theNewInterface);
			}
		}
		else
		{

			// Some initialization code
			init_server_handle(theHandle, socket_type, config_specific, whatsthis);

		}

		*hdlFamilyReturn = theHandle;

		return(JVX_NO_ERROR);
	}

	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType jvx_connect_server_family_number_interfaces_st(jvx_connect_server_family* hdlFamily, jvxSize* num)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (hdlFamily)
	{
		if (num)
		{

			if (hdlFamily->linkToClient.uselink)
			{
				res = jvx_connect_client_family_number_interfaces_st(hdlFamily->linkToClient.fam, num);
			}
			else
			{
				*num = hdlFamily->theIfs.size();

			}
		}
		return(res);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType jvx_connect_server_family_name_interfaces_st(jvx_connect_server_family* hdlFamily, char* fld, jvxSize szFld, jvxSize idx)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (hdlFamily)
	{

		if (hdlFamily->linkToClient.uselink)
		{
			res = jvx_connect_client_family_name_interfaces_st(hdlFamily->linkToClient.fam, fld, szFld, idx);
		}
		else
		{
			if (idx < hdlFamily->theIfs.size())
			{
				jvxSize minChars = JVX_MIN(szFld, hdlFamily->theIfs[idx].config.name.size());
				memcpy(fld, hdlFamily->theIfs[idx].config.name.c_str(), sizeof(char)*minChars);
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return res;
}

jvxErrorType jvx_connect_server_family_description_interfaces_st(jvx_connect_server_family* hdlFamily, char* fld, jvxSize szFld, jvxSize idx)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (hdlFamily)
	{

		if (hdlFamily->linkToClient.uselink)
		{
			res = jvx_connect_client_family_description_interfaces_st(hdlFamily->linkToClient.fam, fld, szFld, idx);
		}
		else
		{
			
			if (idx < hdlFamily->theIfs.size())
			{
				jvxSize minChars = JVX_MIN(szFld, hdlFamily->theIfs[idx].config.description.size());
				memcpy(fld, hdlFamily->theIfs[idx].config.description.c_str(), sizeof(char)*minChars);
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType jvx_connect_server_family_terminate_st(jvx_connect_server_family* hdlFamily)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	if (hdlFamily)
	{

		if (hdlFamily->linkToClient.uselink)
		{
			res = jvx_connect_client_family_terminate_st(hdlFamily->linkToClient.fam);
			hdlFamily->linkToClient.fam = NULL;
			hdlFamily->linkToClient.theIf = NULL;
			hdlFamily->linkToClient.uselink = false;
		}
		else
		{
			for (i = 0; i < hdlFamily->theIfs.size(); i++)
			{
				jvx_connect_server_terminate_st(&hdlFamily->theIfs[i].thisisme);

				// Opposite of init_client_handle
				JVX_TERMINATE_RW_MUTEX(hdlFamily->theIfs[i].runtime.safeAccess);
			}
		}
		JVX_DSP_SAFE_DELETE_OBJECT(hdlFamily);

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

// =========================================================================================

jvxErrorType
jvx_connect_server_initialize_st(jvx_connect_server_family* hdlFamily,
	jvx_connect_server_if** hdlServerOnReturn,
	jvx_connect_server_callback_type* report_callback,
	jvxHandle* privData,
	jvxInt32 numConnectionsMax,
	jvxSize idx)
{
	jvxSize i;
	if (hdlFamily && hdlServerOnReturn)
	{
		oneServerInterface * theIf = NULL;
		if (idx < hdlFamily->theIfs.size())
		{
			theIf = &hdlFamily->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ID_OUT_OF_BOUNDS);
		}

		*hdlServerOnReturn = &theIf->thisisme;

		if (hdlFamily->linkToClient.uselink)
		{
			theIf->callbacks.handlerStruct = report_callback;
			theIf->callbacks.privData = privData;

			hdlFamily->linkToClient.callbacks.common.callback_provide_data_and_length = jvx_server_local_provide_data_and_length;
			hdlFamily->linkToClient.callbacks.common.callback_report_data_and_read = jvx_server_local_report_data_and_read;
			hdlFamily->linkToClient.callbacks.common.callback_report_error = jvx_server_local_report_error;
			hdlFamily->linkToClient.callbacks.common.callback_report_shutdown_complete = jvx_server_local_shutdown_complete;
			hdlFamily->linkToClient.callbacks.common.callback_report_specific = jvx_server_local_report_specific;
			hdlFamily->linkToClient.callbacks.common.callback_report_startup_complete = jvx_server_local_startup_complete;
			hdlFamily->linkToClient.callbacks.callback_report_connect_outgoing = jvx_server_local_connected_outgoing;
			hdlFamily->linkToClient.callbacks.callback_report_unsuccesful_outgoing = jvx_server_local_unsuccessful_outgoing;
			hdlFamily->linkToClient.callbacks.callback_report_disconnect_outgoing = jvx_server_local_disconnect_outgoing;

			return(jvx_connect_client_interface_initialize_st(hdlFamily->linkToClient.fam, &hdlFamily->linkToClient.theIf,
				&hdlFamily->linkToClient.callbacks, *hdlServerOnReturn, 0, idx));
		}
		else
		{
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			if (theIf->runtime.theState == JVX_CONNECT_STATE_NONE)
			{
				theIf->config.numAllConnections = numConnectionsMax;


				theIf->runtime.numAllConnections_inuse = 0;

				theIf->runtime.spaceForAllConnections = NULL;
				JVX_DSP_SAFE_ALLOCATE_FIELD(theIf->runtime.spaceForAllConnections, oneServerConnection, theIf->config.numAllConnections);
				for (i = 0; i < theIf->config.numAllConnections; i++)
				{
					init_one_connection(&theIf->runtime.spaceForAllConnections[i], i);
				}
				theIf->callbacks.handlerStruct = report_callback;
				theIf->callbacks.privData = privData;

				theIf->runtime.theState = JVX_CONNECT_STATE_INIT;
			}
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);


		}
		return(JVX_NO_ERROR);
	}

	return(JVX_ERROR_INVALID_ARGUMENT);
}





jvxErrorType
jvx_connect_server_terminate_st(jvx_connect_server_if* hdlServer)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			res = jvx_connect_client_interface_terminate_st(theHandle->linkToClient.theIf);

			theIf->callbacks.handlerStruct = NULL;
			theIf->callbacks.privData = NULL;

			// Redirect all callbacks
			theHandle->linkToClient.callbacks.common.callback_provide_data_and_length = NULL;
			theHandle->linkToClient.callbacks.common.callback_report_data_and_read = NULL;
			theHandle->linkToClient.callbacks.common.callback_report_error = NULL;
			theHandle->linkToClient.callbacks.common.callback_report_shutdown_complete = NULL;
			theHandle->linkToClient.callbacks.common.callback_report_specific = NULL;
			theHandle->linkToClient.callbacks.common.callback_report_startup_complete = NULL;
			theHandle->linkToClient.callbacks.callback_report_connect_outgoing = NULL;
			theHandle->linkToClient.callbacks.callback_report_unsuccesful_outgoing = NULL;
			theHandle->linkToClient.callbacks.callback_report_disconnect_outgoing = NULL;
		}
		else
		{
			// No critical section required as the state init means that no parallel thread is running
			if (theIf->runtime.theState == JVX_CONNECT_STATE_INIT)
			{
				theIf->config.numAllConnections = 0;
				theIf->runtime.numAllConnections_inuse = 0;

				JVX_DSP_SAFE_DELETE_FIELD(theIf->runtime.spaceForAllConnections);
				theIf->runtime.spaceForAllConnections = NULL;
				theIf->callbacks.handlerStruct = NULL;
				theIf->callbacks.privData = NULL;

				theIf->runtime.theState = JVX_CONNECT_STATE_NONE;
			}
			else
			{
				res = JVX_ERROR_WRONG_STATE;
			}
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}




jvxErrorType
jvx_connect_server_start_listening_st(jvx_connect_server_if* hdlServer,
								   jvxHandle* config_specific, jvxConnectionPrivateTypeEnum whatsthis)
{
	if(hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_interface_connect_st(theHandle->linkToClient.theIf, config_specific, whatsthis));
		}
		else
		{

			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			if (theIf->runtime.theState == JVX_CONNECT_STATE_INIT)
			{
				// Enable mode specific configuration
				if (config_specific)
				{
					switch (theIf->config.socket_type)
					{
					case JVX_CONNECT_SOCKET_TYPE_TCP:
						assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_TCP);
						theIf->config.modeSpecific.forTcp = *((jvx_connect_spec_tcp*)config_specific);
						break;
					case JVX_CONNECT_SOCKET_TYPE_UDP:
						assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_UDP);
						theIf->config.modeSpecific.forUdp = *((jvx_connect_spec_udp*)config_specific);
						break;
#ifdef JVX_WITH_PCAP
					case JVX_CONNECT_SOCKET_TYPE_PCAP:
						assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_PCAP);
						theIf->config.modeSpecific.forPcap = *((jvx_connect_spec_pcap*)config_specific);
						break;
#endif
#ifdef OS_SUPPORTS_BTH
					case JVX_CONNECT_SOCKET_TYPE_BTH:
						memcpy(&theIf->config.modeSpecific.forBth, config_specific,
							sizeof(jvx_connect_server_spec_bth));
						break;
#endif
					}
				}

#ifdef JVX_OS_WINDOWS

				if (
					(theIf->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP) ||
					(theIf->config.socket_type == JVX_CONNECT_SOCKET_TYPE_UDP))
				{
					/* initialize TCP for Windows ("winsock") */
					/* this is not necessary for GNU systems  */
					short wVersionRequested;
					WSADATA wsaData;
					wVersionRequested = MAKEWORD(2, 2);
					int res = WSAStartup(wVersionRequested, &wsaData);
					assert(res == 0);
				}
#endif

				theIf->runtime.theState = JVX_CONNECT_STATE_STARTED;

				// Start socket thread
				JVX_CREATE_THREAD(theIf->thread.threadHdl, jvx_connect_server_thread_entry, hdlServer,
					theIf->thread.threadId);

			}
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);

		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_connect_server_whoami_st(jvx_connect_server_if* hdlServer, char* whoami_name, jvxSize szText_name, jvx_connect_descriptor_get whattoget)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string errorDescription;
	std::string ip;
	int errCode;
	if (hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_whoami_st(theHandle->linkToClient.theIf, whoami_name, szText_name, whattoget));
		}
		else
		{

			res = JVX_ERROR_WRONG_STATE;
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			if (theIf->runtime.theState >= JVX_CONNECT_STATE_INIT)
			{
				res = JVX_NO_ERROR;
				if (whoami_name)
				{
					// Get the name of this computer
					assert(theIf->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP);

					switch(whattoget)
					{
					case JVX_CONNECT_SOCKET_NAME_MY_NAME:
						errCode = gethostname(whoami_name, JVX_SIZE_INT(szText_name));
						if (errCode == SOCKET_ERROR)
						{
#ifdef JVX_OS_WINDOWS
							errCode = WSAGetLastError();
							switch (errCode)
							{
							case WSAEFAULT:
								errorDescription = "The name parameter is not a valid part of the user address space, or the buffer size specified by the namelen parameter is too small to hold the complete host name.";
								break;
							case WSANOTINITIALISED:
								errorDescription = "A successful WSAStartup call must occur before using this function.";
								break;
							case WSAENETDOWN:
								errorDescription = "The network subsystem has failed.";
								break;
							case WSAEINPROGRESS:
								errorDescription = "A blocking Winsock call is in progress, or the service provider is still processing a callback function.";
								break;
							}
#else
							errorDescription = (std::string)"Was not able to get server by hostname, error: " + strerror(errno);

#endif
							jvx_fillCharStr(whoami_name, szText_name, errorDescription);
							res = JVX_ERROR_INTERNAL;
						}
						break;
					case  JVX_CONNECT_SOCKET_NAME_ALL_IP_ADDR:

						ip = myNameToIpAddresses(theIf->config.modeSpecific.forTcp.connect.family);
						jvx_fillCharStr(whoami_name, szText_name, ip);
						break;

					case  JVX_CONNECT_SOCKET_NAME_ONE_IP_ADDR:
						ip = myNameToFirstIpAddress(theIf->config.modeSpecific.forTcp.connect.family);
						jvx_fillCharStr(whoami_name, szText_name, ip);
						break;
					default:
						assert(0);
					}
				}
			}
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		}
	}
	return(res);
}

jvxErrorType
jvx_connect_server_identify_st(jvx_connect_server_if* hdlServer, char* thatis, jvxSize szText, const char* whoisthat)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string errorDescription;
	jvxSize sz = 0;
	struct hostent* origHost = NULL;
	struct hostent *he = NULL;
	std::string ip;

	if (hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_identify_st(theHandle->linkToClient.theIf, thatis, szText,  whoisthat));
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			if (theIf->runtime.theState >= JVX_CONNECT_STATE_INIT)
			{
				res = JVX_NO_ERROR;
				if (thatis)
				{
					// Get the name of this computer
					assert(theIf->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP);

					ip = ipAddressToHostName(whoisthat, theIf->config.modeSpecific.forTcp.connect.connectToPort, theIf->config.modeSpecific.forTcp.connect.family);
					jvx_fillCharStr(thatis, szText, ip);
				}
			}
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		}
		return(res);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_connect_server_stop_one_connection_st(jvx_connect_server_if* hdlServer, jvxSize idConnection)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if(hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_interface_terminate_st(theHandle->linkToClient.theIf));
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			if (theIf->runtime.theState >= JVX_CONNECT_STATE_STARTED)
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
				if (idConnection < theIf->config.numAllConnections)
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
					if (theIf->runtime.spaceForAllConnections[idConnection].inuse)
					{
						JVX_SHUTDOWN_SOCKET(theIf->runtime.spaceForAllConnections[idConnection].theSocket, JVX_SOCKET_SD_BOTH);
						JVX_CLOSE_SOCKET(theIf->runtime.spaceForAllConnections[idConnection].theSocket);
						res = JVX_NO_ERROR;
					}
				}
			}
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);

		}
	}
	return(res);
}

jvxErrorType
jvx_connect_server_prepare_udp_target_st(jvx_connect_server_if* hdlServer, const char* server, int port, jvx_udp_target* theTarget, jvxConnectionIpType ipProt)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_prepare_udp_target_st(theHandle->linkToClient.theIf, server, port, theTarget, ipProt));
		}
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_connect_server_field_constraint_st(jvx_connect_server_if* hdlServer, jvxSize* bytesPrepend, jvxSize* fldMinSize, jvxSize* fldMaxSize)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;

	if(hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_field_constraint_st(theHandle->linkToClient.theIf, bytesPrepend, fldMinSize, fldMaxSize));
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			if (theIf->runtime.theState >= JVX_CONNECT_STATE_STARTED)
			{
				switch (theHandle->config.socket_type)
				{
				case JVX_CONNECT_SOCKET_TYPE_TCP:
					if (bytesPrepend)
						*bytesPrepend = 0;
					if (fldMinSize)
						*fldMinSize = JVX_SIZE_UNSELECTED;
					if (fldMaxSize)
						*fldMaxSize = JVX_SIZE_UNSELECTED;
					break;
				default:
					assert(0);
				}
				res = JVX_NO_ERROR;
			}
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		}
	}
	return(res);
}

jvxErrorType
jvx_connect_server_set_thread_priority(jvx_connect_server_if* hdlServer, JVX_THREAD_PRIORITY prio)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}
		

		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_set_thread_priority_st(theHandle->linkToClient.theIf, prio));
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			if (theIf->runtime.theState >= JVX_CONNECT_STATE_INIT)
			{
				JVX_SET_THREAD_PRIORITY(theIf->thread.threadHdl, prio);
				res = JVX_NO_ERROR;
			}
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);

		}
		return(res);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_connect_server_send_st(jvx_connect_server_if* hdlServer, jvxSize idChannel, char* buf, jvxSize* szFld, jvxHandle* wheretosendto, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;

	if(hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_send_st(theHandle->linkToClient.theIf, buf, szFld, wheretosendto, whatsthis));
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			if (theIf->runtime.theState == JVX_CONNECT_STATE_LISTENING)
			{
				if (
					(JVX_CHECK_SIZE_SELECTED(idChannel)) && (idChannel < theIf->config.numAllConnections))
				{
					if (theIf->runtime.spaceForAllConnections[idChannel].inuse)
					{
						res = JVX_NO_ERROR;
						jvxSize errCode = 0;

						switch (theHandle->config.socket_type)
						{
						case JVX_CONNECT_SOCKET_TYPE_TCP:
							errCode = send(theIf->runtime.spaceForAllConnections[idChannel].theSocket, buf, JVX_SIZE_INT(*szFld), 0);
							break;
						
						default:
							assert(0);
						}
						if (errCode == SOCKET_ERROR)
						{
							res = JVX_ERROR_INTERNAL;
						}
					}
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		}
	}
	return(res);
}

#if (defined JVX_WITH_PCAP) && (defined JVX_OS_WINDOWS)
jvxErrorType
jvx_connect_server_start_queue_st(jvx_connect_server_if* hdlServer, jvxSize idChannel,  jvxSize szFld, jvxSize numFldsQ)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_start_queue_st(theHandle->linkToClient.theIf, szFld, numFldsQ));
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			if (theIf->runtime.theState >= JVX_CONNECT_STATE_STARTED)
			{
				res = JVX_NO_ERROR;
				jvxSize errCode = 0;

				if (idChannel < theIf->config.numAllConnections)
				{
					switch (theHandle->config.socket_type)
					{

					case JVX_CONNECT_SOCKET_TYPE_PCAP:

						//JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.spaceForAllConnections[idChannel].safeAccess);

						if (theIf->runtime.spaceForAllConnections[idChannel].my_send_queue == NULL)
						{
							theIf->runtime.spaceForAllConnections[idChannel].my_send_queue = pcap_sendqueue_alloc(JVX_SIZE_INT((szFld + sizeof(pcap_pkthdr))*numFldsQ));
							assert(theIf->runtime.spaceForAllConnections[idChannel].my_send_queue);
							res = JVX_NO_ERROR;
						}
						else
						{
							res = JVX_ERROR_NOT_READY;
						}
						//JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
						break;

					default:
						res = JVX_ERROR_UNSUPPORTED;
					}
					if (errCode == SOCKET_ERROR)
					{
						res = JVX_ERROR_INTERNAL;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		}
	}
	return(res);
}

jvxErrorType
jvx_connect_server_stop_queue_st(jvx_connect_server_if* hdlServer, jvxSize idChannel)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_stop_queue_st(theHandle->linkToClient.theIf));
		}
		else
		{
			res = JVX_ERROR_UNSUPPORTED;
		}
	}
	return(res);
}

jvxErrorType
jvx_connect_server_collect_st(jvx_connect_server_if* hdlServer, jvxSize idChannel, char* buf, jvxSize* szFld, jvxHandle* wheretosendto, jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_collect_st(theHandle->linkToClient.theIf, buf, szFld, wheretosendto, whatsthis));
		}
		else
		{
			res = JVX_ERROR_UNSUPPORTED;
		}
	}
	return(res);
}

jvxErrorType
jvx_connect_server_transmit_st(jvx_connect_server_if* hdlServer, jvxSize idChannel)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_transmit_st(theHandle->linkToClient.theIf));
		}
		else
		{
			res = JVX_ERROR_UNSUPPORTED;
		}
	}
	return(res);
}
#else

jvxErrorType jvx_connect_server_start_queue_st(jvx_connect_server_if* hdlServer, jvxSize idChannel, jvxSize szFld, jvxSize numFldsQ)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType jvx_connect_server_collect_st(jvx_connect_server_if* hdlServer, jvxSize idChannel, char* buf, jvxSize* szFld)
{
	return JVX_ERROR_UNSUPPORTED;
}


jvxErrorType jvx_connect_server_transmit_st(jvx_connect_server_if* hdlServer, jvxSize idChannel)
{
	return JVX_ERROR_UNSUPPORTED;
}


jvxErrorType jvx_connect_server_stop_queue_st(jvx_connect_server_if* hdlServer, jvxSize idChannel)
{
	return JVX_ERROR_UNSUPPORTED;
}


#endif


 
jvxErrorType
jvx_connect_server_trigger_shutdown_st(jvx_connect_server_if* hdlServer)
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;

	if(hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_trigger_shutdown_st(theHandle->linkToClient.theIf));
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			if (
				(theIf->runtime.theState >= JVX_CONNECT_STATE_STARTED) &&
				(theIf->runtime.theState < JVX_CONNECT_STATE_WAIT_FOR_SHUTDOWN))
			{
				for (i = 0; i < theIf->config.numAllConnections; i++)
				{
					JVX_SHUTDOWN_SOCKET(theIf->runtime.spaceForAllConnections[i].theSocket, JVX_SOCKET_SD_BOTH);
					JVX_CLOSE_SOCKET(theIf->runtime.spaceForAllConnections[i].theSocket);
				}
				theIf->runtime.theState = JVX_CONNECT_STATE_WAIT_FOR_SHUTDOWN;

				JVX_SHUTDOWN_SOCKET(theIf->runtime.theSocket, JVX_SOCKET_SD_BOTH);
				JVX_CLOSE_SOCKET(theIf->runtime.theSocket);
				res = JVX_NO_ERROR;
			}
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		}
	}
	return(res);
}

jvxErrorType
jvx_connect_server_optional_wait_for_shutdown_st(jvx_connect_server_if* hdlServer)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;

	if(hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_optional_wait_for_shutdown_st(theHandle->linkToClient.theIf));
		}
		else
		{
			if (theIf->runtime.theState > JVX_CONNECT_STATE_INIT)
			{
				// Wait for shutdown. MUST BE OUTSIDE MUTEX SECTION!!!!
				JVX_WAIT_FOR_THREAD_TERMINATE_INF(theIf->thread.threadHdl);

				theIf->runtime.theState = JVX_CONNECT_STATE_INIT;

				// After this moment would be the right place to do the cleanup of the send queues
				res = JVX_NO_ERROR;
			}
		}
	}
	return(res);
}

jvxErrorType jvx_connect_server_lock_state_st(jvx_connect_server_if* hdlServer)
{
	if(hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_lock_state_st(theHandle->linkToClient.theIf));
		}
		else
		{
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType jvx_connect_server_unlock_state_st(jvx_connect_server_if* hdlServer)
{
	if(hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_unlock_state_st(theHandle->linkToClient.theIf));
		}
		else
		{
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType jvx_connect_server_get_state_st(jvx_connect_server_if* hdlServer, jvx_connect_state* stateOnReturn)
{
	if(hdlServer)
	{
		jvx_connect_server_family* theHandle = hdlServer->priv;
		jvxSize idx = hdlServer->id;

		oneServerInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}


		if (theHandle->linkToClient.uselink)
		{
			return(jvx_connect_client_get_state_st(theHandle->linkToClient.theIf, stateOnReturn));
		}
		else
		{
		if(stateOnReturn)
		{
			*stateOnReturn = theIf->runtime.theState;
		}
	}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
