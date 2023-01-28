#ifndef __IJVXFACTORYHOST_H__
#define __IJVXFACTORYHOST_H__

JVX_INTERFACE IjvxMinHost : public IjvxInterfaceFactory, public IjvxCoreStateMachine
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxMinHost() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION request_unique_host_id(jvxSize* uId) = 0;
};

JVX_INTERFACE IjvxFactoryHost: public IjvxMinHost, public IjvxHostInteraction, public IjvxSystemStatus
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxFactoryHost(){};
	virtual jvxErrorType JVX_CALLINGCONVENTION request_id_main_thread(JVX_THREAD_ID* thread_id) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_boot_complete(jvxBool isComplete) = 0;
};
	
#endif	