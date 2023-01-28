#ifndef __CJVXSOCKETSCOMMON__H__
#define __CJVXSOCKETSCOMMON__H__

#include "jvx.h"

#ifdef JVX_OS_WINDOWS
//#define JVX_WINDOWS_SOCKET_IMPL
#endif

class CjvxSocketsCommon
{
public:
	static std::string sockAddrToHostName(struct in_addr* saddrv);
	static std::string sockAddrToHostName(struct in6_addr* saddrv);

	static std::string sockAddrRecvToHostName(struct sockaddr_in* saddrv_ip4, int& port);
	static std::string sockAddrRecvToHostName(struct sockaddr_in6* saddrv_ip6, int& port);

	static jvxErrorType hostNameToSockAddr(struct sockaddr_in* fillmev, const char* hostName, int port);
	static jvxErrorType hostNameToSockAddr(struct sockaddr_in6* fillmev, const char* hostName, int port);

	static int connect_timeout(JVX_SOCKET& sock, const sockaddr* serv_addr, jvxSize sz_serv_addr, jvxSize timeout_ms);

};

class CjvxSocketsConnectionCommon: public IjvxSocketsConnection
{
protected:
	IjvxSocketsConnection_report* theReport = nullptr;

public:

	CjvxSocketsConnectionCommon();
	jvxErrorType initialize(IjvxSocketsConnection_report* theReportArg);
	jvxErrorType terminate();
};

#endif
