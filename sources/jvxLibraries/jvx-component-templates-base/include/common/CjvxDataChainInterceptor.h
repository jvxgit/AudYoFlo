#ifndef __CJVXDATACHAININTERCEPTOR_H__
#define __CJVXDATACHAININTERCEPTOR_H__

#include "common/CjvxInputOutputConnector.h"

typedef enum
{
	JVX_PROCESS_MARK_INOUT_CONNECTOR,
	JVX_PROCESS_MARK_ENTER_CONNECTOR,
	JVX_PROCESS_MARK_LEAVE_CONNECTOR
} jvxDataChainConnectorTag;

class CjvxDataChainInterceptor:	
	public IjvxInputConnector, public IjvxOutputConnector
{
protected:

	IjvxDataChainInterceptorReport* myReport = nullptr;
	IjvxConnectorFactory* myParent = nullptr;
	
	IjvxToolsHost* toolsHost = nullptr;
	IjvxUniqueId* uniqueId = nullptr;

	IjvxConnectorBridge* conn_in = nullptr;
	IjvxConnectorBridge* conn_out = nullptr;

	IjvxInputConnector* icon = nullptr;
	IjvxOutputConnector* ocon = nullptr;

	jvxLinkDataDescriptor* theData_out = nullptr;
	jvxLinkDataDescriptor* theData_in = nullptr;

	IjvxConnectionMaster* theMaster_in = nullptr;
	IjvxConnectionMaster* theMaster_out = nullptr;

	IjvxDataConnectionCommon* theCommon_in = nullptr;
	IjvxDataConnectionCommon* theCommon_out = nullptr;

	std::string nm_process;
	std::string nm_bridge;
	jvxSize id_interceptor;

	std::string nm_ic;

	std::string nm_connect_to;

	jvxBool isBridgeIn;
	jvxBool isBridgeOut;
	jvxBool isProcess;

	IjvxObject* objParent = nullptr;

	jvxBool dedicatedThread;
	jvxBool boostThread;
	jvxDataChainConnectorTag my_tag;
	jvxSize idx_pipe_stage;

public:
	CjvxDataChainInterceptor(
		IjvxConnectorFactory* myParentRef, IjvxObject* objPar, 
		const std::string& name_connection, const std::string& name_bridge,
		jvxSize cnt_interceptor,
		IjvxDataChainInterceptorReport* repRef, jvxBool dedicatedThread = false, 
		jvxBool boostThread = false);

	~CjvxDataChainInterceptor();

	virtual jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_icon(jvxDataFormatGroup format_group,
		jvxDataflow data_flow) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_ocon(jvxDataFormatGroup format_group,
		jvxDataflow data_flow) override;

	jvxErrorType check_master_common();
	jvxErrorType description_interceptor(jvxApiString* str);
	void set_tools_host(IjvxToolsHost* toolshost);
	void tag(jvxDataChainConnectorTag tagParam);

	void updateName(const char*  newNm);

	virtual IjvxInputConnector* reference_icon() override
	{
		return this;
	}

	virtual IjvxOutputConnector* reference_ocon() override
	{
		return this;
	}

	// =========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_connector(jvxApiString* str) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION parent_factory(IjvxConnectorFactory** my_parent) override;
	
	// =============================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION associated_common_icon(IjvxDataConnectionCommon** ref) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_icon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master, 
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxInputConnector** replace_connector) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_icon(IjvxConnectorBridge* obj,
		IjvxInputConnector* replace_connector) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_icon(jvxLinkDataDescriptor* theData 
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon(jvxLinkDataDescriptor* theData 
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset = 0, 
		jvxSize* idx_stage = NULL,
		jvxSize tobeAccessedByStage = 0,
		callback_process_start_in_lock clbk = NULL,
		jvxHandle* priv_ptr = NULL) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(jvxSize idx_stage, 
		jvxBool shift_fwd, 
		jvxSize tobeAccessedByStage = 0,
		callback_process_stop_in_lock clbk = NULL, 
		jvxHandle* priv_ptr = NULL) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* 
		JVX_CONNECTION_FEEDBACK_TYPE_A(var)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle*
		JVX_CONNECTION_FEEDBACK_TYPE_A(var)) override;

	// ====================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION associated_common_ocon(IjvxDataConnectionCommon** ref) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_ocon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master, 
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxOutputConnector** replace_connector) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_ocon(IjvxConnectorBridge* obj,
		IjvxOutputConnector* replace_connector) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ocon(const jvxChainConnectArguments& args
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ocon(const jvxChainConnectArguments& args
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(var))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_ocon(
		jvxSize pipeline_offset, 
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage = 0,
		callback_process_start_in_lock clbk = NULL,
		jvxHandle* priv_ptr = NULL) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_ocon(jvxSize mt_mask, jvxSize idx_stage) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_ocon(
		jvxSize idx_stage, 
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage = 0,
		callback_process_stop_in_lock clbk = NULL,
		jvxHandle* priv_ptr = NULL) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle*, 
		JVX_CONNECTION_FEEDBACK_TYPE(var)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle*, 
		JVX_CONNECTION_FEEDBACK_TYPE(var)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_next(jvxSize* num) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_next(jvxSize idx, IjvxConnectionIterator** next) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_component(
		jvxComponentIdentification* cpTp, 
		jvxApiString* modName,
		jvxApiString* description,
		jvxApiString* lContext) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION connected_icon(IjvxInputConnector** icon) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION connected_ocon(IjvxOutputConnector** ocon) override;

	jvxErrorType request_trigger_itcon(IjvxTriggerInputConnector** itcon) override;
	jvxErrorType return_trigger_itcon(IjvxTriggerInputConnector* itcon) override;
	jvxErrorType JVX_CALLINGCONVENTION request_trigger_otcon(IjvxTriggerOutputConnector** otcon)override;
	jvxErrorType JVX_CALLINGCONVENTION return_trigger_otcon(IjvxTriggerOutputConnector* otcon) override;

	virtual jvxErrorType try_run_processing(IjvxConnectionMaster* master_ref);
};

#endif
