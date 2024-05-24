#ifndef __CAYFAUTOMATIONMODULESSYNCEDIO_H__
#define __CAYFAUTOMATIONMODULESSYNCEDIO_H__

#include "jvx.h"
#include "CayfAutomationModules.h"
#include "CayfAutomationModuleHandler.h"
#include "CayfAutomationModulesSyncedIoMixIn.h"

class CjvxObjectLog;

namespace CayfAutomationModules
{	
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

		ayfConnectConfigConMiscArgs misc;
		// jvxSize connectionCategory = JVX_SIZE_UNSELECTED;		
		// jvxBool dbgOut = false;

		ayfConnectConfigCpEntrySyncIoCommon(
			const std::string& chainNamePrefixArg,
			const std::string& masterNmArg,
			const std::string& oconMasterNmArg,
			const std::string& iconMasterNmArg,
			const std::string& iconNmConnectArg,
			const std::string& oconNmConnectArg,
			const ayfConnectConfigConMiscArgs& miscArgs = ayfConnectConfigConMiscArgs()
			// jvxSize connectionCategoryArg = JVX_SIZE_UNSELECTED,
			// jvxBool dbgOutArg = false
		) :
			masterNm(masterNmArg),chainNamePrefix(chainNamePrefixArg), 
			oconMasterNm(oconMasterNmArg), iconMasterNm(iconMasterNmArg),
			iconNmConnect(iconNmConnectArg), oconNmConnect(oconNmConnectArg),
			misc(miscArgs)
			// connectionCategory(connectionCategoryArg), dbgOut(dbgOutArg) 
		{};
	};

	// This describes the primary chain in which the support node is driven by the device of trigger
	class ayfConnectConfigCpEntrySyncIo: public ayfConnectConfigCpEntrySyncIoCommon
	{
	public:

		// This is how the support node is found: the type
		jvxComponentType cpTp = JVX_COMPONENT_UNKNOWN;

		// This is how the support node is found: the module name
		std::string modName;		

		ayfConnectConfigCpManipulate cpManipulate;

		ayfConnectConfigConMiscArgs misc;
		
	public:

		// 
		ayfConnectConfigCpEntrySyncIo(

			// Identify the required support node
			jvxComponentType cpTpArg = JVX_COMPONENT_UNKNOWN, const std::string& modNameArg = "notFound", 

			const std::string& chainNamePrefixArg = "default", const std::string& masterNmArg = "default",
			const std::string& oconMasterNmArg = "default", const std::string& iconMasterNmArg = "default",
			const std::string& iconNmConnectArg = "default", const std::string& oconNmConnectArg = "default",

			const ayfConnectConfigCpManipulate& cpManipulateArg = ayfConnectConfigCpManipulate(),
			const ayfConnectConfigConMiscArgs& miscArgs = ayfConnectConfigConMiscArgs()) : ayfConnectConfigCpEntrySyncIoCommon(
				chainNamePrefixArg, masterNmArg, 
				oconMasterNmArg, iconMasterNmArg,
				iconNmConnectArg, oconNmConnectArg,
				miscArgs),
			cpTp(cpTpArg), modName(modNameArg), cpManipulate(cpManipulateArg), misc(miscArgs){};
	};

	class ayfConnectConfigCpConInChain
	{
	public:
		jvxComponentIdentification connectCp = JVX_COMPONENT_UNKNOWN;
		std::string connectCon = "default";
		jvxSize connectLinkId = JVX_SIZE_UNSELECTED;

		ayfConnectConfigCpConInChain(
			jvxComponentIdentification connectCpArg = JVX_COMPONENT_UNKNOWN,
			const std::string& connectConArg = "default",
			jvxSize connectLinkIdArg = JVX_SIZE_UNSELECTED) :
			connectCp(connectCpArg), connectCon(connectConArg), connectLinkId(connectLinkIdArg) {};
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
		ayfConnectConfigCpConInChain cpTo;
		ayfConnectConfigCpConInChain cpFrom;

		ayfConnectConfigSyncIo(

			// This is all for chain B
			const std::string& chainNamePrefixArg = "",
			const std::string& masterNmArg = "default",
			const std::string& oconMasterArg = "default",
			const std::string& iconMasterArg = "default",
			const ayfConnectConfigCpConInChain& cpToArg = ayfConnectConfigCpConInChain(),
			const ayfConnectConfigCpConInChain& cpFromArg = ayfConnectConfigCpConInChain(),
			const ayfConnectConfigConMiscArgs& miscArgs = ayfConnectConfigConMiscArgs(),
			// jvxSize connectionCategoryArg = JVX_SIZE_UNSELECTED,
			//jvxBool dbgOutArg = false,

			// This is for chain A
			const ayfConnectConfigCpEntrySyncIo& supportNodeArg = ayfConnectConfigCpEntrySyncIo()) :
			driveTargetCompChain(chainNamePrefixArg, masterNmArg,
				oconMasterArg, iconMasterArg,
				cpToArg.connectCon/*iconConnectArg*/, 
				cpFromArg.connectCon,
				// connectionCategoryArg, dbgOutArg
				miscArgs
				), // connectTo(connectToArg), 
			cpTo(cpToArg),
			cpFrom(cpFromArg),
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

	class ayfEstablishedProcessesSrc2Snk;

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
		virtual IayfEstablishedProcessesSrc2Snk* src2SnkRef() override
		{
			return nullptr;
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

	
	class CayfAutomationModulesSyncedIo: public CayfAutomationModulesSyncedIoPrimaryMixIn, 
		public CayfAutomationModulesCommon,
		public ayfConnectMultiConnectionsRuntime, public CayfAutomationModuleHandlerIf
	{
	
	public:
	protected:
		
		// Callback to interact
		ayfAutoConnect_callbacks* cbPtr = nullptr;

		// Configuration template
		ayfConnectConfigSyncIo config;		

		// ayfConnectDerivedSrc2Snk derived;
	public:

		CayfAutomationModulesSyncedIo()
		{
		};

		jvxErrorType activate(IjvxReport* report,
			IjvxHost* host,
			ayfAutoConnect_callbacks* cb,
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

		virtual IayfEstablishedProcessesCommon* allocate_chain_realization() override;
		virtual void deallocate_chain_realization(IayfEstablishedProcessesCommon* deallocMe) override;
		
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
