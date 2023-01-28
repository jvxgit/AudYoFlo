
#include "CjvxSockets.h"

#define JVX_SOCKET_SERVER_VERBOSE

// =====================================================================================

JVX_THREAD_ENTRY_FUNCTION(jvx_connect_server_thread_entry, lp)
{
	std::string errorDescription;
	CjvxSocketsServer* theServer = (CjvxSocketsServer*)lp;
	theServer->connect_loop_tcp_poll();
	return 0;
}

JVX_THREAD_ENTRY_FUNCTION(jvx_single_connection_loop, lp)
{
	CjvxSocketsServerConnection* this_pointer = (CjvxSocketsServerConnection*)lp;
	if (this_pointer)
	{
		this_pointer->ic_single_connection_loop();
	}
	return 0;
}

CjvxSocketsServer::CjvxSocketsServer()
{

	JVX_INITIALIZE_MUTEX(safeAccess);
	JVX_INITIALIZE_NOTIFICATION_2WAIT(notifyEventLoop);
}

CjvxSocketsServer::~CjvxSocketsServer()
{
	JVX_TERMINATE_MUTEX(safeAccess);
	JVX_TERMINATE_NOTIFICATION_2WAIT(notifyEventLoop);
}

// ===========================================================================

void 
CjvxSocketsServer::connect_loop_tcp_poll()
{
	
	std::string ipaddr;
	std::string errorDescription;
	jvxSize i, y = 0;
	jvxErrorType resL = JVX_NO_ERROR;

#ifdef JVX_OS_WINDOWS

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
	DWORD rese = WSAEventSelect(theSocket, hsockevent, FD_ACCEPT /*| FD_CLOSE*/);
	HANDLE handles[2] = { hsockevent, notifyEventLoop };

	while (running)
	{
		DWORD resw = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		if (resw == WAIT_OBJECT_0 +1)
		{
			accept_other_event();
		}
		if(resw == WAIT_OBJECT_0)
		{
			WSAEnumNetworkEvents(theSocket, hsockevent, &events);
			if (
				(events.lNetworkEvents & FD_CLOSE))
			{
				break;
			}
			else
			{
				if (
					(events.lNetworkEvents & FD_ACCEPT))
				{
					// Only if the "accept" is the reason, we accept!
					resL = accept_new_socket(errorDescription);
					if (resL != JVX_NO_ERROR)
					{
						std::cout << __FUNCTION__ << ": function <accept_new_socket> has failed, reason: <" << errorDescription << std::endl;
					}
				}
			} // if(FD_ISSET(theHandle->runtime.theSocket, &readfds))
		}
	} // while(1)

	WSACloseEvent(hsockevent);
#else

	//
	// This code fragment works as well but it is important to remove the connection handle from within
	// this thread here to be able to wait for the connection loop to run out. Otherwise we only can run 
	// accept_other_event from the callback in the connection loop - which is a deadlock!
	// 
#ifdef JVX_OS_LINUX
	JVX_FILE_DESCRIPTOR_HANDLE  epollfd = JVX_INVALID_FILE_DESCRIPTOR_VALUE;
	struct epoll_event ev, evdown;
	int status = 0;
	int nfds = 0;
	struct epoll_event events[2] = {0};

	epollfd = epoll_create1(0);
	assert(epollfd >= 0);
	
	ev.data.fd = theSocket;
	ev.events = EPOLLIN | EPOLLHUP; // no event EPOLLOUT here
	status = epoll_ctl(epollfd, EPOLL_CTL_ADD, theSocket, &ev);
	assert(status >= 0);
		
	evdown.data.fd = notifyEventLoop;
	evdown.events = EPOLLIN | EPOLLET;
	status = epoll_ctl(epollfd, EPOLL_CTL_ADD, notifyEventLoop, &evdown);
	assert(status >= 0);
#elif defined( JVX_OS_MACOSX) || defined(JVX_OS_IOS)
	int nfds = 0;
	JVX_FILE_DESCRIPTOR_HANDLE  epollfd = kqueue();
	struct kevent events[2] = {0};
	struct kevent monitors[2] = {0};
#else

#error Ended up here for an unknown OS type

#endif

	while (running)
	{
		nfds = JVX_EVENT_WAIT_2WAIT(epollfd, monitors, 2, events, 2,
						    JVX_SIZE_INT(JVX_INFINITE_MS));

		if (nfds < 0)
		{
			// This indicates an error
		}
		else if(nfds == 0)
		{
			// No file descriptor fired, timeout!
			std::cout << __FUNCTION__ << ": Warning: Unexpected timeout." << std::endl;				
		}
		else 
		{
			for(i = 0; i < nfds; i++)
			{
#ifdef JVX_OS_LINUX
				if (events[i].data.fd == theSocket)
#else
				if (events[i].ident == theSocket)
#endif
				{
					resL = accept_new_socket(errorDescription);
					if (resL != JVX_NO_ERROR)
					{
						std::cout << __FUNCTION__ << ": function <accept_new_socket> has failed, reason: <" << errorDescription << std::endl;
					}
				}

#ifdef JVX_OS_LINUX
				if(events[i].data.fd == notifyEventLoop)
#else
				if(events[i].ident == notifyEventLoop[0])
#endif
				{
					accept_other_event();
				}
			}
		}
	}

	/*
	JVX_POLL_SOCKET_STRUCT* fdarray = new JVX_POLL_SOCKET_STRUCT[1];
	while (1)
	{

		int numFd = 1;
		fdarray[0].fd = theSocket;
		fdarray[0].events = JVX_SOCKET_POLL_INIT_DEFAULT;
		fdarray[0].revents = 0;

		// Check socket in blocking manner
		int activity = JVX_POLL_SOCKET_INF(fdarray, numFd); if (fdarray[0].revents != 0)
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
			}
		}
	}

	delete[](fdarray);
	*/
#endif

}

