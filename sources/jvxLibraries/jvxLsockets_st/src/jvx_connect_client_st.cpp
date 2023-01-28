#ifdef _MSC_VER
extern "C"
{
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2bth.h>
}
#endif

#include <iostream>
#include "jvx_dsp_base.h"
#include "jvx_connect_client_st.h"

// include socket stuff
#include <sys/types.h>

#ifdef OS_SUPPORTS_BTH
#include <initguid.h>
#endif

#include "jvx_connect_common_helpers.h"


// =============================================================================================
// Typedefs of structs
// =============================================================================================

struct jvx_connect_client_family;

struct jvx_connect_client_if
{
	jvx_connect_client_family* priv;
	jvxSize id;
};

typedef struct
{
	struct
	{
		std::string name;
		std::string description;
		jvxSize timeout_ms;

		struct
		{
			jvx_connect_spec_tcp forTcpUdp;
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
#ifdef JVX_WITH_PCAP
		pcap_t* thePcapSocket;

#ifndef JVX_PCAP_DIRECT_SEND
		pcap_send_queue* my_send_queue;
#endif
#endif


		JVX_SOCKET theSocket;
		jvx_connect_state theState;
		JVX_RW_MUTEX_HANDLE safeAccess;
		//struct hostent* targetServer;
		struct sockaddr_in targetServer;
		jvxBool targetServerSet;
#ifdef JVX_WITH_PCAP
		char mac[6];
#endif
		jvxTimeStampData tStamp;

		//struct sockaddr_in udpInServer;
		//jvxBool udpOutSet;
	} runtime;

	struct
	{
		jvx_connect_client_callback_type* handlerStruct;
		jvxHandle* privData;
	} callbacks;
	jvx_connect_client_if thisisme;

} oneClientInterface;

// ==============================================================
// ==============================================================
// ==============================================================

struct jvx_connect_client_family
{
	std::vector<oneClientInterface> theIfs;

	struct
	{
		jvx_connect_socket_type socket_type;
	} config;
};



// ========================================================================================
// Some STATIC helper functions

static int connect_timeout(JVX_SOCKET& sock, const sockaddr * serv_addr, jvxSize sz_serv_addr, jvxSize timeout_ms)
{
	fd_set fdset;
	struct timeval tv;
	unsigned long val = 1;
	int res;
	int retVal = 0;
    jvxInt64 valI64 = 0;

	// Set socket into non blocking mode
#ifdef JVX_OS_WINDOWS
	ioctlsocket(sock, FIONBIO, &val);
#else
	ioctl(sock, FIONBIO, &val);
#endif
	// Connect which will fail
	res = connect(sock, serv_addr, (int)sz_serv_addr);

	// Setup select struct
	FD_ZERO(&fdset);
	FD_SET(sock, &fdset);

	if(JVX_CHECK_SIZE_SELECTED(timeout_ms))
	{
	    valI64 = (jvxInt64)timeout_ms * 1000;
	    tv.tv_sec = (JVX_TIME_SEC)(valI64 / 1000000);
	    tv.tv_usec = (JVX_TIME_USEC) (valI64 % 1000000);

	    res = select((int)sock + 1, NULL, &fdset, NULL, &tv);
	}
	else
	{
	    res = select((int)sock + 1, NULL, &fdset, NULL, NULL);
	}
	if(res == 1)
	{
		// Case for connection successful
		int so_error = 0;
		socklen_t len = sizeof(so_error);

		getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&so_error, &len);
		retVal = so_error;
	}
	else
	{
		// Should be a timout
		retVal = -1;
	}

	if(retVal == 0)
	{
		// Set socket into blocking mode
		val = 0;
#ifdef JVX_OS_WINDOWS
		ioctlsocket(sock, FIONBIO, &val);
#else
		ioctl(sock, FIONBIO, &val);
#endif
	}
	return(retVal);
}

static jvxErrorType 
jvx_connect_client_prepare_udp_target_st__core(const char* server, jvx_udp_target* theTarget, int port, jvxConnectionIpType ipProt)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = hostNameToSockAddr(theTarget, server, ipProt, port);
	return(res);
}

