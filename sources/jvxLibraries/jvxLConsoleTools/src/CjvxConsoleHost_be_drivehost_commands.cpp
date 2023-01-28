#include "CjvxConsoleHost_be_drivehost.h"
#include "CjvxHostJsonDefines.h"


// =======================================================================================
// =======================================================================================
// =======================================================================================
// =======================================================================================

void
CjvxConsoleHost_be_drivehost::process_full_command(const std::string& command, jvxSize message_id, jvxHandle* param, jvxSize paramType,
	jvxOneEventClass event_class, jvxOneEventPriority event_priority,
	IjvxEventLoop_frontend* origin, jvxHandle* privBlock, jvxCBitField* extFlags)
{
	jvxSize i;
	std::string f_expr;
	std::vector<std::string> args;
	std::string addArg;
	oneDrivehostCommand dh_command;
	CjvxJsonElementList jlstresp;
	std::string out_string;
	std::string errTxt;
	jvxSize off = 1;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool reportSystemUpdate = false;

	jvxBool translate_command_failed = false;
	jvxBool parse_error = false;
	jvxBool command_empty = false;
	jvxBool regular_command = false;
	jvxBool translate_firstarg_failed = false;

	translate_full_command(
		command, 
		dh_command,
		args,
		addArg,
		translate_command_failed,
		parse_error,
		command_empty,
		regular_command,
		translate_firstarg_failed);

	if(!command_empty)
	{
		if (regular_command)
		{
			res = process_command_abstraction(dh_command, args, addArg, off, jlstresp, extFlags, &reportSystemUpdate);
		}
		if (translate_firstarg_failed)
		{
			errTxt = "Failed to determine command addressing, <" + args[0] + "> is not a valid option, error: " + jvxErrorType_descr(res) + ".";
			JVX_CREATE_ERROR_NO_RETURN(jlstresp, res, errTxt);
		}
		if (translate_command_failed)
		{
			// this->translate_failed();
				// Pass command forward to 				
			jvxBool foundOne = false;
			std::list<jvxOneBackendAndState>::iterator elm = this->linkedSecBackends.begin();
			for (; elm != linkedSecBackends.end(); elm++)
			{
				TjvxEventLoopElement evelm;

				evelm.origin_fe = origin;
				evelm.priv_fe = NULL;
				evelm.target_be = elm->be;
				evelm.priv_be = NULL;

				evelm.param = (jvxHandle*)&command;
				evelm.paramType = JVX_EVENTLOOP_DATAFORMAT_STDSTRING;

				evelm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
				evelm.eventClass = JVX_EVENTLOOP_REQUEST_IMMEDIATE;
				evelm.eventPriority = JVX_EVENTLOOP_PRIORITY_UNKNOWN;
				evelm.delta_t = 0;
				evelm.autoDeleteOnProcess = c_false;
				jvxErrorType resL = evLop->event_schedule(&evelm, NULL, NULL);

				if (resL == JVX_NO_ERROR)
				{
					// This means that the command has been taken by one of the secondary backends
					return;
				}
				else
				{
					//
					//std::cout << jvxErrorType_txt(resL) << std::endl;
				}

			}

			TjvxEventLoopElement evelm;

			evelm.origin_fe = origin;
			evelm.priv_fe = NULL;
			evelm.target_be = NULL;
			evelm.priv_be = NULL;

			evelm.param = (jvxHandle*)&command;
			evelm.paramType = JVX_EVENTLOOP_DATAFORMAT_STDSTRING;

			evelm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
			evelm.eventClass = JVX_EVENTLOOP_REQUEST_IMMEDIATE;
			evelm.eventPriority = JVX_EVENTLOOP_PRIORITY_UNKNOWN;
			evelm.delta_t = 0;
			evelm.autoDeleteOnProcess = c_false;
			jvxErrorType resL = evLop->event_schedule(&evelm, NULL, NULL);

			if (resL == JVX_NO_ERROR)
			{
				// This means that the command has been taken by one of the secondary backends
				return;
			}

			errTxt = "Failed to determine command family, error: ";
			errTxt += jvxErrorType_descr(res);
			errTxt += ".";
			JVX_CREATE_ERROR_NO_RETURN(jlstresp, res, errTxt);
		}
		
		if (parse_error)
		{
			res = JVX_ERROR_PARSE_ERROR;
			errTxt = "Parse error when parsing command " + command;
			JVX_CREATE_ERROR_NO_RETURN(jlstresp, res, errTxt);
		}

		// Output result
		//outputResult(/*res,*/ strResponse, "\n\t\t", '\n', '\t');
		//printToStringView(jlstresp, out_string, res);
		
		// We add a final element here to indicate success
		jvxBool unmatchedFrontend = true;
		CjvxJsonElement jelm_result;
		jelm_result.makeAssignmentString("return_code", jvxErrorType_str[res].friendly);
		jlstresp.insertConsumeElementFront(jelm_result);

		if (linkedPriFrontend.fe)
		{
			if (origin == linkedPriFrontend.fe)
			{
				TjvxEventLoopElement qel;
				qel.message_id = message_id;
				qel.param = &jlstresp;
				qel.paramType = JVX_EVENTLOOP_DATAFORMAT_JVXJSONMULTFIELDS;
				qel.eventClass = event_class;
				qel.eventPriority = event_priority;
				qel.origin_fe = origin;

				linkedPriFrontend.fe->report_assign_output(&qel, res, privBlock);
				unmatchedFrontend = false;
			}
			else
			{
				// Report text outputs on console output only if not in silent_out mode
				if (!config_act.silent_out)
				{
					TjvxEventLoopElement qel;
					qel.message_id = message_id;
					qel.param = &jlstresp;
					qel.paramType = JVX_EVENTLOOP_DATAFORMAT_JVXJSONMULTFIELDS;
					qel.eventClass = event_class;
					qel.eventPriority = event_priority;
					qel.origin_fe = origin;

					linkedPriFrontend.fe->report_assign_output(&qel, res, NULL);					
				}
			}

			if (reportSystemUpdate)
			{
				jvxSpecialEventType specEvent;
				specEvent.ev = JVX_EVENTLOOP_SPECIAL_EVENT_SYSTEM_UPDATE;
				specEvent.dat = NULL;
				TjvxEventLoopElement qel;
				qel.message_id = message_id;
				qel.param = &specEvent;
				qel.paramType = JVX_EVENTLOOP_DATAFORMAT_SPECIAL_EVENT;
				qel.eventClass = event_class;
				qel.eventPriority = event_priority;
				qel.origin_fe = origin;

				linkedPriFrontend.fe->report_special_event(&qel, NULL);
			}
		}

		// Inform all other frontends about the output
		/*
		 * Inform all secondary frontends about the request result but add privBlock only
		 * if the sender is the same entity
		 */
		std::list<jvxOneFrontendAndState>::iterator elm = this->linkedSecFrontends.begin();
		for (; elm != linkedSecFrontends.end(); elm++)
		{
			if (origin == (*elm).fe)
			{
				TjvxEventLoopElement qel;
				qel.message_id = message_id;
				qel.param = &jlstresp;
				qel.paramType = JVX_EVENTLOOP_DATAFORMAT_JVXJSONMULTFIELDS;
				qel.eventClass = event_class;
				qel.eventPriority = event_priority;
				qel.origin_fe = origin;

				(*elm).fe->report_assign_output(&qel, res, privBlock);
				unmatchedFrontend = false;
			}
			else
			{
				TjvxEventLoopElement qel;
				qel.message_id = message_id;
				qel.param = &jlstresp;
				qel.paramType = JVX_EVENTLOOP_DATAFORMAT_JVXJSONMULTFIELDS;
				qel.eventClass = event_class;
				qel.eventPriority = event_priority;
				qel.origin_fe = origin;

				(*elm).fe->report_assign_output(&qel, res, NULL);
				//orign->report_assign_output(&qel, res, NULL);
			}

			if (reportSystemUpdate)
			{
				jvxSpecialEventType specEvent;
				specEvent.ev = JVX_EVENTLOOP_SPECIAL_EVENT_SYSTEM_UPDATE;
				specEvent.dat = NULL;
				TjvxEventLoopElement qel;
				qel.message_id = message_id;
				qel.param = &specEvent;
				qel.paramType = JVX_EVENTLOOP_DATAFORMAT_SPECIAL_EVENT;
				qel.eventClass = event_class;
				qel.eventPriority = event_priority;
				qel.origin_fe = origin;

				(*elm).fe->report_special_event(&qel, NULL);
			}
		}

		if (unmatchedFrontend && origin)
		{
			TjvxEventLoopElement qel;
			qel.message_id = message_id;
			qel.param = &jlstresp;
			qel.paramType = JVX_EVENTLOOP_DATAFORMAT_JVXJSONMULTFIELDS;
			qel.eventClass = event_class;
			qel.eventPriority = event_priority;
			qel.origin_fe = origin;

			origin->report_assign_output(&qel, res, privBlock);

			if (reportSystemUpdate)
			{
				jvxSpecialEventType specEvent;
				specEvent.ev = JVX_EVENTLOOP_SPECIAL_EVENT_SYSTEM_UPDATE;
				specEvent.dat = NULL;
				TjvxEventLoopElement qel;
				qel.message_id = message_id;
				qel.param = &specEvent;
				qel.paramType = JVX_EVENTLOOP_DATAFORMAT_SPECIAL_EVENT;
				qel.eventClass = event_class;
				qel.eventPriority = event_priority;
				qel.origin_fe = origin;

				origin->report_special_event(&qel, NULL);
			}
		}
	}
}

