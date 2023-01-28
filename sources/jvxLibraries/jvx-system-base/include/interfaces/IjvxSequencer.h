#ifndef __IJVXSEQUENCER_H__
#define __IJVXSEQUENCER_H__

// ================= On the fly ==========================

JVX_INTERFACE IjvxSequencerTask_report
{
public:
	virtual ~IjvxSequencerTask_report() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION report_start(jvxSequencerQueueType qtp, jvxHandle* priv);
	virtual jvxErrorType JVX_CALLINGCONVENTION report_complete(jvxSequencerQueueType qtp, jvxHandle* priv);
	virtual jvxErrorType JVX_CALLINGCONVENTION report_error(jvxHandle* priv, jvxSequencerQueueType qtp, jvxSize stepIdx, jvxErrorType errId, const char* error_description);
};

JVX_INTERFACE IjvxSequencerTask
{
public:
	virtual ~IjvxSequencerTask() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION description_task(jvxApiString* astr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_steps(jvxSize& num, jvxSequencerQueueType tp) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION description_step(
		jvxSize idx_steps, 
		jvxSequencerQueueType tp,
		jvxSequencerOneStepReturn* oneSeqStep) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION next_step(
		jvxSequencerQueueType tp,
		jvxSequencerOneStepReturn* oneSeqStep) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION status(jvxSequencerTaskStatus* stat) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION switch_queue(jvxSequencerQueueType new_tp) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION restart() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION set_activate(jvxBool isActive) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION immediate_complete(jvxBool* immediateComplete) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_error(jvxErrorType res, const char* errDescr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION priv_data(jvxHandle** priv) = 0;
};

// =========================================================================

JVX_INTERFACE IjvxSequencer_report
{
 public:
        virtual JVX_CALLINGCONVENTION ~IjvxSequencer_report(){};

	// Report the status of the sequencer if desired
	virtual jvxErrorType JVX_CALLINGCONVENTION report_event(jvxCBitField event_mask, const char* description, jvxSize sequenceId, jvxSize stepId, 
		jvxSequencerQueueType tp, jvxSequencerElementType stp, jvxSize fId, jvxSize current_state, jvxBool indicateFirstError = false) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_callback(jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSize functionId) = 0; 
};

// ==============================================================================

JVX_INTERFACE IjvxSequencer
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxSequencer(){};

	// ============================================================================
	// Edit sequences
	// ============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION state_sequencer(jvxState* theState) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION current_step_depth(jvxSize* stackDepth) = 0;

	// Reset all sequences
	virtual jvxErrorType JVX_CALLINGCONVENTION reset_sequences() = 0;

	// Update the status of the sequences as stored in configuration before (since default sequences are added later
	virtual jvxErrorType JVX_CALLINGCONVENTION update_config_sequences() = 0;

	// Add a new - then empty - sequence
	virtual jvxErrorType JVX_CALLINGCONVENTION add_sequence(const char* description, const char* label) = 0;

	// Remove a previously added sequence
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_sequence(jvxSize handleId) = 0;

	// Switch the order of two sequences
	virtual jvxErrorType JVX_CALLINGCONVENTION switch_sequences(jvxSize idx1, jvxSize idx2) = 0;

	// Mark a sequence to be involved in the overall process
	virtual jvxErrorType JVX_CALLINGCONVENTION mark_sequence_process(jvxSize in_idxsequence, jvxBool runit) = 0;

	//! Make this sequence the default sequence. Default sequences are not stored to config files
	virtual jvxErrorType JVX_CALLINGCONVENTION set_sequence_default(jvxSize in_idxsequence) = 0;

	// Browse registered sequences
	virtual jvxErrorType JVX_CALLINGCONVENTION number_sequences(jvxSize* num) = 0;

	// Browse registered sequences
	virtual jvxErrorType JVX_CALLINGCONVENTION description_sequence(jvxSize idx, jvxApiString* description, jvxApiString* label, jvxSize* numStepsRun,
		jvxSize* numStepsLeave, jvxBool*, jvxBool* isDefault = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_description_sequence(jvxSize idx, const char* descr, const char* label) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_stepping_mode(jvxBool stepping_active, jvxBool stepping_break_active) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_stepping_mode(jvxBool* stepping_active, jvxBool* stepping_break_active) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_processing_mode(jvxCBitField mode) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_processing_mode(jvxCBitField * mode) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_step_continue() = 0;

	// ============================================================================
	// Edit steps
	// ============================================================================

	// Add a step element which is run
	virtual jvxErrorType JVX_CALLINGCONVENTION insert_step_sequence_at(
		jvxSize in_handleId_sequence,
		jvxSequencerQueueType tp,
		jvxSequencerElementType in_elementType,
		jvxComponentIdentification in_targetComponentType,
		const char* description = "No description",
		jvxSize in_functionId = JVX_SIZE_UNSELECTED,
		jvxInt64 timeout_ms = -1,
		const char* in_label_step = "UNKNOWN",
		const char* in_label_cond1 = "UNKNOWN",
		const char* in_label_cond2 = "UNKNWON",
		jvxBool break_active = false,
		jvxCBitField assoc_mode = (jvxCBitField)-1,
		jvxSize position = JVX_SIZE_UNSELECTED) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION edit_step_sequence(
		jvxSize in_idx_sequence,
		jvxSequencerQueueType tp,
		jvxSize in_idx_step,
		jvxSequencerElementType in_elementType,
		jvxComponentIdentification in_targetComponentType,
		const char* description = "No description",
		jvxSize in_functionId = JVX_SIZE_UNSELECTED,
		jvxInt64 timeout_ms = -1,
		const char* in_label_step = "UNKNOWN",
		const char* in_label_cond1 = "UNKNOWN",
		const char* in_label_cond2 = "UNKNWON",
		jvxBool break_active = false,
		jvxCBitField assoc_mode = (jvxCBitField)-1,
		jvxCBitField whatToSet = (jvxCBitField)-1) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_step_sequence(jvxSize in_idx_sequence, jvxSequencerQueueType tp,
		jvxSize position) = 0;

	// Switch the order of two steps
	virtual jvxErrorType JVX_CALLINGCONVENTION switch_steps_sequence(jvxSize in_idxsequence, jvxSequencerQueueType tp,
		jvxSize idx1, jvxSize idx2) = 0;

	// Browse registered sequences
	virtual jvxErrorType JVX_CALLINGCONVENTION description_step_sequence(jvxSize idx_seq, 
		jvxSize idx_steps, 
		jvxSequencerQueueType tp,
		jvxApiString* description,
		jvxSequencerElementType* out_elementType, jvxComponentIdentification* out_targetComponentType,
		jvxSize* out_functionId, jvxInt64* out_timeout_ms,  jvxApiString* label_name, jvxApiString* cond_label_true, 
		jvxApiString* cond_label_false, jvxBool* break_active, jvxCBitField* assoc_mode) = 0;

	// ============================================================================
	// Control process
	// ============================================================================

	// Run a process which is composed of a list of sequences
	virtual jvxErrorType JVX_CALLINGCONVENTION run_process(
		IjvxSequencer_report* report, 
		jvxCBitField eventMask, 
		jvxSize granularity_report_state) = 0;

	// Initiate a termination of the current process. This will not wait until process is over
	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_abort_process() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_abort_sequence() = 0;

	// Wait for a process to complete
	virtual jvxErrorType JVX_CALLINGCONVENTION acknowledge_completion_process() = 0;

	// Enable looping of currently setup process
	virtual jvxErrorType JVX_CALLINGCONVENTION enable_loop_process(jvxBool enable) = 0;

	// Return the current status of the sequencer
	virtual jvxErrorType JVX_CALLINGCONVENTION status_process(jvxSequencerStatus* status, jvxSize* idxSequence, jvxSequencerQueueType* qTp, jvxSize* idxStep, jvxBool* loopEnabled) = 0;

	// Functions specific to external trigger mode

	// If externally triggered, procede with next step
	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_step_process_extern(jvxInt64 timestamp_us, jvxSequencerElementType* sequencer_element_type_on_leave = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_complete_process_extern(jvxInt64 timestamp_us) = 0;

	// If externally triggered, find out where state machine is stuck
	virtual jvxErrorType JVX_CALLINGCONVENTION read_step_type_process_extern(jvxSequencerElementType* theElementType) = 0;

	// =================================================================================
	// On the fly processing
	// =================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION add_sequencer_task(IjvxSequencerTask* theTask, jvxSize* uIdTask) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_sequencer_task(jvxSize uIdTask) = 0;
};

#endif
