#include "common/CjvxSequencer.h"

CjvxSequencer::CjvxSequencer()
{

	_common_set_sequencer.theState = JVX_STATE_INIT;
	_common_set_sequencer.uniqueId = 1;
	_common_set_sequencer.default_step_timeout_on_leave_ms = 5000;
	_common_set_sequencer.in_processing_state = JVX_SIZE_UNSELECTED;
	_common_set_sequencer.stepping_active = false;
	_common_set_sequencer.stepping_break_active = false;
	_common_set_sequencer.step_state = JVX_SEQUENCER_STEP_STATE_NONE;
	_common_set_sequencer.intercept_count = JVX_SIZE_UNSELECTED;
	_common_set_sequencer.run_mode_start = 0xFFFF; // All modes
	_common_set_sequencer.version_cfg_major = 2;
	_common_set_sequencer.version_cfg_minor = 0;
	_common_set_sequencer.in_processing = false;
}

CjvxSequencer::~CjvxSequencer()
{
#ifdef JVX_SEQUENCER_WITH_LOCK
	JVX_TERMINATE_MUTEX(_common_set_sequencer.theAccess);
#endif
}

jvxErrorType 
CjvxSequencer::_set_stepping_mode(jvxBool stepping_active, jvxBool stepping_break_active)
{
	_common_set_sequencer.stepping_break_active = false;
	_common_set_sequencer.stepping_active = stepping_active;
	if (!_common_set_sequencer.stepping_active)
	{
		_common_set_sequencer.stepping_break_active = stepping_break_active;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSequencer::_get_stepping_mode(jvxBool* stepping_active, jvxBool* stepping_break_active)
{
	if (stepping_active)
	{
		*stepping_active = _common_set_sequencer.stepping_active;
	}

	if (stepping_break_active)
	{
		*stepping_break_active = _common_set_sequencer.stepping_break_active;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSequencer::_set_processing_mode(jvxCBitField mode)
{
	_common_set_sequencer.run_mode_start = mode;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSequencer::_get_processing_mode(jvxCBitField * mode)
{
	if (mode)
		*mode = _common_set_sequencer.run_mode_start;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSequencer::_trigger_step_continue()
{
	if (_common_set_sequencer.step_state == JVX_SEQUENCER_STEP_STATE_ENTERED)
	{
		_common_set_sequencer.step_state = JVX_SEQUENCER_STEP_STATE_RELEASED;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSequencer::_init_sequences()
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_sequencer.theState == JVX_STATE_INIT)
	{
		_common_set_sequencer.theState = JVX_STATE_ACTIVE;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	return(res);
}

jvxErrorType
CjvxSequencer::_state_sequencer(jvxState * stat)
{
	if (stat)
	{
		*stat = _common_set_sequencer.theState;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxSequencer::_current_step_depth(jvxSize* stackDepth)
{
	if (stackDepth)
	{
		*stackDepth = _common_set_sequencer.operationStack.size();
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxSequencer::_terminate_sequences()
{
	jvxBool stopThread = false;
	jvxErrorType res = JVX_NO_ERROR;

	this->_trigger_abort_process();
	this->_acknowledge_completion_process();
	this->_reset_sequences();

	_common_set_sequencer.theState = JVX_STATE_INIT;

	return(res);
}

jvxErrorType
CjvxSequencer::_reset_sequences()
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_sequencer.theState == JVX_STATE_ACTIVE)
	{
		_common_set_sequencer.theSequences.all_sequences.clear();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	return(res);
}

// Add a new - then empty - sequence
jvxErrorType
CjvxSequencer::_add_sequence(const char* description, const char* label)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_sequencer.theState == JVX_STATE_ACTIVE)
	{
		std::vector<oneSequence>::iterator elm = _common_set_sequencer.theSequences.all_sequences.begin();
		elm = findSequenceNameInList(_common_set_sequencer.theSequences.all_sequences, description);
		if (elm == _common_set_sequencer.theSequences.all_sequences.end())
		{
			oneSequence newElm;
			newElm.currentState = JVX_SEQUENCE_STATE_NONE;
			newElm.description = description;
			newElm.label = label;
			newElm.isDefaultSeq = false;
			//newElm.elmActive;
			//newElm.lstOfEnterSteps;
			//newElm.lstOfLeaveSteps;
			//newElm.lstOfRunningSteps;
			newElm.timer_since_last_increment = -1;
			newElm.stepIdCnt = 0;
			newElm.markedForProcess = false;

			_common_set_sequencer.theSequences.all_sequences.push_back(newElm);
		}
		else
		{
			res = JVX_ERROR_DUPLICATE_ENTRY;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

// Add a new - then empty - sequence
jvxErrorType
CjvxSequencer::_find_sequence_id_name(const char* description, jvxSize* id_out)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize id_out_v = JVX_SIZE_UNSELECTED;

	if (_common_set_sequencer.theState == JVX_STATE_ACTIVE)
	{
		for (i = 0; i < _common_set_sequencer.theSequences.all_sequences.size(); i++)
		{
			if (_common_set_sequencer.theSequences.all_sequences[i].description == description)
			{
				id_out_v = i;
				break;
			}
		}
		if (JVX_CHECK_SIZE_UNSELECTED(id_out_v))
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	if (id_out)
	{
		*id_out = id_out_v;
	}

	return(res);
}

// Remove a previously added sequence
jvxErrorType
CjvxSequencer::_remove_sequence(jvxSize idx)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_sequencer.theState == JVX_STATE_ACTIVE)
	{
		std::vector<oneSequence>::iterator elm = _common_set_sequencer.theSequences.all_sequences.begin();
		elm = findSequenceIdInList(_common_set_sequencer.theSequences.all_sequences, idx);
		if (elm != _common_set_sequencer.theSequences.all_sequences.end())
		{
			_common_set_sequencer.theSequences.all_sequences.erase(elm);
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

// Add a step element which is run
jvxErrorType
CjvxSequencer::_insert_step_sequence_at(
	jvxSize idx,
	jvxSequencerQueueType queueType,
	jvxSequencerElementType in_elementType,
	jvxComponentIdentification in_targetComponentType,
	const char* description,
	jvxSize in_functionId,
	jvxInt64 timeout_ms,
	const char* in_label_step,
	const char* in_label_cond_true,
	const char* in_label_cond_false,
	jvxBool break_active,
	jvxCBitField assoc_mode,
	jvxSize posi)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<oneSequenceStep>* theList = NULL;
	std::vector<oneSequenceStep> newList;
	if (_common_set_sequencer.theState == JVX_STATE_ACTIVE)
	{
		if (idx < _common_set_sequencer.theSequences.all_sequences.size())
		{
			oneSequenceStep step;
			step.step_tp = in_elementType;
			step.step_target = in_targetComponentType;
			step.step_function_id = in_functionId;
			step.step_label = in_label_step;
			step.step_cond_label_true = in_label_cond_true;
			step.step_cond_label_false = in_label_cond_false;
			step.step_timeout_ms = timeout_ms;
			step.description = description;
			step.break_active = break_active;
			step.assoc_mode = assoc_mode;

			switch (queueType)
			{
			case JVX_SEQUENCER_QUEUE_TYPE_RUN:
				theList = &_common_set_sequencer.theSequences.all_sequences[idx].lstOfRunningSteps;
				break;
			case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
				theList = &_common_set_sequencer.theSequences.all_sequences[idx].lstOfLeaveSteps;
				break;
			default:
				assert(0);
			}
			if ((posi != (jvxSize)-1) && (posi < theList->size()))
			{
				for (i = 0; i < posi; i++)
				{
					newList.push_back((*theList)[i]);
				}
				newList.push_back(step);

				for (; i < theList->size(); i++)
				{
					newList.push_back((*theList)[i]);
				}

				switch (queueType)
				{
				case JVX_SEQUENCER_QUEUE_TYPE_RUN:
					_common_set_sequencer.theSequences.all_sequences[idx].lstOfRunningSteps = newList;
					break;
				case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
					_common_set_sequencer.theSequences.all_sequences[idx].lstOfLeaveSteps = newList;
					break;
				default:
					assert(0);
				}
			}
			else
			{
				(*theList).push_back(step);
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxSequencer::_edit_step_sequence(
	jvxSize idxsequence,
	jvxSequencerQueueType tp,
	jvxSize idxstep,
	jvxSequencerElementType in_elementType,
	jvxComponentIdentification in_targetComponentType,
	const char* description,
	jvxSize in_functionId,
	jvxInt64 timeout_ms,
	const char* in_label_step,
	const char* in_label_cond_true,
	const char* in_label_cond_false,
	jvxBool break_active,
	jvxCBitField assoc_mode,
	jvxCBitField whatToSet)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<oneSequenceStep>* theList = NULL;
	std::vector<oneSequenceStep> newList;
	if (_common_set_sequencer.theState == JVX_STATE_ACTIVE)
	{
		if (idxsequence < _common_set_sequencer.theSequences.all_sequences.size())
		{
			switch (tp)
			{
			case JVX_SEQUENCER_QUEUE_TYPE_RUN:
				theList = &_common_set_sequencer.theSequences.all_sequences[idxsequence].lstOfRunningSteps;
				break;
			case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
				theList = &_common_set_sequencer.theSequences.all_sequences[idxsequence].lstOfLeaveSteps;
				break;
			default:
				assert(0);
			}
			if (idxstep < theList->size())
			{
				if (whatToSet & JVX_SEQUENCER_EDIT_STEP_DESCRIPTION)
				{
					(*theList)[idxstep].description = description;
				}
				if (whatToSet & JVX_SEQUENCER_EDIT_STEP_LABEL_FALSE)
				{
					(*theList)[idxstep].step_cond_label_false = in_label_cond_false;
				}
				if (whatToSet & JVX_SEQUENCER_EDIT_STEP_LABEL_TRUE)
				{
					(*theList)[idxstep].step_cond_label_true = in_label_cond_true;
				}
				if (whatToSet & JVX_SEQUENCER_EDIT_STEP_FID)
				{
					(*theList)[idxstep].step_function_id = in_functionId;
				}
				if (whatToSet & JVX_SEQUENCER_EDIT_STEP_LABEL)
				{
					(*theList)[idxstep].step_label = in_label_step;
				}
				if (whatToSet & JVX_SEQUENCER_EDIT_STEP_TARGET)
				{
					(*theList)[idxstep].step_target = in_targetComponentType;
				}
				if (whatToSet & JVX_SEQUENCER_EDIT_STEP_TIMEOUT)
				{
					(*theList)[idxstep].step_timeout_ms = timeout_ms;
				}
				if (whatToSet & JVX_SEQUENCER_EDIT_STEP_TYPE)
				{
					(*theList)[idxstep].step_tp = in_elementType;
				}
				if (whatToSet & JVX_SEQUENCER_EDIT_STEP_BREAK_STEP)
				{
					(*theList)[idxstep].break_active = break_active;
				}
				if (whatToSet & JVX_SEQUENCER_EDIT_STEP_ASSOC_MODE)
				{
					(*theList)[idxstep].assoc_mode = assoc_mode;
				}
				_common_set_sequencer.theSequences.all_sequences[idxsequence].isDefaultSeq = false;
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxSequencer::_switch_steps_sequence(jvxSize idxsequence, jvxSequencerQueueType tp, jvxSize idx1, jvxSize idx2)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<oneSequenceStep>* theList = NULL;
	std::vector<oneSequenceStep> newList;
	if (_common_set_sequencer.theState == JVX_STATE_ACTIVE)
	{
		if (idxsequence < _common_set_sequencer.theSequences.all_sequences.size())
		{
			switch (tp)
			{
			case JVX_SEQUENCER_QUEUE_TYPE_RUN:
				theList = &_common_set_sequencer.theSequences.all_sequences[idxsequence].lstOfRunningSteps;
				break;
			case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
				theList = &_common_set_sequencer.theSequences.all_sequences[idxsequence].lstOfLeaveSteps;
				break;
			default:
				assert(0);
			}

			jvxSize minIdx = JVX_MIN(idx1, idx2);
			jvxSize maxIdx = JVX_MAX(idx1, idx2);
			if (minIdx < maxIdx)
			{
				if ((minIdx < theList->size()) && (maxIdx < theList->size()))
				{
					for (i = 0; i < minIdx; i++)
					{
						newList.push_back((*theList)[i]);
					}
					newList.push_back((*theList)[maxIdx]);
					for (i = minIdx + 1; i < maxIdx; i++)
					{
						newList.push_back((*theList)[i]);
					}
					newList.push_back((*theList)[minIdx]);
					for (i = maxIdx + 1; i < theList->size(); i++)
					{
						newList.push_back((*theList)[i]);
					}

					switch (tp)
					{
					case JVX_SEQUENCER_QUEUE_TYPE_RUN:
						_common_set_sequencer.theSequences.all_sequences[idxsequence].lstOfRunningSteps = newList;
						break;
					case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
						_common_set_sequencer.theSequences.all_sequences[idxsequence].lstOfLeaveSteps = newList;
						break;
					default:
						assert(0);
					}

					_common_set_sequencer.theSequences.all_sequences[idxsequence].isDefaultSeq = false;
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxSequencer::_remove_step_sequence(jvxSize idxsequence, jvxSequencerQueueType tp, jvxSize idx)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<oneSequenceStep>* theList = NULL;
	std::vector<oneSequenceStep> newList;
	if (_common_set_sequencer.theState == JVX_STATE_ACTIVE)
	{
		if (idxsequence < _common_set_sequencer.theSequences.all_sequences.size())
		{
			switch (tp)
			{
			case JVX_SEQUENCER_QUEUE_TYPE_RUN:
				theList = &_common_set_sequencer.theSequences.all_sequences[idxsequence].lstOfRunningSteps;
				break;
			case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
				theList = &_common_set_sequencer.theSequences.all_sequences[idxsequence].lstOfLeaveSteps;
				break;
			default:
				assert(0);
			}
			std::vector<oneSequenceStep>::iterator elm = findSequenceStepIdInList(*theList, idx);
			if (elm != theList->end())
			{
				theList->erase(elm);

				_common_set_sequencer.theSequences.all_sequences[idxsequence].isDefaultSeq = false;
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
};

jvxErrorType
CjvxSequencer::_reset_sequence(jvxSize in_handleId_sequence)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_sequencer.theState == JVX_STATE_ACTIVE)
	{
		std::vector<oneSequence>::iterator elm = _common_set_sequencer.theSequences.all_sequences.begin();
		elm = findSequenceIdInList(_common_set_sequencer.theSequences.all_sequences, in_handleId_sequence);
		if (elm != _common_set_sequencer.theSequences.all_sequences.end())
		{
			elm->lstOfRunningSteps.clear();
			elm->lstOfLeaveSteps.clear();
			elm->isDefaultSeq = false;
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxSequencer::_set_sequence_default(jvxSize in_idxsequence)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_sequencer.theState == JVX_STATE_ACTIVE)
	{
		if (in_idxsequence < _common_set_sequencer.theSequences.all_sequences.size())
		{
			_common_set_sequencer.theSequences.all_sequences[in_idxsequence].isDefaultSeq = true;
		}
	}
	return(res);
}

jvxErrorType
CjvxSequencer::_mark_sequence_process(jvxSize idxsequence, jvxBool runit)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<oneSequenceStep>* theList = NULL;
	std::vector<oneSequenceStep> newList;
	if (_common_set_sequencer.theState == JVX_STATE_ACTIVE)
	{
		if (idxsequence < _common_set_sequencer.theSequences.all_sequences.size())
		{
			_common_set_sequencer.theSequences.all_sequences[idxsequence].markedForProcess = runit;
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

// Run a process which is composed of a list of sequences
jvxErrorType
CjvxSequencer::_run_process(IjvxSequencer_report* report, jvxHandle* privateData,
	jvxCBitField eventMask, jvxInt64 timestamp_us, jvxSize gran_report_state_wait)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool foundStartPoint = false;

	if (_common_set_sequencer.theState == JVX_STATE_ACTIVE)
	{
		foundStartPoint = findNextValidStep(0);
		if (!foundStartPoint)
		{
			res = JVX_ERROR_NOT_READY;
		}
		else
		{
			_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_STARTUP;
			_common_set_sequencer.theState = JVX_STATE_PROCESSING;
			_common_set_sequencer.in_processing_state = 0;
			_common_set_sequencer.granularity_report_state = gran_report_state_wait;
			_common_set_sequencer.granularity_report_state_cnt = 0;
			_common_set_sequencer.in_processing = false;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	if (res == JVX_NO_ERROR)
	{
		_common_set_sequencer.report = report;
		_common_set_sequencer.inOperation->eventMask = eventMask;
		_common_set_sequencer.inOperation->howtosaygoodbye = JVX_SEQUENCER_EVENT_NONE;
		/*
		_common_set_sequencer.inOperation->idxSequence = 0;
		_common_set_sequencer.inOperation->idxStep = 0;*/

		_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_STARTUP; // has been written before..

		_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_IN_OPERATION;

		_common_set_sequencer.in_processing_state = 0;
		_common_set_sequencer.run_mode_operate = _common_set_sequencer.run_mode_start;
		_common_set_sequencer.run_mode_save = _common_set_sequencer.run_mode_operate;
		_common_set_sequencer.intercept_count = JVX_SIZE_UNSELECTED;

		_common_set_sequencer.inOperation->in_processing_state_on_enter = 0;

		auto elm = _common_set_sequencer.immediate_tasks.begin();
		for (; elm != _common_set_sequencer.immediate_tasks.end(); elm++)
		{
			elm->second.task->restart();
			elm->second.stack_height_run = 0;
		}

		res = sequencer_started(timestamp_us);

		if (_common_set_sequencer.report)
		{
			_common_set_sequencer.report->report_event(_common_set_sequencer.inOperation->theSeqState,
				JVX_SEQUENCER_EVENT_SEQUENCE_STARTUP_COMPLETE,
				("Startup of sequence " + jvx_size2String(_common_set_sequencer.inOperation->idxSequence) + " successfully completed").c_str(),
				JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, JVX_SEQUENCER_QUEUE_TYPE_NONE, JVX_SEQUENCER_TYPE_COMMAND_NONE, JVX_SIZE_UNSELECTED,
				_common_set_sequencer.in_processing_state, true);
		}
	}

	return(res);
}


jvxErrorType
CjvxSequencer::_trigger_step_process_extern(jvxInt64 timestamp_us, jvxSequencerElementType* sequencer_element_type_on_leave)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (sequencer_element_type_on_leave)
	{
		*sequencer_element_type_on_leave = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	}
	if (_common_set_sequencer.theState == JVX_STATE_PROCESSING)
	{
		if (
			(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_IN_OPERATION) ||
			(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_ERROR) ||
			(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_WANTS_TO_STOP_PROCESS) ||
			(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_WANTS_TO_STOP_SEQUENCE) ||
			(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_SHUTDOWN_IN_PROGRESS))
		{
			res = JVX_ERROR_REQUEST_CALL_AGAIN;
			while (res == JVX_ERROR_REQUEST_CALL_AGAIN)
			{
				res = sequencer_step(timestamp_us, NULL);
			}
		}
		else if (_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_SHUTDOWN_COMPLETE)
		{
			res = JVX_ERROR_PROCESS_COMPLETE;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		if (sequencer_element_type_on_leave)
		{
			*sequencer_element_type_on_leave = _common_set_sequencer.inOperation->thisStep.step_tp;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxSequencer::_trigger_complete_process_extern(jvxInt64 timestamp_us)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_sequencer.theState == JVX_STATE_PROCESSING)
	{
		if (_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_SHUTDOWN_COMPLETE)
		{
			res = sequencer_stopped( timestamp_us);
			_common_set_sequencer.theState = JVX_STATE_COMPLETE;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxSequencer::_read_step_type_process_extern(jvxSequencerElementType* theElementType)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_sequencer.theState == JVX_STATE_PROCESSING)
	{
		if (_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_IN_OPERATION)
		{
			if (theElementType)
			{
				*theElementType = _common_set_sequencer.inOperation->thisStep.step_tp;
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxSequencer::_trigger_abort_process()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_sequencer.theState == JVX_STATE_PROCESSING)
	{
		if (
			(_common_set_sequencer.inOperation->theSeqState > JVX_SEQUENCER_STATUS_NONE) &&
			(_common_set_sequencer.inOperation->theSeqState < JVX_SEQUENCER_STATUS_WANTS_TO_STOP_PROCESS))
		{
			_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_WANTS_TO_STOP_PROCESS;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxSequencer::_trigger_abort_sequence()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_sequencer.theState == JVX_STATE_PROCESSING)
	{
		if (
			(_common_set_sequencer.inOperation->theSeqState > JVX_SEQUENCER_STATUS_NONE) &&
			(_common_set_sequencer.inOperation->theSeqState < JVX_SEQUENCER_STATUS_WANTS_TO_STOP_PROCESS))
		{
			_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_WANTS_TO_STOP_SEQUENCE;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

// Abort a certain process
jvxErrorType
CjvxSequencer::_acknowledge_completion_process()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_sequencer.theState == JVX_STATE_COMPLETE)
	{
		if (_common_set_sequencer.inOperation->theSeqState != JVX_SEQUENCER_STATUS_SHUTDOWN_COMPLETE)
		{
			assert(0);
			res = JVX_ERROR_NOT_READY;
		}
		else
		{
			_common_set_sequencer.theState = JVX_STATE_ACTIVE;
			_common_set_sequencer.in_processing_state = JVX_SIZE_UNSELECTED;

			// Removes current element from stack
			_common_set_sequencer.inOperation = NULL;
			_common_set_sequencer.operationStack.clear();
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

// Return the current status of the sequencer
jvxErrorType
CjvxSequencer::_status_process(jvxSequencerStatus* status, jvxSize * seqId, jvxSequencerQueueType* qTp, jvxSize * stepId, jvxBool* loop_enabled)
{
	if (status)
	{
		*status = JVX_SEQUENCER_STATUS_NONE;
	}
	if (seqId)
	{
		*seqId = JVX_SIZE_UNSELECTED;
	}
	if (seqId)
	{
		*stepId = JVX_SIZE_UNSELECTED;
	}
	if (loop_enabled)
	{
		*loop_enabled = false;
	}
	if (qTp)
	{
		*qTp = JVX_SEQUENCER_QUEUE_TYPE_NONE;
	}

	if (_common_set_sequencer.inOperation)
	{
		if (status)
		{
			*status = _common_set_sequencer.inOperation->theSeqState;
		}
		if (seqId)
		{
			*seqId = _common_set_sequencer.inOperation->idxSequence;
		}
		if (seqId)
		{
			*stepId = _common_set_sequencer.inOperation->idxStep;
		}
		if (loop_enabled)
		{
			*loop_enabled = _common_set_sequencer.inOperation->loop_enabled;
		}
		if (qTp)
		{
			*qTp = _common_set_sequencer.inOperation->currentQueueTp;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxSequencer::_enable_loop_process(jvxBool enable)
{
	_common_set_sequencer.loop_enable = enable;
	return(JVX_NO_ERROR);
}

// Browse registered sequences
jvxErrorType
CjvxSequencer::_number_sequences(jvxSize* num)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_sequencer.theState >= JVX_STATE_ACTIVE)
	{
		if (num)
		{
			*num = _common_set_sequencer.theSequences.all_sequences.size();
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}


// Browse registered sequences
jvxErrorType
CjvxSequencer::_description_sequence(jvxSize idx, jvxApiString* description, jvxApiString* label, jvxSize* numStepsRun, jvxSize* numStepsLeave, jvxBool* markedForProcess, jvxBool* isDefaultSequence)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_sequencer.theState >= JVX_STATE_ACTIVE)
	{
		if (idx < _common_set_sequencer.theSequences.all_sequences.size())
		{
			if (description)
			{
				description->assign(_common_set_sequencer.theSequences.all_sequences[idx].description);
			}
			if (numStepsRun)
			{
				*numStepsRun = _common_set_sequencer.theSequences.all_sequences[idx].lstOfRunningSteps.size();
			}
			if (numStepsLeave)
			{
				*numStepsLeave = _common_set_sequencer.theSequences.all_sequences[idx].lstOfLeaveSteps.size();
			}
			if (markedForProcess)
			{
				*markedForProcess = _common_set_sequencer.theSequences.all_sequences[idx].markedForProcess;
			}
			if (isDefaultSequence)
			{
				*isDefaultSequence = _common_set_sequencer.theSequences.all_sequences[idx].isDefaultSeq;
			}
			if (label)
			{
				label->assign(_common_set_sequencer.theSequences.all_sequences[idx].label);
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxSequencer::_set_description_sequence(jvxSize idx, const char* descr, const char* label)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_sequencer.theState >= JVX_STATE_ACTIVE)
	{
		if (idx < _common_set_sequencer.theSequences.all_sequences.size())
		{
			if (descr)
			{
				_common_set_sequencer.theSequences.all_sequences[idx].description = descr;
				_common_set_sequencer.theSequences.all_sequences[idx].isDefaultSeq = false;
			}
			if (label)
			{
				_common_set_sequencer.theSequences.all_sequences[idx].label = label;
				_common_set_sequencer.theSequences.all_sequences[idx].isDefaultSeq = false;
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

// Browse registered sequences
jvxErrorType
CjvxSequencer::_description_step_sequence(jvxSize idx_seq, jvxSize idx_steps, jvxSequencerQueueType tp, jvxApiString* description,
	jvxSequencerElementType* out_elementType, jvxComponentIdentification* out_targetComponentType,
	jvxSize* out_functionId, jvxInt64* out_timeout_ms, jvxApiString* label_name, jvxApiString* cond_label_true, 
	jvxApiString* cond_label_false, jvxBool* break_action, jvxCBitField* assoc_mode)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_sequencer.theState >= JVX_STATE_ACTIVE)
	{
		if (idx_seq < _common_set_sequencer.theSequences.all_sequences.size())
		{
			std::vector<oneSequenceStep>* theList = NULL;
			switch (tp)
			{
			case JVX_SEQUENCER_QUEUE_TYPE_RUN:
				theList = &_common_set_sequencer.theSequences.all_sequences[idx_seq].lstOfRunningSteps;
				break;
			case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
				theList = &_common_set_sequencer.theSequences.all_sequences[idx_seq].lstOfLeaveSteps;
				break;
			default: assert(0);
			}
			if (idx_steps < (*theList).size())
			{
				if (description)
				{
					description->assign((*theList)[idx_steps].description);
				}
				if (out_elementType)
				{
					*out_elementType = (*theList)[idx_steps].step_tp;
				}
				if (out_targetComponentType)
				{
					*out_targetComponentType = (*theList)[idx_steps].step_target;
				}
				if (out_functionId)
				{
					*out_functionId = (jvxInt32)((*theList)[idx_steps].step_function_id);
				}
				if (out_timeout_ms)
				{
					*out_timeout_ms = (*theList)[idx_steps].step_timeout_ms;
				}
				if (label_name)
				{
					label_name->assign((*theList)[idx_steps].step_label);
				}
				if (cond_label_true)
				{
					cond_label_true->assign((*theList)[idx_steps].step_cond_label_true);
				}
				if (cond_label_false)
				{
					cond_label_false->assign((*theList)[idx_steps].step_cond_label_false);
				}
				if (break_action)
				{
					*break_action = (*theList)[idx_steps].break_active;
				}
				if (assoc_mode)
				{
					*assoc_mode = (*theList)[idx_steps].assoc_mode;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxSequencer::_switch_sequences(jvxSize idx1, jvxSize idx2)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_sequencer.theState >= JVX_STATE_ACTIVE)
	{
		if (
			(idx1 < _common_set_sequencer.theSequences.all_sequences.size()) &&
			(idx2 < _common_set_sequencer.theSequences.all_sequences.size())
			)
		{
			std::vector<oneSequence> newList;
			jvxSize minId = JVX_MIN(idx1, idx2);
			jvxSize maxId = JVX_MAX(idx1, idx2);

			for (i = 0; i < idx1; i++)
			{
				newList.push_back(_common_set_sequencer.theSequences.all_sequences[i]);
			}
			newList.push_back(_common_set_sequencer.theSequences.all_sequences[maxId]);
			for (i = idx1 + 1; i < idx2; i++)
			{
				newList.push_back(_common_set_sequencer.theSequences.all_sequences[i]);
			}
			newList.push_back(_common_set_sequencer.theSequences.all_sequences[minId]);
			for (i = idx2 + 1; i < _common_set_sequencer.theSequences.all_sequences.size(); i++)
			{
				newList.push_back(_common_set_sequencer.theSequences.all_sequences[i]);
			}
			_common_set_sequencer.theSequences.all_sequences = newList;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

// ==========================================================================================
// Put/get configuration
// ==========================================================================================
jvxErrorType
CjvxSequencer::_put_configuration_seq(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllEntries, 
	const char* filename, jvxInt32 lineno)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxConfigData* secVersion = NULL;
	jvxApiString versionStr;
	jvxConfigData* sectionAll = NULL;

	this->_reset_sequences();

	res = processor->getReferenceSubsectionCurrentSection_name(sectionToContainAllEntries, &sectionAll, JVX_CONFIG_SEQUENCER_SEQ_SECTION);
	if ((res == JVX_NO_ERROR) && sectionAll)
	{
		processor->getReferenceEntryCurrentSection_name(sectionAll, &secVersion, JVX_CONFIG_SEQUENCER_SEQ_VERSION);
		if (secVersion)
		{
			res = processor->getAssignmentString(secVersion, &versionStr);
		}

		if (versionStr.std_str() == JVX_CONFIG_SEQUENCER_SEQ_VERSION_2_0)
		{
			res = put_configuration_seq_v2_0(callConf, processor, sectionAll, filename, lineno);
		}
		else
		{
			res = put_configuration_seq_v1_0(callConf, processor, sectionAll, filename, lineno);
		}
	}
	return res;
}

jvxErrorType
CjvxSequencer::put_configuration_seq_v1_0(
	jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionAll, 
	const char* filename, jvxInt32 lineno)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i, j;
	jvxSize num = 0;
	jvxSize numS = 0;

	jvxConfigData* sectionStatus = NULL;
	jvxConfigData* datTmp = NULL;
	jvxConfigData* datTmp1 = NULL;
	jvxConfigData* datTmp2 = NULL;
	jvxConfigData* datTmp3 = NULL;
	jvxConfigData* datTmp_steps = NULL;
	//jvxDataList* lstD1 = NULL;
	std::string entry;
	std::string label;
	jvxInt16 entryInt16;
	jvxApiString descr;

	jvxValue valIsDefault;
	jvxBool flagIsDefSeq = false;
	jvxValue valIsEngaged;
	jvxBool flagIsEngaged = false;
	jvxBool b_action;

	std::vector<std::string> entriesStr, entriesStrLab, entriesStrLabTrue, entriesStrLabFalse;
	std::vector<jvxInt16> entriesInt16;
	std::vector<std::string> entriesStringComp;
	std::vector<jvxSize> entriesSizeComm;
	std::vector<jvxInt64> entriesInt64Timeouts;
	std::vector<jvxInt16> entriesInt16BreakAction;
	std::vector<jvxSize> entriesCompSlotIds;
	std::vector<jvxSize> entriesCompSlotSubIds;
	processor->getNumberSubsectionsCurrentSection(sectionAll, &num);
	for (i = 0; i < num; i++)
	{
		datTmp = NULL;
		res = processor->getReferenceSubsectionCurrentSection_id(sectionAll, &datTmp, i);
		if ((res == JVX_NO_ERROR) && datTmp)
		{
			// Read section-names
			entriesStr.clear();
			label.clear();
			HjvxConfigProcessor_readEntry_assignmentString(processor, datTmp, JVX_CONFIG_SEQUENCER_SEQ_LABEL, &label);

			if (HjvxConfigProcessor_readEntry_assignmentString(processor, datTmp, JVX_CONFIG_SEQUENCER_SEQ_NAMES, &entry) == JVX_NO_ERROR)
			{

				HjvxConfigProcessor_readEntry_assignment<jvxInt16>(processor, datTmp, JVX_CONFIG_SEQUENCER_SEQ_MARKED_FLAG, &entryInt16);

				this->_add_sequence(entry.c_str(), label.c_str());
				if (entryInt16 != 0)
				{
					this->_mark_sequence_process(i, true);
				}
				else
				{
					this->_mark_sequence_process(i, false);
				}

				entriesStr.clear();
				entriesStrLab.clear();
				entriesStrLabTrue.clear();
				entriesStrLabFalse.clear();
				entriesInt16.clear();
				entriesStringComp.clear();
				entriesSizeComm.clear();
				entriesInt64Timeouts.clear();
				entriesInt16BreakAction.clear();
				entriesCompSlotIds.clear();
				entriesCompSlotSubIds.clear();

				// Next, add all steps for run sequence
				std::string searchMe = JVX_CONFIG_ONE_SEQUENCE_STEPS_PREFIX + (std::string)"run_" + jvx_int2String((int)i);

				datTmp_steps = NULL;
				res = processor->getReferenceEntryCurrentSection_name(datTmp, &datTmp_steps, searchMe.c_str());
				if ((res == JVX_NO_ERROR) && datTmp_steps)
				{
					HjvxConfigProcessor_readEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_DECSCS, &entriesStr);
					HjvxConfigProcessor_readEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL, &entriesStrLab);
					HjvxConfigProcessor_readEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_TRUE, &entriesStrLabTrue);
					HjvxConfigProcessor_readEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_FALSE, &entriesStrLabFalse);

					HjvxConfigProcessor_readEntry_1dList<jvxInt16>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_TYPES, entriesInt16);
					HjvxConfigProcessor_readEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENTS, &entriesStringComp);
					HjvxConfigProcessor_readEntry_1dList<jvxSize>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_SLOTIDS, entriesCompSlotIds);
					HjvxConfigProcessor_readEntry_1dList<jvxSize>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_SLOTSUBIDS, entriesCompSlotSubIds);

					HjvxConfigProcessor_readEntry_1dList<jvxSize>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMMAND_IDS, entriesSizeComm);
					HjvxConfigProcessor_readEntry_1dList<jvxInt64>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_TIMEOUTS, entriesInt64Timeouts);
					HjvxConfigProcessor_readEntry_1dList<jvxInt16>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_BREAK_ACTION, entriesInt16BreakAction);

					jvxSize numSteps = 0;
					numSteps = entriesStr.size();
					numSteps = JVX_MIN(numSteps, entriesStrLab.size());
					numSteps = JVX_MIN(numSteps, entriesStrLabTrue.size());
					numSteps = JVX_MIN(numSteps, entriesStrLabFalse.size());
					numSteps = JVX_MIN(numSteps, entriesInt16.size());
					numSteps = JVX_MIN(numSteps, entriesStringComp.size());
					numSteps = JVX_MIN(numSteps, entriesSizeComm.size());
					numSteps = JVX_MIN(numSteps, entriesInt64Timeouts.size());
					// If break action is missing, ignore. It is a new feature!

					for (j = 0; j < numSteps; j++)
					{
						jvxComponentIdentification cTp;
						cTp.tp = jvx_componentNameToComponentType(entriesStringComp[j].c_str());
						cTp.slotid = 0;
						if (j < entriesCompSlotIds.size())
						{
							cTp.slotid = entriesCompSlotIds[j];
						}
						cTp.slotsubid = 0;
						if (j < entriesCompSlotSubIds.size())
						{
							cTp.slotsubid = entriesCompSlotSubIds[j];
						}

						b_action = false;
						if (j < entriesInt16BreakAction.size())
						{
							if (entriesInt16BreakAction[j])
							{
								b_action = true;
							}
						}

						this->_insert_step_sequence_at(i, JVX_SEQUENCER_QUEUE_TYPE_RUN, (jvxSequencerElementType)entriesInt16[j], cTp,
							entriesStr[j].c_str(), entriesSizeComm[j],
							entriesInt64Timeouts[j], entriesStrLab[j].c_str(),
							entriesStrLabTrue[j].c_str(), entriesStrLabFalse[j].c_str(),
							b_action, -1);
					}
				}

				// Next, add all steps for run sequence
				searchMe = JVX_CONFIG_ONE_SEQUENCE_STEPS_PREFIX + (std::string)"leave_" + jvx_int2String((int)i);

				entriesStr.clear();
				entriesStrLab.clear();
				entriesStrLabTrue.clear();
				entriesStrLabFalse.clear();
				entriesInt16.clear();
				entriesStringComp.clear();
				entriesSizeComm.clear();
				entriesInt64Timeouts.clear();
				entriesCompSlotIds.clear();
				entriesCompSlotSubIds.clear();
				entriesInt16BreakAction.clear();

				datTmp_steps = NULL;
				res = processor->getReferenceEntryCurrentSection_name(datTmp, &datTmp_steps, searchMe.c_str());
				if ((res == JVX_NO_ERROR) && datTmp_steps)
				{
					HjvxConfigProcessor_readEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_DECSCS, &entriesStr);
					HjvxConfigProcessor_readEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL, &entriesStrLab);
					HjvxConfigProcessor_readEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_TRUE, &entriesStrLabTrue);
					HjvxConfigProcessor_readEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_FALSE, &entriesStrLabFalse);
					HjvxConfigProcessor_readEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENTS, &entriesStringComp);
					HjvxConfigProcessor_readEntry_1dList<jvxSize>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_SLOTIDS, entriesCompSlotIds);
					HjvxConfigProcessor_readEntry_1dList<jvxSize>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_SLOTSUBIDS, entriesCompSlotSubIds);

					HjvxConfigProcessor_readEntry_1dList<jvxInt16>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_TYPES, entriesInt16);
					HjvxConfigProcessor_readEntry_1dList<jvxSize>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMMAND_IDS, entriesSizeComm);
					HjvxConfigProcessor_readEntry_1dList<jvxInt64>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_TIMEOUTS, entriesInt64Timeouts);
					HjvxConfigProcessor_readEntry_1dList<jvxInt16>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_BREAK_ACTION, entriesInt16BreakAction);

					jvxSize numSteps = 0;
					numSteps = entriesStr.size();
					numSteps = JVX_MIN(numSteps, entriesStrLab.size());
					numSteps = JVX_MIN(numSteps, entriesStrLabTrue.size());
					numSteps = JVX_MIN(numSteps, entriesStrLabFalse.size());
					numSteps = JVX_MIN(numSteps, entriesInt16.size());
					numSteps = JVX_MIN(numSteps, entriesStringComp.size());
					numSteps = JVX_MIN(numSteps, entriesSizeComm.size());
					numSteps = JVX_MIN(numSteps, entriesInt64Timeouts.size());
					// If break action is missing, ignore. It is a new feature!

					for (j = 0; j < numSteps; j++)
					{
						jvxComponentIdentification cTp;
						cTp.tp = jvx_componentNameToComponentType(entriesStringComp[j].c_str());
						cTp.slotid = 0;
						if (j < entriesCompSlotIds.size())
						{
							cTp.slotid = entriesCompSlotIds[j];
						}
						cTp.slotsubid = 0;
						if (j < entriesCompSlotSubIds.size())
						{
							cTp.slotsubid = entriesCompSlotSubIds[j];
						}
						b_action = false;
						if (j < entriesInt16BreakAction.size())
						{
							if (entriesInt16BreakAction[j])
							{
								b_action = true;
							}
						}
						this->_insert_step_sequence_at(i, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, (jvxSequencerElementType)entriesInt16[j], cTp,
							entriesStr[j].c_str(), entriesSizeComm[j],
							entriesInt64Timeouts[j], entriesStrLab[j].c_str(),
							entriesStrLabTrue[j].c_str(), entriesStrLabFalse[j].c_str(),
							b_action, -1);
					}
				} 	//res = processor->getReferenceEntryCurrentSection_name(datTmp, &datTmp_steps, searchMe.c_str());
			}// if(HjvxConfigProcessor_readEntry_assignmentString(processor, datTmp, JVX_CONFIG_SEQUENCER_SEQ_NAMES, &entry) == JVX_NO_ERROR)	
		} //res = processor->getReferenceSubsectionCurrentSection_id(sectionToContainAllEntries, &datTmp, i);
	}//for(i = 0; i < num; i++)

	res = processor->getReferenceSubsectionCurrentSection_name(sectionAll, &sectionStatus, JVX_CONFIG_SEQUENCER_SEQ_STATUS);
	if ((res == JVX_NO_ERROR) && sectionStatus)
	{
		processor->getNumberSubsectionsCurrentSection(sectionStatus, &numS);
		for (i = 0; i < numS; i++)
		{
			datTmp = NULL;
			processor->getReferenceSubsectionCurrentSection_id(sectionStatus, &datTmp, i);
			if (datTmp)
			{
				processor->getReferenceEntryCurrentSection_name(datTmp, &datTmp1, "description");
				processor->getReferenceEntryCurrentSection_name(datTmp, &datTmp2, "is_default");
				processor->getReferenceEntryCurrentSection_name(datTmp, &datTmp3, "is_engaged");
				if (datTmp1 && datTmp2 && datTmp3)
				{
					res = processor->getAssignmentString(datTmp1, &descr);
					if (res == JVX_NO_ERROR)
					{
						res = processor->getAssignmentValue(datTmp2, &valIsDefault);
						if (res == JVX_NO_ERROR)
						{
							res = processor->getAssignmentValue(datTmp3, &valIsEngaged);
							if (res == JVX_NO_ERROR)
							{
								jvxBool foundit = false;
								valIsDefault.toContent(&flagIsDefSeq);
								valIsEngaged.toContent(&flagIsEngaged);

								oneEntryConfigSequences entry;
								entry.descr = descr.std_str();
								entry.isDefault = flagIsDefSeq;
								entry.isEngaged = flagIsEngaged;
								lst_config_seqs.push_back(entry);
							} // if (res == JVX_NO_ERROR) -- res = processor->getAssignmentValue(datTmp3, &valIsEngaged);
						} // if (res == JVX_NO_ERROR) -- res = processor->getAssignmentValue(datTmp2, &valIsDefault);
					} // if (res == JVX_NO_ERROR) -- res = processor->getAssignmentString(datTmp1, &descr);
				} // if (datTmp1 && datTmp2 && datTmp3)
			} // if (datTmp) -- processor->getReferenceSubsectionCurrentSection_id(sectionStatus, &datTmp, i);
		} // for (i = 0; i < numS; i++)
	}

	return(JVX_NO_ERROR);
}

void
CjvxSequencer::addOneStepSequencerConfig(IjvxConfigProcessor* processor, 
	jvxConfigData* oneStep, const std::string& nameSeq, 
	jvxSize idSeq, jvxSequencerQueueType qTp)
{
	std::string descr;
	std::string label;
	std::string entry;
	std::string label_true;
	std::string label_false;
	jvxSize szComId = JVX_SIZE_UNSELECTED;
	jvxSize szTimeout = JVX_SIZE_UNSELECTED;
	jvxBool b_active = false;
	jvxSize szBreak = JVX_SIZE_UNSELECTED;
	jvxBool errorDetected = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCBitField assoc_mode = 0;

	res = HjvxConfigProcessor_readEntry_assignmentString(processor, oneStep,
		JVX_CONFIG_ONE_SEQUENCE_STEP_DECSCS, &descr);
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Problem reading assignment <" << JVX_CONFIG_ONE_SEQUENCE_STEP_DECSCS
			<< "> in list of sequencer run steps for sequence <" << nameSeq << ">." << std::endl;
		errorDetected = true;
	}

	res = HjvxConfigProcessor_readEntry_assignmentString(processor, oneStep,
		JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL, &label);
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Problem reading assignment <" << JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL
			<< "> in list of sequencer run steps for sequence <" << nameSeq << ">." << std::endl;
		errorDetected = true;
	}

	res = HjvxConfigProcessor_readEntry_assignmentString(processor, oneStep,
		JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_TRUE, &label_true);
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Problem reading assignment <" << JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_TRUE
			<< "> in list of sequencer run steps for sequence <" << nameSeq << ">." << std::endl;
		errorDetected = true;
	}

	HjvxConfigProcessor_readEntry_assignmentString(processor, oneStep,
		JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_FALSE, &label_false);
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Problem reading assignment <" << JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_FALSE
			<< "> in list of sequencer run steps for sequence <" << nameSeq << ">." << std::endl;
		errorDetected = true;
	}

	res = HjvxConfigProcessor_readEntry_assignmentString(processor, oneStep,
		JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_IDENTIFICATION, &entry);
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Problem reading assignment <" << JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_IDENTIFICATION
			<< "> in list of sequencer run steps for sequence <" << nameSeq << ">." << std::endl;
		errorDetected = true;
	}

	jvxComponentIdentification idC = JVX_COMPONENT_UNKNOWN;
	res = jvxComponentIdentification_decode(idC, entry);
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Problem converting component identification assignment <"
			<< entry << "> into a valid value for sequence <" << nameSeq << ">." << std::endl;
		errorDetected = true;
	}

	res = HjvxConfigProcessor_readEntry_assignmentString(processor, oneStep,
		JVX_CONFIG_ONE_SEQUENCE_STEP_TYPES, &entry);
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Problem reading assignment <" << JVX_CONFIG_ONE_SEQUENCE_STEP_TYPES
			<< "> in list of sequencer run steps for sequence <" << nameSeq << ">." << std::endl;
		errorDetected = true;
	}

	jvxSequencerElementType seqETp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	entry = jvxSequencerElementType_decode(seqETp, entry);
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Problem converting sequencer element assignment <"
			<< entry << "> into a valid value for sequence <" << nameSeq << ">." << std::endl;
		errorDetected = true;
	}

	res = HjvxConfigProcessor_readEntry_assignment<jvxSize>(processor, oneStep,
		JVX_CONFIG_ONE_SEQUENCE_STEP_COMMAND_IDS, &szComId);
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Problem reading value <" << JVX_CONFIG_ONE_SEQUENCE_STEP_COMMAND_IDS
			<< "> in list of sequencer run steps for sequence <" << nameSeq << ">." << std::endl;
		errorDetected = true;
	}

	res = HjvxConfigProcessor_readEntry_assignment<jvxSize>(processor, oneStep,
		JVX_CONFIG_ONE_SEQUENCE_STEP_TIMEOUTS, &szTimeout);
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Problem reading value <" << JVX_CONFIG_ONE_SEQUENCE_STEP_COMMAND_IDS
			<< "> in list of sequencer run steps for sequence <" << nameSeq << ">." << std::endl;
		errorDetected = true;
	}

	b_active = false;
	HjvxConfigProcessor_readEntry_assignment<jvxSize>(processor, oneStep,
		JVX_CONFIG_ONE_SEQUENCE_STEP_BREAK_ACTION, &szBreak);
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Problem reading value <" << JVX_CONFIG_ONE_SEQUENCE_STEP_BREAK_ACTION
			<< "> in list of sequencer run steps for sequence <" << nameSeq << ">." << std::endl;
		errorDetected = true;
	}

	if (szBreak != 0)
	{
		b_active = true;
	}

	assoc_mode = JVX_SEQUENCER_MODE_ALL;
	HjvxConfigProcessor_readEntry_assignment<jvxCBitField>(processor, oneStep,
		JVX_CONFIG_ONE_SEQUENCE_STEP_ASSOC_MODE, &assoc_mode);
	/*
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Problem reading value <" << JVX_CONFIG_ONE_SEQUENCE_STEP_BREAK_ACTION
			<< "> in list of sequencer run steps for sequence <" << nameSeq << ">." << std::endl;
		errorDetected = true;
	}
	*/
	if (!errorDetected)
	{
		res = this->_insert_step_sequence_at(idSeq, qTp,
			seqETp, idC, descr.c_str(), szComId, szTimeout, label.c_str(),
			label_true.c_str(), label_false.c_str(),
			b_active, assoc_mode, JVX_SIZE_UNSELECTED);
		assert(res == JVX_NO_ERROR);
	}
	else
	{
		std::cout << "Skipping adding sequencer step due to read error." << std::endl;
	}
}

jvxErrorType
CjvxSequencer::put_configuration_seq_v2_0(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxConfigData* sectionAll, 
	const char* filename, jvxInt32 lineno)
{
	oneEntryConfigSequences entry;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i, j;
	jvxSize numSeq = 0;
	jvxSize numStep = 0;
	jvxApiString fldStr;
	jvxSize cnt = 0;

	jvxConfigData* sectionStatus = NULL;
	jvxConfigData* oneSequence = NULL;
	jvxConfigData* datTmp = NULL;
	jvxValue val;
	jvxBool isDefault;
	jvxBool isEngaged;
	std::string nameSeq;

	lst_config_seqs.clear();
	processor->getNumberSubsectionsCurrentSection(sectionAll, &numSeq);
	for (i = 0; i < numSeq; i++)
	{
		oneSequence = NULL;
		res = processor->getReferenceSubsectionCurrentSection_id(sectionAll, &oneSequence, i);
		if (oneSequence)
		{
			isDefault = false;
			datTmp = NULL;
			processor->getReferenceEntryCurrentSection_name(oneSequence, &datTmp, JVX_CONFIG_SEQUENCER_SEQ_DEFAULT);
			if (datTmp)
			{
				res = processor->getAssignmentValue(datTmp, &val);
				if (res == JVX_NO_ERROR)
				{
					val.toContent(&isDefault);
				}
			}

			isEngaged = false;
			datTmp = NULL;
			processor->getReferenceEntryCurrentSection_name(oneSequence, &datTmp, JVX_CONFIG_SEQUENCER_SEQ_ENGAGED);
			if (datTmp)
			{
				res = processor->getAssignmentValue(datTmp, &val);
				if (res == JVX_NO_ERROR)
				{
					val.toContent(&isEngaged);
				}
			}

			
			datTmp = NULL;
			processor->getReferenceEntryCurrentSection_name(oneSequence, &datTmp, JVX_CONFIG_SEQUENCER_SEQ_NAMES);
			res = processor->getAssignmentString(datTmp, &fldStr);
			if (res == JVX_NO_ERROR)
			{
				nameSeq = fldStr.std_str();
			}

			entry.descr = nameSeq;
			entry.isDefault = isDefault;
			entry.isEngaged = isEngaged;
			lst_config_seqs.push_back(entry);

			if (!isDefault)
			{
				datTmp = NULL;
				processor->getReferenceEntryCurrentSection_name(oneSequence, &datTmp, JVX_CONFIG_SEQUENCER_SEQ_LABEL);
				res = processor->getAssignmentString(datTmp, &fldStr);
				if (res == JVX_NO_ERROR)
				{
					std::string labelSeq = fldStr.std_str();
					jvxConfigData* sectionSteps = NULL;

					this->_add_sequence(nameSeq.c_str(), labelSeq.c_str());
					this->_mark_sequence_process(cnt, isEngaged);

					sectionSteps = NULL;
					processor->getReferenceEntryCurrentSection_name(oneSequence, &sectionSteps,
						(JVX_CONFIG_ONE_SEQUENCE_STEPS_PREFIX + (std::string)"run").c_str());
					if (sectionSteps)
					{
						jvxSize numSteps = 0;
						jvxConfigData* oneStep = NULL;
						processor->getNumberEntriesCurrentSection(sectionSteps, &numSteps);
						for (j = 0; j < numSteps; j++)
						{
							processor->getReferenceEntryCurrentSection_id(sectionSteps, &oneStep, j);
							if (oneStep)
							{
								addOneStepSequencerConfig(processor, oneStep, nameSeq, cnt, JVX_SEQUENCER_QUEUE_TYPE_RUN);
							} // if (oneStep)
						} // for (j = 0; j < numSteps; j++)
					} // if (sectionSteps)

					sectionSteps = NULL;
					processor->getReferenceEntryCurrentSection_name(oneSequence, &sectionSteps,
						(JVX_CONFIG_ONE_SEQUENCE_STEPS_PREFIX + (std::string)"leave").c_str());
					if (sectionSteps)
					{
						jvxSize numSteps = 0;
						jvxConfigData* oneStep = NULL;
						processor->getNumberEntriesCurrentSection(sectionSteps, &numSteps);
						for (j = 0; j < numSteps; j++)
						{
							processor->getReferenceEntryCurrentSection_id(sectionSteps, &oneStep, j);
							if (oneStep)
							{
								addOneStepSequencerConfig(processor, oneStep, nameSeq, cnt, JVX_SEQUENCER_QUEUE_TYPE_LEAVE);
							} // if (oneStep)
						} // for (j = 0; j < numSteps; j++)
					} // if (sectionSteps)
					cnt++;
				} // if (res == JVX_NO_ERROR) -- 
				  // processor->getReferenceEntryCurrentSection_name(oneSequence, &datTmp, JVX_CONFIG_SEQUENCER_SEQ_LABEL);
				  // res = processor->getAssignmentString(datTmp, &fldStr);
			} // if (!isDefault)
		} // res = processor->getReferenceSubsectionCurrentSection_id(sectionAll, &oneSequence, i);
		  // if ((res == JVX_NO_ERROR) && datTmp)
	} // for (i = 0; i < numSeq; i++)
	return(JVX_NO_ERROR);
}

// =====================================================================================

jvxErrorType
CjvxSequencer::_update_config_sequences()
{
	jvxSize i, j;
	for(i = 0; i < lst_config_seqs.size(); i++)
	{ 
		jvxBool foundit = false;
		for (j = 0; j < this->_common_set_sequencer.theSequences.all_sequences.size(); j++)
		{
			if (this->_common_set_sequencer.theSequences.all_sequences[j].description == lst_config_seqs[i].descr)
			{
				if (_common_set_sequencer.theSequences.all_sequences[j].isDefaultSeq == lst_config_seqs[i].isDefault)
				{
					foundit = true;
					break;
				}
			}
		}
		if (foundit)
		{
			this->_common_set_sequencer.theSequences.all_sequences[j].markedForProcess = lst_config_seqs[i].isEngaged;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSequencer::get_configuration_seq_v2_0(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections, 
	IjvxObject* me_object)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* datTmp = NULL;
	jvxConfigData* sectionAll = NULL;
	jvxConfigData* sectionOneSeq = NULL;
	jvxConfigData* commentStr = NULL;
	jvxConfigData* sectionSteps = NULL;
	jvxConfigData* sectionOneStep = NULL;

	jvxSize i, j;
	jvxSize numStepsRun = 0;
	jvxSize numStepsLeave = 0;
	jvxSize numSeq = 0;
	jvxBool markedForProcessing = false;
	jvxBool isDefaultSequence = false;
	jvxApiString fldStr;
	jvxApiString fldLabel;
	jvxApiString fldStr_lab;
	jvxApiString fldStr_lab_true;
	jvxApiString fldStr_lab_false;
	jvxValue oneVal;
	jvxSequencerElementType elmType = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	jvxComponentIdentification comType;
	comType.tp = JVX_COMPONENT_UNKNOWN;
	comType.slotid = JVX_SIZE_UNSELECTED;
	comType.slotsubid = JVX_SIZE_UNSELECTED;
	jvxSize idxCommandId = JVX_SIZE_UNSELECTED;
	jvxInt64 timeoutms = -1;
	jvxBool b_action = false;
	jvxCBitField assoc_mode;
	std::string entry;
	jvxSize sz;

	res = processor->createEmptySection(&sectionAll, JVX_CONFIG_SEQUENCER_SEQ_SECTION);
	assert(sectionAll);
	processor->createAssignmentString(&datTmp, JVX_CONFIG_SEQUENCER_SEQ_VERSION, JVX_CONFIG_SEQUENCER_SEQ_VERSION_2_0);
	assert(datTmp);
	processor->addSubsectionToSection(sectionAll, datTmp);
	datTmp = NULL;
	this->_number_sequences(&numSeq);
	for (i = 0; i < numSeq; i++)
	{
		numStepsRun = 0;
		numStepsLeave = 0;
		markedForProcessing = false;
		isDefaultSequence = false;

		res = this->_description_sequence(i, &fldStr, &fldLabel,
			&numStepsRun, &numStepsLeave,
			&markedForProcessing, &isDefaultSequence);
		assert(res == JVX_NO_ERROR);

		res = processor->createEmptySection(&sectionOneSeq, (JVX_CONFIG_SEQUENCER_SEQ_SUBSECTION_PREFIX + jvx_int2String((int)i)).c_str());
		assert(res == JVX_NO_ERROR);

		oneVal.assign(isDefaultSequence);
		processor->createAssignmentValue(&datTmp, JVX_CONFIG_SEQUENCER_SEQ_DEFAULT, oneVal);
		processor->addSubsectionToSection(sectionOneSeq, datTmp);
		datTmp = NULL;

		oneVal.assign(markedForProcessing);
		processor->createAssignmentValue(&datTmp, JVX_CONFIG_SEQUENCER_SEQ_ENGAGED, oneVal);
		processor->addSubsectionToSection(sectionOneSeq, datTmp);
		datTmp = NULL;

		if (isDefaultSequence)
		{
			processor->createComment(&commentStr, (" Default sequence with name <" + fldStr.std_str() + ">").c_str());
			processor->addSubsectionToSection(sectionAll, commentStr);
			entry = fldStr.std_str();
			HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneSeq, JVX_CONFIG_SEQUENCER_SEQ_NAMES, &entry);

		}
		else
		{
		    entry = fldStr.std_str();
	 		HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneSeq, JVX_CONFIG_SEQUENCER_SEQ_NAMES, &entry);

			entry = fldLabel.std_str();
			HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneSeq, JVX_CONFIG_SEQUENCER_SEQ_LABEL, &entry);

			// 
			sectionSteps = NULL;
			processor->createEmptySection(&sectionSteps, (JVX_CONFIG_ONE_SEQUENCE_STEPS_PREFIX + (std::string)"run").c_str());
			if (sectionSteps)
			{
				for (j = 0; j < numStepsRun; j++)
				{
					sectionOneStep = NULL;
					processor->createEmptySection(&sectionOneStep,
						(JVX_CONFIG_ONE_SEQUENCE_ONE_STEP_PREFIX + jvx_size2String(j)).c_str());

					res = this->_description_step_sequence(i, j,
						JVX_SEQUENCER_QUEUE_TYPE_RUN, &fldStr, &elmType, &comType,
						&idxCommandId, &timeoutms, &fldStr_lab, &fldStr_lab_true,
						&fldStr_lab_false, &b_action, &assoc_mode);
					assert(res == JVX_NO_ERROR);

					entry = fldStr.std_str();
					HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_DECSCS, &entry);

					entry = fldStr_lab.std_str();
					HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL, &entry);

					entry = fldStr_lab_true.std_str();
					HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_TRUE, &entry);

					entry = fldStr_lab_false.std_str();
					HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_FALSE, &entry);

					entry = jvxComponentIdentification_txt(comType);
					HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_IDENTIFICATION, &entry);

					entry = jvxSequencerElementType_txt(elmType);
					HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_TYPES, &entry);

					sz = idxCommandId;
					HjvxConfigProcessor_writeEntry_assignment<jvxSize>(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_COMMAND_IDS, &sz);

					sz = timeoutms;
					HjvxConfigProcessor_writeEntry_assignment<jvxSize>(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_TIMEOUTS, &sz);

					if (b_action)
						sz = 1;
					else
						sz = 0;
					HjvxConfigProcessor_writeEntry_assignment<jvxSize>(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_BREAK_ACTION, &sz);

					HjvxConfigProcessor_writeEntry_assignment<jvxCBitField>(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_ASSOC_MODE, &assoc_mode);

					processor->addSubsectionToSection(sectionSteps, sectionOneStep);
				} // for (j = 0; j < numStepsRun; j++)
				processor->addSubsectionToSection(sectionOneSeq, sectionSteps);
			} // if (sectionSteps)

			sectionSteps = NULL;
			processor->createEmptySection(&sectionSteps, (JVX_CONFIG_ONE_SEQUENCE_STEPS_PREFIX + (std::string)"leave").c_str());
			if (sectionSteps)
			{
				for (j = 0; j < numStepsLeave; j++)
				{
					sectionOneStep = NULL;
					processor->createEmptySection(&sectionOneStep,
						(JVX_CONFIG_ONE_SEQUENCE_ONE_STEP_PREFIX + jvx_size2String(j)).c_str());

					res = this->_description_step_sequence(i, j,
						JVX_SEQUENCER_QUEUE_TYPE_LEAVE, &fldStr, &elmType, &comType,
						&idxCommandId, &timeoutms, &fldStr_lab, &fldStr_lab_true,
						&fldStr_lab_false, &b_action, &assoc_mode);
					assert(res == JVX_NO_ERROR);

					entry = fldStr.std_str();
					HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_DECSCS, &entry);

					entry = fldStr_lab.std_str();
					HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL, &entry);

					entry = fldStr_lab_true.std_str();
					HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_TRUE, &entry);

					entry = fldStr_lab_false.std_str();
					HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_FALSE, &entry);

					entry = jvxComponentIdentification_txt(comType);
					HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_IDENTIFICATION, &entry);

					entry = jvxSequencerElementType_txt(elmType);
					HjvxConfigProcessor_writeEntry_assignmentString(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_TYPES, &entry);

					sz = idxCommandId;
					HjvxConfigProcessor_writeEntry_assignment<jvxSize>(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_COMMAND_IDS, &sz);

					sz = timeoutms;
					HjvxConfigProcessor_writeEntry_assignment<jvxSize>(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_TIMEOUTS, &sz);

					if (b_action)
						sz = 1;
					else
						sz = 0;
					HjvxConfigProcessor_writeEntry_assignment<jvxSize>(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_BREAK_ACTION, &sz);

					HjvxConfigProcessor_writeEntry_assignment<jvxCBitField>(processor, sectionOneStep,
						JVX_CONFIG_ONE_SEQUENCE_STEP_ASSOC_MODE, &assoc_mode);
					processor->addSubsectionToSection(sectionSteps, sectionOneStep);
				} // for (j = 0; j < numStepsRun; j++)
				processor->addSubsectionToSection(sectionOneSeq, sectionSteps);
			} // if (sectionSteps)
		} // else -- if (isDefaultSequence)

		processor->addSubsectionToSection(sectionAll, sectionOneSeq);
	} // for (i = 0; i < numSeq; i++)

	processor->addSubsectionToSection(sectionWhereToAddAllSubsections, sectionAll);
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxSequencer::get_configuration_seq_v1_0(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections, IjvxObject* me_object)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i, j;
	jvxSize num = 0;
	jvxSize numStepsRun = 0;
	jvxSize numStepsLeave = 0;
	jvxConfigData* datTmp = NULL;
	jvxConfigData* datTmp1 = NULL;
	jvxConfigData* datTmp_steps = NULL;
	jvxConfigData* sectionAll = NULL;
	jvxConfigData* sectionStatus = NULL;
	jvxConfigData* commentStr = NULL;
	jvxConfigData* oneSectionStatus = NULL;
	jvxConfigData* oneEntrySectionStatus = NULL;
	jvxBool markedForProcessing = false;

	jvxApiString fldStr;
	jvxApiString fldLabel;
	jvxApiString fldStr_lab;
	jvxApiString fldStr_lab_true;
	jvxApiString fldStr_lab_false;
	std::string entry;
	std::string label;
	jvxInt16 entryInt16;

	jvxSequencerElementType elmType = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	jvxComponentIdentification comType;
	comType.tp = JVX_COMPONENT_UNKNOWN;
	comType.slotid = JVX_SIZE_UNSELECTED;
	comType.slotsubid = JVX_SIZE_UNSELECTED;
	jvxSize idxCommandId = JVX_SIZE_UNSELECTED;
	jvxInt64 timeoutms = -1;
	jvxBool isDefaultSequence = false;
	jvxBool b_action = false;
	std::vector<std::string> entriesStr, entriesStrLab, entriesStrLabTrue, entriesStrLabFalse;
	std::vector<jvxInt16> entriesInt16;
	std::vector<std::string> entriesStringComp;
	std::vector<jvxSize> entriesSizeComm;
	std::vector<jvxInt64> entriesInt64Timeouts;
	std::vector<jvxInt16> entriesInt16BreakAction;
	std::vector<jvxSize> entriesSizeCompSlotids;
	std::vector<jvxSize> entriesSizeCompSlotSubids;

	res = processor->createEmptySection(&sectionAll, JVX_CONFIG_SEQUENCER_SEQ_SECTION);
	res = processor->createEmptySection(&sectionStatus, JVX_CONFIG_SEQUENCER_SEQ_STATUS);

	if (res == JVX_NO_ERROR)
	{
		this->_number_sequences(&num);
		for (i = 0; i < num; i++)
		{
			numStepsRun = 0;
			numStepsLeave = 0;
			markedForProcessing = false;
			isDefaultSequence = false;

			res = this->_description_sequence(i, &fldStr, &fldLabel, &numStepsRun, &numStepsLeave, &markedForProcessing, &isDefaultSequence);
			if (res == JVX_NO_ERROR)
			{
				jvxValue oneVal;
				processor->createEmptySection(&oneSectionStatus, (JVX_CONFIG_ONE_SEQU_SEQ_STATUS_PREFIX + jvx_size2String(i)).c_str());
				processor->createAssignmentString(&oneEntrySectionStatus, "description", fldStr.c_str());
				processor->addSubsectionToSection(oneSectionStatus, oneEntrySectionStatus);

				oneVal.assign(isDefaultSequence);
				processor->createAssignmentValue(&oneEntrySectionStatus, "is_default", oneVal);
				processor->addSubsectionToSection(oneSectionStatus, oneEntrySectionStatus);

				oneVal.assign(markedForProcessing);
				processor->createAssignmentValue(&oneEntrySectionStatus, "is_engaged", oneVal);
				processor->addSubsectionToSection(oneSectionStatus, oneEntrySectionStatus);
				processor->addSubsectionToSection(sectionStatus, oneSectionStatus);

				if (isDefaultSequence)
				{
					processor->createComment(&commentStr, (" Default sequence with name <" + fldStr.std_str() + ">").c_str());
					processor->addSubsectionToSection(sectionAll, commentStr);
				}
				else
				{
					res = processor->createEmptySection(&datTmp, (JVX_CONFIG_SEQUENCER_SEQ_SUBSECTION_PREFIX + jvx_int2String((int)i)).c_str());
					if (res == JVX_NO_ERROR)
					{
						entry = fldStr.std_str();
						label = fldLabel.std_str();
						HjvxConfigProcessor_writeEntry_assignmentString(processor, datTmp, JVX_CONFIG_SEQUENCER_SEQ_NAMES, &entry);
						HjvxConfigProcessor_writeEntry_assignmentString(processor, datTmp, JVX_CONFIG_SEQUENCER_SEQ_LABEL, &label);

						entryInt16 = (jvxInt16)markedForProcessing;
						HjvxConfigProcessor_writeEntry_assignment<jvxInt16>(processor, datTmp, JVX_CONFIG_SEQUENCER_SEQ_MARKED_FLAG, &entryInt16);

						processor->createEmptySection(&datTmp_steps, (JVX_CONFIG_ONE_SEQUENCE_STEPS_PREFIX + (std::string)"run_" + jvx_int2String((int)i)).c_str());
						if (datTmp_steps)
						{
							entriesStr.clear();
							entriesStrLab.clear();
							entriesStrLabTrue.clear();
							entriesStrLabFalse.clear();

							entriesInt16.clear();
							entriesStringComp.clear();
							entriesSizeComm.clear();
							entriesInt64Timeouts.clear();
							entriesSizeCompSlotids.clear();
							entriesSizeCompSlotSubids.clear();
							entriesInt16BreakAction.clear();

							for (j = 0; j < numStepsRun; j++)
							{
								res = this->_description_step_sequence(i, j, JVX_SEQUENCER_QUEUE_TYPE_RUN, &fldStr, &elmType, &comType,
									&idxCommandId, &timeoutms, &fldStr_lab, &fldStr_lab_true, &fldStr_lab_false, &b_action, NULL);
								if (res == JVX_NO_ERROR)
								{
									entriesStr.push_back(fldStr.std_str());
									entriesStrLab.push_back(fldStr_lab.std_str());
									entriesStrLabTrue.push_back(fldStr_lab_true.std_str());
									entriesStrLabFalse.push_back(fldStr_lab_false.std_str());

									entriesInt16.push_back(elmType);
									entriesStringComp.push_back(jvxComponentType_txt(comType.tp));
									entriesSizeCompSlotids.push_back(comType.slotid);
									entriesSizeCompSlotSubids.push_back(comType.slotsubid);
									entriesSizeComm.push_back(idxCommandId);
									entriesInt64Timeouts.push_back(timeoutms);
									entriesInt16BreakAction.push_back(b_action);

								}
								else
								{
									assert(0);
								}
							}
							HjvxConfigProcessor_writeEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_DECSCS, &entriesStr);
							HjvxConfigProcessor_writeEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL, &entriesStrLab);
							HjvxConfigProcessor_writeEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_TRUE, &entriesStrLabTrue);
							HjvxConfigProcessor_writeEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_FALSE, &entriesStrLabFalse);
							HjvxConfigProcessor_writeEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENTS, &entriesStringComp);
							HjvxConfigProcessor_writeEntry_1dList<jvxSize>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_SLOTIDS, entriesSizeCompSlotids);
							HjvxConfigProcessor_writeEntry_1dList<jvxSize>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_SLOTSUBIDS, entriesSizeCompSlotSubids);

							HjvxConfigProcessor_writeEntry_1dList<jvxInt16>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_TYPES, entriesInt16);
							HjvxConfigProcessor_writeEntry_1dList<jvxSize>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMMAND_IDS, entriesSizeComm);
							HjvxConfigProcessor_writeEntry_1dList<jvxInt64>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_TIMEOUTS, entriesInt64Timeouts);
							HjvxConfigProcessor_writeEntry_1dList<jvxInt16>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_BREAK_ACTION, entriesInt16BreakAction);

							// Add subsection
							processor->addSubsectionToSection(datTmp, datTmp_steps);
						}//processor->createEmptySection(&datTmp_steps, ( JVX_CONFIG_ONE_SEQUENCE_STEPS_PREFIX + (std::string)"run_" + jvx_int2String((int)i)).c_str());
						datTmp_steps = NULL;

						processor->createEmptySection(&datTmp_steps, (JVX_CONFIG_ONE_SEQUENCE_STEPS_PREFIX + (std::string)"leave_" + jvx_int2String((int)i)).c_str());
						if (datTmp_steps)
						{
							entriesStr.clear();
							entriesStrLab.clear();
							entriesStrLabTrue.clear();
							entriesStrLabFalse.clear();

							entriesInt16.clear();
							entriesStringComp.clear();
							entriesSizeComm.clear();
							entriesInt64Timeouts.clear();
							entriesInt16BreakAction.clear();
							entriesSizeCompSlotids.clear();
							entriesSizeCompSlotSubids.clear();

							for (j = 0; j < numStepsLeave; j++)
							{
								res = this->_description_step_sequence(i, j, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, &fldStr, &elmType, &comType,
									&idxCommandId, &timeoutms, &fldStr_lab, &fldStr_lab_true, &fldStr_lab_false, &b_action, NULL);
								if (res == JVX_NO_ERROR)
								{
									entriesStr.push_back(fldStr.std_str());
									entriesStrLab.push_back(fldStr_lab.std_str());
									entriesStrLabTrue.push_back(fldStr_lab_true.std_str());
									entriesStrLabFalse.push_back(fldStr_lab_false.std_str());

									entriesInt16.push_back(elmType);
									entriesStringComp.push_back(jvxComponentType_txt(comType.tp));
									entriesSizeCompSlotids.push_back(comType.slotid);
									entriesSizeCompSlotSubids.push_back(comType.slotsubid);
									entriesSizeComm.push_back((jvxInt16)idxCommandId);
									entriesInt64Timeouts.push_back(timeoutms);
									entriesInt16BreakAction.push_back(b_action);
								}
								else
								{
									assert(0);
								}
							}
							HjvxConfigProcessor_writeEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_DECSCS, &entriesStr);
							HjvxConfigProcessor_writeEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL, &entriesStrLab);
							HjvxConfigProcessor_writeEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_TRUE, &entriesStrLabTrue);
							HjvxConfigProcessor_writeEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_LABEL_FALSE, &entriesStrLabFalse);
							HjvxConfigProcessor_writeEntry_assignmentStringList(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENTS, &entriesStringComp);
							HjvxConfigProcessor_writeEntry_1dList<jvxSize>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_SLOTIDS, entriesSizeCompSlotids);
							HjvxConfigProcessor_writeEntry_1dList<jvxSize>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMPONENT_SLOTSUBIDS, entriesSizeCompSlotSubids);

							HjvxConfigProcessor_writeEntry_1dList<jvxInt16>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_TYPES, entriesInt16);
							HjvxConfigProcessor_writeEntry_1dList<jvxSize>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_COMMAND_IDS, entriesSizeComm);
							HjvxConfigProcessor_writeEntry_1dList<jvxInt64>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_TIMEOUTS, entriesInt64Timeouts);
							HjvxConfigProcessor_writeEntry_1dList<jvxInt16>(processor, datTmp_steps, JVX_CONFIG_ONE_SEQUENCE_STEP_BREAK_ACTION, entriesInt16BreakAction);

							// Add subsection
							processor->addSubsectionToSection(datTmp, datTmp_steps);
						}//processor->createEmptySection(&datTmp_steps, ( JVX_CONFIG_ONE_SEQUENCE_STEPS_PREFIX + (std::string)"run_" + jvx_int2String((int)i)).c_str());
						datTmp_steps = NULL;

						processor->addSubsectionToSection(sectionAll, datTmp);
					} // res = processor->createEmptySection(&datTmp, (JVX_CONFIG_SEQUENCER_SEQ_SUBSECTION_PREFIX + jvx_int2String((int)i)).c_str());

				} // if (!isDefaultSequence)

			}// res = this->_description_sequence(i, &fldStr, &numStepsRun, &numStepsLeave, &markedForProcessing);

			datTmp = NULL;

		} // for(i = 0; i < num; i++)

		processor->addSubsectionToSection(sectionAll, sectionStatus);

		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, sectionAll);
	} // res = processor->createEmptySection(&sectionAll, JVX_CONFIG_SEQUENCER_SEQ_SECTION);
	sectionAll = NULL;
	return(JVX_NO_ERROR);
}