void
CjvxConsoleHost_be_drivehost::outputResult(std::string printthis, std::string startline, char newline, char tab)
{
	std::string oneLine;
	jvxSize cnt = 0;
	while (cnt < printthis.size())
	{
		if (printthis[cnt] == newline)
		{
			std::cout << startline << oneLine << std::flush;
			oneLine.clear();
		}
		else if (printthis[cnt] == tab)
		{
			oneLine += JVX_PRINT_DRIVEHOST_TAB;
		}
		else
		{
			oneLine += printthis.substr(cnt, 1);
		}
		cnt++;
	}

	if (!oneLine.empty())
	{
		std::cout << startline << oneLine << std::flush;
	}
	std::cout << std::endl;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::process_command_abstraction(
	const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, 
	const std::string& addArg,
	jvxSize off, CjvxJsonElementList& jsec, 
	jvxCBitField* extFlags, jvxBool* systemUpdate)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	switch (dh_command.family)
	{
	case JVX_DRIVEHOST_FAMILY_SHOW:
		switch (dh_command.address)
		{
		case JVX_DRIVEHOST_ADRESS_CONNECTION_DROPZONE:
			res = show_dropzone(dh_command, args, off, jsec);
			break;
		case JVX_DRIVEHOST_ADRESS_CONFIG:
			res = show_current_config(dh_command, args, off, jsec);
			break;
		default:
			return CjvxHostJsonCommandsShow::process_command_abstraction(dh_command, args, addArg, off, jsec, systemUpdate);			
		}
		break;
	case JVX_DRIVEHOST_FAMILY_ACT:
		switch (dh_command.address)
		{
		case JVX_DRIVEHOST_ADRESS_CONFIG:
			res = act_edit_config(dh_command, args, off, jsec);
			break;
		default:
			return CjvxHostJsonCommandsActShow::process_command_abstraction(dh_command, args, addArg, off, jsec, systemUpdate);
			break;
		}
		break;
	case JVX_DRIVEHOST_FAMILY_FILE:
		switch (dh_command.address)
		{
		case JVX_DRIVEHOST_ADRESS_CONFIG:
			res = file_config(dh_command, args, off, jsec, extFlags);
			break;
		default:
			return JVX_ERROR_UNSUPPORTED;
			break;
		}
		break;

	default:
		return JVX_ERROR_UNSUPPORTED;
	}
	
	return res;
}
