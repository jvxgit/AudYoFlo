#ifndef __IJVXGLOBALLOCK_H__
#define __IJVXGLOBALLOCK_H__

JVX_INTERFACE IjvxLock 
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxLock() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION  lock() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION  unlock() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION  try_lock() = 0;	
};

JVX_INTERFACE IjvxGlobalLock : public IjvxObject
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxGlobalLock() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_global_lock(const char* idToken, IjvxLock** lock, jvxBool* installedOnRequest) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_global_lock(IjvxLock* lock) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_global_locks(jvxSize* num) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION token_global_locks(jvxSize id, jvxApiString& tokenOnReturn) = 0;
};

#endif