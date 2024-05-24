#include "CayfAutomationModulesSrc2SnkPreChainPriChain.h"
#include "common/CjvxObjectLog.h"

namespace CayfAutomationModules
{
	CayfAutomationModulesSrc2SnkPreChainPriChain::CayfAutomationModulesSrc2SnkPreChainPriChain() : CayfAutomationModulesSrc2SnkPreChain()
	{
	}

	jvxErrorType
	CayfAutomationModulesSrc2SnkPreChainPriChain::activate(
			IjvxReport* report,
			IjvxHost* host,
			ayfAutoConnect_callbacks* cb,
			jvxSize purpId,
			const ayfConnectConfigCpEntrySyncIo& cfgPriChain,
			const ayfConnectConfigSrc2SnkPreChain& cfgSecChain,
			CjvxObjectLog* ptrLog)
	{
		jvxErrorType res = JVX_NO_ERROR; 
		
		res = CayfAutomationModulesSrc2SnkPreChain::activate(report,
			host,cb,purpId,			
			cfgSecChain, ptrLog);
		
		// Setup a dummy configuration
		config_syncio = ayfConnectConfigSyncIo(
			"", "default",
			"default", "default", CayfAutomationModules::ayfConnectConfigCpConInChain(),
			CayfAutomationModules::ayfConnectConfigCpConInChain(),
			CayfAutomationModules::ayfConnectConfigConMiscArgs(),
			cfgPriChain);

		return res;
	}

	void
		CayfAutomationModulesSrc2SnkPreChainPriChain::try_connect(jvxComponentIdentification tp_reg, jvxBool& fullyEstablished)
	{
		// Here, the audio device has been activated.
		jvxSize proc_id = JVX_SIZE_UNSELECTED;
		jvxBool rep_global = false;
		jvxSize bridgeId = 0;

		fullyEstablished = false;
		jvxBool establishedLoc = false;

		// Here, the audio device has been activated.
		// At this point, we need to run the connection

		jvxErrorType res = JVX_NO_ERROR;
		jvxSize theDataConnectionDefRule_id = JVX_SIZE_UNSELECTED;
		IjvxDataConnections* con = nullptr;

		jvxErrorType resC = JVX_NO_ERROR;
		con = reqInterface<IjvxDataConnections>(refHostRefPtr);
		if (con)
		{
			// Get the creation rule!
			auto elm = module_connections.find(tp_reg);
			assert(elm != module_connections.end());

			IayfEstablishedProcessesCommon* sglElmPtr = elm->second;
			IayfEstablishedProcessesSyncio& sglElm = *sglElmPtr->syncIoRefRef();

			// Check if this part must be connected!!

			// =================================================================================================================
			// Connect the primary chain
			// =================================================================================================================
			CayfAutomationModulesSyncedIoPrimaryMixIn::try_connect(con, tp_reg, sglElm.supportNodeRuntime, sglElmPtr->connectedProcesses(), *this);

			// =================================================================================================================
			// Connect the secondary chain
			// =================================================================================================================

			if (!jvx_bitTest(sglElm.supportNodeRuntime.states.connectionsEstablishFlags, 1))
			{
				// Allocate the secondary connections
				jvxBool established = false;
				
				// derived.
				CayfAutomationModulesSrc2Snk::try_connect(tp_reg, established);
				//CayfAutomationModulesSrc2Snk::try_connect(sglElm.supportNodeRuntime.cpId, &established);
			}
		}
	}


	jvxErrorType 
	CayfAutomationModulesSrc2SnkPreChainPriChain::activate_all_submodules(const jvxComponentIdentification& tp_activated)
	{
		
		return CayfAutomationModulesSrc2SnkPreChain::activate_all_submodules(tp_activated);
	}

	IayfEstablishedProcessesCommon* 
	CayfAutomationModulesSrc2SnkPreChainPriChain::allocate_chain_realization()
	{
		CayfEstablishedProcessesSrc2SnkPriChain* newChainRealization = nullptr;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(newChainRealization, CayfEstablishedProcessesSrc2SnkPriChain);
		return newChainRealization;
	}	

	void 
	CayfAutomationModulesSrc2SnkPreChainPriChain::pre_connect_support_components(IjvxObject* obj_dev, IayfEstablishedProcessesCommon* realizeChainArg)
	{
		jvxErrorType res = JVX_NO_ERROR;
		ayfConnectConfigCpEntrySyncIoRuntime cpElm(config_syncio);

		IayfEstablishedProcessesSyncio& realizeChain = *(realizeChainArg->syncIoRefRef());

		cpElm.cpId = cpElm.driveSupportNodeChain.cpTp;
		res = jvx_activateObjectInModule(refHostRefPtr, cpElm.cpId, cpElm.driveSupportNodeChain.modName, obj_dev, true, cpElm.driveSupportNodeChain.cpManipulate.manSuffix,
			cpElm.driveSupportNodeChain.cpManipulate.attachUi, cpElm.driveSupportNodeChain.cpManipulate.tpRemap);

		if (res == JVX_NO_ERROR)
		{
			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Activated module <" << cpElm.driveSupportNodeChain.modName << "> with suffix <" << cpElm.driveSupportNodeChain.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
		}
		else
		{
			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Failed to activate module <" << cpElm.driveSupportNodeChain.modName << "> with suffix <" << cpElm.driveSupportNodeChain.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
		}

		if (res == JVX_NO_ERROR)
		{
			// New module has been activated!!
			jvxBool established = false;
			
			// Slightly postponed
			realizeChain.preset(cpElm);

			realizeChain.supportNodeRuntime.states.subModulesActive = true;
			realizeChain.supportNodeRuntime.states.connectionsEstablishFlags = 0x0;
			realizeChain.supportNodeRuntime.derivedConfig.allowPostPonedConnect = this->allowPostPonedConnect;
			realizeChain.supportNodeRuntime.derivedConfig.targetFlagsConnection = this->targetFlagsConnection;
		}

		// Preset the derived configuration
		derived.tpMaster = cpElm.cpId;
		derived.tpSink = cpElm.cpId;
		derived.tpSrc = CayfAutomationModulesSrc2Snk::config.tpAssign;
	}

	jvxErrorType
		CayfAutomationModulesSrc2SnkPreChainPriChain::on_connection_not_established(jvxComponentIdentification tp_activated, IayfEstablishedProcessesCommon* realizeChainPtr)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (!allowPostPonedConnect)
		{
			// If we do not establish the connection we can deactivate the added objects
			module_connections.erase(tp_activated);
			this->deallocate_chain_realization(realizeChainPtr);
			res = JVX_ERROR_INVALID_SETTING;
		}
		return res;
	}
}

