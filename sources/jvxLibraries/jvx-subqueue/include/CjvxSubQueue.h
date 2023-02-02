#include "jvxNodes/CjvxBareNode1io_rearrange.h"
#include "pcg_CjvxSubQueue_pcg.h"

#include "HjvxMicroConnection.h"



JVX_INTERFACE CjvxSubQueue_interact
{
public:

	enum class jvxSubQueueOperationState
	{
		JVX_SUB_QUEUE_OPERATION_STATE_NOT_READY,
		JVX_SUB_QUEUE_OPERATION_STATE_READY,
		JVX_SUB_QUEUE_OPERATION_STATE_ERROR,
	
		JVX_SUB_QUEUE_OPERATION_STATE_LOCKED,
		JVX_SUB_QUEUE_OPERATION_STATE_NO_UPDATE
	};

	// =======================================================================
	
	virtual ~CjvxSubQueue_interact() {};

	virtual jvxSubQueueOperationState queue_setup_status() = 0;

	virtual jvxErrorType lock_queue(jvxApiString* newMode = nullptr) = 0;
	virtual jvxSize number_involved_components() = 0;
	virtual jvxErrorType name_component(jvxSize id, jvxApiString& astrCp) = 0;
	virtual jvxErrorType allocate_component_with_connectors(jvxSize id, IjvxHiddenInterface*& cpHI, 
		jvxApiString& astrIc, jvxApiString& astrOc, jvxComponentType& tp, 
		jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor, jvxConfigData* secReadForm) = 0;
	virtual jvxErrorType get_configuration(IjvxHiddenInterface*& cpHI,
		jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor, jvxApiString* cfgToken) = 0;
	virtual jvxErrorType release_component(IjvxHiddenInterface*& cpHI) = 0;
	virtual jvxErrorType unlock_queue(jvxSubQueueOperationState newMode) = 0;
};

/*
 *****************************************************************************
 */
class CjvxSubQueue : public CjvxBareNode1io_rearrange,
	public HjvxMicroConnection_hooks_simple,
	public HjvxMicroConnection_hooks_fwd,
	public CjvxSubQueue_pcg
{
protected:

	enum class jvxSubQueueOperationMode
	{
		JVX_SUB_QUEUE_OPERATION_MODE_FLEXIBLE,
		JVX_SUB_QUEUE_OPERATION_MODE_INTEGRATE
	};

	CjvxSubQueue_interact* ctrlSubQueue = nullptr;
	refComp<IjvxConfigProcessor> refCfgParser;
	jvxSubQueueOperationMode operationMode = jvxSubQueueOperationMode::JVX_SUB_QUEUE_OPERATION_MODE_INTEGRATE;

private:
	
	class oneInvolvedComponent
	{
	public:
		IjvxHiddenInterface* ptrI = nullptr;
		IjvxProperties* ptrP = nullptr;
		std::string nmI;
	};

	class oneMode
	{
	public:
		std::string modeName;
		std::list<oneInvolvedComponent> ptrHis;
	};

	public:
	HjvxMicroConnection* theMicroConnection = nullptr;	
	std::map <std::string, std::map<std::string, std::string> > theModes;
	oneMode currentMode;

public:

	JVX_CALLINGCONVENTION CjvxSubQueue(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSubQueue();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType process_start_icon(
			jvxSize pipeline_offset,
			jvxSize* idx_stage,
			jvxSize tobeAccessedByStage,
			callback_process_start_in_lock clbk,
			jvxHandle* priv_ptr) override;
	jvxErrorType process_stop_icon(
		jvxSize idx_stage,
		jvxBool operate_first_call,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock cb,
		jvxHandle* priv_ptr)override;
	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	jvxErrorType put_configuration(jvxCallManagerConfiguration* callMan,
			IjvxConfigProcessor* processor,
			jvxHandle* sectionToContainAllSubsectionsForMe,
			const char* filename,
			jvxInt32 lineno)override;	
	jvxErrorType get_configuration(jvxCallManagerConfiguration* callMan,
			IjvxConfigProcessor* processor,
			jvxHandle* sectionWhereToAddAllSubsections)override;

	// ==========================================================================================
	// Interface HjvxMicroConnection_hooks_simple
	// ==========================================================================================

	virtual jvxErrorType hook_test_negotiate(jvxLinkDataDescriptor* proposed JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType hook_test_accept(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType hook_test_update(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType hook_check_is_ready(jvxBool* is_ready, jvxApiString* astr) override;
	virtual jvxErrorType hook_forward(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// ==========================================================================================
	// Interface HjvxMicroConnection_hooks_fwd
	// ==========================================================================================

	virtual jvxErrorType hook_prepare(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType hook_postprocess(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType hook_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType hook_stop(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType hook_process_start(
		jvxSize pipeline_offset,
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr) override;
	virtual jvxErrorType hook_process_stop(
		jvxSize idx_stage,
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr) override;
	virtual jvxErrorType hook_process(
		jvxSize mt_mask, jvxSize idx_stage) override;

	// Helping function
	void clear_all_involved_references();
	void update_config_tables();

	/*

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	// ===============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	// ===============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===============================================================

	// Test function
	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	void test_set_output_parameters()override;

	// ===============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	// ===============================================================

	jvxErrorType transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	*/
}; 
