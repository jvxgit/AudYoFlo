#ifndef _CJVXINPUTCONNECTOR_H__
#define _CJVXINPUTCONNECTOR_H__

class CjvxInputConnector: public IjvxInputConnector
{
public:
	virtual JVX_CALLINGCONVENTION ~CjvxInputConnector(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION associated_common_icon(IjvxDataConnectionCommon** ref) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION connected_ocon(IjvxOutputConnector** ocon) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_icon(
		jvxDataFormatGroup format_group,
		jvxDataflow data_flow) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_icon(
		IjvxConnectorBridge* obj,
		IjvxConnectionMaster* master, 
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxInputConnector** replace_connector = NULL) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_icon(
		IjvxConnectorBridge* obj,
		IjvxInputConnector* replace_connector = NULL) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset = 0,
		jvxSize* idx_stage = NULL, 
		jvxSize tobeAccessedByStage = 0,
		callback_process_start_in_lock = NULL, 
		jvxHandle* priv_ptr = NULL) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask = JVX_SIZE_UNSELECTED, jvxSize idx_stage = JVX_SIZE_UNSELECTED) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(jvxSize idx_stage = JVX_SIZE_UNSELECTED, 
		jvxBool operate_first_call = true, 
		jvxSize tobeAccessedByStage = 0,
		callback_process_stop_in_lock = NULL, 
		jvxHandle* priv_ptr = NULL) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var)) override;

};

#endif
