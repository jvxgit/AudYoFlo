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
		std::list<ayfConnectConfigCpEntry> connectedNodes_prechain;
		std::string oconNmSrcPreChain;
		std::string iconNmEnterPreChain;
		jvxComponentIdentification tpEnter = JVX_COMPONENT_UNKNOWN;
		jvxSize oconIdTriggerPreChain = JVX_SIZE_UNSELECTED;
		jvxSize iconIdTriggerPreChain = JVX_SIZE_UNSELECTED;

		ayfConnectConfigSrc2SnkPreChain(
			// Name of the output 
			const std::string& oconNmSrcPreChainArg,
			const std::list<ayfConnectConfigCpEntry>& connectedNodesArg,
			const std::string& iconNmTriggerArg,
			const jvxComponentIdentification& cpEnterArg,
			const ayfConnectConfigSrc2Snk& cfg,
			jvxSize oconIdTriggerPreChainArg = JVX_SIZE_UNSELECTED,
			jvxSize iconIdTriggerPreChainArg = JVX_SIZE_UNSELECTED) : ayfConnectConfigSrc2Snk(cfg),
			oconNmSrcPreChain(oconNmSrcPreChainArg), 
			connectedNodes_prechain(connectedNodesArg), 
			iconNmEnterPreChain(iconNmTriggerArg), tpEnter(cpEnterArg),
			oconIdTriggerPreChain(oconIdTriggerPreChainArg), 
			iconIdTriggerPreChain(iconIdTriggerPreChainArg)
		{};


	};

	class CayfAutomationModulesSrc2SnkPreChain : public CayfAutomationModulesSrc2Snk
	{

	protected:
		std::string oconNmSrcPreChain = "default";
		std::string iconNmEnterPreChain = "default";
		jvxSize oconIdTriggerPreChain = JVX_SIZE_UNSELECTED; 
		jvxSize iconIdTriggerPreChain = JVX_SIZE_UNSELECTED;

		jvxComponentIdentification tpEnter = JVX_COMPONENT_UNKNOWN;
		
		struct
		{
			std::list<ayfConnectConfigCpEntry> connectedNodes;
		} config_prechain;

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
			IayfEstablishedProcessesCommon* sglElmPtr,
			const std::string& oconNameSrc,
			const std::string& iconNameSink,
			jvxSize& bridgeId,
			jvxSize oconIdTrigger,
			jvxSize iconIdTrigger) override;

		void deriveArguments(ayfConnectDerivedSrc2Snk& derivedArgs, const jvxComponentIdentification& tp_activated, IayfEstablishedProcessesCommon* realizeChainArg) override;

		virtual IayfEstablishedProcessesCommon* allocate_chain_realization() override;
		virtual void deallocate_chain_realization(IayfEstablishedProcessesCommon* ptr) override;
		jvxErrorType pre_run_chain_prepare(IjvxObject* obj_dev, IayfEstablishedProcessesCommon* realizeChain) override;
		jvxErrorType post_run_chain_prepare(IayfEstablishedProcessesCommon* realizeChain) override;

		void print(std::ostream& out);
	};
}

#endif