// =========================================================================================

jvxErrorType
CjvxSequencer::_get_configuration_seq(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections, IjvxObject* me_object)
{
	switch (_common_set_sequencer.version_cfg_major)
	{
	case 2:
		return get_configuration_seq_v2_0(callConf, processor, sectionWhereToAddAllSubsections, me_object);
	}

	return get_configuration_seq_v1_0(callConf, processor, sectionWhereToAddAllSubsections, me_object);
}


// ==========================================================================================
// Helpers
// ==========================================================================================

std::vector<oneSequence>::iterator
CjvxSequencer::findSequenceNameInList(std::vector<oneSequence>& all_sequences, std::string description)
{
	std::vector<oneSequence>::iterator elm = all_sequences.begin();
	for (; elm != all_sequences.end(); elm++)
	{
		if (elm->description == description)
		{
			break;
		}
	}
	return(elm);
}

std::vector<oneSequence>::iterator
CjvxSequencer::findSequenceIdInList(std::vector<oneSequence>& all_sequences, jvxSize idx)
{
	std::vector<oneSequence>::iterator elm = all_sequences.begin();
	jvxSize cnt = 0;
	for (; elm != all_sequences.end(); elm++, cnt++)
	{
		if (cnt == idx)
		{
			break;
		}
	}
	return(elm);
}

