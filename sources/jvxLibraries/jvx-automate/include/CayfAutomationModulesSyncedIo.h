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
	
	// =========================================================================================
	// 
	// Here, we extend the config entry with a (simple) runtime parameter: the location of the support component
	class ayfConnectConfigCpEntrySyncIoRuntime : public ayfConnectConfigCpEntrySyncIo
	{
	public:
		struct
		{
			jvxBool subModulesActive = false;
			jvxCBitField connectionsEstablishFlags = 0;
			jvxSize uidProcesses[2] =
			{
				JVX_SIZE_UNSELECTED,
				JVX_SIZE_UNSELECTED
			};
		} states;

		struct
		{
			jvxBool allowPostPonedConnect = true;
			jvxCBitField targetFlagsConnection = 0x1;
		} derivedConfig;

		jvxComponentIdentification cpId = JVX_COMPONENT_UNKNOWN;
		ayfConnectConfigCpEntrySyncIoRuntime(const ayfConnectConfigCpEntrySyncIo& cp = ayfConnectConfigCpEntrySyncIo()) :
			ayfConnectConfigCpEntrySyncIo(cp), cpId(JVX_COMPONENT_UNKNOWN) {};
	};
}

#endif