jvxErrorType 
CjvxSocketsServer::accept_other_event()
{
	jvxBool runLoop = true;
	while (runLoop)
	{
		CjvxSocketsServerConnection* elmRemove = NULL;
		JVX_LOCK_MUTEX(safeAccess);
		auto elm = active_connections.begin();
		for (; elm != active_connections.end(); elm++)
		{
			if((*elm)->operPen == CjvxSocketsServerConnection::jvxSocketsServerPending::JVX_SOCKETS_PENDING_CLEAR)
			{
				elmRemove = (*elm);
				active_connections.erase(elm);
				break;
			}
		}
		JVX_UNLOCK_MUTEX(safeAccess);

		if (elmRemove)
		{
			JVX_WAIT_FOR_THREAD_TERMINATE_INF(elmRemove->threadHdl);
			JVX_SHUTDOWN_SOCKET(elmRemove->socket->theSocket, JVX_SOCKET_SD_BOTH);
			JVX_CLOSE_SOCKET(elmRemove->socket->theSocket);
			JVX_DSP_SAFE_DELETE_OBJECT(elmRemove->socket);
			JVX_DSP_SAFE_DELETE_OBJECT(elmRemove);
		}
		else 
		{
			runLoop = false;
		}
	}

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsServer::accept_new_socket(std::string& errorDescription)
{
	jvxErrorType res = JVX_NO_ERROR;
	sockaddr_in remoteAddrv4;
	sockaddr_in6 remoteAddrv6;
	sockaddr_un remoteAddrun;
	jvxHandle* ptrRemoteAddr = NULL;
	jvxSize szRemoteAddr = 0;
	socklen_t clilen;
	int myPort = 0;

	switch (family)
	{
	case jvxSocketsFamily::JVX_SOCKET_IP_V4:
		ptrRemoteAddr = &remoteAddrv4;
		szRemoteAddr = sizeof(remoteAddrv4);
		break;
	case jvxSocketsFamily::JVX_SOCKET_IP_V6:
		ptrRemoteAddr = &remoteAddrv6;
		szRemoteAddr = sizeof(remoteAddrv6);
		break;
	case jvxSocketsFamily::JVX_SOCKET_IP_UNIX:
		ptrRemoteAddr = &remoteAddrun;
		szRemoteAddr = sizeof(remoteAddrun);
		break;
	default:
		assert(0);
	}

	memset(ptrRemoteAddr, 0, szRemoteAddr);
	clilen = JVX_SIZE_INT(szRemoteAddr);
	
	// New incoming connection or listening stopped
	JVX_SOCKET newSocket = accept(theSocket, (struct sockaddr*)ptrRemoteAddr, (socklen_t*)&clilen);// < we can not put this in mutex to avoid blocking

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
			if (theReport)
			{
				theReport->report_server_error(
					jvxSocketsErrorType::JVX_SOCKET_ERROR_ACCEPT_FAILED,
					errorDescription.c_str());
			}

			res = JVX_ERROR_INTERNAL;
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
			if (theReport)
			{
				theReport->report_server_error(
					jvxSocketsErrorType::JVX_SOCKET_ERROR_ACCEPT_FAILED,
					errorDescription.c_str());
			}
			res = JVX_ERROR_INTERNAL;
		}
	}
	else // if(newSocket == INVALID_SOCKET or ERROR)
	{
		// Add new incoming connection to connection list
		jvxSize newId = JVX_SIZE_UNSELECTED;
		jvxSize reportId = JVX_SIZE_UNSELECTED;
		std::string connectionName;
		switch (family)
		{
		case jvxSocketsFamily::JVX_SOCKET_IP_V4:
			connectionName = CjvxSocketsCommon::sockAddrRecvToHostName((sockaddr_in*)ptrRemoteAddr, myPort);
			connectionName += "<ipv4," + jvx_int2String(myPort) + ">";
			break;
		case jvxSocketsFamily::JVX_SOCKET_IP_V6:
			CjvxSocketsCommon::sockAddrRecvToHostName((sockaddr_in6*)ptrRemoteAddr, myPort);
			connectionName += "<ipv6" + jvx_int2String(myPort) + ">";
			break;
		case jvxSocketsFamily::JVX_SOCKET_IP_UNIX:
			// Nothing to do here	
			connectionName = "Unix socket via " + fnode_unix;
			myPort = 0;
			while (1)
			{
				std::string connectionNameTest = connectionName + "<" + jvx_int2String(myPort) + ">";
				auto elm = active_connections.begin();
				for (; elm != active_connections.end(); elm++)
				{
					if ((*elm)->description == connectionNameTest)
					{
						break;
					}
				}
				if (elm != active_connections.end())
				{
					myPort++;
				}
				else
				{
					break;
				}
			}
			connectionName += "<" + jvx_int2String(myPort) + ">";
			break;
		default:
			assert(0);
		}

		if (
			(theState == jvxSocketServerState::JVX_STATE_SERVER_SHUTTING_DOWN) ||
			(active_connections.size() >= numMaxConnections) ||
			(lockIncoming))
		{
			// Reject new connection
			if (theReport)
			{
				theReport->report_server_denied(connectionName.c_str());
			}
			JVX_SHUTDOWN_SOCKET(newSocket, JVX_SOCKET_SD_BOTH);
			JVX_CLOSE_SOCKET(newSocket);
		}
		else
		{
			// New / removed connections always come within this thread, therefore, no safe access required
			// Valid new socket
			CjvxSocketsServerConnection* newConnection = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(newConnection, CjvxSocketsServerConnection);

			newConnection->description = connectionName;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(newConnection->socket, CjvxSocketsInterfaceTcpUdp);
			newConnection->theReport = static_cast<IjvxSocketsServer_report*>(this);
			newConnection->socket->theSocket = newSocket;

			if (
				(family == jvxSocketsFamily::JVX_SOCKET_IP_V4) ||
				(family == jvxSocketsFamily::JVX_SOCKET_IP_V6))
			{
				if (disableNagleAlgorithmTcp)
				{
					int flag = 1;
					int errCode = setsockopt(newConnection->socket->theSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
					assert(errCode == 0);
				}
			}

			JVX_CREATE_THREAD(newConnection->threadHdl,
				jvx_single_connection_loop,
				reinterpret_cast<jvxHandle*>(newConnection),
				newConnection->threadId);

			if (connection_high_prio)
			{
				JVX_SET_THREAD_PRIORITY(newConnection->threadHdl, JVX_THREAD_PRIORITY_REALTIME);
			}

			JVX_LOCK_MUTEX(safeAccess);
			active_connections.push_back(newConnection);
			JVX_UNLOCK_MUTEX(safeAccess);
		}
	}// if(newSocket == INVALID_SOCKET)
	return res;
}