std::vector<oneSequenceStep>::iterator
CjvxSequencer::findSequenceStepIdInList(std::vector<oneSequenceStep>& all_steps, jvxSize idx)
{
	std::vector<oneSequenceStep>::iterator elm = all_steps.begin();
	jvxSize cnt = 0;
	for (; elm != all_steps.end(); elm++, cnt++)
	{
		if (cnt == idx)
		{
			break;
		}
	}
	return(elm);
}

bool
CjvxSequencer::findNextValidStep(jvxSize offset)
{
	jvxSize i;
	bool foundIt = false;
	for (i = offset; i < _common_set_sequencer.theSequences.all_sequences.size(); i++)
	{
		if (_common_set_sequencer.theSequences.all_sequences[i].markedForProcess)
		{
			if (
				(_common_set_sequencer.theSequences.all_sequences[i].lstOfRunningSteps.size() > 0) ||
				(_common_set_sequencer.theSequences.all_sequences[i].lstOfLeaveSteps.size() > 0))
			{
				// First sequencer object
				currentStatusSeqContext newElm;

				newElm.idxSequence = i;
				if (_common_set_sequencer.theSequences.all_sequences[i].lstOfRunningSteps.size() > 0)
				{
					newElm.idxStep = 0;
					newElm.currentQueueTp = JVX_SEQUENCER_QUEUE_TYPE_RUN;
				}
				else
				{
					newElm.idxStep = 0;
					newElm.currentQueueTp = JVX_SEQUENCER_QUEUE_TYPE_LEAVE;
				}

				_common_set_sequencer.operationStack.push_back(newElm);
				auto elm = _common_set_sequencer.operationStack.rbegin();
				
				assert(elm != _common_set_sequencer.operationStack.rend());
				_common_set_sequencer.inOperation = &(*elm);

				foundIt = true;
				break;
			}
		}
	}
	return(foundIt);
}

