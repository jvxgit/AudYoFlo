#ifndef _CAYFAUTOMATIONMODULESSRC2SNKTRIGGER_H__
#define _CAYFAUTOMATIONMODULESSRC2SNKTRIGGER_H__

#include "CayfAutomationModulesSrc2Snk.h"

namespace CayfAutomationModules
{
	/* These classes extend the base functionality to add a pre-chain. The diagram is as follows 
	* [SNK -> <MOD1.1> <MOD1.2> -> ENTER ->] <-ALTERNATIVE LINK-> [->SRC ->  <MOD1> -> <MOD2> -> .. -> <MODN> -> SNK]
	* The first part is the pre-chain, the second part a "normal" Src2Snk chain.
	* The following are the additional arguments:
	* 1) cfg to configure the second part chain. Note that src is the involved component whereas snk is the trigger component
	* 2) 
	 */
	class ayfConnectConfigSrc2SnkPreChain : public ayfConnectConfigSrc2Snk
	{
	public:
		std::string oconNmSrcPreChain;
		std::string iconNmEnterPreChain;
		jvxComponentIdentification tpEnter = JVX_COMPONENT_UNKNOWN;

		ayfConnectConfigSrc2SnkPreChain(
			const ayfConnectConfigSrc2Snk& cfg,

			// Name of the output 
			const std::string& oconNmSrcPreChainArg = "default",
			const std::string& iconNmTriggerArg = "default",
			const jvxComponentIdentification& cpEnterArg = JVX_COMPONENT_UNKNOWN) : ayfConnectConfigSrc2Snk(cfg),
			oconNmSrcPreChain(oconNmSrcPreChainArg), iconNmEnterPreChain(iconNmTriggerArg)
		{};


	};

	class CayfAutomationModulesSrc2SnkPreChain : public CayfAutomationModulesSrc2Snk
	{

	protected:
		std::string oconNmSrcPreChain = "default";
		std::string iconNmEnterPreChain = "default";
		jvxComponentIdentification tpEnter = JVX_COMPONENT_UNKNOWN;
		
	public:
		CayfAutomationModulesSrc2SnkPreChain();

		jvxErrorType activate(IjvxReport* report,
			IjvxHost* host,
			ayfAutoConnect_callbacks* cb,
			jvxSize purpId,
			const ayfConnectConfigSrc2SnkPreChain& cfg,
			CjvxObjectLog* ptrLog = nullptr);

		void create_bridges(
			IjvxDataConnectionRule* theDataConnectionDefRuleHdl,
			jvxComponentIdentification tp_src,
			jvxComponentIdentification tp_sink,
			std::list<ayfConnectConfigCpEntryRuntime>& elm,
			const std::string& oconNameSrc,
			const std::string& iconNameSink,
			jvxSize& bridgeId,
			jvxSize segId = 0) override;

		void deriveArguments(ayfConnectDerivedSrc2Snk& derivedArgs, const jvxComponentIdentification& tp_activated) override;
	};
}

#endif