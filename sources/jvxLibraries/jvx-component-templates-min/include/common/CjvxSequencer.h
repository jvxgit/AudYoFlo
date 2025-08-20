#ifndef _CJVXSEQUENCER_H__
#define _CJVXSEQUENCER_H__

#include "jvx-helpers.h"

#include "typedefs/TjvxSequencer_cpp.h"

#define JVX_CONFIG_SEQUENCER_SEQ_SECTION "sequencer_section"
#define JVX_CONFIG_SEQUENCER_SEQ_SUBSECTION_PREFIX  "sequencer_subsection_"
#define JVX_CONFIG_SEQUENCER_SEQ_STATUS "sequencer_status"
#define JVX_CONFIG_SEQUENCER_SEQ_NAMES "sequencer_sequence_names"
#define JVX_CONFIG_SEQUENCER_SEQ_LABEL "sequencer_sequence_label"
#define JVX_CONFIG_SEQUENCER_SEQ_MARKED_FLAG "sequencer_marked_for_process"

#define JVX_CONFIG_ONE_SEQUENCE_STEPS_PREFIX "sequencer_sequence_steps_"
#define JVX_CONFIG_ONE_SEQU_SEQ_STATUS_PREFIX "sequencer_status_"

#define JVX_CONFIG_ONE_SEQUENCE_STEP_DECSCS "sequencer_sequence_step_descriptions"
#define JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL "sequencer_sequence_step_label"
#define JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_TRUE "sequencer_sequence_step_label_true"
#define JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_FALSE "sequencer_sequence_step_label_false"
#define JVX_CONFIG_ONE_SEQUENCE_STEP_TYPES "sequencer_sequence_step_types"
#define JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENTS "sequencer_sequence_step_components"
#define JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_SLOTIDS "sequencer_sequence_step_component_slotids"
#define JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_SLOTSUBIDS "sequencer_sequence_step_component_slotsubids"
#define JVX_CONFIG_ONE_SEQUENCE_STEP_COMMAND_IDS "sequencer_sequence_step_command_ids"
#define JVX_CONFIG_ONE_SEQUENCE_STEP_TIMEOUTS "sequencer_sequence_step_timeouts"
#define JVX_CONFIG_ONE_SEQUENCE_STEP_BREAK_ACTION "sequencer_sequence_step_break_actions"
#define JVX_CONFIG_ONE_SEQUENCE_STEP_ASSOC_MODE "sequencer_sequence_step_assoc_mode"

// Some additional entries for version 2.0
#define JVX_CONFIG_SEQUENCER_SEQ_VERSION "sequencer_version"
#define JVX_CONFIG_SEQUENCER_SEQ_VERSION_2_0 "sequencer_version_2.0"
#define JVX_CONFIG_SEQUENCER_SEQ_DEFAULT "sequencer_is_default"
#define JVX_CONFIG_SEQUENCER_SEQ_ENGAGED "sequencer_is_engaged"
#define JVX_CONFIG_ONE_SEQUENCE_ONE_STEP_PREFIX "sequencer_step_"
#define JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_IDENTIFICATION "sequencer_component_identification"

#define JVX_HBDX_CHECK_FREEZE_SEQUENCER_OPERATION \
	if(this->_common_set_sequencer.theState >= JVX_STATE_PROCESSING) \
	{ \
		return(JVX_ERROR_SEQUENCER_TOOK_CONTROL); \
	}

// #define JVX_SEQUENCER_WITH_LOCK
struct currentStatusSeqContext
{
	jvxSize idxSequence;
	jvxSize idxStep;
	jvxBool loop_enabled;
	IjvxHost* hostRef;
	jvxUInt64 eventMask;
	jvxSequencerStatus theSeqState;
	jvxSequencerQueueType currentQueueTp;
	jvxInt64 timestamp_store;
	//bool externalTrigger;
	jvxBool isFullShutdown;
	jvxBool isErrorShutdown;
	jvxSize in_processing_state_on_enter;
	struct
	{
		jvxSequencerElementType step_tp;
	} thisStep;
	jvxSequencerEvent howtosaygoodbye;
	IjvxSequencerTask* sequencer_task = nullptr;

	currentStatusSeqContext()
	{
		idxSequence = JVX_SIZE_UNSELECTED;
		idxStep = JVX_SIZE_UNSELECTED;
		loop_enabled = false;
		hostRef = NULL;
		eventMask = 0;
		theSeqState = JVX_SEQUENCER_STATUS_NONE;
		currentQueueTp = JVX_SEQUENCER_QUEUE_TYPE_NONE;
		timestamp_store = JVX_SIZE_UNSELECTED;
		isFullShutdown = false;
		isErrorShutdown = false;
	}
};

