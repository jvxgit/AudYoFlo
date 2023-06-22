#ifndef __IjvxLogRemoteHandler_H__
#define __IjvxLogRemoteHandler_H__

#include "jvx.h"

JVX_INTERFACE IjvxLogRemoteHandler: public IjvxObject
{
public:
	virtual ~IjvxLogRemoteHandler() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;

	virtual void configure(const char* logTagModule, jvxLogLevel lev) = 0;
	virtual std::ostream* log_str() = 0;
	virtual jvxErrorType start_lock() = 0;
	virtual void stop_lock() = 0;
};

#endif