bool
CjvxSequencer::findNextTask(jvxSequencerQueueType tp, jvxSize id_next_task)
{
	bool foundIt = false;
	auto elm = _common_set_sequencer.immediate_tasks.find(id_next_task);
	if (elm != _common_set_sequencer.immediate_tasks.end())
	{
		jvxSequencerTaskStatus curStat;
		elm->second.task->status(&curStat);
		jvxBool stateForPush = false;
		switch (tp)
		{
		case JVX_SEQUENCER_QUEUE_TYPE_RUN:
			if (curStat.stat == jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_WAITING)
			{
				stateForPush = true;
			}
			break;
		case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
			if (curStat.stat == jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_RUN_COMPLETE)
			{
				stateForPush = true;
			}
			break;
		}

		if(stateForPush)
		{
			addNextTask_core(elm);
			/*
			currentStatusSeqContext newElm;

			// Copy everything from previous element
			newElm = *_common_set_sequencer.inOperation;

			newElm.idxSequence = JVX_SIZE_UNSELECTED;
			newElm.idxStep = elm->first;
			newElm.sequencer_task = elm->second.task;
			elm->second.stack_height_run = _common_set_sequencer.operationStack.size();

			_common_set_sequencer.operationStack.push_back(newElm);
			auto elm = _common_set_sequencer.operationStack.rbegin();

			assert(elm != _common_set_sequencer.operationStack.rend());
			_common_set_sequencer.inOperation = &(*elm);

			_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_IN_OPERATION;
			_common_set_sequencer.inOperation->in_processing_state_on_enter = _common_set_sequencer.in_processing_state;
			*/
			foundIt = true;
		}
	}
	return(foundIt);
}

