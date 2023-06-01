#ifndef __CJVXNVTASKS_H__
#define __CJVXNVTASKS_H__

#include "jvx.h"
#include "jvxNodes/CjvxBareNtask.h"
#include "pcg_CjvxNVTasks_pcg.h"
#include "jvxNodes/CjvxInputOutputConnector_Vtask.h"
#include "common/jvxTypedefsVTask.h"

// Define some constants
namespace {
	constexpr jvxSize JVX_NVTASK_OFFSET_VARIABLE_TASKS = 1024;
};

//#define JVX_CJVXNVTASKS_UPDATE_MASTER_RATE

class CjvxNVTasks : public CjvxBareNtask, public IjvxInputOutputConnectorVtask, public CjvxNVTasks_pcg
{
public:
	enum class jvxCombineSignalsMode
	{
		JVX_COMBINE_SIGNALS_MODE_REPLACE,
		JVX_COMBINE_SIGNALS_MODE_MIX
	};

protected:

	struct
	{
		std::map<jvxSize, jvxOneVariableConnectorTaskDefinition> variableTasks;
	} _common_set_node_vtask;

	typedef enum 
	{
		JVX_AUNTASKS_SECONDARY_MASTER,
		JVX_AUNTASKS_PRIMARY_MASTER,
	} jvxAuNTasks_configFlowType;
		
	CjvxNegotiate_input neg_secondary_input;
	CjvxNegotiate_output neg_secondary_output;

	IjvxInputConnector* sec_input;
	IjvxInputConnector* pri_input;

	buffer_cfg cfg_sec_buffer;
	jvxBool sec_zeroCopyBuffering_rt;

	jvxBool couple_buffersize_vtasks = true;

	jvxAuNTasks_configFlowType parameter_config;

	struct
	{
		jvxBool buffersize_pri_to_sec;
		jvxBool rate_pri_to_sec;
	} auto_parameters;	

	class oneEntryProcessingVTask
	{
	public:
		jvxSize connId = JVX_SIZE_UNSELECTED;
		jvxSize uid = JVX_SIZE_UNSELECTED;
		oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* icon = NULL;
		std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* > ocon;

		oneEntryProcessingVTask()
		{
			uid = 0;
			icon = NULL;
		}
	};

	struct
	{
		std::map<jvxSize, oneEntryProcessingVTask> lst_in_proc_tasks;

		// This lock secures the lst_in_proc_tasks and all other locations where new ACTIVE connections are switched from prepared to active
		// it mostly protects list lst_in_proc_tasks but may be used in extend.
		// It is also used in the process call to only check started links
		JVX_MUTEX_HANDLE safeAcces_proc_tasks;
		jvxSize uId_proc_tasks;
	} _common_set_nv_proc;	

#ifdef JVX_CJVXNVTASKS_UPDATE_MASTER_RATE
	jvxTimeStampData tStampRates;
	jvxTick tStampRates_last_out;
	jvxData delta_t_smoothed;
#endif

	jvxCombineSignalsMode combinSignalMode = jvxCombineSignalsMode::JVX_COMBINE_SIGNALS_MODE_MIX;
public:

	JVX_CALLINGCONVENTION CjvxNVTasks(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxNVTasks();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

#ifdef JVX_CJVXNVTASKS_UPDATE_MASTER_RATE
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;
#endif

	virtual void activate_local_tasks()override;
	virtual void deactivate_local_tasks()override;

	// =======================================================================
	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_icon_vtask(
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon_vtask(
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon_vtask(
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon_vtask(
		jvxSize idTask,
		jvxSize subIdTask,
		jvxLinkDataDescriptor* theDataOut
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	//! This callback is used to run a final correction when outputting parameters to the connected secondary chain
	virtual void test_set_output_parameters(jvxSize ctxtId, jvxSize ctxtSubId, jvxLinkDataDescriptor* theDataOut);

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon_vtask(
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon_vtask(
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon_vtask(
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon_vtask(
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon_vtask(
		jvxLinkDataTransferType tp,
		jvxHandle* data, 
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon_vtask(
		jvxLinkDataTransferType tp,
		jvxHandle* data,
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon_vtask(
		jvxSize pipeline_offset,
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr,
		jvxSize procId) override;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon_vtask(		
		jvxSize mt_mask,
		jvxSize idx_stage,
		jvxSize procId) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon_vtask(
		jvxSize idx_stage,
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr,
		jvxSize procId) override;

	// =========================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION report_select_input_vtask(
		jvxSize ctxtIdx,
		jvxSize ctxtSubIdx,
		IjvxConnectorBridge* obj,
		IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxInputConnector* expose_connector,
		IjvxInputConnector** replace_connector) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_select_output_vtask(
		jvxSize ctxtIdx,
		jvxSize ctxtSubIdx,
		IjvxConnectorBridge* obj,
		IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common,
		IjvxOutputConnector* expose_connector,
		IjvxOutputConnector** replace_connector) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_unselect_input_vtask(
		jvxSize ctxtIdx,
		jvxSize ctxtSubIdx,
		IjvxInputConnector* expose_connector,
		IjvxInputConnector* replace_connector) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_unselect_output_vtask(
		jvxSize ctxtIdx,
		jvxSize ctxtSubIdx,
		IjvxOutputConnector* expose_connector,
		IjvxOutputConnector* replace_connector) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_next_icon_vtask(
		jvxSize* num, 
		jvxSize idTask,
		jvxSize subIdTask) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_next_icon_vtask(
		jvxSize idx, 
		IjvxConnectionIterator** next,
		jvxSize idTask,
		jvxSize subIdTask) override;

	virtual jvxErrorType is_ready(jvxBool* isReady, jvxApiString* reason)override;

	jvxErrorType test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
			jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	// =============================================================================

	// This function is invoked in a test call run to indicate new input parameters
	// It may be from the "test" function or from the "complain" function
	virtual void accept_input_settings_vtask_start(jvxLinkDataDescriptor* datIn,
		oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
		std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon);

	// This function is invoked in a test call run to indicate new input parameters
	// It may be from the "test" function or from the "complain" function
	virtual void accept_input_settings_vtask_stop(std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask,
		CjvxNegotiate_output>* >& ocon);
	
	// This function is invoked in a test call run to indicate new output parameters
		// It may be from the "test" function or from the "complain" function
	virtual void accept_output_settings_vtask_start(jvxLinkDataDescriptor* datOut,
		oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>*& ocon,
		std::list<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* >& icon);
	
	// This function is invoked in a test call run to indicate new output parameters
	// It may be from the "test" function or from the "complain" function
	virtual void accept_output_settings_vtask_stop(
		std::list<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* >& icon);

	// This function return the single input connector and the related output connectors
	// In the default implem
	virtual jvxErrorType find_i_map(jvxSize ctxtId, jvxSize ctxtSubId,
		oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
		std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>*>& ocon);

	virtual jvxErrorType find_o_map(jvxSize ctxtId, jvxSize ctxtSubId,
		std::list<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*>& icon,
		oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>*& ocon);

	virtual void show_properties_io_task_vtask(const jvxOneConnectorTaskRuntime& task);

	virtual jvxErrorType transfer_forward_icon_vtask_core(
		jvxLinkDataTransferType tp, jvxHandle* data,
		oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
		std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	virtual jvxErrorType transfer_backward_ocon_vtask_core(
		jvxLinkDataTransferType tp, jvxHandle* data,
		oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>*& ocon,
		std::list<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* >& icon
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// =============================================================

	virtual jvxErrorType prepare_connect_icon_vtask_core(
		oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
		std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	virtual jvxErrorType start_connect_icon_vtask_core(
		oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
		std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	virtual jvxErrorType stop_connect_icon_vtask_core(
		oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
		std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	virtual jvxErrorType postprocess_connect_icon_vtask_core(
		oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
		std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// =============================================================================

	virtual jvxErrorType process_buffers_icon_nvtask_master(
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out,
		jvxSize mt_mask, jvxSize idx_stage)override;

	virtual jvxErrorType process_buffers_icon_ntask_attached(
		jvxSize ctxtId,
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out,
		jvxSize mt_mask, jvxSize idx_stage)override;

	jvxErrorType process_buffers_icon_vtask_attached(
		oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
		std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon,
		jvxSize mt_mask, jvxSize idx_stage);

	// =============================================================================

private:

	// Following functions to run code IN the critical section in which the connection is hooked up 
	// for processing

	// In lock _common_set_nv_proc.safeAcces_proc_tasks: new connection has just been added to _common_set_nv_proc.lst_in_proc_tasks
	void safe_callback_start_connection(jvxSize uId);

	// In lock _common_set_nv_proc.safeAcces_proc_tasks: connection is still in _common_set_nv_proc.lst_in_proc_tasks but will be removed on return
	void safe_callback_stop_connection(jvxSize uId);

	jvxErrorType attach_single_input_connector(
		CjvxInputConnectorVtask* copyThis, 
		jvxSize ctxtTypeIdx, 
		jvxSize ctxtIdx,
		IjvxConnectorBridge* obj,
		IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common,
		std::vector<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>>& icons,
		jvxBitField* activeInputs);
	
	jvxErrorType attach_single_output_connector(
		CjvxOutputConnectorVtask* copyThis, 
		jvxSize ctxtTypeId,
		jvxSize ctxtIdx,
		IjvxConnectorBridge* obj,
		IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common,
		std::vector<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>>& ocons,
		jvxBitField* activeOutputs);

	void update_exposed_tasks();
	
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(select_task);
	void show_properties_io_task(jvxBool* trigger_ui_task);
};

#endif