#include "jvxHosts/CjvxFullHost.h"

typedef enum
{
	JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP,
	JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP_INCREMENT,
	JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP_LEAVE_LOOP,
	JVX_SEQUENCER_PROCEDE_JUMP_TO_LABEL,
	JVX_SEQUENCER_PROCEDE_CALL_SEQUENCE,
	JVX_SEQUENCER_PROCEDE_RUN_TASK,
	JVX_SEQUENCER_PROCEDE_NEED_TO_WAIT,
	JVX_SEQUENCER_PROCEDE_BREAK,
	JVX_SEQUENCER_PROCEDE_EXIT_ERROR,
	JVX_SEQUENCER_PROCEDE_WANTS_TO_STOP_SEQUENCE,
	JVX_SEQUENCER_PROCEDE_WANTS_TO_STOP_PROCESS,
} whatToDoNextType;

/*
jvxErrorType
CjvxHostBdx1::prepareOneDevice(jvxComponentType tp, IjvxObject* theObj)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxDataProcessorSelector* theSelector = NULL;

	theObj->
	switch(tp)
	{
	case JVX_COMPONENT_SCANNER_TECHNOLOGY:
		res = JVX_ERROR_UNSUPPORTED;
		break;
	case JVX_COMPONENT_SCANNER_DEVICE:
		res = theScanner
		break;
	case JVX_COMPONENT_SPECTRUM_PROCESSOR_TECHNOLOGY:
		res = JVX_ERROR_UNSUPPORTED;
		break;
	case JVX_COMPONENT_SPECTRUM_PROCESSOR_DEVICE:
		res = theSpecProcDev->prepare();
		break;
	case JVX_COMPONENT_LAMP_TECHNOLOGY:
		res = JVX_ERROR_UNSUPPORTED;
		break;
	case JVX_COMPONENT_LAMP_DEVICE:
		res = theLampDev->prepare();
		break;
	case JVX_COMPONENT_CAMERA_TECHNOLOGY:
		res = JVX_ERROR_UNSUPPORTED;
		break;
	case JVX_COMPONENT_CAMERA_DEVICE:
		res = theCameraDev->prepare();
		break;
	case JVX_COMPONENT_ASCAN_ENDPOINT:
		res = theAscanEndpt->prepare();
		break;
	}
	return res;
}
*/
/*
jvxErrorType
CjvxHostBdx1::prepareCompone(IjvxScannerDevice* theDevice)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxObject* theObj_sp = NULL;
	IjvxSpectrumProcessorDevice* theDev_sp = NULL;

	IjvxObject* theObj_as = NULL;
	IjvxAscanEndpoint* theDev_as = NULL;

	theDe
	this->request_object_component(JVX_COMPONENT_SPECTRUM_PROCESSOR_DEVICE, &theObj_sp);
	if(theObj_sp)
	{
		theObj_sp->request_specialization(reinterpret_cast<jvxHandle**>(&theDev_sp), NULL, NULL);
	}

	this->request_object_component(JVX_COMPONENT_ASCAN_ENDPOINT, &theObj_sp);
	if(theObj_sp)
	{
		theObj_sp->request_specialization(reinterpret_cast<jvxHandle**>(&theDev_as), NULL, NULL);
	}

	res = theDevice->prepare(theDev_sp, theDev_as);
	return res;
}
*/

