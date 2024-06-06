#ifndef _CJVXOUTPUTCONNECTOR_H__
#define _CJVXOUTPUTCONNECTOR_H__

#include "jvx.h"

class CjvxInputOutputConnectorCore;

class CjvxOutputConnectorCore
{
public:
	class common_set_ocon_t
	{
	public:
		IjvxConnectorBridge* conn_out = nullptr;
		jvxLinkDataDescriptor theData_out;
		IjvxDataConnectionCommon* theCommon_from = nullptr;

		jvxBool allows_termination = false;
		jvxBool setup_for_termination = false;

		// "Outside" representation of this output connector
		IjvxOutputConnector* ocon = nullptr;

		struct
		{
			jvxDataFormatGroup format_group = JVX_DATAFORMAT_GROUP_NONE;
			jvxDataflow data_flow = JVX_DATAFLOW_DONT_CARE;
		} caps_out;
	};

	common_set_ocon_t _common_set_ocon;

	CjvxInputOutputConnectorCore* _common_set_io_common_ptr = nullptr;

public:
	CjvxOutputConnectorCore();

	jvxErrorType activate(CjvxInputOutputConnectorCore* commRef, IjvxOutputConnector* ocon);
	jvxErrorType deactivate();

	jvxErrorType _associated_connection_ocon(IjvxDataConnectionCommon** ref);
	jvxErrorType _available_to_connect_ocon();

	jvxErrorType _supports_connector_class_ocon(
		jvxDataFormatGroup format_group,
		jvxDataflow data_flow);

	jvxErrorType _select_connect_ocon(IjvxConnectorBridge* bri,
		IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxOutputConnector** replace_connector);

	jvxErrorType _unselect_connect_ocon(IjvxConnectorBridge* bri,
		IjvxOutputConnector* replace_connector);

	// =====================================================================

	jvxErrorType _connect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb), 
		jvxDataProcessorHintDescriptor* add_me = NULL);

	jvxErrorType _disconnect_connect_ocon(const jvxChainConnectArguments& args, JVX_CONNECTION_FEEDBACK_TYPE_F(fdb) 
		jvxDataProcessorHintDescriptor** pointer_remove = NULL);

	// =====================================================================

	jvxErrorType _test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	// =====================================================================

	jvxErrorType _transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType _transfer_backward_ocon(jvxBool forward, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType _transfer_backward_backward(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// =====================================================================

	jvxErrorType _prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType _postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	// =======================================================================

	jvxErrorType _start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType _stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	// =======================================================================

	jvxErrorType _process_start_ocon(jvxSize pipeline_offset,
		jvxSize* idx_stage, jvxSize tobeAccessedByStage = 0,
		callback_process_start_in_lock clbk = NULL,
		jvxHandle* priv_ptr = NULL);
	jvxErrorType _process_buffers_ocon(jvxSize mt_mask, jvxSize idx_stage);
	jvxErrorType _process_stop_ocon(jvxSize idx_stage, jvxBool shift_fwd, jvxSize tobeAccessed = 0,
		callback_process_stop_in_lock clbk = NULL, jvxHandle* priv_ptr = NULL);

	// =======================================================================

	jvxErrorType _connected_icon(IjvxInputConnector** icon);

	// virtual IjvxOutputConnector* _reference_ocon() = 0;

	virtual jvxErrorType _check_common_ocon(IjvxDataConnectionCommon* ass_connection_common, IjvxConnectionMaster* master) = 0;


};

#endif
