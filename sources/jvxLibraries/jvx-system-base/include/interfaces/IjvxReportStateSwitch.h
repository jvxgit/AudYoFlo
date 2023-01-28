#ifndef IJVXREPORTONSTATESWITCH_H__
#define IJVXREPORTONSTATESWITCH_H__

JVX_INTERFACE IjvxReportStateSwitch
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxReportStateSwitch(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION pre_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION post_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp, jvxErrorType suc) = 0;

};

#endif