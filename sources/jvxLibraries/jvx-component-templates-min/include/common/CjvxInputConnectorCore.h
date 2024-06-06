#ifndef _CJVXINPUTCONNECTOR_H__
#define _CJVXINPUTCONNECTOR_H__

#include "jvx.h"

class CjvxInputOutputConnectorCore;

class CjvxInputConnectorCore
{
public:

	class common_set_icon_t
	{
	public:
		IjvxConnectorBridge* conn_in = nullptr;
		jvxLinkDataDescriptor* theData_in = nullptr;
		IjvxDataConnectionCommon* theCommon_to = nullptr;

		// "Outside" representation of this input connector
		IjvxInputConnector* icon = nullptr;

		struct
		{
			jvxDataFormatGroup format_group = JVX_DATAFORMAT_GROUP_NONE;
			jvxDataflow data_flow = JVX_DATAFLOW_DONT_CARE;
		} caps_in;
	};

	common_set_icon_t _common_set_icon;

	CjvxInputOutputConnectorCore* _common_set_io_common_ptr = nullptr;

public:
	CjvxInputConnectorCore();

	jvxErrorType activate(CjvxInputOutputConnectorCore* commRef, IjvxInputConnector* icon);
	jvxErrorType deactivate();

	jvxErrorType _associated_connection_icon(IjvxDataConnectionCommon** ref);
	jvxErrorType _available_to_connect_icon();


	jvxErrorType _supports_connector_class_icon(
		jvxDataFormatGroup format_group,
		jvxDataflow data_flow);

	// ===============================================================================

	jvxErrorType _select_connect_icon(IjvxConnectorBridge* bri, IjvxConnectionMaster* master,
			IjvxDataConnectionCommon* ass_connection_common,
			IjvxInputConnector** replace_connector);

	jvxErrorType _unselect_connect_icon(IjvxConnectorBridge* bri, IjvxInputConnector* replace_connector);

	// ===============================================================================

	jvxErrorType _connect_connect_icon(jvxLinkDataDescriptor *theData, jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType _connect_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	jvxErrorType _disconnect_connect_icon(jvxLinkDataDescriptor* theData, jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType _disconnect_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	// ===============================================================================

	jvxErrorType _prepare_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType _prepare_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	jvxErrorType _postprocess_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType _postprocess_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	// ===============================================================================

	jvxErrorType _start_connect_icon(jvxBool forward, jvxSize myUniquePipelineId JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType _start_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual void _start_connect_common(jvxSize myUniquePipelineId) = 0;

	jvxErrorType _stop_connect_icon(jvxBool forward, jvxSize* uId JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType _stop_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual void _stop_connect_common(jvxSize* myUniquePipelineId) = 0;
	// ===============================================================================

	jvxErrorType _test_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType _test_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	// ===============================================================================
	
	jvxErrorType JVX_CALLINGCONVENTION _transfer_forward_icon(jvxBool forward, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType JVX_CALLINGCONVENTION _transfer_forward_forward(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
		jvxErrorType JVX_CALLINGCONVENTION _transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// ===============================================================================

		jvxErrorType _process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage, jvxSize tobeAccessedByStage = 0,
			callback_process_start_in_lock clbk = NULL,
			jvxHandle* priv_ptr = NULL);
		virtual jvxErrorType _process_start_forward() = 0;

	// ===============================================================================
	

	jvxErrorType _process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd, jvxSize tobeAccessed = 0,
			callback_process_stop_in_lock clbk = NULL, jvxHandle* priv_ptr = NULL);
	virtual jvxErrorType _process_stop_forward(jvxSize idx_stage, jvxBool shift_fwd,
		jvxSize tobeAccessedByStage, callback_process_stop_in_lock clbk, jvxHandle* priv_ptr) = 0;

		// ===============================================================================

	jvxErrorType _process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage);
	virtual jvxErrorType _process_buffers_forward(jvxSize mt_mask, jvxSize idx_stage) = 0;

	// ===============================================================================
	
	jvxErrorType _connected_ocon(IjvxOutputConnector** ocon);

	// ===============================================================================

		// ===============================================================================
		virtual jvxErrorType _check_common_icon(IjvxDataConnectionCommon* ass_connection_common, IjvxConnectionMaster* master) = 0;

	// ===============================================================================

		jvxErrorType
			shift_buffer_pipeline_idx_on_start(jvxSize pipeline_offset,
				jvxSize* idx_stage,
				jvxSize tobeAccessedByStage,
				callback_process_start_in_lock clbk = NULL,
				jvxHandle* priv_ptr = NULL);
		jvxErrorType shift_buffer_pipeline_idx_on_stop(jvxSize idx_stage,
			jvxBool shift_fwd, jvxSize tobeAccessed,
			callback_process_stop_in_lock clbk, jvxHandle* priv_ptr);

		jvxErrorType allocate_pipeline_and_buffers_prepare_to();
		jvxErrorType deallocate_pipeline_and_buffers_postprocess_to();
		jvxErrorType deallocate_pipeline_and_buffers_postprocess_to_zerocopy();

	
};

#endif
