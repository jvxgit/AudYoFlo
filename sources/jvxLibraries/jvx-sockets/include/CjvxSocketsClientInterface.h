#ifndef __CJVXSOCKETCLIENTINTERFACE_H__
#define __CJVXSOCKETCLIENTINTERFACE_H__

#include "CjvxSockets.h"

class CjvxSocketsClientInterface
{
public:
		
	// =======================================================================

	jvxSocketsConnectionType socketType = jvxSocketsConnectionType::JVX_SOCKET_TYPE_TCP;
	jvxSocketsConnectionState theState = jvxSocketsConnectionState::JVX_STATE_CONNECTION_INIT;

	// std::list<CjvxSocketClientConnection> connections;
	std::string name;
	jvxSize id = 0;
	jvxSize numRefsMax = JVX_SIZE_UNSELECTED;
	std::string last_error = "no error";

	JVX_THREAD_ID idMainThread = JVX_NULLTHREAD;
	
	JVX_THREAD_HANDLE theThreadHdl = JVX_NULLTHREAD;
	JVX_THREAD_ID theThreadId = 0;

	IjvxSocketsClient_report* theReportClient = nullptr;
	CjvxSocketsConnection* theConnection = nullptr;

	JVX_MUTEX_HANDLE safeAccessConnection;

public:

	CjvxSocketsClientInterface(jvxSocketsConnectionType socketType
		= jvxSocketsConnectionType::JVX_SOCKET_TYPE_TCP);
	virtual ~CjvxSocketsClientInterface();

	jvxErrorType configure();
	jvxErrorType start(IjvxSocketsClient_report* theReportClientArg);
	jvxErrorType stop();

	virtual jvxErrorType pre_start_socket() = 0;
	virtual jvxErrorType start_socket() = 0;
	virtual jvxErrorType set_opts_socket() = 0;
	virtual jvxErrorType connect_socket() = 0;
	virtual jvxErrorType start_connection_loop() = 0;

	virtual jvxErrorType stop_connection_loop() = 0;
	virtual jvxErrorType stop_socket() = 0;
};

#endif