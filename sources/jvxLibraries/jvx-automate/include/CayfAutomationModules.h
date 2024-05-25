#ifndef __CAYFAUTOMATIONMODULES_H__
#define __CAYFAUTOMATIONMODULES_H__

#include "jvx.h"
class CjvxObjectLog;

#include "CayfAutomationModulesTypes.h"
#include "CayfAutomationModuleHandler.h"

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
		
		virtual jvxErrorType allow_master_connect(
			jvxSize purposeId, jvxComponentIdentification tpIdTrigger) = 0;
	};	

	class ayfOneConnectedProcess
	{
	public:
		jvxSize processUid = JVX_SIZE_UNSELECTED;
		std::string chainName;
	};

	JVX_INTERFACE IayfEstablishedProcessesCommon
	{
	public:
		virtual ~IayfEstablishedProcessesCommon() {};
		virtual std::list<ayfOneConnectedProcess>& connectedProcesses() = 0;
		virtual jvxHandle* specificType(ayfEstablishedProcessType tp) = 0;		
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

	class CayfAutomationModulesCommon
	{
	public:
		IjvxReport* reportRefPtr = nullptr;
		IjvxHost* refHostRefPtr = nullptr;
		CjvxObjectLog* objLogRefPtr = nullptr;
		jvxSize purposeId = JVX_SIZE_UNSELECTED;
		jvxBool lockOperation = false;

		std::map<ayfOneModuleChainDefinition, IayfEstablishedProcessesCommon*> module_connections;

	public:
		CayfAutomationModulesCommon() ;

		jvxErrorType activate(IjvxReport* report,
			IjvxHost* host,
			jvxSize purpId,
			CjvxObjectLog* ptrLog);

		jvxErrorType deactivate();

		virtual IayfEstablishedProcessesCommon* allocate_chain_realization() = 0;
		virtual void deallocate_chain_realization(IayfEstablishedProcessesCommon* deallocMe) = 0;
		virtual jvxErrorType pre_run_chain_prepare(IjvxObject* obj_dev, IayfEstablishedProcessesCommon* realizeChain) = 0;
		virtual void pre_run_chain_connect(jvxComponentIdentification tp_reg, 
			IjvxDataConnections* con, IayfEstablishedProcessesCommon* realizeChain) = 0;

		virtual jvxErrorType post_run_chain_prepare(IayfEstablishedProcessesCommon* realizeChain) = 0;
	};

	class ayfConnectConfigCpEntry
	{
	public:

		ayfConnectConfigCpEntry(
			jvxComponentType cpTpArg,
			const std::string& modNameArg = "",
			const std::string& oconNmArg = "",
			const std::string& iconNmArg = "",
			const ayfConnectConfigCpManipulate& cpManArg = ayfConnectConfigCpManipulate(),
			jvxSize idOconRefTriggerConnectorArg = JVX_SIZE_UNSELECTED,
			jvxSize idIconRefTriggerConnectorArg = JVX_SIZE_UNSELECTED) :
			cpTp(cpTpArg), modName(modNameArg), oconNm(oconNmArg),
			iconNm(iconNmArg), cpManipulate(cpManArg),
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
	
	class CayfEstablishedProcessesMixin
	{
	private:
		std::list<ayfOneConnectedProcess> connectedProcessesInstance;
	public:
		virtual std::list<ayfOneConnectedProcess>& _connectedProcesses() 
		{
			return connectedProcessesInstance;
		}
	};

	class ayfConnectMultiConnectionsRuntime
	{
	public:
		// Allow that a secondary connection can be connected lateron
		jvxBool allowPostPonedConnect = true;

		// This flag constellation defines what to achieve when connecting.
		// Some connections require multiple connections (-> 0x3)
		jvxCBitField targetFlagsConnection = 0x1;
	};	
};
#endif