void
CjvxSequencer::addNextTask_core(std::map<jvxSize, oneTaskStep>::iterator& elm)
{
	currentStatusSeqContext newElm;

	elm->second.task->set_activate(true);

	// Copy everything from previous element
	newElm = *_common_set_sequencer.inOperation;

	newElm.idxSequence = JVX_SIZE_UNSELECTED;
	newElm.idxStep = elm->first;
	newElm.sequencer_task = elm->second.task;
	elm->second.stack_height_run = _common_set_sequencer.operationStack.size();

	_common_set_sequencer.operationStack.push_back(newElm);
	auto elmL = _common_set_sequencer.operationStack.rbegin();

	assert(elmL != _common_set_sequencer.operationStack.rend());
	_common_set_sequencer.inOperation = &(*elmL);

	_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_IN_OPERATION;
	_common_set_sequencer.inOperation->in_processing_state_on_enter = _common_set_sequencer.in_processing_state;
}

bool
CjvxSequencer::findNextSequenceCall(const std::string& label)
{
	jvxSize i;
	bool foundIt = false;
	for (i = 0; i < _common_set_sequencer.theSequences.all_sequences.size(); i++)
	{
		if (_common_set_sequencer.theSequences.all_sequences[i].label == label)
		{
			if (
				(_common_set_sequencer.theSequences.all_sequences[i].lstOfRunningSteps.size() > 0) ||
				(_common_set_sequencer.theSequences.all_sequences[i].lstOfLeaveSteps.size() > 0))
			{
				currentStatusSeqContext newElm;
				
				// Copy everything from previous element
				newElm = *_common_set_sequencer.inOperation;

				newElm.idxSequence = i;
				if (_common_set_sequencer.theSequences.all_sequences[i].lstOfRunningSteps.size() > 0)
				{
					newElm.idxStep = 0;
					newElm.currentQueueTp = JVX_SEQUENCER_QUEUE_TYPE_RUN;
				}
				else
				{
					newElm.idxStep = 0;
					newElm.currentQueueTp = JVX_SEQUENCER_QUEUE_TYPE_LEAVE;
				}

				_common_set_sequencer.operationStack.push_back(newElm);
				auto elm = _common_set_sequencer.operationStack.rbegin();

				assert(elm != _common_set_sequencer.operationStack.rend());
				_common_set_sequencer.inOperation = &(*elm);

				_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_IN_OPERATION;
				_common_set_sequencer.inOperation->in_processing_state_on_enter = _common_set_sequencer.in_processing_state;

				foundIt = true;
				break;
			}
		}
	}
	return(foundIt);
}