jvxErrorType 
CjvxSocketsServer::start(IjvxSocketsServer_report* theReportArg,
	jvxBool disableNagleAlgorithmTcpArg,
	jvxSize localPortArg,
	jvxSocketsFamily familyArg,
	jvxSize numMaxConnectionsArg,
	jvxBool highPrioThreads)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	std::string errDescr;
	jvxSocketsErrorType errId = jvxSocketsErrorType::JVX_SOCKET_NO_ERROR;
	if (theState == jvxSocketServerState::JVX_STATE_SERVER_INIT)
	{
		theState = jvxSocketServerState::JVX_STATE_SERVER_STARTED;
		res = JVX_NO_ERROR;
	}

	// Start socket thread
	if (res == JVX_NO_ERROR)
	{
		theReport = theReportArg;
		disableNagleAlgorithmTcp = disableNagleAlgorithmTcpArg;
		localPort = localPortArg;
		family = familyArg;
		numMaxConnections = numMaxConnectionsArg;
		connection_high_prio = highPrioThreads;

		res = startup_core(errDescr, errId);

		if (res == JVX_NO_ERROR)
		{

			running = true;

			JVX_CREATE_THREAD(threadHdlListen,
				jvx_connect_server_thread_entry,
				reinterpret_cast<jvxHandle*>(this),
				threadIdListen);
		}
		else
		{
			this->stop();
		}
	}
	return res;
}

