#include "CjvxSockets.h"

JVX_THREAD_ENTRY_FUNCTION(static_connect_loop_udp_poll, this_pointer)
{
	CjvxSocketsClientInterfaceTcpUdp* this_p = (CjvxSocketsClientInterfaceTcpUdp*)this_pointer;
	if (this_p)
	{
		this_p->ic_connect_loop_udp_poll();
	}
	return(0);
}

CjvxSocketsClientInterfaceTcpUdp::CjvxSocketsClientInterfaceTcpUdp() : CjvxSocketsClientInterface()
{
	memset(&targetServer_ip4, 0, sizeof(targetServer_ip4));
	memset(&targetServer_ip6, 0, sizeof(targetServer_ip6));
}

jvxErrorType
CjvxSocketsClientInterfaceTcpUdp::configure(
	const std::string& targetNameArg,
	int remotePortArg,
	int localPortArg,
	jvxSocketsFamily familyArg,
	jvxSize timeout_msecsArg)
{
	jvxErrorType res = CjvxSocketsClientInterface::configure();
	if (res == JVX_NO_ERROR)
	{
		targetName = targetNameArg;
		remotePort = remotePortArg;
		localPort = localPortArg;
		family = familyArg;
		timeout_msecs = timeout_msecsArg;
	}
	return res;
};

