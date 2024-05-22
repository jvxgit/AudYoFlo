#ifndef _CAYFAUTOMATIONMODULESSRC2SNKPRECHAINPRICHAIN_H__
#define _CAYFAUTOMATIONMODULESSRC2SNKPRECHAINPRICHAIN_H__

#include "CayfAutomationModulesSrc2SnkPreChain.h"
#include "CayfAutomationModulesSyncedIo.h"

namespace CayfAutomationModules
{
	class CayfAutomationModulesSrc2SnkPreChainPriChain: public CayfAutomationModulesSrc2SnkPreChain
	{
	protected:
		ayfConnectConfigCpEntrySyncIo driveSupportNodeChain;

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
	};
}

#endif