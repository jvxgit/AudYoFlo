#ifndef _CJVXWEBCONTROLNODEWSHANDLE_H_
#define _CJVXWEBCONTROLNODEWSHANDLE_H_

#include "jvx.h"

enum class CjvxWebControlNodeWsEventType
{
	AYF_EVENT_NONE,
	AYF_EVENT_INIT_INTERPRET,
	AYF_EVENT_INTERPRET,
	AYF_EVENT_DIRECT_EMIT
};

struct CjvxWebControlNodeWsEvent
{
	CjvxWebControlNodeWsEventType eventType = CjvxWebControlNodeWsEventType::AYF_EVENT_NONE;
	std::string txt;
	CjvxWebControlNodeWsEvent(const std::string& argTxt, CjvxWebControlNodeWsEventType eventTypeArg);
};

class CjvxWebControlNodeWsHandle : public IjvxReportSystem
{
private:
	IjvxWebServer* wsOut = nullptr;
	jvxHandle* wsConn = nullptr;
	JVX_MUTEX_HANDLE safeAccessCommands;

public:
	std::list<CjvxWebControlNodeWsEvent> commands;
	jvxState stat = JVX_STATE_INIT;
	IjvxSubReport* subReport = nullptr;

	CjvxWebControlNodeWsHandle(IjvxWebServer* serv, jvxHandle* connArg);
	~CjvxWebControlNodeWsHandle();
	void lock();
	void unlock();
	void clear_ws_container();

	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) override;
};

#endif
