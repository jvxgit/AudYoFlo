#ifndef __IJVX_EVENTLOOP_H__
#define __IJVX_EVENTLOOP_H__

// ==========================================================
JVX_INTERFACE IjvxEventLoop_frontend
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxEventLoop_frontend(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION report_register_fe_commandline(IjvxCommandLine* comLine) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION report_readout_fe_commandline(IjvxCommandLine* comLine) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_process_event(TjvxEventLoopElement* theQueueElement) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_cancel_event(TjvxEventLoopElement* theQueueElement) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_assign_output(TjvxEventLoopElement* theQueueElement, jvxErrorType sucOperation, jvxHandle* priv) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_special_event(TjvxEventLoopElement* theQueueElement, jvxHandle* priv) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_want_to_shutdown_ext(jvxBool restart) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_property(jvxFrontendSupportRequestType tp, jvxHandle* load) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_if_command_forward(IjvxReportSystemForward** fwdCalls) = 0;
};
	
JVX_INTERFACE IjvxEventLoop_backend
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxEventLoop_backend(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION report_register_be_commandline(IjvxCommandLine* comLine) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_readout_be_commandline(IjvxCommandLine* comLine) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_event(TjvxEventLoopElement* theQueueElement) = 0;

};

// ==================================================================

JVX_INTERFACE IjvxEventLoop
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxEventLoop(){};
		
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(
		IjvxHiddenInterface* theHost,
		jvxSize maxNum_lowprioqueue,
		jvxSize maxNum_highprioqueue,
		jvxSize maxNum_timerqueue,
		jvxSize maxNumberWait_msec) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION start() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION reserve_message_id(
		jvxSize* message_id) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION event_schedule(TjvxEventLoopElement* newElm, jvxErrorType* res_scheduled_event, jvxHandle* blockPrivate) = 0;
		
	virtual jvxErrorType JVX_CALLINGCONVENTION event_clear(jvxSize event_id, 
		IjvxEventLoop_frontend* fe_filter, IjvxEventLoop_backend* be_filter) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION events_process() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION frontend_block(IjvxEventLoop_frontend* fe_filter) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION frontend_unblock(IjvxEventLoop_frontend* fe_filter) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION backend_block(IjvxEventLoop_backend* fe_filter) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION backend_unblock(IjvxEventLoop_backend* fe_filter) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION force_clear_end_event_loop() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop() = 0; 

	virtual jvxErrorType JVX_CALLINGCONVENTION wait_stop(jvxSize timeout_msec) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;
};

#endif
