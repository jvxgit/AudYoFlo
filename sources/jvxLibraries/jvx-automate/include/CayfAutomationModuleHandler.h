#ifndef __CAYFAUTOMATIONMODULEHANDLER_H__
#define __CAYFAUTOMATIONMODULEHANDLER_H__

#include "jvx.h"

namespace CayfAutomationModules
{
	JVX_INTERFACE CayfAutomationModuleHandlerIf
	{
	public:
		virtual ~CayfAutomationModuleHandlerIf() {};

		virtual jvxErrorType adapt_all_submodules(
			jvxSize uIdProc,			
			const std::string& modName, const std::string& description,
			const jvxComponentIdentification& tpCp, jvxReportCommandRequest req) = 0;

		virtual jvxErrorType activate_all_submodules(const jvxComponentIdentification& tp_activated) = 0;
		virtual jvxErrorType deactivate_all_submodules(const jvxComponentIdentification& tp_deactivated) = 0;

		virtual jvxErrorType associate_process(jvxSize uIdProcess, 
			const std::string& nmChain) = 0;
		virtual jvxErrorType deassociate_process(jvxSize uIdProcess) = 0;

		virtual void postponed_try_connect() = 0;
	};

	// =============================================================

	class CayfAutomationModuleHandler
	{
	protected:
		std::list<CayfAutomationModuleHandlerIf*> registeredEntries;

	public:
		CayfAutomationModuleHandler();

		// Call this function on "activate" to add a module element to the list
		void register_element(CayfAutomationModuleHandlerIf* regMe);
		
		// Call this function on "deactivate" to remove a module element from the list
		void unregister_element(CayfAutomationModuleHandlerIf* regMe);

		// The following functions are called often but will act only seldomly.
		
		// Call this function on "report_connection_factory_to_be_added" to add required submodules and to connect the chain
		void try_activate_submodules(const jvxComponentIdentification& tpCp);

		// Call this function on "report_connection_factory_removed" to deactivate the involved submodules. This function is called
		// once the ..
		void try_deactivate_submodules(const jvxComponentIdentification& tpCp);

		// Once a process is reported to be connected, the name of the chain will be matched to all chains
		// connected via this class to find the appropriate one. Then, the process Id will be added.
		// Call this function in "report_connect_process" which is called from within "handle_report_uid"
		void try_associate_process(jvxSize uIdProcess, const std::string& nameChain);

		// Call this function is "report_disconnect_process" which is called from within "handle_report_uid"
		void try_deassociate_process(jvxSize uIdProcess);

		void try_adapt_submodules(jvxSize uIdProc, 
			const std::string& modName, const std::string& description,
			const jvxComponentIdentification& tpCp, jvxReportCommandRequest req);

		void report_configuration_done();
	};
}

#endif