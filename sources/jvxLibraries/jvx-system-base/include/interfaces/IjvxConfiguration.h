#ifndef __IJVXCONFIGURATION_H__
#define __IJVXCONFIGURATION_H__

JVX_INTERFACE IjvxConfigurationDone
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxConfigurationDone() {};

	//! Note: This function is only called if there is a configuration file. You should use system_ready if the function shall be called in every situation.
	virtual jvxErrorType JVX_CALLINGCONVENTION done_configuration() = 0;
};

JVX_INTERFACE IjvxConfiguration: public IjvxInterfaceReference
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxConfiguration(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1 ) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan, 
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) = 0;
};

#endif