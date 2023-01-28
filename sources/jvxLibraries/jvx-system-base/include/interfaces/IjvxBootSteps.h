#ifndef __IJVXBOOTSTEPS_H__
#define __IJVXBOOTSTEPS_H__

JVX_INTERFACE IjvxBootSteps
{
 public:
  virtual JVX_CALLINGCONVENTION ~IjvxBootSteps(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION on_connection_rules_started() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_components_before_configure() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_components_configured( ) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_sequencer_started() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_connections_started() = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION on_sequencer_stop() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_connections_stop() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_components_stop() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_connectionrules_stop() = 0;
};

#endif
