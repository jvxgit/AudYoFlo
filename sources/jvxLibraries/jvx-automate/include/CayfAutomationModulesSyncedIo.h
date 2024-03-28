#ifndef __CAYFAUTOMATIONMODULESSYNCEDIO_H__
#define __CAYFAUTOMATIONMODULESSYNCEDIO_H__

#include "jvx.h"
#include "CayfAutomationModules.h"
#include "CayfAutomationModuleHandler.h"

class CjvxObjectLog;

namespace CayfAutomationModules
{
	JVX_INTERFACE ayfAutoConnectSyncedIo_callbacks: public ayfAutoConnect_callbacks
	{
	public:
		virtual ~ayfAutoConnectSyncedIo_callbacks() {};
		virtual jvxErrorType allow_master_connect(
			jvxSize purposeId,
			jvxComponentIdentification tpIdTrigger) = 0;
	};

	// This component describes the connection of the support node which is used in the secondary chain
	class ayfConnectConfigCpEntrySyncIoCommon
	{
	public:
		
		// Name of the chain followed by slotid/subslotid tags
		std::string chainNamePrefix;

		// Master name
		std::string masterNm;
		
		// output connector name
		std::string oconMasterNm;
		
		// Input connector name
		std::string iconMasterNm;

		// Sink connector name of the prechain - component will be introduced by surrounding stuff
		std::string iconNmConnect;
		
		// Source connector name of the postchain - component will be introduced by surrounding stuff
		std::string oconNmConnect;

		jvxSize connectionCategory = JVX_SIZE_UNSELECTED;
		jvxBool dbgOut = false;

		ayfConnectConfigCpEntrySyncIoCommon(
			const std::string& chainNamePrefixArg,
			const std::string& masterNmArg,
			const std::string& oconMasterNmArg,
			const std::string& iconMasterNmArg,
			const std::string& iconNmConnectArg,
			const std::string& oconNmConnectArg,
			jvxSize connectionCategoryArg = JVX_SIZE_UNSELECTED,
			jvxBool dbgOutArg = false) :
			masterNm(masterNmArg),chainNamePrefix(chainNamePrefixArg), 
			oconMasterNm(oconMasterNmArg), iconMasterNm(iconMasterNmArg),
			iconNmConnect(iconNmConnectArg), oconNmConnect(oconNmConnectArg),
			connectionCategory(connectionCategoryArg), dbgOut(dbgOutArg) {};
	};

	// This describes the primary chain in which the support node is driven by the device of trigger
	class ayfConnectConfigCpEntrySyncIo: public ayfConnectConfigCpEntrySyncIoCommon
	{
	public:

		// This is how the support node is found: the type
		jvxComponentType cpTp = JVX_COMPONENT_UNKNOWN;

		// This is how the support node is found: the module name
		std::string modName;

		// A way to parameterize the component name
		std::string manSuffix;

	public:

		// 
		ayfConnectConfigCpEntrySyncIo(

			// Identify the required support node
			jvxComponentType cpTpArg = JVX_COMPONENT_UNKNOWN, const std::string& modNameArg = "notFound", const std::string& manSuffixArg = "",

			const std::string& chainNamePrefixArg = "default", const std::string& masterNmArg = "default",
			const std::string& oconMasterNmArg = "default", const std::string& iconMasterNmArg = "default",
			const std::string& iconNmConnectArg = "default", const std::string& oconNmConnectArg = "default",

			jvxSize connectionCategoryArg = JVX_SIZE_UNSELECTED, jvxBool dbgOutArg = false) : ayfConnectConfigCpEntrySyncIoCommon(
				chainNamePrefixArg, masterNmArg, 
				oconMasterNmArg, iconMasterNmArg,
				iconNmConnectArg, oconNmConnectArg,
				connectionCategoryArg, dbgOutArg),
			cpTp(cpTpArg), modName(modNameArg)
			{};
	};

	// A)
	// [NEW DEVICE] -> [SUPPORT COMPONENT] -> [NEW DEVICE]
	// B) 
	// [SUPPORT COMPONENT] -> [target node (1)] -> [SUPPORT COMPONENT]
	// 
	// This is the actual configuration of the full chain connection definition
	// This defines the device to support node connection within subclass ayfConnectConfigCpEntrySyncIoCommon
	class ayfConnectConfigSyncIo
	{
	public:

		// All related to the support node
		ayfConnectConfigCpEntrySyncIo driveSupportNodeChain; // A
		
		ayfConnectConfigCpEntrySyncIoCommon driveTargetCompChain; // B