jvxErrorType 
CjvxSocketsServer::start(IjvxSocketsServer_report* theReportArg,
	jvxBool disableNagleAlgorithmTcpArg,
	const std::string& fnodeArg,
	jvxSize numMaxConnectionsArg,
	jvxBool highPrioThreads)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	std::string errDescr;
	jvxSocketsErrorType errId = jvxSocketsErrorType::JVX_SOCKET_NO_ERROR;

	if (theState == jvxSocketServerState::JVX_STATE_SERVER_INIT)
	{
		fileCreated = false;
		theState = jvxSocketServerState::JVX_STATE_SERVER_STARTED;
		res = JVX_NO_ERROR;
	}

	// Start socket thread
	if (res == JVX_NO_ERROR)
	{
		theReport = theReportArg;
		disableNagleAlgorithmTcp = disableNagleAlgorithmTcpArg;
		localPort = JVX_SIZE_UNSELECTED;
		family = jvxSocketsFamily::JVX_SOCKET_IP_UNIX;
		numMaxConnections = numMaxConnectionsArg;
		fnode_unix = fnodeArg;
		connection_high_prio = highPrioThreads;

		res = startup_core(errDescr, errId);

		if (res == JVX_NO_ERROR)
		{

			running = true;

			JVX_CREATE_THREAD(threadHdlListen,
				jvx_connect_server_thread_entry,
				reinterpret_cast<jvxHandle*>(this),
				threadIdListen);
		}
		else
		{
			this->stop();
			theState = jvxSocketServerState::JVX_STATE_SERVER_INIT;
		}
	}
	return res;
}

