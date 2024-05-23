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
			ayfAutoConnectSrc2Snk_callbacks* cb,
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
		config = ayfConnectConfigSyncIo(
			"", "default",
			"default", "default", CayfAutomationModules::ayfConnectConfigCpConInChain(),
			CayfAutomationModules::ayfConnectConfigCpConInChain(),
			CayfAutomationModules::ayfConnectConfigConMiscArgs(),
			cfgPriChain);

		return res;
	}

	jvxErrorType 
	CayfAutomationModulesSrc2SnkPreChainPriChain::activate_all_submodules(const jvxComponentIdentification& tp_activated)
	{
		
		return CayfAutomationModulesSrc2SnkPreChain::activate_all_submodules(tp_activated);
	}

	IayfEstablishedProcessesCommon* 
	CayfAutomationModulesSrc2SnkPreChainPriChain::allocate_chain_realization(jvxHandle* cpElm)
	{
		CayfEstablishedProcessesSrc2SnkPriChain* newChainRealization = nullptr;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(newChainRealization, CayfEstablishedProcessesSrc2SnkPriChain);
		return newChainRealization;
	}	

	void 
	CayfAutomationModulesSrc2SnkPreChainPriChain::pre_connect_support_components(IjvxObject* obj_dev, IayfEstablishedProcessesCommon* realizeChainArg)
	{
		jvxErrorType res = JVX_NO_ERROR;
		ayfConnectConfigCpEntrySyncIoRuntime cpElm(config);

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
			realizeChain.supportNodeRuntime.states.subModulesActive = true;
			realizeChain.supportNodeRuntime.states.connectionsEstablishFlags = 0x0;
			realizeChain.supportNodeRuntime.derivedConfig.allowPostPonedConnect = this->allowPostPonedConnect;
			realizeChain.supportNodeRuntime.derivedConfig.targetFlagsConnection = this->targetFlagsConnection;
		}
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

