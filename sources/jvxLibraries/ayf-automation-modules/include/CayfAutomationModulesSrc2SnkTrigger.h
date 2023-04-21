#ifndef _CAYFAUTOMATIONMODULESSRC2SNKTRIGGER_H__
#define _CAYFAUTOMATIONMODULESSRC2SNKTRIGGER_H__

#include "CayfAutomationModulesSrc2Snk.h"

namespace CayfAutomationModules
{
	class ayfConnectConfigTrigger : public ayfConnectConfig
	{
	public:
		std::string oconNmTrigger;
		std::string iconNmTrigger;

		ayfConnectConfigTrigger(
			const ayfConnectConfig& cfg,
			const std::string& oconNmTriggerArg = "default",
			const std::string& iconNmTriggerArg = "default") : ayfConnectConfig(cfg),
			oconNmTrigger(oconNmTriggerArg), iconNmTrigger(iconNmTriggerArg)
		{};


	};

	class CayfAutomationModulesSrc2SnkTrigger : public CayfAutomationModulesSrc2Snk
	{

	protected:
		std::string oconNmTrigger;
		std::string iconNmTrigger;
	public:
		CayfAutomationModulesSrc2SnkTrigger();

		jvxErrorType activate(IjvxReport* report,
			IjvxHost* host,
			ayfAutoConnect_callbacks* cb,
			jvxSize purpId,
			const ayfConnectConfigTrigger& cfg,
			ayfTriggerComponent trigCompTypeArg = ayfTriggerComponent::AYF_TRIGGER_COMPONENT_IS_SOURCE,
			CjvxObjectLog* ptrLog = nullptr);

		virtual jvxComponentIdentification& preset_master();

		void create_bridges(
			IjvxDataConnectionRule* theDataConnectionDefRuleHdl,
			jvxComponentIdentification tp_src,
			jvxComponentIdentification tp_sink,
			std::list<ayfConnectConfigCpEntryRuntime>& elm,
			jvxSize& bridgeId) override;
	};
}

#endif