jvxErrorType
CjvxSocketsClientInterfaceTcpUdp::pre_start_socket()
{
#ifdef JVX_OS_WINDOWS
	/* initialize TCP for Windows ("winsock") */
	/* this is not necessary for GNU systems  */
	short wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	int res = WSAStartup(wVersionRequested, &wsaData);
	assert(res == 0);
#endif
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSocketsClientInterfaceTcpUdp::set_opts_socket()
{
	int flag = 1;
	int errCode = 0;
	jvxErrorType res = JVX_NO_ERROR;
	if (socketType == jvxSocketsConnectionType::JVX_SOCKET_TYPE_UDP)
	{
		if (allowReuseSocketUdp)
		{
#ifdef JVX_OS_WINDOWS
			flag = 1;
			errCode = setsockopt(theSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(int));
			if (errCode != 0)
			{
				res = JVX_ERROR_INTERNAL;
				errCode = WSAGetLastError();
				switch (errCode)
				{
				case WSANOTINITIALISED:
					last_error = "A successful WSAStartup call must occur before using this function.";
					break;
				case WSAENETDOWN:
					last_error = "The network subsystem has failed.";
					break;
				case WSAEFAULT:
					last_error = "The buffer pointed to by the optval parameter is not in a valid part of the process address space or the optlen parameter is too small.";
					break;
				case WSAEINPROGRESS:
					last_error = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
					break;
				case WSAEINVAL:
					last_error = "The level parameter is not valid, or the information in the buffer pointed to by the optval parameter is not valid.";
					break;
				case WSAENETRESET:
					last_error = "The connection has timed out when SO_KEEPALIVE is set.";
					break;
				case WSAENOPROTOOPT:
					last_error = "The option is unknown or unsupported for the specified provider or socket (see SO_GROUP_PRIORITY limitations).";
					break;
				case WSAENOTCONN:
					last_error = "The connection has been reset when SO_KEEPALIVE is set.";
					break;
				case WSAENOTSOCK:
					last_error = "The descriptor is not a socket.";
					break;
				}
			}

#else
			flag = 1;
			errCode = setsockopt(theSocket, IPPROTO_UDP, SO_REUSEADDR, (char*)&flag, sizeof(int));    /* length of option value */
			if (errCode < 0)
			{
				res = JVX_ERROR_INTERNAL;
				last_error = (std::string)"Was not able to set socket options, Error: " + strerror(errno);
			}
#endif

		}
	}
	return res;
}
jvxErrorType
CjvxSocketsClientInterfaceTcpUdp::start_socket()
{
	int sockTp = SOCK_STREAM;
	int proto = IPPROTO_TCP;
	if (socketType == jvxSocketsConnectionType::JVX_SOCKET_TYPE_UDP)
	{
		proto = IPPROTO_UDP;
		sockTp = SOCK_DGRAM;
	}

	switch (family)
	{
	case jvxSocketsFamily::JVX_SOCKET_IP_V4:
		theSocket = socket(AF_INET, sockTp, proto);
		break;
	case jvxSocketsFamily::JVX_SOCKET_IP_V6:
		theSocket = socket(AF_INET6, sockTp, proto);
		break;
	default:
		assert(0);
	}

	if (
		(theSocket == INVALID_SOCKET) ||
		(theSocket == SOCKET_ERROR))
	{

#ifdef JVX_OS_WINDOWS
		int errCode = WSAGetLastError();
		switch (errCode)
		{
		case WSANOTINITIALISED:
			last_error = "A successful WSAStartup call must occur before using this function.";
			break;
		case WSAENETDOWN:
			last_error = "The network subsystem or the associated service provider has failed.";
			break;
		case WSAEAFNOSUPPORT:
			last_error = "The specified address family is not supported. For example, an application tried to create a socket for the AF_IRDA address family but an infrared adapter and device driver is not installed on the local computer.";
			break;
		case WSAEINPROGRESS:
			last_error = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
			break;
		case WSAEMFILE:
			last_error = "No more socket descriptors are available.";
			break;
		case WSAEINVAL:
			last_error = "An invalid argument was supplied. This error is returned if the af parameter is set to AF_UNSPEC and the type and currentConfiguration.socketProtocolType parameter are unspecified.";
			break;
		case WSAENOBUFS:
			last_error = "No buffer space is available. The socket cannot be created.";
			break;
		case WSAEPROTONOSUPPORT:
			last_error = "The specified currentConfiguration.socketProtocolType is not supported.";
			break;
		case WSAEPROTOTYPE:
			last_error = "The specified currentConfiguration.socketProtocolType is the wrong type for this socket.";
			break;
		case WSAESOCKTNOSUPPORT:
			last_error = "The specified socket type is not supported in this address family.";
			break;
		}

#else // JVX_OS_WINDOWS
		last_error = (std::string)"Was not able to create socket, error: " + strerror(errno);
#endif // JVX_OS_WINDOWS
		return JVX_ERROR_INTERNAL;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsClientInterfaceTcpUdp::connect_socket()
{
	jvxErrorType res = JVX_NO_ERROR;
	struct sockaddr_in my_addr_v4;
	struct sockaddr_in6 my_addr_v6;
	int errCode = 0;

	if (localPort > 0)
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
		switch (family)
		{
		case jvxSocketsFamily::JVX_SOCKET_IP_V4:
			memset((char*)&my_addr_v4, 0, sizeof(my_addr_v4));

			// Specify listening port
			my_addr_v4.sin_family = AF_INET;
			my_addr_v4.sin_addr.s_addr = htonl(INADDR_ANY);
			my_addr_v4.sin_port = htons(localPort);

			// ==================================================================
			// Bind the new socket
			// ==================================================================
			errCode = bind(theSocket, (struct sockaddr*)&my_addr_v4, sizeof(my_addr_v4));
			break;
		case jvxSocketsFamily::JVX_SOCKET_IP_V6:
			memset((char*)&my_addr_v6, 0, sizeof(my_addr_v6));

			// Specify listening port
			my_addr_v6.sin6_family = AF_INET6;
			my_addr_v6.sin6_addr = in6addr_any;
			my_addr_v6.sin6_port = htons(localPort);

			// ==================================================================
			// Bind the new socket
			// ==================================================================
			errCode = bind(theSocket, (struct sockaddr*)&my_addr_v6, sizeof(my_addr_v6));
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
				last_error = "A successful WSAStartup call must occur before using this function.";;
				break;
			case WSAENETDOWN:
				last_error = "The network subsystem has failed.";
				break;
			case WSAEADDRINUSE:
				last_error = "The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but it could be delayed until the connect function if the bind was to a partially wildcard address (involving ADDR_ANY) and if a specific address needs to be committed at the time of the connect function.";
				break;
			case WSAEINTR:
				last_error = "The socket was closed.";
				break;
			case WSAEINPROGRESS:
				last_error = "A blocking Winsock call is in progress, or the service provider is still processing a callback function.";
				break;
			case WSAEALREADY:
				last_error = "A nonblocking connect call is in progress on the specified socket.  To preserve backward compatibility, this error is reported as WSAEINVAL to Winsock applications that link to either Winsock.dll or Wsock32.dll.";
				break;
			case WSAEADDRNOTAVAIL:
				last_error = "The remote address is not a valid address (such as ADDR_ANY).";
				break;
			case WSAEAFNOSUPPORT:
				last_error = "Addresses in the specified family cannot be used with this socket.";
				break;
			case WSAEFAULT:
				last_error = "The name or namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family.";
				break;
			case WSAEINVAL:
				last_error = "The parameter s is a listening socket.";
				break;
			case WSAEISCONN:
				last_error = "The socket is already connected (connection-oriented sockets only).";
				break;
			case WSAENETUNREACH:
				last_error = "The network cannot be reached from this host at this time.";
				break;
			case WSAENOBUFS:
				last_error = "No buffer space is available. The socket cannot be connected.";
				break;
			case WSAEWOULDBLOCK:
				last_error = "The socket is marked as nonblocking and the connection cannot be completed immediately.";
				break;
			case WSAEACCES:
				last_error = "An attempt to connect a datagram socket to a broadcast address failed because the setsockopt option SO_BROADCAST is not enabled.";
				break;
			default:
				assert(0); // Try to detect the reason!!
			}

			// Report error

#else
			last_error = (std::string)"Was not able to connect to server, error: " + strerror(errno);
#endif
		}
	}
	return res;
}

jvxErrorType
CjvxSocketsClientInterfaceTcpUdp::start_connection_loop()
{
	// Create thread:
	JVX_CREATE_THREAD(theThreadHdl,
		static_connect_loop_udp_poll,
		reinterpret_cast<jvxHandle*>(this),
		theThreadId);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsClientInterfaceTcpUdp::stop_connection_loop()
{
#if 0
	if (socketStartComplete)
	{
		// If the thread was started in the main thread, close sockets here, otherwise this will happen in worker thread
		JVX_SHUTDOWN_SOCKET(theSocket, JVX_SOCKET_SD_BOTH);
		JVX_CLOSE_SOCKET(theSocket);
	}
	#endif

	JVX_SHUTDOWN_SOCKET(theSocket, JVX_SOCKET_SD_BOTH);
	JVX_CLOSE_SOCKET(theSocket);

	JVX_WAIT_FOR_THREAD_TERMINATE_INF(theThreadHdl);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsClientInterfaceTcpUdp::stop_socket()
{
	return JVX_NO_ERROR;
}
void
CjvxSocketsClientInterfaceTcpUdp::ic_connect_loop_udp_poll()
{
	CjvxSocketsConnectionTcpUdp* theConnectionTcpUdp = NULL;

	JVX_DSP_SAFE_ALLOCATE_OBJECT(theConnectionTcpUdp, CjvxSocketsConnectionTcpUdp);
	theConnectionTcpUdp->configure(this);

	theConnection = static_cast<CjvxSocketsConnection*>(theConnectionTcpUdp);

	if (theReportClient)
	{
		theReportClient->report_client_connect(static_cast<IjvxSocketsConnection*>(theConnection));
	}

	theConnection->main_loop();

	if (theReportClient)
	{
		theReportClient->report_client_disconnect(static_cast<IjvxSocketsConnection*>(theConnection));
	}
	JVX_DSP_SAFE_DELETE_OBJECT(theConnectionTcpUdp);
	theConnectionTcpUdp = NULL;
	theConnection = NULL;
}

jvxErrorType 
CjvxSocketsConnectionTcpUdp::configure(CjvxSocketsClientInterfaceTcpUdp* parArg)
{
	parent = parArg;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsConnectionTcpUdp::disconnect()
{
	JVX_CLOSE_SOCKET(parent->theSocket);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsConnectionTcpUdp::transfer(IjvxSocketsConnection_transfer* oneBuf, jvxSize* num_sent)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxByte* buf_send = NULL;
	jvxSize buf_len = 0;
	jvxBool updateServerAddress = false;
	IjvxSocketsConnection_transfer_udp* udp_transfer = NULL;

	if (oneBuf)
	{
		res = oneBuf->get_data(&buf_send, &buf_len);
		if (res == JVX_NO_ERROR)
		{
			oneBuf->specialization(reinterpret_cast<jvxHandle**>(&udp_transfer), jvxSocketsConnectionTransfer::JVX_SOCKET_TRANSFER_UDP);
			if (udp_transfer)
			{
				updateServerAddress = (!parent->targetServerSet);
				if (!updateServerAddress)
				{
					udp_transfer->udp_force_new_target(&updateServerAddress);
				}
				if (updateServerAddress)
				{
					jvxApiString newTarget;
					int newPort;
					jvxErrorType resL = udp_transfer->udp_get_new_target(&newTarget, &newPort);
					if (resL == JVX_NO_ERROR)
					{
						switch (parent->family)
						{
						case jvxSocketsFamily::JVX_SOCKET_IP_V4:
							resL = CjvxSocketsCommon::hostNameToSockAddr(&parent->targetServer_ip4,
								newTarget.c_str(), newPort);
							break;
						case jvxSocketsFamily::JVX_SOCKET_IP_V6:
							resL = CjvxSocketsCommon::hostNameToSockAddr(&parent->targetServer_ip6,
								newTarget.c_str(), newPort);
							break;
						}

						if (resL == JVX_NO_ERROR)
						{
							parent->targetName = newTarget.std_str();
							parent->remotePort = newPort;
							parent->targetServerSet = true;
						}
					}
				}
			}

			if (buf_len)
			{
				int errCode = 0;
				if (parent->targetServerSet)
				{
					switch (parent->family)
					{
					case jvxSocketsFamily::JVX_SOCKET_IP_V4:
						errCode = sendto(parent->theSocket,
							buf_send, (int)buf_len, 0, (const sockaddr*)&parent->targetServer_ip4,
							(int)sizeof(parent->targetServer_ip4));
						break;
					case jvxSocketsFamily::JVX_SOCKET_IP_V6:
						errCode = sendto(parent->theSocket,
							buf_send, (int)buf_len, 0, (const sockaddr*)&parent->targetServer_ip6,
							(int)sizeof(parent->targetServer_ip6));
						break;
					}

					if (errCode >= 0)
					{
						if (num_sent)
						{
							*num_sent = errCode;
						}
					}
					else
					{
						res = JVX_ERROR_INTERNAL;
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
							std::cout << __FUNCTION__ << ": Error: The packet size of <" << buf_len << "> is too large." << std::endl;
							break;
						default:
							std::cout << __FUNCTION__ << ": Error: Function <sendto> failed, error: " << errCode << "." << std::endl;
						}
#else
						std::cout << __FUNCTION__ << ": Error: Function <sendto> failed, error: " << strerror(errCode) << std::endl;

#endif	
					}
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
		}
	}
	return res;
}
void
CjvxSocketsConnectionTcpUdp::main_loop()
{

	std::string ipaddr;
	std::string errorDescription;
	//std::string tmpStr  = "Unknown";
	sockaddr_in remoteAddrv4;
	sockaddr_in6 remoteAddrv6;
	//socklen_t remoteAddrLen = sizeof(JVX_SOCKADDR_IN);
	jvxSize buf_length = 0;
	jvxByte* buf_field = NULL;
	jvxSocketsErrorType errType = jvxSocketsErrorType::JVX_SOCKET_NO_ERROR;

	jvxBool reportDisconnect = false;
	JVX_POLL_SOCKET_STRUCT fdarray;

	int activity = 0;
	char oneChar;

	jvxHandle* ptrRemoteAddr = NULL;
	jvxSize szRemoteAddr = 0;

	switch (parent->family)
	{
	case jvxSocketsFamily::JVX_SOCKET_IP_V4:
		ptrRemoteAddr = &remoteAddrv4;
		szRemoteAddr = sizeof(remoteAddrv4);
		break;
	case jvxSocketsFamily::JVX_SOCKET_IP_V6:
		ptrRemoteAddr = &remoteAddrv6;
		szRemoteAddr = sizeof(remoteAddrv6);
		break;
	default:
		assert(0);
	}

	while (1)
	{
		int numFd = 1;
		fdarray.fd = parent->theSocket;
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
			JVX_IOCTRL_SOCKET_FIONREAD(parent->theSocket, &bytes_available);

			while (bytes_available)
			{
				maxNumCopy = bytes_available;
				ptrRead = NULL;
				if (theReportConnection)
				{
					theReportConnection->provide_data_and_length(&ptrRead, &maxNumCopy, &offset, NULL);
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
					readNum = recvfrom(parent->theSocket, buf_field, (int)bytes_available, 0, (sockaddr*)ptrRemoteAddr, &remoteAddrLen);

					if (parent->targetServerSet == false)
					{
						switch (parent->family)
						{
						case jvxSocketsFamily::JVX_SOCKET_IP_V4:
							parent->targetName = CjvxSocketsCommon::sockAddrRecvToHostName(
								&remoteAddrv4, parent->remotePort);
							memcpy(&parent->targetServer_ip4, &ptrRemoteAddr, remoteAddrLen);
							parent->targetServerSet = true;
							break;
						case jvxSocketsFamily::JVX_SOCKET_IP_V6:
							parent->targetName = CjvxSocketsCommon::sockAddrRecvToHostName(
								&remoteAddrv6, parent->remotePort);
							memcpy(&parent->targetServer_ip6, &ptrRemoteAddr, remoteAddrLen);
							parent->targetServerSet = true;
							break;
						}
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
						if (theReportConnection)
						{
							theReportConnection->report_connection_error(
								jvxSocketsErrorType::JVX_SOCKET_ERROR_RESOLVE_HOSTNAME,
								errorDescription.c_str());
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
							if (theReportConnection)
							{
								theReportConnection->report_data_and_read(ptrRead, readNum, offset, NULL);
								//	(jvxHandle*)theIf->config.modeSpecific.forTcpUdp.connect.targetName.c_str(), JVX_CONNECT_PRIVATE_ARG_TYPE_INPUT_STRING);
							}
							toRead -= szRead;
							offsetLocal += szRead;
							if (toRead)
							{
								maxNumCopy = toRead;
								ptrRead = NULL;
								if (theReportConnection)
								{
									theReportConnection->provide_data_and_length(
										&ptrRead, &maxNumCopy, &offset, NULL);
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
					readNum = recvfrom(parent->theSocket, ptrRead + offset, (int)szRead, 0, (sockaddr*)ptrRemoteAddr, &remoteAddrLen);// No way to mutex this

					if (parent->targetName.empty())
					{
						switch (parent->family)
						{
						case jvxSocketsFamily::JVX_SOCKET_IP_V4:
							parent->targetName = CjvxSocketsCommon::sockAddrRecvToHostName(&remoteAddrv4,
								parent->remotePort);
							memcpy(&parent->targetServer_ip4, ptrRemoteAddr, remoteAddrLen);
							parent->targetServerSet = true;
							break;
						case jvxSocketsFamily::JVX_SOCKET_IP_V6:
							parent->targetName = CjvxSocketsCommon::sockAddrRecvToHostName(&remoteAddrv6,
								parent->remotePort);
							memcpy(&parent->targetServer_ip6, ptrRemoteAddr, remoteAddrLen);
							parent->targetServerSet = true;
							break;
						}
					}

					switch (readNum)
					{
					case SOCKET_ERROR:
						errType = jvxSocketsErrorType::JVX_SOCKET_ERROR_GENERAL_CONNECTION;
#ifdef JVX_OS_WINDOWS
						readNum = WSAGetLastError();
						switch (readNum)
						{
						case WSAECONNRESET:
							errorDescription = (std::string)"Connection Reset";
							errType = jvxSocketsErrorType::JVX_SOCKET_ERROR_CONNECTION_RESET;
							break;
						default:
							errorDescription = (std::string)"Error receiving data from socket";
						}
#else
						errorDescription = (std::string)"Error receiving data from socket";
#endif

						// Report error
						if (theReportConnection)
						{
							theReportConnection->report_connection_error(errType, errorDescription.c_str());
						}
						bytes_available = 0;
						break;

					default:
						// Regular connection receive case
						if (theReportConnection)
						{
							theReportConnection->report_data_and_read(ptrRead, readNum, offset, NULL);
							// (jvxHandle*)theIf->config.modeSpecific.forTcpUdp.connect.targetName.c_str(), JVX_CONNECT_PRIVATE_ARG_TYPE_INPUT_STRING);
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
			break; // while(1) loop
		}
	} // while(1)

	if (buf_field)
	{
		JVX_DSP_SAFE_DELETE_FIELD(buf_field);
	}
}

// ===========================================================================
