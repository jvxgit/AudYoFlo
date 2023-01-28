#ifndef __CJVXSOCKETSERVER_H__
#define __CJVXSOCKETSERVER_H__

#include "CjvxSockets.h"

class CjvxSocketsConnectionTcp;

class CjvxSocketsServerConnection
{
public:
	enum class jvxSocketsServerPending
	{
		JVX_SOCKETS_PENDING_NOTHING,
		JVX_SOCKETS_PENDING_CLEAR
	};

public:
	IjvxSocketsServer_report* theReport = nullptr;
	CjvxSocketsConnectionTcp* connection = nullptr;
	CjvxSocketsInterfaceTcpUdp* socket = nullptr;
	JVX_THREAD_ID threadId = 0;
	JVX_THREAD_HANDLE threadHdl = JVX_NULLTHREAD;
	std::string description = "unknown";
	jvxSocketsServerPending operPen = jvxSocketsServerPending::JVX_SOCKETS_PENDING_NOTHING;
	//jvxBool alive = true;

	jvxSocketsConnectionState theState = jvxSocketsConnectionState::JVX_STATE_CONNECTION_INIT;
public:
	CjvxSocketsServerConnection() {};
	int ic_single_connection_loop();
};

class CjvxSocketsServer: public CjvxSocketsInterfaceTcpUdp, public IjvxSocketsServer_report
{
	enum class jvxSocketServerState
	{
		JVX_STATE_SERVER_INIT,
		JVX_STATE_SERVER_STARTED,
		JVX_STATE_SERVER_LISTENING,
		JVX_STATE_SERVER_SHUTTING_DOWN
	};

protected:
	IjvxSocketsServer_report* theReport = nullptr; 

	jvxBool disableNagleAlgorithmTcp = true;

	jvxSize localPort = 45657; /* specify a -1 if no port is specified */
	jvxSocketsFamily family = jvxSocketsFamily::JVX_SOCKET_IP_V4;
	jvxSize numMaxConnections = 10;

	jvxSize timeout_msec = 5000;

	std::string fnode_unix;
	jvxBool fileCreated = false;
	jvxBool abstractFileMode = false;

	jvxBool connection_high_prio = false;

	jvxSocketServerState theState = jvxSocketServerState::JVX_STATE_SERVER_INIT;

	JVX_MUTEX_HANDLE safeAccess;
	JVX_THREAD_ID myMainThreadId = JVX_NULLTHREAD;

	JVX_THREAD_ID threadIdListen = 0;
	JVX_THREAD_HANDLE threadHdlListen = JVX_NULLTHREAD;

	std::list<CjvxSocketsServerConnection*> active_connections;

//#ifdef JVX_WINDOWS_SOCKET_IMPL
	JVX_NOTIFY_HANDLE_2WAIT_DECLARE(notifyEventLoop) = JVX_INVALID_HANDLE_VALUE_2WAIT;
	jvxBool running = false;
//#endif
	jvxBool lockIncoming = false;

public:
	CjvxSocketsServer();
	~CjvxSocketsServer();

	jvxErrorType start(IjvxSocketsServer_report* theReport,
		jvxBool disableNagleAlgorithmTcpArg = true,
		jvxSize localPortArg = 45657,
		jvxSocketsFamily familyArg = jvxSocketsFamily::JVX_SOCKET_IP_V4,
		jvxSize numMaxConnectionsArg = JVX_SIZE_UNSELECTED,
		jvxBool highPrioThreads = false);

	jvxErrorType start(IjvxSocketsServer_report* theReport,
		jvxBool disableNagleAlgorithmTcpArg = true,
		const std::string& fnode = "./my-unix-socket-emntry",
		jvxSize numMaxConnectionsArg = JVX_SIZE_UNSELECTED,
		jvxBool highPrioThreads = false);

	jvxErrorType startup_core(
		std::string& errorDescription,
		jvxSocketsErrorType& errId);

	jvxErrorType lock_incoming(jvxBool lockit);

	jvxErrorType stop();

public:
	int in_thread_connect();
	void connect_loop_tcp_poll();

	jvxErrorType accept_new_socket(std::string& errorDescription);
	jvxErrorType accept_other_event();

	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_denied(const char* description) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_connect(const char* description, IjvxSocketsConnection* newConnection) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_disconnect(const char* description, IjvxSocketsConnection* newConnection) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_error(jvxSocketsErrorType err, const char* description) override;
};

#endif
