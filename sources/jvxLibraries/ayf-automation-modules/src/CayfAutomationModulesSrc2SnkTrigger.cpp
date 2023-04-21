#include "CayfAutomationModulesSrc2SnkTrigger.h"
#include "common/CjvxObjectLog.h"

namespace CayfAutomationModules
{

	CayfAutomationModulesSrc2SnkTrigger::CayfAutomationModulesSrc2SnkTrigger(): CayfAutomationModulesSrc2Snk()
	{

	}

	jvxErrorType 
		CayfAutomationModulesSrc2SnkTrigger::activate(IjvxReport* report,
		IjvxHost* host,
		ayfAutoConnect_callbacks* cb,
		jvxSize purpId,
		const ayfConnectConfigTrigger& cfg,
		ayfTriggerComponent trigCompTypeArg,
		CjvxObjectLog* ptrLog)
	{
		jvxErrorType res = CayfAutomationModulesSrc2Snk::activate(report,
			host,
			cb,
			purpId,
			cfg,
			ayfTriggerComponent::AYF_TRIGGER_COMPONENT_IS_SINK,
			ptrLog);
		if (res == JVX_NO_ERROR)
		{
			this->oconNmTrigger = cfg.oconNmTrigger;
			this->iconNmTrigger = cfg.iconNmTrigger;
		}
		return res;
	}

	jvxComponentIdentification&
		CayfAutomationModulesSrc2SnkTrigger::preset_master()
	{
		return config.tpSink;
	}

	void 
		CayfAutomationModulesSrc2SnkTrigger::create_bridges(
			IjvxDataConnectionRule* theDataConnectionDefRuleHdl,
			jvxComponentIdentification tp_src,
			jvxComponentIdentification tp_sink,
			std::list<ayfConnectConfigCpEntryRuntime>& elm,
			jvxSize& bridgeId)
	{

		// ==================================================================================
		JVX_START_LOCK_LOG_REF(objLogRefPtr, 3);
		log << "Connect from <" << jvxComponentIdentification_txt(tp_sink) <<
			"> , connector <" << oconNmTrigger << "> to <" << jvxComponentIdentification_txt(tp_src) <<
			"> , connector <" << iconNmTrigger << ">." << std::endl;
		JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

		jvxErrorType res = theDataConnectionDefRuleHdl->add_bridge_specification(
			tp_sink,
			"*", oconNmTrigger.c_str(),
			tp_src,
			"*", iconNmTrigger.c_str(),
			("Bridge_" + jvx_size2String(bridgeId)).c_str());
		assert(res == JVX_NO_ERROR);
		bridgeId++;

		// ==================================================================================
		// Forward all next stages
		// ==================================================================================
		CayfAutomationModulesSrc2Snk::create_bridges(
			theDataConnectionDefRuleHdl, tp_src, tp_sink, elm, bridgeId);
	}
}

