#ifndef __HJVXSEQUENCER_H__
#define __HJVXSEQUENCER_H__

#include "jvx.h"

class CjvxSequencerTask: public IjvxSequencerTask
{
public:
	class oneStepSequencer
	{
	public:
		jvxSequencerElementType element_type = JVX_SEQUENCER_TYPE_COMMAND_INTERCEPT;
		jvxComponentIdentification component_id;
		std::string description = "No description";
		jvxSize function_id = JVX_SIZE_UNSELECTED;
		jvxInt64 timeout_ms = -1;
		std::string label_step = "UNKNOWN";
		std::string label_cond_true = "UNKNOWN";
		std::string label_cond_false = "UNKNOWN";
		jvxBool break_condition = false;
		jvxCBitField assoc_mode = (jvxCBitField)-1;
		jvxBool allow_fail = false;
	};

	std::string description;
	jvxHandle* priv = nullptr;

	std::list<oneStepSequencer> run_queue;
	jvxSize run_current_position = 0;

	std::list<oneStepSequencer> leave_queue;
	jvxSize leave_current_position = 0;

	jvxSize current_step_idx = JVX_SIZE_UNSELECTED;
	jvxSequencerQueueType current_queue_type = JVX_SEQUENCER_QUEUE_TYPE_NONE;

	jvxSequencerTaskState my_status = jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_WAITING;
	jvxSequencerQueueType queue_tp = JVX_SEQUENCER_QUEUE_TYPE_NONE;

	IjvxSequencerTask_report* report = nullptr;

	jvxBool is_active = false;
	jvxBool is_complete = false;
	jvxBool flag_immediate_complete = false;

public:
	CjvxSequencerTask();
	void reset(const char* taskname = nullptr, IjvxSequencerTask_report* repArg = nullptr, jvxHandle* privArg = NULL);

	virtual jvxErrorType JVX_CALLINGCONVENTION description_task(jvxApiString* astr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_steps(jvxSize& num, jvxSequencerQueueType tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION description_step(
		jvxSize idx_steps, 
		jvxSequencerQueueType tp,
		jvxSequencerOneStepReturn* oneSeqStep)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION next_step(
		jvxSequencerQueueType tp,
		jvxSequencerOneStepReturn* oneSeqStep) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION status(jvxSequencerTaskStatus* stat) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION switch_queue(jvxSequencerQueueType new_tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION restart() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION set_activate(jvxBool isActive) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION immediate_complete(jvxBool* immediateComplete) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_error(jvxErrorType res, const char* errDescr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION priv_data(jvxHandle** priv) override;

	jvxErrorType add_step(jvxSequencerQueueType tp,
		oneStepSequencer oneStep);
	void copy_one_step(
		jvxSequencerOneStepReturn* oneSeqStep,
		oneStepSequencer& elm);
};

#endif

