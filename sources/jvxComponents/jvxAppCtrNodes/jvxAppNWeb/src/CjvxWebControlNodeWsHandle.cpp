#include "CjvxWebControlNodeWsHandle.h"

/* This list contains those command requests that are ignored and not 
 * reported via websocket. 
 */
jvxReportCommandRequest lstFilterIgnore[] =
{
	jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_COMPONENT,
	jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_SUBDEVICE,
	jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_CONFIGURATION_COMPLETE,
	jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_COMPONENT,
	jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_SUBDEVICE,
	jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED,
	jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_RESCHEDULE_MAIN,
	jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE,
	jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN,
	jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC,
	jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_LIMIT
};

CjvxWebControlNodeWsEvent::CjvxWebControlNodeWsEvent(const std::string& argTxt, CjvxWebControlNodeWsEventType eventTypeArg)
{
	eventType = eventTypeArg;
	txt = argTxt;
}

CjvxWebControlNodeWsHandle::CjvxWebControlNodeWsHandle(IjvxWebServer* serv, jvxHandle* connArg)
{
	wsOut = serv;
	wsConn = connArg;
	JVX_INITIALIZE_MUTEX(safeAccessCommands);
}

CjvxWebControlNodeWsHandle::~CjvxWebControlNodeWsHandle()
{
	JVX_TERMINATE_MUTEX(safeAccessCommands);
}

void 
CjvxWebControlNodeWsHandle::lock()
{
	JVX_LOCK_MUTEX(safeAccessCommands);
}

void 
CjvxWebControlNodeWsHandle::unlock()
{
	JVX_UNLOCK_MUTEX(safeAccessCommands);
}

void 
CjvxWebControlNodeWsHandle::clear_ws_container()
{
	if (subReport)
	{
		subReport->unregister_sub_report(static_cast<IjvxReportSystem*>(this));
	}
	subReport = nullptr;
	JVX_TERMINATE_MUTEX(safeAccessCommands);
}

jvxErrorType 
CjvxWebControlNodeWsHandle::request_command(const CjvxReportCommandRequest& request) 
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string transToken;
	jvxBool inIgnoreList = false;
	int cnt = 0;
	auto req = request.request();
	while (lstFilterIgnore[cnt] != jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_LIMIT)
	{
		if (lstFilterIgnore[cnt] == req)
		{
			inIgnoreList = true;
			break;
		}
		cnt++;
	}

	if (inIgnoreList)
	{
		return JVX_ERROR_INVALID_SETTING;
	}

	res = jvx::helper::json::commandRequest2Json(request, transToken);
	if (res == JVX_NO_ERROR)
	{
		wsOut->in_connect_send_ws_packet(reinterpret_cast<struct mg_connection*>(wsConn), 0x1 /* text mode */, transToken.c_str(), transToken.size());
	}
	return res;
}


