#ifndef __IJVXUNIQUEID_H__
#define __IJVXUNIQUEID_H__

JVX_INTERFACE IjvxUniqueId
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxUniqueId() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION obtain_unique_id(jvxSize* idOnReturn, const char* desc) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION release_unique_id(jvxSize idOnReturn) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_unique_id(jvxSize idOnReturn, jvxApiString* onRet) = 0;

};

#endif

