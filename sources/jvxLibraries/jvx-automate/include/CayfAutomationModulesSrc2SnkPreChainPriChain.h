#ifndef _CAYFAUTOMATIONMODULESSRC2SNKPRECHAINPRICHAIN_H__
#define _CAYFAUTOMATIONMODULESSRC2SNKPRECHAINPRICHAIN_H__

#include "CayfAutomationModulesSrc2SnkPreChain.h"
#include "CayfAutomationModulesSyncedIo.h"

namespace CayfAutomationModules
{
	class CayfEstablishedProcessesSrc2SnkPriChain : public IayfEstablishedProcessesCommon, 
		public IayfEstablishedProcessesSrc2Snk, public IayfEstablishedProcessesSyncio,
		public CayfEstablishedProcessesMixin
	{
	public:
		virtual IayfEstablishedProcessesSrc2Snk* src2SnkRef() override
		{
			return this;
		};

		virtual IayfEstablishedProcessesSyncio* syncIoRefRef() override
		{
			return this;
		};
		virtual std::list<ayfOneConnectedProcess>& connectedProcesses() override
		{
			return _connectedProcesses();
		}
	};

	class CayfAutomationModulesSrc2SnkPreChainPriChain: public CayfAutomationModulesSrc2SnkPreChain, public ayfConnectMultiConnectionsRuntime
	{
	protected:
		
		ayfConnectConfigSyncIo config;
		ayfConnectConfigCpEntrySyncIoRuntime supportNodeRuntime;

		// std::map<ayfOneModuleChainDefinition, ayfEstablishedProcessesCommon*> module_connections;

	public:
		
		// CayfAutomationModules::ayfConnectConfigSrc2SnkPreChain -> defines the pre-chain
		// CayfAutomationModules::ayfConnectConfigSrc2Snk-> defines the post-chain

		CayfAutomationModulesSrc2SnkPreChainPriChain();

		jvxErrorType activate(IjvxReport* report,
			IjvxHost* host,
			ayfAutoConnectSrc2Snk_callbacks* cb,
			jvxSize purpId,
			const ayfConnectConfigCpEntrySyncIo& cfgPriChain,
			const ayfConnectConfigSrc2SnkPreChain& cfgSecChain,
			CjvxObjectLog* ptrLog = nullptr);

		jvxErrorType activate_all_submodules(const jvxComponentIdentification& tp_activated);

		IayfEstablishedProcessesCommon* allocate_chain_realization(jvxHandle* cpElm = nullptr)override;
		// void deallocate_chain_realization(IayfEstablishedProcessesCommon* deleteMe)override; <- take from base class
		void pre_connect_support_components(IjvxObject* obj_dev, IayfEstablishedProcessesCommon* realizeChain) override;
		jvxErrorType on_connection_not_established(jvxComponentIdentification tp_activated, IayfEstablishedProcessesCommon* realizeChainPtr)override;
	};
}

#endif