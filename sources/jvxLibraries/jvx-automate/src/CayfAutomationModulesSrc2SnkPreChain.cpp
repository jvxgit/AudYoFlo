#include "CayfAutomationModulesCast.h"
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
			config_prechain.connectedNodes = cfg.connectedNodes_prechain;
			
			if (tpEnter.tp == JVX_COMPONENT_UNKNOWN)
			{
				tpEnter = cfg.tpAssign;
			}
		}
		return res;
	}

	void
		CayfAutomationModulesSrc2SnkPreChain::deriveArguments(
			ayfConnectDerivedSrc2Snk& derivedArgs, 
			const jvxComponentIdentification& tp_activated,
			IayfEstablishedProcessesCommon* realizeChainArg)
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
			IayfEstablishedProcessesCommon* sglElmPtr,
			const std::string& oconNameSrc,
			const std::string& iconNameSink,
			jvxSize& bridgeId,			
			jvxSize oconIdTrigger,
			jvxSize iconIdTrigger)
	{
		// ==================================================================================
		JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << "Connect from <" << jvxComponentIdentification_txt(tp_sink) <<
			"> , connector <" << oconNmSrcPreChain << "> to <" << jvxComponentIdentification_txt(tpEnter) <<
			"> , connector <" << iconNmEnterPreChain << ">." << std::endl;
		JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

		IayfEstablishedProcessesSrc2SnkPreChain& sglElm = castEstablishProcess<IayfEstablishedProcessesSrc2SnkPreChain>(sglElmPtr);

		// First part of the connections  the prechain!!
		CayfAutomationModulesSrc2Snk::create_bridges(
			theDataConnectionDefRuleHdl, tp_sink, tpEnter, sglElm.lstEntries_prechain,
			sglElmPtr, oconNmSrcPreChain, iconNmEnterPreChain, bridgeId,  oconIdTriggerPreChain, iconIdTriggerPreChain);


		// ==================================================================================
		// Forward all next stages
		// ==================================================================================
		CayfAutomationModulesSrc2Snk::create_bridges(
			theDataConnectionDefRuleHdl, tp_src, tp_sink, elm, sglElmPtr,
			config.oconNmSource, config.iconNmSink, bridgeId, oconIdTrigger, iconIdTrigger);
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

	IayfEstablishedProcessesCommon* 
	CayfAutomationModulesSrc2SnkPreChain::allocate_chain_realization()
	{
		CayfEstablishedProcessesSrc2SnkPreChain* newChainRealization = nullptr;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(newChainRealization, CayfEstablishedProcessesSrc2SnkPreChain);
		return newChainRealization;
	}
	void 
		CayfAutomationModulesSrc2SnkPreChain::deallocate_chain_realization(IayfEstablishedProcessesCommon* ptr)
	{
		CayfAutomationModulesSrc2Snk::deallocate_chain_realization(ptr);
	}

	jvxErrorType
		CayfAutomationModulesSrc2SnkPreChain::pre_run_chain_prepare(IjvxObject* obj_dev, IayfEstablishedProcessesCommon* sglElmPtr)
	{
		jvxErrorType res = JVX_NO_ERROR;

		IayfEstablishedProcessesSrc2SnkPreChain& sglElm = castEstablishProcess<IayfEstablishedProcessesSrc2SnkPreChain>(sglElmPtr);
		// Do nothing here!

		for (auto& elmM : config_prechain.connectedNodes)
		{
			ayfConnectConfigCpEntryRuntime cpElm(elmM);

			cpElm.cpId = cpElm.cpTp;
			res = jvx_activateObjectInModule(refHostRefPtr, cpElm.cpId, cpElm.modName, obj_dev, true, cpElm.cpManipulate.manSuffix, cpElm.cpManipulate.attachUi, cpElm.cpManipulate.tpRemap);

			if (res == JVX_NO_ERROR)
			{
				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
				log << "Activated module <" << cpElm.modName << "> with suffix <" << cpElm.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

				sglElm.lstEntries_prechain.push_back(cpElm);
			}
			else
			{
				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
				log << "Failed to activate module <" << cpElm.modName << "> with suffix <" << cpElm.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
				break;
			}
		}

		if (res == JVX_NO_ERROR)
		{
			res = CayfAutomationModulesSrc2Snk::pre_run_chain_prepare(obj_dev, sglElmPtr);
		}
		return res;
	};

	jvxErrorType
		CayfAutomationModulesSrc2SnkPreChain::post_run_chain_prepare(IayfEstablishedProcessesCommon* sglElmPtr)
	{
		IayfEstablishedProcessesSrc2SnkPreChain& sglElm = castEstablishProcess<IayfEstablishedProcessesSrc2SnkPreChain>(sglElmPtr);
		/* 
		*(reinterpret_cast<IayfEstablishedProcessesSrc2SnkPreChain*>(
			sglElmPtr->specificType(CayfAutomationModules::ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNKPRECHAIN)));
		*/
		for (auto elmI : sglElm.lstEntries_prechain)
		{
			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Deactivating  module <" << elmI.modName << "> with suffix <" << elmI.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(elmI.cpId) << ">." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

			jvxErrorType res = jvx_deactivateObjectInModule(refHostRefPtr, elmI.cpId);
			assert(res == JVX_NO_ERROR);
		}
		sglElm.lstEntries_prechain.clear();

		CayfAutomationModulesSrc2Snk::post_run_chain_prepare(sglElmPtr);
		return JVX_NO_ERROR;
	}
}

