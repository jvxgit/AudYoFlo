#ifndef __CJVXINPUTOUTPUT_VTASK_H__
#define __CJVXINPUTOUTPUT_VTASK_H__

#include "jvx.h"
#include "jvxNodes/CjvxInputOutputConnector_NVtask.h"

JVX_INTERFACE IjvxInputOutputConnectorVtask
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxInputOutputConnectorVtask() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION report_select_input_vtask(
		jvxSize ctxtIdx,
		jvxSize ctxtSubIdx,
		IjvxConnectorBridge* obj,
		IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxInputConnector* expose_connector,
		IjvxInputConnector** replace_connector) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_select_output_vtask(
		jvxSize ctxtIdx,
		jvxSize ctxtSubIdx,
		IjvxConnectorBridge* obj,
		IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxOutputConnector* expose_connector,
		IjvxOutputConnector** replace_connector) = 0;

	// =============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION report_unselect_input_vtask(
		jvxSize ctxtIdx,
		jvxSize ctxtSubIdx,
		IjvxInputConnector* expose_connector,
		IjvxInputConnector* replace_connector) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_unselect_output_vtask(
		jvxSize ctxtIdx,
		jvxSize ctxtSubIdx,
		IjvxOutputConnector* expose_connector,
		IjvxOutputConnector* replace_connector) = 0;

	// =============================================================

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_icon_vtask(
		jvxSize idTask, 
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon_vtask(
		jvxSize idTask, 
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon_vtask(
		jvxSize idTask, 
		jvxSize subIdTask 
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon_vtask(
		jvxSize idTask,
		jvxSize subIdTask,
		jvxLinkDataDescriptor* theDataOut
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon_vtask(
		jvxSize idTask, 
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon_vtask(
		jvxSize idTask, 
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon_vtask(
		jvxSize idTask, 
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon_vtask(
		jvxSize idTask, 
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon_vtask(jvxLinkDataTransferType tp, 
		jvxHandle* data, 
		jvxSize idTask, 
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon_vtask(
		jvxLinkDataTransferType tp, 
		jvxHandle* data, 
		jvxSize idTask, 
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon_vtask(
		jvxSize pipeline_offset,
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr,
		jvxSize procId) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon_vtask(		
		jvxSize mt_mask, 
		jvxSize idx_stage,
		jvxSize procId) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon_vtask(
		jvxSize idx_stage,
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr, 
		jvxSize procId) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_next_icon_vtask(
		jvxSize* num, 
		jvxSize idTask,
		jvxSize subIdTask) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_next_icon_vtask(
		jvxSize idx, 
		IjvxConnectionIterator** next,
		jvxSize idTask,
		jvxSize subIdTask) = 0;
};

class CjvxCommonVtask : public CjvxCommonNVtask
{
public:

	enum class vTaskConnectiorRole
	{
		JVX_VTASK_CONNECTOR_EXPOSE,
		JVX_VTASK_CONNECTOR_USE
	} ;

public:
	class common_set_comvtask_tp
	{
	public:
		vTaskConnectiorRole myRole;
		IjvxInputOutputConnectorVtask* cbRef;
		common_set_comvtask_tp()
		{
			myRole = vTaskConnectiorRole::JVX_VTASK_CONNECTOR_EXPOSE;
			cbRef = NULL;
		};
	};
	
	common_set_comvtask_tp _common_set_comvtask;

public:
	CjvxCommonVtask(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, IjvxInputOutputConnectorVtask* report,
		IjvxConnectionMaster* aM, IjvxObject* objRef, jvxSize ctxtSubId,
		CjvxCommonVtask::vTaskConnectiorRole role = vTaskConnectiorRole::JVX_VTASK_CONNECTOR_EXPOSE);

	~CjvxCommonVtask();
};

class CjvxInputConnectorVtask : public CjvxInputConnectorNVtask
{
public:

	struct
	{
		jvxSize id;
	} in_proc;

	CjvxCommonVtask common_vtask;

	CjvxInputConnectorVtask(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, IjvxInputOutputConnectorVtask* report,
		IjvxConnectionMaster* aM, IjvxObject* objRef, jvxSize ctxtSubId,
		CjvxCommonVtask::vTaskConnectiorRole role = CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_EXPOSE) ;

	CjvxInputConnectorVtask(CjvxInputConnectorVtask* copyThis, jvxSize ctxtIdx,
		CjvxCommonVtask::vTaskConnectiorRole role = CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_USE);

	~CjvxInputConnectorVtask();
		
	jvxSize context_subid()
	{
		return _common_nvtask->_context_subid();
	};

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_CLASS(descriptor_connector, common_vtask, (jvxApiString* str), (str));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_CLASS(parent_factory, common_vtask,(IjvxConnectorFactory** my_parent), (my_parent));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_FUNCNAME_CLASS(associated_common_icon, associated_common, common_vtask,(IjvxDataConnectionCommon** ref), (ref));

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_icon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxInputConnector** replace_connector)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_icon(IjvxConnectorBridge* bri,
		IjvxInputConnector* replace_connector)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))override;

	// ==========================================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(jvxSize pipeline_offset, 
		jvxSize* idx_stage, 
		jvxSize tobeAccessedByStage = 0,
		callback_process_start_in_lock clbk = NULL,
		jvxHandle* priv_ptr = NULL)override;


	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage )override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
		jvxSize idx_stage, 
		jvxBool shift_fwd, 
		jvxSize tobeAccessedByStage = 0,
		callback_process_stop_in_lock clbk = NULL, 
		jvxHandle* priv_ptr = NULL)override;


	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_next(jvxSize* num) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_next(jvxSize idx, IjvxConnectionIterator** next) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_component(
		jvxComponentIdentification* cpTp, 
		jvxApiString* modName,
		jvxApiString* description,
		jvxApiString* lContext) override;

};

class CjvxOutputConnectorVtask : public CjvxOutputConnectorNVtask
{
public:

	CjvxCommonVtask common_vtask;

	CjvxOutputConnectorVtask(jvxSize ctxId, const char* descr,
		IjvxConnectorFactory* par, IjvxInputOutputConnectorVtask* report,
		IjvxConnectionMaster* aM,
		IjvxObject* objRef,
		jvxSize ctxtSubId = 0,
		CjvxCommonVtask::vTaskConnectiorRole role = CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_EXPOSE);

	CjvxOutputConnectorVtask(CjvxOutputConnectorVtask* copyThis, jvxSize ctxtIdx,
		CjvxCommonVtask::vTaskConnectiorRole role = CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_USE);

	// =======================================================================================================
	// =======================================================================================================

	jvxSize context_subid()
	{
		return _common_nvtask->_context_subid();
	};

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_CLASS(descriptor_connector, 
		common_vtask,(jvxApiString* str), (str));

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_CLASS(parent_factory, 
		common_vtask, (IjvxConnectorFactory** my_parent), (my_parent));

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_FUNCNAME_CLASS(associated_common_ocon, 
		associated_common, common_vtask, (IjvxDataConnectionCommon** ref), (ref));

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(prepare_connect_ocon, 
		(JVX_CONNECTION_FEEDBACK_TYPE(fdb)), (JVX_CONNECTION_FEEDBACK_CALL(fdb)));

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(postprocess_connect_ocon, 
		(JVX_CONNECTION_FEEDBACK_TYPE(fdb)), (JVX_CONNECTION_FEEDBACK_CALL(fdb)));

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(start_connect_ocon,
		(JVX_CONNECTION_FEEDBACK_TYPE(fdb)), (JVX_CONNECTION_FEEDBACK_CALL(fdb)));

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(stop_connect_ocon, 
		(JVX_CONNECTION_FEEDBACK_TYPE(fdb)), (JVX_CONNECTION_FEEDBACK_CALL(fdb)));

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(process_start_ocon, 
		(jvxSize pipeline_offset, jvxSize* idx_stage, jvxSize tobeAccessedByStage, callback_process_start_in_lock clbk, jvxHandle* priv_ptr), 
		(pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(process_stop_ocon, 
		(jvxSize idx_stage, jvxBool shift_fwd, jvxSize tobeAccessedByStage, callback_process_stop_in_lock cb, jvxHandle* priv_ptr), 
		(idx_stage, shift_fwd, tobeAccessedByStage, cb, priv_ptr));

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(process_buffers_ocon,
		(jvxSize mt_mask = JVX_SIZE_UNSELECTED, jvxSize idx_stage = JVX_SIZE_UNSELECTED),
		(mt_mask, idx_stage));

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(connect_connect_ocon,
		(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)),
		(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb)));

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(disconnect_connect_ocon,
		(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)),
		(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb)));

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(transfer_forward_ocon,
		(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(var)),
		(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(var)));

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_ocon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxOutputConnector** replace_connector)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_ocon(
		IjvxConnectorBridge* obj,
		IjvxOutputConnector* replace_connector)override;

	//virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ocon(IjvxConnectionMaster* theMaster JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	//virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(var))override;


	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* JVX_CONNECTION_FEEDBACK_TYPE_A(var))override;
};

#endif
