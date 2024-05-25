#include "CayfAutomationModulesCast.h"
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
		driveSupportNodeChain = cfgPriChain;		
		return res;
	}

#if 0
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
			IayfEstablishedProcessesSyncio& sglElm = *(reinterpret_cast<IayfEstablishedProcessesSyncio*>(sglElmPtr->specificType(CayfAutomationModules::ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SYNCIO)));

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
#endif

	jvxErrorType 
	CayfAutomationModulesSrc2SnkPreChainPriChain::activate_all_submodules(const jvxComponentIdentification& tp_activated)
	{		
		return CayfAutomationModulesSrc2SnkPreChain::activate_all_submodules(tp_activated);
	}

	IayfEstablishedProcessesCommon* 
	CayfAutomationModulesSrc2SnkPreChainPriChain::allocate_chain_realization()
	{
		CayfEstablishedProcessesSrc2SnkPreChainPriChain* newChainRealization = nullptr;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(newChainRealization, CayfEstablishedProcessesSrc2SnkPreChainPriChain);
		return newChainRealization;
	}	

	void 		
	CayfAutomationModulesSrc2SnkPreChainPriChain::deallocate_chain_realization(IayfEstablishedProcessesCommon* sglElmPtr)
	{
		return CayfAutomationModulesSrc2SnkPreChain::deallocate_chain_realization(sglElmPtr);
	}

	jvxErrorType
	CayfAutomationModulesSrc2SnkPreChainPriChain::pre_run_chain_prepare(IjvxObject* obj_dev, IayfEstablishedProcessesCommon* realizeChainArg)
	{
		jvxErrorType res = JVX_NO_ERROR;
		ayfConnectConfigCpEntrySyncIoRuntime cpElm(driveSupportNodeChain);

		IayfEstablishedProcessesSrc2SnkPreChainPriChain& realizeChain = castEstablishProcess< IayfEstablishedProcessesSrc2SnkPreChainPriChain>(realizeChainArg);

		cpElm.cpId = cpElm.cpTp;
		res = jvx_activateObjectInModule(refHostRefPtr, cpElm.cpId, cpElm.modName, obj_dev, true, cpElm.cpManipulate.manSuffix,
			cpElm.cpManipulate.attachUi, cpElm.cpManipulate.tpRemap);

		if (res == JVX_NO_ERROR)
		{
			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Activated module <" << cpElm.modName << "> with suffix <" << cpElm.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
		}
		else
		{
			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Failed to activate module <" << cpElm.modName << "> with suffix <" << cpElm.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
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

			res = CayfAutomationModulesSrc2SnkPreChain::pre_run_chain_prepare(obj_dev, realizeChainArg);
		}		

		return res;
	}

	jvxErrorType 
	CayfAutomationModulesSrc2SnkPreChainPriChain::post_run_chain_prepare(IayfEstablishedProcessesCommon* sglElmPtr)
	{
		IayfEstablishedProcessesSrc2SnkPreChainPriChain& sglElm = castEstablishProcess< IayfEstablishedProcessesSrc2SnkPreChainPriChain>(sglElmPtr);

		JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << "Deactivating  module <" << sglElm.supportNodeRuntime.modName << "> with suffix <" <<
			sglElm.supportNodeRuntime.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(sglElm.supportNodeRuntime.cpId) << ">." << std::endl;
		JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

		jvxErrorType res = jvx_deactivateObjectInModule(refHostRefPtr, sglElm.supportNodeRuntime.cpId);
		assert(res == JVX_NO_ERROR);

		if (sglElm.supportNodeRuntime.states.connectionsEstablishFlags != 0)
		{
			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_0_NORMAL_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "WARNING: The status of the established connectons is not 0 when all connections related to module <" << 
				sglElm.supportNodeRuntime.modName << "> are closed. This typically happens if the automation component does not forward the process disconnect message." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
		}
		sglElm.supportNodeRuntime.states.subModulesActive = false;

		return CayfAutomationModulesSrc2SnkPreChain::post_run_chain_prepare(sglElmPtr);
	}

	void
		CayfAutomationModulesSrc2SnkPreChainPriChain::pre_run_chain_connect(
			jvxComponentIdentification tp_reg,
			IjvxDataConnections* con, 
			IayfEstablishedProcessesCommon* sglElmPtr)
	{
		IayfEstablishedProcessesSrc2SnkPreChainPriChain& sglElm = castEstablishProcess< IayfEstablishedProcessesSrc2SnkPreChainPriChain>(sglElmPtr);
		// =================================================================================================================
		// Connect the primary chain
		// =================================================================================================================
		CayfAutomationModulesSyncedIoPrimaryMixIn::try_connect(con, tp_reg, sglElm.supportNodeRuntime, sglElmPtr->connectedProcesses(), *this, 
			static_cast<CayfAutomationModuleReportConnection*>(this), sglElmPtr);
	}

	void 
		CayfAutomationModulesSrc2SnkPreChainPriChain::deriveArguments(ayfConnectDerivedSrc2Snk& derivedArgs, 
			const jvxComponentIdentification& tp_activated, 
			IayfEstablishedProcessesCommon* sglElmPtr)
	{
		// Preset the derived configuration
		IayfEstablishedProcessesSrc2SnkPreChainPriChain& sglElm = castEstablishProcess< IayfEstablishedProcessesSrc2SnkPreChainPriChain>(sglElmPtr);
		
		derived.tpMaster = sglElm.supportNodeRuntime.cpId; // cpElm.cpId;
		derived.tpSink = sglElm.supportNodeRuntime.cpId;
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

	void 
		CayfAutomationModulesSrc2SnkPreChainPriChain::report_connection_established(jvxSize proId, ayfEstablishedProcessType tp, IayfEstablishedProcessesCommon* sglElmPtr)
	{
		IayfEstablishedProcessesSrc2SnkPreChainPriChain& sglElm = castEstablishProcess< IayfEstablishedProcessesSrc2SnkPreChainPriChain>(sglElmPtr);
		switch (tp)
		{
		case ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNKPRECHAINPRICHAIN:
			jvx_bitSet(sglElm.supportNodeRuntime.states.connectionsEstablishFlags, 0);
			sglElm.supportNodeRuntime.states.uidProcesses[0] = proId;
			break;
		case ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNK:
			jvx_bitSet(sglElm.supportNodeRuntime.states.connectionsEstablishFlags, 1);
			sglElm.supportNodeRuntime.states.uidProcesses[1] = proId;
			break;
		}
	}

	void
		CayfAutomationModulesSrc2SnkPreChainPriChain::report_to_be_disconnected(jvxSize uidProcess, IayfEstablishedProcessesCommon* sglElmPtr)
	{
		IayfEstablishedProcessesSrc2SnkPreChainPriChain& sglElm = castEstablishProcess< IayfEstablishedProcessesSrc2SnkPreChainPriChain>(sglElmPtr);
		if (uidProcess == sglElm.supportNodeRuntime.states.uidProcesses[0])
		{
			jvx_bitClear(sglElm.supportNodeRuntime.states.connectionsEstablishFlags, 0);
			sglElm.supportNodeRuntime.states.uidProcesses[0] = JVX_SIZE_UNSELECTED;
		}
		if (uidProcess == sglElm.supportNodeRuntime.states.uidProcesses[1])
		{
			jvx_bitClear(sglElm.supportNodeRuntime.states.connectionsEstablishFlags, 1);
			sglElm.supportNodeRuntime.states.uidProcesses[1] = JVX_SIZE_UNSELECTED;
		}

	}
}