jvxErrorType
CjvxSocketsServer::lock_incoming(jvxBool lockit)
{
	lockIncoming = lockit;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSocketsServer::startup_core(std::string& errorDescription,
	jvxSocketsErrorType& errId)
{
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_OS_WINDOWS
	/* initialize TCP for Windows ("winsock") */
	/* this is not necessary for GNU systems  */
	short wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	int resSocket = WSAStartup(wVersionRequested, &wsaData);
	assert(resSocket == 0);
#endif

	myMainThreadId = JVX_GET_CURRENT_THREAD_ID();
	struct sockaddr_in my_addr_v4;
	struct sockaddr_in6 my_addr_v6;
	struct sockaddr_un my_addr_un;

	bool errorDetected = false;
	jvxBool waitForConnectionThread = false;
	int errCode = 0;
	int flag = 0;
	jvxSize len = 0;
	jvxSize copyLen = 0;
	jvxBool reportDisconnect = false;
	int proto = IPPROTO_TCP;
	jvxBool setOptions = true;

#ifdef JVX_OS_WINDOWS
	char reuseaddress = 0;
#else
	int reuseaddress = 0;
#endif
	// Init the socket

	int family_int = 0;
	switch (family)
	{
	case jvxSocketsFamily::JVX_SOCKET_IP_V4:
		family_int = AF_INET;
		break;
	case jvxSocketsFamily::JVX_SOCKET_IP_V6:
		family_int = AF_INET6;
		break;
	case jvxSocketsFamily::JVX_SOCKET_IP_UNIX:
		family_int = AF_UNIX;
		proto = PF_UNSPEC;
		setOptions = false;
		abstractFileMode = false;
		if (this->fnode_unix[0] == ' ')
		{
			abstractFileMode = true;
		}
		else
		{
			if (JVX_FILE_EXISTS(this->fnode_unix.c_str()))
			{
				std::cout << __FUNCTION__ << ": Warning: Socket file <" << this->fnode_unix << "> already exists, removing it!" << std::endl;
				JVX_FILE_REMOVE(this->fnode_unix.c_str());
			}
		}
		break;
	default:
		assert(0);
	}

	// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
	// Create socket
	// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
	theSocket = socket(family_int, SOCK_STREAM, proto);//IPPROTO_TCP

	if ((theSocket == INVALID_SOCKET) || (theSocket == SOCKET_ERROR))
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
		errId = jvxSocketsErrorType::JVX_SOCKET_ERROR_INIT_FAILED;
		res = JVX_ERROR_INTERNAL;
		goto exit_error;
	}

	if (setOptions)
	{
		// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
		// Set socket options
		// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#ifdef JVX_OS_WINDOWS

	// Set some parameters for the socket
		reuseaddress = 1;
		errCode = setsockopt(theSocket, SOL_SOCKET, SO_REUSEADDR, &reuseaddress, sizeof(char));

		if (errCode != 0)
		{
			errCode = WSAGetLastError();
			switch (errCode)
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
		errCode = setsockopt(theSocket, SOL_SOCKET, SO_REUSEADDR, &reuseaddress, sizeof(int));
		if (errCode < 0)
		{
			errorDescription = (std::string)"Was not able to set socket options, Error: " + strerror(errno);
			errorDetected = true;
		}
#endif

		if (errorDetected)
		{
			errId = jvxSocketsErrorType::JVX_SOCKET_ERROR_SETOPT_I_FAILED;
			res = JVX_ERROR_INTERNAL;
			goto exit_error_close;
		}

		if (disableNagleAlgorithmTcp)
		{
#ifdef JVX_OS_WINDOWS

			int flag = 1;
			int errCode = setsockopt(theSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
			if (errCode != 0)
			{
				errCode = WSAGetLastError();
				switch (errCode)
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
			errCode = setsockopt(theSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));    /* length of option value */
			if (errCode < 0)
			{
				errorDescription = (std::string)"Was not able to set socket options, Error: " + strerror(errno);
				errorDetected = true;
			}

#endif

			if (errorDetected)
			{
				errId = jvxSocketsErrorType::JVX_SOCKET_ERROR_SETOPT_I_FAILED;
				res = JVX_ERROR_INTERNAL;
				goto exit_error_close;
			}
		}
	}


	// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
	// Bind
	// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
	switch (family)
	{
	case jvxSocketsFamily::JVX_SOCKET_IP_V4:
		len = sizeof(my_addr_v4);
		memset((char*)&my_addr_v4, 0, len);

		// Specify listening port
		my_addr_v4.sin_family = AF_INET;
		my_addr_v4.sin_addr.s_addr = htonl(INADDR_ANY);
		my_addr_v4.sin_port = htons((u_short)localPort);

		// ==================================================================
		// Bind the new socket
		// ==================================================================
		errCode = bind(theSocket, (struct sockaddr*)&my_addr_v4, (int)len);
		break;
	case jvxSocketsFamily::JVX_SOCKET_IP_V6:
		len = sizeof(my_addr_v6);
		memset((char*)&my_addr_v6, 0, len);

		// Specify listening port
		my_addr_v6.sin6_family = AF_INET6;
		my_addr_v6.sin6_addr = in6addr_any;
		my_addr_v6.sin6_port = htons((u_short)localPort);

		// ==================================================================
		// Bind the new socket
		// ==================================================================
		errCode = bind(theSocket, (struct sockaddr*)&my_addr_v6, (int)len);
		break;
	case jvxSocketsFamily::JVX_SOCKET_IP_UNIX:
		/*
		https://stackoverflow.com/questions/26757306/delete-af-unix-file-automatically
		https://devblogs.microsoft.com/commandline/windowswsl-interop-with-af_unix/
		https://github.com/microsoft/WSL/issues/4240
		*/
		len = sizeof(my_addr_un);
		memset((char*)&my_addr_un, 0, len);
		copyLen = sizeof(my_addr_un.sun_path) - 1;// ARRAYSIZE(my_addr_un.sun_path) - 1;
		copyLen = JVX_MIN(copyLen,
			this->fnode_unix.size());
		// Specify listening port
		
		my_addr_un.sun_family = AF_UNIX;
		memcpy(&my_addr_un.sun_path, this->fnode_unix.c_str(), copyLen);

		// Make it abstract and non visible!
		if (abstractFileMode)
		{
#ifdef JVX_SOCKET_SERVER_VERBOSE
			std::cout << "Creating UNIX socket in ABSTRACT file <" << this->fnode_unix << ">." << std::endl;
#endif
			my_addr_un.sun_path[0] = 0;
		}
		else
		{
#ifdef JVX_SOCKET_SERVER_VERBOSE
			std::cout << "Creating UNIX socket in file <" << this->fnode_unix << ">." << std::endl;
#endif
		}

		// ==================================================================
		// Bind the new socket
		// ==================================================================
		errCode = bind(theSocket, (struct sockaddr*)&my_addr_un, (int)len);
		break;
	default:
		assert(0);
	}

	if (errCode != 0)
	{
#ifdef JVX_OS_WINDOWS
		errCode = WSAGetLastError();
		switch (errCode)
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
		errorDetected = true;
		errId = jvxSocketsErrorType::JVX_SOCKET_ERROR_BIND_FAILED;
		res = JVX_ERROR_INTERNAL;
		goto exit_error_close;
	}

	if (!abstractFileMode && (family == jvxSocketsFamily::JVX_SOCKET_IP_UNIX))
	{
		fileCreated = true;
	}

	// Start to listen to incoming connections
	errCode = listen(theSocket, SOMAXCONN);// <- we can not safe this by a mutex since otherwise we would block everything
	if (errCode != 0)
	{
#ifdef JVX_OS_WINDOWS
		errCode = WSAGetLastError();
		switch (errCode)
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
		errorDetected = true;
		errId = jvxSocketsErrorType::JVX_SOCKET_ERROR_LISTEN_FAILED;
		res = JVX_ERROR_INTERNAL;
		goto exit_error_close;
	}

	theState = jvxSocketServerState::JVX_STATE_SERVER_LISTENING;
	return res;

exit_error_close:
	JVX_SHUTDOWN_SOCKET(theSocket, JVX_SOCKET_SD_BOTH);
	JVX_CLOSE_SOCKET(theSocket);
exit_error:
	return res;
}

// ==============================================================

jvxErrorType
CjvxSocketsServer::stop()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theState != jvxSocketServerState::JVX_STATE_SERVER_INIT)
	{
		if (myMainThreadId != JVX_GET_CURRENT_THREAD_ID())
		{
			std::cout << __FUNCTION__ << "Warning: Call from non-main thread detected." << std::endl;
		}

		JVX_WAIT_RESULT resL = JVX_WAIT_SUCCESS;
		
		// Disconnect all existing connections
		theState = jvxSocketServerState::JVX_STATE_SERVER_SHUTTING_DOWN;

		// Trigger all connections to disconnect
		JVX_LOCK_MUTEX(safeAccess);
		auto elm = active_connections.begin();
		for (; elm != active_connections.end(); elm++)
		{
			if ((*elm)->connection)
			{
				// When may this be still on:
				// Each 
				(*elm)->connection->disconnect();
			}

		}
		JVX_UNLOCK_MUTEX(safeAccess);

		// Wait until all connections have gone
		while (active_connections.size())
		{
			JVX_SLEEP_MS(200);
		}

		// Stop listen thread
		running = false;
		JVX_SET_NOTIFICATION_2WAIT(notifyEventLoop);
#if 0
		JVX_SHUTDOWN_SOCKET(theSocket, JVX_SOCKET_SD_BOTH);
		JVX_CLOSE_SOCKET(theSocket);
#endif

		//JVX_WAIT_RESULT resW = JVX_WAIT_FOR_THREAD_TERMINATE_MS(threadHdlListen, timeout_msec);
		JVX_WAIT_RESULT resW = JVX_WAIT_FOR_THREAD_TERMINATE_INF(threadHdlListen);
		if (resW != JVX_WAIT_SUCCESS)
		{
			JVX_TERMINATE_THREAD(threadHdlListen, 0);
		}

		JVX_SHUTDOWN_SOCKET(theSocket, JVX_SOCKET_SD_BOTH);
		JVX_CLOSE_SOCKET(theSocket);

		if (fileCreated)
		{
			if (JVX_FILE_EXISTS(fnode_unix.c_str()))
			{
				JVX_FILE_REMOVE(fnode_unix.c_str());
			}
		}
		theState = jvxSocketServerState::JVX_STATE_SERVER_INIT;
	}
	return res;
}

