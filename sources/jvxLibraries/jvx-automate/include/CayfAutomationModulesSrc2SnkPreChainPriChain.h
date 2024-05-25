#ifndef _CAYFAUTOMATIONMODULESSRC2SNKPRECHAINPRICHAIN_H__
#define _CAYFAUTOMATIONMODULESSRC2SNKPRECHAINPRICHAIN_H__

#include "CayfAutomationModulesSrc2SnkPreChain.h"
#include "CayfAutomationModulesSyncedIo.h"

namespace CayfAutomationModules
{
	class IayfEstablishedProcessesSrc2SnkPreChainPriChain
	{
	public:

		// The extended version of the config struct
		ayfConnectConfigCpEntrySyncIoRuntime supportNodeRuntime;

		IayfEstablishedProcessesSrc2SnkPreChainPriChain(const ayfConnectConfigCpEntrySyncIoRuntime& supportNodeArg = ayfConnectConfigCpEntrySyncIoRuntime()) : supportNodeRuntime(supportNodeArg) {};

		void preset(const ayfConnectConfigCpEntrySyncIoRuntime& supportNodeArg)
		{
			supportNodeRuntime = supportNodeArg;
		}
	};

	class CayfEstablishedProcessesSrc2SnkPreChainPriChain : 
		public IayfEstablishedProcessesCommon, IayfEstablishedProcessesSrc2SnkPreChainPriChain,
		public IayfEstablishedProcessesSrc2SnkPreChain, public CayfEstablishedProcessesMixin
	{
	public:
		CayfEstablishedProcessesSrc2SnkPreChainPriChain() {};

		void preset(const ayfConnectConfigCpEntrySyncIoRuntime& supportNodeArg)
		{
			IayfEstablishedProcessesSrc2SnkPreChainPriChain::preset(supportNodeArg);
		}

		jvxHandle* specificType(ayfEstablishedProcessType tp)
		{
			switch (tp)
			{
			case ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNKPRECHAINPRICHAIN:
				return reinterpret_cast<jvxHandle*>(static_cast<IayfEstablishedProcessesSrc2SnkPreChainPriChain*>(this));
				break;
			case ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNKPRECHAIN:
				return reinterpret_cast<jvxHandle*>(static_cast<IayfEstablishedProcessesSrc2SnkPreChain*>(this));
				break;
			case ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNK:
				return reinterpret_cast<jvxHandle*>(static_cast<IayfEstablishedProcessesSrc2Snk*>(this));
				break;
			}
			return nullptr;
		}

		virtual std::list<ayfOneConnectedProcess>& connectedProcesses() override
		{
			return _connectedProcesses();
		}
	};

	// ================================================================================================
	// ================================================================================================

	class CayfEstablishedProcessesSrc2SnkPriChain : public IayfEstablishedProcessesCommon, 
		public IayfEstablishedProcessesSrc2Snk, public IayfEstablishedProcessesSyncio,
		public CayfAutomationModuleReportConnection, public CayfEstablishedProcessesMixin
	{
	public:
		virtual jvxHandle* specificType(ayfEstablishedProcessType tp) override
		{
			switch (tp)
			{
			case ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNK:
				return reinterpret_cast<jvxHandle*>(static_cast<IayfEstablishedProcessesSrc2Snk*>(this));
				break;
			case ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SYNCIO:
				return reinterpret_cast<jvxHandle*>(static_cast<IayfEstablishedProcessesSyncio*>(this));
				break;
			}
			return nullptr;
		};

		virtual std::list<ayfOneConnectedProcess>& connectedProcesses() override
		{
			return _connectedProcesses();
		}
	};

	class CayfAutomationModulesSrc2SnkPreChainPriChain: public CayfAutomationModulesSrc2SnkPreChain, public ayfConnectMultiConnectionsRuntime
	{
	protected:
		
		ayfConnectConfigSyncIo config_syncio;
		ayfConnectConfigCpEntrySyncIoRuntime supportNodeRuntime;

		// std::map<ayfOneModuleChainDefinition, ayfEstablishedProcessesCommon*> module_connections;

	public:
		
		// CayfAutomationModules::ayfConnectConfigSrc2SnkPreChain -> defines the pre-chain
		// CayfAutomationModules::ayfConnectConfigSrc2Snk-> defines the post-chain

		CayfAutomationModulesSrc2SnkPreChainPriChain();

		jvxErrorType activate(IjvxReport* report,
			IjvxHost* host,
			ayfAutoConnect_callbacks* cb,
			jvxSize purpId,
			const ayfConnectConfigCpEntrySyncIo& cfgPriChain,
			const ayfConnectConfigSrc2SnkPreChain& cfgSecChain,
			CjvxObjectLog* ptrLog = nullptr);

		// void try_connect(jvxComponentIdentification tp_reg, jvxBool& fullyEstablished)override;

		jvxErrorType activate_all_submodules(const jvxComponentIdentification& tp_activated);

		IayfEstablishedProcessesCommon* allocate_chain_realization()override;
		void deallocate_chain_realization(IayfEstablishedProcessesCommon*)override;
		void pre_run_chain_connect(jvxComponentIdentification tp_reg,
			IjvxDataConnections* con, IayfEstablishedProcessesCommon* realizeChain) override;

		// void deallocate_chain_realization(IayfEstablishedProcessesCommon* deleteMe)override; <- take from base class
		jvxErrorType pre_run_chain_prepare(IjvxObject* obj_dev, IayfEstablishedProcessesCommon* realizeChain) override;
		jvxErrorType post_run_chain_prepare(IayfEstablishedProcessesCommon* realizeChain) override;
		void deriveArguments(ayfConnectDerivedSrc2Snk& derivedArgs, const jvxComponentIdentification& tp_activated,
			IayfEstablishedProcessesCommon* sglElmPtr) override;

		void report_to_be_disconnected(jvxSize uidProcess, IayfEstablishedProcessesCommon* realizeChainPtr) override;
		virtual void report_connection_established(jvxSize proId, ayfEstablishedProcessType, IayfEstablishedProcessesCommon* realizeChainPtr) override;

		jvxErrorType on_connection_not_established(jvxComponentIdentification tp_activated, IayfEstablishedProcessesCommon* realizeChainPtr)override;
	};
}

#endif