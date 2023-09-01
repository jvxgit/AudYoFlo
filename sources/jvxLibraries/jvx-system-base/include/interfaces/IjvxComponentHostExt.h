#ifndef __IJVXCOMPONENTHOSTEXT_H__
#define __IJVXCOMPONENTHOSTEXT_H__

JVX_INTERFACE IjvxComponentHostExt
{
public:
	virtual ~IjvxComponentHostExt(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION attach_external_component(IjvxObject* toBeAttached, const char* moduleGroup, jvxBool regConnFactory) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION detach_external_component(IjvxObject* toBeDetached, const char* moduleGroup, jvxState statOnLeave)  = 0;
};

#endif

