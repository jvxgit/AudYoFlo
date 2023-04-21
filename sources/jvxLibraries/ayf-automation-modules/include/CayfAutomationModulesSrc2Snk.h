#ifndef __CAYFAUTOMATIONMODULESSEC2SNK_H__
#define __CAYFAUTOMATIONMODULESSEC2SNK_H__

#include "jvx.h"
#include "CayfAutomationModules.h"

class CjvxObjectLog;

namespace CayfAutomationModules
{
	class ayfEstablishedOneChain
	{
	public:
		std::list<ayfConnectConfigCpEntryRuntime> lstEntries;
		std::string chainName;
	};

	class CayfAutomationModulesSrc2Snk
	{
	
	public:
		enum class ayfTriggerComponent
		{
			AYF_TRIGGER_COMPONENT_IS_SOURCE,
			AYF_TRIGGER_COMPONENT_IS_SINK
		};

	protected:



		// For each master, store the connected modules
		std::map<jvxComponentIdentification, ayfEstablishedOneChain> ids_sub_components_file_in;
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
			CjvxObjectLog* ptrLog = nullptr)
		{
			reportRefPtr = report;
			refHostRefPtr = host;
			cbPtr = cb;
			purposeId = purpId;
			config = cfg;
			trigCompType = trigCompTypeArg;
			objLogRefPtr = ptrLog;
			return JVX_NO_ERROR;
		}

		jvxErrorType deactivate()
		{
			assert(ids_sub_components_file_in.size() == 0);
			// If we end the program, we may reach here without explicitely deactivatin a single chain.
			// The reason is that we disconnect automation before deactivating non-default components
			/*
			for (auto& elm : ids_sub_components_file_in)
			{
				deactivate_all_submodule_audio_input(elm.first);
			}
			*/

			reportRefPtr = nullptr;
			refHostRefPtr = nullptr;
			cbPtr = nullptr;
			purposeId = JVX_SIZE_UNSELECTED;
			config.chainNamePrefix.clear();
			config.connectedNodes.clear();
			objLogRefPtr = nullptr;
			trigCompType = ayfTriggerComponent::AYF_TRIGGER_COMPONENT_IS_SOURCE;
			return JVX_NO_ERROR;
		}

		void try_connect(
			jvxComponentIdentification tp_ident,
			jvxComponentIdentification tp_src,
			jvxComponentIdentification tp_sink);

		jvxErrorType activate_all_submodules(const jvxComponentIdentification& tp_activated);
		jvxErrorType deactivate_all_submodules(const jvxComponentIdentification& tp_deactivated);
		jvxErrorType adapt_all_submodules(const std::string& modName, jvxComponentIdentification tpCp, const std::string& chainName);

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
