#ifndef __CJVXSOCKETCLIENTINTERFACETCP_H__
#define __CJVXSOCKETCLIENTINTERFACETCP_H__

#include "CjvxSockets.h"

class CjvxSocketsClientInterfaceTcp;

class CjvxSocketsConnectionTcp : public CjvxSocketsConnection
{
	CjvxSocketsInterfaceTcpUdp* parent = nullptr;

#ifdef JVX_WINDOWS_SOCKET_IMPL
	JVX_NOTIFY_HANDLE notifyEventLoop;
	jvxBool running = false;
#endif

public:
	CjvxSocketsConnectionTcp();
	~CjvxSocketsConnectionTcp();
	virtual jvxErrorType configure(CjvxSocketsInterfaceTcpUdp* parArg);
	virtual jvxErrorType disconnect()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer(IjvxSocketsConnection_transfer* oneBuf, jvxSize* num_sent) override;
	void main_loop() override;
	jvxBool operate_one_read();
};

class CjvxSocketsClientInterfaceTcp : public CjvxSocketsClientInterfaceTcpUdp
{
public:
	// ========================================================
	// TCP
	// ========================================================

	jvxBool disableNagleAlgorithmTcp = true;

	CjvxSocketsClientInterfaceTcp();

	jvxErrorType configure(		
		const std::string& targetNameArg = "localhost",
		int remotePortArg = 45657,
		int localPortArg = -1,
		jvxSocketsFamily family = jvxSocketsFamily::JVX_SOCKET_IP_V4,
		jvxSize timeout_msecs = 5000,
		jvxBool disableNagleArg = true);

	virtual jvxErrorType set_opts_socket() override;
	virtual jvxErrorType connect_socket() override;
	virtual jvxErrorType start_connection_loop()override;
	virtual jvxErrorType stop_connection_loop()override;

	void ic_connect_loop_tcp_poll();
};

#endif