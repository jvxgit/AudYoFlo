#include "CjvxSockets.h"



JVX_THREAD_ENTRY_FUNCTION(static_connect_loop_tcp_poll, this_pointer)
{
	CjvxSocketsClientInterfaceTcp* this_p = (CjvxSocketsClientInterfaceTcp*)this_pointer;
	if (this_p)
	{
		this_p->ic_connect_loop_tcp_poll();
	}
	return(0);
}

// =======================================================================

CjvxSocketsClientInterfaceTcp::CjvxSocketsClientInterfaceTcp() :
	CjvxSocketsClientInterfaceTcpUdp()
{	
}

jvxErrorType
CjvxSocketsClientInterfaceTcp::configure(
	const std::string& targetNameArg,
	int remotePortArg,
	int localPortArg,
	jvxSocketsFamily family,
	jvxSize timeout_msecs,
	jvxBool disableNagleArg)
{
	jvxErrorType res = CjvxSocketsClientInterfaceTcpUdp::configure(
		targetNameArg,
		remotePortArg,
		localPortArg,
		family,
		timeout_msecs);
	disableNagleAlgorithmTcp = disableNagleArg;
	return res;
};

jvxErrorType
CjvxSocketsClientInterfaceTcp::set_opts_socket()
{
	int flag = 1;
	int errCode = 0;
	jvxErrorType res = JVX_NO_ERROR;

	if (socketType == jvxSocketsConnectionType::JVX_SOCKET_TYPE_TCP)
	{
		if (disableNagleAlgorithmTcp)
		{
#ifdef JVX_OS_WINDOWS
			flag = 1;
			errCode = setsockopt(theSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
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
			errCode = setsockopt(theSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));    /* length of option value */
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
CjvxSocketsClientInterfaceTcp::connect_socket()
{
	jvxErrorType res = JVX_NO_ERROR;
	int errCode = 0;
	jvxBool connectMe = false;

	if (JVX_CHECK_SIZE_SELECTED(remotePort))
	{
		// =================================================================================
		// LOOKUP TARGET SERVER NAME
		// =================================================================================
		switch (family)
		{
		case jvxSocketsFamily::JVX_SOCKET_IP_V4:
			res = CjvxSocketsCommon::hostNameToSockAddr(
				&targetServer_ip4,
				targetName.c_str(),
				remotePort);
			break;
		case jvxSocketsFamily::JVX_SOCKET_IP_V6:
			res = CjvxSocketsCommon::hostNameToSockAddr(&targetServer_ip6,
				targetName.c_str(),
				remotePort);
			break;
		}	
		connectMe = true;

		if (res != JVX_NO_ERROR)
		{
#ifdef JVX_OS_WINDOWS
			int errCode = WSAGetLastError();
			switch (errCode)
			{
			case WSANOTINITIALISED:
				last_error = "A successful WSAStartup call must occur before using this function.";
				break;
			case WSAENETDOWN:
				last_error = "The network subsystem has failed.";
				break;
			case WSAHOST_NOT_FOUND:
				last_error = "An authoritative answer host was not found.";
				break;
			case WSATRY_AGAIN:
				last_error = "A nonauthoritative host was not found, or the server failure.";
				break;
			case WSANO_RECOVERY:
				last_error = "A nonrecoverable error occurred.";
				break;
			case WSANO_DATA:
				last_error = "A valid name exists, but no data record of the requested type exists.";
				break;
			case WSAEINPROGRESS:
				last_error = "A blocking Winsock call is in progress, or the service provider is still processing a callback function.";
				break;
			case WSAEFAULT:
				last_error = "The name parameter is not a valid part of the user address space.";
				break;
			case WSAEINTR:
				last_error = "The socket was closed.";
				break;
			}
#else
			last_error = (std::string)"Was not able to get server by hostname, error: " + strerror(errno);
#endif
		}
	}

	if (res == JVX_NO_ERROR)
	{
		res = CjvxSocketsClientInterfaceTcpUdp::connect_socket();
	}

	if (res == JVX_NO_ERROR)
	{
		// ==================================================================================
		// Connect to target server..
		// ==================================================================================
		errCode = 0;
			
		if (connectMe)
		{
			errCode = CjvxSocketsCommon::connect_timeout(theSocket,
				(const sockaddr*)&targetServer_ip4,
				sizeof(targetServer_ip4),
				timeout_msecs); // <- we can not put this in mutexes to avoid blockign
		}

		if (errCode != 0)
		{
			res = JVX_ERROR_INTERNAL;

			jvxSocketsErrorType resC = jvxSocketsErrorType::JVX_SOCKET_NO_ERROR;
#ifdef JVX_OS_WINDOWS
			// Here, res already contains the error code, additional error code -1 for timeout
			// Check that connection was successful
			if ((errCode == -1) || (errCode == WSAETIMEDOUT))
			{
				resC = jvxSocketsErrorType::JVX_SOCKET_ERROR_TIMEOUT;
				last_error = "Connection could not be established due to timeout.";
			}
			else if (errCode == WSAECONNREFUSED)
			{
				resC = jvxSocketsErrorType::JVX_SOCKET_ERROR_REFUSED;
				last_error = "Connection was refused.";
			}
			else if (errCode == WSAENOTSOCK)
			{
				resC = jvxSocketsErrorType::JVX_SOCKET_ERROR_ENDED;
				last_error = "Connection ended.";
			}
			else
			{
				resC = jvxSocketsErrorType::JVX_SOCKET_ERROR_MISC;
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
			}


#else
			if (errCode == -1)
			{
				resC = jvxSocketsErrorType::JVX_SOCKET_ERROR_TIMEOUT;
			}
			else
			{
				last_error = (std::string)"Was not able to connect to server, error: " + strerror(errno);
				resC = jvxSocketsErrorType::JVX_SOCKET_ERROR_ENDED;
			}
#endif
		} // if (errCode != 0)
	}
	return res;
}

jvxErrorType
CjvxSocketsClientInterfaceTcp::start_connection_loop()
{
	// Create thread:
	JVX_CREATE_THREAD(theThreadHdl,
		static_connect_loop_tcp_poll,
		reinterpret_cast<jvxHandle*>(this),
		theThreadId);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSocketsClientInterfaceTcp::stop_connection_loop()
{
	JVX_LOCK_MUTEX(safeAccessConnection);
	if (theConnection)
	{
		theConnection->disconnect();
	}
	JVX_UNLOCK_MUTEX(safeAccessConnection);
	return CjvxSocketsClientInterfaceTcpUdp::stop_connection_loop();
}

void
CjvxSocketsClientInterfaceTcp::ic_connect_loop_tcp_poll()
{
	CjvxSocketsConnectionTcp* theConnectionTcp = NULL;

	JVX_DSP_SAFE_ALLOCATE_OBJECT(theConnectionTcp, CjvxSocketsConnectionTcp);
	theConnection = static_cast<CjvxSocketsConnection*>(theConnectionTcp);
	
	theConnectionTcp->configure(this);

	if (theReportClient)
	{
		theReportClient->report_client_connect(static_cast<IjvxSocketsConnection*>(theConnection));
	}

	theConnection->main_loop();

	if (theReportClient)
	{
		theReportClient->report_client_disconnect(static_cast<IjvxSocketsConnection*>(theConnection));
	}

	// This must be protected if the socket is terminated from both sides at the same time
	JVX_LOCK_MUTEX(safeAccessConnection);
	JVX_DSP_SAFE_DELETE_OBJECT(theConnectionTcp);
	theConnectionTcp = NULL;
	theConnection = NULL;
	JVX_UNLOCK_MUTEX(safeAccessConnection);

	// Do we need to put this into a critical section? 
	// I do not think so since we have to stop before we reconnect anyway
	theState = jvxSocketsConnectionState::JVX_STATE_CONNECTION_DISCONNECTED;
}

// ===========================================================================

CjvxSocketsConnectionTcp::CjvxSocketsConnectionTcp()
{
#ifdef JVX_WINDOWS_SOCKET_IMPL
	JVX_INITIALIZE_NOTIFICATION(notifyEventLoop);
	running = true;
#endif
}

CjvxSocketsConnectionTcp::~CjvxSocketsConnectionTcp()
{
#ifdef JVX_WINDOWS_SOCKET_IMPL	
	JVX_INITIALIZE_NOTIFICATION(notifyEventLoop);
#endif
}

jvxErrorType
CjvxSocketsConnectionTcp::configure(CjvxSocketsInterfaceTcpUdp* parArg)
{
	parent = parArg;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsConnectionTcp::disconnect()
{
#ifdef JVX_WINDOWS_SOCKET_IMPL
	running = false;
	JVX_SET_NOTIFICATION(notifyEventLoop);
#else
	JVX_SHUTDOWN_SOCKET(parent->theSocket, JVX_SOCKET_SD_BOTH);
	JVX_CLOSE_SOCKET(parent->theSocket);
#endif
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsConnectionTcp::transfer(IjvxSocketsConnection_transfer* oneBuf, jvxSize* num_sent)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxByte* buf_send = NULL;
	jvxSize buf_len = 0;
	if (oneBuf)
	{
		res = oneBuf->get_data(&buf_send, &buf_len);
		if (res == JVX_NO_ERROR)
		{
			if (buf_len)
			{
				int errCode = 0;
				errCode = send(parent->theSocket, buf_send, (int)buf_len, 0);
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
		}
	}
	return res;
}

/*
void
CjvxSocketsConnectionTcp::report_thread_stopped()
{
}
*/
void 
CjvxSocketsConnectionTcp::main_loop()
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

#ifdef JVX_WINDOWS_SOCKET_IMPL
	/*
	 * The started socket connections all live in a dedicated thread.
	 * If that thread stops by, I want to remove the list of active connections.
	 * To do so, I get back into this thread by triggering the secondary event.
	 * Also, windows does not raise any signal if I close the socket.
	 * Therefore, I need to trigger this thread via the secondary event
	 */
	 // https://comp.os.ms-windows.programmer.win32.narkive.com/xMrAM6RU/waiting-for-sockets-and-objects-at-the-same-time
	WSAEVENT hsockevent = WSACreateEvent();
	WSANETWORKEVENTS events;
	DWORD rese = WSAEventSelect(parent->theSocket, hsockevent, FD_READ | FD_CLOSE);
	HANDLE handles[2] = { hsockevent, notifyEventLoop };

	while (running)
	{
		DWORD resw = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		if (resw == WAIT_OBJECT_0 + 1)
		{
			break;
		}
		if (resw == WAIT_OBJECT_0)
		{
			WSAEnumNetworkEvents(parent->theSocket, hsockevent, &events);
			if (
				(events.lNetworkEvents & FD_CLOSE))
			{
				//report_thread_stopped();
				reportDisconnect = true;
			}
			else
			{
				if (
					(events.lNetworkEvents & FD_READ))
				{
					reportDisconnect = operate_one_read();
				}
				else
				{
					// This case may happen for whatever reason. 
					// There is an intersting discussion on this issue here:
					// https://alt.winsock.programming.narkive.com/dEQlwjuX/wsaenumnetworkevents-returns-no-event-how-is-this-possible
					// The conclusion: just ignore it, it may be triggered due to unknown reason ("spurious wakeup")
					// std::cout << "Read 0 value" << std::endl;
				}
			}
		}
		if (reportDisconnect)
		{
			break; // while(1)
		}
	} // while(1)
#else
	// Docs on the Linux shutdown behavior:
	// https://stackoverflow.com/questions/5039608/poll-cant-detect-event-when-socket-is-closed-locally

	JVX_POLL_SOCKET_STRUCT fdarray;

	while (1)
	{
		fdarray.events = JVX_SOCKET_POLL_INIT_DEFAULT;
			// POLLIN | POLLERR | POLLHUP | POLLNVAL;
		fdarray.fd = parent->theSocket;
		fdarray.revents = 0;

		// Check socket in blocking manner
		activity = JVX_POLL_SOCKET_INF(&fdarray, 1);

		/* 
		 A hang up on the other side is reported by
		 JVX_POLL_SOCKET_RDNORM | JVX_POLL_SOCKET_HUP
		 The hup has higher priority though since it will
		 stop immediately!

		 On UNIX based systems, the HUP may also be reported 
		 by indicating the POLLIN event rather than the POLLHUP.
		 If that is the case, a following number of bytes of 0
		 that might be read indicate the end of connection
		*/

		if (
			(fdarray.revents & JVX_POLL_SOCKET_ERR) ||
			(fdarray.revents & JVX_POLL_SOCKET_HUP) ||
			(fdarray.revents & JVX_POLL_SOCKET_NVAL))
		{
			reportDisconnect = true;
		}
		else
		{
			if (fdarray.revents & JVX_POLL_SOCKET_RDNORM)
			{
				reportDisconnect = operate_one_read();
			}	
		}

		if (reportDisconnect)
		{
			break;
		}
	}
#endif
}

jvxBool
CjvxSocketsConnectionTcp::operate_one_read()
{
	jvxBool reportDisconnect = false;
	char oneChar; 
	char* ptrRead;
	jvxSize maxNumCopy = 0;
	jvxSize szRead = 0;
	int readNum = 0;
	jvxSize offset = 0;
	jvxSize repetitions = 0;
	jvxBool repeatThis = true; // Run this at least once!!

	while (repeatThis)
	{
		repeatThis = false;
		JVX_SOCKET_NUM_BYTES bytes_available = 0;
		int ioerr = JVX_IOCTRL_SOCKET_FIONREAD(parent->theSocket, &bytes_available);

		if (bytes_available == 0)
		{
			// https://github.com/micropython/micropython/issues/3747
			/*
			In case where you're waiting for data to be received (POLLIN) and connection gets terminated by peer - you'll receive POLLIN event followed by zero sized recv().
			This is common behavior for *unix base system, you can check it by man recv..
			*/
			reportDisconnect = true;
		}
		else
		{
			while (bytes_available)
			{
				maxNumCopy = bytes_available;
				ptrRead = NULL;
				offset = 0;

				if (theReportConnection)
				{
					theReportConnection->provide_data_and_length(&ptrRead, &maxNumCopy, &offset, NULL);
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

				szRead = maxNumCopy;
				assert(szRead > 0);
				readNum = recv(parent->theSocket, ptrRead + offset, JVX_SIZE_INT(szRead), 0);
				if (readNum > 0)
				{
					if (theReportConnection)
					{
						theReportConnection->report_data_and_read(ptrRead, readNum, offset,
							NULL);
					}
					bytes_available -= (JVX_SOCKET_NUM_BYTES)readNum;
				}
				else
				{
					/*
					 * No error reported to application
					 */
					 /*
					 if (theReportConnection)
					 {
						 theReportConnection->report_data_and_read(ptrRead, readNum, offset,
							 NULL);
					 }
					 */
#ifdef JVX_OS_WINDOWS
					int errCode = WSAGetLastError();
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

						// we use a special rule: if additional bytes landed in the input receive buffer while we are processing, the assumed buffersize is no longer
						// correct. We need to run this function again to receive the "larger" buffer. If this happens again and again, there is a structural problem.
						// Therefore, we count the repetitions and stop after 10 iterations in a row!
						// I see this event occuring mostly when using a debugger to slow down the receiver loop. Then, it makes sense to repeat to recover from the
						// interruption. -- HK, 22102023
						std::cout << __FUNCTION__ << ": Warning: A data transfer requires a larger buffersize, repetiton = <" << repetitions << ">." << std::endl;
						if (repetitions < 10)
						{
							repeatThis = true;
						}
						repetitions++;
						break;
					default:
						std::cout << __FUNCTION__ << ": Error: Function <recv> failed, error: " << errCode << "." << std::endl;
					}
#else
					std::cout << __FUNCTION__ << ": Error: Function <sendto> failed, error: " << strerror(errno) << std::endl;

#endif	
					if (!repeatThis)
					{
						reportDisconnect = true;
					}
					break;
				}
			} // while (bytes_available)
		}
	}
	return reportDisconnect;
}
