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

protected:

	JVX_THREAD_ID idMainThread = JVX_NULLTHREAD;
	
	JVX_THREAD_HANDLE theThreadHdl = JVX_NULLTHREAD;
	JVX_THREAD_ID theThreadId = 0;

	IjvxSocketsClient_report* theReportClient = nullptr;
	CjvxSocketsConnection* theConnection = nullptr;

	JVX_MUTEX_HANDLE safeAccessConnection;

	// I have really been searching why the socket would not close when using UDP in windows. In order to test 
	// all options, I moved all socket functions into the worker thread. To review the open sockets I ran the
	// command "netstat -an" and saw that closing the socket did not really have an impact.
	// Finally I found that it was the process which I created in addition was created with flags "inherit=true".
	// This then explained the open socket.
	// jvxBool startSocketInWorker = true;
	// jvxBool socketStartComplete = false;
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

	jvxErrorType create_connect_socket();
};

#endif