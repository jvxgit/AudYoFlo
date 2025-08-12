#ifndef __IJVXCONFIGURATIONEXTENDER_H__
#define __IJVXCONFIGURATIONEXTENDER_H__

JVX_INTERFACE IjvxConfigurationExtender_report
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxConfigurationExtender_report(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename, jvxInt32 lineno) = 0;
};
	
JVX_INTERFACE IjvxConfigurationExtender:public IjvxInterfaceReference
{
	
public:

	virtual JVX_CALLINGCONVENTION ~IjvxConfigurationExtender(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_configuration_entries() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_configuration_entry(const char* entryname, jvxHandle* fld, jvxConfigSectionTypes tp, jvxSize id) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_entry(const char* entryname, jvxHandle* fld, jvxConfigSectionTypes* tp, jvxSize id) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION register_extension(IjvxConfigurationExtender_report* bwd, const char* sectionName) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_extension(const char* sectionName) = 0;
};

#endif
