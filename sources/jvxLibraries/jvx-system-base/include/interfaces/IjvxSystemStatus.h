#ifndef __IJVXSYSTEMSTATUS_H__
#define __IJVXSYSTEMSTATUS_H__

JVX_INTERFACE IjvxSystemStatus
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxSystemStatus() {};

	// Callback to report that the system is ready now
	virtual jvxErrorType JVX_CALLINGCONVENTION system_ready() = 0;

	// Callback to report that the system is about to shutdown
	virtual jvxErrorType JVX_CALLINGCONVENTION system_about_to_shutdown() = 0;
};

#endif
