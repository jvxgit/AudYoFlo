#ifndef __CAYFATTYPICAL_H__
#define __CAYFATTYPICAL_H__

#include "CjvxAutomationReportConnect.h"
#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"

#include "CayfAutomationModulesSyncedIo.h"
#include "CayfAutomationModulesSrc2SnkPreChainPriChain.h"

#include "pcg_exports_automation.h"

class CayfATTypical : public CjvxAutomationReportConnect,
	public IjvxProperties, public CjvxProperties, 
	public IjvxConfiguration,
	public genATTypical
{
protected:
	

public:
	CayfATTypical(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CayfATTypical();

	jvxErrorType select(IjvxObject* owner) override;
	jvxErrorType unselect() override;

	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	jvxErrorType handle_report_ss(
		jvxReportCommandRequest req,
		jvxComponentIdentification tp,
		jvxStateSwitch ss,
		CjvxAutomationReport::callSpecificParameters* params) override;

	// =======================================================================
	// These calls to auto-connect and auto-disconnect the components activated and deactivated
	// =======================================================================
	jvxErrorType report_connection_factory_to_be_added(
		jvxComponentIdentification tp_activated,
		IjvxConnectorFactory* toadd) override;

	jvxErrorType report_connection_factory_removed(
		jvxComponentIdentification tp_activated,
		IjvxConnectorFactory* toremove) override;

	//! We define this member function to catch the moment when all configuration has been done! Then, we will try to re-connect
	jvxErrorType request_command(const CjvxReportCommandRequest& request) override;
		
	// =====================================================================
	// Interfaces for CayfAutomationModules from <AudYoFlo>/sources/jvxLibraries/jvx-automate/include/CayfAutomationModules.h
	// =====================================================================
	
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"

	jvxErrorType request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;
	jvxErrorType return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;

	jvxErrorType put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1) override;

	jvxErrorType get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

	// ===========================================================================================
	jvxErrorType prepareAudioTechnologyOnStateSwitch(jvxStateSwitch ss, IjvxProperties* theProp, const std::string& lookforAudioTechWC);

};

#endif