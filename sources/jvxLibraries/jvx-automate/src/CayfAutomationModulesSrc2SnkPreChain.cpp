#include "CayfAutomationModulesSrc2SnkPreChain.h"
#include "common/CjvxObjectLog.h"

namespace CayfAutomationModules
{

	CayfAutomationModulesSrc2SnkPreChain::CayfAutomationModulesSrc2SnkPreChain(): CayfAutomationModulesSrc2Snk()
	{

	}

	jvxErrorType 
		CayfAutomationModulesSrc2SnkPreChain::activate(IjvxReport* report,
		IjvxHost* host,
		ayfAutoConnect_callbacks* cb,
		jvxSize purpId,
		const ayfConnectConfigSrc2SnkPreChain& cfg,
		CjvxObjectLog* ptrLog)
	{
		jvxErrorType res = CayfAutomationModulesSrc2Snk::activate(report,
			host,
			cb,
			purpId,
			cfg,
			ptrLog);
		if (res == JVX_NO_ERROR)
		{
			this->oconNmSrcPreChain= cfg.oconNmSrcPreChain;
			this->iconNmEnterPreChain = cfg.iconNmEnterPreChain;
			this->tpEnter = cfg.tpEnter.tp;
			
			if (tpEnter.tp == JVX_COMPONENT_UNKNOWN)
			{
				tpEnter = cfg.tpInvolved;
			}
		}
		return res;
	}

	void
		CayfAutomationModulesSrc2SnkPreChain::deriveArguments(
			ayfConnectDerivedSrc2Snk& derivedArgs, const jvxComponentIdentification& tp_activated)
	{
		derivedArgs.tpSink = tp_activated;
		derivedArgs.tpSrc = config.tpInvolved;
		derivedArgs.tpMaster = tp_activated;
	}
	
	void 
		CayfAutomationModulesSrc2SnkPreChain::create_bridges(
			IjvxDataConnectionRule* theDataConnectionDefRuleHdl,
			jvxComponentIdentification tp_src,
			jvxComponentIdentification tp_sink,
			std::list<ayfConnectConfigCpEntryRuntime>& elm,
			const std::string& oconNameSrc,
			const std::string& iconNameSink,
			jvxSize& bridgeId,
			jvxSize segId,
			jvxSize oconIdTrigger,
			jvxSize iconIdTrigger)
	{
		if (segId == 0)
		{
			// ==================================================================================
			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Connect from <" << jvxComponentIdentification_txt(tp_sink) <<
				"> , connector <" << oconNmSrcPreChain << "> to <" << jvxComponentIdentification_txt(tpEnter) <<
				"> , connector <" << iconNmEnterPreChain << ">." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

			// First part of the connections
			CayfAutomationModulesSrc2Snk::create_bridges(
				theDataConnectionDefRuleHdl, tp_sink, tpEnter, elm,
				oconNmSrcPreChain, iconNmEnterPreChain, bridgeId, 0, oconIdTrigger, iconIdTriggerPreChain);


			// ==================================================================================
			// Forward all next stages
			// ==================================================================================
			CayfAutomationModulesSrc2Snk::create_bridges(
				theDataConnectionDefRuleHdl, tp_src, tp_sink, elm, config.oconNmSource,
				config.iconNmSink, bridgeId, 1, oconIdTriggerPreChain, iconIdTrigger);
		}
	}
}

