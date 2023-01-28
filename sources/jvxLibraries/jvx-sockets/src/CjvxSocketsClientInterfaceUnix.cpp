#include "CjvxSocketsClientInterfaceUnix.h"

CjvxSocketsClientInterfaceUnix::CjvxSocketsClientInterfaceUnix():
	CjvxSocketsClientInterfaceTcp()
{

}

jvxErrorType
CjvxSocketsClientInterfaceUnix::configure(
	const std::string& targetNameArg,
	const std::string& clientNameArg)
{
	jvxErrorType res = CjvxSocketsClientInterfaceTcp::configure( targetNameArg);
	if (this->targetName[0] == ' ')
	{
		abstract_file_mode_server = true;
	}
	client_socket = clientNameArg;
	if (client_socket[0] == ' ')
	{
		abstract_file_mode_client = true;
	}
	return res;
}

jvxErrorType
CjvxSocketsClientInterfaceUnix::start_socket()
{
	int proto = PF_UNSPEC;
	theSocket = socket(AF_UNIX, SOCK_STREAM, proto);

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
CjvxSocketsClientInterfaceUnix::set_opts_socket()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSocketsClientInterfaceUnix::connect_socket()
{
	jvxErrorType res = JVX_NO_ERROR;
	int errCode = 0;
	struct sockaddr_un my_addr_un;
	struct sockaddr_un target_addr_un;
	jvxSize len = 0;
	jvxSize copyLen = 0;
	 
	/*
	https://stackoverflow.com/questions/26757306/delete-af-unix-file-automatically
	https://devblogs.microsoft.com/commandline/windowswsl-interop-with-af_unix/
	https://github.com/microsoft/WSL/issues/4240
	*/
	len = sizeof(my_addr_un);
	memset((char*)&my_addr_un, 0, len);
	copyLen = sizeof(my_addr_un.sun_path) - 1;// ARRAYSIZE(my_addr_un.sun_path) - 1;
	copyLen = JVX_MIN(copyLen,
		this->client_socket.size());
	// Specify listening port

	my_addr_un.sun_family = AF_UNIX;
	memcpy(&my_addr_un.sun_path, this->client_socket.c_str(), copyLen);

	// Make it abstract and non visible!
	if (abstract_file_mode_client)
	{
		std::cout << "Binding UNIX socket in ABSTRACT file <" << this->targetName << ">." << std::endl;
		my_addr_un.sun_path[0] = 0;
	}
	else
	{
		std::cout << "Binding UNIX socket in file <" << this->targetName << ">." << std::endl;
	}
	errCode = bind(theSocket, (struct sockaddr*)&my_addr_un, (int)len);
	if (errCode != 0)
	{
		res = JVX_ERROR_INTERNAL;

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

	if (res == JVX_NO_ERROR)
	{
		// ==================================================================================
		// Connect to target server..
		// ==================================================================================
		len = sizeof(target_addr_un);
		memset((char*)&target_addr_un, 0, len);
		copyLen = sizeof(target_addr_un.sun_path) - 1;// ARRAYSIZE(my_addr_un.sun_path) - 1;
		copyLen = JVX_MIN(copyLen,
			this->targetName.size());
		// Specify listening port

		target_addr_un.sun_family = AF_UNIX;
		memcpy(&target_addr_un.sun_path, this->targetName.c_str(), copyLen);

		// Make it abstract and non visible!
		if (abstract_file_mode_server)
		{
			std::cout << "Connecting to UNIX socket in ABSTRACT file <" << this->targetName << ">." << std::endl;
			target_addr_un.sun_path[0] = 0;
		}
		else
		{
			std::cout << "Connecting to UNIX socket in file <" << this->targetName << ">." << std::endl;
		}
		errCode = connect(theSocket, (const struct sockaddr*)&target_addr_un, (int)len);
		if (errCode != 0)
		{
			res = JVX_ERROR_INTERNAL;

#ifdef JVX_OS_WINDOWS
			errCode = WSAGetLastError();// Here, res already contains the error code, additional error code -1 for timeout
			// Check that connection was successful
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
				last_error = "The parameter is is a listening socket.";
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
			case WSAECONNREFUSED:
				last_error = "The connection was refused.";
				break;
			default:
				last_error = "Unlisted error code <" + jvx_int2String(errCode) + ">"; // assert(0); // Try to detect the reason!!
			}
			


#else
			last_error = (std::string)"Was not able to connect to server, error: " + strerror(errno);
#endif
		} // if (errCode != 0)
	}
	return res;
}
// ===========================================================================================
