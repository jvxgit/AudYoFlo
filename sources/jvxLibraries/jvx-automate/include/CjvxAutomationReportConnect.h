#ifndef __CJVXAUTOMATIONSIMPLE_H__
#define __CJVXAUTOMATIONSIMPLE_H__

#include "CjvxAutomationReport.h"
#include "common/CjvxObject.h"

class CjvxAutomationReportConnect :
	public CjvxAutomationReport, public IjvxAutoDataConnect,
	public IjvxSimpleComponent, public CjvxObject
{
protected:
	IjvxReport* reportRef = nullptr;
	std::string nmComponent = "CjvxAutomationSimple";

public:
	CjvxAutomationReportConnect(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxAutomationReportConnect();

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// =============================================================================================
	// CjvxAutomation
	// =============================================================================================

	/**
	 * In this callback idents are reported together with a specific command. Typically, a command request indicates a device that was born or has died.
	 * The typical action in this function is to select and activate the new device automatically.
	 * Once a new component was activated, it is reported to the "autoconnect" part where other components may be added.
	 */
	virtual jvxErrorType handle_report_ident(jvxReportCommandRequest req,
		jvxComponentIdentification tp,
		const std::string& ident,
		CjvxAutomationReport::callSpecificParameters* params) override;

	/**
	 * Report unique id to show that new data connection process has been established. For a given process we may iterate the involved components.
	 */
	virtual jvxErrorType handle_report_uid(jvxReportCommandRequest req,
		jvxComponentIdentification tp,
		jvxSize uid,
		CjvxAutomationReport::callSpecificParameters* params) override;

	/**
	 * Report state switch.
	 */
	virtual jvxErrorType handle_report_ss(
		jvxReportCommandRequest req,
		jvxComponentIdentification tp,
		jvxStateSwitch ss,
		callSpecificParameters* params) override;

	virtual jvxErrorType request_command(const CjvxReportCommandRequest& request)override;

	// ======================================================================================
	// Interface IjvxAutoDataConnect
	// ======================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION report_connection_factory_to_be_added(
		jvxComponentIdentification tp_activated,
		IjvxConnectorFactory* toadd		
	) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_connection_factory_removed(
		jvxComponentIdentification tp_activated,
		IjvxConnectorFactory* toremove		
	) override;

#include "codeFragments/simplify/jvxObjects_simplify.h"

#define JVX_STATEMACHINE_FULL_SKIP_INITIALIZE
#define JVX_STATE_MACHINE_DEFINE_SKIP_ACTIVATE_DEACTIVATE
#include "codeFragments/simplify/jvxStateMachineCore_simplify.h"
#undef JVX_STATEMACHINE_FULL_SKIP_INITIALIZE
#undef JVX_STATE_MACHINE_DEFINE_SKIP_ACTIVATE_DEACTIVATE

#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

	// Hidden interface to expose the "automation" interfaces
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION object_hidden_interface(IjvxObject** objRef) override;
};

#endif
