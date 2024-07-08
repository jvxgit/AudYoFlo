#ifndef __IJVXCONNECTOR_H__
#define __IJVXCONNECTOR_H__

JVX_INTERFACE IjvxConnectorFactory;
JVX_INTERFACE IjvxInputConnector;
JVX_INTERFACE IjvxOutputConnector;
JVX_INTERFACE IjvxDataConnectionCommon;
JVX_INTERFACE IjvxTriggerOutputConnector;
JVX_INTERFACE IjvxTriggerInputConnector;

typedef jvxErrorType(*callback_process_stop_in_lock)(jvxSize pipe_idx, jvxHandle* priv_ptr);
typedef jvxErrorType(*callback_process_start_in_lock)(jvxSize* pipe_idx, jvxHandle* priv_ptr);

JVX_INTERFACE IjvxConnectorBridge
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxConnectorBridge(){};
	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_bridge(jvxApiString* str) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION properties_bridge(jvxBool* dedicatedThread, jvxBool* boost, jvxBool* interceptor) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connect_to(IjvxInputConnector** theToRef) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connect_to(IjvxInputConnector* theToRef) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connect_from(IjvxOutputConnector** theFromRef) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connect_from(IjvxOutputConnector* theFromRef) = 0; 
};

JVX_INTERFACE IjvxCommonConnector
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxCommonConnector(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_connector(jvxApiString* str) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION parent_factory(IjvxConnectorFactory** my_parent) = 0;
};

// =============================================================================

JVX_INTERFACE IjvxInputConnectorSelect : public IjvxCommonConnector, public IjvxConnectionIterator
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxInputConnectorSelect() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_icon(
		jvxDataFormatGroup format_group,
		jvxDataflow data_flow) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_icon(
		IjvxConnectorBridge* obj,
		IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxInputConnector** replace_connector = NULL) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_icon(
		IjvxConnectorBridge* obj,
		IjvxInputConnector* replace_connector = NULL) = 0;

	virtual IjvxInputConnector* reference_icon() = 0;
};

JVX_INTERFACE IjvxInputConnector : public IjvxInputConnectorSelect
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxInputConnector() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION associated_connection_icon(IjvxDataConnectionCommon** ref) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION available_to_connect_icon() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION connected_ocon(IjvxOutputConnector** ocon) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	//! Start processing a single frame. We may use the parameters to influence behavior:
	//! 1) idx_stage: Stage id for releasing. If JVX_SIZE_UNSELECTED, use the id as given by link data struct
	//! 2) operate_first_call: Bool to forward buffer index increment
	//! 3) tobeAccessedByStage: holds the process step Id related to the stage
	//! 4) callback_process_stop_in_lock: safe passing lock function
	//! 5) priv_data for 4)
	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset = 0,
		jvxSize* idx_stage = NULL, 
		jvxSize tobeAccessedByStage = 0,
		callback_process_start_in_lock = NULL, 
		jvxHandle* priv_ptr = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask = JVX_SIZE_UNSELECTED, jvxSize idx_stage = JVX_SIZE_UNSELECTED) = 0;

	//! Stop processing a single frame. We may use the parameters to influence behavior:
	//! 1) idx_stage: Stage id for releasing. If JVX_SIZE_UNSELECTED, use the id as given by link data struct
	//! 2) operate_first_call: Bool to forward buffer index increment
	//! 3) tobeAccessedByStage: holds the process step Id related to the stage
	//! 4) callback_process_stop_in_lock: safe passing lock function
	//! 5) priv_data for 4)
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(jvxSize idx_stage = JVX_SIZE_UNSELECTED, 
		jvxBool operate_first_call = true, 
		jvxSize tobeAccessedByStage = 0,
		callback_process_stop_in_lock = NULL, 
		jvxHandle* priv_ptr = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_trigger_otcon(IjvxTriggerOutputConnector** otcon) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_trigger_otcon(IjvxTriggerOutputConnector* otcon) = 0;
};

// =============================================================================

JVX_INTERFACE IjvxOutputConnectorSelect : public IjvxCommonConnector
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxOutputConnectorSelect() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_ocon(
		jvxDataFormatGroup format_group,
		jvxDataflow data_flow) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_ocon(IjvxConnectorBridge* obj,
		IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxOutputConnector** replace_connector = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_ocon(
		IjvxConnectorBridge* obj,
		IjvxOutputConnector* replace_connector = NULL) = 0;

	virtual IjvxOutputConnector* reference_ocon() = 0;
};

JVX_INTERFACE IjvxOutputConnector: public IjvxOutputConnectorSelect
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxOutputConnector() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION associated_connection_ocon(IjvxDataConnectionCommon** ref) = 0;

	// Return JVX_NO_ERROR if connector is available for a new connection
	virtual jvxErrorType JVX_CALLINGCONVENTION available_to_connect_ocon() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION connected_icon(IjvxInputConnector** icon) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(var)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_ocon(jvxSize pipeline_offset, jvxSize* idx_stage,
		jvxSize tobeAccessedByStage = 0,
		callback_process_start_in_lock = NULL,
		jvxHandle* priv_ptr = NULL) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_ocon(jvxSize mt_mask, jvxSize idx_stage ) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_ocon(jvxSize idx_stage ,
		jvxBool operate_first_call,
		jvxSize tobeAccessedByStage = 0,
		callback_process_stop_in_lock = NULL,
		jvxHandle* priv_ptr = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_trigger_itcon(IjvxTriggerInputConnector** itcon) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_trigger_itcon(IjvxTriggerInputConnector* itcon) = 0;

};
	
JVX_INTERFACE IjvxConnectorFactory : public IjvxInterfaceReference
{
public:
	virtual ~IjvxConnectorFactory(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION number_input_connectors(jvxSize* num_in_connectors) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_input_connector(jvxSize idx, IjvxInputConnectorSelect** ref) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_input_connector( IjvxInputConnectorSelect* ref) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION number_output_connectors(jvxSize* num_out_connectors) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_output_connector(jvxSize idx, IjvxOutputConnectorSelect** ref) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_output_connector( IjvxOutputConnectorSelect* ref) = 0;
};

#endif
