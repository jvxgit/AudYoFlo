#ifndef __HJVXMICROCONNECTIONS_H__
#define __HJVXMICROCONNECTIONS_H__

#include "jvx.h"

#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"
#include "common/CjvxConnectionMaster.h"
#include "common/CjvxConnectorMasterFactory.h"
#include "common/CjvxObject.h"

/*
 * The micro connection can be used in three ways:
 * 3) In forward mode:
 *    In this mode, the component to hold the microconnection acts as a container and 
 *    forwards all data processing requests to the micro connection. The micro connection
 *    then processes along all elements in the microconnection chain and at the end
 *    of the chain, the next element to follow the component to hold the micro connection
 *    is addressed via the HjvxMicroConnection_hooks_fwd refernence directly. 
 *    This approach might even be used to exchange processing components while processing is
 *    online.
 *    This mode is activated by calling 
 *    res = theMicroConnection->connect_connection(_common_set_ldslave.theData_in, 
 *						&_common_set_ldslave.theData_out,
 *						static_cast<HjvxMicroConnection_hooks_simple*>(this), 
 *						static_cast<HjvxMicroConnection_hooks_fwd*>(this),
 *						false);
 * 
 * 
 * If we want to use another link data container as input and put the output to the
 * field related to another link data container, we need to pass references in
 * "connect_connection" and specify the option "useOtherContainers" as true
 * in the member function "prepare_connection". Then, the "prepare_process_connection" and 
 * "postprocess_process_connection" must not be called since the container have been reserved before.
 *
 * If we do not want to reuse other containers, we need to pass extra containers in "connect_connection"
 * and then call prepare with "useOtherContainers" as false. Then, we need to call the functions
 * "prepare_process_connection" and "postprocess_process_connection" in the process loop.
 */
enum class jvxMicroConnectionHookSpec
{
	JVX_MICROCONNECTION_HOOKS_SIMPLE = 1,
	JVX_MICROCONNECTION_HOOKS_FULL = 2,
};

