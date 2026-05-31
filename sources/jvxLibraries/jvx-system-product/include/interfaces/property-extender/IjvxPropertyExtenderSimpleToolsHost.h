#ifndef _IJVXPROPERTYEXTENDERSIMPLETOOLSHOST_H__
#define _IJVXPROPERTYEXTENDERSIMPLETOOLSHOST_H__

JVX_INTERFACE IjvxPropertyExtenderSimpleToolsHost
{
public:
	virtual ~IjvxPropertyExtenderSimpleToolsHost() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION number_tools(const jvxComponentIdentification&, jvxSize* num) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION identification_tool(const jvxComponentIdentification&, jvxSize idx, jvxApiString* description, jvxApiString* descriptor, jvxBool* multipleInstances) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION instance_tool(jvxComponentType, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION return_instance_tool(jvxComponentType tp, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor) = 0;
};

JVX_INTERFACE IjvxPropertyExtenderSimpleToolsHostInstall
{
public:
	virtual ~IjvxPropertyExtenderSimpleToolsHostInstall() {};	

	virtual jvxErrorType install(IjvxPropertyExtenderSimpleToolsHost* installThis) = 0;
	virtual jvxErrorType uninstall(IjvxPropertyExtenderSimpleToolsHost* uninstallThis) = 0;
};

#endif
