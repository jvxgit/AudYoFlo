#include "CjvxConsoleHost_be_drivehost.h"
#include "CjvxHostJsonDefines.h"

extern "C"
{
#include "jvx-crc.h"
}

jvxErrorType
CjvxConsoleHost_be_drivehost::act_edit_config(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string actionString;
	std::string errTxt;
	jvxSize szSlots = 0;
	jvxSize szSlotsub = 0;
	CjvxJsonElement jelm_result;
	// Arg1: add sequence, remove sequence, activate sequence, add step, remove step, modify step
	if (args.size() > (off + 1))
	{
		CjvxJsonElementList jsecLoc;
		res = CjvxHostJsonCommandsActShow::act_edit_config(dh_command, args, off, jsec);
		if (res == JVX_NO_ERROR)
		{
			// Success, complete 
			return res;
		}

		// Delete the json struct as it contains an error only
		jsec.deleteAll();
		res = JVX_NO_ERROR;
		errTxt.clear();

		// From here local processing
		actionString = args[off];

		if (actionString == "timeout_seq")
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

jvxErrorType
CjvxConsoleHost_be_drivehost::act_start_sequencer(CjvxJsonElementList& jelmret)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxSize num = 0;
	jvxSize mID = 0;
	std::string errTxt;
	if (runtime.myState == JVX_STATE_ACTIVE)
	{
		runtime.myState = JVX_STATE_PROCESSING;
		//runtime.acceptNewTimerEvents = true;
		runtime.timeout_msec = config.timeout_seq_msec;

		res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
		if ((res == JVX_NO_ERROR) && sequencer)
		{
			JVX_GET_TICKCOUNT_US_SETREF(&runtime.myTimerRef);
			runtime.firstError = "";
			res = sequencer->run_process(static_cast<IjvxSequencer_report*>(this),
				(jvxCBitField)-1, theHostFeatures.granularity_state_report);
			if (res == JVX_NO_ERROR)
			{
				TjvxEventLoopElement evelm;

				evelm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
				evelm.priv_fe = reinterpret_cast<jvxHandle*>(this);
				evelm.target_be = static_cast<IjvxEventLoop_backend*>(this);
				evelm.priv_be = reinterpret_cast<jvxHandle*>(this);

				evelm.param = NULL;
				evelm.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;

				evelm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC;
				evelm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER_RESPONSE;
				evelm.eventPriority = JVX_EVENTLOOP_PRIORITY_TIMER;
				evelm.delta_t = 0;
				evelm.autoDeleteOnProcess = c_false;
				evelm.message_id = JVX_SIZE_UNSELECTED;

				res = evLop->event_schedule(&evelm, NULL, NULL);
				runtime.sequencer.currentTimerId = evelm.message_id;

				if (res == JVX_NO_ERROR)
				{
					linestart = JVX_DEFINE_PROCESSING_ON;
				}
				else
				{
					errTxt = "Passing timer message to event loop failed.";
					res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
					runtime.myState = JVX_STATE_ACTIVE;
					//runtime.acceptNewTimerEvents = false;
				}
			}
			else
			{
				errTxt = "Sequencer failed to start.";
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
				JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);

				runtime.myState = JVX_STATE_ACTIVE;
				//runtime.acceptNewTimerEvents = false;
			}

			involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
		}
		else
		{
			errTxt = "Failed to obtain sequencer handle.";
			res = JVX_ERROR_INTERNAL;
			JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);

			runtime.myState = JVX_STATE_ACTIVE;
		}
	}
	else
	{
		errTxt = "Wrong state to start a sequencer.";
		res = JVX_ERROR_INTERNAL;
		JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
	}
	return res;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::act_stop_sequencer(CjvxJsonElementList& jelmret, jvxBool full_stop)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxSize num = 0;
	jvxSize mID = 0;
	std::string errTxt;
	if (runtime.myState == JVX_STATE_PROCESSING)
	{
		// The rest will be handled in callbacks
		res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
		assert(res == JVX_NO_ERROR);
		assert(sequencer);

		// ++++++++++++++++++++++++++++++++++++++++++++++++
		// Remove timer from event queue here!!!
		// ++++++++++++++++++++++++++++++++++++++++++++++++
		res = this->evLop->event_clear(runtime.sequencer.currentTimerId, NULL, NULL);
		assert(res == JVX_NO_ERROR);

		if (full_stop)
		{
			res = sequencer->trigger_abort_process();
		}
		else
		{
			res = sequencer->trigger_abort_sequence();
		}
		assert(res == JVX_NO_ERROR);

		// Run timer immediately (timeout 0)
		one_step_sequencer(sequencer, 0);
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));

		return res;
	}
	else
	{
		errTxt = "Wrong state to stop the sequencer.";
		res = JVX_ERROR_INTERNAL;
		JVX_CREATE_ERROR_NO_RETURN(jelmret, res, errTxt);
		runtime.myState = JVX_STATE_ACTIVE;
	}
	return res;
}