jvxErrorType
CjvxSocketsServer::report_server_denied(const char* description)
{
	if (theReport)
	{
		theReport->report_server_denied(description);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsServer::report_server_connect(const char* description, IjvxSocketsConnection* newConnection)
{
	if (theReport)
	{
		theReport->report_server_connect(description, newConnection);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsServer::report_server_disconnect(const char* description, IjvxSocketsConnection* newConnection)
{
	if (theReport)
	{
		theReport->report_server_disconnect(description, newConnection);
	}

	// Trigger event to stop the thread and deactivate active conection

	JVX_SET_NOTIFICATION_2WAIT(notifyEventLoop); 

#if 0

	// Kill this connection from list of connections
	// Here we have produced a deadlock: this function is within the connection thread 
	// which we will wait for to terminate in accept_other_event
	accept_other_event();
#endif
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsServer::report_server_error(jvxSocketsErrorType err, const char* description)
{
	if (theReport)
	{
		theReport->report_server_error(err, description);
	}
	return JVX_NO_ERROR;
}

// =======================================================================

int
CjvxSocketsServerConnection::ic_single_connection_loop()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_DSP_SAFE_ALLOCATE_OBJECT(connection, CjvxSocketsConnectionTcp);

	connection->configure(socket);

	theState = jvxSocketsConnectionState::JVX_STATE_CONNECTION_CONNECTED;
	if (theReport)
	{
		theReport->report_server_connect(description.c_str(), static_cast<IjvxSocketsConnection*>(connection));
	}

	connection->main_loop();

	// Indicate that this thread is dead
	this->operPen = jvxSocketsServerPending::JVX_SOCKETS_PENDING_CLEAR;

	if (theReport)
	{
		theReport->report_server_disconnect(description.c_str(), static_cast<IjvxSocketsConnection*>(connection));
	}

	JVX_DSP_SAFE_DELETE_OBJECT(connection);
	connection = NULL;

	// Do we need to put this into a critical section? 
	// I do not think so since we have to stop before we reconnect anyway
	theState = jvxSocketsConnectionState::JVX_STATE_CONNECTION_DISCONNECTED;

	return 0;
}
