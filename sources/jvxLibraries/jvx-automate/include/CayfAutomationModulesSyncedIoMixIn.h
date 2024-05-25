#ifndef __CAYFAUTOMATIONMODULESSYNCEDIOMIXIN_H__
#define __CAYFAUTOMATIONMODULESSYNCEDIOMIXIN_H__

#include "jvx.h"
#include "CayfAutomationModules.h"
#include "CayfAutomationModuleHandler.h"

class CjvxObjectLog;

namespace CayfAutomationModules
{
	class ayfConnectConfigCpEntrySyncIoRuntime;
	class CayfAutomationModulesSyncedIoPrimaryMixIn
	{
	protected:
		CayfAutomationModuleReportConnection* report = nullptr;
	public:
		static void try_connect(
			IjvxDataConnections* con,
			jvxComponentIdentification tp_reg,
			ayfConnectConfigCpEntrySyncIoRuntime& supportNodeRuntime,
			std::list<ayfOneConnectedProcess>& connectedProcesses,
			CayfAutomationModulesCommon& modCommon,
			CayfAutomationModuleReportConnection* report,
			IayfEstablishedProcessesCommon* realizeChainPtr);
	};
};

#endif

