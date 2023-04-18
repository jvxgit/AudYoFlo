#include "CjvxHostJsonCommandsActShow.h"
#include "CjvxHostJsonDefines.h"

extern "C"
{
#include "jvx-crc.h"
}

jvxErrorType
CjvxHostJsonCommandsActShow::act_edit_config(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string actionString;
	std::string errTxt;
	jvxSize szSlots = 0;
	jvxSize szSlotsub = 0;
	CjvxJsonElement jelm_result; 
	// Arg1: add sequence, remove sequence, activate sequence, add step, remove step, modify step
	if (args.size() > off)
	{
		res = CjvxHostJsonCommandsShow::act_edit_config(dh_command, args, off, jsec);
		if (res == JVX_NO_ERROR)
		{
			// Success, complete 
			return res;
		}

		jsec.deleteAll();
		errTxt = "";
		res = JVX_NO_ERROR;
		actionString = args[off];

		if (actionString == "silent_mode")
		{
			if (args.size() > 2)
			{
				// "set" operation
				if (jvx_check_yes(args[2]))
				{ 
					config_act.silent_out = c_true;
				}
				else if (jvx_check_no(args[2]))
				{
					config_act.silent_out = c_false;
				}
				else
				{
					res = JVX_ERROR_PARSE_ERROR;
					errTxt = "The requested config value <" + args[2] + "> is not known.";
					JVX_CREATE_ERROR_NO_RETURN(jsec, res, errTxt);
				}
			}
			else
			{
				if (config_act.silent_out)
				{
					config_act.silent_out = false;
				}
				else
				{
					config_act.silent_out = true;
				}
			}
		}		
#if 0
		else if (actionString == "timeout_seq")
		{
			if (args.size() > 2)
			{
				// "set" operation
				jvxBool err = false;
				jvxSize newVal = jvx_string2Size(args[2], err);
				if (!err)
				{
					config.timeout_seq_msec = newVal;
				}
				else
				{
					res = JVX_ERROR_PARSE_ERROR;
					errTxt = "The value <" + args[2] + "> is not a valid size.";
					JVX_CREATE_ERROR_NO_RETURN(jsec, res, errTxt);
				}
			}
		}
#endif
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
			errTxt = "The requested operation <" + actionString + "> is not known.";
			JVX_CREATE_ERROR_NO_RETURN(jsec, res, errTxt);
		}
	}
	else
	{
		oneDrivehostCommand dhc = dh_command;
		dhc.family = JVX_DRIVEHOST_FAMILY_SHOW;
		res = this->show_current_config(dhc, args, off, jsec);
	}

	return res;
}


// =====================================================================================================
// =====================================================================================================


jvxErrorType
CjvxHostJsonCommandsActShow::act_edit_sequencer(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string actionString;
	std::string errTxt;
	// Arg1: add sequence, remove sequence, activate sequence, add step, remove step, modify step
	if (args.size() > off)
	{
		actionString = args[off];
		if (actionString == "add sequence")
		{
			res = act_edit_add_sequence(args, off+1, jelmret);
		}
		else if (actionString == "remove sequence")
		{
			res = act_edit_remove_sequence(args, off + 1, jelmret);
		}
		else if (actionString == "activate sequence")
		{
			res = act_edit_activate_sequence(args, off + 1, jelmret);
		}
		else if (actionString == "deactivate sequence")
		{
			res = act_edit_deactivate_sequence(args, off + 1, jelmret);
		}
		else if (actionString == "add step")
		{
			res = act_edit_add_step_sequence(args, off + 1, jelmret);
		}
		else if (actionString == "remove step")
		{
			res = act_edit_remove_step_sequence(args, off + 1, jelmret);
		}
		else if (actionString == "modify step")
		{
			res = act_edit_modify_step_sequence(args, off + 1, jelmret);
		}
		else if (actionString == "switch steps")
		{
			res = act_edit_switch_step_sequence(args, off + 1, jelmret);
		}
		else if (actionString == "start")
		{
			res = act_start_sequencer(jelmret);
		}
		else if (actionString == "stop")
		{
			res = act_stop_sequencer(jelmret, true);
		}
		else if (actionString == "stops")
		{
			res = act_stop_sequencer(jelmret, false);
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
			errTxt = "The requested operation <" + actionString + "> is not known.";
			JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
		}

	}
	return res;
}

jvxErrorType 
CjvxHostJsonCommandsActShow::act_edit_add_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string nameSequence;
	std::string labelSequence;
	std::string errTxt;

	IjvxSequencer* sequencer = NULL;
	
	if (args.size() > off)
	{
		nameSequence = args[off];
	}
	else
	{
		JVX_CREATE_ERROR_RETURN(jelmret, JVX_ERROR_ID_OUT_OF_BOUNDS, "No sequence name specified.");
	}

	if (args.size() > off+1)
	{
		labelSequence = args[off+1];
	}
	
	res = hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if ((res == JVX_NO_ERROR) && sequencer)
	{
		if (nameSequence.empty())
		{
			JVX_CREATE_ERROR_NO_RETURN(jelmret, JVX_ERROR_INVALID_ARGUMENT, "It is not possible to add a new sequence with empty name.");
		}
		else
		{
			res = sequencer->add_sequence(nameSequence.c_str(), labelSequence.c_str());
			if (res != JVX_NO_ERROR)
			{
				errTxt = "Failed to add sequence <";
				errTxt += nameSequence;
				errTxt += ">, reason: ";
				errTxt += jvxErrorType_descr(res);
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
			}
		}
		res = hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
	} // res = runtime.theHostRef->request_hidden_interface(JVX_INTERFACE
	else
	{
		errTxt = "Failed to obtain sequencer handle.";
		res = JVX_ERROR_INTERNAL;
		JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
	}
	return res;
}

jvxErrorType 
CjvxHostJsonCommandsActShow::act_edit_remove_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idS = JVX_SIZE_UNSELECTED;
	jvxSize num = JVX_SIZE_UNSELECTED;
	jvxBool err = false;
	jvxSize i;
	jvxApiString  fldStr;
	IjvxSequencer* sequencer = NULL;
	std::string argString;
	std::string listedString;
	std::string errTxt;
	if (args.size() > off)
	{
		argString = args[off];
	}

	// =======================================================

	res = hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if ((res == JVX_NO_ERROR) && sequencer)
	{
		sequencer->number_sequences(&num);

		idS = jvx_string2Size(argString, err);
		if (err)
		{
			idS = JVX_SIZE_UNSELECTED;
		}

		if (JVX_CHECK_SIZE_UNSELECTED(idS))
		{
			for (i = 0; i < num; i++)
			{
				sequencer->description_sequence(i, &fldStr, NULL,  NULL, NULL, NULL, NULL);
				listedString = fldStr.std_str();

				if (listedString == argString)
				{
					idS = i;
					break;
				}
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(idS))
		{
			if (idS < num)
			{
				res = sequencer->remove_sequence(idS);
				if (res != JVX_NO_ERROR)
				{
					errTxt = "Failed to remove sequence id #";
					errTxt += jvx_size2String(idS);
					errTxt += ", reason: ";
					errTxt += jvxErrorType_descr(res);
					JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
				}
			}
			else
			{
				errTxt = "Sequence id #";
				errTxt += jvx_size2String(idS);
				errTxt += " out of range. There are only ";
				errTxt += jvx_size2String(num);
				errTxt += " sequences available.";
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
			}
		}
		else
		{
			{
				errTxt = "Could not find sequence with identification <";
				errTxt += argString;
				errTxt += ">.";
				errTxt += "\n";
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
			}
		}
		res = hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
	} // res = runtime.theHostRef->request_hidden_interface(JVX_INTERFACE
	else
	{
		errTxt = "Failed to obtain sequencer handle.";
		errTxt += "\n";
		res = JVX_ERROR_INTERNAL;
		JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
	}
	return res;
}

jvxErrorType 
CjvxHostJsonCommandsActShow::act_edit_activate_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idS = JVX_SIZE_UNSELECTED;
	jvxSize num = JVX_SIZE_UNSELECTED;
	jvxBool err = false;
	jvxSize i;
	jvxApiString  fldStr;
	IjvxSequencer* sequencer = NULL;
	std::string argString;
	std::string listedString;
	std::string errTxt;
	if (args.size() > off)
	{
		argString = args[off];
	}

	// =======================================================

	res = hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if ((res == JVX_NO_ERROR) && sequencer)
	{
		sequencer->number_sequences(&num);

		idS = jvx_string2Size(argString, err);
		if (err)
		{
			idS = JVX_SIZE_UNSELECTED;
		}

		if (JVX_CHECK_SIZE_UNSELECTED(idS))
		{
			for (i = 0; i < num; i++)
			{
				sequencer->description_sequence(i, &fldStr, NULL, NULL, NULL, NULL, NULL);
				listedString = fldStr.std_str();
				if (listedString == argString)
				{
					idS = i;
					break;
				}
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(idS))
		{
			if (idS < num)
			{
				res = sequencer->mark_sequence_process(idS, true);
				if (res != JVX_NO_ERROR)
				{
					errTxt = "Failed to mark sequence id #";
					errTxt += jvx_size2String(idS);
					errTxt += " for processing, reason: ";
					errTxt += jvxErrorType_descr(res);
					JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
				}
			}
			else
			{
				errTxt = "Sequence id #";
				errTxt += jvx_size2String(idS);
				errTxt += " out of range. There are only ";
				errTxt += jvx_size2String(num);
				errTxt += " sequences available.";
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);

			}
		}
		else
		{
			{
				errTxt = "Could not find sequence with identification <";
				errTxt += argString;
				errTxt += ">.";
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
			}
		}
		res = hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
	} // res = runtime.theHostRef->request_hidden_interface(JVX_INTERFACE
	else
	{
		errTxt = "Failed to obtain sequencer handle.";
		res = JVX_ERROR_INTERNAL;
		JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
	}
	return res;
}