		// This is what we want to connect the input (tp trigger) to. The support node helps to synchronize and
		// adapt the processing formats (B.1)
		jvxComponentIdentification connectTo = JVX_COMPONENT_UNKNOWN;
		jvxComponentIdentification connectFrom = JVX_COMPONENT_UNKNOWN;

		ayfConnectConfigSyncIo(

			// This is all for chain B
			const std::string& chainNamePrefixArg = "",
			const std::string& masterNmArg = "default",
			const std::string& oconMasterArg = "default",
			const std::string& iconMasterArg = "default",
			jvxComponentIdentification connectToArg = JVX_COMPONENT_UNKNOWN,
			jvxComponentIdentification connectFromArg = JVX_COMPONENT_UNKNOWN,
			const std::string& iconConnectArg = "default",
			const std::string& oconConnectArg = "default",

			// This is for chain A
			const ayfConnectConfigCpEntrySyncIo& supportNodeArg = ayfConnectConfigCpEntrySyncIo(),

			jvxSize connectionCategoryArg = JVX_SIZE_UNSELECTED,
			jvxBool dbgOutArg = false) :
			driveTargetCompChain(chainNamePrefixArg, masterNmArg,
				oconMasterArg, iconMasterArg,
				iconConnectArg, oconConnectArg,
				connectionCategoryArg, dbgOutArg), connectTo(connectToArg), connectFrom(connectFromArg),
			driveSupportNodeChain(supportNodeArg){};
	};

	// Here, we extend the config entry with a (simple) runtime parameter: the location of the support component
	class ayfConnectConfigCpEntrySyncIoRuntime : public ayfConnectConfigSyncIo
	{
	public:
		struct
		{
			jvxBool subModulesActive = false;
			jvxCBitField connectionsEstablishFlags = 0;
		} states;

		struct
		{
			jvxBool allowPostPonedConnect = true;
			jvxCBitField targetFlagsConnection = 0x1;
		} derivedConfig;

		jvxComponentIdentification cpId = JVX_COMPONENT_UNKNOWN;
		ayfConnectConfigCpEntrySyncIoRuntime(const ayfConnectConfigSyncIo& cp = ayfConnectConfigSyncIo()) :
			ayfConnectConfigSyncIo(cp), cpId(JVX_COMPONENT_UNKNOWN) {};
	};

	class ayfEstablishedProcessesSyncio
	{
	public:

		// The extended version of the config struct
		ayfConnectConfigCpEntrySyncIoRuntime supportNodeRuntime;

		// The connected processes - should be two in the end
		std::list<ayfOneConnectedProcess> connectedProcesses;

		ayfEstablishedProcessesSyncio(const ayfConnectConfigCpEntrySyncIoRuntime& supportNodeArg = ayfConnectConfigCpEntrySyncIoRuntime()) : supportNodeRuntime(supportNodeArg) {};
	};

	class CayfAutomationModulesSyncedIo: public CayfAutomationModulesCommon, public CayfAutomationModuleHandlerIf
	{
	
	public:
	protected:

		// Allocated and operated syncio structures
		std::map<ayfOneModuleChainDefinition, ayfEstablishedProcessesSyncio> module_connections;
		
		// Callback to interact
		ayfAutoConnectSyncedIo_callbacks* cbPtr = nullptr;

		// Configuration template
		ayfConnectConfigSyncIo config;		

		jvxBool allowPostPonedConnect = true;
		jvxCBitField targetFlagsConnection = 0x1;

		// ayfConnectDerivedSrc2Snk derived;
	public:

		CayfAutomationModulesSyncedIo()
		{
		};

		jvxErrorType activate(IjvxReport* report,
			IjvxHost* host,
			ayfAutoConnectSyncedIo_callbacks* cb,
			jvxSize purpId,
			const ayfConnectConfigSyncIo& cfgArg,
			jvxBool allowPostConnection = true,
			jvxCBitField targetConnect = 0x1,
			CjvxObjectLog* ptrLog = nullptr);
		jvxErrorType deactivate();

		jvxErrorType associate_process(jvxSize uIdProcess,
			const std::string& nmChain) override;
		jvxErrorType deassociate_process(jvxSize uIdProcess) override;
		jvxErrorType activate_all_submodules(const jvxComponentIdentification& tp_activated) override;
		jvxErrorType deactivate_all_submodules(const jvxComponentIdentification& tp_deactivated) override;
		jvxErrorType adapt_all_submodules(jvxSize uIdProc, const std::string& modName, const std::string& description,
			const jvxComponentIdentification& tp_activated, jvxReportCommandRequest req) override;

		virtual void postponed_try_connect() override;
		void try_connect(jvxComponentIdentification tp_reg, jvxBool& established);
		/*
	

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
		*/
	};
}

#endif
