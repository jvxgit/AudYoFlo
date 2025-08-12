#ifndef _IJVXEVENTLOOP_FRONTEND_CTRL_H__
#define _IJVXEVENTLOOP_FRONTEND_CTRL_H__

JVX_INTERFACE IjvxEventLoop_backend_ext;

JVX_INTERFACE IjvxEventLoop_frontend_ctrl: public IjvxEventLoop_frontend
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxEventLoop_frontend_ctrl() {};

	//! Add a reference to the event loop
	virtual jvxErrorType JVX_CALLINGCONVENTION add_reference_event_loop(IjvxEventLoopObject* eventLoop) = 0;

	//! Clears the reference to the event loop
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_reference_event_loop(IjvxEventLoopObject* eventLoop) = 0;

	//! Set the reference to the primary backend. All input address the primary backend
	virtual jvxErrorType JVX_CALLINGCONVENTION set_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend) = 0;

	//! Reset the reference to the primary backend. 
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend) = 0;

	//! Add secondary backends if any exist
	virtual jvxErrorType JVX_CALLINGCONVENTION add_sec_reference_event_backend(IjvxEventLoop_backend* theBackend) = 0;
	
	//! Clear one of the secondary backends if any exist
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_sec_reference_event_backend(IjvxEventLoop_backend* theBackend) = 0;

	//! Shows the main application of the call to start blocks or not. Principally, the primary frontend should not return whereelse all other should!
	virtual jvxErrorType JVX_CALLINGCONVENTION returns_from_start(jvxBool* doesReturn) = 0;

	//! Function called when starting the event loop processing
	virtual jvxErrorType JVX_CALLINGCONVENTION start(int argc, char* argv[]) = 0;
	
	//! Function called when stopping the event loop processing
	virtual jvxErrorType JVX_CALLINGCONVENTION stop() = 0;

	//! Function that checks on shutdown if any frontend would prefer a restart
	virtual jvxErrorType JVX_CALLINGCONVENTION wants_restart(jvxBool* wantsRestart) = 0;

};

#endif
