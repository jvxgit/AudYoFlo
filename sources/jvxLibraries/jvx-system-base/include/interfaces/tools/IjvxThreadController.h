#ifndef _IJVXTHREADCONTROLLER_H__
#define _IJVXTHREADCONTROLLER_H__

JVX_INTERFACE IjvxThreadController: public IjvxObject
{
public:
  virtual JVX_CALLINGCONVENTION ~IjvxThreadController(){};

	//! Initialize the module, set size of buffer and strategy
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef) = 0;

	//! Initialize the module, set size of buffer and strategy
	virtual jvxErrorType JVX_CALLINGCONVENTION number_cores(jvxSize* numCores) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION configure(jvxInt32 percentNonTimeCritival, jvxInt32 timeout_refresh_ms) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION register_tc_thread(jvxInt32* id_address) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_tc_thread(jvxInt32 id_address) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION associate_tc_thread(JVX_THREAD_ID theThread, jvxInt32 id_address) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION post_process() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() = 0;

	//! Terminate
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;
};

#endif