bool
CjvxSequencer::findNextSequenceStack()
{
	if (_common_set_sequencer.operationStack.size() > 1)
	{
		IjvxSequencerTask* task = _common_set_sequencer.inOperation->sequencer_task;

		_common_set_sequencer.inOperation = NULL;
		_common_set_sequencer.operationStack.pop_back();

		auto elm = _common_set_sequencer.operationStack.rbegin();
		assert(elm != _common_set_sequencer.operationStack.rend());
		_common_set_sequencer.inOperation = &(*elm);

		if (task)
		{
			auto elmT = _common_set_sequencer.immediate_tasks.begin();
			for (;elmT != _common_set_sequencer.immediate_tasks.end(); elmT++)
			{
				if (elmT->second.task == task)
				{
					elmT->second.task->set_activate(false);
				}
			}
		}
		return true;
	}
	return false;
}

// ==============================================================================

jvxErrorType
CjvxSequencer::_add_sequencer_task(IjvxSequencerTask* theTask, jvxSize* uIdTask)
{
	if (uIdTask)
	{
		jvxSize uId = _common_set_sequencer.uniqueId++;
		auto elm = _common_set_sequencer.immediate_tasks.begin();
		for (; elm != _common_set_sequencer.immediate_tasks.end(); elm++)
		{
			if (elm->second.task == theTask)
			{
				return JVX_ERROR_DUPLICATE_ENTRY;
			}
		}
		*uIdTask = uId;
		oneTaskStep newElm;
		newElm.task = theTask;
		_common_set_sequencer.immediate_tasks[uId] = newElm;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxSequencer::_remove_sequencer_task(jvxSize uIdTask)
{
	auto elm = _common_set_sequencer.immediate_tasks.find(uIdTask);
	if (elm != _common_set_sequencer.immediate_tasks.end())
	{
		_common_set_sequencer.immediate_tasks.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

