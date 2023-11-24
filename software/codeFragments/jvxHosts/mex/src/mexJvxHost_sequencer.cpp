#include "mexJvxHost.h"
#include "realtimeViewer_helpers.h"
#include "CjvxMatlabToCConverter.h"

jvxErrorType 
	mexJvxHost::info_sequencer(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::info_sequencer");
	jvxState theState = JVX_STATE_NONE;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], true);
			}
			if(nlhs > 1)
			{
				this->mexReturnStructSequencer(plhs[1]);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
	mexJvxHost::add_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::add_sequence");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			std::string description = "No Description";
			std::string label;

			bool parametersOk = true;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2String(description, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("sequence_description", paramId, "string");
				parametersOk = false;
			}

			paramId = 2;
			res = CjvxMatlabToCConverter::mexArgument2String(label, prhs, paramId, nrhs);

			if(parametersOk)
			{
				involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

				if(theSequencer)
				{
					jvxErrorType res = theSequencer->add_sequence(description.c_str(), label.c_str());
					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

					if(res == JVX_NO_ERROR)
					{
						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], true);
						}
						mexFillEmpty(plhs, nlhs, 1);
					}
					else
					{
						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if(nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::mark_sequence_default(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize i;
	jvxSize idxFound = JVX_SIZE_UNSELECTED;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::add_sequence");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;
	jvxSize num = 0;
	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			std::string description = "No Description";

			bool parametersOk = true;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2String(description, prhs, paramId, nrhs);
			if (res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("sequence_description", paramId, "string");
				parametersOk = false;
			}

			if (parametersOk)
			{
				involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

				if (theSequencer)
				{
					jvxErrorType res = theSequencer->number_sequences(&num);
					jvxApiString descr;
					for (i = 0; i < num; i++)
					{
						theSequencer->description_sequence(i, &descr, NULL, NULL, NULL, NULL, NULL);
						if (descr.std_str() == description)
						{
							idxFound = i;
							break;
						}
					}

					if (JVX_CHECK_SIZE_SELECTED(idxFound))
					{
						theSequencer->set_sequence_default(idxFound);
					}
					else
					{
						res = JVX_ERROR_ELEMENT_NOT_FOUND;
					}
					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

					if (res == JVX_NO_ERROR)
					{
						if (nlhs > 0)
						{
							this->mexReturnBool(plhs[0], true);
						}
						mexFillEmpty(plhs, nlhs, 1);
					}
					else
					{
						if (nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if (nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Could not find sequence with name <" + description + ">.", 
								JVX_ERROR_ELEMENT_NOT_FOUND);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
				}
				else
				{
					if (nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}
jvxErrorType 
	mexJvxHost::remove_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::add_sequence");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxSize idxSequence = 0;

			jvxInt32 valI = 0;
			bool parametersOk = true;
			jvxInt32 paramId = 0;


			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				idxSequence = valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_index", paramId, "numeric");
				parametersOk = false;
			}


			if(parametersOk)
			{
				involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

				if(theSequencer)
				{
					jvxErrorType res = theSequencer->remove_sequence(idxSequence);
					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

					if(res == JVX_NO_ERROR)
					{
						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], true);
						}
						mexFillEmpty(plhs, nlhs, 1);
					}
					else
					{
						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if(nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
	mexJvxHost::reset_sequences(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::add_sequence");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{

			involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

			if(theSequencer)
			{
				jvxErrorType res = theSequencer->reset_sequences();
				involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

				if(res == JVX_NO_ERROR)
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], true);
					}
					mexFillEmpty(plhs, nlhs, 1);
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
	mexJvxHost::insert_step_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::add_sequence");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;
	std::string nm;
	jvxCBitField assoc_mode = JVX_SEQUENCER_MODE_ALL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxSize idxSequence = 0;
			jvxSequencerQueueType qTp = JVX_SEQUENCER_QUEUE_TYPE_RUN;
			jvxSequencerElementType sTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
			jvxComponentIdentification tpC = JVX_COMPONENT_UNKNOWN;
			std::string description = "No Description";
			jvxInt32 funcId = 0;
			jvxInt64 timeout_ms = -1;
			std::string label = "";
			std::string label_true = "";
			std::string label_false = "";
			jvxInt32 posi = -1;

			jvxInt32 valI = 0;
			bool parametersOk = true;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				idxSequence = valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_index", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 2;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				if((valI >= 0) && (valI < JVX_SEQUENCER_TYPE_COMMAND_LIMIT))
				{
					sTp = (jvxSequencerElementType) valI;
				}
				else
				{
					MEX_PARAMETER_INVALID_VALUE_ERROR("sequence_step_element_type", paramId, valI, JVX_SEQUENCER_TYPE_COMMAND_LIMIT);
					parametersOk = false;
				}
			}
			else
			{
				res = CjvxMatlabToCConverter::mexArgument2String(nm, prhs, paramId, nrhs);
				if (res == JVX_NO_ERROR)
				{
					res = jvxSequencerElementType_decode(sTp, nm);
					if (res != JVX_NO_ERROR)
					{
						MEX_PARAMETER_UNKNOWN_ERROR("sequence_step_element_type", paramId, nm);
						parametersOk = false;
					}
				}
				else
				{
					MEX_PARAMETER_ERROR("sequence_step_element_type", paramId, "numeric");
					parametersOk = false;
				}
			}

			paramId = 3;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				if((valI >= 0) && (valI < JVX_SEQUENCER_QUEUE_TYPE_LIMIT))
				{
					qTp = (jvxSequencerQueueType) valI;
				}
				else
				{
					MEX_PARAMETER_INVALID_VALUE_ERROR("sequence_queue_type", paramId, valI, JVX_SEQUENCER_QUEUE_TYPE_LIMIT);
					parametersOk = false;
				}
			}
			else
			{
				res = CjvxMatlabToCConverter::mexArgument2String(nm, prhs, paramId, nrhs);
				if (res == JVX_NO_ERROR)
				{
					res = jvxSequencerQueueType_decode(qTp, nm);
					if (res != JVX_NO_ERROR)
					{
						MEX_PARAMETER_UNKNOWN_ERROR("sequence_queue_type", paramId, nm);
						parametersOk = false;
					}
				}
				else
				{
					MEX_PARAMETER_ERROR("sequence_step_element_type", paramId, "numeric");
					parametersOk = false;
				}
			}

			paramId = 4;
			res = CjvxMatlabToCConverter::mexArgument2ComponentIdentification(tpC, prhs, paramId, nrhs);
			
			switch (res)
			{
			case JVX_NO_ERROR:
				break;

			case JVX_ERROR_ID_OUT_OF_BOUNDS:
				MEX_PARAMETER_INVALID_VALUE_ERROR("component_type", paramId, valI, JVX_COMPONENT_ALL_LIMIT);
				parametersOk = false;
				break;

			case JVX_ERROR_PARSE_ERROR:
				MEX_PARAMETER_UNKNOWN_ERROR("component_identification", paramId, nm);
				break;

			case JVX_ERROR_INVALID_ARGUMENT:

				MEX_PARAMETER_ERROR("component_type", paramId, "numeric or string");
				parametersOk = false;

			case JVX_ERROR_INVALID_SETTING:
				MEX_PARAMETER_OBJ_NOT_FOUND("component_type", paramId, "jvxComponentType");
				parametersOk = false;
				break;
			default:
				assert(0);
			}
	
			paramId = 5;
			res = CjvxMatlabToCConverter::mexArgument2String(description, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("sequence_step_description", paramId, "string");
				parametersOk = false;
			}

			paramId = 6;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				funcId = (jvxSize) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_step_func_id", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 7;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt64>(timeout_ms, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("sequence_step_timeout_ms", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 8;
			res = CjvxMatlabToCConverter::mexArgument2String(label, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("sequence_step_label", paramId, "string");
				parametersOk = false;
			}
	

			paramId = 9;
			res = CjvxMatlabToCConverter::mexArgument2String(label_true, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("sequence_step_label_true", paramId, "string");
				parametersOk = false;
			}

			paramId = 10;
			res = CjvxMatlabToCConverter::mexArgument2String(label_false, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("sequence_step_label_false", paramId, "string");
				parametersOk = false;
			}

			paramId = 11;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				posi = (jvxSize) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("insert_position", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 12;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if (res == JVX_NO_ERROR)
			{
				tpC.slotid = JVX_INT_SIZE(valI);
			}
			
			paramId = 13;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if (res == JVX_NO_ERROR)
			{
				tpC.slotsubid = JVX_INT_SIZE(valI);
			}


			if(parametersOk)
			{

				involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

				if(theSequencer)
				{
					jvxErrorType res = theSequencer->insert_step_sequence_at(idxSequence, qTp, sTp, tpC,
						description.c_str(), funcId,
						timeout_ms, label.c_str(), label_true.c_str(), label_false.c_str(), false, assoc_mode, posi);

					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

					if(res == JVX_NO_ERROR)
					{

						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], true);
						}
						mexFillEmpty(plhs, nlhs, 1);
					}
					else
					{
						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if(nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
	mexJvxHost::edit_step_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::add_sequence");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;
	jvxCBitField assoc_mode = JVX_SEQUENCER_MODE_ALL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxSize idxSequence = 0;
			jvxSequencerQueueType qTp = JVX_SEQUENCER_QUEUE_TYPE_RUN;
			jvxSequencerElementType sTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			std::string description = "No Description";
			jvxSize funcId = 0;
			jvxInt64 timeout_ms = -1;
			std::string label = "";
			std::string label_true = "";
			std::string label_false = "";
			jvxSize posi = -1;
			jvxSize slotid = JVX_SIZE_SLOT_RETAIN;
			jvxSize slotsubid = JVX_SIZE_SLOT_RETAIN;
			jvxBool b_action = false;
			
			jvxInt32 valI = 0;
			bool parametersOk = true;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				idxSequence = valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_index", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 2;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				sTp = (jvxSequencerElementType) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_step_element_type", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 3;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				qTp = (jvxSequencerQueueType) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_step_queue_type", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 4;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				tpC = (jvxComponentType) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("component_type", paramId, "numeric");
				parametersOk = false;
			}
			
			paramId = 5;
			res = CjvxMatlabToCConverter::mexArgument2String(description, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("sequence_step_description", paramId, "string");
				parametersOk = false;
			}

			paramId = 6;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				funcId = (jvxSize) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_step_func_id", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 7;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt64>(timeout_ms, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("sequence_step_timeout_ms", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 8;
			res = CjvxMatlabToCConverter::mexArgument2String(label, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("sequence_step_label", paramId, "string");
				parametersOk = false;
			}
	

			paramId = 9;
			res = CjvxMatlabToCConverter::mexArgument2String(label_true, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("sequence_step_label_true", paramId, "string");
				parametersOk = false;
			}

			paramId = 10;
			res = CjvxMatlabToCConverter::mexArgument2String(label_false, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("sequence_step_label_false", paramId, "string");
				parametersOk = false;
			}

			paramId = 11;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				posi = (jvxSize) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("insert_position", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 12;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if (res == JVX_NO_ERROR)
			{
				slotid = JVX_INT_SIZE(valI);
			}
			paramId = 13;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if (res == JVX_NO_ERROR)
			{
				slotsubid = JVX_INT_SIZE(valI);
			}


			if(parametersOk)
			{
				involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

				if(theSequencer)
				{
					jvxErrorType res = theSequencer->edit_step_sequence(idxSequence, qTp, posi, sTp, jvxComponentIdentification(tpC, slotid, slotsubid),
						description.c_str(), funcId,
						timeout_ms, label.c_str(), label_true.c_str(), label_false.c_str(), b_action, assoc_mode);

					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

					if(res == JVX_NO_ERROR)
					{

						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], true);
						}
						mexFillEmpty(plhs, nlhs, 1);
					}
					else
					{
						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if(nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
	mexJvxHost::remove_step_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::add_sequence");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxSize idxSequence = 0;
			jvxSequencerQueueType qTp = JVX_SEQUENCER_QUEUE_TYPE_RUN;
			jvxSequencerElementType sTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			std::string description = "No Description";
			jvxSize funcId = 0;
			jvxInt64 timeout_ms = -1;
			std::string label = "";
			std::string label_true = "";
			std::string label_false = "";
			jvxSize posi = -1;


			jvxInt32 valI = 0;
			bool parametersOk = true;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				idxSequence = valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_index", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 2;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				qTp = (jvxSequencerQueueType) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_step_queue_type", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 3;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				posi = (jvxSize)valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_step_index", paramId, "numeric");
				parametersOk = false;
			}

			if(parametersOk)
			{

				involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

				if(theSequencer)
				{
					jvxErrorType res = theSequencer->remove_step_sequence(idxSequence, qTp, posi);

					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

					if(res == JVX_NO_ERROR)
					{

						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], true);
						}
						mexFillEmpty(plhs, nlhs, 1);
					}
					else
					{
						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if(nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
	mexJvxHost::switch_step_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::add_sequence");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxSize idxSequence = 0;
			jvxSequencerQueueType qTp = JVX_SEQUENCER_QUEUE_TYPE_RUN;
			jvxSize posi1 = -1;
			jvxSize posi2 = -1;

			jvxInt32 valI = 0;
			bool parametersOk = true;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				idxSequence = valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_index", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 2;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				qTp = (jvxSequencerQueueType) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_step_queue_type", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 3;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI,prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				posi1 = (jvxSize)valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_step_index[1]", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 4;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				posi2 = (jvxSize)valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_step_index[2]", paramId, "numeric");
				parametersOk = false;
			}


			if(parametersOk)
			{

				involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

				if(theSequencer)
				{
					jvxErrorType res = theSequencer->switch_steps_sequence(idxSequence, qTp, posi1, posi2);

					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

					if(res == JVX_NO_ERROR)
					{

						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], true);
						}
						mexFillEmpty(plhs, nlhs, 1);
					}
					else
					{
						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if(nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
	mexJvxHost::set_active_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::add_sequence");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxSize idxSequence = 0;
			jvxBool runnit = false;

			jvxInt32 valI = 0;
			bool parametersOk = true;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				idxSequence = valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("sequence_index", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 2;
			res = CjvxMatlabToCConverter::mexArgument2Bool(runnit, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("activation", paramId, "bool");
				parametersOk = false;
			}

			if(parametersOk)
			{

				involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

				if(theSequencer)
				{
					jvxErrorType res = theSequencer->mark_sequence_process(idxSequence, runnit);

					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

					if(res == JVX_NO_ERROR)
					{

						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], true);
						}
						mexFillEmpty(plhs, nlhs, 1);
					}
					else
					{
						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if(nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
	mexJvxHost::start_process_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::start_process_sequence");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxInt64 eventMask = -1;
			jvxInt32 granStateReport = 1;
			jvxInt32 valI = 0;

			bool parametersOk = true;
			jvxInt32 paramId = 0;


			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt64>(eventMask, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("event_mask", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 2;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(granStateReport, prhs, paramId, nrhs);
			if (res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("gran_state_report", paramId, "numeric");
				parametersOk = false;
			}

			if(parametersOk)
			{		

				involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

				if(theSequencer)
				{
					jvxErrorType res = theSequencer->run_process(static_cast<IjvxSequencer_report*>(this), eventMask, granStateReport);

					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

					if(res == JVX_NO_ERROR)
					{

						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], true);
						}
						mexFillEmpty(plhs, nlhs, 1);
					}
					else
					{
						if(nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if(nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);

					}
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
	mexJvxHost::trigger_stop_process(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::trigger_stop_process");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

			if(theSequencer)
			{
				jvxErrorType res = theSequencer->trigger_abort_process();
				involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

				if(res == JVX_NO_ERROR)
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], true);
					}
					mexFillEmpty(plhs, nlhs, 1);
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::trigger_stop_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;

	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

			if (theSequencer)
			{
				jvxErrorType res = theSequencer->trigger_abort_sequence();
				involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

				if (res == JVX_NO_ERROR)
				{
					if (nlhs > 0)
					{
						this->mexReturnBool(plhs[0], true);
					}
					mexFillEmpty(plhs, nlhs, 1);
				}
				else
				{
					if (nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
mexJvxHost::wait_completion_process(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::wait_complete_process");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE) 
		{
			involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

			if(theSequencer)
			{
				jvxInt64 tStamp_us = -1;
				jvxErrorType res = theSequencer->trigger_complete_process_extern(tStamp_us);
				if (res != JVX_NO_ERROR)
				{
					if (nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to <trigger_complete_process_extern> failed.", res);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
					
				res = theSequencer->acknowledge_completion_process();
				if (res != JVX_NO_ERROR)
				{
					if (nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to <wait_completion_process> failed.", res);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}

				involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

				if(res == JVX_NO_ERROR)
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], true);
					}
					mexFillEmpty(plhs, nlhs, 1);
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
	mexJvxHost::status_process_sequence(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::status_process_sequence");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxSequencerStatus status = JVX_SEQUENCER_STATUS_NONE;
			jvxSize idxSeq = JVX_SIZE_UNSELECTED;
			jvxSequencerQueueType qTp = JVX_SEQUENCER_QUEUE_TYPE_NONE;
			jvxSize idxStep = 0;
			jvxBool loopEn = false;

			involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

			if(theSequencer)
			{
				jvxErrorType res = theSequencer->status_process(&status, &idxSeq, &qTp, &idxStep, &loopEn);
				involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

				if(res == JVX_NO_ERROR)
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], true);
					}
					if(nlhs > 1)
					{
						this->mexReturnSequencerStatus(plhs[1], status, idxSeq, qTp, idxStep, loopEn);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
	mexJvxHost::trigger_external_process(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxBdxHost::trigger_external_process");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;
	jvxSequencerElementType last_step_type = JVX_SEQUENCER_TYPE_COMMAND_NONE;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxInt64 tStamp_us = -1;
			jvxInt32 valI = 0;

			bool parametersOk = true;
			jvxInt32 paramId = 0;


			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt64>(tStamp_us, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("timestamp_us", paramId, "numeric");
				parametersOk = false;
			}

			involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

			if(theSequencer)
			{
				jvxErrorType res = theSequencer->trigger_step_process_extern(tStamp_us, &last_step_type);
				involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

				if((res == JVX_NO_ERROR) || (res == JVX_ERROR_ABORT) || (res == JVX_ERROR_PROCESS_COMPLETE))
				{
					jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
					theSequencer->status_process(&stat, NULL, NULL, NULL, NULL);

					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], true);
					}
					if(nlhs > 1)
					{
						this->mexReturnInt32(plhs[1], stat);
					}
					if(nlhs > 2)
					{
						this->mexReturnInt32(plhs[2], last_step_type);
					}
					if (nlhs > 3)
					{
						this->mexReturnInt32(plhs[3], res);
					}
					mexFillEmpty(plhs, nlhs, 4);
				}
				else  
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

	/*
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::trigger_external_process");
	jvxState theState = JVX_STATE_NONE;
	IjvxSequencer* theSequencer = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxInt64 tStamp_us = -1;
			jvxInt32 valI = 0;

			bool parametersOk = true;
			jvxInt32 paramId = 0;


			paramId = 1;
			res = mexArgument2Index<jvxInt64>(tStamp_us, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("timestamp_us", paramId, "numeric");
				parametersOk = false;
			}

			involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

			if(theSequencer)
			{
				jvxErrorType res = theSequencer->trigger_step_process_extern(tStamp_us);
				involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));

				if(res == JVX_NO_ERROR)
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], true);
					}
					if(nlhs > 1)
					{
						this->mexReturnBool(plhs[1], false);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
				else if(res == JVX_ERROR_ABORT)
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], true);
					}
					if(nlhs > 1)
					{
						this->mexReturnBool(plhs[1], true);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
				else 
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to sequencer reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to get access to sequencer in host", JVX_ERROR_NOT_READY);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}*/