// ===================================================================
// ===================================================================
/*
How it all ends:

1) trigger_abort_process

2) trigger_step_process_extern -> returns JVX_ERROR_PROCESS_COMPLETE or JVX_ERROR_ABORT

3) -> trigger_complete_process_extern -> set state to JVX_STATE_COMPLETE

4) wait_completion_process -> resets state of sequencer

*/
class CjvxSequencer
{
protected:
	typedef struct
	{
		std::string descr;
		jvxBool isDefault;
		jvxBool isEngaged;
	} oneEntryConfigSequences;

	class oneTaskStep
	{
	public:
		IjvxSequencerTask* task = nullptr;
		jvxSize stack_height_run = 0;
	};

    struct
	{
        setOfSequences theSequences;
		jvxState theState;
		jvxSize uniqueId;

		std::list<currentStatusSeqContext> operationStack;
		currentStatusSeqContext* inOperation;

		jvxInt64 default_step_timeout_on_leave_ms;
		jvxBool loop_enable;

		IjvxSequencer_report* report;
		jvxSize in_processing_state;

		jvxSize granularity_report_state;
		jvxSize granularity_report_state_cnt;

		jvxBool stepping_active;
		jvxBool stepping_break_active;
		jvxSequencerStepState step_state;
		jvxSize intercept_count; 

		jvxSize version_cfg_major;
		jvxSize version_cfg_minor;

		jvxCBitField run_mode_start;
		jvxCBitField run_mode_operate;
		jvxCBitField run_mode_save;

		jvxBool in_processing;

		std::map<jvxSize, oneTaskStep> immediate_tasks;
	} _common_set_sequencer;

	std::vector<oneEntryConfigSequences> lst_config_seqs;

public:

	CjvxSequencer();

	~CjvxSequencer();

	virtual jvxErrorType _init_sequences();

	virtual jvxErrorType _state_sequencer(jvxState * stat);

	virtual jvxErrorType _current_step_depth(jvxSize* stackDepth);

	virtual jvxErrorType _terminate_sequences();

	virtual jvxErrorType _reset_sequences();

	virtual jvxErrorType _update_config_sequences();

	virtual jvxErrorType _set_stepping_mode(jvxBool stepping_active, jvxBool stepping_break_active);

	virtual jvxErrorType _get_stepping_mode(jvxBool* stepping_active, jvxBool* stepping_break_active);

	virtual jvxErrorType _set_processing_mode(jvxCBitField mode);

	virtual jvxErrorType _get_processing_mode(jvxCBitField * mode);

	virtual jvxErrorType _trigger_step_continue();

	// Add a new - then empty - sequence
	virtual jvxErrorType _add_sequence(const char* description, const char* label);

	// Add a new - then empty - sequence
	virtual jvxErrorType _find_sequence_id_name(const char* description, jvxSize* id_out);

	// Remove a previously added sequence
	virtual jvxErrorType _remove_sequence(jvxSize idx);

	// Add a step element which is run
	virtual jvxErrorType _insert_step_sequence_at(
		jvxSize idx,
		jvxSequencerQueueType queueType,
		 jvxSequencerElementType in_elementType,
		jvxComponentIdentification in_targetComponentType,
		const char* description = "No description",
		jvxSize in_functionId = JVX_SIZE_UNSELECTED,
		jvxInt64 timeout_ms = -1,
		const char* in_label_step = "UNKNOWN",
		const char* in_label_cond_true = "UNKNOWN",
		const char* in_label_cond_false = "UNKNOWN",
		jvxBool break_active = false,
		jvxCBitField assoc_mode = (jvxCBitField)-1,
		jvxSize posi = JVX_SIZE_UNSELECTED);

	virtual jvxErrorType _edit_step_sequence(
		jvxSize idxsequence,
		jvxSequencerQueueType tp,
		jvxSize idxstep,
		 jvxSequencerElementType in_elementType,
		jvxComponentIdentification in_targetComponentType,
		const char* description = "No description",
		jvxSize in_functionId = JVX_SIZE_UNSELECTED,
		jvxInt64 timeout_ms = -1,
		const char* in_label_step = "UNKNOWN",
		const char* in_label_cond_true = "UNKNOWN",
		const char* in_label_cond_false = "UNKNOWN",
		jvxBool break_active = false,
		jvxCBitField assoc_mode = (jvxCBitField)-1,
		jvxCBitField whatToSet = (jvxCBitField)-1);

	virtual jvxErrorType _switch_steps_sequence(jvxSize idxsequence, jvxSequencerQueueType tp, jvxSize idx1, jvxSize idx2);

	virtual jvxErrorType _remove_step_sequence(jvxSize idxsequence, jvxSequencerQueueType tp, jvxSize idx);

	virtual jvxErrorType _reset_sequence(jvxSize in_handleId_sequence);

