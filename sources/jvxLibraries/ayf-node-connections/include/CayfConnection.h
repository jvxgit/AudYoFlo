#ifndef __CAYFSIMPLECONNECTION_H__
#define __CAYFSIMPLECONNECTION_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "HjvxMicroConnection.h"

namespace AyfConnection
{
	enum class ayfConnectionOperationMode
	{
		AYF_CONNECTION_EFFICIENT,
		AYF_CONNECTION_FLEXIBLE
	};

	class CayfOneNode
	{
	private:

	public:		
		jvxComponentIdentification cpTp = JVX_COMPONENT_UNKNOWN;
		IjvxObject* theObj = nullptr;
		IjvxNode* theNode = nullptr;
		jvxBool activatedInSystem = false;
		std::string modName = "not-set";
		jvxState statOnInit = JVX_STATE_NONE;	
		jvxSize uId = JVX_SIZE_UNSELECTED;
		CayfOneNode();
		CayfOneNode(IjvxObject* theObjArg):theObj(theObjArg)
		{ 
		};
		~CayfOneNode();		
	};

	// Need this operator to find element in lst
	bool operator == (const CayfOneNode& lhs, const CayfOneNode& rhs);

	// ==========================================================
	class CayfConnectionComponent
	{
	public:
		CayfConnectionComponent() {};
		CayfConnectionComponent(const std::string& modNameArg, jvxComponentType tpArg, jvxBool tryActivateArg = false, jvxSize uIdArg = JVX_SIZE_UNSELECTED) :
			modName(modNameArg), tp(tpArg), tryActivateComponent(tryActivateArg), uId(uIdArg) {};
		CayfConnectionComponent(const std::string& modNameArg, IjvxObject* theObjArg, jvxBool tryActivateArg = false, jvxSize uIdArg = JVX_SIZE_UNSELECTED) :
			modName(modNameArg), theObj(theObjArg), tryActivateComponent(tryActivateArg), uId(uIdArg) {};

		jvxSize uId = JVX_SIZE_UNSELECTED;
		std::string modName;
		jvxBool tryActivateComponent = false;
		jvxComponentType tp = JVX_COMPONENT_UNKNOWN;
		IjvxObject* theObj = nullptr;
	};

	class CayfConnectionConfig
	{
	public:
		CayfConnectionConfig() {};
		CayfConnectionConfig(IjvxHiddenInterface* hostRefArg,
			IjvxObject* ownerArg,
			std::list<CayfConnectionComponent> modSpecLinearArg,
			jvxLinkDataDescriptor* anchorInArg,
			jvxLinkDataDescriptor* anchorOutArg,
			ayfConnectionOperationMode connectionModeArg = ayfConnectionOperationMode::AYF_CONNECTION_FLEXIBLE,
			HjvxMicroConnection_hooks_simple* bwdRefSplArg = nullptr,
			HjvxMicroConnection_hooks_fwd* bwdRefFwdArg = nullptr,
			jvxBool activateNodeArg = true,
			jvxSize numBuffersArg = 1,
			const std::string nmProcessArg = "simple-connection-chain",
			const std::string& descrProcessArg = "Simple Connection",
			const std::string& descrorProcessArg = "simpleConnection"):
			hostRef(hostRefArg),
			owner(ownerArg),
			modSpecLinear(modSpecLinearArg),
			anchorIn(anchorInArg),
			anchorOut(anchorOutArg),
			connectionMode(connectionModeArg),
			bwdRefSpl(bwdRefSplArg),
			bwdRefFwd(bwdRefFwdArg),
			activateNode(activateNodeArg),
			numBuffers(numBuffersArg),
			nmProcess(nmProcessArg),
			descrProcess(descrProcessArg),
			descrorProcess(descrorProcessArg) {};

		IjvxHiddenInterface* hostRef = nullptr;
		IjvxObject* owner = nullptr;
		std::list<CayfConnectionComponent> modSpecLinear;
		HjvxMicroConnection_hooks_simple* bwdRefSpl = nullptr;
		HjvxMicroConnection_hooks_fwd* bwdRefFwd = nullptr;
		jvxBool activateNode = true;
		jvxLinkDataDescriptor* anchorIn = nullptr;
		jvxLinkDataDescriptor* anchorOut = nullptr;
		ayfConnectionOperationMode connectionMode = ayfConnectionOperationMode::AYF_CONNECTION_EFFICIENT;
		jvxSize numBuffers = 1;
		std::string nmProcess;
		std::string descrProcess;
		std::string descrorProcess;
	};

