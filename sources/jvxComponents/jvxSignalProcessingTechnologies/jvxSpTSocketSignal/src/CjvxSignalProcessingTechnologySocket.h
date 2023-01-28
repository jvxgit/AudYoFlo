#ifndef __CJVXSIGNALPROCESSINGTECHNOLOGYSOCKET__H__
#define __CJVXSIGNALPROCESSINGTECHNOLOGYSOCKET__H__

#include "CjvxSockets.h"

#include "jvxTechnologies/CjvxTemplateTechnologyVD.h"
#include "CjvxSignalProcessingDeviceSocket.h"

namespace {
	constexpr jvxSize JVX_SCHEDULE_ID_NEW_CONNECTION = 1;
	constexpr jvxSize JVX_SCHEDULE_ID_CONNECTION_DROPPED = 2;
};

class CjvxSignalProcessingTechnologySocket : 
	public CjvxTemplateTechnologyVD< CjvxSignalProcessingDeviceSocket>,
	public IjvxSocketsServer_report, public IjvxSchedule
{
protected:

	CjvxSocketsServer theDeviceSocket;
	CjvxSocketsClientFactory theDeviceSocketFactory;
	CjvxSocketsClientInterfaceUnix* theUnixSocket = nullptr;

	std::map<IjvxSocketsConnection*, CjvxSignalProcessingDeviceSocket*> pendig_activate;
	JVX_MUTEX_HANDLE safeAccessConnections;

public:

	// Socket backward interface
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_error(jvxSocketsErrorType err, const char* description)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_denied(const char* description)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_connect(const char* description, IjvxSocketsConnection* newConnection)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_disconnect(const char* description, IjvxSocketsConnection* newConnection)override;

	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION report_client_error(jvxSocketErrorType err, const char* description)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_client_connect(IjvxSocketConnection* connection)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_client_disconnect(IjvxSocketConnection* connection)override;
	*/

public:
	JVX_CALLINGCONVENTION CjvxSignalProcessingTechnologySocket(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxSignalProcessingTechnologySocket();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION return_device(IjvxDevice* dev) override;

	jvxErrorType report_new_connection(IjvxSocketsConnection* connection);
	virtual jvxErrorType JVX_CALLINGCONVENTION schedule_main_loop(jvxSize rescheduleId, jvxHandle* user_data) override;
};

#endif
