#ifndef __TJVXSEQUENCER_CPP_H__
#define __TJVXSEQUENCER_CPP_H__

// One sequence element, can be wait or command
class oneSequenceStep
{
public:
	// What kind of step do we see?
	 jvxSequencerElementType step_tp;

	// Which component does the step address?
	jvxComponentIdentification step_target;

	// Timeout to remain in this sequencer step
	jvxInt64 step_timeout_ms;

	// Address this step in jump labels using this step_id
	jvxSize step_function_id;

	// Description of this step
	std::string description;

	// Give this step a label to become a jump target
	std::string step_label;

	// In case of a conditional jump, target this label for "true"
	std::string step_cond_label_true;

	// In case of a conditional jump, target this label for "false"
	std::string step_cond_label_false;

	jvxBool break_active;

	jvxCBitField assoc_mode;

	jvxBool allow_fail = false;

} ;

// One sequence consisting of the commands, a unique process id and a timer for timeouts
typedef struct
{
	// List of steps to perform within the sequencer thread
	std::vector<oneSequenceStep> lstOfRunningSteps;

	// List of steps to perform when leaving (still within thread)
	std::vector<oneSequenceStep> lstOfLeaveSteps;

	// Iterator to iterate along the steps when running
	std::vector<oneSequenceStep>::iterator elmActive;

	// Current state of this sequence
	jvxProcessState currentState;

	// Timer to detecdt timeouts
	jvxInt64 timer_since_last_increment;

	// Description of this sequence
	std::string description;

	std::string label;

	jvxBool isDefaultSeq;

	jvxSize stepIdCnt;

	// Mark this sequence to be executed when processing
	jvxBool markedForProcess;

} oneSequence;

// All sequences which may be available stored somewhere
typedef struct
{
	std::vector<oneSequence> all_sequences;
} setOfSequences;

#endif
