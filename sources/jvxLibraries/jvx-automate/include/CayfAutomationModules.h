#ifndef __CAYFAUTOMATIONMODULES_H__
#define __CAYFAUTOMATIONMODULES_H__

#include "jvx.h"
class CjvxObjectLog;

namespace CayfAutomationModules
{	
	JVX_INTERFACE ayfAutoConnect_callbacks
	{
	public:
		virtual ~ayfAutoConnect_callbacks() {};
		virtual jvxErrorType adapt_single_property_on_event(
			jvxSize purposeId,
			const std::string& nmChain,
			const std::string& modName,
			const std::string& description,
			jvxReportCommandRequest req,
			IjvxProperties* props) = 0;
	};

	class CayfAutomationModulesCommon
	{
	protected:
		IjvxReport* reportRefPtr = nullptr;
		IjvxHost* refHostRefPtr = nullptr;
		CjvxObjectLog* objLogRefPtr = nullptr;
		jvxSize purposeId = JVX_SIZE_UNSELECTED;
		jvxBool lockOperation = false;
	public:
		CayfAutomationModulesCommon() ;

		jvxErrorType activate(IjvxReport* report,
			IjvxHost* host,
			jvxSize purpId,
			CjvxObjectLog* ptrLog);

		jvxErrorType deactivate();
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
			jvxBool addUniqueIdArg = false,
			jvxSize idOconRefTriggerConnectorArg = JVX_SIZE_UNSELECTED,
			jvxSize idIconRefTriggerConnectorArg = JVX_SIZE_UNSELECTED) :
			cpTp(cpTpArg), modName(modNameArg), oconNm(oconNmArg),
			iconNm(iconNmArg), manSuffix(manSuffixArg), addUniqueId(addUniqueIdArg),
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
		jvxBool addUniqueId = true;
		jvxSize idOconRefTriggerConnector = JVX_SIZE_UNSELECTED;
		jvxSize idIconRefTriggerConnector = JVX_SIZE_UNSELECTED;
	};	

	class ayfOneModuleChainDefinition
	{
	public:
		ayfOneModuleChainDefinition(const jvxComponentIdentification& cpIdArg) :
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
};
#endif
