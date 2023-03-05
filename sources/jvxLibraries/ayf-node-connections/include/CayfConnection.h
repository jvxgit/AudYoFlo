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
		IjvxSimpleNode* theNode = nullptr;
		jvxBool activatedInSystem = false;
		std::string modName = "not-set";
		std::string inputConnectorName = "default";
		std::string outputConnectorName = "default";
		jvxState statOnInit = JVX_STATE_NONE;
		jvxSize uId = JVX_SIZE_UNSELECTED;
		CayfOneNode();
		CayfOneNode(IjvxObject* theObjArg, 
			const std::string& inConnName = "default", 
			const std::string& outConnName = "default") :theObj(theObjArg),
			inputConnectorName(outConnName),
			outputConnectorName(outConnName)
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
		CayfConnectionComponent(const std::string& modNameArg, jvxComponentType tpArg, 
			jvxBool tryActivateArg = false, 
			jvxSize uIdArg = JVX_SIZE_UNSELECTED,
			IjvxReferenceSelector* deciderArg = nullptr,
			const std::string& outputConnectorNameArg = "default") :
			modName(modNameArg), tp(tpArg), tryActivateComponent(tryActivateArg), 
			uId(uIdArg), decider(deciderArg),
			outputConnectorName(outputConnectorNameArg) {};
		CayfConnectionComponent(const std::string& modNameArg, IjvxObject* theObjArg, jvxBool tryActivateArg = false, jvxSize uIdArg = JVX_SIZE_UNSELECTED,
			IjvxReferenceSelector* deciderArg = nullptr,
			const std::string& outputConnectorNameArg = "default") :
			modName(modNameArg), theObj(theObjArg), tryActivateComponent(tryActivateArg), uId(uIdArg),
			decider(deciderArg),
			outputConnectorName (outputConnectorNameArg) {};

		jvxSize uId = JVX_SIZE_UNSELECTED;
		std::string modName;
		jvxBool tryActivateComponent = false;
		jvxComponentType tp = JVX_COMPONENT_UNKNOWN;
		IjvxObject* theObj = nullptr;
		IjvxReferenceSelector* decider = nullptr;
		std::string outputConnectorName = "default";
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
			const std::string nmProcessArg = "simple-connection-chain",
			const std::string& descrProcessArg = "Simple Connection",
			const std::string& descrorProcessArg = "simpleConnection",
			HjvxMicroConnection_hooks_simple* bwdRefSplArg = nullptr,
			HjvxMicroConnection_hooks_fwd* bwdRefFwdArg = nullptr,
			jvxBool activateNodeArg = true,
			jvxSize numBuffersArg = 1):
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

	public:

		// ===========================================================================================
		// The simple interface
		// ===========================================================================================

		/*
		 * Main startup function : This call establishes and tests the micro connection.
		 * The argument cbSs allows to track the state changes of the component, 
		 * the argument idConnDependsOn allows to specify a uId to identify a component
		 * if the same type is used more than once.
		 * The argument fwdTestChain allows to forward requests to test a chain to the 
		 * outside chain. This typically is desired on EFFICIENT connections.
		 */
		jvxErrorType init_connect(const CayfConnectionConfig& cfg,
			IayfConnectionStateSwitchNode* cbSs = nullptr,
			jvxSize idConnDependsOn = JVX_SIZE_UNSELECTED, 
			jvxBool fwdTestChain = false);

		//! Function to prepare and start the involved subchain. This calls prepare_connection and start_connection.
		jvxErrorType prepare_start();

		//! Function to stop and postprocess the involved subchain. This calls stop_connection and postprocess_connection.
		jvxErrorType stop_postprocess();

		/*
		 * Main shutdown function : This call shuts down the micro connection.
		 */
		jvxErrorType disconnect_terminate();

		// ===========================================================================================

		jvxErrorType state(jvxState* stat);

		jvxErrorType test_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

		jvxErrorType negotiate_connection(jvxLinkDataTransferType tp, jvxHandle* data, 
			jvxLinkDataDescriptor_con_params& ldat JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

		jvxErrorType transfer_forward_connection(jvxLinkDataTransferType tp, jvxHandle* data 
			JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

		/*
		 * This call triggers the reservation of the buffers right before processing. Note that always,
		 * the option NOT to reuse buffers in place is active.
		 */
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

	private:

		jvxErrorType provide_constraints(
			const std::string& nameProcess = "simple-connection-chain",
			const std::string& descriptionConnection = "Simple Connection",
			const std::string& descriptorConnection = "simpleConnection");
		jvxErrorType reset_constraints();

		void test_run_complete(jvxErrorType lastResult);
		jvxErrorType register_node(const std::string& modName, IjvxObject* theObj, jvxBool activatedInSystem = false,
			IjvxConfigProcessor* confProc = nullptr, jvxConfigData* secConfig = nullptr, jvxSize uid = JVX_SIZE_UNSELECTED,
			const std::string& inConnName = "default", const std::string& outConnName = "default");
		jvxErrorType unregister_node(IjvxObject* theObj);

		jvxErrorType register_node(const std::string& modName, jvxComponentType tp, jvxBool activateNode = true, jvxSize uid = JVX_SIZE_UNSELECTED, 
			IjvxReferenceSelector* decider = nullptr, const std::string& inConnName = "default", const std::string& outConnName = "default");
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