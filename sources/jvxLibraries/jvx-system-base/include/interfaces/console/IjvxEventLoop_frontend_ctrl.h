#ifndef _IJVXEVENTLOOP_FRONTEND_CTRL_H__
#define _IJVXEVENTLOOP_FRONTEND_CTRL_H__

JVX_INTERFACE IjvxEventLoop_backend_ext;

JVX_INTERFACE IjvxEventLoop_frontend_ctrl: public IjvxEventLoop_frontend
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxEventLoop_frontend_ctrl() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION add_reference_event_loop(IjvxEventLoopObject* eventLoop) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_reference_event_loop(IjvxEventLoopObject* eventLoop) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_sec_reference_event_backend(IjvxEventLoop_backend* theBackend) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_sec_reference_event_backend(IjvxEventLoop_backend* theBackend) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION returns_from_start(jvxBool* doesReturn) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION start(int argc, char* argv[]) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION wants_restart(jvxBool* wantsRestart) = 0;

};

#endif