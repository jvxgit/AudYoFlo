#ifndef __CAYFAUTOMATIONMODULES_H__
#define __CAYFAUTOMATIONMODULES_H__

#include "jvx.h"
class CjvxObjectLog;

namespace CayfAutomationModules
{
	JVX_INTERFACE ayfAutoConnect_callbacks
	{
		virtual ~ayfAutoConnect_callbacks() {};
		virtual jvxErrorType adapt_single_property_on_connect(
			jvxSize purposeId, 
			const std::string& nmChain,
			const std::string& modName, 
			IjvxProperties* props) = 0;
		virtual jvxErrorType allow_master_connect(
			jvxSize purposeId,
			jvxComponentIdentification tpIdTrigger,
			jvxComponentIdentification& cpIdSrc,
			jvxComponentIdentification& cpIdSink,
			std::string& oconConnectorName,
			std::string& iconConnectorName) = 0;

	};

	class ayfConnectConfigCpEntry
	{
	public:

		ayfConnectConfigCpEntry(
			jvxComponentType cpTpArg,
			const std::string& modNameArg = "",
			const std::string& oconNmArg = "",
			const std::string& iconNmArg = "",
			const std::string& manSuffixArg = "") :
			cpTp(cpTpArg), modName(modNameArg), oconNm(oconNmArg),
			iconNm(iconNmArg), manSuffix(manSuffixArg)
		{
		};

		jvxComponentType cpTp = JVX_COMPONENT_UNKNOWN;
		std::string modName;
		std::string oconNm;
		std::string iconNm;
		std::string manSuffix;
	};

	// 
	class ayfConnectConfig
	{
	public:
		std::list<ayfConnectConfigCpEntry> connectedNodes;
		std::string nmMaster;
		std::string oconNmSource;
		std::string iconNmSink;
		std::string chainNamePrefix = "myChainCfgName";
		jvxSize connectionCategory = JVX_SIZE_UNSELECTED;
		jvxComponentIdentification tpSrc = JVX_COMPONENT_UNKNOWN;
		jvxComponentIdentification tpSink = JVX_COMPONENT_UNKNOWN;
		jvxBool dbgOut = false;

		ayfConnectConfig() {};
		ayfConnectConfig(const std::string& chainName,
			const std::list<ayfConnectConfigCpEntry>& connectedNodesArg,
			const std::string& nmMasterArg = "default",
			const std::string& oconNmSourceArg = "default",
			const std::string& iconNmSinkArg = "",
			jvxSize connectionCategoryArg = JVX_SIZE_UNSELECTED, 
			jvxComponentIdentification tpSrcArg = JVX_COMPONENT_UNKNOWN,
			jvxComponentIdentification tpSinkArg = JVX_COMPONENT_UNKNOWN,
			jvxBool dbgOutArg = false) :
			chainNamePrefix(chainName), connectedNodes(connectedNodesArg),
			nmMaster(nmMasterArg), oconNmSource(oconNmSourceArg), iconNmSink(iconNmSinkArg),
			connectionCategory(connectionCategoryArg), tpSrc(tpSrcArg), tpSink(tpSinkArg), dbgOut(dbgOutArg)
		{};
	};

	class ayfConnectConfigCpEntryRuntime : public ayfConnectConfigCpEntry
	{
	public:
		jvxComponentIdentification cpId = JVX_COMPONENT_UNKNOWN;
		ayfConnectConfigCpEntryRuntime(const ayfConnectConfigCpEntry& cp) :
			ayfConnectConfigCpEntry(cp), cpId(JVX_COMPONENT_UNKNOWN){};
	};	
};
#endif
