#ifndef __CJVXAPPHOSTBASE_H__
#define __CJVXAPPHOSTBASE_H__

#include "CjvxAppFactoryHostBase.h"

#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
#define JVX_APPHOST_CLASSNAME CjvxAppHostBase_nd
#else
#define JVX_APPHOST_CLASSNAME CjvxAppHostBase
#endif

class JVX_APPHOST_CLASSNAME : public JVX_APP_FACTORY_HOST_CLASSNAME, public IjvxReportStateSwitch, public IjvxBootSteps
{
protected:
	
	struct
	{	
		jvxSize num_slots_max;
		jvxSize num_subslots_max;
	} _command_line_parameters_hosttype;
	
	struct
	{
		IjvxHost* hHost;
	} involvedHost;

	std::map<jvxComponentIdentification, jvx_componentIdPropsCombo> theRegisteredComponentReferenceTriples;

	// This reference must be set from the derived class to be used internally.
	// Typically, the pointer is set within the constructor of the derived class
	configureHost_features* confHostFeatures = nullptr;

	jvxBool onConnectionRulesStarted = false;
	jvxBool onComponentsBeforeConfig = false;
	jvxBool onComponentsConfigured = false;
	jvxBool onSequencerStarted = false;
	jvxBool onConnectionsStarted = false;

	IjvxReportSystem* if_report_automate_ = nullptr;
	IjvxAutoDataConnect* if_autoconnect_ = nullptr;	

	std::string modSelectionStringAutomate;

public:
	JVX_APPHOST_CLASSNAME();
	~JVX_APPHOST_CLASSNAME();

	virtual jvxErrorType openHostFactory(jvxApiString* errorMessage, jvxHandle* context) override;
	virtual jvxErrorType closeHostFactory(jvxApiString* errorMessage, jvxHandle* context)override;
	virtual jvxErrorType shutdownHostFactory(jvxApiString* errorMessage, jvxHandle* context)override;

	virtual void init_command_line_parameters(IjvxCommandLine* commLine)override;
	virtual void register_command_line_parameters(IjvxCommandLine* commLine)override;
	virtual void read_command_line_parameters(IjvxCommandLine* commLine)override;

	virtual jvxErrorType boot_initialize_base(jvxSize* numSlots, const std::map<jvxComponentIdentification, jvxSize>& specSubSlots);
	virtual jvxErrorType boot_initialize_product() = 0;

	virtual void boot_prepare_host_start() override;
	virtual void boot_prepare_host_stop() override; 
	virtual void shutdown_postprocess_host() override;

	virtual jvxErrorType shutdown_terminate_product() = 0;
	jvxErrorType shutdown_terminate_base();

	jvxErrorType reference_properties(const jvxComponentIdentification& tpId, IjvxAccessProperties** accProps);

	virtual jvxErrorType JVX_CALLINGCONVENTION on_connection_rules_started()  override;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_components_before_configure() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_components_configured() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_sequencer_started() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_connections_started() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION on_sequencer_stop() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_connections_stop() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_components_stop() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION on_connectionrules_stop() override;

private:
	virtual void resetReferences()override;
};


#endif
