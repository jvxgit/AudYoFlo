#ifndef __CAYFAUTOMATIONMODULESSEC2SNK_H__
#define __CAYFAUTOMATIONMODULESSEC2SNK_H__

#include "jvx.h"
#include "CayfAutomationModules.h"
#include "CayfAutomationModuleHandler.h"

class CjvxObjectLog;

namespace CayfAutomationModules
{
	class ayfOneModuleChainDefinition
	{
	public:
		ayfOneModuleChainDefinition(const jvxComponentIdentification& cpIdArg): 
			cpId(cpIdArg)
		{

		}
		jvxComponentIdentification cpId;
		bool operator < (const ayfOneModuleChainDefinition& elm) const
		{
			if (cpId < elm.cpId)
			{
				return true;
			}			
			return false;
		}
	};

	class ayfOneConnectedProcess
	{
	public:
		jvxSize processUid = JVX_SIZE_UNSELECTED;
		std::string chainName;
	};

	class ayfEstablishedProcesses
	{
	public:
		std::list<ayfConnectConfigCpEntryRuntime> lstEntries;
		std::list<ayfOneConnectedProcess> connectedProcesses;
	};

	class CayfAutomationModulesSrc2Snk: public CayfAutomationModuleHandlerIf
	{
	
	public:
		enum class ayfTriggerComponent
		{
			AYF_TRIGGER_COMPONENT_IS_SOURCE,
			AYF_TRIGGER_COMPONENT_IS_SINK
		};

	protected:



		// For each master, store the connected modules
		std::map<ayfOneModuleChainDefinition, ayfEstablishedProcesses> module_connections;
		IjvxReport* reportRefPtr = nullptr;
		IjvxHost* refHostRefPtr = nullptr;
		CjvxObjectLog* objLogRefPtr = nullptr;
		ayfAutoConnect_callbacks* cbPtr = nullptr;
		ayfConnectConfig config;
		ayfTriggerComponent trigCompType = ayfTriggerComponent::AYF_TRIGGER_COMPONENT_IS_SOURCE;
		jvxSize purposeId = JVX_SIZE_UNSELECTED;
	public:

		CayfAutomationModulesSrc2Snk()
		{
		};

		jvxErrorType activate(IjvxReport* report,
			IjvxHost* host,
			ayfAutoConnect_callbacks* cb,
			jvxSize purpId,
			const ayfConnectConfig& cfg,
			ayfTriggerComponent trigCompTypeArg = ayfTriggerComponent::AYF_TRIGGER_COMPONENT_IS_SOURCE,
			CjvxObjectLog* ptrLog = nullptr);
		jvxErrorType deactivate();

		void try_connect(
			jvxComponentIdentification tp_ident,
			jvxComponentIdentification tp_src,
			jvxComponentIdentification tp_sink);

		jvxErrorType associate_process(jvxSize uIdProcess,
			const std::string& nmChain) override;
		jvxErrorType deassociate_process(jvxSize uIdProcess) override;
		jvxErrorType activate_all_submodules(const jvxComponentIdentification& tp_activated) override;
		jvxErrorType deactivate_all_submodules(const jvxComponentIdentification& tp_deactivated) override;
		jvxErrorType adapt_all_submodules(jvxSize uIdProc, const std::string& modName,
			const jvxComponentIdentification& tp_activated) override;

		virtual jvxComponentIdentification& preset_master();

		virtual void create_bridges(
			IjvxDataConnectionRule* theDataConnectionDefRuleHdl, 
			jvxComponentIdentification tp_src,
			jvxComponentIdentification tp_sink,
			std::list<ayfConnectConfigCpEntryRuntime>& elm,
			jvxSize& bridgeId);
	};
}

#endif
