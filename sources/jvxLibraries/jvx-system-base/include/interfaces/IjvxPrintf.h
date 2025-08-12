#ifndef __IJVXPRINTF_H__
#define __IJVXPRINTF_H__

JVX_INTERFACE IjvxPrintf
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxPrintf(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION request_printf(the_jvx_printf* fPtr) = 0;

};

#endif
