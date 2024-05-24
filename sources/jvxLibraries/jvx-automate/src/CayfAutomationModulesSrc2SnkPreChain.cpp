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
			this->oconIdTriggerPreChain = cfg.oconIdTriggerPreChain;
			this->iconIdTriggerPreChain = cfg.iconIdTriggerPreChain;
			this->tpEnter = cfg.tpEnter;
			
			if (tpEnter.tp == JVX_COMPONENT_UNKNOWN)
			{
				tpEnter = cfg.tpAssign;
			}
		}
		return res;
	}

	void
		CayfAutomationModulesSrc2SnkPreChain::deriveArguments(
			ayfConnectDerivedSrc2Snk& derivedArgs, const jvxComponentIdentification& tp_activated)
	{
		// This is the sink of the post chain
		derivedArgs.tpSink = tp_activated;

		// This is the source of the post chain
		derivedArgs.tpSrc = config.tpAssign;

		// The master comes from the pre-chain
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

			// First part of the connections  the prechain!!
			CayfAutomationModulesSrc2Snk::create_bridges(
				theDataConnectionDefRuleHdl, tp_sink, tpEnter, elm,
				oconNmSrcPreChain, iconNmEnterPreChain, bridgeId, 0, oconIdTriggerPreChain, iconIdTriggerPreChain);


			// ==================================================================================
			// Forward all next stages
			// ==================================================================================
			CayfAutomationModulesSrc2Snk::create_bridges(
				theDataConnectionDefRuleHdl, tp_src, tp_sink, elm, config.oconNmSource,
				config.iconNmSink, bridgeId, 1, oconIdTrigger, iconIdTrigger);
		}
	}

	void 
		CayfAutomationModulesSrc2SnkPreChain::print(std::ostream& out)
	{
		jvxSize segId = 0;
		// First the pre-chain
		out << config.chainNamePrefix << std::endl;	

		std::string refIdIn;
		std::string refIdOut;
		if (JVX_CHECK_SIZE_SELECTED(oconIdTriggerPreChain))
		{
			refIdOut = "[" + jvx_size2String(oconIdTriggerPreChain) + "]";
		}
		out << "-->" << "<" << "TRIGGER" << ">#" << config.nmMaster << " + " << oconNmSrcPreChain << refIdOut << std::endl;

		printList(out, segId, config.connectedNodes);
		if (JVX_CHECK_SIZE_SELECTED(iconIdTriggerPreChain))
		{
			refIdIn = "[" + jvx_size2String(iconIdTriggerPreChain) + "]";
		}
		out << "-->" << jvxComponentIdentification_txt(tpEnter) << "+" << iconNmEnterPreChain << refIdIn << "*|-" << std::flush;

		segId++;
		refIdIn.clear();
		refIdOut.clear();

		if (JVX_CHECK_SIZE_SELECTED(config.oconIdTrigger))
		{
			refIdOut = "[" + jvx_size2String(config.oconIdTrigger) + "]";
		}
		out << "-|*" << "<" << jvxComponentIdentification_txt(config.tpAssign) << "+" << config.oconNmSource << refIdOut << std::endl;

		printList(out, segId, config.connectedNodes);
		if (JVX_CHECK_SIZE_SELECTED(config.iconIdTrigger))
		{
			refIdIn = "[" + jvx_size2String(config.iconIdTrigger) + "]";
		}
		out << "-->" << "TRIGGER" << "+" << config.iconNmSink << refIdIn << "*||" << std::flush;
	}
}

