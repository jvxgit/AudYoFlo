#ifndef __IJVXEXTERNALCALLTARGET_H__
#define __IJVXEXTERNALCALLTARGET_H__

JVX_INTERFACE IjvxExternalCallTarget
{
public:
	virtual ~IjvxExternalCallTarget(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION extcall_lasterror(jvxApiError* theErr) = 0;

	//virtual jvxErrorType JVX_CALLINGCONVENTION extcall_deallocate(jvxError* str) = 0;
};

#endif