	class CayfConnection: 
		// public IjvxHiddenInterface, // We must be a hidden interface to become the owner of the nodes
		public CjvxObjectMin
	{
	private:
		CayfConnectionConfig cfg;
		std::list<CayfOneNode> registeredNodes;

		std::string descriptionConnection;
		std::string descriptorConnection;
		std::string nameProcess;
		jvxSize numAttemptsNewProcess = 100; // Limit the number of simpe connections with the same name
		HjvxMicroConnection* microConn = nullptr;
		IayfConnectionStateSwitchNode* ssCb = nullptr;
		jvxSize idProcDepends = JVX_SIZE_UNSELECTED;
		jvxBool fwdTestToDepend = false;

	public:
		CayfConnection();
		~CayfConnection();

		// =====================================================================
		jvxErrorType provide_constraints(IjvxObject* theOwner, 
			const std::string& nameProcess = "simple-connection-chain",
			const std::string& descriptionConnection = "Simple Connection",
			const std::string& descriptorConnection = "simpleConnection");
		jvxErrorType reset_constraints();
		// =====================================================================

		jvxErrorType startup(const CayfConnectionConfig& cfg,
			IayfConnectionStateSwitchNode* cbSs = nullptr,
			jvxSize idConnDependsOn = JVX_SIZE_UNSELECTED, 
			jvxBool fwdTestChain = false);
		jvxErrorType shutdown();

		jvxErrorType state(jvxState* stat);

		jvxErrorType test_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

		jvxErrorType negotiate_connection(jvxLinkDataTransferType tp, jvxHandle* data, 
			jvxLinkDataDescriptor_con_params& ldat JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

		jvxErrorType transfer_forward_connection(jvxLinkDataTransferType tp, jvxHandle* data 
			JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

		jvxErrorType prepare_connection();
		jvxErrorType postprocess_connection();
		jvxErrorType start_connection();
		jvxErrorType stop_connection();
		jvxErrorType prepare_process_connection(
			jvxLinkDataDescriptor** lkDataOnReturn,
			jvxSize pipeline_offset = 0,
			jvxSize* idx_stage = NULL,
			jvxSize tobeAccessedByStage = 0,
			callback_process_start_in_lock = NULL,
			jvxHandle* priv_ptr = NULL);
		jvxErrorType process_connection(jvxLinkDataDescriptor** lkDataOnReturn);
		jvxErrorType postprocess_process_connection(
			jvxSize idx_stage = JVX_SIZE_UNSELECTED,
			jvxBool operate_first_call = true,
			jvxSize tobeAccessedByStage = 0,
			callback_process_stop_in_lock = NULL,
			jvxHandle* priv_ptr = NULL);

		jvxErrorType process_connection(
			jvxLinkDataDescriptor* lkDataIn,
			jvxSize mt_mask, jvxSize idx_stage,
			jvxLinkDataDescriptor* lkDataOut);

		// Interface "IjvxHiddenInterface"
		/*
		virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType, jvxHandle**)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType, jvxHandle*)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION object_hidden_interface(IjvxObject** objRef) override;
		*/

		jvxSize numRegisteredObjects();
		IjvxObject* objectAtPosition(jvxSize idx);

		void test_run_complete(jvxErrorType lastResult);

	private:

		jvxErrorType register_node(const std::string& modName, IjvxObject* theObj, jvxBool activatedInSystem = false,
			IjvxConfigProcessor* confProc = nullptr, jvxConfigData* secConfig = nullptr, jvxSize uid = JVX_SIZE_UNSELECTED);
		jvxErrorType unregister_node(IjvxObject* theObj);

		jvxErrorType register_node(const std::string& modName, jvxComponentType tp, jvxBool activateNode = true, jvxSize uid = JVX_SIZE_UNSELECTED);
		jvxErrorType unregister_nodes(const std::string& modName);

		// Interface "IjvxCoreStateMachine"
		jvxErrorType initialize(IjvxHiddenInterface* hostRef);
		jvxErrorType select(IjvxObject* owner) ;
		jvxErrorType activate() ;
		jvxErrorType deactivate() ;
		jvxErrorType unselect() ;
		jvxErrorType terminate() ;

		jvxErrorType create_micro_connection();
		jvxErrorType destroy_micro_connection();

	};

}

#endif