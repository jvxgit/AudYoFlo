#include "jvx-helpers.h"

CjvxSequencerTask::CjvxSequencerTask()
{
}

void
CjvxSequencerTask::reset(const char* taskname, IjvxSequencerTask_report* repArg, jvxHandle* privArg)
{
	description.clear();
	if (taskname)
	{
		description = taskname;
	}

	priv = privArg;
	report = repArg;

	this->restart();

	run_queue.clear();
	leave_queue.clear();
}

jvxErrorType 
CjvxSequencerTask::description_task(jvxApiString* astr)
{
	if (astr)
	{
		astr->assign(description);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSequencerTask::number_steps(jvxSize& num, jvxSequencerQueueType tp)
{
	num = 0;
	switch (tp)
	{
	case JVX_SEQUENCER_QUEUE_TYPE_RUN:
		num = run_queue.size();
		break;
	case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
		num = leave_queue.size();
		break;
	}
	return JVX_NO_ERROR;
}
	
jvxErrorType 
CjvxSequencerTask::description_step(
	jvxSize idx_steps,
	jvxSequencerQueueType tp,
	jvxSequencerOneStepReturn* oneSeqStep)
{
	jvxSize num = 0;
	std::list<oneStepSequencer>::iterator elm;
	
	switch (tp)
	{
	case JVX_SEQUENCER_QUEUE_TYPE_RUN:
		num = run_queue.size();
		elm = run_queue.begin();
		break;
	case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
		num = leave_queue.size();
		elm = run_queue.begin();
		break;
	}
	if (idx_steps < num)
	{
		std::advance(elm, idx_steps);
		copy_one_step(oneSeqStep, *elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}
	
void 
CjvxSequencerTask::copy_one_step(
	jvxSequencerOneStepReturn* oneSeqStep,
	oneStepSequencer& elm)
{
	if (oneSeqStep->out_description)
	{
		oneSeqStep->out_description->assign(elm.description);
	}
	if (oneSeqStep->out_cond_label_false)
	{
		oneSeqStep->out_cond_label_false->assign(elm.label_cond_false);
	}
	if (oneSeqStep->out_cond_label_true)
	{
		oneSeqStep->out_cond_label_true->assign(elm.label_cond_true);
	}
	if (oneSeqStep->out_label_name)
	{
		oneSeqStep->out_label_name->assign(elm.label_step);
	}
	if (oneSeqStep->out_elementType)
	{
		*oneSeqStep->out_elementType = elm.element_type;
	}
	if (oneSeqStep->out_functionId)
	{
		*oneSeqStep->out_functionId = elm.function_id;
	}
	if (oneSeqStep->out_targetComponentType)
	{
		*oneSeqStep->out_targetComponentType = elm.component_id;
	}
	if (oneSeqStep->out_timeout_ms)
	{
		*oneSeqStep->out_timeout_ms = elm.timeout_ms;
	}
	if (oneSeqStep->out_break_condition)
	{
		*oneSeqStep->out_break_condition = elm.break_condition;
	}
	if (oneSeqStep->out_assoc_mode)
	{
		*oneSeqStep->out_assoc_mode = elm.assoc_mode;
	}
}

jvxErrorType 
CjvxSequencerTask::next_step(
	jvxSequencerQueueType tp,
	jvxSequencerOneStepReturn* oneSeqStep)
{
	jvxSize num = 0;
	std::list<oneStepSequencer>::iterator elm;
	jvxSize idx_steps = 0;
	
	if (tp > queue_tp)
	{
		queue_tp = tp;
	}
	switch (my_status)
	{
	case jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_WAITING:
		switch (queue_tp)
		{
		case JVX_SEQUENCER_QUEUE_TYPE_RUN:

			my_status = jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_RUN_ACTIVATED;
			run_current_position = JVX_SIZE_UNSELECTED;

			if (report)
			{
				report->report_start(tp, priv);
			}

			if (run_queue.size() == 0)
			{
				if (report)
				{
					report->report_complete(tp, priv);
				}
				my_status = jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_RUN_COMPLETE;
				return JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				run_current_position = 0;
				auto elm = run_queue.begin();
				std::advance(elm, run_current_position);
				copy_one_step(oneSeqStep, *elm);

				current_step_idx = run_current_position;
				current_queue_type = queue_tp;

				run_current_position++;
			}
			break;

		case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
			return JVX_ERROR_WRONG_STATE;
			break;
		}
		break;
	case jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_RUN_ACTIVATED:
		switch (queue_tp)
		{
		case JVX_SEQUENCER_QUEUE_TYPE_RUN:
			if (run_current_position < run_queue.size())
			{
				auto elm = run_queue.begin();
				std::advance(elm, run_current_position);
				copy_one_step(oneSeqStep, *elm);

				current_step_idx = run_current_position;
				current_queue_type = queue_tp;

				run_current_position++;
			}
			else
			{
				if (report)
				{
					report->report_complete(tp, priv);
				}
				my_status = jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_RUN_COMPLETE;
				return JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			break;
		case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
			return JVX_ERROR_WRONG_STATE;
			break;
		}
		break;
	case jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_RUN_COMPLETE:
		switch (queue_tp)
		{
		case JVX_SEQUENCER_QUEUE_TYPE_RUN:
			return JVX_ERROR_WRONG_STATE;
			break;
		case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
			my_status = jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_LEAVE_ACTIVATED;
			run_current_position = JVX_SIZE_UNSELECTED;

			if (report)
			{
				report->report_start(tp, priv);
			}

			if (leave_queue.size() == 0)
			{
				if (report)
				{
					report->report_complete(tp, priv);
				}
				my_status = jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_LEAVE_COMPLETE;
				return JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			else
			{
				leave_current_position = 0;
				auto elm = leave_queue.begin();
				std::advance(elm, leave_current_position);
				copy_one_step(oneSeqStep, *elm);

				current_step_idx = run_current_position;
				current_queue_type = queue_tp;

				leave_current_position++;
			}
			break;
		}
		break;
	case jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_LEAVE_ACTIVATED:
		switch (queue_tp)
		{
		case JVX_SEQUENCER_QUEUE_TYPE_RUN:
			return JVX_ERROR_WRONG_STATE;
			break;
		case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
			if (leave_current_position < leave_queue.size())
			{
				auto elm = leave_queue.begin();
				std::advance(elm, leave_current_position);
				copy_one_step(oneSeqStep, *elm);

				current_step_idx = run_current_position;
				current_queue_type = queue_tp;

				leave_current_position++;
			}
			else
			{
				if (report)
				{
					report->report_complete(tp, priv);
				}
				my_status = jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_LEAVE_COMPLETE;
				return JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			break;
		}
		break;
	case jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_LEAVE_COMPLETE:
		switch (queue_tp)
		{
		case JVX_SEQUENCER_QUEUE_TYPE_RUN:
			return JVX_ERROR_WRONG_STATE;
			break;
		case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
			return JVX_ERROR_WRONG_STATE;
			break;
		}
		break;
	}
	return JVX_NO_ERROR;
}
	
jvxErrorType 
CjvxSequencerTask::status(jvxSequencerTaskStatus* retStatus)
{
	if (retStatus)
	{
		retStatus->stat = my_status;
		retStatus->qTp = queue_tp;
		retStatus->is_active = is_active;
	}
	return JVX_NO_ERROR;
}
	
jvxErrorType 
CjvxSequencerTask::switch_queue(jvxSequencerQueueType new_tp)
{
	queue_tp = new_tp;
	if (queue_tp == JVX_SEQUENCER_QUEUE_TYPE_LEAVE)
	{
		if (my_status == jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_RUN_ACTIVATED)
		{
			// On eror, cancel following tasks
			my_status = jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_RUN_COMPLETE;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSequencerTask::restart()
{
	run_current_position = 0;
	leave_current_position = 0;
	my_status = jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_WAITING;
	queue_tp = JVX_SEQUENCER_QUEUE_TYPE_RUN;
	
	current_step_idx = JVX_SIZE_UNSELECTED;
	current_queue_type = JVX_SEQUENCER_QUEUE_TYPE_NONE;

	is_active = false;
	is_complete = false;
	flag_immediate_complete = false;

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSequencerTask::set_activate(jvxBool isActive)
{
	assert(is_active != isActive);
	is_active = isActive;
	if (
		(!is_active) &&
		(my_status == jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_LEAVE_COMPLETE))
	{
		is_complete = true;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSequencerTask::immediate_complete(jvxBool* immediateComplete)
{
	if (immediateComplete)
	{
		*immediateComplete = flag_immediate_complete;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSequencerTask::report_error(jvxErrorType err, const char* errDescr)
{
	if (report)
	{
		report->report_error(priv, current_queue_type, current_step_idx, err, errDescr);
	}
	return JVX_NO_ERROR;
}
jvxErrorType 
CjvxSequencerTask::priv_data(jvxHandle** privPtr)
{
	if (privPtr)
	{
		*privPtr = priv;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSequencerTask::add_step(jvxSequencerQueueType tp,
	oneStepSequencer oneStep)
{
	switch (tp)
	{
	case JVX_SEQUENCER_QUEUE_TYPE_RUN:
		run_queue.push_back(oneStep);
		break;
	case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
		leave_queue.push_back(oneStep);
		break;
	}
	return JVX_NO_ERROR;
}
