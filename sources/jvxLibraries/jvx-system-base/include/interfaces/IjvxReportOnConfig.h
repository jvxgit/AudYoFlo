#ifndef IJVXREPORTONCONFIG_H__
#define IJVXREPORTONCONFIG_H__

JVX_INTERFACE IjvxReportOnConfig
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxReportOnConfig(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_select_on_config(const jvxComponentIdentification& tp) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_active_on_config(const jvxComponentIdentification& tp) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_inactive_on_config(const jvxComponentIdentification& tp) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_component_unselect_on_config(const jvxComponentIdentification& tp) = 0;

};

#endif
