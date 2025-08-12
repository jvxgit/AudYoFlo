#ifndef __IJVXINFORMATION_H__
#define __IJVXINFORMATION_H__

JVX_INTERFACE IjvxInformation
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxInformation(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION number_info_tokens(jvxSize* num) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION info_token(jvxSize idx, jvxApiString* fldStr) = 0;
};

#endif
