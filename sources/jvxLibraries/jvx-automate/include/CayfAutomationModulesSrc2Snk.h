#ifndef __CAYFAUTOMATIONMODULESSEC2SNK_H__
#define __CAYFAUTOMATIONMODULESSEC2SNK_H__

#include "jvx.h"
#include "CayfAutomationModules.h"
#include "CayfAutomationModuleHandler.h"

class CjvxObjectLog;

namespace CayfAutomationModules
{
	/*
	* This class allows to costruct a chain composed of the following parts:
	* SRC ->  <MOD1> -> <MOD2> -> .. -> <MODN> -> SNK
	* The chain is created once a SRC has become visible/was born
	* The SRC provides at the same time the master.
	* The module requires info about:
	* 1) Name of the master of the SRC (in the master factory)
	* 2) Name of the output connector of the SRC (in the connector factory)
	* 3) Name of the input connector in the SNK (in the connector factory)
	 */
	class ayfConnectConfigSrc2Snk
	{
	public:
		std::list<ayfConnectConfigCpEntry> connectedNodes;
		std::string nmMaster;
		std::string oconNmSource;
		std::string iconNmSink;
		std::string chainNamePrefix = "myChainCfgName";
		jvxSize connectionCategory = JVX_SIZE_UNSELECTED;
		// jvxComponentIdentification tpSrc = JVX_COMPONENT_UNKNOWN;
		jvxComponentIdentification tpInvolved = JVX_COMPONENT_UNKNOWN;
		jvxBool dbgOut = false;
		jvxSize oconIdTrigger = JVX_SIZE_UNSELECTED;
		jvxSize iconIdTrigger = JVX_SIZE_UNSELECTED;
		
		ayfConnectConfigSrc2Snk() {};
		ayfConnectConfigSrc2Snk(const std::string& chainName,
			const std::list<ayfConnectConfigCpEntry>& connectedNodesArg,
			const std::string& nmMasterArg = "default",
			const std::string& oconNmSourceArg = "default",
			const std::string& iconNmSinkArg = "",
			jvxSize connectionCategoryArg = JVX_SIZE_UNSELECTED,
			// jvxComponentIdentification tpSrcArg = JVX_COMPONENT_UNKNOWN,
			jvxComponentIdentification tpInvolvedArg = JVX_COMPONENT_UNKNOWN,
			jvxBool dbgOutArg = false,
			jvxSize oconTriggerIdArg = JVX_SIZE_UNSELECTED,
			jvxSize iconTriggerIdArg = JVX_SIZE_UNSELECTED) :
			chainNamePrefix(chainName), connectedNodes(connectedNodesArg),
			nmMaster(nmMasterArg), oconNmSource(oconNmSourceArg), iconNmSink(iconNmSinkArg),
			connectionCategory(connectionCategoryArg), tpInvolved(tpInvolvedArg), dbgOut(dbgOutArg),
			oconIdTrigger(oconTriggerIdArg), iconIdTrigger(iconTriggerIdArg)
		{};
	};

	class ayfConnectConfigCpEntryRuntime : public ayfConnectConfigCpEntry
	{
	public:
		jvxComponentIdentification cpId = JVX_COMPONENT_UNKNOWN;
		ayfConnectConfigCpEntryRuntime(const ayfConnectConfigCpEntry& cp) :
			ayfConnectConfigCpEntry(cp), cpId(JVX_COMPONENT_UNKNOWN) {};
	};

	class ayfOneModuleChainDefinition
	{
	public:
		ayfOneModuleChainDefinition(const jvxComponentIdentification& cpIdArg): 
			cpId(cpIdArg)
		{

		}
		jvxComponentIdentification cpId;
		bool operator < (const ayfOneModuleChainDefinition& elm) const
		{
			if (cpId < elm.cpId)
			{
				return true;
			}			
			return false;
		}
	};

	class ayfOneConnectedProcess
	{
	public:
		jvxSize processUid = JVX_SIZE_UNSELECTED;
		std::string chainName;
	};

	class ayfEstablishedProcesses
	{
	public:
		std::list<ayfConnectConfigCpEntryRuntime> lstEntries;
		std::list<ayfOneConnectedProcess> connectedProcesses;
	};
	
	class CayfAutomationModulesSrc2Snk: public CayfAutomationModuleHandlerIf
	{
	
	public:

		class ayfConnectDerivedSrc2Snk
		{
		public:
			jvxComponentIdentification tpSrc = JVX_COMPONENT_UNKNOWN;
			jvxComponentIdentification tpSink = JVX_COMPONENT_UNKNOWN;
			jvxComponentIdentification tpMaster = JVX_COMPONENT_UNKNOWN;
		};

	protected:

		// For each master, store the connected modules
		std::map<ayfOneModuleChainDefinition, ayfEstablishedProcesses> module_connections;
		IjvxReport* reportRefPtr = nullptr;
		IjvxHost* refHostRefPtr = nullptr;
		CjvxObjectLog* objLogRefPtr = nullptr;
		ayfAutoConnect_callbacks* cbPtr = nullptr;
		ayfConnectConfigSrc2Snk config;
		jvxSize purposeId = JVX_SIZE_UNSELECTED;
		ayfConnectDerivedSrc2Snk derived;
		jvxBool lockOperation = false;
	public:

		CayfAutomationModulesSrc2Snk()
		{
		};

		jvxErrorType activate(IjvxReport* report,
			IjvxHost* host,
			ayfAutoConnect_callbacks* cb,
			jvxSize purpId,
			const ayfConnectConfigSrc2Snk& cfg,
			CjvxObjectLog* ptrLog = nullptr);
		jvxErrorType deactivate();

		virtual void try_connect(
			jvxComponentIdentification tp_ident,
			jvxBool& established);

		jvxErrorType associate_process(jvxSize uIdProcess,
			const std::string& nmChain) override;
		jvxErrorType deassociate_process(jvxSize uIdProcess) override;
		jvxErrorType activate_all_submodules(const jvxComponentIdentification& tp_activated) override;
		jvxErrorType deactivate_all_submodules(const jvxComponentIdentification& tp_deactivated) override;
		jvxErrorType adapt_all_submodules(jvxSize uIdProc, const std::string& modName,
			const jvxComponentIdentification& tp_activated) override;

		virtual void create_bridges(
			IjvxDataConnectionRule* theDataConnectionDefRuleHdl, 
			jvxComponentIdentification tp_src,
			jvxComponentIdentification tp_sink,
			std::list<ayfConnectConfigCpEntryRuntime>& elm,
			const std::string& oconNameSrc,
			const std::string& iconNameSink,			
			jvxSize& bridgeId,
			jvxSize segId,
			jvxSize oconIdTrigger,
			jvxSize iconIdTrigger);

		// Functions to be called in the connection rule creation to adapt to use case

		// Depending on the use-case, derive the sinks and sources
		virtual void deriveArguments(ayfConnectDerivedSrc2Snk& derivedArgs, const jvxComponentIdentification& tp_activated);
	};
}

#endif