jvxErrorType
CjvxHostJsonCommandsActShow::act_edit_deactivate_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idS = JVX_SIZE_UNSELECTED;
	jvxSize num = JVX_SIZE_UNSELECTED;
	jvxBool err = false;
	jvxSize i;
	jvxApiString  fldStr;
	IjvxSequencer* sequencer = NULL;
	std::string argString;
	std::string listedString;
	std::string errTxt;
	if (args.size() > off)
	{
		argString = args[off];
	}

	// =======================================================

	res = hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if ((res == JVX_NO_ERROR) && sequencer)
	{
		sequencer->number_sequences(&num);

		idS = jvx_string2Size(argString, err);
		if (err)
		{
			idS = JVX_SIZE_UNSELECTED;
		}

		if (JVX_CHECK_SIZE_UNSELECTED(idS))
		{
			for (i = 0; i < num; i++)
			{
				sequencer->description_sequence(i, &fldStr, NULL, NULL, NULL, NULL, NULL);
				listedString = fldStr.std_str();
				if (listedString == argString)
				{
					idS = i;
					break;
				}
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(idS))
		{
			if (idS < num)
			{
				res = sequencer->mark_sequence_process(idS, false);
				if (res != JVX_NO_ERROR)
				{
					errTxt = "Failed to mark sequence id #";
					errTxt += jvx_size2String(idS);
					errTxt += " for processing, reason: ";
					errTxt += jvxErrorType_descr(res);
					JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
				}
			}
			else
			{
				errTxt = "Sequence id #";
				errTxt += jvx_size2String(idS);
				errTxt += " out of range. There are only ";
				errTxt += jvx_size2String(num);
				errTxt += " sequences available.";
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
			}
		}
		else
		{
			{
				errTxt = "Could not find sequence with identification <";
				errTxt += argString;
				errTxt += ">.";
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
			}
		}
		res = hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
	} // res = runtime.theHostRef->request_hidden_interface(JVX_INTERFACE
	else
	{
		errTxt = "Failed to obtain sequencer handle.";
		res = JVX_ERROR_INTERNAL;
		JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
	}
	return res;
}
jvxErrorType
CjvxHostJsonCommandsActShow::act_edit_add_step_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idS = JVX_SIZE_UNSELECTED;
	jvxSize num = JVX_SIZE_UNSELECTED;
	jvxBool err = false;
	IjvxSequencer* sequencer = NULL;
	std::string errTxt;

	jvxSequencerQueueType qTp = JVX_SEQUENCER_QUEUE_TYPE_NONE;
	jvxSequencerElementType elmTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	jvxComponentIdentification targetCp = JVX_COMPONENT_UNKNOWN;
	jvxComponentType tp = JVX_COMPONENT_UNKNOWN;
	std::string descr = "No description";
	jvxSize fId = JVX_SIZE_UNSELECTED;
	jvxInt64 timeout = -1;
	std::string inLabel = "UNKNOWN";
	std::string inLabelC1 = "UNKNOWN";
	std::string inLabelC2 = "UNKNOWN";
	jvxSize idSS = JVX_SIZE_UNSELECTED;
	jvxApiString  fldStr;
	std::string argStringSeqId;
	std::string listedString;
	jvxBool b_action = false;
	
	if (args.size() > off)
	{
		argStringSeqId = args[off];
	}
	off++;

	if (args.size() > off)
	{
		for (i = JVX_SEQUENCER_QUEUE_TYPE_NONE + 1; i < JVX_SEQUENCER_QUEUE_TYPE_LIMIT; i++)
		{
			if (args[off] == jvxSequencerQueueType_str[i].friendly)
			{
				qTp = (jvxSequencerQueueType)i;
				break;
			}
		}
		if (qTp == JVX_SEQUENCER_QUEUE_TYPE_NONE)
		{
			errTxt = "Specification " + args[off] + " is not a valid sequencer queue type.";
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	else
	{
		errTxt = "No sequencer queue type specified.";
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
	}
	off++;

	if (args.size() > off)
	{
		idSS = jvx_string2Size(args[off], err);
		if(err)
		{ 
			errTxt = "Invalid specification of position <" + args[off] + ">.";
			res = JVX_ERROR_PARSE_ERROR;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	off++;

	// ======================================================================================
	if (args.size() > off)
	{
		for (i = JVX_SEQUENCER_TYPE_COMMAND_NONE + 1; i < JVX_SEQUENCER_TYPE_COMMAND_LIMIT; i++)
		{
			if (args[off] == jvxSequencerElementType_txt(i))
			{
				elmTp = (jvxSequencerElementType)i;
				break;
			}
		}
		if (elmTp == JVX_SEQUENCER_TYPE_COMMAND_NONE)
		{
			errTxt = "Specification " + args[off] + " is not a valid sequencer element step type.";
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	off++;

	if (args.size() > off)
	{
		jvxComponentIdentification_decode(targetCp, args[off]);		
	}
	off++;

	// All other arguments are optional
	if (args.size() > off)
	{
		descr = args[off];
	}
	off++;

	if (args.size() > off)
	{
		fId = jvx_string2Size(args[off], err);
		if (err)
		{
			errTxt = "Invalid specification of function id <" + args[off] + ">.";
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	off++;

	if (args.size() > off)
	{
		timeout = jvx_string2Size(args[off], err);
		if (err)
		{
			errTxt = "Invalid specification of timeout <" + args[off] + ">.";
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	off++;

	if (args.size() > off)
	{
		inLabel = args[off];
	}
	off++;

	if (args.size() > off)
	{
		inLabelC1 = args[off];
	}
	off++;

	if (args.size() > off)
	{
		inLabelC2 = args[off];
	}
	off++;

	// ==========================================================================================================

	res = hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if ((res == JVX_NO_ERROR) && sequencer)
	{
		sequencer->number_sequences(&num);

		idS = jvx_string2Size(argStringSeqId, err);
		if (err)
		{
			idS = JVX_SIZE_UNSELECTED;
		}

		if (JVX_CHECK_SIZE_UNSELECTED(idS))
		{
			for (i = 0; i < num; i++)
			{
				sequencer->description_sequence(i, &fldStr, NULL, NULL, NULL, NULL);
				listedString = fldStr.std_str();
				if (listedString == argStringSeqId)
				{
					idS = i;
					break;
				}
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(idS))
		{
			if (idS < num)
			{
				res = sequencer->insert_step_sequence_at(idS, qTp, elmTp, targetCp, descr.c_str(), fId, timeout, inLabel.c_str(), inLabelC1.c_str(), inLabelC2.c_str(), b_action, idSS);
				if (res != JVX_NO_ERROR)
				{
					errTxt = "Failed to insert step in sequence #";
					errTxt += jvx_size2String(idS);
					errTxt += ", reason: ";
					errTxt += jvxErrorType_descr(res);
					JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
				}
			}
			else
			{
				errTxt = "Sequence id #";
				errTxt += jvx_size2String(idS);
				errTxt += " out of range. There are only ";
				errTxt += jvx_size2String(num);
				errTxt += " sequences available.";
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);

			}
			res = hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
		} // res = runtime.theHostRef->request_hidden_interface(JVX_INTERFACE
		else
		{
			errTxt = "Failed to obtain sequencer handle.";
			res = JVX_ERROR_INTERNAL;
			JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
		}
	}
	return res;
}

jvxErrorType
CjvxHostJsonCommandsActShow::act_edit_modify_step_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idS = JVX_SIZE_UNSELECTED;
	jvxSize num = JVX_SIZE_UNSELECTED;
	jvxBool err = false;
	IjvxSequencer* sequencer = NULL;
	std::string errTxt;

	jvxSequencerQueueType qTp = JVX_SEQUENCER_QUEUE_TYPE_NONE;
	jvxSequencerElementType elmTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	jvxComponentIdentification targetCp = JVX_COMPONENT_UNKNOWN;
	jvxComponentType tp = JVX_COMPONENT_UNKNOWN;
	std::string descr = "No description";
	jvxSize fId = JVX_SIZE_UNSELECTED;
	jvxInt64 timeout = -1;
	std::string inLabel = "UNKNOWN";
	std::string inLabelC1 = "UNKNOWN";
	std::string inLabelC2 = "UNKNOWN";
	jvxSize idSS = JVX_SIZE_UNSELECTED;
	jvxApiString  fldStr;
	jvxBool b_action = false;
	std::string argStringSeqId;
	std::string listedString;

	if (args.size() > off)
	{
		argStringSeqId = args[off];
	}
	off++;

	if (args.size() > off)
	{
		for (i = JVX_SEQUENCER_QUEUE_TYPE_NONE + 1; i < JVX_SEQUENCER_QUEUE_TYPE_LIMIT; i++)
		{
			if (args[off] == jvxSequencerQueueType_str[i].friendly)
			{
				qTp = (jvxSequencerQueueType)i;
				break;
			}
		}
		if (qTp == JVX_SEQUENCER_QUEUE_TYPE_NONE)
		{
			errTxt = "Specification " + args[off] + " is not a valid sequencer queue type.";
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	else
	{
		errTxt = "No sequencer queue type specified.";
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
	}
	off++;

	if (args.size() > off)
	{
		idSS = jvx_string2Size(args[off], err);
		if (err)
		{
			errTxt = "Invalid specification of position <" + args[off] + ">.";
			res = JVX_ERROR_PARSE_ERROR;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	off++;

	// ======================================================================================
	if (args.size() > off)
	{
		for (i = JVX_SEQUENCER_TYPE_COMMAND_NONE + 1; i < JVX_SEQUENCER_TYPE_COMMAND_LIMIT; i++)
		{
			if (args[off] == jvxSequencerElementType_txt(i))
			{
				elmTp = (jvxSequencerElementType)i;
				break;
			}
		}
		if (elmTp == JVX_SEQUENCER_TYPE_COMMAND_NONE)
		{
			errTxt = "Specification " + args[off] + " is not a valid sequencer element step type.";
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	off++;

	if (args.size() > off)
	{
		jvxComponentIdentification_decode(targetCp, args[off]);
	}
	off++;

	// All other arguments are optional
	if (args.size() > off)
	{
		descr = args[off];
	}
	off++;

	if (args.size() > off)
	{
		fId = jvx_string2Size(args[off], err);
		if (err)
		{
			errTxt = "Invalid specification of function id <" + args[off] + ">.";
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	off++;

	if (args.size() > off)
	{
		timeout = jvx_string2Size(args[off], err);
		if (err)
		{
			errTxt = "Invalid specification of timeout <" + args[off] + ">.";
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	off++;

	if (args.size() > off)
	{
		inLabel = args[off];
	}
	off++;

	if (args.size() > off)
	{
		inLabelC1 = args[off];
	}
	off++;

	if (args.size() > off)
	{
		inLabelC2 = args[off];
	}
	off++;

	// ======================================================================================

	res = hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if ((res == JVX_NO_ERROR) && sequencer)
	{
		sequencer->number_sequences(&num);

		idS = jvx_string2Size(argStringSeqId, err);
		if (err)
		{
			idS = JVX_SIZE_UNSELECTED;
		}

		if (JVX_CHECK_SIZE_UNSELECTED(idS))
		{
			for (i = 0; i < num; i++)
			{
				sequencer->description_sequence(i, &fldStr, NULL,  NULL, NULL, NULL, NULL);
				listedString = fldStr.std_str();
				if (listedString == argStringSeqId)
				{
					idS = i;
					break;
				}
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(idS))
		{
			if (idS < num)
			{
				res = sequencer->edit_step_sequence(idS, qTp, idSS, elmTp, targetCp, descr.c_str(), fId, timeout, inLabel.c_str(), inLabelC1.c_str(), inLabelC2.c_str(), b_action);
				if (res != JVX_NO_ERROR)
				{
					errTxt = "Failed to insert step in sequence #";
					errTxt += jvx_size2String(idS);
					errTxt += ", reason: ";
					errTxt += jvxErrorType_descr(res);
					JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
				}
			}
			else
			{
				errTxt = "Sequence id #";
				errTxt += jvx_size2String(idS);
				errTxt += " out of range. There are only ";
				errTxt += jvx_size2String(num);
				errTxt += " sequences available.";
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);

			}
			res = hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
		} // res = runtime.theHostRef->request_hidden_interface(JVX_INTERFACE
		else
		{
			errTxt = "Failed to obtain sequencer handle.";
			res = JVX_ERROR_INTERNAL;
			JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
		}
	}
	return res;
}

jvxErrorType
CjvxHostJsonCommandsActShow::act_edit_remove_step_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idS = JVX_SIZE_UNSELECTED;
	jvxSize num = JVX_SIZE_UNSELECTED;
	jvxBool err = false;
	IjvxSequencer* sequencer = NULL;
	std::string errTxt;

	jvxSequencerQueueType qTp = JVX_SEQUENCER_QUEUE_TYPE_NONE;
	jvxSize idSS = JVX_SIZE_UNSELECTED;
	jvxApiString  fldStr;
	std::string argStringSeqId;
	std::string listedString;

	if (args.size() > off)
	{
		argStringSeqId = args[off];
	}
	off++;

	if (args.size() > off)
	{
		for (i = JVX_SEQUENCER_QUEUE_TYPE_NONE + 1; i < JVX_SEQUENCER_QUEUE_TYPE_LIMIT; i++)
		{
			if (args[off] == jvxSequencerQueueType_str[i].friendly)
			{
				qTp = (jvxSequencerQueueType)i;
				break;
			}
		}
		if (qTp == JVX_SEQUENCER_QUEUE_TYPE_NONE)
		{
			errTxt = "Specification " + args[off] + " is not a valid sequencer queue type.";
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	else
	{
		errTxt = "No sequencer queue type specified.";
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
	}
	off++;

	if (args.size() > off)
	{
		idSS = jvx_string2Size(args[off], err);
		if (err)
		{
			errTxt = "Invalid specification of position <" + args[off] + ">.";
			res = JVX_ERROR_PARSE_ERROR;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}

	// ==========================================================================================================

	res = hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if ((res == JVX_NO_ERROR) && sequencer)
	{
		sequencer->number_sequences(&num);

		idS = jvx_string2Size(argStringSeqId, err);
		if (err)
		{
			idS = JVX_SIZE_UNSELECTED;
		}

		if (JVX_CHECK_SIZE_UNSELECTED(idS))
		{
			for (i = 0; i < num; i++)
			{
				sequencer->description_sequence(i, &fldStr, NULL, NULL, NULL, NULL, NULL);
				listedString = fldStr.std_str();
				if (listedString == argStringSeqId)
				{
					idS = i;
					break;
				}
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(idS))
		{
			if (idS < num)
			{
				res = sequencer->remove_step_sequence(idS, qTp, idSS);
				if (res != JVX_NO_ERROR)
				{
					errTxt = "Failed to remove step in sequence #";
					errTxt += jvx_size2String(idS);
					errTxt += ", reason: ";
					errTxt += jvxErrorType_descr(res);
					JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
				}
			}
			else
			{
				errTxt = "Sequence id #";
				errTxt += jvx_size2String(idS);
				errTxt += " out of range. There are only ";
				errTxt += jvx_size2String(num);
				errTxt += " sequences available.";
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);

			}
			res = hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
		} // res = runtime.theHostRef->request_hidden_interface(JVX_INTERFACE
		else
		{
			errTxt = "Failed to obtain sequencer handle.";
			res = JVX_ERROR_INTERNAL;
			JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
		}
	}
	return res;
}

jvxErrorType
CjvxHostJsonCommandsActShow::act_edit_switch_step_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idS = JVX_SIZE_UNSELECTED;
	jvxSize num = JVX_SIZE_UNSELECTED;
	jvxBool err = false;
	IjvxSequencer* sequencer = NULL;
	std::string errTxt;

	jvxSequencerQueueType qTp = JVX_SEQUENCER_QUEUE_TYPE_NONE;
	jvxSize idSSFrom = JVX_SIZE_UNSELECTED;
	jvxSize idSSTo = JVX_SIZE_UNSELECTED;
	jvxApiString  fldStr;
	std::string argStringSeqId;
	std::string listedString;

	if (args.size() > off)
	{
		argStringSeqId = args[off];
	}
	off++;

	if (args.size() > off)
	{
		for (i = JVX_SEQUENCER_QUEUE_TYPE_NONE + 1; i < JVX_SEQUENCER_QUEUE_TYPE_LIMIT; i++)
		{
			if (args[off] == jvxSequencerQueueType_str[i].friendly)
			{
				qTp = (jvxSequencerQueueType)i;
				break;
			}
		}
		if (qTp == JVX_SEQUENCER_QUEUE_TYPE_NONE)
		{
			errTxt = "Specification " + args[off] + " is not a valid sequencer queue type.";
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	else
	{
		errTxt = "No sequencer queue type specified.";
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
	}
	off++;

	if (args.size() > off)
	{
		idSSFrom = jvx_string2Size(args[off], err);
		if (err)
		{
			errTxt = "Invalid specification of source position <" + args[off] + ">.";
			res = JVX_ERROR_PARSE_ERROR;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}
	off++; 
	if (args.size() > off)
	{
		idSSTo = jvx_string2Size(args[off], err);
		if (err)
		{
			errTxt = "Invalid specification of target position <" + args[off] + ">.";
			res = JVX_ERROR_PARSE_ERROR;
			JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
		}
	}

	// ==========================================================================================================

	res = hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if ((res == JVX_NO_ERROR) && sequencer)
	{
		sequencer->number_sequences(&num);

		idS = jvx_string2Size(argStringSeqId, err);
		if (err)
		{
			idS = JVX_SIZE_UNSELECTED;
		}

		if (JVX_CHECK_SIZE_UNSELECTED(idS))
		{
			for (i = 0; i < num; i++)
			{
				sequencer->description_sequence(i, &fldStr, NULL, NULL, NULL, NULL, NULL);
				listedString = fldStr.std_str();
				if (listedString == argStringSeqId)
				{
					idS = i;
					break;
				}
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(idS))
		{
			if (idS < num)
			{
				res = sequencer->switch_steps_sequence(idS, qTp, idSSFrom, idSSTo);
				if (res != JVX_NO_ERROR)
				{
					errTxt = "Failed to remove step in sequence #";
					errTxt += jvx_size2String(idS);
					errTxt += ", reason: ";
					errTxt += jvxErrorType_descr(res);
					JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
				}
			}
			else
			{
				errTxt = "Sequence id #";
				errTxt += jvx_size2String(idS);
				errTxt += " out of range. There are only ";
				errTxt += jvx_size2String(num);
				errTxt += " sequences available.";
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);

			}
			res = hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
		} // res = runtime.theHostRef->request_hidden_interface(JVX_INTERFACE
		else
		{
			errTxt = "Failed to obtain sequencer handle.";
			res = JVX_ERROR_INTERNAL;
			JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
		}
	}
	return res;
}

jvxErrorType
CjvxHostJsonCommandsActShow::act_single_component(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, 
	const std::string& addArg, jvxSize off, CjvxJsonElementList& jelmret, jvxBool* systemUpdate)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num, i,j;
	jvxSize idS = JVX_SIZE_UNSELECTED;
	jvxSize idComp = JVX_SIZE_UNSELECTED;
	std::string actionString;
	std::string listComponent;
	std::string firstArg, secondArg, thirdArg, fourthArg, fifthArg;
	jvxBool foundOperation = false;
	jvxState stat = JVX_STATE_NONE;
	jvxStateSwitch theSwitch = JVX_STATE_SWITCH_NONE;
	std::string errTxt;

	if (args.size() > off)
	{
		actionString = args[off];
		if (args.size() > off + 1)
		{
			firstArg = args[off+1];
		}
		if (args.size() > off + 2)
		{
			secondArg = args[off + 2];
		}
		if (args.size() > off + 3)
		{
			thirdArg = args[off + 3];
		}
		if (args.size() > off + 4)
		{
			fourthArg = args[off + 4];
		}
		if (args.size() > off + 5)
		{
			fifthArg = args[off + 5];
		}
	}

	// We have a valid component type specifiction and a valid component descriptor selection
	// Next, check WHAT to do..

	if (actionString == "get_property")
	{
		jvxBool content_only = false;
		jvxBool compact = false;
		for (j = 0; j < addArg.size(); j++)
		{
			switch (addArg[j])
			{
			case 'c':
				content_only = true;
				break;
			case 'C':
				compact = true;
				break;
			default:
				// Unknown option ignored
				break;

			}				
		}

		res = act_get_property_component(dh_command, args, off+1, jelmret, content_only, compact);
		foundOperation = true;
	}	

	if (!foundOperation)
	{
		if (actionString == "set_property")
		{
			jvxBool report = false;
			if (addArg == "r")
			{
				report = true;
			}
			res = act_set_property_component(dh_command, firstArg, secondArg, thirdArg, jelmret, report);
			foundOperation = true;
		}
	}

	// All non-specific names may be a state switch
	if(!foundOperation)
	{
		for (i = 1; i < JVX_STATE_SWITCH_LIMIT; i++)
		{
			if (actionString == jvxStateSwitch_str[i].friendly)
			{
				theSwitch = (jvxStateSwitch)i;
				foundOperation = true;
				break;
			}
		}
	
		if (foundOperation)
		{
			res = act_state_switch_component(dh_command, firstArg, theSwitch, jelmret, secondArg, thirdArg, systemUpdate);
		}
	}

	if (!foundOperation)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		errTxt = "The requested operation <" + actionString + "> is not known.";
		JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
	}
	return res;
}

jvxErrorType
CjvxHostJsonCommandsActShow::act_edit_connection(const oneDrivehostCommand& dh_command, 
	const std::vector<std::string>& args, jvxSize off, 
	CjvxJsonElementList& jelmret)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num, i;
	jvxSize idS = JVX_SIZE_UNSELECTED;
	jvxSize idComp = JVX_SIZE_UNSELECTED;
	std::string actionString;
	std::string listComponent;
	std::string firstArg, secondArg, thirdArg, fourthArg;
	jvxBool foundOperation = false;
	jvxState stat = JVX_STATE_NONE;
	IjvxDataConnections* connections = NULL;
	jvxStateSwitch theSwitch = JVX_STATE_SWITCH_NONE;
	std::string errTxt;
	jvxSize idConnection = JVX_SIZE_UNSELECTED;
	IjvxDataConnectionProcess* proc = NULL;
	jvxApiString fldStr;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	assert(hHost);
	
	if (args.size() > off)
	{
		actionString = args[off];
		if (args.size() > off + 1)
		{
			firstArg = args[off + 1];
		}
		if (args.size() > off + 2)
		{
			secondArg = args[off + 2];
		}
		if (args.size() > off + 3)
		{
			thirdArg = args[off + 3];
		}
		if (args.size() > off + 4)
		{
			fourthArg = args[off + 4];
		}
	}

	if (actionString == "create")
	{
		foundOperation = true;
		res = hHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&connections));
		if ((res == JVX_NO_ERROR) && connections)
		{
			jvxBool interceptors = jvx_check_yes(secondArg);
			jvxBool essential = jvx_check_yes(thirdArg);
			res = connections->create_connection_process(firstArg.c_str(), NULL, interceptors, essential, false, false);
			if (res != JVX_NO_ERROR)
			{
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, "Failed to create connection process with specification <" + firstArg + ">.");
				goto exit_fail;
			}
			res = hHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(connections));
			connections = NULL;
		}
	}
	else
	{
		// We have a valid component type specifiction and a valid component descriptor selection
		// Next, check WHAT to do..
		jvxBool err = false;
		idConnection = jvx_string2Size(secondArg, err);
		if (err)
		{
			// Just list all connections
			res = hHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&connections));
			if ((res == JVX_NO_ERROR) && connections)
			{
				CjvxJsonArray jarr;
				connections->number_connections_process(&num);
				for (i = 0; i < num; i++)
				{
					connections->reference_connection_process(i, &proc);
					if (proc)
					{
						proc->descriptor_connection(&fldStr);
						connections->return_reference_connection_process(proc);
						proc = NULL;
						if (fldStr.std_str() == secondArg)
						{
							idConnection = i;
							break;
						}
					}
				}
				res = hHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(connections));
				connections = NULL;
			}
		}

		if (JVX_CHECK_SIZE_SELECTED(idConnection))
		{
			res = hHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&connections));
			if ((res == JVX_NO_ERROR) && connections)
			{
				connections->reference_connection_process(idConnection, &proc);
				if (proc)
				{
					if ((actionString == "disconnect"))
					{
						foundOperation = true;
						proc->status(&stat);
						if (stat == JVX_STATE_ACTIVE)
						{
							res = proc->remove_connection();
							if (res != JVX_NO_ERROR)
							{
								JVX_CREATE_ERROR_NO_RETURN(jelmret, res, "Failed to disconnect connection process with specification <" + secondArg + ">.");
								goto exit_fail;
							}
						}
						else
						{
							JVX_CREATE_ERROR_NO_RETURN(jelmret, res, "Failed to disconnect connection process with specification <" + secondArg + ">.");
							goto exit_fail;
						}
					}

					if (actionString == "remove")
					{
						foundOperation = true;
						proc->status(&stat);
						if (stat == JVX_STATE_ACTIVE)
						{
							res = proc->remove_connection();
							if (res != JVX_NO_ERROR)
							{
								
							}
						}
						else
						{
						}

						// The remove part
						proc->status(&stat);
						if (stat == JVX_STATE_INIT)
						{
							proc->unique_id_connections(&uId);

							connections->return_reference_connection_process(proc);
							proc = NULL;

							res = connections->remove_connection_process(uId);
							if (res != JVX_NO_ERROR)
							{
								JVX_CREATE_ERROR_NO_RETURN(jelmret, res, "Failed to remove connection process with specification <" + secondArg + ">.");
								goto exit_fail;
							}
						}
						else
						{
							JVX_CREATE_ERROR_NO_RETURN(jelmret, res, "Failed to disconnect connection process with specification <" + secondArg + ">.");
							goto exit_fail;
						}
					}

					if (proc)
					{
						connections->return_reference_connection_process(proc);
					}
					proc = NULL;
				}
				res = hHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(connections));
				connections = NULL;
			}
		}
	}

	if (!foundOperation)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		errTxt = "The requested operation <" + actionString + "> is not known.";
		JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
	}
	return res;

exit_fail:
	if (proc)
	{
		connections->return_reference_connection_process(proc);
		proc = NULL;
	}
	if (connections)
	{
		res = hHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(connections));
		connections = NULL;
	}
	return res;

}

jvxErrorType
CjvxHostJsonCommandsActShow::act_edit_dropzone(const oneDrivehostCommand& dh_command,
	const std::vector<std::string>& args, jvxSize off,
	CjvxJsonElementList& jelmret)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	std::string arg1, arg2, arg3, arg4, arg5, arg6;
	IjvxDataConnections* connections = NULL;
	std::string errTxt;
	jvxApiString fldStr;

	if (args.size() > off)
	{
		arg1 = args[off];
		if (args.size() > off + 1)
		{
			arg2 = args[off + 1];
		}
		if (args.size() > off + 2)
		{
			arg3 = args[off + 2];
		}
		if (args.size() > off + 3)
		{
			arg4 = args[off + 3];
		}
		if (args.size() > off + 4)
		{
			arg5 = args[off + 4];
		}
		if (args.size() > off + 5)
		{
			arg6 = args[off + 5];
		}
	}
	assert(hHost);

	res = hHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&connections));
	if ((res == JVX_NO_ERROR) && connections)
	{
		
		if (arg1 == "add_master")
		{
			// arg1: command;
			// arg2..: depending
			// 1) add master 
			// arg2: Name master factory; arg3: Name master; arg4<opt>: component id 
			resL = act_add_master_dz(arg2, arg3, arg4, connections, errTxt);
			if (resL != JVX_NO_ERROR)
			{
				JVX_CREATE_ERROR_NO_RETURN(jelmret, resL, errTxt);
				goto exit_fail;
			}
		}
		else if(arg1 == "add_ocon")
		{
			// 2) add out connector
			// arg2: Name connection factory; arg3: Name connector; arg4<opt>: component id 
			resL = act_add_ocon_dz(arg2, arg3, arg4, connections, errTxt);
			if (resL != JVX_NO_ERROR)
			{
				JVX_CREATE_ERROR_NO_RETURN(jelmret, resL, errTxt);
				goto exit_fail;
			}
		}
		else if (arg1 == "add_icon")
		{
			// 3) add in connector
			// arg2: Name connection factory; arg3: Name connector; arg4<opt>: component id 
			resL = act_add_icon_dz(arg2, arg3, arg4, connections, errTxt);
			if (resL != JVX_NO_ERROR)
			{
				JVX_CREATE_ERROR_NO_RETURN(jelmret, resL, errTxt);
				goto exit_fail;
			}
		}
		else if (arg1 == "add_bridge")
		{
			// 4) Connect bridge
			// arg2: id factory from; arg3: id connector from;
			// arg4: id factory to; arg5: id connector to; arg6: Name bridge
			resL = act_create_bridge_dz(arg2, arg3, arg4, errTxt);
			if (resL != JVX_NO_ERROR)
			{
				JVX_CREATE_ERROR_NO_RETURN(jelmret, resL, errTxt);
				goto exit_fail;
			}
		}
		else if (arg1 == "rem_bridge")
		{
			// 5) Disconnect bridge
			// arg2: id bridge
			resL = act_disconnect_bridge_dz(arg2, errTxt);
			if (resL != JVX_NO_ERROR)
			{
				JVX_CREATE_ERROR_NO_RETURN(jelmret, resL, errTxt);
				goto exit_fail;
			}
		}
		else if (arg1 == "process_connect")
		{
			// 6) Connection start
			// arg2: Connection name or id
			resL = act_process_connect_dz(arg2, arg3, connections, errTxt);
			if (resL != JVX_NO_ERROR)
			{
				JVX_CREATE_ERROR_NO_RETURN(jelmret, resL, errTxt);
				goto exit_fail;
			}
		}
		else if (arg1 == "process_create_connect")
		{
			// 7) Connection create and start
			// arg2: connection name
			resL = act_connection_create_connect_dz(arg2, arg3, arg4, arg5, connections, errTxt);
			if (resL != JVX_NO_ERROR)
			{
				JVX_CREATE_ERROR_NO_RETURN(jelmret, resL, errTxt);
				goto exit_fail;
			}
		}
		else if (arg1 == "reset")
		{
			// 8) Clear dropzone
			// Clear all values to initial in dropzone
			jvx_reset_dropzone();
		}
		else
		{
			JVX_CREATE_ERROR_NO_RETURN(jelmret, JVX_ERROR_ELEMENT_NOT_FOUND, "Unknown command specification <" + arg1 + ">.");
			goto exit_fail;
		}
		res = hHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(connections));
	}
	else
	{
		JVX_CREATE_ERROR_RETURN(jelmret, JVX_ERROR_INVALID_ARGUMENT, "Option <" + arg1 + "> is not valid.");
	}
	return JVX_NO_ERROR;
exit_fail:
	if (connections)
	{
		hHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(connections));
	}
	return res;
}

jvxErrorType
CjvxHostJsonCommandsActShow::act_add_master_dz(const std::string& arg2, const std::string& arg3, const std::string& arg4,
	IjvxDataConnections* connections, std::string& errTxt)
{
	jvxErrorType resL = JVX_NO_ERROR;
	IjvxConnectionMasterFactory* masFac = NULL;
	IjvxConnectionMaster* mas = NULL;
	jvxComponentIdentification tpId;
	jvxComponentIdentification tpIdL;
	jvxApiString strMF;
	jvxBool foundit = false;
	jvxBool inUse = false;
	jvxBool isDuplicate = false;
	jvxSize num, numcons, i, j;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxApiString fldStr;
	IjvxDataConnectionProcess* proc = NULL;

	if (!arg4.empty())
	{
		resL = jvxComponentIdentification_decode(tpId, arg4);
		if (resL != JVX_NO_ERROR)
		{
			errTxt = "Expression <" + arg4 + "> is not a valid component identification.";
			return resL;
		}
	}

	foundit = false;
	connections->number_connection_master_factories(&num);
	for (i = 0; i < num; i++)
	{
		connections->reference_connection_master_factory(i, &masFac, &uId);
		if (masFac)
		{
			jvx_request_interfaceToObject(masFac, NULL, &tpIdL, &strMF);
			if (strMF.std_str() == arg2)
			{
				if (tpId.tp == JVX_COMPONENT_UNKNOWN)
				{
					foundit = true;
				}
				else
				{
					if (tpIdL.tp == tpId.tp)
					{
						if ((tpIdL.slotid == tpId.slotid) || (tpIdL.slotid == JVX_SIZE_DONTCARE))
						{
							if ((tpIdL.slotsubid == tpId.slotsubid) || (tpIdL.slotsubid == JVX_SIZE_DONTCARE))
							{
								foundit = true;
							}
						}
					}
				}
			}
			if (foundit)
			{
				foundit = false;
				masFac->number_connector_masters(&numcons);
				for (j = 0; j < numcons; j++)
				{
					masFac->reference_connector_master(j, &mas);
					if (mas)
					{
						mas->descriptor_master(&fldStr);
						mas->associated_process(&proc);
						masFac->return_reference_connector_master(mas);
						mas = NULL;

						if (fldStr.std_str() == arg3)
						{
							if (proc)
							{
								inUse = true;
							}
							else
							{
								lst_elm_master newElm;
								newElm.identify.mas_fac_uid = uId;
								newElm.identify.mas_id = j;

								auto elm = lst_masters.begin();
								for (; elm != lst_masters.end(); elm++)
								{
									if (
										(elm->identify.mas_fac_uid == newElm.identify.mas_fac_uid) &&
										(elm->identify.mas_id == newElm.identify.mas_id))
									{
										isDuplicate = true;
										break;
									}
								}
								if (!isDuplicate)
								{
									lst_masters.push_back(newElm);
								}
							}
							foundit = true;
							break;
						}
					}
				}
			}
			connections->return_reference_connection_master_factory(masFac);
			masFac = NULL;
		}
		if (foundit)
		{
			break;
		}
	} // for (i = 0; i < num; i++)
	if (!foundit)
	{
		errTxt = "Specified master connector not found.";
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	if (inUse)
	{
		errTxt = "Specified master connector already in use.";
		return JVX_ERROR_ALREADY_IN_USE;
	}
	if (isDuplicate)
	{
		errTxt = "Specified master connector has already been added.";
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsActShow::act_add_ocon_dz(const std::string& arg2, const std::string& arg3, const std::string& arg4,
	IjvxDataConnections* connections, std::string& errTxt)
{
	jvxErrorType resL = JVX_NO_ERROR;
	IjvxConnectorFactory* conFac = NULL;
	IjvxInputConnectorSelect* icons = NULL;
	jvxComponentIdentification tpId;
	jvxComponentIdentification tpIdL;
	IjvxDataConnectionCommon* proc = NULL;
	jvxApiString strMF;
	jvxBool foundit = false;
	jvxSize num, numcons, i, j;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxApiString fldStr;
	jvxBool inUse = false;
	jvxBool isDuplicate = false;

	if (!arg4.empty())
	{
		resL = jvxComponentIdentification_decode(tpId, arg4);
		if (resL != JVX_NO_ERROR)
		{
			errTxt = "Expression <" + arg4 + "> is not a valid component identification.";
			return resL;
		}
	}

	foundit = false;
	connections->number_connection_factories(&num);
	for (i = 0; i < num; i++)
	{
		connections->reference_connection_factory(i, &conFac, &uId);
		if (conFac)
		{
			jvx_request_interfaceToObject(conFac, NULL, &tpIdL, &strMF);
			if (strMF.std_str() == arg2)
			{
				if (tpId.tp == JVX_COMPONENT_UNKNOWN)
				{
					foundit = true;
				}
				else
				{
					if (tpIdL.tp == tpId.tp)
					{
						if ((tpIdL.slotid == tpId.slotid) || (tpIdL.slotid == JVX_SIZE_DONTCARE))
						{
							if ((tpIdL.slotsubid == tpId.slotsubid) || (tpIdL.slotsubid == JVX_SIZE_DONTCARE))
							{
								foundit = true;
							}
						}
					}
				}
			}
			if (foundit)
			{
				foundit = false;
				conFac->number_input_connectors(&numcons);
				for (j = 0; j < numcons; j++)
				{
					conFac->reference_input_connector(j, &icons);
					if (icons)
					{
						icons->descriptor_connector(&fldStr);
						IjvxInputConnector* iconc = icons->reference_icon();
						if (iconc)
						{
							iconc->associated_common_icon(&proc);
						}
						conFac->return_reference_input_connector(icons);
						icons = NULL;

						if (fldStr.std_str() == arg3)
						{
							if (proc)
							{
								inUse = true;
							}
							else
							{
								lst_elm_icons newElm;
								newElm.identify.fac_uid = uId;
								newElm.identify.icon_id = j;
								auto elm = lst_inputs.begin();
								for (; elm != lst_inputs.end(); elm++)
								{
									if (
										(elm->identify.fac_uid == newElm.identify.fac_uid) &&
										(elm->identify.icon_id == newElm.identify.icon_id))
									{
										isDuplicate = true;
										break;
									}
								}
								if (!isDuplicate)
								{
									lst_inputs.push_back(newElm);
								}
							}
							foundit = true;
							break;
						}
					}
				}
			}
			connections->return_reference_connection_factory(conFac);
			conFac = NULL;
		}
		if (foundit)
		{
			break;
		}
	} // for (i = 0; i < num; i++)
	if (!foundit)
	{
		errTxt = "Specified output connector not found.";
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	if (inUse)
	{
		errTxt = "Specified output connector already in use.";
		return JVX_ERROR_ALREADY_IN_USE;
	}
	if (isDuplicate)
	{
		errTxt = "Specified output connector has already been added.";
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	return JVX_NO_ERROR;
}


jvxErrorType
CjvxHostJsonCommandsActShow::act_create_bridge_dz(const std::string& arg2, const std::string& arg3, 
	const std::string& arg4, std::string& errTxt)
{
	// arg2: id factory from; arg3: id connector from;
	// arg4: id factory to; arg5: id connector to; arg6: Name bridge
	jvxBool err = false;

	/*jvxErrorType resL = JVX_NO_ERROR;
	
	jvxComponentIdentification tpId;
	jvxComponentIdentification tpIdL;
	IjvxDataConnectionCommon* proc = NULL;
	jvxApiString strMF;*/
	IjvxConnectorFactory* conFac = NULL;
	IjvxInputConnector* icon = NULL;
	IjvxInputConnector* ocon = NULL;
	jvxBool foundit = false;
	jvxSize num, numcons, i, j;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	/*jvxApiString fldStr;*/
	jvxBool inUse = false;
	jvxBool isDuplicate = false;

	jvxSize id_from = jvx_string2Size(arg2, err);
	if (err)
	{
		errTxt = "Unable to convert argument 2, <" + arg2 + "> into a selection id.";
		return JVX_ERROR_PARSE_ERROR;
	}
	jvxSize id_to = jvx_string2Size(arg3, err);
	if (err)
	{
		errTxt = "Unable to convert argument 3, <" + arg3 + "> into a selection id.";
		return JVX_ERROR_PARSE_ERROR;
	}
	
	isDuplicate = false;
	std::string bridge_name = arg4;
	auto elmb = lst_bridges.begin();
	for (; elmb != lst_bridges.end(); elmb++)
	{
		if(elmb->bridge_name == arg4)
		{
			isDuplicate = true;
			break;
		}
	}
	if (isDuplicate)
	{
		errTxt = "Bridge with name <" + arg4 + "> has been defined before.";
		return JVX_ERROR_DUPLICATE_ENTRY;
	}

	auto elmo = lst_outputs.begin();
	std::advance(elmo, id_from);
	if (elmo == lst_outputs.end())
	{
		errTxt = "Unable to select output connector with id <" + jvx_size2String(id_from) + ">.";
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}

	inUse = false;
	elmb = lst_bridges.begin();
	for (; elmb != lst_bridges.end(); elmb++)
	{
		if (
			(elmo->identify.fac_uid == elmb->identify_out.fac_uid) &&
			(elmo->identify.ocon_id == elmb->identify_out.ocon_id))
		{
			inUse = true;
			break;
		}
	}

	if (inUse)
	{
		errTxt = "Specified output connector has been associated to the bridge <" + elmb->bridge_name + ">.";
		return JVX_ERROR_DUPLICATE_ENTRY;
	}

	auto elmi = lst_inputs.begin();
	std::advance(elmi, id_to);
	if (elmi == lst_inputs.end())
	{
		errTxt = "Unable to select input connector with id <" + jvx_size2String(id_to) + ">.";
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}

	inUse = false;
	elmb = lst_bridges.begin();
	for (; elmb != lst_bridges.end(); elmb++)
	{
		if (
			(elmi->identify.fac_uid == elmb->identify_in.fac_uid) &&
			(elmi->identify.icon_id == elmb->identify_in.icon_id))
		{
			inUse = true;
			break;
		}
	}

	if (inUse)
	{
		errTxt = "Specified input connector has been associated to the bridge <" + elmb->bridge_name + ">.";
		return JVX_ERROR_DUPLICATE_ENTRY;
	}


	lst_elm_bidges newB;
	newB.bridge_name = arg4;
	newB.identify_out.fac_uid = elmo->identify.fac_uid;
	newB.identify_out.ocon_id = elmo->identify.ocon_id;
	newB.identify_in.fac_uid = elmi->identify.fac_uid;
	newB.identify_in.icon_id = elmi->identify.icon_id;

	lst_bridges.push_back(newB);

	return JVX_NO_ERROR;
}


jvxErrorType
CjvxHostJsonCommandsActShow::act_disconnect_bridge_dz(const std::string& arg2, std::string& errTxt)
{
	// 5) Disconnect bridge
	// arg2: id bridge
	jvxBool err = false;
	jvxSize id_bridge = jvx_string2Size(arg2, err);
	if (err)
	{
		errTxt = "Unable to convert argument 2, <" + arg2 + "> into a selection id.";
		return JVX_ERROR_PARSE_ERROR;
	}

	auto elmb = lst_bridges.begin();
	std::advance(elmb, id_bridge);
	if (elmb != lst_bridges.end())
	{
		lst_bridges.erase(elmb);
		return JVX_NO_ERROR;
	}

	errTxt = "Could not address bridge with id <" + jvx_size2String(id_bridge) + ">, there are only " +
		jvx_size2String(lst_bridges.size()) + " bridges in the list.";
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}


jvxErrorType
CjvxHostJsonCommandsActShow::act_process_connect_dz(const std::string& arg2, const std::string& arg3, IjvxDataConnections* connections, std::string& errTxt)
{
	// 6) Connection start
	// arg2: Connection name or id
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool err = false;
	IjvxDataConnectionProcess* proc = NULL;
	jvxSize i = 0, num = 0;
	jvxState stat = JVX_STATE_NONE;
	jvxApiString fldStr;
	jvxSize id_master = jvx_string2Size(arg2, err);
	if (err)
	{
		errTxt = "Unable to convert argument 2, <" + arg2 + "> into a master selection.";
		return JVX_ERROR_PARSE_ERROR;
	}

	jvxSize id_proc = jvx_string2Size(arg3, err);
	if (err)
	{
		id_proc = JVX_SIZE_UNSELECTED;
		connections->number_connections_process(&num);
		for (i = 0; i < num; i++)
		{
			connections->reference_connection_process(i, &proc);
			if (proc)
			{
				proc->descriptor_connection(&fldStr);
				connections->return_reference_connection_process(proc);
				proc = NULL;

				if(fldStr.std_str() == arg3)
				{
					id_proc = i;
					break;
				}
			}
		}

		if (JVX_CHECK_SIZE_UNSELECTED(id_proc))
		{
			errTxt = "Unable to neither convert argument 2, <" + arg2 + "> into a selection id nor identify as a process name.";
			return JVX_ERROR_PARSE_ERROR;
		}
	}

	// Set selection id in master selection
	id_select_master = id_master;

	resL = connections->reference_connection_process(id_proc, &proc);
	if (
		(resL == JVX_NO_ERROR) && proc)
	{
		proc->status(&stat);
		if (stat == JVX_STATE_INIT)
		{
			resL = connect_process_from_dropzone(connections, proc JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
			if (resL != JVX_NO_ERROR)
			{
				errTxt = "Failed to connect connection process <" + jvx_size2String(id_proc) + ">, error: " + jvxErrorType_descr(resL);
				goto exit_fail;
			}
		}
		else
		{
			errTxt = "Failed to connect connection process <" + jvx_size2String(id_proc) + ">, process is not in the right state.";
			goto exit_fail;
		}
		connections->return_reference_connection_process(proc);
		proc = NULL;
	}
	jvx_cleanup_candidates_dropzone(connections);
	return JVX_NO_ERROR;
exit_fail:
	if (proc)
	{
		connections->return_reference_connection_process(proc);
	}
	proc = NULL;
	return resL;
}


jvxErrorType
CjvxHostJsonCommandsActShow::act_connection_create_connect_dz(const std::string& arg2, const std::string& arg3, 
	const std::string& arg4, const std::string& arg5, IjvxDataConnections* connections, std::string& errTxt)
{
	// 7) Connection create and start
	// arg2: connection name
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool err = false;
	IjvxDataConnectionProcess* proc = NULL;
	jvxSize num = 0;
	jvxState stat = JVX_STATE_NONE;
	jvxSize id_proc = jvx_string2Size(arg2, err);
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxSize id_master = jvx_string2Size(arg2, err);
	jvxBool interceptors = false;
	jvxBool essential = false;
	if (err)
	{
		errTxt = "Unable to convert argument 2, <" + arg2 + "> into a master selection.";
		return JVX_ERROR_PARSE_ERROR;
	}

	process_name = arg3;
	interceptors = jvx_check_yes(arg4);
	essential = jvx_check_yes(arg5);
	resL = connections->create_connection_process(process_name.c_str(), &uId, interceptors, essential, false, false);
	if (resL == JVX_NO_ERROR)
	{

		resL = connections->reference_connection_process_uid(uId, &proc);
		if (
			(resL == JVX_NO_ERROR) && proc)
		{
			proc->status(&stat);
			if (stat == JVX_STATE_INIT)
			{
				resL = connect_process_from_dropzone(connections, proc JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
				if (resL != JVX_NO_ERROR)
				{
					errTxt = "Failed to connect connection process <" + jvx_size2String(id_proc) + ">, error: " + jvxErrorType_descr(resL);
					goto exit_fail;
				}
			}
			else
			{
				errTxt = "Failed to connect connection process <" + jvx_size2String(id_proc) + ">, process is not in the right state.";
				goto exit_fail;
			}
			connections->return_reference_connection_process(proc);
			proc = NULL;
		}
		jvx_cleanup_candidates_dropzone(connections);
		return JVX_NO_ERROR;
	}
	else
	{
		errTxt = "Failed to create connection process with name <" + arg2 + ">, error: " + jvxErrorType_descr(resL);
		return resL;
	}
exit_fail:
	if (proc)
	{
		connections->return_reference_connection_process(proc);
	}
	proc = NULL;
	return resL;
}

jvxErrorType
CjvxHostJsonCommandsActShow::act_add_icon_dz(const std::string& arg2, const std::string& arg3, const std::string& arg4,
	IjvxDataConnections* connections, std::string& errTxt)
{
	jvxErrorType resL = JVX_NO_ERROR;
	IjvxConnectorFactory* conFac = NULL;
	IjvxOutputConnectorSelect* ocons = NULL;
	jvxComponentIdentification tpId;
	jvxComponentIdentification tpIdL;
	IjvxDataConnectionCommon* proc = NULL;
	jvxApiString strMF;
	jvxBool foundit = false;
	jvxSize num, numcons, i, j;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxApiString fldStr;
	jvxBool inUse = false;
	jvxBool isDuplicate = false;
	
	if (!arg4.empty())
	{
		resL = jvxComponentIdentification_decode(tpId, arg4);
		if (resL != JVX_NO_ERROR)
		{
			errTxt = "Expression <" + arg4 + "> is not a valid component identification.";
			return resL;
		}
	}

	foundit = false;
	connections->number_connection_factories(&num);
	for (i = 0; i < num; i++)
	{
		connections->reference_connection_factory(i, &conFac, &uId);
		if (conFac)
		{
			jvx_request_interfaceToObject(conFac, NULL, &tpIdL, &strMF);
			if (strMF.std_str() == arg2)
			{
				if (tpId.tp == JVX_COMPONENT_UNKNOWN)
				{
					foundit = true;
				}
				else
				{
					if (tpIdL.tp == tpId.tp)
					{
						if ((tpIdL.slotid == tpId.slotid) || (tpIdL.slotid == JVX_SIZE_DONTCARE))
						{
							if ((tpIdL.slotsubid == tpId.slotsubid) || (tpIdL.slotsubid == JVX_SIZE_DONTCARE))
							{
								foundit = true;
							}
						}
					}
				}
			}
			if (foundit)
			{
				foundit = false;
				conFac->number_input_connectors(&numcons);
				for (j = 0; j < numcons; j++)
				{
					conFac->reference_output_connector(j, &ocons);
					if (ocons)
					{
						ocons->descriptor_connector(&fldStr);
						IjvxOutputConnector* oconc = ocons->reference_ocon();
						if (oconc)
						{
							oconc->associated_common_ocon(&proc);
						}
						conFac->return_reference_output_connector(ocons);
						ocons = NULL;

						if (fldStr.std_str() == arg3)
						{
							if (proc)
							{
								inUse = true;
							}
							else
							{
								lst_elm_ocons newElm;
								newElm.identify.fac_uid = uId;
								newElm.identify.ocon_id = j;
								auto elm = lst_outputs.begin();
								for (; elm != lst_outputs.end(); elm++)
								{
									if (
										(elm->identify.fac_uid == newElm.identify.fac_uid) &&
										(elm->identify.ocon_id == newElm.identify.ocon_id))
									{
										isDuplicate = true;
										break;
									}
								}
								if (!isDuplicate)
								{
									lst_outputs.push_back(newElm);
								}
							}
							foundit = true;
							break;
						}
					}
				}
			}
			connections->return_reference_connection_factory(conFac);
			conFac = NULL;
		}
		if (foundit)
		{
			break;
		}
	} // for (i = 0; i < num; i++)
	if (!foundit)
	{
		errTxt = "Specified output connector not found.";
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	if (inUse)
	{
		errTxt = "Specified output connector already in use.";
		return JVX_ERROR_ALREADY_IN_USE;
	}
	if (isDuplicate)
	{
		errTxt = "Specified output connector has already been added.";
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsActShow::act_get_property_component(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, 
	jvxSize off, CjvxJsonElementList& jelmret, jvxBool content_only, jvxBool compact)
{
	jvxCallManagerProperties callGate;
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentIdentification tp((jvxComponentType)dh_command.subaddress, dh_command.subsubaddress, dh_command.subsubsubaddress);
	jvx_propertyReferenceTriple theTriple;
	jvxPropertyDescriptor theDescr;
	std::string value;
	jvxSize idS;
	jvxSize offsetS = 0;
	jvxSize numElms = JVX_SIZE_UNSELECTED;
	jvxBool err = false;
	jvxBool contentOnlyBool = false;
	std::string errTxt;
	jvxContext ctxt;
	jvxContext* ctxt_ptr = NULL;
	jvxSize offset = 0;
	std::string ctxtStr;

	if (JVX_CHECK_SIZE_SELECTED(dh_command.subsubaddress))
	{
		tp.slotid = dh_command.subsubaddress;
	}
	if (JVX_CHECK_SIZE_SELECTED(dh_command.subsubsubaddress))
	{
		tp.slotsubid = dh_command.subsubsubaddress;
	}

	if(args.size() > off)
	{
		std::string props = args[off];
		res = act_get_property_component_core(tp, props, args, off, jelmret, content_only, compact, errTxt);
		if (res != JVX_NO_ERROR)
		{
			JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
		}
	}
	else
	{
		// Error case
		res = show_property_component(tp, "", args, off, jelmret, content_only, compact, errTxt);
		if (res != JVX_NO_ERROR)
		{
			JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
		}
	}
	return res;

}

jvxErrorType
CjvxHostJsonCommandsActShow::act_set_property_component(const oneDrivehostCommand& dh_command, const std::string& identificationTarget,
	const std::string& loadTarget, const std::string& offsetStart, CjvxJsonElementList& jelmret, jvxBool reportSet)
{
	std::string errTxt;
	// Argument numElements does not really make sense: the number is inherent to passed string

	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentIdentification tp((jvxComponentType)dh_command.subaddress, dh_command.subsubaddress, dh_command.subsubsubaddress);

	res = act_set_property_component_core(tp, identificationTarget, loadTarget, offsetStart, jelmret, reportSet, errTxt);
	if (res != JVX_NO_ERROR)
	{
		JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
	}
	return res;
}

jvxErrorType
CjvxHostJsonCommandsActShow::act_state_switch_component(const oneDrivehostCommand& dh_command, 
	const std::string& identificationComponent, jvxStateSwitch theSwitch, CjvxJsonElementList& jelmret,
	const std::string& argSlotid, const std::string& argSlotSubid, jvxBool* systemUpdate)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString fldStr;
	jvxSize i, num;
	jvxSize idComp;
	jvxSize idS = JVX_SIZE_UNSELECTED;
	jvxSize idSlot = 0;
	jvxSize idSlotSub = 0;
	std::string listComponent;
	jvxComponentIdentification tp;
	jvxBool err = false;
	std::string errTxt;

	jvxSize szSlotid;
	jvxSize szSlotSubid;
	jvxBool doUpdate = false;

	// jvxState stat = JVX_STATE_NONE;
	if (hHost)
	{
		jvxComponentIdentification tp((jvxComponentType)dh_command.subaddress, dh_command.subsubaddress, dh_command.subsubsubaddress);
		//tpAll[dh_command.subaddress] = tp;

		if (!argSlotid.empty())
		{
			tp.slotid = jvx_string2Size(argSlotid, err);
			if (err)
			{
				errTxt = "Parse error when parsing slot id, expression <" + argSlotid + ">.";
				res = JVX_ERROR_PARSE_ERROR;
				JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
			}
		}
		if (!argSlotSubid.empty())
		{
			tp.slotsubid = jvx_string2Size(argSlotSubid, err);
			if (err)
			{
				errTxt = "Parse error when parsing slot sub id, expression <" + argSlotSubid + ">.";
				res = JVX_ERROR_PARSE_ERROR;
				JVX_CREATE_ERROR_RETURN(jelmret, res, errTxt);
			}
		}
		hHost->selection_component(tp, &idS);

		idComp = JVX_SIZE_UNSELECTED;
		hHost->number_components_system(tp, &num);
		for (i = 0; i < num; i++)
		{
			hHost->descriptor_component_system(tp, i, &fldStr);
			listComponent = fldStr.std_str();

			if (listComponent == identificationComponent)
			{
				idComp = i;
				break;
			}
		}

		if (JVX_CHECK_SIZE_UNSELECTED(idComp))
		{
			jvxBool err = false;
			if (identificationComponent.empty())
			{
				idComp = JVX_SIZE_UNSELECTED;
			}
			else
			{
				idComp = (jvxSize)jvx_string2Size(identificationComponent, err);
				if (err == true)
				{
					idComp = JVX_SIZE_UNSELECTED;
				}
			}
		}

		switch (theSwitch)
		{
		case JVX_STATE_SWITCH_NONE:
		
			return JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_STATE_SWITCH_SELECT:

			if (JVX_CHECK_SIZE_UNSELECTED(idComp))
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
				errTxt = "Failed to select component instance <" + identificationComponent + "> of type ";
				errTxt += jvxComponentType_txt(tp.tp);
				errTxt += ", reason: ";
				errTxt += jvxErrorType_descr(res);
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
			}
			else
			{
				if (JVX_CHECK_SIZE_SELECTED(idS))
				{
					// Unselect old instance
					hHost->unselect_selected_component(tp);
				}

				// Activate new instance
				res = hHost->select_component(tp, idComp);
				if (res == JVX_NO_ERROR)
				{
					doUpdate = true;
				}
				else
				{
					errTxt = "Failed to select component instance <" + identificationComponent + "> of type ";
					errTxt += jvxComponentType_txt(tp.tp);
					errTxt += ", reason: ";
					errTxt += jvxErrorType_descr(res);
					JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
				}
			}
			break;
		case JVX_STATE_SWITCH_UNSELECT:

			res = hHost->unselect_selected_component(tp);
			if (res == JVX_NO_ERROR)
			{
				hHost->number_slots_component_system(tp, &szSlotid, nullptr, nullptr, nullptr);
				if (szSlotid == 0)
				{
					tp.slotid = 0;
				}
				else
				{
					if (tp.slotid >= szSlotid)
					{
						tp.slotid = szSlotid - 1;
					}
				}
				hHost->number_slots_component_system(tp, nullptr,  &szSlotSubid, nullptr, nullptr);
				if (szSlotSubid == 0)
				{
					tp.slotsubid = 0;
				}
				else
				{
					if (tp.slotsubid >= szSlotSubid)
					{
						tp.slotsubid = szSlotSubid - 1;
					}
				}
				doUpdate = true;
			}
			else
			{
				errTxt = "Failed to activate selected component of type ";
				errTxt += jvxComponentType_txt(tp.tp);
				errTxt += ", reason: ";
				errTxt += jvxErrorType_descr(res);
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
			}
			break;

		case JVX_STATE_SWITCH_ACTIVATE:

			if (JVX_CHECK_SIZE_UNSELECTED(idComp) && (!identificationComponent.empty()))
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
				errTxt = "Failed to activate component with descriptor <" + identificationComponent + "> of type ";
				errTxt += jvxComponentType_txt(tp.tp);
				errTxt += ", reason: ";
				errTxt += jvxErrorType_descr(res);
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
			}
			else
			{
				if (JVX_CHECK_SIZE_SELECTED(idComp))
				{
					if (JVX_CHECK_SIZE_SELECTED(idS))
					{
						// Unselect old instance
						hHost->unselect_selected_component(tp);
						doUpdate = true;
					}

					// Activate new instance
					res = hHost->select_component(tp, idComp);
					if (res == JVX_NO_ERROR)
					{
						doUpdate = true;
					}
					else
					{
						errTxt = "Failed to select component with descriptor <" + identificationComponent + "> of type ";
						errTxt += jvxComponentType_txt(tp.tp);
						errTxt += ", reason: ";
						errTxt += jvxErrorType_descr(res);
						JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
					}
				}
			}
			res = hHost->activate_selected_component(tp);
			if (res == JVX_NO_ERROR)
			{
				doUpdate = true;
			}
			else
			{
				errTxt = "Failed to activate selected component of type ";
				errTxt += jvxComponentType_txt(tp.tp);
				errTxt += ", reason: ";
				errTxt += jvxErrorType_descr(res);
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
			}
			break;

		case JVX_STATE_SWITCH_DEACTIVATE:

			res = hHost->deactivate_selected_component(tp);
			if (res == JVX_NO_ERROR)
			{
				doUpdate = true;
			}
			else
			{
				errTxt = "Failed to activate selected component of type ";
				errTxt += jvxComponentType_txt(tp.tp);
				errTxt += ", reason: ";
				errTxt += jvxErrorType_descr(res);
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
			}
			break;
		case JVX_STATE_SWITCH_PREPARE:
		case JVX_STATE_SWITCH_START:
		case JVX_STATE_SWITCH_STOP:
		case JVX_STATE_SWITCH_POSTPROCESS:
			res = JVX_ERROR_NO_ACCESS;
			errTxt = "Failed to switch state of component of type ";
			errTxt += jvxComponentType_txt(tp.tp);
			errTxt += ", this state switch may only be done by the sequencer.";
			JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
			break;
		default:
			res = JVX_ERROR_UNSUPPORTED;
			errTxt = "Failed to switch state of component of type ";
			errTxt += jvxComponentType_txt(tp.tp);
			errTxt += ", state switches of type ";
			errTxt += jvxStateSwitch_txt(theSwitch);
			errTxt += " is not supported.";
			JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
		}
	}


	if (systemUpdate)
		*systemUpdate = doUpdate;

	return res;
}

jvxErrorType
CjvxHostJsonCommandsActShow::process_command_abstraction(
	const oneDrivehostCommand& dh_command, const std::vector<std::string>& args,
	const std::string& addArg, jvxSize off, CjvxJsonElementList& jsec, jvxBool* systemUpdate)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (dh_command.family)
	{
	case JVX_DRIVEHOST_FAMILY_ACT:
		switch (dh_command.address)
		{
		case JVX_DRIVEHOST_ADRESS_SEQUENCER:
			res = act_edit_sequencer(dh_command, args, off, jsec);
			break;
		case JVX_DRIVEHOST_ADRESS_SINGLE_COMPONENT:
			res = act_single_component(dh_command, args, addArg, off, jsec, systemUpdate);
			break;
		case JVX_DRIVEHOST_ADRESS_CONNECTIONS:
			res = act_edit_connection(dh_command, args, off, jsec);
			break;
		case JVX_DRIVEHOST_ADRESS_CONNECTION_DROPZONE:
			res = act_edit_dropzone(dh_command, args, off, jsec);
			break;
		default:
			return JVX_ERROR_UNSUPPORTED;
			break;
		}
		break;

	default:
		return CjvxHostJsonCommandsShow::process_command_abstraction(
			dh_command, args, addArg, off,
			jsec, systemUpdate);
		break;
	}

	return res;
}