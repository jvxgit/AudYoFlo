#ifdef _MSC_VER
extern "C"
{
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2bth.h>
}
#endif

#include "CjvxSockets.h"

std::string
CjvxSocketsCommon::sockAddrToHostName(struct in_addr* saddrv)
{
	std::string tmpStr;

	char ip_addr[INET_ADDRSTRLEN] = { 0 };
	const char* retVal = NULL;
	retVal = inet_ntop(AF_INET, saddrv, (char*)ip_addr, INET_ADDRSTRLEN);

	if (retVal)
	{
		tmpStr = ip_addr;
	}
	else
	{
		assert(0);
	}
	return tmpStr;
}

std::string 
CjvxSocketsCommon::sockAddrToHostName(struct in6_addr* saddrv)
{
	std::string tmpStr;

	char ip_addr[INET6_ADDRSTRLEN] = { 0 };
	const char* retVal = NULL;
	retVal = inet_ntop(AF_INET6, (struct in6_addr*)saddrv, (char*)ip_addr, INET6_ADDRSTRLEN);
	if (retVal)
	{
		tmpStr = ip_addr;
	}
	else
	{
		assert(0);
	}
	return tmpStr;
}

std::string 
CjvxSocketsCommon::sockAddrRecvToHostName(struct sockaddr_in* saddrv_ip4, int& port)
{
	port = htons(saddrv_ip4->sin_port);
	return(sockAddrToHostName(&saddrv_ip4->sin_addr));
}

std::string
CjvxSocketsCommon::sockAddrRecvToHostName(struct sockaddr_in6* saddrv_ip6, int& port)
{
	port = htons(saddrv_ip6->sin6_port);
	return(sockAddrToHostName(&saddrv_ip6->sin6_addr));
}

jvxErrorType 
CjvxSocketsCommon::hostNameToSockAddr(sockaddr_in* fillmev, const char* hostName, int port)
{
	int status;
	struct addrinfo hints, * res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;

	hints.ai_family = AF_INET;
	if ((status = getaddrinfo(hostName, NULL, &hints, &res)) == 0)
	{
		struct sockaddr_in* fillme = (struct sockaddr_in*)fillmev;
		memset(fillme, 0, sizeof(sockaddr_in));
		*fillme = *((struct sockaddr_in*)(res->ai_addr));
		//memcpy((char *)&fillme->sin_addr.s_addr, (char *)res->ai_addr->h_addr, target->h_length);
		fillme->sin_family = AF_INET;
		fillme->sin_port = htons(port);
		freeaddrinfo(res);
		return JVX_NO_ERROR;
	}
	return(JVX_ERROR_INTERNAL);
}

jvxErrorType 
CjvxSocketsCommon::hostNameToSockAddr(sockaddr_in6* fillmev, const char* hostName, int port)
{
	int status;
	struct addrinfo hints, * res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;

	hints.ai_family = AF_INET6;
	if ((status = getaddrinfo(hostName, NULL, &hints, &res)) == 0)
	{
		struct sockaddr_in6* fillme = (struct sockaddr_in6*)fillmev;
		memset(fillme, 0, sizeof(sockaddr_in6));
		*fillme = *((struct sockaddr_in6*)(res->ai_addr));
		//memcpy((char *)&fillme->sin_addr.s_addr, (char *)res->ai_addr->h_addr, target->h_length);
		fillme->sin6_family = AF_INET6;
		fillme->sin6_port = htons(port);
		freeaddrinfo(res);
		return JVX_NO_ERROR;
	}		
	return(JVX_ERROR_INTERNAL);
}

int 
CjvxSocketsCommon::connect_timeout(JVX_SOCKET& sock, const sockaddr* serv_addr, jvxSize sz_serv_addr, jvxSize timeout_ms)
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

	if (JVX_CHECK_SIZE_SELECTED(timeout_ms))
	{
		valI64 = (jvxInt64)timeout_ms * 1000;
		tv.tv_sec = (JVX_TIME_SEC)(valI64 / 1000000);
		tv.tv_usec = (JVX_TIME_USEC)(valI64 % 1000000);

		res = select((int)sock + 1, NULL, &fdset, NULL, &tv);
	}
	else
	{
		res = select((int)sock + 1, NULL, &fdset, NULL, NULL);
	}
	if (res == 1)
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

	if (retVal == 0)
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