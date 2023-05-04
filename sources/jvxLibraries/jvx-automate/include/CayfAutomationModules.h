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
			const std::string& description,
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
			jvxSize assSegmentIdArg = 0,
			const std::string& manSuffixArg = "",
			jvxSize idOconRefTriggerConnectorArg = JVX_SIZE_UNSELECTED,
			jvxSize idIconRefTriggerConnectorArg = JVX_SIZE_UNSELECTED) :
			cpTp(cpTpArg), modName(modNameArg), oconNm(oconNmArg),
			iconNm(iconNmArg), manSuffix(manSuffixArg), 
			assSegmentId(assSegmentIdArg),
			idOconRefTriggerConnector(idOconRefTriggerConnectorArg), idIconRefTriggerConnector(idIconRefTriggerConnectorArg)
		{
		};

		jvxComponentType cpTp = JVX_COMPONENT_UNKNOWN;
		std::string modName;
		std::string oconNm;
		std::string iconNm;
		std::string manSuffix;
		jvxSize assSegmentId = 0;
		jvxSize idOconRefTriggerConnector = JVX_SIZE_UNSELECTED;
		jvxSize idIconRefTriggerConnector = JVX_SIZE_UNSELECTED;
	};	
};
#endif
