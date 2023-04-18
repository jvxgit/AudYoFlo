#ifndef _CJVXOUTPUTCONNECTOR_H__
#define _CJVXOUTPUTCONNECTOR_H__

#include "jvx.h"

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
			jvxDataflow data_flow = JVX_DATAFLOW_NONE;
		} caps_out;
	};

	common_set_ocon_t _common_set_ocon;
public:
	CjvxOutputConnectorCore();

	jvxErrorType activate(IjvxOutputConnector* ocon);
	jvxErrorType deactivate();

	jvxErrorType _associated_common_ocon(IjvxDataConnectionCommon** ref);

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

	jvxErrorType _prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType _postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	// =======================================================================

	jvxErrorType _start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType _stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	// =======================================================================

	jvxErrorType _connected_icon(IjvxInputConnector** icon);

	// virtual IjvxOutputConnector* _reference_ocon() = 0;

	virtual jvxErrorType _check_common_ocon(IjvxDataConnectionCommon* ass_connection_common, IjvxConnectionMaster* master) = 0;


};

#endif