JVX_INTERFACE HjvxMicroConnection_hooks_simple 
{
public:
	~HjvxMicroConnection_hooks_simple() {};

	/*
	 * This function is called if the first element in micro connection complains about the connection parameters
	 */
	virtual jvxErrorType hook_test_negotiate(jvxLinkDataDescriptor* proposed JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	/* This is the endpoint of the micro connection. In dataIn, the call gets the output constraints of the
	 * final element in the chain.
     */
	virtual jvxErrorType hook_test_accept(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	/*
	 * This call is triggered if the element following the chain complains about the settings
	 */
	virtual jvxErrorType hook_test_update(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	/*
	 * This call is triggered on the call to transfer_forward with a request to check status ready.
	 * This callback is called at the beginning of the microconnection chain before entering the sub chain
	 */
	virtual jvxErrorType hook_check_is_ready(jvxBool* is_ready, jvxApiString * astr) = 0;

	/*
	 * This is the callback where the transfer forward ends up after running through the chain! 
	 */
	virtual jvxErrorType hook_forward(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// virtual jvxErrorType hook_text_chain() = 0;
};

JVX_INTERFACE HjvxMicroConnection_hooks_fwd
{
public:
	virtual ~HjvxMicroConnection_hooks_fwd() {};

	/*
	 * This call is the endpoint of the prepare function called before starting to process.
	 * We should jump directly to the next connected component.
	 */
	virtual jvxErrorType hook_prepare(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	/*
	 * This call is the endpoint of the postprocess function called before starting to process.
	 * We should jump directly to the next connected component.
	 */
	virtual jvxErrorType hook_postprocess(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	/*
	 * This call is the endpoint of the start function called before starting to process.
	 * We should jump directly to the next connected component.
     */
	virtual jvxErrorType hook_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	/*
	 * This call is the endpoint of the stop function called before starting to process.
	 * We should jump directly to the next connected component.
	 */
	virtual jvxErrorType hook_stop(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	
	/*
	 * This call is the endpoint of the process_start function called before starting to process.
	 * We should jump directly to the next connected component.
	 */
	virtual jvxErrorType hook_process_start(
		jvxSize pipeline_offset,
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr) = 0;

	/*
	 * This call is the endpoint of the process_stop function called before starting to process.
	 * We should jump directly to the next connected component.
	 */
	virtual jvxErrorType hook_process_stop(
		jvxSize idx_stage,
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr) = 0;

	/*
	 * This call is the endpoint of the process function called before starting to process.
	 * We should jump directly to the next connected component.
	 */
	virtual jvxErrorType hook_process(
		jvxSize mt_mask, jvxSize idx_stage) = 0;
};

class HjvxMicroConnection :	
	public IjvxObject, protected CjvxObject,
	public IjvxConnectorFactory, protected CjvxConnectorFactory,
	public IjvxInputConnector, protected IjvxOutputConnector, protected CjvxInputOutputConnector,
	public IjvxConnectionMaster, protected CjvxConnectionMaster,
	public IjvxConnectionMasterFactory, protected CjvxConnectionMasterFactory
{
public:

	enum class jvxConnectionType
	{
		JVX_MICROCONNECTION_UNCONTROLLED = 0,

		//! This option is used whenever the microchain becomes part of an outside chain. The linkage is fixed such that it can not 
		//! be unlinked when in operation
		JVX_MICROCONNECTION_ENGAGE = 1,

		//! This option is used whenever a connection shall be flexible such that it can be linked and unlinked during outside operation. 
		//! Input and output parameters of the micro chain are fixed
		JVX_MICROCONNECTION_FLEXIBLE_INOUT_FIXED = 2,

		//! This option is used whenever a connection shall be flexible such that it can be linked and unlinked during outside operation. 
		//! Input and output parameters of the micro chain are not fixed. If the test fails, the parameter theData_proposeInput contains 
		//! information about the desired parameters
		JVX_MICROCONNECTION_FLEXIBLE_INOUT_ADAPT = 3
	};

private:

	struct oneInvolvedObject
	{
		IjvxHiddenInterface* theTarget = nullptr;
		IjvxInputConnector* iconr = nullptr;
		IjvxOutputConnector* oconr = nullptr;
		IjvxConnectorFactory* theConFac = nullptr;

		jvxSize uId_bridge_node_node = JVX_SIZE_UNSELECTED;
		oneInvolvedObject()
		{
			theTarget = nullptr;
			iconr = nullptr;
			oconr = nullptr;
			theConFac = nullptr;
			uId_bridge_node_node = JVX_SIZE_UNSELECTED;
		};
	};
	IjvxDataConnections* theConnections = nullptr;
	std::vector<oneInvolvedObject> involvedObjects;

	IjvxOutputConnector* mocon = nullptr;
	IjvxInputConnector* micon = nullptr;
	IjvxConnectionMaster* master = nullptr;

	HjvxMicroConnection_hooks_simple* refHandleSimple = nullptr;
	HjvxMicroConnection_hooks_fwd* refHandleFwd = nullptr;

	jvxSize uId_process = JVX_SIZE_UNSELECTED;
	jvxSize uId_bridge_dev_node = JVX_SIZE_UNSELECTED;
	jvxSize uId_bridge_node_dev = JVX_SIZE_UNSELECTED;
	jvxLinkDataDescriptor* theData_outlnk = nullptr;
	jvxLinkDataDescriptor* theData_inlnk = nullptr;
	jvxLinkDataDescriptor* theData_proposeInput = nullptr;

	/** 
	 * If this option is true, the input buffers will be used in place - that is, the input buffers will be used on the output side of the microconnection.
	 * This option can only be used if the first element in micro chain accpets buffers with the JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT flag set.
	 * If the option is not set, the field theData_inlnk will contain the buffers which are in use in the micro chain to allow access to the micro chain.
	 */
	jvxBool useBuffersInPlaceInput = false;

	/**
	 * If this option is true, the output buffers of the end of the micro chain will be those as passed in theData_outlnk - directly.
	 * In this case, running the micro chain will end up with filling in the data that can be directly forwarded to the next element 
	 * outside the micro chain. If the option is not set, dedicated buffers are allocated which are returned in the variable theData_outlnk.
	 */
	jvxBool useBuffersInPlaceOutput = false;

	std::string dbg_hint;
	std::string last_error = "";

	jvxConnectionType typeConnection = jvxConnectionType::JVX_MICROCONNECTION_ENGAGE;	
	jvxBool copyAttachedData = false;

public:
	HjvxMicroConnection(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~HjvxMicroConnection();

	/** Option 1: Activate a micro connection chain
	 * The href must be a hidden interface exposingat least the interface JVX_INTERFACE_DATA_CONNECTIONS
	 * The target references must be interfaces exposing at least JVX_INTERFACE_CONNECTOR_FACTORY
	 * If the interfaces are not exposed properly, the micro connection will notcomplete successfully.
	 *///=====================================================================
	jvxErrorType activate_connection(
		IjvxHiddenInterface* href,
		IjvxHiddenInterface** target,
		const char** iconn,
		const char** oconn,
		jvxSize num_objs,
		const std::string& mastern = "default",
		const std::string& mioconn = "default",
		const std::string& nmprocess = "process",
		jvxBool interceptors = false,
		IjvxObject* theOwner = NULL,
		jvxSize idProcDepends = JVX_SIZE_UNSELECTED,
		jvxBool fwdTestToDependent = false);
	
	/** Option 2: Activate a micro connection chain
	 * The href must be a hidden interface exposingat least the interface JVX_INTERFACE_DATA_CONNECTIONS
	 * The target references must be interfaces exposing at least JVX_INTERFACE_CONNECTOR_FACTORY
	 * If the interfaces are not exposed properly, the micro connection will notcomplete successfully.
	 *///=====================================================================
	jvxErrorType activate_connection(
		IjvxHiddenInterface* href,
		IjvxHiddenInterface* target,
		const std::string& iconn = "default",
		const std::string& oconn = "default",
		const std::string& mastern = "default",
		const std::string& mioconn = "default",
		const std::string& nmprocess = "default",
		jvxBool interceptors = false,
		IjvxObject* theOwner = NULL,
		jvxSize idProcDepends = JVX_SIZE_UNSELECTED
	); 
	
	/*
	jvxErrorType sync_processing_parameters(
		jvxLinkDataDescriptor_con_params* con_params);
		*/

	jvxErrorType deactivate_connection();

	/**
	 * Connect the micro connection. The input and the output link data connector containers are stored as a reference.
	 * A callback reference may be passed in ref argument to catch the most important calls. If no refernce is passed specific
	 * constraints apply such as identical input/output parameters.
	 * The function connects the micro connection by creating a process and connecting it.
	 * The test function may be called on connection by specifying test_on_connect to true. To call the test_function can
	 * be dangerous: the connect sets the runtime parameters to default values (0) which are then used in the test to be spread out 
	 * to all components. Therefore, the connection is initialized by setting the parameters from theData_in right after connect and before the 
	 * initial test!
	 */
	jvxErrorType connect_connection(jvxLinkDataDescriptor* theData_in, 
		jvxLinkDataDescriptor* theData_out, 
		jvxConnectionType connTypeArg = jvxConnectionType::JVX_MICROCONNECTION_FLEXIBLE_INOUT_FIXED,
		HjvxMicroConnection_hooks_simple* refSimple = NULL,
		HjvxMicroConnection_hooks_fwd* refFull = NULL,
		jvxLinkDataDescriptor* proposedParametersInput = nullptr);

	jvxErrorType disconnect_connection();
	jvxErrorType test_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType prepare_connection(jvxBool buffersInPlaceIn, jvxBool buffersInPlaceOut, jvxBool copyAttachedData = false);
	jvxErrorType postprocess_connection();
	jvxErrorType start_connection();
	jvxErrorType stop_connection();

	jvxErrorType transfer_forward_connection(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	jvxErrorType transfer_backward_connection(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb));

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

	// =====================================================================
	// All object default implementations
	// =====================================================================
#include "codeFragments/simplify/jvxObjects_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// Link to default input/output connector factory and input/output connector implementations
	// =====================================================================
#include "codeFragments/simplify/jvxConnectorFactory_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// =====================================================================
#define JVX_INPUT_OUTPUT_CONNECTOR_MASTER
#define JVX_INPUT_OUTPUT_SUPPRESS_PREPARE_POSTPROCESS_TO
#define JVX_INPUT_OUTPUT_SUPPRESS_START_STOP_TO
#define JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#define JVX_CONNECTION_MASTER_SKIP_TRANSFER_FORWARD_ICON
#define JVX_CONNECTION_MASTER_SKIP_TRANSFER_BACKWARD_OCON
#define JVX_SUPPRESS_AUTO_READY_CHECK_ICON
#define JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
#define JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_STARTSTOP_TO
#define JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO
#include "codeFragments/simplify/jvxInputOutputConnector_simplify.h"
#undef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
#undef JVX_INPUT_OUTPUT_SUPPRESS_PREPARE_POSTPROCESS_TO
#undef JVX_INPUT_OUTPUT_SUPPRESS_START_STOP_TO
#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#undef JVX_CONNECTION_MASTER_SKIP_TRANSFER_FORWARD_ICON
#undef JVX_CONNECTION_MASTER_SKIP_TRANSFER_BACKWARD_OCON
#undef JVX_SUPPRESS_AUTO_READY_CHECK_ICON
#undef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
#undef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_STARTSTOP_TO
#undef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_chain_master(const jvxChainConnectArguments& args,
		const jvxLinkDataDescriptor_con_params* init_params JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// =====================================================================
	// =====================================================================
	
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset,
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk = NULL,
		jvxHandle* priv_ptr = NULL) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(jvxSize idx_stage,
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	// =====================================================================
	// =====================================================================

	// =====================================================================
	// Link to default master factory and master implementations
	// =====================================================================
#include "codeFragments/simplify/jvxConnectorMasterFactory_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// =====================================================================
#define JVX_CONNECTION_MASTER_SKIP_TEST
#define JVX_SUPPRESS_AUTO_READY_CHECK_MASTER
#define JVX_CONNECTION_MASTER_SKIP_CONNECT
#include "codeFragments/simplify/jvxConnectionMaster_simplify.h"
#undef JVX_CONNECTION_MASTER_SKIP_TEST
#undef JVX_SUPPRESS_AUTO_READY_CHECK_MASTER
#undef JVX_CONNECTION_MASTER_SKIP_CONNECT

	jvxErrorType JVX_CALLINGCONVENTION test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType JVX_CALLINGCONVENTION transfer_chain_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// =====================================================================
	// =====================================================================

	// =====================================================================
	// For all non-object interfaces, return object reference
	// =====================================================================
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
	// =====================================================================
	// =====================================================================

};

#endif
