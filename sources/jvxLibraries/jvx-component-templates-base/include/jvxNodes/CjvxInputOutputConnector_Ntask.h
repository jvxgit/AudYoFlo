#ifndef __CJVXINPUTOUTPUT_NTASK_H__
#define __CJVXINPUTOUTPUT_NTASK_H__

#include "jvx.h"
#include "jvxNodes/CjvxInputOutputConnector_NVtask.h"

class CjvxInputConnectorNtask;
class CjvxOutputConnectorNtask;

JVX_INTERFACE IjvxInputOutputConnectorNtask
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxInputOutputConnectorNtask() {};

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ntask(
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out,
		jvxSize idTask) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ntask(
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, 
		jvxSize idTask) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon_ntask(jvxLinkDataDescriptor* theData_out, jvxSize idTask,
		IjvxInputConnector* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon_ntask(jvxLinkDataTransferType tp, jvxHandle* data, jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon_ntask(jvxLinkDataTransferType tp, jvxHandle* data, jvxLinkDataDescriptor* theData_out,
		jvxLinkDataDescriptor* theData_in, jvxSize idTask, CjvxInputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon_ntask(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out,
		jvxSize idTask, CjvxOutputConnectorNtask* refto,
		jvxSize mt_mask, jvxSize idx_stage) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_ocon_ntask(jvxSize idTask,
		jvxSize mt_mask, jvxSize idx_stage) = 0;
};

// =========================================================================

class CjvxCommonNtask: public CjvxCommonNVtask
{
public:
	struct
	{
		IjvxInputOutputConnectorNtask* cbRef;
	} _common_set_comntask;

public:
	CjvxCommonNtask(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, IjvxInputOutputConnectorNtask* report,
		IjvxConnectionMaster* aM, IjvxObject* objRef, jvxSize ctxtSubId);
};

// ===============================================================================

class CjvxInputConnectorNtask : public CjvxInputConnectorNVtask
{
public:

	struct
	{
		CjvxOutputConnectorNtask* ocon;
	} _common_set_icon_ntask;

	JVX_MUTEX_HANDLE safeAccessConnection;
	CjvxCommonNtask common_ntask;

	CjvxInputConnectorNtask(jvxSize ctxId, const char* descr,
		IjvxConnectorFactory* par, IjvxInputOutputConnectorNtask* report,
		IjvxConnectionMaster* aM,
		IjvxObject* objRef,
		jvxSize ctxtSubId = 0);
	~CjvxInputConnectorNtask();

	// =======================================================================================================
	jvxErrorType set_reference(CjvxOutputConnectorNtask* ref);
	// =======================================================================================================

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_CLASS(descriptor_connector, common_ntask, (jvxApiString* str), (str));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_CLASS(parent_factory, common_ntask, (IjvxConnectorFactory** my_parent),
		(my_parent));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_FUNCNAME_CLASS(associated_connection_icon, associated_connection, common_ntask,
		(IjvxDataConnectionCommon** ref), (ref));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_FUNCNAME_CLASS(available_to_connect_icon, available_to_connect, common_ntask, (), ());

	// =======================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_icon(
		IjvxConnectorBridge* obj,
		IjvxConnectionMaster* master,
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

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
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

// ===================================================================================

class CjvxOutputConnectorNtask : public CjvxOutputConnectorNVtask
{
public:
	struct
	{
		CjvxInputConnectorNtask* icon;

	} _common_set_ocon_ntask;

	CjvxCommonNtask common_ntask;

	CjvxOutputConnectorNtask(jvxSize ctxId, const char* descr, 
		IjvxConnectorFactory* par, IjvxInputOutputConnectorNtask* report,
		IjvxConnectionMaster* aM,
		IjvxObject* objRef,
		jvxSize ctxtSubId = 0);

	// =======================================================================================================
	jvxErrorType set_reference(CjvxInputConnectorNtask*ref);

	// Referencing the base class implemetation with _
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_CLASS(descriptor_connector, common_ntask, (jvxApiString* str), (str));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_CLASS(parent_factory, common_ntask, (IjvxConnectorFactory** my_parent),
		(my_parent));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_FUNCNAME_CLASS(associated_connection_ocon, associated_connection, common_ntask, 
		(IjvxDataConnectionCommon** ref), (ref));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_FUNCNAME_CLASS(available_to_connect_ocon, available_to_connect, common_ntask,(), ());

	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(prepare_connect_ocon, (JVX_CONNECTION_FEEDBACK_TYPE(fdb)),
		(JVX_CONNECTION_FEEDBACK_CALL(fdb)));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(postprocess_connect_ocon, (JVX_CONNECTION_FEEDBACK_TYPE(fdb)), 
		(JVX_CONNECTION_FEEDBACK_CALL(fdb)));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(start_connect_ocon, (JVX_CONNECTION_FEEDBACK_TYPE(fdb)),
		(JVX_CONNECTION_FEEDBACK_CALL(fdb)));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(stop_connect_ocon, (JVX_CONNECTION_FEEDBACK_TYPE(fdb)), 
		(JVX_CONNECTION_FEEDBACK_CALL(fdb)));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(process_start_ocon, 
		(jvxSize pipeline_offset, jvxSize* idx_stage, 
			jvxSize tobeAccessedByStage, 
			callback_process_start_in_lock clbk, 
			jvxHandle* priv_ptr), 
		(pipeline_offset, idx_stage, 
			tobeAccessedByStage, clbk, 
			priv_ptr));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(process_stop_ocon, \
		(jvxSize idx_stage, jvxBool shift_fwd, jvxSize tobeAccessedByStage, callback_process_stop_in_lock cb, jvxHandle* priv_ptr), 
		(idx_stage, shift_fwd, tobeAccessedByStage, cb, priv_ptr));
	JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(transfer_forward_ocon, 
		(jvxLinkDataTransferType tp, 
			jvxHandle* hdl 
			JVX_CONNECTION_FEEDBACK_TYPE_A(var)),
		(tp, hdl JVX_CONNECTION_FEEDBACK_CALL_A(var)));

	// =======================================================================================================

	JVX_SUBCALL_FUNCTION_DECLARE_OVERRIDE(process_buffers_ocon,
		(jvxSize mt_mask, jvxSize idx_stage));

	// =======================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_ocon(
		IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxOutputConnector** replace_connector)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_ocon(
		IjvxConnectorBridge* obj,
		IjvxOutputConnector* replace_connector)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(var))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION connected_icon(IjvxInputConnector**)override;
	

};

#endif