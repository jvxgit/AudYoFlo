#ifndef _IJVXEVENTLOOP_BACKEND_CTRL_H__
#define _IJVXEVENTLOOP_BACKEND_CTRL_H__

JVX_INTERFACE IjvxEventLoopObject;

typedef enum
{
	JVX_BACKEND_STATUS_WANTS_RESTART_SHIFT = 1,
	JVX_BACKEND_STATUS_SCHEDULER_RUNNING_SHIFT = 2
} jvxBackendStatusShiftOptions;

JVX_INTERFACE IjvxEventLoop_backend_ctrl: public IjvxEventLoop_backend
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxEventLoop_backend_ctrl() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION add_external_interface(jvxInterfaceType tp, jvxHandle* theIf) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_external_interface(jvxInterfaceType tp, jvxHandle* theIf) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_reference_event_loop(IjvxEventLoopObject* eventLoop) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_reference_event_loop(IjvxEventLoopObject* eventLoop) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_pri_reference_frontend(IjvxEventLoop_frontend* theFrontend) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_pri_reference_frontend(IjvxEventLoop_frontend* theFrontend) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_sec_reference_frontend(IjvxEventLoop_frontend* theFrontend) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_sec_reference_frontend(IjvxEventLoop_frontend* theFrontend) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_pri_reference_backend(IjvxEventLoop_backend* theFrontend) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_pri_reference_backend(IjvxEventLoop_backend* theFrontend) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_sec_reference_backend(IjvxEventLoop_backend* theFrontend) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_sec_reference_backend(IjvxEventLoop_backend* theFrontend) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_receiving() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_receiving() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION wants_restart(jvxBool* wantsRestart) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION backend_status(jvxCBitField* curr_status) = 0;


};

#endif

