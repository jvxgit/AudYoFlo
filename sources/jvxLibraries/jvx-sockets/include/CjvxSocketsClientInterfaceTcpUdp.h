#ifndef __CJVXSOCKETCLIENTINTERFACETCPUDP_H__
#define __CJVXSOCKETCLIENTINTERFACETCPUDP_H__

#include "CjvxSockets.h"

class CjvxSocketsClientInterfaceTcpUdp;

class CjvxSocketsConnectionTcpUdp : public CjvxSocketsConnection
{
	CjvxSocketsClientInterfaceTcpUdp* parent = nullptr;
public:
	CjvxSocketsConnectionTcpUdp() {};
	virtual jvxErrorType transfer(IjvxSocketsConnection_transfer* oneBuf, jvxSize* num_sent) override;
	virtual jvxErrorType disconnect() override;
	jvxErrorType configure(CjvxSocketsClientInterfaceTcpUdp* par);
	void main_loop() override;
};

// ====================================================================
class CjvxSocketsClientInterfaceTcpUdp: 
	public CjvxSocketsInterfaceTcpUdp,
	public CjvxSocketsClientInterface
{
public:
	// ========================================================
	// TCP & UDP
	// ========================================================

	struct sockaddr_in targetServer_ip4;
	struct sockaddr_in6 targetServer_ip6;
	jvxBool targetServerSet = false;
	jvxSize timeout_msecs = 5000;

	std::string targetName = "localhost";
	int remotePort = 45657;
	int localPort = -1; /* specify a -1 if no port is specified */
	jvxSocketsFamily family = jvxSocketsFamily::JVX_SOCKET_IP_V4;

public:
	CjvxSocketsClientInterfaceTcpUdp();

	jvxErrorType configure(		
		const std::string& targetNameArg = "localhost", 
		int remotePortArg = 45657,
		int localPortArg = -1,
		jvxSocketsFamily family = jvxSocketsFamily::JVX_SOCKET_IP_V4,
		jvxSize timeout_msecs = 5000);
	
	virtual jvxErrorType pre_start_socket() override;
	virtual jvxErrorType start_socket()override;
	virtual jvxErrorType connect_socket() override;
	virtual jvxErrorType start_connection_loop()override;
	
	virtual jvxErrorType stop_connection_loop()override;
	virtual jvxErrorType stop_socket() override;

	void ic_connect_loop_udp_poll();
};

#endif