static void
init_client_handle(jvx_connect_client_family* theHandle, jvx_connect_socket_type socket_tp)
{
#ifdef OS_SUPPORTS_BTH
	std::vector<std::string> namesBluetoothDevs;
#endif

#ifdef JVX_WITH_PCAP
	jvxSize i, cnt;
	pcap_if_t *alldevs, *d;
	char errbuf[PCAP_ERRBUF_SIZE];
#endif

	theHandle->config.socket_type= socket_tp;

	oneClientInterface theNewInterface;

	theNewInterface.config.name = "Unknown";
	theNewInterface.config.description = "Unknown";

		// Default configurations
	theNewInterface.config.modeSpecific.forTcpUdp.disableNagleAlgorithmTcp = 0;

#ifdef OS_SUPPORTS_BTH
	theNewInterface.config.modeSpecific.forBth.guidPtr = &g_guidServiceClass;
#endif

	theNewInterface.runtime.theSocket = JVX_INVALID_SOCKET_HANDLE;
	theNewInterface.runtime.theState = JVX_CONNECT_STATE_NONE;

	theNewInterface.callbacks.handlerStruct = NULL;
	theNewInterface.callbacks.privData = NULL;

	memset(&theNewInterface.runtime.targetServer, 0, sizeof(theNewInterface.runtime.targetServer));
	theNewInterface.runtime.targetServerSet = false;


#ifdef JVX_OS_WINDOWS
	theNewInterface.thread.threadHdl = INVALID_HANDLE_VALUE;
#endif
	theNewInterface.thread.threadId = JVX_NULLTHREAD;

	theNewInterface.thisisme.priv = theHandle;

	switch(socket_tp)
	{
	case JVX_CONNECT_SOCKET_TYPE_TCP:
	case JVX_CONNECT_SOCKET_TYPE_UDP:

		if (socket_tp == JVX_CONNECT_SOCKET_TYPE_TCP)
		{
			theNewInterface.config.name = "TCP Client";
		}
		else
		{
			theNewInterface.config.name = "UDP Client";
		}
		theNewInterface.config.description = "The Single Interface";

		theNewInterface.thisisme.id = 0;
		JVX_INITIALIZE_RW_MUTEX(theNewInterface.runtime.safeAccess);

		theHandle->theIfs.push_back(theNewInterface);
		break;

#ifdef JVX_WITH_PCAP
	case JVX_CONNECT_SOCKET_TYPE_PCAP:

	    // retreive the device list from the local machine
	    if (pcap_findalldevs(&alldevs, errbuf) == -1)
		{
	        fprintf(stderr, "Error in pcap_findalldevs_ex: %s\n", errbuf);
			assert(0);
		}

		cnt = 0;
	    // print the list
	    for (d = alldevs; d != NULL; d = d->next)
	    {
			theNewInterface.config.name = d->name;
			get_macs(theNewInterface.config.name, theNewInterface.runtime.mac);
			theNewInterface.config.description = d->name;
			if(d->description)
			{
				theNewInterface.config.description = d->description;
			}
			theNewInterface.config.description += "<";
			for(i = 0; i < 6; i++)
			{
				theNewInterface.config.description += jvx_intx2String((jvxUInt8)theNewInterface.runtime.mac[i]);
				if(i != 5)
				{
					theNewInterface.config.description += ":";
				}
			}
			theNewInterface.config.description += ">";

			theNewInterface.thisisme.id = cnt;
			JVX_INITIALIZE_RW_MUTEX(theNewInterface.runtime.safeAccess);

#ifndef JVX_PCAP_DIRECT_SEND
			theNewInterface.runtime.my_send_queue = NULL;
#endif
			theHandle->theIfs.push_back(theNewInterface);
			cnt++;
		}

		pcap_freealldevs(alldevs);
		break;
#endif

#if defined(OS_SUPPORTS_BTH)
	case JVX_CONNECT_SOCKET_TYPE_BTH:
		// Get the names of all paired bluetooth devices for display
		listNamesBth(namesBluetoothDevs);
		for(i = 0; i < namesBluetoothDevs.size(); i++)
		{
			theNewInterface.config.description = namesBluetoothDevs[i];

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

static void jvx_connect_loop_udp_poll(jvx_connect_client_if* theHandleId)
{
	std::string ipaddr;
	std::string errorDescription;
	//std::string tmpStr  = "Unknown";
	sockaddr_in remoteAddrv4;
	sockaddr_in6 remoteAddrv6;
	//socklen_t remoteAddrLen = sizeof(JVX_SOCKADDR_IN);
	jvxSize buf_length = 0;
	jvxByte* buf_field = NULL;

	JVX_POLL_SOCKET_STRUCT fdarray;
	
	int activity = 0;
	char oneChar;
	jvxInt32 errCode = JVX_ERROR_NO_ERROR;
	jvxSize idx = theHandleId->id;
	jvx_connect_client_family* theHandle = theHandleId->priv;
	jvxBool reportDisconnect = false;
	oneClientInterface * theIf = NULL;

	jvxHandle* ptrRemoteAddr = NULL;
	jvxSize szRemoteAddr = 0;

	if(idx < theHandle->theIfs.size())
	{
		theIf = &theHandle->theIfs[idx];
	}
	else
	{
		assert(0);
	}

	switch (theIf->config.modeSpecific.forTcpUdp.connect.family)
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

	while(1)
	{
		int numFd = 1;
		fdarray.fd = theIf->runtime.theSocket;
		fdarray.events = JVX_SOCKET_POLL_INIT_DEFAULT;
		fdarray.revents = 0;

		// Check socket in blocking manner
		activity = JVX_POLL_SOCKET_INF(&fdarray, numFd);

		if (fdarray.revents & JVX_POLL_SOCKET_RDNORM)
		{
			char* ptrRead = NULL;
			jvxSize maxNumCopy = 0;
			//jvxSize readNum = 0;
			jvxSSize readNum = 0;
			jvxSize offset = 0;
			jvxSize szRead = 0;

			memset(ptrRemoteAddr, 0, szRemoteAddr);
			socklen_t remoteAddrLen = JVX_SIZE_INT(szRemoteAddr);

			JVX_SOCKET_NUM_BYTES bytes_available = 0;
			JVX_IOCTRL_SOCKET_FIONREAD(theIf->runtime.theSocket, &bytes_available);

			while (bytes_available)
			{
				maxNumCopy = bytes_available;
				ptrRead = NULL;
				if (theIf->callbacks.handlerStruct->common.callback_provide_data_and_length)
				{
					theIf->callbacks.handlerStruct->common.callback_provide_data_and_length(theIf->callbacks.privData, 
						0, &ptrRead, &maxNumCopy, &offset, theIf->thisisme.id, NULL, 
						JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
				}
				else
				{
					maxNumCopy = 0;
				}

				if (ptrRead == NULL)
				{
					ptrRead = &oneChar;
					maxNumCopy = 1;
					offset = 0;
				}

				if (maxNumCopy < bytes_available)
				{
					jvxSize offsetLocal = 0;
					jvxSize toRead = bytes_available;

					if (buf_length < bytes_available)
					{
						JVX_DSP_SAFE_DELETE_FIELD(buf_field);
						buf_field = NULL;
						buf_length = bytes_available;
						JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(buf_field, jvxByte, buf_length);
					}
					readNum = recvfrom(theIf->runtime.theSocket, buf_field, (int)bytes_available, 0, (sockaddr*)ptrRemoteAddr, &remoteAddrLen);

					if (theIf->runtime.targetServerSet == false)
					{
						theIf->config.modeSpecific.forTcpUdp.connect.targetName = sockAddrRecvToHostName(ptrRemoteAddr,
							theIf->config.modeSpecific.forTcpUdp.connect.family,
							theIf->config.modeSpecific.forTcpUdp.connect.connectToPort);

						memcpy(&theIf->runtime.targetServer, &ptrRemoteAddr, remoteAddrLen);
						theIf->runtime.targetServerSet = true;
					}

					switch (readNum)
					{
					case SOCKET_ERROR:
#ifdef JVX_OS_WINDOWS
						readNum = WSAGetLastError();
						switch (readNum)
						{
						default:
							errorDescription = (std::string)"Error receiving data from socket";
						}
#else
						errorDescription = (std::string)"Error receiving data from socket";
#endif
						// Report error
						if (theIf->callbacks.handlerStruct)
						{

							if (theIf->callbacks.handlerStruct->common.callback_report_error)
							{
								theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_RESOLVE_HOSTNAME, errorDescription.c_str(), theIf->thisisme.id);
							}
						}
						break;
					default:
						
						while (1)
						{
							szRead = JVX_MIN(toRead, maxNumCopy);
							// std::cout << "Pointer Read = " << (int)ptrRead << std::endl;

							memcpy(ptrRead + offset, buf_field + offsetLocal, szRead);
							readNum = szRead;
							// Regular connection receive case
							if (theIf->callbacks.handlerStruct->common.callback_report_data_and_read)
							{
								theIf->callbacks.handlerStruct->common.callback_report_data_and_read(theIf->callbacks.privData, 0, ptrRead, readNum, offset, theIf->thisisme.id, 
									(jvxHandle*)theIf->config.modeSpecific.forTcpUdp.connect.targetName.c_str(), JVX_CONNECT_PRIVATE_ARG_TYPE_INPUT_STRING);
							}
							toRead -= szRead;
							offsetLocal += szRead;
							if (toRead)
							{
								maxNumCopy = toRead;
								ptrRead = NULL;
								if (theIf->callbacks.handlerStruct->common.callback_provide_data_and_length)
								{
									theIf->callbacks.handlerStruct->common.callback_provide_data_and_length(theIf->callbacks.privData,
										0, &ptrRead, &maxNumCopy, &offset, theIf->thisisme.id, NULL,
										JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
								}
								else
								{
									maxNumCopy = 0;
								}

								if (ptrRead == NULL)
								{
									ptrRead = &oneChar;
									maxNumCopy = 1;
									offset = 0;
								}
							}
							else
							{
								break;
							}
						} // while (1)
						bytes_available = 0;

					} // switch (readNum)
				}// if (maxNumCopy < bytes_available)
				else
				{
					szRead = maxNumCopy;
					assert(szRead > 0);
					readNum = recvfrom(theIf->runtime.theSocket, ptrRead + offset, (int)szRead, 0, (sockaddr*)ptrRemoteAddr, &remoteAddrLen);// No way to mutex this

					if (theIf->config.modeSpecific.forTcpUdp.connect.targetName.empty())
					{
						theIf->config.modeSpecific.forTcpUdp.connect.targetName = sockAddrRecvToHostName(ptrRemoteAddr, theIf->config.modeSpecific.forTcpUdp.connect.family,
							theIf->config.modeSpecific.forTcpUdp.connect.connectToPort);

						memcpy(&theIf->runtime.targetServer, ptrRemoteAddr, remoteAddrLen);
						theIf->runtime.targetServerSet = true;
					}

					switch (readNum)
					{
					case SOCKET_ERROR:
						errCode = JVX_ERROR_ID_GENERAL_CONNECTION_ERROR;
#ifdef JVX_OS_WINDOWS
						readNum = WSAGetLastError();
						switch (readNum)
						{
						case WSAECONNRESET:
							errorDescription = (std::string)"Connection Reset";
							errCode = JVX_ERROR_ID_GENERAL_CONNECTION_RESET;
							break;
						default:
							errorDescription = (std::string)"Error receiving data from socket";
						}
#else
						errorDescription = (std::string)"Error receiving data from socket";
#endif

						// Report error
						if (theIf->callbacks.handlerStruct)
						{

							if (theIf->callbacks.handlerStruct->common.callback_report_error)
							{
								theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_RESOLVE_HOSTNAME, errorDescription.c_str(), theIf->thisisme.id);
							}
						}
						bytes_available = 0;
						break;

					default:
						// Regular connection receive case
						if (theIf->callbacks.handlerStruct->common.callback_report_data_and_read)
						{
							theIf->callbacks.handlerStruct->common.callback_report_data_and_read(theIf->callbacks.privData, 0, ptrRead, readNum, offset, theIf->thisisme.id, 
								(jvxHandle*)theIf->config.modeSpecific.forTcpUdp.connect.targetName.c_str(), JVX_CONNECT_PRIVATE_ARG_TYPE_INPUT_STRING);
						}
						bytes_available -= (JVX_SOCKET_NUM_BYTES)readNum;
					}
				}
			} // while (bytes_available)
		} //
		else
		{
			if (
				(fdarray.revents == POLLNVAL) ||
				(fdarray.revents == POLLERR) ||
				(fdarray.revents == POLLHUP))
			{
				// The loop may be over since socket was closed..
				reportDisconnect = true;
			}
			else
			{
				// This was detected to be a timeout
			  std::cout << "POLL EVENT = " << fdarray.revents << std::endl;
				assert(0); // <- what may the reason rto end up here?
			}
		}

		if (reportDisconnect)
		{
			// Set the state back to "started"
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			theIf->runtime.theState = JVX_CONNECT_STATE_STARTED;
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);

			if (theIf->callbacks.handlerStruct)
			{
				if (theIf->callbacks.handlerStruct->callback_report_disconnect_outgoing)
				{
					theIf->callbacks.handlerStruct->callback_report_disconnect_outgoing(theIf->callbacks.privData, theIf->thisisme.id);
				}
			}
			break; // while(1) loop
		}
	} // while(1)

	if (buf_field)
	{
		JVX_DSP_SAFE_DELETE_FIELD(buf_field);
	}
}

#ifdef JVX_WITH_PCAP
static void jvx_connect_loop_pcap(jvx_connect_client_if* theHandleId)
{
	jvxSize toberead = 0;
	jvxByte* ptr = NULL;
	jvxUInt8* verProtocol = NULL;
	struct bpf_program  program;
	std::string errorDescription;
	char oneChar;
	jvxSize offset;
	jvxSize szRead;
	jvxRawSocketHeader* headerConv = NULL;
	//char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr *header;
	const u_char *pkt_data;
	jvxBool acceptJavoxProtocolOnly = true;
	char* ptrRead = NULL;
	jvxSize maxNumCopy = 0;
	jvxSize readNum = 0;
	jvxBool keepOnRunning = true;

	jvxSize idx = theHandleId->id;
	jvx_connect_client_family* theHandle = theHandleId->priv;
	jvxBool reportDisconnect = false;
	jvxByte* macsrc = NULL;

	oneClientInterface * theIf = NULL;
	int resL;
	jvxBool prepareBuffersJvx = false;
	jvxConnectionPrivateTypeEnum privEnum = JVX_CONNECT_PRIVATE_ARG_TYPE_NONE;

	if(idx < theHandle->theIfs.size())
	{
		theIf = &theHandle->theIfs[idx];
	}
	else
	{
		assert(0);
	}

	if (jvx_bitTest(theIf->config.modeSpecific.forPcap.receive_mode, JVX_PCAP_RECEIVE_FILTER_INCOMING_MAC_SHIFT))
	{
		std::string filterExpr = "ether dst " + jvx_macAddressToString((jvxUInt8*)theIf->runtime.mac);
		resL = pcap_compile(
			theIf->runtime.thePcapSocket,
			&program,
			filterExpr.c_str(),
			1,
			0);
		if (resL == 0)
		{
			resL = pcap_setfilter(
				theIf->runtime.thePcapSocket,
				&program);
			if (resL == 0)
			{
				std::cout << "Filter set to " << filterExpr << std::endl;
			}
		}
	}


	if (jvx_bitTest(theIf->config.modeSpecific.forPcap.receive_mode, JVX_PCAP_RECEIVE_FILTER_INCOMING_ONLY_SHIFT))
	{
		resL = pcap_setdirection(theIf->runtime.thePcapSocket, PCAP_D_IN);
		if (resL != 0)
		{
			std::cout << pcap_geterr(theIf->runtime.thePcapSocket) << std::endl;
		}
		assert(resL == 0);
	}
	

	acceptJavoxProtocolOnly = true;
	if (jvx_bitTest(theIf->config.modeSpecific.forPcap.receive_mode, JVX_PCAP_RECEIVE_OUTPUT_RAW_SHIFT))
	{
		acceptJavoxProtocolOnly = false;
	}

	if (jvx_bitTest(theIf->config.modeSpecific.forPcap.receive_mode, JVX_PCAP_RECEIVE_OUTPUT_REPORT_JVX_PACKETS_SHIFT))
	{
		prepareBuffersJvx = true;
		privEnum = JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_PCAP_DST_SRC_MAC;
	}

	while(1)
	{
		int res = pcap_next_ex(theIf->runtime.thePcapSocket, &header, &pkt_data);
		switch(res)
		{
		case 0:
			// Timeout case, do nothing than restart
			break;

		case 1:
			// Packet successfully read
			ptrRead = NULL;
			maxNumCopy = 0;
			readNum = 0;
			assert(header->len == header->caplen);
			assert(header->len >= (sizeof(jvxRawSocketHeader) + sizeof(jvxProtocolFamilyHeader)));
			headerConv = (jvxRawSocketHeader*)pkt_data;
			verProtocol = (jvxUInt8*)&headerConv->proto_ident;

			if (!acceptJavoxProtocolOnly)
			{
				if (theIf->callbacks.handlerStruct->common.callback_report_specific)
				{
					resL = theIf->callbacks.handlerStruct->common.callback_report_specific(theIf->callbacks.privData, JVX_CONNECT_SPECIFIC_SUBTYPE_PCAP_RAW, (jvxHandle*)pkt_data, (jvxSize)header->len, theIf->thisisme.id);
				}
			}
			else
			{
				if ((verProtocol[0] == 'J') && (verProtocol[1] == 'X'))
				{
					toberead = header->len;
					ptr = (jvxByte*)pkt_data;

					// Copy field of incoming mac to take over the dest mac for all future outgoing packets
					memcpy(theIf->config.modeSpecific.forPcap.dest_mac, ((jvxRawSocketHeader*)ptr)->src_mac, 6);

					// Provide some local decoding to fit in high level packet constraints
					if(prepareBuffersJvx)
					{
						macsrc = ptr;

						assert(toberead >= sizeof(jvxRawSocketHeader) + sizeof(jvxProtocolHeader));

						// Skip header information
						ptr += sizeof(jvxRawSocketHeader);
						toberead -= sizeof(jvxRawSocketHeader);

						jvxProtocolHeader* hdr = (jvxProtocolHeader*)ptr;
						assert(toberead >= hdr->paketsize);

						// Skip those additional bytes required to achieve minimum packet size constraint (64)
						toberead = hdr->paketsize;
					}

					while (toberead)
					{
						maxNumCopy = toberead;
						resL = JVX_NO_ERROR;
						if (theIf->callbacks.handlerStruct->common.callback_provide_data_and_length)
						{
							resL = theIf->callbacks.handlerStruct->common.callback_provide_data_and_length(theIf->callbacks.privData, 0, &ptrRead, &maxNumCopy, &offset, theIf->thisisme.id, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
						}
						else
						{
							maxNumCopy = 0;
						}

						if (maxNumCopy == 0)
						{
							ptrRead = &oneChar;
							maxNumCopy = 1;
							offset = 0;
						}

						if (resL == JVX_NO_ERROR)
						{
							szRead = maxNumCopy;
							assert(szRead >= 1);
							readNum = JVX_MIN(toberead, szRead);

							memcpy(ptrRead + offset, ptr, readNum);
							if (theIf->callbacks.handlerStruct->common.callback_report_data_and_read)
							{
								theIf->callbacks.handlerStruct->common.callback_report_data_and_read(theIf->callbacks.privData, 0, ptrRead, readNum, offset, theIf->thisisme.id, macsrc, privEnum);
							}
							ptr += readNum;
							toberead -= readNum;
						}
						else
						{
							// Cancel the packet by not copying it
							toberead = 0;
						}
					}
				}
			}
			break;
		default:

			reportDisconnect = true;
		}

		if (reportDisconnect)
		{
			// Set the state back to "started"
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			theIf->runtime.theState = JVX_CONNECT_STATE_STARTED;
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);

			if (theIf->callbacks.handlerStruct)
			{
				if (theIf->callbacks.handlerStruct->callback_report_disconnect_outgoing)
				{
					theIf->callbacks.handlerStruct->callback_report_disconnect_outgoing(theIf->callbacks.privData, theIf->thisisme.id);
				}
			}

			break; // while(1)
		}
	} // while(1)
}
#endif

static void jvx_connect_loop_tcp_poll(jvx_connect_client_if* theHandleId)
{
//	int clilen;
	std::string ipaddr;
//	struct sockaddr_in cli_addr;
	std::string errorDescription;
	jvxBool reportDisconnect = false;

	int max_sd = 0;
	//int i;
	int activity = 0;
	char oneChar;

	JVX_POLL_SOCKET_STRUCT fdarray;

	jvxSize idx = theHandleId->id;
	jvx_connect_client_family* theHandle = theHandleId->priv;

	oneClientInterface * theIf = NULL;
	if(idx < theHandle->theIfs.size())
	{
		theIf = &theHandle->theIfs[idx];
	}
	else
	{
		assert(0);
	}

	while(1)
	{
		fdarray.events = JVX_SOCKET_POLL_INIT_DEFAULT;
		fdarray.fd = theIf->runtime.theSocket;
		fdarray.revents = 0;

		// Check socket in blocking manner
		activity = JVX_POLL_SOCKET_INF(&fdarray, 1);


		// We do not need any protection: If socket was closed before, we will get the appropriate error code next
		if(fdarray.revents & JVX_POLL_SOCKET_RDNORM)
		{

			char* ptrRead;
			jvxSize maxNumCopy = 0;
			jvxSize szRead = 0;
			jvxSize readNum = 0;
			jvxSize offset = 0;

			JVX_SOCKET_NUM_BYTES bytes_available = 0;
			JVX_IOCTRL_SOCKET_FIONREAD(theIf->runtime.theSocket, &bytes_available);
			
			while (bytes_available)
			{
				maxNumCopy  = bytes_available;
				ptrRead = NULL;
				offset = 0;

				if (theIf->callbacks.handlerStruct->common.callback_provide_data_and_length)
				{
					theIf->callbacks.handlerStruct->common.callback_provide_data_and_length(theIf->callbacks.privData, 0, &ptrRead, &maxNumCopy, &offset, theIf->thisisme.id, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
				}
				else
				{
					maxNumCopy = 0;
				}
				
				if(maxNumCopy == 0)
				{
					ptrRead = &oneChar;
					maxNumCopy = 1;
					offset = 0;
				}

				szRead = maxNumCopy;
				assert(szRead > 0);
				readNum = recv(theIf->runtime.theSocket, ptrRead + offset, JVX_SIZE_INT(szRead), 0);
				if (readNum > 0)
				{
					if (theIf->callbacks.handlerStruct->common.callback_report_data_and_read)
					{
						theIf->callbacks.handlerStruct->common.callback_report_data_and_read(theIf->callbacks.privData, 0, ptrRead, readNum, offset, theIf->thisisme.id, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
					}
					bytes_available -= (JVX_SOCKET_NUM_BYTES)readNum;
				}
				else
				{
					reportDisconnect = true;
					break;
				}
			} // while (bytes_available)
		}
		else
		{
			if(
				(fdarray.revents & JVX_POLL_SOCKET_ERR) ||
				(fdarray.revents & JVX_POLL_SOCKET_HUP) ||
				(fdarray.revents & JVX_POLL_SOCKET_NVAL))
			{
				reportDisconnect = true;
			}
			else
			{
				assert(0);
			}
		}

		if (reportDisconnect)
		{
			// Set the state back to "started"
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
			theIf->runtime.theState = JVX_CONNECT_STATE_STARTED;
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);

			if (theIf->callbacks.handlerStruct)
			{
				if (theIf->callbacks.handlerStruct->callback_report_disconnect_outgoing)
				{
					theIf->callbacks.handlerStruct->callback_report_disconnect_outgoing(theIf->callbacks.privData, theIf->thisisme.id);
				}
			}

			break; // while(1)
		}
	} // while(1)
}

JVX_THREAD_ENTRY_FUNCTION(jvx_connect_client_thread_report, lp)
{
	std::string errorDescription;
	jvx_connect_client_if* theHandleId = (jvx_connect_client_if* )lp;
	jvx_connect_client_family* theHandle = theHandleId->priv;
	jvxSize idx = theHandleId->id;
	
	oneClientInterface * theIf = NULL;
	if(idx < theHandle->theIfs.size())
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
	if(theIf->callbacks.handlerStruct)
	{
		if(theIf->callbacks.handlerStruct->callback_report_connect_outgoing)
		{
			theIf->callbacks.handlerStruct->callback_report_connect_outgoing(theIf->callbacks.privData, theIf->thisisme.id);
		}
	}
	return(0);
}

JVX_THREAD_ENTRY_FUNCTION(jvx_connect_client_thread_entry, lp)
{
	jvxErrorType resL = JVX_NO_ERROR;

#ifdef JVX_WITH_PCAP
	char errbuf[PCAP_ERRBUF_SIZE];
#endif
	std::string errorDescription;
	jvx_connect_client_if* theHandleId = (jvx_connect_client_if* )lp;
	jvx_connect_client_family* theHandle = theHandleId->priv;
	jvxSize idx = theHandleId->id;
	jvxBool waitForConnectionThread = false;
	jvxBool reportDisconnect = false;
	jvxBool requestConnect = false;
	oneClientInterface * theIf = NULL;
	if(idx < theHandle->theIfs.size())
	{
		theIf = &theHandle->theIfs[idx];
	}
	else
	{
		assert(0);
	}

	bool errorDetected = false;
#ifdef OS_SUPPORTS_BTH
	SOCKADDR_BTH RemoteBthAddr = { 0 };
#endif
	struct sockaddr_in my_addr_v4;
	struct sockaddr_in6 my_addr_v6;
	int flag = 0;
	int errCode = 0;
	int family = 0;
#ifdef JVX_WITH_PCAP
	int pcapsflags = PCAP_OPEN_FLAGS_NORMAL;
#endif

	switch (theHandle->config.socket_type)
	{
	case JVX_CONNECT_SOCKET_TYPE_TCP:
	case JVX_CONNECT_SOCKET_TYPE_UDP:
		switch (theIf->config.modeSpecific.forTcpUdp.connect.family)
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


	// Init the socket

	// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
	// Create socket
	// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

	JVX_GET_TICKCOUNT_US_SETREF(&theIf->runtime.tStamp);

	switch(theHandle->config.socket_type)
	{
	case JVX_CONNECT_SOCKET_TYPE_TCP:
		theIf->runtime.theSocket = socket(family, SOCK_STREAM, IPPROTO_TCP);//IPPROTO_TCP
		break;
	case JVX_CONNECT_SOCKET_TYPE_UDP:
		theIf->runtime.theSocket = socket(family, SOCK_DGRAM, IPPROTO_UDP);//IPPROTO_UDP
		break;

#ifdef JVX_WITH_PCAP
	case JVX_CONNECT_SOCKET_TYPE_PCAP:
		if (jvx_bitTest(theIf->config.modeSpecific.forPcap.receive_mode, JVX_PCAP_RECEIVE_FILTER_NO_SELF_CAPTURE_SHIFT))
		{
			pcapsflags = PCAP_OPEN_FLAGS_NO_SELF_CAPTURE;
		}

#ifdef JVX_OS_WINDOWS
		theIf->runtime.thePcapSocket = pcap_open(theIf->config.name.c_str(),
			PCAP_SNAP_LEN /* snaplen */,
			pcapsflags /* flags */,
			PCAP_READ_TIMEOUT,
			NULL,
			errbuf);
#else // JVX_OS_WINDOWS
		theIf->runtime.thePcapSocket = pcap_open_live(theIf->config.name.c_str(),
			PCAP_SNAP_LEN /* snaplen */,
			pcapsflags /* flags */,
			PCAP_READ_TIMEOUT,
			errbuf);
#endif // JVX_OS_WINDOWS

		break;
#endif // JVX_WITH_PCAP

#ifdef OS_SUPPORTS_BTH
	case JVX_CONNECT_SOCKET_TYPE_BTH:
		theHandle->runtime.theSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		break;
#endif
	default:
		assert(0);
	}

	if(
		(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP) ||
		(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_UDP))
	{
		if(
			(theIf->runtime.theSocket == INVALID_SOCKET) ||
			(theIf->runtime.theSocket == SOCKET_ERROR))
		{
			errorDetected = true;

#ifdef JVX_OS_WINDOWS
			int errCode = WSAGetLastError();
			switch(errCode)
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

#else // JVX_OS_WINDOWS
			errorDescription = (std::string)"Was not able to create socket, error: " + strerror(errno);
#endif // JVX_OS_WINDOWS
		}

	}
#ifdef JVX_WITH_PCAP
	else if(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_PCAP)
	{
		if(theIf->runtime.thePcapSocket == NULL)
		{
			errorDetected = true;
			errorDescription = errbuf;
		}
	}
#endif

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

	if(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP)
	{
		if(theIf->config.modeSpecific.forTcpUdp.disableNagleAlgorithmTcp)
		{
#ifdef JVX_OS_WINDOWS

			flag = 1;
			errCode = setsockopt(theIf->runtime.theSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
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
	} // if(theHandle->config.socket_type == JVX_CONNECT_SERVER_SOCKET_TYPE_TCP)


	// Transform name of server into IP connection data
	if(
		(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP) || 
		(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_UDP))
	{
		if (theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP)
		{
			// =================================================================================
			// LOOKUP TARGET SERVER NAME
			// =================================================================================

			resL = hostNameToSockAddr(&theIf->runtime.targetServer, theIf->config.modeSpecific.forTcpUdp.connect.targetName.c_str(), 
				theIf->config.modeSpecific.forTcpUdp.connect.family, theIf->config.modeSpecific.forTcpUdp.connect.connectToPort);

			if (resL != JVX_NO_ERROR)
			{
#ifdef JVX_OS_WINDOWS
				int errCode = WSAGetLastError();
				switch (errCode)
				{
				case WSANOTINITIALISED:
					errorDescription = "A successful WSAStartup call must occur before using this function.";
					break;
				case WSAENETDOWN:
					errorDescription = "The network subsystem has failed.";
					break;
				case WSAHOST_NOT_FOUND:
					errorDescription = "An authoritative answer host was not found.";
					break;
				case WSATRY_AGAIN:
					errorDescription = "A nonauthoritative host was not found, or the server failure.";
					break;
				case WSANO_RECOVERY:
					errorDescription = "A nonrecoverable error occurred.";
					break;
				case WSANO_DATA:
					errorDescription = "A valid name exists, but no data record of the requested type exists.";
					break;
				case WSAEINPROGRESS:
					errorDescription = "A blocking Winsock call is in progress, or the service provider is still processing a callback function.";
					break;
				case WSAEFAULT:
					errorDescription = "The name parameter is not a valid part of the user address space.";
					break;
				case WSAEINTR:
					errorDescription = "The socket was closed.";
					break;
				}
#else
				errorDescription = (std::string)"Was not able to get server by hostname, error: " + strerror(errno);
#endif
				// Report error
				if (theIf->callbacks.handlerStruct)
				{

					if (theIf->callbacks.handlerStruct->common.callback_report_error)
					{
						theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_RESOLVE_HOSTNAME, errorDescription.c_str(), theIf->thisisme.id);
					}
				}

				goto leave_securely;
			} // if (resL != JVX_NO_ERROR)
		}

		 if (theIf->config.modeSpecific.forTcpUdp.connect.localPort > 0)
		 {
			 // Condition for bind: only if a specific port is specified, the port is bind.
			 // The OS will choose a random port for outgoing in that case which in TCP is not a problem.
			 // If we do not do this, there will be a timeout once the connection is closed since 
			 // TCP waits for lingering packets after socket close
			 // Some corrections here: 
			 // - SO_REUSEADDR may help to avoid lingering ports
			 //   (https://stackoverflow.com/questions/15198834/bind-failed-address-already-in-use)
			 // - we may use  getsockname() to find out the OS specified port
			 //   (https://stackoverflow.com/questions/1075399/how-to-bind-to-any-available-port)

			 // Prepare bind
			 switch (theIf->config.modeSpecific.forTcpUdp.connect.family)
			 {
			 case JVX_IP_V4:
				 memset((char *)&my_addr_v4, 0, sizeof(my_addr_v4));

				 // Specify listening port
				 my_addr_v4.sin_family = family;
				 my_addr_v4.sin_addr.s_addr = htonl(INADDR_ANY);
				 my_addr_v4.sin_port = htons(theIf->config.modeSpecific.forTcpUdp.connect.localPort);

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
				 my_addr_v6.sin6_port = htons(theIf->config.modeSpecific.forTcpUdp.connect.localPort);

				 // ==================================================================
				 // Bind the new socket
				 // ==================================================================
				 errCode = bind(theIf->runtime.theSocket, (struct sockaddr *)&my_addr_v6, sizeof(my_addr_v6));
				 break;
			 }

			 if (errCode != 0)
			 {
#ifdef JVX_OS_WINDOWS
				 // Here, res already contains the error code, additional error code -1 for timeout
				// Check that connection was successful
				 errCode = WSAGetLastError();
				 switch (errCode)
				 {
				 case WSANOTINITIALISED:
					 errorDescription = "A successful WSAStartup call must occur before using this function.";;
					 break;
				 case WSAENETDOWN:
					 errorDescription = "The network subsystem has failed.";
					 break;
				 case WSAEADDRINUSE:
					 errorDescription = "The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but it could be delayed until the connect function if the bind was to a partially wildcard address (involving ADDR_ANY) and if a specific address needs to be committed at the time of the connect function.";
					 break;
				 case WSAEINTR:
					 errorDescription = "The socket was closed.";
					 break;
				 case WSAEINPROGRESS:
					 errorDescription = "A blocking Winsock call is in progress, or the service provider is still processing a callback function.";
					 break;
				 case WSAEALREADY:
					 errorDescription = "A nonblocking connect call is in progress on the specified socket.  To preserve backward compatibility, this error is reported as WSAEINVAL to Winsock applications that link to either Winsock.dll or Wsock32.dll.";
					 break;
				 case WSAEADDRNOTAVAIL:
					 errorDescription = "The remote address is not a valid address (such as ADDR_ANY).";
					 break;
				 case WSAEAFNOSUPPORT:
					 errorDescription = "Addresses in the specified family cannot be used with this socket.";
					 break;
				 case WSAEFAULT:
					 errorDescription = "The name or namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family.";
					 break;
				 case WSAEINVAL:
					 errorDescription = "The parameter s is a listening socket.";
					 break;
				 case WSAEISCONN:
					 errorDescription = "The socket is already connected (connection-oriented sockets only).";
					 break;
				 case WSAENETUNREACH:
					 errorDescription = "The network cannot be reached from this host at this time.";
					 break;
				 case WSAENOBUFS:
					 errorDescription = "No buffer space is available. The socket cannot be connected.";
					 break;
				 case WSAEWOULDBLOCK:
					 errorDescription = "The socket is marked as nonblocking and the connection cannot be completed immediately.";
					 break;
				 case WSAEACCES:
					 errorDescription = "An attempt to connect a datagram socket to a broadcast address failed because the setsockopt option SO_BROADCAST is not enabled.";
					 break;
				 default:
					 assert(0); // Try to detect the reason!!
				 }

				 // Report error

#else
				 errorDescription = (std::string)"Was not able to connect to server, error: " + strerror(errno);
#endif

				 // Report error
				 if (theIf->callbacks.handlerStruct)
				 {

					 if (theIf->callbacks.handlerStruct->common.callback_report_error)
					 {
						 theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_RESOLVE_HOSTNAME, errorDescription.c_str(), theIf->thisisme.id);
					 }
				 }

				 goto leave_securely;

			 } // if (errCode != 0)
		 }
	}// if(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP)
	else if (theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_UDP)
	{
		// Prepare bind
		switch (theIf->config.modeSpecific.forTcpUdp.connect.family)
		{
		case JVX_IP_V4:
			memset((char *)&my_addr_v4, 0, sizeof(my_addr_v4));

			// Specify listening port
			my_addr_v4.sin_family = family;
			my_addr_v4.sin_addr.s_addr = htonl(INADDR_ANY);
			my_addr_v4.sin_port = htons(theIf->config.modeSpecific.forTcpUdp.connect.localPort);

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
			my_addr_v6.sin6_port = htons(theIf->config.modeSpecific.forTcpUdp.connect.localPort);

			// ==================================================================
			// Bind the new socket
			// ==================================================================
			errCode = bind(theIf->runtime.theSocket, (struct sockaddr *)&my_addr_v6, sizeof(my_addr_v6));
			break;
		default: 
			assert(0);
		}

		if (errCode != 0)
		{
#ifdef JVX_OS_WINDOWS
			// Here, res already contains the error code, additional error code -1 for timeout
			// Check that connection was successful

			switch (errCode)
			{
			case WSANOTINITIALISED:
				errorDescription = "A successful WSAStartup call must occur before using this function.";;
				break;
			case WSAENETDOWN:
				errorDescription = "The network subsystem has failed.";
				break;
			case WSAEADDRINUSE:
				errorDescription = "The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but it could be delayed until the connect function if the bind was to a partially wildcard address (involving ADDR_ANY) and if a specific address needs to be committed at the time of the connect function.";
				break;
			case WSAEINTR:
				errorDescription = "The socket was closed.";
				break;
			case WSAEINPROGRESS:
				errorDescription = "A blocking Winsock call is in progress, or the service provider is still processing a callback function.";
				break;
			case WSAEALREADY:
				errorDescription = "A nonblocking connect call is in progress on the specified socket.  To preserve backward compatibility, this error is reported as WSAEINVAL to Winsock applications that link to either Winsock.dll or Wsock32.dll.";
				break;
			case WSAEADDRNOTAVAIL:
				errorDescription = "The remote address is not a valid address (such as ADDR_ANY).";
				break;
			case WSAEAFNOSUPPORT:
				errorDescription = "Addresses in the specified family cannot be used with this socket.";
				break;
			case WSAEFAULT:
				errorDescription = "The name or namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family.";
				break;
			case WSAEINVAL:
				errorDescription = "The parameter s is a listening socket.";
				break;
			case WSAEISCONN:
				errorDescription = "The socket is already connected (connection-oriented sockets only).";
				break;
			case WSAENETUNREACH:
				errorDescription = "The network cannot be reached from this host at this time.";
				break;
			case WSAENOBUFS:
				errorDescription = "No buffer space is available. The socket cannot be connected.";
				break;
			case WSAEWOULDBLOCK:
				errorDescription = "The socket is marked as nonblocking and the connection cannot be completed immediately.";
				break;
			case WSAEACCES:
				errorDescription = "An attempt to connect a datagram socket to a broadcast address failed because the setsockopt option SO_BROADCAST is not enabled.";
				break;
			default:
				assert(0); // Try to detect the reason!!
			}

			// Report error
			if (theIf->callbacks.handlerStruct)
			{

				if (theIf->callbacks.handlerStruct->common.callback_report_error)
				{
					theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_RESOLVE_HOSTNAME, errorDescription.c_str(), theIf->thisisme.id);
				}
			}

#else
			errorDescription = (std::string)"Was not able to connect to server, error: " + strerror(errno);
			// Report error
			if (theIf->callbacks.handlerStruct)
			{

				if (theIf->callbacks.handlerStruct->common.callback_report_error)
				{
					theIf->callbacks.handlerStruct->common.callback_report_error(theIf->callbacks.privData, JVX_ERROR_ID_RESOLVE_HOSTNAME, errorDescription.c_str(), theIf->thisisme.id);
				}
			}

#endif
			goto leave_securely;
		} // if (errCode != 0)
	}
	else if(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_BTH)
	{
		// Nothing to do here...
	}


	// ==================================================================================

	// Report that system has been started successfully
	if (theIf->callbacks.handlerStruct)
	{

		if (theIf->callbacks.handlerStruct->common.callback_report_startup_complete)
		{
			theIf->callbacks.handlerStruct->common.callback_report_startup_complete(theIf->callbacks.privData, theIf->thisisme.id);
		}
	}

	// ==============================================================================
	// Translate the hostname into a protocol specific something
	// ==============================================================================
	if (theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP)
	{
		// ==================================================================================
		// Connect to target server..
		// ==================================================================================

		errCode = connect_timeout(theIf->runtime.theSocket, (const sockaddr *)&theIf->runtime.targetServer, sizeof(theIf->runtime.targetServer), theIf->config.timeout_ms); // <- we can not put this in mutexes to avoid blockign
		if (errCode != 0)
		{
			jvx_connect_fail_reason reas = JVX_CONNECT_CONNECTION_SUCCESS;
#ifdef JVX_OS_WINDOWS
			// Here, res already contains the error code, additional error code -1 for timeout
			// Check that connection was successful
			if ((errCode == -1) || (errCode == WSAETIMEDOUT))
			{
				reas = JVX_CONNECT_CONNECTION_FAILED_TIMEOUT;
			}
			else if (errCode == WSAECONNREFUSED)
			{
				reas = JVX_CONNECT_CONNECTION_FAILED_REFUSED;
			}
			else if (errCode == WSAENOTSOCK)
			{
				reas = JVX_CONNECT_CONNECTION_FAILED_ENDED;
			}
			else
			{
				reas = JVX_CONNECT_CONNECTION_FAILED_MISC;
				switch (errCode)
				{
				case WSANOTINITIALISED:
					errorDescription = "A successful WSAStartup call must occur before using this function.";;
					break;
				case WSAENETDOWN:
					errorDescription = "The network subsystem has failed.";
					break;
				case WSAEADDRINUSE:
					errorDescription = "The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but it could be delayed until the connect function if the bind was to a partially wildcard address (involving ADDR_ANY) and if a specific address needs to be committed at the time of the connect function.";
					break;
				case WSAEINTR:
					errorDescription = "The socket was closed.";
					break;
				case WSAEINPROGRESS:
					errorDescription = "A blocking Winsock call is in progress, or the service provider is still processing a callback function.";
					break;
				case WSAEALREADY:
					errorDescription = "A nonblocking connect call is in progress on the specified socket.  To preserve backward compatibility, this error is reported as WSAEINVAL to Winsock applications that link to either Winsock.dll or Wsock32.dll.";
					break;
				case WSAEADDRNOTAVAIL:
					errorDescription = "The remote address is not a valid address (such as ADDR_ANY).";
					break;
				case WSAEAFNOSUPPORT:
					errorDescription = "Addresses in the specified family cannot be used with this socket.";
					break;
				case WSAEFAULT:
					errorDescription = "The name or namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family.";
					break;
				case WSAEINVAL:
					errorDescription = "The parameter s is a listening socket.";
					break;
				case WSAEISCONN:
					errorDescription = "The socket is already connected (connection-oriented sockets only).";
					break;
				case WSAENETUNREACH:
					errorDescription = "The network cannot be reached from this host at this time.";
					break;
				case WSAENOBUFS:
					errorDescription = "No buffer space is available. The socket cannot be connected.";
					break;
				case WSAEWOULDBLOCK:
					errorDescription = "The socket is marked as nonblocking and the connection cannot be completed immediately.";
					break;
				case WSAEACCES:
					errorDescription = "An attempt to connect a datagram socket to a broadcast address failed because the setsockopt option SO_BROADCAST is not enabled.";
					break;
				default:
					assert(0); // Try to detect the reason!!
				}
			}



#else
			if (errCode == -1)
			{
				reas = JVX_CONNECT_CONNECTION_FAILED_TIMEOUT;
			}
			else
			{
				errorDescription = (std::string)"Was not able to connect to server, error: " + strerror(errno);
				reas =  JVX_CONNECT_CONNECTION_FAILED_ENDED;
			}
#endif
			// Report error
			if (theIf->callbacks.handlerStruct)
			{

				if (theIf->callbacks.handlerStruct->callback_report_unsuccesful_outgoing)
				{
					theIf->callbacks.handlerStruct->callback_report_unsuccesful_outgoing(theIf->callbacks.privData, reas, theIf->thisisme.id);
				}
			}

			goto leave_securely;
		} // if (errCode != 0)
	}
	else if (theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_BTH)
	{
#ifdef OS_SUPPORTS_BTH
		if (NameToBthAddr((LPSTR)theHandle->config.target.c_str(), &RemoteBthAddr) != 0)
		{
			assert(0);
		}

		RemoteBthAddr.addressFamily = AF_BTH;
		RemoteBthAddr.serviceClassId = g_guidServiceClass;
		RemoteBthAddr.port = 0;
		errCode = connect_timeout(theIf->runtime.theSocket, (const sockaddr *)&RemoteBthAddr, sizeof(SOCKADDR_BTH), theHandle->config.timeout_ms); // <- we can not put this in mutexes to avoid blockign
#else
		assert(0);
#endif

	}

	// Set the state to "connected"
	JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
	theIf->runtime.theState = JVX_CONNECT_STATE_CONNECTED;
	JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);

	waitForConnectionThread = false;
#ifdef JVX_WITH_PCAP
	if (theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_PCAP)
	{
		if (theIf->config.modeSpecific.forPcap.timeoutConnect_msec > 0)
		{
			waitForConnectionThread = true;
			JVX_CREATE_THREAD(theIf->thread_delayedReport.threadHdl, jvx_connect_client_thread_report, theHandleId, theIf->thread_delayedReport.threadId);
		}
		else
		{
			// Report successful connection now..
			if (theIf->callbacks.handlerStruct)
			{
				if (theIf->callbacks.handlerStruct->callback_report_connect_outgoing)
				{
					theIf->callbacks.handlerStruct->callback_report_connect_outgoing(theIf->callbacks.privData, theIf->thisisme.id);
				}
			}
		}
	}
	else
	{
#endif
		// Report successful connection now..
		if(theIf->callbacks.handlerStruct)
		{
			if(theIf->callbacks.handlerStruct->callback_report_connect_outgoing)
			{
				theIf->callbacks.handlerStruct->callback_report_connect_outgoing(theIf->callbacks.privData, theIf->thisisme.id);
			}
		}
#ifdef JVX_WITH_PCAP
	}
#endif

	// ===================================================================================================
	// At this point, endless loop until server is closed...
	// ===================================================================================================

	switch(theHandle->config.socket_type)
	{
	case JVX_CONNECT_SOCKET_TYPE_TCP:
		jvx_connect_loop_tcp_poll(theHandleId);
		break;
	case JVX_CONNECT_SOCKET_TYPE_UDP:
		jvx_connect_loop_udp_poll(theHandleId);
		break;
#ifdef JVX_WITH_PCAP
	case JVX_CONNECT_SOCKET_TYPE_PCAP:
		jvx_connect_loop_pcap(theHandleId);
		break;
#endif
	}

	switch(theHandle->config.socket_type)
	{
	case JVX_CONNECT_SOCKET_TYPE_TCP:
	case JVX_CONNECT_SOCKET_TYPE_UDP:
		theIf->runtime.theSocket = JVX_INVALID_SOCKET_HANDLE;
			break;
#ifdef JVX_WITH_PCAP
	case JVX_CONNECT_SOCKET_TYPE_PCAP:

		theIf->runtime.thePcapSocket = NULL;
		break;
#endif
	default:
		assert(0);
		break;
	}

	if(waitForConnectionThread)
	{
		JVX_WAIT_FOR_THREAD_TERMINATE_MS(theIf->thread_delayedReport.threadHdl, 5000);
	}

leave_securely:

	theIf->runtime.theSocket = JVX_INVALID_SOCKET_HANDLE;

	if(theIf->callbacks.handlerStruct)
	{
		if(theIf->callbacks.handlerStruct->common.callback_report_shutdown_complete)
		{
			theIf->callbacks.handlerStruct->common.callback_report_shutdown_complete(theIf->callbacks.privData, theIf->thisisme.id);
		}
	}

	return(0);
}

// ========================================================================================
// Main API functions
// ========================================================================================

jvxErrorType
jvx_connect_client_family_initialize_st(jvx_connect_client_family** hdlFamilyReturn,
								   jvx_connect_socket_type socket_type)
{
	if(hdlFamilyReturn)
	{
		jvx_connect_client_family* theHandle = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(theHandle, jvx_connect_client_family);

		// Some initialization code
		init_client_handle(theHandle, socket_type);

		*hdlFamilyReturn = theHandle;

		return(JVX_NO_ERROR);
	}

	return(JVX_ERROR_INVALID_ARGUMENT);
}

/**
 * Return the number of physical interfaces - Makes sense only for Bluetooth and pcap
 *///===================================================================================
jvxErrorType jvx_connect_client_family_number_interfaces_st(jvx_connect_client_family* hdlFamily, jvxSize* num)
{
	if(hdlFamily)
	{
		if(num)
		{
			*num = hdlFamily->theIfs.size();
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType jvx_connect_client_family_name_interfaces_st(jvx_connect_client_family* hdlFamily, char* fld, jvxSize szFld, jvxSize idx)
{
	if(hdlFamily)
	{
		if(idx < hdlFamily->theIfs.size())
		{
			jvxSize minChars = JVX_MIN(szFld,hdlFamily->theIfs[idx].config.name.size());
			memcpy(fld, hdlFamily->theIfs[idx].config.name.c_str(), sizeof(char)*minChars);
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType jvx_connect_client_family_description_interfaces_st(jvx_connect_client_family* hdlFamily, char* fld, jvxSize szFld, jvxSize idx)
{
	if(hdlFamily)
	{
		if(idx < hdlFamily->theIfs.size())
		{
			jvxSize minChars = JVX_MIN(szFld,hdlFamily->theIfs[idx].config.description.size());
			memcpy(fld, hdlFamily->theIfs[idx].config.description.c_str(), sizeof(char)*minChars);
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_connect_client_family_terminate_st(jvx_connect_client_family* hdlFamily)
{
	jvxSize i;
	if(hdlFamily)
	{
		for(i = 0; i < hdlFamily->theIfs.size(); i++)
		{
			jvx_connect_client_interface_terminate_st(&hdlFamily->theIfs[i].thisisme);

			// Opposite of init_client_handle
			JVX_TERMINATE_RW_MUTEX(hdlFamily->theIfs[i].runtime.safeAccess);
		}

		JVX_DSP_SAFE_DELETE_OBJECT(hdlFamily);

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
//=====================================================================
//=                                                                   =
//=                                                                   =
//=                                                                   =
//=                                                                   =
//=                                                                   =
//=                                                                   =
//=                                                                   =
//=                                                                   =
//=====================================================================

/*
 * ====================================================================*/
jvxErrorType
jvx_connect_client_interface_initialize_st(jvx_connect_client_family* hdlFamily,
										   jvx_connect_client_if** hdlClientOut,
										   jvx_connect_client_callback_type* report_callback,
										   jvxHandle* privateUser,
										   jvxSize timeout_ms,
										   jvxSize idx)
{
	if(hdlFamily && hdlClientOut)
	{

		oneClientInterface * theIf = NULL;
		if(idx < hdlFamily->theIfs.size())
		{
			theIf = &hdlFamily->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ID_OUT_OF_BOUNDS);
		}

		//theIf->thisisme;
		*hdlClientOut = &theIf->thisisme;

		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		if(theIf->runtime.theState == JVX_CONNECT_STATE_NONE)
		{
			theIf->config.timeout_ms = timeout_ms;
			theIf->callbacks.handlerStruct = report_callback;
			theIf->callbacks.privData = privateUser;

			theIf->config.modeSpecific.forTcpUdp.connect.connectToPort = -1;
			theIf->config.modeSpecific.forTcpUdp.connect.family = JVX_IP_V4;
			theIf->config.modeSpecific.forTcpUdp.connect.localPort = -1;
			theIf->config.modeSpecific.forTcpUdp.connect.targetName = "unknown";
			theIf->config.modeSpecific.forTcpUdp.disableNagleAlgorithmTcp = true;

#ifdef JVX_OS_WINDOWS

			if(
				(hdlFamily->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP) ||
				(hdlFamily->config.socket_type == JVX_CONNECT_SOCKET_TYPE_UDP))
			{

				/* initialize TCP for Windows ("winsock") */
				/* this is not necessary for GNU systems  */
				short wVersionRequested;
				WSADATA wsaData;
				wVersionRequested = MAKEWORD (2, 2);
				int res = WSAStartup (wVersionRequested, &wsaData);
				assert(res == 0);
			}
#endif


			theIf->runtime.theState = JVX_CONNECT_STATE_INIT;
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_connect_client_interface_connect_st(jvx_connect_client_if* hdlClient, jvxHandle* config_specific, 
	jvxConnectionPrivateTypeEnum whatsthis)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if(idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		if(theIf->runtime.theState == JVX_CONNECT_STATE_INIT)
		{
			// Enable mode specific configuration
			if (config_specific)
			{
				switch (theHandle->config.socket_type)
				{
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
				}
			}
			else
			{
				switch (theHandle->config.socket_type)
				{
				case JVX_CONNECT_SOCKET_TYPE_TCP:
					std::cout << "Uninitialized configuration of TCP connection." << std::endl;
					assert(0);
					break;
				case JVX_CONNECT_SOCKET_TYPE_UDP:
					std::cout << "Uninitialized configuration of UDP connection." << std::endl;
					assert(0);
					break;
				}
			}

			theIf->runtime.theState = JVX_CONNECT_STATE_STARTED;
			// Start socket thread
			JVX_CREATE_THREAD(theIf->thread.threadHdl, jvx_connect_client_thread_entry, hdlClient, theIf->thread.threadId);
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		return(res);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_connect_client_set_thread_priority_st(jvx_connect_client_if* hdlClient, JVX_THREAD_PRIORITY prio)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if(idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		res = JVX_ERROR_WRONG_STATE;
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		if(theIf->runtime.theState >= JVX_CONNECT_STATE_INIT)
		{
			JVX_SET_THREAD_PRIORITY(theIf->thread.threadHdl, prio);
			res = JVX_NO_ERROR;
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		return(res);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}


jvxErrorType
jvx_connect_client_field_constraint_st(jvx_connect_client_if* hdlClient, jvxSize* bytesPrepend, jvxSize* fldMinSize, jvxSize* fldMaxSize)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if(hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if(idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		res = JVX_ERROR_WRONG_STATE;
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		if(theIf->runtime.theState >= JVX_CONNECT_STATE_INIT)
		{
			res = JVX_NO_ERROR;
#ifdef JVX_WITH_PCAP
			switch(theHandle->config.socket_type)
			{
			case JVX_CONNECT_SOCKET_TYPE_PCAP:
				if(bytesPrepend)
				{
					*bytesPrepend = sizeof(jvxRawSocketHeader);
				}
				if(fldMinSize)
				{
					*fldMinSize = ETH_MIN_SIZE_RAW_SOCKET;
				}
				if(fldMaxSize)
				{
					*fldMaxSize = PCAP_SNAP_LEN;
				}
				break;
			default:
#endif
				if(bytesPrepend)
				{
					*bytesPrepend = 0;
				}
				if(fldMinSize)
				{
					*fldMinSize = 0;
				}
				if(fldMaxSize)
				{
					*fldMaxSize = JVX_SIZE_UNSELECTED;
				}
#ifdef JVX_WITH_PCAP
			}
#endif
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);		
	}
	return(res);
}

jvxErrorType
jvx_connect_client_prepare_udp_target_st(jvx_connect_client_if* hdlClient, const char* server, int port, jvx_udp_target* theTarget, jvxConnectionIpType ipProt)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if(idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		res = JVX_ERROR_WRONG_STATE;
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		if(theIf->runtime.theState >= JVX_CONNECT_STATE_STARTED) 
		{
			if(theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_UDP)
			{
				res = jvx_connect_client_prepare_udp_target_st__core(server, theTarget, port, ipProt);
			}
			else
			{
				res = JVX_ERROR_UNSUPPORTED;
			}
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		return(res);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_connect_client_send_st(jvx_connect_client_if* hdlClient, char* buf, jvxSize* szFld, jvxHandle* wheretosendto, jvxConnectionPrivateTypeEnum whatsthis)
{
	//char errbuf[PCAP_ERRBUF_SIZE];
	//struct pcap_pkthdr header;
	//jvxUInt64 tStamp1, tStamp2;

	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if(hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if(idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		res = JVX_ERROR_WRONG_STATE;
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		if(theIf->runtime.theState >= JVX_CONNECT_STATE_CONNECTED) 
		{
			res = JVX_NO_ERROR;
			int errCode = 0;

			switch(theHandle->config.socket_type)
			{
			case JVX_CONNECT_SOCKET_TYPE_TCP:
				errCode = send(theIf->runtime.theSocket, buf, JVX_SIZE_INT(*szFld), 0);
				break;
			case JVX_CONNECT_SOCKET_TYPE_UDP:
				if(wheretosendto)
				{
					assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_UDP_TARGET);
					errCode = sendto(theIf->runtime.theSocket, buf, JVX_SIZE_INT(*szFld), 0, (const sockaddr *)wheretosendto, sizeof(sockaddr_in));
				}
				else
				{
					if (!theIf->runtime.targetServerSet)
					{
						if (!theIf->config.modeSpecific.forTcpUdp.connect.targetName.empty())
						{
							res = jvx_connect_client_prepare_udp_target_st__core(theIf->config.modeSpecific.forTcpUdp.connect.targetName.c_str(),
								&theIf->runtime.targetServer,
								theIf->config.modeSpecific.forTcpUdp.connect.connectToPort, theIf->config.modeSpecific.forTcpUdp.connect.family);
							theIf->runtime.targetServerSet = true;
						}
						else
						{
							res = JVX_ERROR_NOT_READY;
						}
					}

					if(res == JVX_NO_ERROR)
					{
						errCode = sendto(theIf->runtime.theSocket, buf, JVX_SIZE_INT(*szFld), 0,
						(const sockaddr *)&theIf->runtime.targetServer,
							sizeof(sockaddr_in));
					}
				}

				break;
#ifdef JVX_WITH_PCAP
			case JVX_CONNECT_SOCKET_TYPE_PCAP:

				if (wheretosendto)
				{
					assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_PCAP_TARGET);
					jvx_connect_send_spec_pcap* sendme = (jvx_connect_send_spec_pcap*)wheretosendto;

					if (!sendme->sendRaw)
					{
						// This fills a source and a target mac 
						pcap_prepareBufferSend((char*)buf, sendme->dest_mac, (jvxUInt8*)theIf->runtime.mac);
					}
				}
				else
				{
					pcap_prepareBufferSend((char*)buf, theIf->config.modeSpecific.forPcap.dest_mac, (jvxUInt8*)theIf->runtime.mac);
				}
				errCode = pcap_sendpacket(theIf->runtime.thePcapSocket, (const u_char*)buf , JVX_SIZE_INT(*szFld));
				break;
#endif
			default:
				assert(0);
			}

			if (errCode < 0)
			{
				switch (theHandle->config.socket_type)
				{
				case JVX_CONNECT_SOCKET_TYPE_TCP:
				case JVX_CONNECT_SOCKET_TYPE_UDP:
#ifdef JVX_OS_WINDOWS
					errCode = WSAGetLastError();
					switch (errCode)
					{
					case WSAEFAULT:
						std::cout << __FUNCTION__ << ": Error: The name parameter is not a valid part of the user address space, " <<
							"or the buffer size specified by the namelen parameter is too small to hold the complete host name." << std::endl;
						break;
					case WSANOTINITIALISED:
						std::cout << __FUNCTION__ << ": Error: A successful WSAStartup call must occur before using this function." << std::endl;
						break;
					case WSAENETDOWN:
						std::cout << __FUNCTION__ << ": Error: The network subsystem has failed." << std::endl;
						break;
					case WSAEINPROGRESS:
						std::cout << __FUNCTION__ << ": Error: A blocking Winsock call is in progress, or the service " <<
							"provider is still processing a callback function." << std::endl;
						break;
					case WSAEWOULDBLOCK:
						std::cout << __FUNCTION__ << ": Error: A data transfer would block the socket." << std::endl;
						break;
					case WSAEMSGSIZE:
						std::cout << __FUNCTION__ << ": Error: The packet size of <" << *szFld << "> is too large." << std::endl;
						break;
					default:
						std::cout << __FUNCTION__ << ": Error: Function <sendto> failed, error: " << errCode << "." << std::endl;
					}
#else
					std::cout << __FUNCTION__ << ": Error: Function <sendto> failed, error: " << strerror(errCode) << std::endl;

#endif
					break;
				}

				*szFld = errCode;
				res = JVX_ERROR_INTERNAL;
			}
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
	}
	return(res);
}

#if (defined JVX_WITH_PCAP) && (defined JVX_OS_WINDOWS)
jvxErrorType
jvx_connect_client_start_queue_st(jvx_connect_client_if* hdlClient, jvxSize szFld, jvxSize numFldsQ)
{
	//char errbuf[PCAP_ERRBUF_SIZE];
	//struct pcap_pkthdr header;
	//jvxUInt64 tStamp1, tStamp2;

	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		res = JVX_ERROR_WRONG_STATE;
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		if (theIf->runtime.theState >= JVX_CONNECT_STATE_INIT) 
		{
			res = JVX_NO_ERROR;
			jvxSize errCode = 0;

			switch (theHandle->config.socket_type)
			{

			case JVX_CONNECT_SOCKET_TYPE_PCAP:

				if (theIf->runtime.my_send_queue == NULL)
				{
					theIf->runtime.my_send_queue = pcap_sendqueue_alloc(JVX_SIZE_INT((szFld + sizeof(pcap_pkthdr))*numFldsQ));
					assert(theIf->runtime.my_send_queue);
				}
				else
				{
					res = JVX_ERROR_NOT_READY;
				}
				break;
			default:
				res = JVX_ERROR_UNSUPPORTED;
			}
			if (errCode == SOCKET_ERROR)
			{
				res = JVX_ERROR_INTERNAL;
			}
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
	}
	return(res);
}

jvxErrorType
jvx_connect_client_stop_queue_st(jvx_connect_client_if* hdlClient)
{
	//char errbuf[PCAP_ERRBUF_SIZE];
	//struct pcap_pkthdr header;
	//jvxUInt64 tStamp1, tStamp2;

	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		res = JVX_ERROR_WRONG_STATE;
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		if (theIf->runtime.theState >= JVX_CONNECT_STATE_INIT)
		{
			res = JVX_NO_ERROR;
			jvxSize errCode = 0;

			switch (theHandle->config.socket_type)
			{
			case JVX_CONNECT_SOCKET_TYPE_PCAP:

				if (theIf->runtime.my_send_queue)
				{
					pcap_sendqueue_destroy(theIf->runtime.my_send_queue);
					theIf->runtime.my_send_queue = NULL;
				}
				else
				{
					res = JVX_ERROR_NOT_READY;
				}
				break;
			default:
				res = JVX_ERROR_UNSUPPORTED;
			}
			if (errCode == SOCKET_ERROR)
			{
				res = JVX_ERROR_INTERNAL;
			}
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
	}
	return(res);
}

jvxErrorType
jvx_connect_client_collect_st(jvx_connect_client_if* hdlClient, char* buf, jvxSize* szFld, jvxHandle* wheretosendto, jvxConnectionPrivateTypeEnum whatsthis)
{
	//char errbuf[PCAP_ERRBUF_SIZE];
#ifdef JVX_WITH_PCAP
	struct pcap_pkthdr header;
#endif
	//jvxUInt64 tStamp1, tStamp2;

	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		res = JVX_ERROR_WRONG_STATE;
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		if (theIf->runtime.theState >= JVX_CONNECT_STATE_STARTED)
		{
			res = JVX_NO_ERROR;
			jvxSize errCode = 0;

			switch (theHandle->config.socket_type)
			{

			case JVX_CONNECT_SOCKET_TYPE_PCAP:

				if (theIf->runtime.my_send_queue)
				{
					if (wheretosendto)
					{
						assert(whatsthis == JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_PCAP_TARGET);
						jvx_connect_send_spec_pcap* sendme = (jvx_connect_send_spec_pcap*)wheretosendto;

						if (!sendme->sendRaw)
						{
							// This fills a source and a target mac 
							pcap_prepareBufferSend((char*)buf, sendme->dest_mac, (jvxUInt8*)theIf->runtime.mac);
						}
					}
					else
					{
						pcap_prepareBufferSend((char*)buf, theIf->config.modeSpecific.forPcap.dest_mac, (jvxUInt8*)theIf->runtime.mac);
					}

					header.len = JVX_SIZE_INT(*szFld);
					header.caplen = JVX_SIZE_INT(*szFld);
					errCode = pcap_sendqueue_queue(theIf->runtime.my_send_queue, &header, (const u_char*)buf);
				}
				else
				{
					res = JVX_ERROR_INVALID_SETTING;
				}
				break;
			default:
				res = JVX_ERROR_UNSUPPORTED;
			}
			if (errCode == SOCKET_ERROR)
			{
				res = JVX_ERROR_INTERNAL;
			}
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
	}
	return(res);
}

jvxErrorType
jvx_connect_client_transmit_st(jvx_connect_client_if* hdlClient)
{
	//char errbuf[PCAP_ERRBUF_SIZE];
	//struct pcap_pkthdr header;
	//jvxUInt64 tStamp1, tStamp2;

	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		res = JVX_ERROR_WRONG_STATE;
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		if (theIf->runtime.theState >= JVX_CONNECT_STATE_STARTED) 
		{
			res = JVX_NO_ERROR;
			jvxSize errCode = 0;

			switch (theHandle->config.socket_type)
			{

			case JVX_CONNECT_SOCKET_TYPE_PCAP:

				if (theIf->runtime.my_send_queue)
				{
					errCode = pcap_sendqueue_transmit(theIf->runtime.thePcapSocket, theIf->runtime.my_send_queue, 0);
					theIf->runtime.my_send_queue->len = 0;
				}
				else
				{
					res = JVX_ERROR_NOT_READY;
				}
				break;


			default:
				res = JVX_ERROR_UNSUPPORTED;
			}
			if (errCode == SOCKET_ERROR)
			{
				res = JVX_ERROR_INTERNAL;
			}
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
	}
	return(res);
}
#else

jvxErrorType 
jvx_connect_client_start_queue_st(jvx_connect_client_if* hdlClient, jvxSize szFld, jvxSize numFldsQ)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
jvx_connect_client_collect_st(jvx_connect_client_if* hdlClient, char* buf, jvxSize* szFld, jvxHandle* wheretosendto, jvxConnectionPrivateTypeEnum whatsthis)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
jvx_connect_client_transmit_st(jvx_connect_client_if* hdlClient)
{
	return JVX_ERROR_UNSUPPORTED;
}


jvxErrorType 
jvx_connect_client_stop_queue_st(jvx_connect_client_if* hdlClient)
{
	return JVX_ERROR_UNSUPPORTED;
}

#endif

jvxErrorType
 jvx_connect_client_trigger_shutdown_st(jvx_connect_client_if* hdlClient)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;

	if(hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if(idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		res = JVX_ERROR_WRONG_STATE;
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);

		// Avoid double shutdown triggers!!
		if(
			(theIf->runtime.theState >= JVX_CONNECT_STATE_STARTED) && 
			(theIf->runtime.theState < JVX_CONNECT_STATE_WAIT_FOR_SHUTDOWN))
		{
			theIf->runtime.theState = JVX_CONNECT_STATE_WAIT_FOR_SHUTDOWN;
			switch(theHandle->config.socket_type)
			{
			case JVX_CONNECT_SOCKET_TYPE_TCP:
			case JVX_CONNECT_SOCKET_TYPE_UDP:
				JVX_SHUTDOWN_SOCKET(theIf->runtime.theSocket, JVX_SOCKET_SD_BOTH);
				JVX_CLOSE_SOCKET(theIf->runtime.theSocket);
				break;
#ifdef JVX_WITH_PCAP
			case JVX_CONNECT_SOCKET_TYPE_PCAP:

				
				pcap_close(theIf->runtime.thePcapSocket);
				break;
#endif
			default:
				assert(0);
				break;
			}
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);

		res = JVX_NO_ERROR;

	}
	return(res);
}

jvxErrorType
jvx_connect_client_optional_wait_for_shutdown_st(jvx_connect_client_if* hdlClient)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;

	if(hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if(idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		res = JVX_ERROR_WRONG_STATE;

		if(theIf->runtime.theState > JVX_CONNECT_STATE_INIT) // Even if state is
		{
			// Wait for shutdown. MUST BE OUTSIDE MUTEX SECTION!!!!
			JVX_WAIT_FOR_THREAD_TERMINATE_INF(theIf->thread.threadHdl);

			theIf->runtime.theState = JVX_CONNECT_STATE_INIT;

			// After this moment would be the right place to do the cleanup of the send queues
			res = JVX_NO_ERROR;
		}
	}
	return(res);
}

jvxErrorType
jvx_connect_client_interface_terminate_st(jvx_connect_client_if* hdlClient)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;

	if(hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if(idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		// No critical section required as the state init means that no parallel thread is running
		if(theIf->runtime.theState == JVX_CONNECT_STATE_INIT)
		{
			theIf->runtime.theState = JVX_CONNECT_STATE_NONE;
			theIf->config.timeout_ms = JVX_SIZE_UNSELECTED;
			theIf->callbacks.handlerStruct = NULL;
			theIf->callbacks.privData = NULL;

			theIf->config.modeSpecific.forTcpUdp.connect.localPort = -1;
			theIf->config.modeSpecific.forTcpUdp.connect.connectToPort = -1;
			theIf->config.modeSpecific.forTcpUdp.connect.targetName = "";
			theIf->config.modeSpecific.forTcpUdp.disableNagleAlgorithmTcp = false;


#ifdef JVX_WITH_PCAP
			memset(&theIf->config.modeSpecific.forPcap, 0, sizeof(jvx_connect_spec_pcap));
#endif
#ifdef OS_SUPPORTS_BTH
			memset(&theIf->config.modeSpecific.forBth, 0, sizeof(jvx_connect_spec_bth));
#endif
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType
jvx_connect_client_lock_state_st(jvx_connect_client_if* hdlClient)
{
	if(hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if(idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_connect_client_unlock_state_st(jvx_connect_client_if* hdlClient)
{
	if(hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if(idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_connect_client_get_state_st(jvx_connect_client_if* hdlClient, jvx_connect_state* stateOnReturn)
{
	if(hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if(idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		if(stateOnReturn)
		{
			*stateOnReturn = theIf->runtime.theState;
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

// Private function used by server
jvxHandle* 
jvx_connect_client_priv_get_callback_priv(jvx_connect_client_if* hdlClient)
{
	jvx_connect_client_family* theHandle = hdlClient->priv;
	jvxSize idx = hdlClient->id;

	oneClientInterface * theIf = NULL;
	if (idx < theHandle->theIfs.size())
	{
		theIf = &theHandle->theIfs[idx];
	}
	else
	{
		return(NULL);
	}
	return(theIf->callbacks.privData);
}

jvxErrorType
jvx_connect_client_whoami_st(jvx_connect_client_if* hdlClient, char* whoami_name, jvxSize szText_name, jvx_connect_descriptor_get whattoget)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string errorDescription;
	std::string ip;
	if (hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		res = JVX_ERROR_WRONG_STATE;
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		if (theIf->runtime.theState >= JVX_CONNECT_STATE_INIT)
		{
			res = JVX_NO_ERROR;
			if (whoami_name)
			{
				switch (whattoget)
				{
				case JVX_CONNECT_SOCKET_NAME_MY_NAME:
					if ((theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_TCP) || (theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_UDP))
					{
						int errCode = gethostname(whoami_name, JVX_SIZE_INT(szText_name));
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
					}
					else
					{
#ifdef JVX_WITH_PCAP
						if (theHandle->config.socket_type == JVX_CONNECT_SOCKET_TYPE_PCAP)
						{
							jvx_fillCharStr(whoami_name, szText_name, theIf->config.description);
						}
						else
						{
#endif
							res = JVX_ERROR_UNSUPPORTED;
#ifdef JVX_WITH_PCAP
						}
#endif
					}
					break;
				case JVX_CONNECT_SOCKET_NAME_ALL_IP_ADDR:
					switch (theHandle->config.socket_type)
					{
					case JVX_CONNECT_SOCKET_TYPE_TCP:
					case JVX_CONNECT_SOCKET_TYPE_UDP:

						// Everything fine, next, get the IP address
						// Get the IP addresses (all)
						ip = myNameToIpAddresses(theIf->config.modeSpecific.forTcpUdp.connect.family);
						jvx_fillCharStr(whoami_name, szText_name, ip);
						break;
					default:
						res = JVX_ERROR_UNSUPPORTED;
					}
					break;
				case JVX_CONNECT_SOCKET_NAME_ONE_IP_ADDR:
					switch (theHandle->config.socket_type)
					{
					case JVX_CONNECT_SOCKET_TYPE_TCP:
					case JVX_CONNECT_SOCKET_TYPE_UDP:

						// Everything fine, next, get the IP address
						// Get the IP addresses (all)
						ip = myNameToFirstIpAddress(theIf->config.modeSpecific.forTcpUdp.connect.family);
						jvx_fillCharStr(whoami_name, szText_name, ip);
						break;

					default:
						res = JVX_ERROR_UNSUPPORTED;
					}
					break;
				default:
					assert(0);
				}
			}
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
	}
	return(res);
}

jvxErrorType
jvx_connect_client_identify_st(jvx_connect_client_if* hdlClient, char* thatis, jvxSize szText, const char* whoisthat)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string errorDescription;
	jvxSize sz = 0;
	struct hostent* origHost = NULL;
	struct hostent *he = NULL;
	std::string ip;

	if (hdlClient)
	{
		jvx_connect_client_family* theHandle = hdlClient->priv;
		jvxSize idx = hdlClient->id;

		oneClientInterface * theIf = NULL;
		if (idx < theHandle->theIfs.size())
		{
			theIf = &theHandle->theIfs[idx];
		}
		else
		{
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		res = JVX_ERROR_WRONG_STATE;
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		if (theIf->runtime.theState >= JVX_CONNECT_STATE_INIT)
		{
			res = JVX_NO_ERROR;
			if (thatis)
			{
				// Get the name of this computer
				switch(theHandle->config.socket_type)
				{
				case JVX_CONNECT_SOCKET_TYPE_TCP:
				case JVX_CONNECT_SOCKET_TYPE_UDP:

					// Everything fine, next, get the IP address
					// Get the IP address
					ip = ipAddressToHostName(whoisthat, theIf->config.modeSpecific.forTcpUdp.connect.connectToPort, theIf->config.modeSpecific.forTcpUdp.connect.family);
					jvx_fillCharStr(thatis, szText, ip);
					break;

					jvx_fillCharStr(thatis, szText, ip);
					break;

				default:
					res = JVX_ERROR_UNSUPPORTED;
				}
			}
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(theIf->runtime.safeAccess);
		return(res);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
