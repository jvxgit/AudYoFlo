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
		jvxErrorType res = CayfAutomationModulesSrc2SnkPreChain::activate(report,
			host,cb,purpId,cfgSecChain, ptrLog);
		driveSupportNodeChain = cfgPriChain;
		return res;
	}

	jvxErrorType 
		CayfAutomationModulesSrc2SnkPreChainPriChain::activate_all_submodules(const jvxComponentIdentification& tp_activated)
	{
		return CayfAutomationModulesSrc2SnkPreChain::activate_all_submodules(tp_activated);
	}
}

