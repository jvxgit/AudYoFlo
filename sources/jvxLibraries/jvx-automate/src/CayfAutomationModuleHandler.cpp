#include "CayfAutomationModuleHandler.h"

namespace CayfAutomationModules
{
	CayfAutomationModuleHandler::CayfAutomationModuleHandler()
	{

	}

	void
		CayfAutomationModuleHandler::register_element(CayfAutomationModuleHandlerIf* regMe)
	{
		registeredEntries.push_back(regMe);
	}

	void
		CayfAutomationModuleHandler::unregister_element(CayfAutomationModuleHandlerIf* regMe)
	{
		auto elm = registeredEntries.begin();
		for (; elm != registeredEntries.end(); elm++)
		{
			if (*elm == regMe)
			{
				registeredEntries.erase(elm);
				break;
			}
		}
	}

	void 
		CayfAutomationModuleHandler::try_associate_process(jvxSize uIdProcess, const std::string& nmChain)
	{
		for (auto& elm : registeredEntries)
		{
			elm->associate_process(uIdProcess, nmChain);
		}
	}
	
	void 
		CayfAutomationModuleHandler::try_deassociate_process(jvxSize uIdProcess)
	{
		for (auto& elm : registeredEntries)
		{
			elm->deassociate_process(uIdProcess);
		}
	}

	void
		CayfAutomationModuleHandler::try_adapt_submodules(jvxSize uIdProc, 
			const std::string& modName, const std::string& description, 
			const jvxComponentIdentification& tpCp, jvxReportCommandRequest req)
	{
		for (auto& elm : registeredEntries)
		{
			elm->adapt_all_submodules(uIdProc, modName, description, tpCp, req);
		}
	}
	void
		CayfAutomationModuleHandler::try_activate_submodules(const jvxComponentIdentification& tpCp)
	{
		for (auto& elm : registeredEntries)
		{
			elm->activate_all_submodules(tpCp);
		}
	}

	void
		CayfAutomationModuleHandler::try_deactivate_submodules(const jvxComponentIdentification& tpCp)
	{
		for (auto& elm : registeredEntries)
		{
			elm->deactivate_all_submodules(tpCp);
		}

	}

	void 
		CayfAutomationModuleHandler::report_configuration_done()
	{
		for (auto& elm : registeredEntries)
		{
			elm->postponed_try_connect();
		}
	}
};