	virtual jvxErrorType _mark_sequence_process(jvxSize idxsequence, jvxBool runit);

	virtual jvxErrorType _set_sequence_default(jvxSize in_idxsequence);

	// Run a process which is composed of a list of sequences
	virtual jvxErrorType _run_process(IjvxSequencer_report* report, jvxHandle* privateData, 
		jvxCBitField eventMask /*= JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS*/, jvxInt64 timestamp_us /*= JVX_SIZE_UNSELECTED*/,
		jvxSize run_granularity_report_state_wait /*= JVX_SIZE_UNSELECTED*/);


	virtual jvxErrorType _trigger_step_process_extern(jvxInt64 timestamp_us, jvxSequencerElementType* sequencer_element_type_on_leave, jvxBool forceStop);

	virtual jvxErrorType _trigger_complete_process_extern(jvxInt64 timestamp_us);

	virtual jvxErrorType _read_step_type_process_extern( jvxSequencerElementType* theElementType);

	virtual jvxErrorType _trigger_abort_process();
	virtual jvxErrorType _trigger_abort_sequence();

	// Abort a certain process
	virtual jvxErrorType _acknowledge_completion_process();

	// Return the current status of the sequencer
	virtual jvxErrorType _status_process(jvxSequencerStatus* status, jvxSize * seqId, jvxSequencerQueueType* qTp, jvxSize * stepId, jvxBool* loop_enabled);

	virtual jvxErrorType _enable_loop_process(jvxBool enable);
   // Browse registered sequences
	virtual jvxErrorType _number_sequences(jvxSize* num);

	// Browse registered sequences
	virtual jvxErrorType _description_sequence(jvxSize idx, jvxApiString* description, jvxApiString* label, jvxSize* numStepsRun, jvxSize* numStepsLeave, jvxBool* markedForProcess, jvxBool* isDefaultSeq);
 
	virtual jvxErrorType _set_description_sequence(jvxSize idx, const char* descr, const char* label);

	// Browse registered sequences
	virtual jvxErrorType _description_step_sequence(jvxSize idx_seq, jvxSize idx_steps, jvxSequencerQueueType tp, jvxApiString* description,
		jvxSequencerElementType* out_elementType, jvxComponentIdentification* out_targetComponentType,
		jvxSize* out_functionId, jvxInt64* out_timeout_ms, jvxApiString* label_name, 
		jvxApiString* cond_label_true, jvxApiString* cond_label_false, jvxBool* break_action, jvxCBitField* assoc_mode);

	virtual jvxErrorType _switch_sequences(jvxSize idx1, jvxSize idx2);

	virtual jvxErrorType _add_sequencer_task(IjvxSequencerTask* theTask, jvxSize* uIdTask);
	virtual jvxErrorType _remove_sequencer_task(jvxSize uIdTask);
	
	// ==========================================================================================
	// Put/get configuration
	// ==========================================================================================
	virtual jvxErrorType _put_configuration_seq(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllEntries, 
		const char* filename = "", jvxInt32 lineno = -1);

	virtual jvxErrorType _get_configuration_seq(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections, IjvxObject* me_object);

	jvxErrorType put_configuration_seq_v1_0(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
			jvxHandle* sectionAll, 
			const char* filename, jvxInt32 lineno);
	jvxErrorType put_configuration_seq_v2_0(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionAll, 
		const char* filename, jvxInt32 lineno);

	jvxErrorType get_configuration_seq_v2_0(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections, 
		IjvxObject* me_object);
	jvxErrorType get_configuration_seq_v1_0(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections, 
		IjvxObject* me_object);

	// ==========================================================================================
	// Helpers
	// ==========================================================================================

	void addOneStepSequencerConfig(IjvxConfigProcessor* processor, jvxConfigData* oneStep, 
		const std::string& nameSeq, jvxSize idSeq, jvxSequencerQueueType qTp);

	std::vector<oneSequence>::iterator findSequenceNameInList(std::vector<oneSequence>& all_sequences, std::string description);

	std::vector<oneSequence>::iterator findSequenceIdInList(std::vector<oneSequence>& all_sequences, jvxSize idx);
	
	std::vector<oneSequenceStep>::iterator findSequenceStepIdInList(std::vector<oneSequenceStep>& all_steps, jvxSize idx);

	bool findNextValidStep(jvxSize offset);
	bool findNextSequenceCall(const std::string& label);
	bool findNextTask(jvxSequencerQueueType tp, jvxSize id_next_task);
	bool findNextSequenceStack();
	void addNextTask_core(std::map<jvxSize, oneTaskStep>::iterator& elm);


	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_started(jvxInt64 timestamp_us) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_step(jvxInt64 timestamp_us, jvxSize* delta_ms) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_stopped(jvxInt64 timestamp_us) = 0;
};

#endif
