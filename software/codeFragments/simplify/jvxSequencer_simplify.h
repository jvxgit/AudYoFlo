	// ====================================================================================
	// Interface IjvxSequencer
	// ====================================================================================

		// Return the current status of the sequencer
	virtual jvxErrorType JVX_CALLINGCONVENTION state_sequencer(jvxState * stat)override
	{
		return(CjvxSequencer::_state_sequencer(stat));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION current_step_depth(jvxSize* stackDepth)override
	{
		return CjvxSequencer::_current_step_depth(stackDepth);
	};

	// Reset all sequences
	virtual jvxErrorType JVX_CALLINGCONVENTION reset_sequences()override
	{
		return(CjvxSequencer::_reset_sequences());
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION update_config_sequences()override
	{
		return(CjvxSequencer::_update_config_sequences());
	};

	// Add a new - then empty - sequence
	virtual jvxErrorType JVX_CALLINGCONVENTION add_sequence(const char* description, const char* label)override
	{
		return(CjvxSequencer::_add_sequence(description, label));
	};

	// Remove a previously added sequence
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_sequence(jvxSize handleId)override
	{
		return(CjvxSequencer::_remove_sequence(handleId));
	};

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
		const char* in_label_cond2 = "UNKNOWN",
		jvxBool break_active = false,
		jvxCBitField assoc_mode = (jvxCBitField)-1,
		jvxSize position = JVX_SIZE_UNSELECTED) override
	{
		return(CjvxSequencer::_insert_step_sequence_at(in_handleId_sequence,
			tp, in_elementType, in_targetComponentType, description,in_functionId, timeout_ms, 
			in_label_step, in_label_cond1, in_label_cond2, break_active, assoc_mode, position));
	};

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
		const char* in_label_cond2 = "UNKNOWN",
		jvxBool break_active = false,
		jvxCBitField assoc_mode = (jvxCBitField)-1,
		jvxCBitField whatToSet = -1)override
	{
		return(_edit_step_sequence(in_idx_sequence, tp, in_idx_step, in_elementType, in_targetComponentType, description, in_functionId,
			timeout_ms, in_label_step, in_label_cond1, in_label_cond2, break_active, assoc_mode, whatToSet));
	};

	

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_step_sequence(jvxSize in_idx_sequence, jvxSequencerQueueType tp, jvxSize in_idx_step)override
	{
		return(_remove_step_sequence(in_idx_sequence, tp, in_idx_step));
	};
	// Switch the order of two steps
	virtual jvxErrorType JVX_CALLINGCONVENTION switch_steps_sequence(jvxSize in_handleId_sequence, jvxSequencerQueueType tp, jvxSize idx1, jvxSize idx2)override
	{
		return(CjvxSequencer::_switch_steps_sequence(in_handleId_sequence, tp, idx1, idx2));
	};


	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_abort_process()override
	{
		return(CjvxSequencer::_trigger_abort_process());
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_abort_sequence()override
	{
		return(CjvxSequencer::_trigger_abort_sequence());
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION enable_loop_process(jvxBool enable)override
	{
		return(_enable_loop_process(enable));
	};

	// Wait for a certain process to complete
	virtual jvxErrorType JVX_CALLINGCONVENTION acknowledge_completion_process()override
	{
		return(CjvxSequencer::_acknowledge_completion_process());
	};

	// Return the current status of the sequencer
	virtual jvxErrorType JVX_CALLINGCONVENTION status_process(jvxSequencerStatus * status,jvxSize * seqId, jvxSequencerQueueType* qTp, 
	jvxSize * stepId,jvxBool* loop_enabled)override
	{
		return(CjvxSequencer::_status_process(status, seqId,qTp, stepId,loop_enabled));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION set_stepping_mode(jvxBool stepping_active, jvxBool stepping_break_active) override
	{
		return CjvxSequencer::_set_stepping_mode(stepping_active, stepping_break_active);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION get_stepping_mode(jvxBool* stepping_active, jvxBool* stepping_break_active) override
	{
		return CjvxSequencer::_get_stepping_mode(stepping_active, stepping_break_active);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION set_processing_mode(jvxCBitField mode)override
	{
		return _set_processing_mode(mode);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION get_processing_mode(jvxCBitField* mode)override
	{
		return _get_processing_mode(mode);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_step_continue() override
	{
		return CjvxSequencer::_trigger_step_continue();
	};

	// Browse registered sequences
	virtual jvxErrorType JVX_CALLINGCONVENTION number_sequences(jvxSize* num)override
	{
		return(_number_sequences(num));
	};

	// Browse registered sequences
	virtual jvxErrorType JVX_CALLINGCONVENTION description_sequence(jvxSize idx, jvxApiString* description, jvxApiString* label, jvxSize* numStepsRun, 
	jvxSize* numStepsLeave, jvxBool* markedForProcess, jvxBool* isDefault)override
	{
		return(_description_sequence(idx, description, label, numStepsRun, numStepsLeave, markedForProcess, isDefault));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION set_description_sequence(jvxSize idx, const char* descr, const char* label)override
	{
		return _set_description_sequence(idx, descr, label);
	}
	
	// Browse registered sequences
	virtual jvxErrorType JVX_CALLINGCONVENTION description_step_sequence(jvxSize idx_seq, jvxSize idx_step, jvxSequencerQueueType tp, jvxApiString* description,
		jvxSequencerElementType* out_elementType, jvxComponentIdentification* out_targetComponentType,
		jvxSize* out_functionId, jvxInt64* out_timeout_ms, jvxApiString* label_name, jvxApiString* cond_label_true, 
		jvxApiString* cond_label_false, jvxBool* break_active, jvxCBitField* assoc_mode)override
	{
		return(_description_step_sequence(idx_seq, idx_step, tp, description, out_elementType, out_targetComponentType, out_functionId, out_timeout_ms,
			label_name, cond_label_true, cond_label_false, break_active, assoc_mode));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION mark_sequence_process(jvxSize in_idxsequence, jvxBool runit)override
	{
		return(_mark_sequence_process(in_idxsequence, runit));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION set_sequence_default(jvxSize in_idxsequence) override
	{
		return(CjvxSequencer::_set_sequence_default(in_idxsequence));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION switch_sequences(jvxSize idx1, jvxSize idx2)override
	{
		return(_switch_sequences(idx1, idx2));
	};

	// Run a process which is composed of a list of sequences
	virtual jvxErrorType JVX_CALLINGCONVENTION run_process(IjvxSequencer_report* report, 
		jvxUInt64 eventMask, jvxSize granularity_state)override
	{
		return(_run_process(report, reinterpret_cast<jvxHandle*>(this), eventMask, -1, granularity_state));
	};

	//! External trigger of sequencer if externally triggered
	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_step_process_extern(jvxInt64 timestamp_us, jvxSequencerElementType* sequencer_element_type_on_leave)override
	{
		return(_trigger_step_process_extern(timestamp_us, sequencer_element_type_on_leave));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_complete_process_extern(jvxInt64 timestamp_us)override
	{
		return(_trigger_complete_process_extern(timestamp_us));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION read_step_type_process_extern(jvxSequencerElementType* theElementType)override
	{
		return(_read_step_type_process_extern(theElementType));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION add_sequencer_task(IjvxSequencerTask* theTask, jvxSize* uIdTask) override
	{
		return _add_sequencer_task(theTask, uIdTask);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_sequencer_task(jvxSize uIdTask) override
	{
		return _remove_sequencer_task(uIdTask);
	};
