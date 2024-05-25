#ifndef __CAYFAUTOMATIONMODULESPRIVATE_H__
#define __CAYFAUTOMATIONMODULESPRIVATE_H__

#include "jvx.h"
#include "CayfAutomationModules.h"
#include "CayfAutomationModuleHandler.h"
#include "CayfAutomationModulesSyncedIoMixIn.h"
#include "CayfAutomationModulesSyncedIo.h"
#include "CayfAutomationModulesSrc2SnkPreChain.h"

class CjvxObjectLog;

namespace CayfAutomationModules
{
	// =====================================================================================================

	class IayfEstablishedProcessesSyncio
	{
	public:

		// The extended version of the config struct
		ayfConnectConfigCpEntrySyncIoRuntime supportNodeRuntime;

		IayfEstablishedProcessesSyncio(const ayfConnectConfigCpEntrySyncIoRuntime& supportNodeArg = ayfConnectConfigCpEntrySyncIoRuntime()) : supportNodeRuntime(supportNodeArg) {};

		void preset(const ayfConnectConfigCpEntrySyncIoRuntime& supportNodeArg)
		{
			supportNodeRuntime = supportNodeArg;
		}
	};

	class CayfEstablishedProcessesSyncio : public IayfEstablishedProcessesCommon, public IayfEstablishedProcessesSyncio, public CayfEstablishedProcessesMixin
	{
	public:
		CayfEstablishedProcessesSyncio(const ayfConnectConfigCpEntrySyncIoRuntime& supportNodeArg = ayfConnectConfigCpEntrySyncIoRuntime()) : IayfEstablishedProcessesSyncio(supportNodeArg) {};

		void preset(const ayfConnectConfigCpEntrySyncIoRuntime& supportNodeArg)
		{
			IayfEstablishedProcessesSyncio::preset(supportNodeArg);
		}

		jvxHandle* specificType(ayfEstablishedProcessType tp)
		{
			switch (tp)
			{
			case ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SYNCIO:
				return reinterpret_cast<jvxHandle*>(static_cast<IayfEstablishedProcessesSyncio*>(this));
				break;
			}
			return nullptr;
		}

		virtual std::list<ayfOneConnectedProcess>& connectedProcesses() override
		{
			return _connectedProcesses();
		}
	};

	// ======================================================================================

	class IayfEstablishedProcessesSrc2SnkPreChain : public IayfEstablishedProcessesSrc2Snk
	{
	public:
		std::list<ayfConnectConfigCpEntryRuntime> lstEntries_prechain;
	};

	class CayfEstablishedProcessesSrc2SnkPreChain : public IayfEstablishedProcessesCommon, public IayfEstablishedProcessesSrc2SnkPreChain, public CayfEstablishedProcessesMixin
	{
		virtual jvxHandle* specificType(ayfEstablishedProcessType tp) override
		{
			switch (tp)
			{
			case ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNK:
				return reinterpret_cast<jvxHandle*>(static_cast<IayfEstablishedProcessesSrc2Snk*>(this));
				break;
			case ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNKPRECHAIN:
				return reinterpret_cast<jvxHandle*>(static_cast<IayfEstablishedProcessesSrc2SnkPreChain*>(this));
				break;
			}
			return nullptr;
		}

		virtual std::list<ayfOneConnectedProcess>& connectedProcesses() override
		{
			return _connectedProcesses();
		}
	};

	// ===========================================================================================================

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

	/*
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
	*/

};

#endif