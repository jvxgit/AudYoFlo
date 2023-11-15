#ifndef __IJVXCOMPONENTHOSTEXT_H__
#define __IJVXCOMPONENTHOSTEXT_H__

JVX_INTERFACE IjvxComponentHostExt
{
public:
	virtual ~IjvxComponentHostExt(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION attach_external_component(IjvxObject* toBeAttached, const char* moduleGroup, jvxBool regConnFactory, jvxBool noCfgSave = true, jvxSize desiredSlotId = JVX_SIZE_DONTCARE, jvxSize desiredSlotSubId = 0) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION detach_external_component(IjvxObject* toBeDetached, const char* moduleGroup, jvxState statOnLeave)  = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION load_config_content_module(const char* modName, jvxConfigData** cfgData) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION release_config_content_module(const char* modName, jvxConfigData* cfgData) = 0;
};

#endif

