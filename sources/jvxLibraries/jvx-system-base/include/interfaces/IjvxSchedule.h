#ifndef __IJVXSCHEDULE_H__
#define __IJVXSCHEDULE_H__

JVX_INTERFACE IjvxSchedule
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxSchedule(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION schedule_main_loop(jvxSize rescheduleId, jvxHandle* user_data) = 0;
};

#endif
