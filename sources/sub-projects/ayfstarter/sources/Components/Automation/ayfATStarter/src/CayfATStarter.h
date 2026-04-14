#ifndef __CAYFATSTARTER_H__
#define __CAYFATSTARTER_H__

#include "CjvxAutomationReportConnect.h"
#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"

#include "CayfAutomationModulesSyncedIo.h"
#include "CayfAutomationModulesSrc2SnkPreChainPriChain.h"

#include "pcg_exports_automation.h"

class CayfATStarter : public CjvxAutomationReportConnect,
	public CayfAutomationModules::ayfAutoConnect_callbacks,
	public IjvxProperties, public CjvxProperties, 
	public IjvxConfiguration,
	public genATStarter
{
protected:
	
	jvxSize phoneConnections = 0;
	IjvxProperties* propsMixerNode = nullptr;

	// ==========================================================================
	// Audio input/output and output
	// ==========================================================================
	CayfAutomationModules::CayfAutomationModulesSrc2SnkPreChainPriChain ioAudioWithModules;

	// ==========================================================================
	// Audiofile input
	// ==========================================================================
	CayfAutomationModules::CayfAutomationModulesSrc2Snk iSources;

	// ==========================================================================
	// Audiofile output
	// ==========================================================================
	CayfAutomationModules::CayfAutomationModulesSrc2SnkPreChain oSinks;

	// ==========================================================================
	// Video I/O
	// ==========================================================================
	CayfAutomationModules::CayfAutomationModulesSrc2SnkPreChain ioVideo;


public:
	CayfATStarter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CayfATStarter();

	jvxErrorType activate();
	jvxErrorType deactivate();

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
	jvxErrorType adapt_single_property_on_event(jvxSize purposeId,
		const std::string& nmChain, const std::string& modName,
		const std::string& description, jvxReportCommandRequest req, IjvxProperties* props) override;

	//! Callback to allow trigger of autoconnection rule. Purpose ID provides the ID as installed, tpIdTrigger allows to filter upon which component to act.
	jvxErrorType allow_master_connect(jvxSize purposeId, jvxComponentIdentification tpIdTrigger) override;
	
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"

	jvxErrorType request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;
	jvxErrorType return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;

	jvxErrorType put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1) override;

	jvxErrorType get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

};

#endif