#ifndef __CJVXCONSOLE_HOST_FE_H__
#define __CJVXCONSOLE_HOST_FE_H__

#include "jvx.h"
#include "interfaces/console/IjvxEventLoop_frontend_ctrl.h"
#include "CjvxJson.h"
#include "typedefs/console/TjvxFrontendBackend.h"
#include "commandline/CjvxCommandLine.h"

#define JVX_NUMBER_CTRLC_EXIT 6

class CjvxConsoleHost_fe_console: public IjvxEventLoop_frontend_ctrl
{
	typedef struct
	{
		jvxByte* buf;
		jvxSize selector;
	} oneAllocatedField;

	IjvxEventLoopObject* evLop;
	jvxOnePriBackendAndState linkedPriBackend;
	std::list<jvxOneBackendAndState> linkedSecBackends;

    JVX_NOTIFY_HANDLE informShutdown;
	jvxBool config_noconsole;
	jvxBool noconsole_restart;

  // Default initialization only works in Windows, in linux the struct is more than just a simple handle value
  JVX_NOTIFY_HANDLE_2WAIT_DECLARE(informStopLoop) = JVX_INVALID_HANDLE_VALUE_2WAIT;
  
	struct
	{
		jvxBool compact_out;
	} config;
private:

	std::vector<oneAllocatedField> flds;
	JVX_MUTEX_HANDLE safeAccessMemList;
	jvxSize timerCount;
	jvxState theState;
	jvxBool printAllRequestsOutput;
	jvxBool wantsRestart;
	CjvxCommandLine commLine;
	jvxSize ctrlccounter;
public:

	CjvxConsoleHost_fe_console();
	~CjvxConsoleHost_fe_console();

	// Interface IjvxEventLoop_frontend_ctrl
	// void set_reference(IjvxEventLoopObject* eventLoop, IjvxEventLoop_backend* theBackend);
	virtual jvxErrorType JVX_CALLINGCONVENTION add_reference_event_loop(IjvxEventLoopObject* eventLoop)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_reference_event_loop(IjvxEventLoopObject* eventLoop)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_sec_reference_event_backend(IjvxEventLoop_backend* theBackend)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_sec_reference_event_backend(IjvxEventLoop_backend* theBackend)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION returns_from_start(jvxBool* doesReturn)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start(int argc, char* argv[])override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION wants_restart(jvxBool *)override;

	// Interface IjvxEventLoop_frontend
	virtual jvxErrorType JVX_CALLINGCONVENTION report_process_event(TjvxEventLoopElement* theQueueElement)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_cancel_event(TjvxEventLoopElement* theQueueElement)override;

	//void start_timer(jvxSize time_diff_msec);
	//void wait_for_input();

	virtual jvxErrorType JVX_CALLINGCONVENTION report_assign_output(TjvxEventLoopElement* theQueueElement, jvxErrorType sucOperation, jvxHandle* priv) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_special_event(TjvxEventLoopElement* theQueueElement, jvxHandle* priv) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_register_fe_commandline(IjvxCommandLine* comLine)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_readout_fe_commandline(IjvxCommandLine* comLine)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_want_to_shutdown_ext(jvxBool restart) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_property(jvxFrontendSupportRequestType tp, jvxHandle* load) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_if_command_forward(IjvxReportSystemForward** fwdCalls) override;

	jvxErrorType cleanup(
		jvxSize message_id,
		jvxHandle* param, jvxSize paramType,
		jvxSize event_type,
		jvxOneEventClass event_class,
		jvxOneEventPriority event_priority,
		jvxCBool autoDeleteOnProcess);

    void trigger_sigint_ext();
    void trigger_sigint_ext_core();
	void console_wait();
	void trigger_deactivate();
	void output_sequencer_event(jvxSpecialEventType_seq* seq);
	jvxErrorType handle_ctrl_c(jvxBool ctrlcpressed, jvxBool config_noconsole);

private:
};

#endif