jvxErrorType
CjvxFullHost::sequencer_step(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	oneSequenceStep theStepCopy;
	oneSequenceStep* theStep = NULL;
	jvxUInt64 reportFlag = 0;
	jvxBool regularNextStep = true;
	jvxBool run = true;
	jvxBool searchNext = false;
	IjvxObject* theObject = NULL;
	jvxHandle* hdl = NULL;
	jvxComponentIdentification theType(JVX_COMPONENT_UNKNOWN);
	std::string errorDescription;
	jvxBool reportEndOfProcess = false;
	jvxBool wantImmediateReturn = false;
	jvxBool foundProcess = false;
	jvxBool foundProcessInLoop = false;
	jvxBool needtowaitstep = false;
	jvxSize id_next_task = JVX_SIZE_UNSELECTED;

	if (_common_set_sequencer.in_processing)
	{
		// Re-entering sequencer here. This may happen if specific operations take a longer time
		// We need to leave immediately
		return JVX_ERROR_COMPONENT_BUSY;
	}

	_common_set_sequencer.in_processing = true;
	if(_common_set_sequencer.report)
	{
		if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_DEBUG_MESSAGE)
		{
			_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_DEBUG_MESSAGE,
				"Enter Event Loop", JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, JVX_SEQUENCER_QUEUE_TYPE_NONE,
				JVX_SEQUENCER_TYPE_COMMAND_NONE, JVX_SIZE_UNSELECTED, 
				_common_set_sequencer.in_processing_state);
		}
	}

	// ====================================================================
	// Loop for as many steps as possible
	// ====================================================================
	while(run)
	{
		IjvxTechnology* theTech = NULL;
		IjvxDevice* theDev = NULL;
		IjvxNode* theNode = NULL;

		IjvxSequencerControl* theSeqContr = NULL;
		jvxBool success = false;
		std::string label_next_step ;
		std::string label_next_seq;
		whatToDoNextType whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;

		jvxBool reportEndOfSequence = false;
		jvxSize thePcOffset = 0;
		jvxSize numProcesses = 0;
		IjvxDataConnectionProcess* theProc = NULL;
		jvxBool indicateFirstError = (_common_set_sequencer.operationStack.size() == 1);

		if((!_common_set_sequencer.stepping_active) && (!_common_set_sequencer.stepping_break_active))
		{
			_common_set_sequencer.step_state = JVX_SEQUENCER_STEP_STATE_NONE;
		}

		needtowaitstep = false;
		if (_common_set_sequencer.stepping_active)
		{
			switch (_common_set_sequencer.step_state)
			{
			case JVX_SEQUENCER_STEP_STATE_NONE:
				_common_set_sequencer.step_state = JVX_SEQUENCER_STEP_STATE_ENTERED;
				if (_common_set_sequencer.report)
				{
					if (_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_REPORT_ENTERED_STEP_BREAK)
					{
						_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_DEBUG_MESSAGE,
							"Single Step Break", JVX_SIZE_UNSELECTED,
							JVX_SIZE_UNSELECTED,
							JVX_SEQUENCER_QUEUE_TYPE_NONE,
							JVX_SEQUENCER_TYPE_COMMAND_NONE, 
							JVX_SIZE_UNSELECTED,
							_common_set_sequencer.in_processing_state);
					}
				}
				// no break!!
			case JVX_SEQUENCER_STEP_STATE_ENTERED:

				needtowaitstep = true;
				break;

			case JVX_SEQUENCER_STEP_STATE_RELEASED:
				_common_set_sequencer.step_state = JVX_SEQUENCER_STEP_STATE_NONE;
				break;
			}
		}

		if (needtowaitstep)
		{
			res = JVX_NO_ERROR;
			goto step_leave;
		}

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// Find the pointer to current sequence step
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// If we have completed the sequencer before, do nothing else
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		if(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_SHUTDOWN_COMPLETE)
		{
			break;
		}

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// Check wether processing was aborted from outside
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		if(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_WANTS_TO_STOP_PROCESS)
		{
			whatToDoNext = JVX_SEQUENCER_PROCEDE_WANTS_TO_STOP_PROCESS;
			// State is switched to shutdown lateron
		}
		else if (_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_WANTS_TO_STOP_SEQUENCE)
		{
			if (_common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].markedForProcess)
			{
				// Cancel stop sequence
				_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_IN_OPERATION;
				continue;
			}
			else
			{
				whatToDoNext = JVX_SEQUENCER_PROCEDE_WANTS_TO_STOP_SEQUENCE;
			}
			// State is switched to shutdown lateron
		}
		else
		{			
			auto elm = _common_set_sequencer.immediate_tasks.begin();
			for (; elm != _common_set_sequencer.immediate_tasks.end(); elm++)
			{
				jvxBool f_imm = false;
				elm->second.task->immediate_complete(&f_imm);
				if (f_imm)
				{
					jvxSequencerTaskStatus stat; 
					elm->second.task->status(&stat);

					// Skip all steps in run queue if there are any and procede with leave queue
					if (
						(stat.stat == jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_RUN_ACTIVATED) ||
						(stat.stat == jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_RUN_COMPLETE))
					{
						elm->second.task->switch_queue(JVX_SEQUENCER_QUEUE_TYPE_LEAVE);
					}

					if (stat.stat == jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_RUN_COMPLETE)
					{
						addNextTask_core(elm);
					}
				}
			}

			if (_common_set_sequencer.inOperation->sequencer_task)
			{
				
				jvxSequencerOneStepReturn stepRet;
				jvxApiString astr_label;
				jvxApiString astr_label_true;
				jvxApiString astr_label_false;
				jvxApiString astr_descr;
				stepRet.out_elementType = &theStepCopy.step_tp;
				stepRet.out_targetComponentType = &theStepCopy.step_target;
				stepRet.out_label_name = &astr_label;
				stepRet.out_cond_label_true = &astr_label_true;
				stepRet.out_cond_label_false = &astr_label_false;
				stepRet.out_functionId = &theStepCopy.step_function_id;
				stepRet.out_description = &astr_descr;
				stepRet.out_timeout_ms = &theStepCopy.step_timeout_ms;
				stepRet.out_break_condition = &theStepCopy.break_active;
				stepRet.out_assoc_mode = &theStepCopy.assoc_mode;
				stepRet.out_allow_fail = &theStepCopy.allow_fail;
				resL = _common_set_sequencer.inOperation->sequencer_task->next_step(
					_common_set_sequencer.inOperation->currentQueueTp, &stepRet);
				if (resL == JVX_NO_ERROR)
				{
					theStepCopy.step_cond_label_false = astr_label_false.std_str();
					theStepCopy.step_cond_label_true = astr_label_true.std_str();
					theStepCopy.step_label = astr_label.std_str();
					theStepCopy.description = astr_descr.std_str();
					theStep = &theStepCopy;
				}
				else
				{
					// Recoder the stack and continue
					jvxBool suc = findNextSequenceStack();
					assert(suc);
					// we need to restart the loop to run next operation from stack
					continue;
				}
			}
			else
			{
				if (JVX_CHECK_SIZE_SELECTED(_common_set_sequencer.inOperation->idxSequence))
				{
					if (_common_set_sequencer.inOperation->idxSequence < _common_set_sequencer.theSequences.all_sequences.size())
					{
						switch (_common_set_sequencer.inOperation->currentQueueTp)
						{
						case JVX_SEQUENCER_QUEUE_TYPE_RUN:
							if (_common_set_sequencer.inOperation->idxStep < _common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfRunningSteps.size())
							{
								theStep = &_common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfRunningSteps[_common_set_sequencer.inOperation->idxStep];
							}
							break;
						case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
							if (_common_set_sequencer.inOperation->idxStep < _common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfLeaveSteps.size())
							{
								theStep = &_common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfLeaveSteps[_common_set_sequencer.inOperation->idxStep];
							}
							break;
						}
					}
				}
			}

			if(!theStep)
			{
				whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
				errorDescription = "Failed to determine next step";
				res = JVX_ERROR_UNEXPECTED;
				goto step_leave;
			}

			// Store the current step type for external triggering
			_common_set_sequencer.inOperation->thisStep.step_tp = theStep->step_tp;

			needtowaitstep = false;
			if (_common_set_sequencer.stepping_break_active && theStep->break_active)
			{
				switch (_common_set_sequencer.step_state)
				{
				case JVX_SEQUENCER_STEP_STATE_NONE:
					_common_set_sequencer.step_state = JVX_SEQUENCER_STEP_STATE_ENTERED;
					if (_common_set_sequencer.report)
					{
						if (_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_REPORT_ENTERED_STEP_BREAK)
						{
							_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_DEBUG_MESSAGE,
								"Step Breakpoint", JVX_SIZE_UNSELECTED,
								JVX_SIZE_UNSELECTED,
								JVX_SEQUENCER_QUEUE_TYPE_NONE,
								JVX_SEQUENCER_TYPE_COMMAND_NONE,
								JVX_SIZE_UNSELECTED,
								_common_set_sequencer.in_processing_state);
						}
					}
					// no break!!
				case JVX_SEQUENCER_STEP_STATE_ENTERED:

					needtowaitstep = true;
					break;

				case JVX_SEQUENCER_STEP_STATE_RELEASED:
					_common_set_sequencer.step_state = JVX_SEQUENCER_STEP_STATE_NONE;
					break;
				}
			}

			if (needtowaitstep)
			{
				res = JVX_NO_ERROR;
				goto step_leave;
			}

			if ((theStep->assoc_mode & _common_set_sequencer.run_mode_operate) == 0)
			{
				whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
				if (_common_set_sequencer.report)
				{
					if (_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_REPORT_SKIPPED_STEP)
					{
						errorDescription = "Skipped operation ";
						errorDescription += jvxSequencerElementType_txt(theStep->step_tp);
						errorDescription += " for sequencer step <";
						errorDescription += theStep->description;
						errorDescription += ">, component <";
						errorDescription += jvxComponentIdentification_txt(theStep->step_target);
						errorDescription += "> due to non matching mode selection.";
						_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_REPORT_SKIPPED_STEP,
							errorDescription.c_str(), 
							_common_set_sequencer.inOperation->idxSequence,
							_common_set_sequencer.inOperation->idxStep,
							_common_set_sequencer.inOperation->currentQueueTp,
							theStep->step_tp, theStep->step_function_id,
							_common_set_sequencer.in_processing_state);
					}
				}
				
				goto jump_to_next_step;
			}

			// Set object reference to invalid..
			theObject = NULL;

			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			// Get reference to target object
			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			if(res == JVX_NO_ERROR)
			{
				res = request_object_selected_component(theStep->step_target, &theObject);
				if(!((res == JVX_NO_ERROR) && theObject))
				{
					switch(theStep->step_tp)
					{
					case JVX_SEQUENCER_TYPE_COMMAND_NONE:
					case JVX_SEQUENCER_TYPE_JUMP:
					case JVX_SEQUENCER_TYPE_WAIT_FOREVER:
					case JVX_SEQUENCER_TYPE_WAIT_AND_RUN_TASKS:
					case JVX_SEQUENCER_TYPE_REQUEST_START_IN_PROCESSING_LOOP:
					case JVX_SEQUENCER_TYPE_REQUEST_STOP_IN_PROCESSING_LOOP:
					case JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER:
					case JVX_SEQUENCER_TYPE_CALLBACK:
					case JVX_SEQUENCER_TYPE_BREAK:
					case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE:
					case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START:
					case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_STOP:
					case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS:
					case JVX_SEQUENCER_TYPE_COMMAND_OUTPUT_TEXT:
					case JVX_SEQUENCER_TYPE_COMMAND_CALL_SEQUENCE:
					case JVX_SEQUENCER_TYPE_COMMAND_SWITCH_STATE:
					case JVX_SEQUENCER_TYPE_COMMAND_RESET_STATE:
					case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_TEST:
					case JVX_SEQUENCER_TYPE_COMMAND_INTERCEPT:
					case JVX_SEQUENCER_TYPE_COMMAND_INVOKE_ERROR:
						// This is ok, we do not need an object
						res = JVX_NO_ERROR;
						break;
					default:
						/*
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Failed to get reference to component ";
						errorDescription += jvxComponentType_txt(theStep->step_target);
						res = JVX_ERROR_ELEMENT_NOT_FOUND;
						*/
						if (_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_INFO_TEXT)
						{
							std::string txt = "Skipping step <";
							txt += jvxSequencerElementType_txt(theStep->step_tp);
							txt += "> on component <";
							txt += jvxComponentIdentification_txt(theStep->step_target);
							txt += ">: Failed to get reference to component.";

							_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_INFO_TEXT,
								txt.c_str(), 
								_common_set_sequencer.inOperation->idxSequence, 
								_common_set_sequencer.inOperation->idxStep, 
								_common_set_sequencer.inOperation->currentQueueTp,
								theStep->step_tp, theStep->step_function_id,
								_common_set_sequencer.in_processing_state);
						}
						whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
						res = JVX_NO_ERROR;
						goto jump_to_next_step;
					}
				}
			}

			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			// Get object specialization reference
			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			if(res == JVX_NO_ERROR)
			{
				if(theObject) // Some valid steps require no object pointer...
				{
					hdl = NULL;
					res = theObject->request_specialization(&hdl, &theType, NULL);
					if(!((res == JVX_NO_ERROR) && (hdl)))
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Failed to get specialization of component <";
						errorDescription += jvxComponentIdentification_txt(theStep->step_target) + ">.";
						res = JVX_ERROR_INTERNAL;
						return_object_selected_component(theStep->step_target, theObject);
					}
				}
			}

			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			// Set pointer to correct object reference
			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			if(res == JVX_NO_ERROR)
			{
				if(theObject) // Some valid steps require no object pointer...
				{
					bool foundType = false;
					switch(theStep->step_target.tp)
					{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
						theTech = reinterpret_cast<IjvxTechnology*>(hdl);
						foundType = true;
						break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
						theDev = reinterpret_cast<IjvxDevice*>(hdl);
						foundType = true;
						break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
						theNode = reinterpret_cast<IjvxNode*>(hdl);
						foundType = true;
						break;
					}

					if(!foundType)
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Component type <";
						errorDescription += jvxComponentIdentification_txt(theStep->step_target);
						errorDescription += "> not supported.";
						res = JVX_ERROR_UNSUPPORTED;
						return_object_selected_component(theStep->step_target, theObject);
					}
				}
			}

			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			// Execute the actual command
			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			

			if(res == JVX_NO_ERROR)
			{
				if(_common_set_sequencer.report)
				{
					if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_DEBUG_MESSAGE)
					{
						_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_DEBUG_MESSAGE,
							"Start Execute Command", _common_set_sequencer.inOperation->idxSequence,
							_common_set_sequencer.inOperation->idxStep,
							_common_set_sequencer.inOperation->currentQueueTp,
							theStep->step_tp, theStep->step_function_id,
							_common_set_sequencer.in_processing_state);
					}
				}

				// There are few steps that are foreseen for "Planned Stop". all others are denoted as "Abort". Decision for type of 
				// completion is always if stop is triggered in run queue 
				if (_common_set_sequencer.inOperation->currentQueueTp == JVX_SEQUENCER_QUEUE_TYPE_RUN)
				{
					_common_set_sequencer.inOperation->howtosaygoodbye = JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT;
				}

				if (theStep->step_tp != JVX_SEQUENCER_TYPE_COMMAND_INTERCEPT)
				{
					_common_set_sequencer.intercept_count = JVX_SIZE_UNSELECTED;
				}

				switch (theStep->step_tp)
				{
				case JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_ACTIVATE:

					switch (theStep->step_target.tp)
					{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
						res = theTech->activate();
						break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
						res = theDev->activate();
						break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
						res = theNode->activate();
						break;
					}
					if (res == JVX_NO_ERROR)
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					}
					else
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Operation <activate> failed for component type <";
						errorDescription += jvxComponentIdentification_txt(theStep->step_target);
						errorDescription += ">; reason: ";
						errorDescription += jvxErrorType_descr(res);
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					}

					break;

				case JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE:

					switch (theStep->step_target.tp)
					{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
						res = theTech->prepare();
						break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
						res = theDev->prepare();
						break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
						res = theNode->prepare();
						break;
					}
					if (res == JVX_NO_ERROR)
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					}
					else
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Operation <prepare> failed for component type <";
						errorDescription += jvxComponentIdentification_txt(theStep->step_target);
						errorDescription += ">, reason: ";
						errorDescription += jvxErrorType_descr(res);
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					}

					break;

				case JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_START:

					switch (theStep->step_target.tp)
					{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
						res = theTech->start();
						break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
						res = theDev->start();
						break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
						res = theNode->start();
						break;
					}

					if (res == JVX_NO_ERROR)
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					}
					else
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Operation <start> failed for component type <";
						errorDescription += jvxComponentIdentification_txt(theStep->step_target);
						errorDescription += ">, reason: ";
						errorDescription += jvxErrorType_descr(res);
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					}
					break;

				case JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_STOP:

					switch (theStep->step_target.tp)
					{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
						res = theTech->stop();
						break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
						res = theDev->stop();
						break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
						res = theNode->stop();
						break;
					}
					if (res == JVX_NO_ERROR)
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					}
					else
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Operation <stop> failed for component type <";
						errorDescription += jvxComponentIdentification_txt(theStep->step_target);
						errorDescription += ">, reason: ";
						errorDescription += jvxErrorType_descr(res);
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					}
					break;

				case JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_POSTPROCESS:

					switch (theStep->step_target.tp)
					{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
						res = theTech->postprocess();
						break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
						res = theDev->postprocess();
						break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
						res = theNode->postprocess();
						break;
					}
					if (res == JVX_NO_ERROR)
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					}
					else
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Operation <postprocess> failed for component type <";
						errorDescription += jvxComponentIdentification_txt(theStep->step_target);
						errorDescription += ">, reason: ";
						errorDescription += jvxErrorType_descr(res);
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					}
					break;

				case JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_DEACTIVATE:

					switch (theStep->step_target.tp)
					{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
						res = theTech->deactivate();
						break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
						res = theDev->deactivate();
						break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
						res = theNode->deactivate();
						break;
					}
					if (res == JVX_NO_ERROR)
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					}
					else
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Operation <deactivate> failed for component type <";
						errorDescription += jvxComponentIdentification_txt(theStep->step_target);
						errorDescription += ">, reason: ";
						errorDescription += jvxErrorType_descr(res);
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					}
					break;

				case JVX_SEQUENCER_TYPE_CONDITION_WAIT:
				case JVX_SEQUENCER_TYPE_UNCONDITION_WAIT:
				case JVX_SEQUENCER_TYPE_CONDITION_JUMP:
				case JVX_SEQUENCER_TYPE_COMMAND_SPECIFIC:


					switch (theStep->step_target.tp)
					{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
						res = theTech->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theSeqContr));
						break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
						res = theDev->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theSeqContr));
						break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
						res = theNode->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theSeqContr));
						break;
					}

					if ((res == JVX_NO_ERROR) && theSeqContr)
					{
						if (theStep->step_tp == JVX_SEQUENCER_TYPE_CONDITION_WAIT)
						{
							res = theSeqContr->status_condition(theStep->step_function_id, &success, &_common_set_sequencer.in_processing_state);
							if (res == JVX_NO_ERROR)
							{
								if (success == true)
								{
									whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
								}
								else
								{
									whatToDoNext = JVX_SEQUENCER_PROCEDE_NEED_TO_WAIT;
									// Show that we need to continue waiting
								}
							}
							else
							{
								whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
								if (res != JVX_ERROR_ELEMENT_NOT_FOUND)
								{
									jvxApiString descr;
									theSeqContr->description_condition(theStep->step_function_id, NULL, &descr, NULL);
									errorDescription = "Wait for condition <" + descr.std_str() + "> failed for component <";
								}
								else
								{
									errorDescription = "Wait for condition in step <" + theStep->description + "> failed for component <";
								}
								errorDescription += jvxComponentIdentification_txt(theStep->step_target);
								errorDescription += ">, reason: ";
								errorDescription += jvxErrorType_descr(res);
							}
						}
						else if (theStep->step_tp == JVX_SEQUENCER_TYPE_UNCONDITION_WAIT)
						{
							whatToDoNext = JVX_SEQUENCER_PROCEDE_NEED_TO_WAIT;
						}
						else if (theStep->step_tp == JVX_SEQUENCER_TYPE_CONDITION_JUMP)
						{
							res = theSeqContr->status_condition(theStep->step_function_id, &success, &_common_set_sequencer.in_processing_state);
							if (res == JVX_NO_ERROR)
							{
								if (success == true)
								{
									label_next_step = theStep->step_cond_label_true;
								}
								else
								{
									label_next_step = theStep->step_cond_label_false;
								}
								whatToDoNext = JVX_SEQUENCER_PROCEDE_JUMP_TO_LABEL;
							}
							else
							{
								whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
								if (res != JVX_ERROR_ELEMENT_NOT_FOUND)
								{
									jvxApiString descr;
									theSeqContr->description_condition(theStep->step_function_id, NULL, &descr, NULL);
									errorDescription = "Conditional jump <" + descr.std_str() + "> failed for component <";
								}
								else
								{
									errorDescription = "Conditional jump in step <" + theStep->description + "> failed for component <";
								}
								errorDescription += jvxComponentIdentification_txt(theStep->step_target);
								errorDescription += ">, reason: ";
								errorDescription += jvxErrorType_descr(res);
							}
						}
						else if (theStep->step_tp == JVX_SEQUENCER_TYPE_COMMAND_SPECIFIC)
						{
							res = theSeqContr->trigger_command(theStep->step_function_id, &_common_set_sequencer.in_processing_state);
							if (res == JVX_NO_ERROR)
							{
								whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
							}
							else
							{
								if (res == JVX_ERROR_POSTPONE)
								{
									whatToDoNext = JVX_SEQUENCER_PROCEDE_NEED_TO_WAIT;
									res = JVX_NO_ERROR;
								}
								else
								{
									whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
									if (res != JVX_ERROR_ELEMENT_NOT_FOUND)
									{
										jvxApiString descr;
										theSeqContr->description_command(theStep->step_function_id, NULL, &descr, NULL);
										errorDescription = "Command <" + descr.std_str() + "> failed for component <";
									}
									else
									{
										errorDescription = "Command in step <" + theStep->description + "> failed for component <";
									}
									errorDescription += jvxComponentIdentification_txt(theStep->step_target);
									errorDescription += ">, reason: ";
									errorDescription += jvxErrorType_descr(res);
								}
							}
						}
					}
					else
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Request to receive sequencer control failed for component <";
						errorDescription += jvxComponentIdentification_txt(theStep->step_target);
						errorDescription += ">, reason: ";
						errorDescription += jvxErrorType_descr(res);
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					}
					break;

				case JVX_SEQUENCER_TYPE_JUMP:
					label_next_step = theStep->step_cond_label_true;
					whatToDoNext = JVX_SEQUENCER_PROCEDE_JUMP_TO_LABEL;
					break;

				case JVX_SEQUENCER_TYPE_WAIT_FOREVER:
					// Just do nothing else than return
					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEED_TO_WAIT;
					_common_set_sequencer.inOperation->howtosaygoodbye = JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE;
					// JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT; <- CHECK THIS HK
					break;

				case JVX_SEQUENCER_TYPE_WAIT_AND_RUN_TASKS:
				{
					auto elm = _common_set_sequencer.immediate_tasks.begin();
					for (; elm != _common_set_sequencer.immediate_tasks.end(); elm++)
					{
						jvxSequencerTaskStatus statTask;
						elm->second.task->status(&statTask);
						if (statTask.stat == jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_WAITING)
						{
							id_next_task = elm->first;
							break;
						}
					}
					if (elm != _common_set_sequencer.immediate_tasks.end())
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_RUN_TASK;
					}
					else
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_NEED_TO_WAIT;
						_common_set_sequencer.inOperation->howtosaygoodbye = JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE;
					}
				}
				// JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT; <- CHECK THIS HK
				break;

				case JVX_SEQUENCER_TYPE_BREAK:

					// Just do nothing else than return
					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEED_TO_WAIT;
					_common_set_sequencer.inOperation->howtosaygoodbye = JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE;
					break;

				case JVX_SEQUENCER_TYPE_REQUEST_START_IN_PROCESSING_LOOP:
				case JVX_SEQUENCER_TYPE_REQUEST_STOP_IN_PROCESSING_LOOP:

					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP_LEAVE_LOOP;
					_common_set_sequencer.inOperation->howtosaygoodbye = JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE;
					break;

				case JVX_SEQUENCER_TYPE_WAIT_CONDITION_RELATIVE_JUMP:

					_common_set_sequencer.inOperation->howtosaygoodbye = JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE;
					switch (theStep->step_target.tp)
					{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
						res = theTech->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theSeqContr));
						break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
						res = theDev->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theSeqContr));
						break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
						res = theNode->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theSeqContr));
						break;
					}
					if ((res == JVX_NO_ERROR) && theSeqContr)
					{
						res = theSeqContr->trigger_relative_jump(theStep->step_function_id, &thePcOffset, &_common_set_sequencer.in_processing_state);
						if ((res == JVX_ERROR_POSTPONE) || ((res == JVX_NO_ERROR) && (thePcOffset == 0)))
						{
							whatToDoNext = JVX_SEQUENCER_PROCEDE_NEED_TO_WAIT;
							res = JVX_NO_ERROR;
						}
						else
						{
							if (res == JVX_NO_ERROR)
							{
								whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP_INCREMENT;
							}
							else
							{
								whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
								if (res != JVX_ERROR_ELEMENT_NOT_FOUND)
								{
									jvxApiString descr;
									theSeqContr->description_relative_jump(theStep->step_function_id, NULL, &descr, NULL);
									errorDescription = "Relative jump <" + descr.std_str() + "> failed for component <";
								}
								else
								{
									errorDescription = "Relative jump in step <" + theStep->description + "> failed for component <";
								}
								errorDescription += jvxComponentIdentification_txt(theStep->step_target);
								errorDescription += ">, reason: ";
								errorDescription += jvxErrorType_descr(res);
								res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
							}
						}
					}
					else
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Request to receive sequencer control failed for component <";
						errorDescription += jvxComponentIdentification_txt(theStep->step_target);
						errorDescription += ">, reason: ";
						errorDescription += jvxErrorType_descr(res);
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					}
					break;

				case JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER:

					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					break;
				case JVX_SEQUENCER_TYPE_CALLBACK:

					resL = JVX_NO_ERROR;
					if (_common_set_sequencer.report)
					{
						resL = _common_set_sequencer.report->sequencer_callback(_common_set_sequencer.inOperation->idxSequence,
							_common_set_sequencer.inOperation->idxStep, _common_set_sequencer.inOperation->currentQueueTp,
							theStep->step_function_id);
					}
					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;

					if (res != JVX_NO_ERROR)
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Sequencer callback failed for function id <" + jvx_size2String(theStep->step_function_id) + "> due to following reason: " + jvxErrorType_descr(res);
					}

					break;

				case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_TEST:

					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					_number_connections_process(&numProcesses);
					foundProcess = false;
					for (i = 0; i < numProcesses; i++)
					{
						resL = reference_connection_process(i, &theProc);
						assert(resL == JVX_NO_ERROR);
						if (theProc)
						{
							jvxBool processMatches = false;
							jvxState statC = JVX_STATE_NONE;
							theProc->status(&statC);

							if (statC == JVX_STATE_ACTIVE)
							{
								theProc->match_master(&processMatches, theStep->step_target, theStep->step_cond_label_true.c_str());
								if (processMatches)
								{
									foundProcess = true;
									// Always run a final test before the prepare
									JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdbt);
									/*
									res = theProc->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A(fdbt));
									if (res != JVX_NO_ERROR)
									{
										whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
										resL = return_reference_connection_process(theProc);
										assert(resL == JVX_NO_ERROR);
										break;
									}
									else
									{*/
									JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdbp);
									res = theProc->test_chain(false JVX_CONNECTION_FEEDBACK_CALL_A(fdbp));
									if (res != JVX_NO_ERROR)
									{
										whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
										resL = return_reference_connection_process(theProc);
										assert(resL == JVX_NO_ERROR);
										break;
									}
									//}
								}
							}
							resL = return_reference_connection_process(theProc);
							assert(resL == JVX_NO_ERROR);
						}
					}
					if (foundProcess == false)
					{
						errorDescription = "Warning in Chain Test: a process chain matching <" + 
							jvxComponentIdentification_txt(theStep->step_target) + ">, and expression <" +
							theStep->step_cond_label_true + "> could not be matched.";
						_report_text_message(errorDescription.c_str(), JVX_REPORT_PRIORITY_WARNING);
					}
					break;
				case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE:

					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					_number_connections_process(&numProcesses);
					foundProcess = false;
					for (i = 0; i < numProcesses; i++)
					{
						resL = reference_connection_process(i, &theProc);
						assert(resL == JVX_NO_ERROR);
						if (theProc)
						{
							jvxSize theControlUid = JVX_SIZE_UNSELECTED;
							theProc->depends_on_process(&theControlUid);

							// Prepare only process that do not depend on another process
							if (JVX_CHECK_SIZE_UNSELECTED(theControlUid))
							{
								jvxBool processMatches = false;
								jvxState statC = JVX_STATE_NONE;
								jvxBool testC = false;

								/* Run the "test" right before the actual prepare*/
								JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdbtt);
								//theProc->test_chain(false, fdbtt);

								theProc->status(&statC);
								theProc->status_chain(&testC JVX_CONNECTION_FEEDBACK_CALL_A_NULL);

								if (statC == JVX_STATE_ACTIVE && testC)
								{

									theProc->match_master(&processMatches, theStep->step_target, theStep->step_cond_label_true.c_str());
									if (processMatches)
									{
										foundProcess = true;
										// Always run a final test before the prepare
										JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdbt);
										/*
										res = theProc->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A(fdbt));
										if (res != JVX_NO_ERROR)
										{
											whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
											resL = return_reference_connection_process(theProc);
											assert(resL == JVX_NO_ERROR);
											break;
										}
										else
										{*/
										JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdbp);
										res = theProc->prepare_chain(JVX_CONNECTION_FEEDBACK_CALL(fdbp));
										if (res != JVX_NO_ERROR)
										{
											errorDescription = "Function <prepare_chain>, operation <";
											errorDescription += theStep->description;
											errorDescription += "> failed, reason: ";
											errorDescription += jvxErrorType_txt(res);
											errorDescription += ".";
											whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
											resL = return_reference_connection_process(theProc);
											assert(resL == JVX_NO_ERROR);
											break;
										}
										//}
									}

								}
								else
								{
									if (!testC)
									{
										std::cout << "Function <prepare_chain>, operation <" << theStep->description
											<< ">: Tried to prepare chain but latest test failed" << std::endl;
									}
								}
							}
							resL = return_reference_connection_process(theProc);
							assert(resL == JVX_NO_ERROR);
						}
					}
					if (foundProcess == false)
					{
						errorDescription = "Warning in Chain Prepare: a process chain matching <" + 
							jvxComponentIdentification_txt(theStep->step_target) + ">, and expression <" +
							theStep->step_cond_label_true + "> could not be matched.";
						_report_text_message(errorDescription.c_str(), JVX_REPORT_PRIORITY_WARNING);
					}
					break;
				case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START:
					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					_number_connections_process(&numProcesses);
					foundProcess = false;
					for (i = 0; i < numProcesses; i++)
					{
						resL = reference_connection_process(i, &theProc);
						assert(resL == JVX_NO_ERROR);
						if (theProc)
						{
							jvxSize theControlUid = JVX_SIZE_UNSELECTED;
							theProc->depends_on_process(&theControlUid);

							// Prepare only process that do not depend on another process
							if (JVX_CHECK_SIZE_UNSELECTED(theControlUid))
							{
								jvxBool processMatches = false;
								jvxState statC = JVX_STATE_NONE;
								jvxBool testC = false;
								theProc->status(&statC);
								theProc->status_chain(&testC JVX_CONNECTION_FEEDBACK_CALL_A_NULL);

								if (statC == JVX_STATE_PREPARED && testC)
								{
									theProc->match_master(&processMatches, theStep->step_target, theStep->step_cond_label_true.c_str());
									if (processMatches)
									{
										foundProcess = true;
										JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
										res = theProc->start_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
										if (res != JVX_NO_ERROR)
										{
											whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
											resL = return_reference_connection_process(theProc);
											assert(resL == JVX_NO_ERROR);
											break;
										}
									}
								}
							}
							resL = return_reference_connection_process(theProc);
							assert(resL == JVX_NO_ERROR);
						}
					}
					if (foundProcess == false)
					{
						errorDescription = "Warning in Chain Start: a process chain matching <" + 
							jvxComponentIdentification_txt(theStep->step_target) + ">, and expression <" +
							theStep->step_cond_label_true + "> could not be matched.";
						_report_text_message(errorDescription.c_str(), JVX_REPORT_PRIORITY_WARNING);
					}
					break;

				case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_STOP:
					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					foundProcess = false;
					foundProcessInLoop = true;

					// We need two loops here: with every "stop" we may have stopped another process
					while (foundProcessInLoop)
					{
						foundProcessInLoop = false;
						_number_connections_process(&numProcesses);

						for (i = 0; i < numProcesses; i++)
						{
							resL = reference_connection_process(i, &theProc);
							assert(resL == JVX_NO_ERROR);
							if (theProc)
							{
								jvxSize theControlUid = JVX_SIZE_UNSELECTED;
								theProc->depends_on_process(&theControlUid);

								// Prepare only process that do not depend on another process
								if (JVX_CHECK_SIZE_UNSELECTED(theControlUid))
								{
									jvxBool processMatches = false;
									jvxState statC = JVX_STATE_NONE;
									jvxBool testC = false;
									theProc->status(&statC);
									theProc->status_chain(&testC JVX_CONNECTION_FEEDBACK_CALL_A_NULL);

									if (statC == JVX_STATE_PROCESSING && testC)
									{
										theProc->match_master(&processMatches, theStep->step_target, theStep->step_cond_label_true.c_str());
										if (processMatches)
										{
											foundProcess = true;
											JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
											res = theProc->stop_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
											if (res == JVX_NO_ERROR)
											{
												foundProcessInLoop = true;
											}
											else
											{
												whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;												
											}
											resL = return_reference_connection_process(theProc);
											assert(resL == JVX_NO_ERROR);
											break;
										}
									}
								}

								// Need to break on every postprocess since the number of processes may have changed!
								resL = return_reference_connection_process(theProc);
								assert(resL == JVX_NO_ERROR);
							}
						}
					}
					if (foundProcess == false)
					{
						errorDescription = "Warning in Chain Stop: a process chain matching <" + 
							jvxComponentIdentification_txt(theStep->step_target) + ">, and expression <" +
							theStep->step_cond_label_true + "> could not be matched.";
						_report_text_message(errorDescription.c_str(), JVX_REPORT_PRIORITY_WARNING);
					}
					break;

				case JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS:
					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					foundProcess = false;
					foundProcessInLoop = true;

					// We need two loops here: with every "postprocess" we may have stopped another process
					while (foundProcessInLoop)
					{
						foundProcessInLoop = false;
						_number_connections_process(&numProcesses);
						for (i = 0; i < numProcesses; i++)
						{
							resL = reference_connection_process(i, &theProc);
							assert(resL == JVX_NO_ERROR);
							if (theProc)
							{
								jvxSize theControlUid = JVX_SIZE_UNSELECTED;
								theProc->depends_on_process(&theControlUid);

								// Prepare only process that do not depend on another process
								if (JVX_CHECK_SIZE_UNSELECTED(theControlUid))
								{
									jvxBool processMatches = false;
									jvxState statC = JVX_STATE_NONE;
									jvxBool testC = false;
									theProc->status(&statC);
									theProc->status_chain(&testC JVX_CONNECTION_FEEDBACK_CALL_A_NULL);

									if (statC == JVX_STATE_PREPARED && testC)
									{
										theProc->match_master(&processMatches, theStep->step_target, theStep->step_cond_label_true.c_str());
										if (processMatches)
										{
											foundProcess = true;
											JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
											res = theProc->postprocess_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
											if (res == JVX_NO_ERROR)
											{
												foundProcessInLoop = true;
											}
											else
											{
												whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
											}

											// Need to break on every postprocess since the number of processes may have changed!
											resL = return_reference_connection_process(theProc);
											assert(resL == JVX_NO_ERROR);
											break;
										}
									}
								}
								resL = return_reference_connection_process(theProc);
								assert(resL == JVX_NO_ERROR);
							}
						}
					}
					if (foundProcess == false)
					{
						errorDescription = "Warning in Chain Postprocess: a process chain matching <" + 
							jvxComponentIdentification_txt(theStep->step_target) + ">, and expression <" +
							theStep->step_cond_label_true + "> could not be matched.";
						_report_text_message(errorDescription.c_str(), JVX_REPORT_PRIORITY_WARNING);
					}
					break;
				case JVX_SEQUENCER_TYPE_COMMAND_OUTPUT_TEXT:
					_report_text_message(theStep->step_cond_label_true.c_str(), JVX_REPORT_PRIORITY_NONE);
					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					break;
				case JVX_SEQUENCER_TYPE_COMMAND_NONE:
					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					break;
				case JVX_SEQUENCER_TYPE_COMMAND_CALL_SEQUENCE:
					whatToDoNext = JVX_SEQUENCER_PROCEDE_CALL_SEQUENCE;
					label_next_seq = theStep->step_cond_label_true;

					break;
				case JVX_SEQUENCER_TYPE_COMMAND_SWITCH_STATE:
					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					if (JVX_CHECK_SIZE_STATE_INCREMENT(theStep->step_function_id))
					{
						_common_set_sequencer.in_processing_state++;
					}
					else if (JVX_CHECK_SIZE_STATE_DECREMENT(theStep->step_function_id))
					{
						_common_set_sequencer.in_processing_state++;
					}
					else
					{
						_common_set_sequencer.in_processing_state = theStep->step_function_id;
					}
					break;

				case JVX_SEQUENCER_TYPE_COMMAND_INTERCEPT:
					
					if (JVX_CHECK_SIZE_UNSELECTED(_common_set_sequencer.intercept_count))
					{
						_common_set_sequencer.intercept_count = 1;
					}
					if (_common_set_sequencer.intercept_count == 0)
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
						_common_set_sequencer.intercept_count = JVX_SIZE_UNSELECTED;
					}
					else
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_NEED_TO_WAIT;
						_common_set_sequencer.intercept_count--;
					}
					break;

				case JVX_SEQUENCER_TYPE_COMMAND_RESET_STATE:
					whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					_common_set_sequencer.in_processing_state = _common_set_sequencer.inOperation->in_processing_state_on_enter;
					break;

				case JVX_SEQUENCER_TYPE_COMMAND_SET_PROPERTY:

					// Not yet implemented!!
					assert(0);
					break;

				case JVX_SEQUENCER_TYPE_COMMAND_INVOKE_ERROR:
					whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
					errorDescription = "Error Invoked, reason: <";
					errorDescription += theStep->step_cond_label_true;
					errorDescription += ">.";
					res = JVX_ERROR_INTERNAL;
					break;

				case JVX_SEQUENCER_TYPE_COMMAND_INVOKE_FATAL_ERROR:

					whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
					errorDescription = "Error Invoked, reason: <";
					errorDescription += theStep->step_cond_label_true;
					errorDescription += ">.";
					_common_set_sequencer.inOperation->isErrorShutdown = true;
					indicateFirstError = true;
					res = JVX_ERROR_INTERNAL;
					break;

				default:

					whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
					errorDescription = "Unsupported sequencer operation ";
					errorDescription += jvxSequencerElementType_txt(theStep->step_tp);
					errorDescription += " for component <";
					errorDescription += jvxComponentIdentification_txt(theStep->step_target);
					errorDescription += ">, reason: ";
					errorDescription += jvxErrorType_descr(res);
					res = JVX_ERROR_UNSUPPORTED;
				}//switch(theStep->step_tp)

				// Return the object
				hdl = NULL;
				return_object_selected_component(theStep->step_target, theObject);

				if(_common_set_sequencer.report)
				{
					if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_DEBUG_MESSAGE)
					{
						_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_DEBUG_MESSAGE,
							"Stop Execute Command", _common_set_sequencer.inOperation->idxSequence,
							_common_set_sequencer.inOperation->idxStep,
							_common_set_sequencer.inOperation->currentQueueTp,
							theStep->step_tp, theStep->step_function_id,
							_common_set_sequencer.in_processing_state);
					}
				}
			}//if(res == JVX_NO_ERROR)

			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			// Check for timeout
			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			if(whatToDoNext == JVX_SEQUENCER_PROCEDE_NEED_TO_WAIT)
			{
				jvxInt64 deltaT = (timestamp_us - _common_set_sequencer.inOperation->timestamp_store) / 1000;
				if(theStep->step_timeout_ms > 0)
				{
					if (theStep->step_tp == JVX_SEQUENCER_TYPE_UNCONDITION_WAIT)
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP;
					}
					else
					{
						if (deltaT > theStep->step_timeout_ms)
						{
							res = JVX_ERROR_TIMEOUT;
							whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
							errorDescription = "Timeout in operation ";
							errorDescription += jvxSequencerElementType_txt(theStep->step_tp);
							errorDescription += " for sequencer step <";
							errorDescription += theStep->description;
							errorDescription += ">, component <";
							errorDescription += jvxComponentIdentification_txt(theStep->step_target);
							errorDescription += ">.";
						}
					}
				}
				else
				{
					if (_common_set_sequencer.inOperation->currentQueueTp == JVX_SEQUENCER_QUEUE_TYPE_LEAVE)
					{
						if (deltaT > _common_set_sequencer.default_step_timeout_on_leave_ms)
						{
							res = JVX_ERROR_TIMEOUT;
							whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
							errorDescription = "Timeout in operation ";
							errorDescription += jvxSequencerElementType_txt(theStep->step_tp);
							errorDescription += " for sequencer step <";
							errorDescription += theStep->description;
							errorDescription += ">, component <";
							errorDescription += jvxComponentIdentification_txt(theStep->step_target);
							errorDescription += ">.";
						}
					}
				}
			}
			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			// Next, report what happened if desired
			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

			if(res != JVX_NO_ERROR)
			{
				if (_common_set_sequencer.inOperation->sequencer_task)
				{
					_common_set_sequencer.inOperation->sequencer_task->report_error(res, errorDescription.c_str());
				}
				else
				{
					if(_common_set_sequencer.report)
					{
						if (_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR)
						{
							_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR,
								errorDescription.c_str(), _common_set_sequencer.inOperation->idxSequence,
								_common_set_sequencer.inOperation->idxStep,
								_common_set_sequencer.inOperation->currentQueueTp,
								theStep->step_tp, theStep->step_function_id,
								_common_set_sequencer.in_processing_state,
								indicateFirstError);
						}
					}
				}
			}
			else
			{
				if(whatToDoNext != JVX_SEQUENCER_PROCEDE_NEED_TO_WAIT)
				{
					if(_common_set_sequencer.report)
					{
						if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP)
						{
							_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP,
								theStep->description.c_str(),
								_common_set_sequencer.inOperation->idxSequence,
								_common_set_sequencer.inOperation->idxStep,
								_common_set_sequencer.inOperation->currentQueueTp,
								theStep->step_tp, theStep->step_function_id,
								_common_set_sequencer.in_processing_state,
								(_common_set_sequencer.operationStack.size() == 1));
						}
					}
				}
			}
		}

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// Find the next valid step or terminate
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
jump_to_next_step:
		bool runInner = true;
		while(runInner)
		{
			switch(whatToDoNext)
			{
			case JVX_SEQUENCER_PROCEDE_NEED_TO_WAIT:

				_common_set_sequencer.granularity_report_state_cnt++;
				if (_common_set_sequencer.granularity_report_state_cnt == _common_set_sequencer.granularity_report_state)
				{
					if (_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_REPORT_OPERATION_STATE)
					{
						_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_REPORT_OPERATION_STATE,
							"Report State while waiting", _common_set_sequencer.inOperation->idxSequence,
							_common_set_sequencer.inOperation->idxStep,
							_common_set_sequencer.inOperation->currentQueueTp,
							theStep->step_tp, theStep->step_function_id,
							_common_set_sequencer.in_processing_state,
							(_common_set_sequencer.operationStack.size() == 1));
					}
					_common_set_sequencer.granularity_report_state_cnt = 0;
				}
				// If we need to wait, stop this loop
				runInner = false;
				run = false;
				break;

			case JVX_SEQUENCER_PROCEDE_EXIT_ERROR:

				_common_set_sequencer.granularity_report_state_cnt = 0;

				if (_common_set_sequencer.inOperation->sequencer_task)
				{
					// In immediate task, only switch to leave queue. If error in leave queue, continue
					if (!theStep->allow_fail)
					{
						_common_set_sequencer.inOperation->sequencer_task->switch_queue(JVX_SEQUENCER_QUEUE_TYPE_LEAVE);
					}
					runInner = false;
					res = JVX_NO_ERROR;
				}
				else
				{
					// If we see this, we need to cancel the run queue and switch to leave quque
					// If there is no more element in leave queue, report end of everything
					if (_common_set_sequencer.inOperation->currentQueueTp == JVX_SEQUENCER_QUEUE_TYPE_RUN)
					{
						_common_set_sequencer.run_mode_save = _common_set_sequencer.run_mode_operate;
						_common_set_sequencer.run_mode_operate = JVX_SEQUENCER_MODE_ERROR;

						// Switch to leave queue
						_common_set_sequencer.inOperation->currentQueueTp = JVX_SEQUENCER_QUEUE_TYPE_LEAVE;

						// Set the seqeuner into error mode
						_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_ERROR;

						// Start proceding with LEAVE queue
						if (_common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfLeaveSteps.size() > 0)
						{
							_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
							_common_set_sequencer.inOperation->idxStep = 0;
							res = JVX_NO_ERROR;
						}
						else
						{
							reportEndOfSequence = true;
							reportEndOfProcess = true;
							run = false;
						}
						runInner = false;
					}
					else
					{
						// Move sequence counter forward
						_common_set_sequencer.inOperation->idxStep++;

						// If the previous one was not the last one, procede with next step - even in case of error
						if (_common_set_sequencer.inOperation->idxStep < _common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfLeaveSteps.size())
						{
							_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
							res = JVX_NO_ERROR;
						}
						else
						{
							reportEndOfSequence = true;
							reportEndOfProcess = true;
							run = false;
						}

						res = JVX_NO_ERROR;
						runInner = false;
					}
				}
				break;

			case JVX_SEQUENCER_PROCEDE_WANTS_TO_STOP_PROCESS:

				_common_set_sequencer.granularity_report_state_cnt = 0;
				_common_set_sequencer.inOperation->isFullShutdown = true;

				// brea not missing, fall through desired!
			case JVX_SEQUENCER_PROCEDE_WANTS_TO_STOP_SEQUENCE:

				_common_set_sequencer.granularity_report_state_cnt = 0;
				_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_SHUTDOWN_IN_PROGRESS;
				if(_common_set_sequencer.inOperation->currentQueueTp == JVX_SEQUENCER_QUEUE_TYPE_RUN)
				{
					_common_set_sequencer.inOperation->currentQueueTp = JVX_SEQUENCER_QUEUE_TYPE_LEAVE;

					if(_common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfLeaveSteps.size() > 0)
					{
						_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
						_common_set_sequencer.inOperation->idxStep = 0;

						jvxSize stack_depth = _common_set_sequencer.operationStack.size();
						auto elm = _common_set_sequencer.immediate_tasks.begin();
						for (; elm != _common_set_sequencer.immediate_tasks.end(); elm++)
						{
							jvxSequencerTaskStatus stat;
							elm->second.task->status(&stat);
							if (stat.stat == jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_RUN_COMPLETE)
							{
								if (stack_depth == elm->second.stack_height_run)
								{
									findNextTask(_common_set_sequencer.inOperation->currentQueueTp, elm->first);
								}
							}
						}
						res = JVX_NO_ERROR;
					}
					else
					{
						reportEndOfSequence = true;
						reportEndOfProcess = true;
						run = false;						
					}
					runInner = false; // If we have forwarded sequencer, we need to leave the inner loop again
				}
				else
				{
					// Move sequence counter forward
					_common_set_sequencer.inOperation->idxStep++;

					// If the previous one was not the last one, procede with next step - even in case of error
					if(_common_set_sequencer.inOperation->idxStep < _common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfLeaveSteps.size())
					{
						_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
						res = JVX_NO_ERROR;
					}
					else
					{
						reportEndOfSequence = true;
						reportEndOfProcess = true;
						run = false;
					}

					res = JVX_NO_ERROR;
					runInner = false;
				}
				break;


			case JVX_SEQUENCER_PROCEDE_JUMP_TO_LABEL:

				_common_set_sequencer.granularity_report_state_cnt = 0;
				if(_common_set_sequencer.inOperation->currentQueueTp == JVX_SEQUENCER_QUEUE_TYPE_RUN)
				{
					bool foundLabel = false;
					jvxSize i;
					for(i = 0; i < _common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfRunningSteps.size(); i++)
					{
						if(_common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfRunningSteps[i].step_label == label_next_step)
						{
							_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
							_common_set_sequencer.inOperation->idxStep = (jvxInt16)i;
							foundLabel = true;
							break;
						}
					}
					if(!foundLabel)
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Unable to jump to label " + label_next_step;
						if(_common_set_sequencer.report)
						{
							if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP)
							{
								_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR,
									errorDescription.c_str(), _common_set_sequencer.inOperation->idxSequence,
									_common_set_sequencer.inOperation->idxStep,
									_common_set_sequencer.inOperation->currentQueueTp,
									theStep->step_tp, theStep->step_function_id,
									_common_set_sequencer.in_processing_state,
									(_common_set_sequencer.operationStack.size() == 1));
							}
						}
					}
					else
					{
						runInner = false;
					}
				}
				else
				{
					bool foundLabel = false;
					jvxSize i;
					for(i = 0; i < _common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfLeaveSteps.size(); i++)
					{
						if(_common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfLeaveSteps[i].step_label == label_next_step)
						{
							_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
							_common_set_sequencer.inOperation->idxStep = (jvxInt16)i;
							foundLabel = true;
							break;
						}
					}
					if(!foundLabel)
					{
						whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
						errorDescription = "Unable to jump to label " + label_next_step;
						if(_common_set_sequencer.report)
						{
							if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP)
							{
								_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR,
									errorDescription.c_str(), _common_set_sequencer.inOperation->idxSequence,
									_common_set_sequencer.inOperation->idxStep,
									_common_set_sequencer.inOperation->currentQueueTp,
									theStep->step_tp, theStep->step_function_id,
									_common_set_sequencer.in_processing_state,
									(_common_set_sequencer.operationStack.size() == 1));
							}
						}

						// This is a critical situation, sequencer needs to emergency leave
						reportEndOfProcess = true;
						reportEndOfSequence = true;
						run = false;
					}
					else
					{
						runInner = false;
					}
				}

				break;

			case JVX_SEQUENCER_PROCEDE_CALL_SEQUENCE:

				_common_set_sequencer.granularity_report_state_cnt = 0;
				// Start the sub routine
				if (findNextSequenceCall(label_next_seq))
				{
					runInner = false; // <- procede to next step
				}
				else
				{
					whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
					errorDescription = "Unable to call subroutine " + label_next_seq;
					if (_common_set_sequencer.report)
					{
						if (_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP)
						{
							_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR,
								errorDescription.c_str(), _common_set_sequencer.inOperation->idxSequence,
								_common_set_sequencer.inOperation->idxStep,
								_common_set_sequencer.inOperation->currentQueueTp,
								theStep->step_tp, theStep->step_function_id,
								_common_set_sequencer.in_processing_state,
								(_common_set_sequencer.operationStack.size() == 1));
						}
					}
					reportEndOfProcess = true;
					reportEndOfSequence = true;
					run = false;
				}
				break;

			case JVX_SEQUENCER_PROCEDE_RUN_TASK:

				_common_set_sequencer.granularity_report_state_cnt = 0;
				// Start the sub routine
				findNextTask(_common_set_sequencer.inOperation->currentQueueTp, id_next_task);
				runInner = false; // <- procede to next step
				break;
			case JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP:
			case JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP_LEAVE_LOOP:

				_common_set_sequencer.granularity_report_state_cnt = 0;
				if (_common_set_sequencer.inOperation->sequencer_task)
				{
					runInner = false;
				}
				else
				{
					if (_common_set_sequencer.inOperation->currentQueueTp == JVX_SEQUENCER_QUEUE_TYPE_RUN)
					{
						_common_set_sequencer.inOperation->idxStep++;
						if (_common_set_sequencer.inOperation->idxStep >= (jvxInt16)_common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfRunningSteps.size())
						{
							if (_common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfLeaveSteps.size() > 0)
							{
								_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
								_common_set_sequencer.inOperation->idxStep = 0;
								_common_set_sequencer.inOperation->currentQueueTp = JVX_SEQUENCER_QUEUE_TYPE_LEAVE;
								runInner = false;
							}
							else
							{
								runInner = false;

								if (
									(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_SHUTDOWN_IN_PROGRESS) ||
									(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_ERROR))
								{
									reportEndOfSequence = true;
									reportEndOfProcess = true;
									run = false;
								}
								else
								{
									// Try to procede with next sequence
									if (!findNextValidStep(_common_set_sequencer.inOperation->idxSequence + 1))
									{
										// There is no further sequence, let us restart if enabled
										if (_common_set_sequencer.inOperation->loop_enabled)
										{
											// Should never fail
											jvxBool suc = findNextValidStep(0);
											assert(suc);
										}
										else
										{
											reportEndOfSequence = true;
											reportEndOfProcess = true;
											run = false;
										}
									}
									else
									{
										_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
									}
								}
							}
						}
						else
						{
							_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
							runInner = false;
						}
					}//if(_common_set_sequencer.inOperation->currentQueueTp == JVX_SEQUENCER_QUEUE_TYPE_RUN)
					else
					{
						_common_set_sequencer.inOperation->idxStep++;
						if (_common_set_sequencer.inOperation->idxStep < _common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfLeaveSteps.size())
						{
							_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
							runInner = false;
						}
						else
						{
							reportEndOfSequence = true;
							runInner = false;

							if (
								(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_SHUTDOWN_IN_PROGRESS) ||
								(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_ERROR))
							{
								reportEndOfProcess = true;
								run = false;
							}
							else
							{
#if 0
								// Try to procede with next sequence
								if (!findNextValidStep(_common_set_sequencer.inOperation->idxSequence + 1))
								{
									// There is no further sequence, let us restart if enabled
									if (_common_set_sequencer.inOperation->loop_enabled)
									{
										// Should never fail
										assert(findNextValidStep(0));
									}
									else
									{
										reportEndOfSequence = true;
										reportEndOfProcess = true;
										run = false;
									}
								}
								else
								{
									_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
								}
#else
								reportEndOfSequence = true;
								reportEndOfProcess = true;
								run = false;
#endif
							}
						}
					}
				}if (whatToDoNext == JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP_LEAVE_LOOP)
				{
					// We want to leave the loop here
					run = false;
				}
				break;

			case JVX_SEQUENCER_PROCEDE_NEXT_REGULAR_STEP_INCREMENT:

				_common_set_sequencer.granularity_report_state_cnt = 0;
				if(_common_set_sequencer.inOperation->currentQueueTp == JVX_SEQUENCER_QUEUE_TYPE_RUN)
				{
					_common_set_sequencer.inOperation->idxStep += thePcOffset;
					if(_common_set_sequencer.inOperation->idxStep >= _common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfRunningSteps.size())
					{
						if(_common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfLeaveSteps.size() > 0)
						{
							_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
							_common_set_sequencer.inOperation->idxStep = 0;
							_common_set_sequencer.inOperation->currentQueueTp = JVX_SEQUENCER_QUEUE_TYPE_LEAVE;
							runInner = false;
						}
						else
						{
							runInner = false;

							if(
								(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_SHUTDOWN_IN_PROGRESS)||
								(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_ERROR))
							{
								reportEndOfSequence = true;
								reportEndOfProcess = true;
								run = false;
							}
							else
							{
								// Try to procede with next sequence
								if(!findNextValidStep(_common_set_sequencer.inOperation->idxSequence+1))
								{
									// There is no further sequence, let us restart if enabled
									if(_common_set_sequencer.inOperation->loop_enabled)
									{
										// Should never fail
										jvxBool suc = findNextValidStep(0);
										assert(suc);
									}
									else
									{
										reportEndOfSequence = true;
										reportEndOfProcess = true;
										run = false;
									}
								}
								else
								{
									_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
								}
							}
						}
					}
					else
					{
						_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
						runInner = false;
					}
				}//if(_common_set_sequencer.inOperation->currentQueueTp == JVX_SEQUENCER_QUEUE_TYPE_RUN)
				else
				{
					_common_set_sequencer.inOperation->idxStep += thePcOffset;
					if(_common_set_sequencer.inOperation->idxStep < _common_set_sequencer.theSequences.all_sequences[_common_set_sequencer.inOperation->idxSequence].lstOfLeaveSteps.size())
					{
						_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
						runInner = false;
					}
					else
					{
						reportEndOfSequence = true;
						runInner = false;

						if(
							(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_SHUTDOWN_IN_PROGRESS)||
							(_common_set_sequencer.inOperation->theSeqState == JVX_SEQUENCER_STATUS_ERROR))
						{
							reportEndOfProcess = true;
							run = false;
						}
						else
						{
							// Try to procede with next sequence
							if(!findNextValidStep(_common_set_sequencer.inOperation->idxSequence+1))
							{
								// There is no further sequence, let us restart if enabled
								if(_common_set_sequencer.inOperation->loop_enabled)
								{
									// Should never fail
									assert(findNextValidStep(0));
								}
								else
								{
									reportEndOfSequence = true;
									reportEndOfProcess = true;
									run = false;
								}
							}
							else
							{
								_common_set_sequencer.inOperation->timestamp_store = timestamp_us;
							}
						}
					}
				}
				break;
			}//switch(whatToDoNext)
		}//while(runInner)
		if(reportEndOfSequence)
		{
			switch(_common_set_sequencer.inOperation->theSeqState)
			{
			case JVX_SEQUENCER_STATUS_ERROR:
				if(_common_set_sequencer.report)
				{
					if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ERROR)
					{
						_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ERROR,
							"", _common_set_sequencer.inOperation->idxSequence,
							_common_set_sequencer.inOperation->idxStep,
							_common_set_sequencer.inOperation->currentQueueTp,
							theStep->step_tp, theStep->step_function_id,
							_common_set_sequencer.in_processing_state,
							(_common_set_sequencer.operationStack.size() == 1));
					}
				}
				break;
			case JVX_SEQUENCER_STATUS_SHUTDOWN_IN_PROGRESS:
				if(_common_set_sequencer.report)
				{
					if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT)
					{
						if (theStep)
						{
							_common_set_sequencer.report->report_event(
								//JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT,
								_common_set_sequencer.inOperation->howtosaygoodbye,
								"", _common_set_sequencer.inOperation->idxSequence,
								_common_set_sequencer.inOperation->idxStep,
								_common_set_sequencer.inOperation->currentQueueTp,
								theStep->step_tp, theStep->step_function_id,
								_common_set_sequencer.in_processing_state,
								(_common_set_sequencer.operationStack.size() == 1));
						}
						else
						{
							_common_set_sequencer.report->report_event(
								//JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT,
								_common_set_sequencer.inOperation->howtosaygoodbye,
								"", _common_set_sequencer.inOperation->idxSequence,
								_common_set_sequencer.inOperation->idxStep,
								_common_set_sequencer.inOperation->currentQueueTp,
								JVX_SEQUENCER_TYPE_COMMAND_NONE, JVX_SIZE_UNSELECTED,
								_common_set_sequencer.in_processing_state,
								(_common_set_sequencer.operationStack.size() == 1));

						}
					}
				}
				break;
			case JVX_SEQUENCER_STATUS_IN_OPERATION:
				if(_common_set_sequencer.report)
				{
					if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE)
					{
						_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE,
							"", _common_set_sequencer.inOperation->idxSequence,
							_common_set_sequencer.inOperation->idxStep,
							_common_set_sequencer.inOperation->currentQueueTp,
							theStep->step_tp, theStep->step_function_id,
							_common_set_sequencer.in_processing_state,
							(_common_set_sequencer.operationStack.size() == 1));
					}
				}
				break;

			default:
				assert(0);
			}

			bool isFullShutdown = _common_set_sequencer.inOperation->isFullShutdown;
			bool isErrorShutdown = _common_set_sequencer.inOperation->isErrorShutdown;
			bool returnFromSubroutine = findNextSequenceStack();
			if (returnFromSubroutine)
			{
				// Make sure we return immediately to start with this callback again
				wantImmediateReturn = true;
					
				// restart with sequence from stack
				reportEndOfProcess = false;
				reportEndOfSequence = false;
				
				// If a full shutdown was requested, this will also speed up completing the sequence from stack
				if (isErrorShutdown)
				{
					whatToDoNext = JVX_SEQUENCER_PROCEDE_EXIT_ERROR;
					_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_ERROR;
					_common_set_sequencer.inOperation->isErrorShutdown = isErrorShutdown;
				}
				else if (isFullShutdown)
				{
					_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_WANTS_TO_STOP_PROCESS;
					_common_set_sequencer.inOperation->isFullShutdown = isFullShutdown;
				}
				else
				{
					_common_set_sequencer.run_mode_operate = _common_set_sequencer.run_mode_save;
				}
				goto jump_to_next_step;
			}
			reportEndOfSequence = false;
		}
	}//while(run)

	res = JVX_NO_ERROR;

	if(reportEndOfProcess)
	{
		switch(_common_set_sequencer.inOperation->theSeqState)
		{
		case JVX_SEQUENCER_STATUS_ERROR:
			if(_common_set_sequencer.report)
			{
				if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR)
				{
					_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR,
						"", _common_set_sequencer.inOperation->idxSequence,
						_common_set_sequencer.inOperation->idxStep,
						_common_set_sequencer.inOperation->currentQueueTp,
						theStep->step_tp, theStep->step_function_id,
						_common_set_sequencer.in_processing_state,
						(_common_set_sequencer.operationStack.size() == 1));
				}
			}
			break;
		case JVX_SEQUENCER_STATUS_SHUTDOWN_IN_PROGRESS:
			if(_common_set_sequencer.report)
			{
				if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT)
				{
					// Some specific case catches
					if (_common_set_sequencer.inOperation->howtosaygoodbye == JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT)
					{
						_common_set_sequencer.inOperation->howtosaygoodbye = JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT;
					}
					if (_common_set_sequencer.inOperation->howtosaygoodbye == JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE)
					{
						_common_set_sequencer.inOperation->howtosaygoodbye = JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS;
					}

					_common_set_sequencer.report->report_event(
						//JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT,
						_common_set_sequencer.inOperation->howtosaygoodbye,
						"", _common_set_sequencer.inOperation->idxSequence,
						_common_set_sequencer.inOperation->idxStep,
						_common_set_sequencer.inOperation->currentQueueTp,
						theStep->step_tp, theStep->step_function_id,
						_common_set_sequencer.in_processing_state,
						(_common_set_sequencer.operationStack.size() == 1));
				}
			}
			break;
		case JVX_SEQUENCER_STATUS_IN_OPERATION:
			if(_common_set_sequencer.report)
			{
				if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS)
				{
					_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS,
						"", _common_set_sequencer.inOperation->idxSequence,
						_common_set_sequencer.inOperation->idxStep,
						_common_set_sequencer.inOperation->currentQueueTp,
						theStep->step_tp, theStep->step_function_id,
						_common_set_sequencer.in_processing_state,
						(_common_set_sequencer.operationStack.size() == 1));
				}
			}
			break;

		default:
			assert(0);
		}

		_common_set_sequencer.inOperation->theSeqState = JVX_SEQUENCER_STATUS_SHUTDOWN_COMPLETE;
		res = JVX_ERROR_ABORT;
	}

	if(_common_set_sequencer.report)
	{
		if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_DEBUG_MESSAGE)
		{
			if (theStep)
			{
				_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_DEBUG_MESSAGE,
					"Leave Message Queue", _common_set_sequencer.inOperation->idxSequence,
					_common_set_sequencer.inOperation->idxStep,
					_common_set_sequencer.inOperation->currentQueueTp,
					theStep->step_tp, theStep->step_function_id,
					_common_set_sequencer.in_processing_state,
					(_common_set_sequencer.operationStack.size() == 1));
			}
			else
			{
				_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_DEBUG_MESSAGE,
					"Leave Message Queue", _common_set_sequencer.inOperation->idxSequence,
					_common_set_sequencer.inOperation->idxStep,
					_common_set_sequencer.inOperation->currentQueueTp,
					JVX_SEQUENCER_TYPE_COMMAND_NONE, JVX_SIZE_UNSELECTED,
					_common_set_sequencer.in_processing_state,
					(_common_set_sequencer.operationStack.size() == 1));

			}
		}
	}

	if (wantImmediateReturn)
	{
		res = JVX_ERROR_REQUEST_CALL_AGAIN;
	}

step_leave:

	_common_set_sequencer.in_processing = false;
	return res;
}

jvxErrorType
CjvxFullHost::sequencer_started(jvxInt64 timestamp_us)
{
	if (_common_set_sequencer.report)
	{
		_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE,
			"Startup of sequencer thread successfully completed", JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, 
			JVX_SEQUENCER_QUEUE_TYPE_NONE, JVX_SEQUENCER_TYPE_COMMAND_NONE, JVX_SIZE_UNSELECTED,
			_common_set_sequencer.in_processing_state,
			true);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFullHost::sequencer_stopped(jvxInt64 timestamp_us)
{
	if(_common_set_sequencer.report)
	{
		if(_common_set_sequencer.inOperation->eventMask & JVX_SEQUENCER_EVENT_PROCESS_TERMINATED)
		{
			_common_set_sequencer.report->report_event(JVX_SEQUENCER_EVENT_PROCESS_TERMINATED,
				"", JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, JVX_SEQUENCER_QUEUE_TYPE_NONE, JVX_SEQUENCER_TYPE_COMMAND_NONE, JVX_SIZE_UNSELECTED,
				_common_set_sequencer.in_processing_state,
				true);
		}
	}
	return JVX_NO_ERROR;
}
