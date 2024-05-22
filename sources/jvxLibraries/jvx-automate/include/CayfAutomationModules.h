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

	class ayfConnectConfigCpManipulate
	{
	public:
		// This is how the support node is remapped
		jvxComponentType tpRemap = JVX_COMPONENT_UNKNOWN;

		// This flag activates to attach uid to component description via IjvxManipulate - to distinguish between components of the same type
		jvxBool attachUi = false;

		// A way to parameterize the component name
		std::string manSuffix;

		ayfConnectConfigCpManipulate(jvxComponentType tpRemapArg = JVX_COMPONENT_UNKNOWN,
			jvxBool attachUiArg = false, const std::string& manSuffixArg = "") : tpRemap(tpRemapArg), attachUi(attachUiArg), manSuffix(manSuffixArg) {};
	};

	class ayfConnectConfigConMiscArgs
	{
	public:

		// Connection specific arguments
		jvxSize connectionCategory = JVX_SIZE_UNSELECTED;

		// Debug output
		jvxBool dbgOut = false;

		ayfConnectConfigConMiscArgs(
			jvxSize connectionCategoryArg = JVX_SIZE_UNSELECTED,
			jvxBool dbgOutArg = false) :
			connectionCategory(connectionCategoryArg),
			dbgOut(dbgOutArg) {};

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
			const ayfConnectConfigCpManipulate& cpManArg = ayfConnectConfigCpManipulate(),
			jvxSize idOconRefTriggerConnectorArg = JVX_SIZE_UNSELECTED,
			jvxSize idIconRefTriggerConnectorArg = JVX_SIZE_UNSELECTED) :
			cpTp(cpTpArg), modName(modNameArg), oconNm(oconNmArg),
			iconNm(iconNmArg), cpManipulate(cpManArg),
			assSegmentId(assSegmentIdArg),
			idOconRefTriggerConnector(idOconRefTriggerConnectorArg), idIconRefTriggerConnector(idIconRefTriggerConnectorArg)
		{
		};

		jvxComponentType cpTp = JVX_COMPONENT_UNKNOWN;
		std::string modName;
		std::string oconNm;
		std::string iconNm;
		ayfConnectConfigCpManipulate cpManipulate;
		jvxSize assSegmentId = 0;
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
