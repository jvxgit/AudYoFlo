#include "CjvxConsoleHost_be_drivehost.h"
#include "CjvxHostJsonDefines.h"

// =======================================================================================
// =======================================================================================
// =======================================================================================
// =======================================================================================

CjvxConsoleHost_be_drivehost::CjvxConsoleHost_be_drivehost(): 
	CjvxConsoleHost_be_print(),
	CjvxHostJsonCommandsActShow(involvedHost.hHost),
	CjvxProperties("jvxConsoleHost", *this)
{
	jvxSize i;
	
	/*
	tpAll[JVX_COMPONENT_UNKNOWN].reset(JVX_COMPONENT_UNKNOWN);
	for (i = 0; i < JVX_COMPONENT_LIMIT; i++)
	{
		tpAll[i].reset((jvxComponentType)i, 0, 0);
	}
	*/
	config.timeout_seq_msec = 500;	

	runtime.myState = JVX_STATE_INIT;
	linestart = JVX_DEFINE_PROCESSING_OFF;

	theCallback_saveconfig.cb = NULL;
	theCallback_saveconfig.cb_priv = NULL;

	theCallback_loadconfig.cb = NULL;
	theCallback_loadconfig.cb_priv = NULL;

	theCallback_exchg_property.cb = NULL;
	theCallback_exchg_property.cb_priv = NULL;

	confHostFeatures = static_cast<configureHost_features*>(&theHostFeatures);
	
	//this->reqHandle.initialize_fwd_link( this); <- moving into the local thread
}


CjvxConsoleHost_be_drivehost::~CjvxConsoleHost_be_drivehost()
{
}

// ============================================================================================
void
CjvxConsoleHost_be_drivehost::process_init(IjvxCommandLine* comLine)
{
	jvxSize i;
	jvxErrorType resL = JVX_NO_ERROR;
	assert(runtime.myState == JVX_STATE_INIT);

	std::string message;
	jvxApiString errMess;
	std::list<jvxOneFrontendAndState>::iterator elm_fe;
	std::list<jvxOneBackendAndState>::iterator elm_be;

	// Here is now the place to start the request handler
	this->reqHandle.initialize_fwd_link(this);

	message = jvx_textMessagePrioToString("Configuring console host ...", JVX_REPORT_PRIORITY_INFO);
	postMessage_inThread(message);

	// ======================================================================
	// Prepare command line args
	// ======================================================================
	// Command line args in frontends
	if (linkedPriFrontend.fe)
	{
		linkedPriFrontend.fe->report_register_fe_commandline(comLine);
	}

	elm_fe = linkedSecFrontends.begin();
	for (; elm_fe != linkedSecFrontends.end(); elm_fe++)
	{
		elm_fe->fe->report_register_fe_commandline(comLine);
	}

	// Command line args in backends
	report_register_be_commandline(comLine);
	elm_be = linkedSecBackends.begin();
	for (; elm_be != linkedSecBackends.end(); elm_be++)
	{
		elm_be->be->report_register_be_commandline(comLine);
	}

	// ======================================================================

	resL = this->boot_configure(&errMess, comLine, static_cast<IjvxReport*>(this), static_cast<IjvxReportOnConfig*>(this), 
		static_cast<IjvxReportStateSwitch*>(this), static_cast<IjvxReport*>(this));
	if (resL != JVX_NO_ERROR)
	{
		std::string txtMsg;
		if (resL != JVX_ERROR_ABORT)
		{
			txtMsg = "Fatal Error";
		}
		this->fatalStop(txtMsg, errMess.c_str());
	}

	// ======================================================================
	// Read out command line args
	// ======================================================================
	// Command line args in frontends
	if (linkedPriFrontend.fe)
	{
		linkedPriFrontend.fe->report_readout_fe_commandline(comLine);
	}

	elm_fe = linkedSecFrontends.begin();
	for (; elm_fe != linkedSecFrontends.end(); elm_fe++)
	{
		elm_fe->fe->report_readout_fe_commandline(comLine);
	}

	// Command line args in backends
	report_readout_be_commandline(comLine);
	elm_be = linkedSecBackends.begin();
	for (; elm_be != linkedSecBackends.end(); elm_be++)
	{
		elm_be->be->report_readout_be_commandline(comLine);
	}

	// ======================================================================

	message = jvx_textMessagePrioToString("Initializing console host ...", JVX_REPORT_PRIORITY_INFO);
	postMessage_inThread(message);

	resL = boot_initialize(&errMess, NULL);
	if (resL != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

	message = jvx_textMessagePrioToString("Activating audio host ...", JVX_REPORT_PRIORITY_INFO);
	postMessage_inThread(message);

	resL = boot_activate(&errMess, NULL);
	if (resL != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

	evLop->events_process();

	message = jvx_textMessagePrioToString("Preparing audio host ...", JVX_REPORT_PRIORITY_INFO);
	postMessage_inThread(message);

	resL = boot_prepare(&errMess, NULL);
	if (resL != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

	evLop->events_process();
#if 0

	CjvxCommandline::reset_command_line();
	jvxErrorType res = CjvxCommandline::parse_command_line(clStr, clNum);
	
	// initializeHost
	initializeHost();

	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle**>(&config.hostRefConfigExtender));
	if ((res == JVX_NO_ERROR) && config.hostRefConfigExtender)
	{
		res = config.hostRefConfigExtender->register_extension(static_cast<IjvxConfigurationExtender_report*>(this), JVX_CONSOLE_BE_SECTION);
	}

	// Inform all frontends about that the main backend has been started
	assert(linkedPriFrontend.fe);
	if (linkedPriFrontend.st == JVX_STATE_SELECTED)
	{
		resL = linkedPriFrontend.fe->report_process_event(0, involvedComponents.theHost.hFHost, JVX_EVENTLOOP_DATAFORMAT_IJVXHOST_REF,
			JVX_EVENTLOOP_EVENT_INIT, JVX_EVENTLOOP_REQUEST_CALL_BLOCKING, JVX_EVENTLOOP_PRIORITY_NORMAL, false);
		if (resL == JVX_NO_ERROR)
		{
			linkedPriFrontend.st = JVX_STATE_ACTIVE;
		}
		else
		{
			assert(0);
		}
	}
	
	std::list<jvxOneFrontendAndState>::iterator elm = linkedSecFrontends.begin();
	for (; elm != linkedSecFrontends.end(); elm++)
	{
		if (elm->st == JVX_STATE_SELECTED)
		{
			resL = elm->fe->report_process_event(0, involvedComponents.theHost.hFHost, JVX_EVENTLOOP_DATAFORMAT_IJVXHOST_REF,
				JVX_EVENTLOOP_EVENT_INIT, JVX_EVENTLOOP_REQUEST_CALL_BLOCKING, JVX_EVENTLOOP_PRIORITY_NORMAL, false);
			if (resL == JVX_NO_ERROR)
			{
				elm->st = JVX_STATE_ACTIVE;
			}
			else
			{
				assert(0);
			}
		}
	}

	// At this place, the config file is read!
	initConfigureHost();
#endif	
}


jvxErrorType
CjvxConsoleHost_be_drivehost::process_shutdown()
{	
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errMess;
	std::string strResponse = "";

	if (runtime.myState == JVX_STATE_PROCESSING)
	{
		// We trigger the end of sequencer here!!
		CjvxJsonElementList elm;
		jvxErrorType res = this->act_stop_sequencer(elm, true);
		
		strResponse = "Sequencer is still running, triggering to shut it down.";
		strResponse += "\n";
		outputResult(strResponse, "\n\t\t", '\n', '\t');

		// Insert shutdown to message queue and leave
		return JVX_ERROR_POSTPONE;
	}

	res = shutdown_postprocess(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}

	res = shutdown_deactivate(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}

	res = shutdown_terminate(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}

	// Undo the 
	this->reqHandle.initialize_fwd_link(nullptr);
#if 0	
	// Inform that the main backend has been shut down - at that time, the host reference becomes invalid
	// The frontends and backends may not be able to shutdown. In that case, the
	// functions return false and await next attempt shortly after
	std::list<jvxOneFrontendAndState>::iterator elm = linkedSecFrontends.begin();
	for (; elm != linkedSecFrontends.end(); elm++)
	{
		if (elm->st == JVX_STATE_ACTIVE)
		{
			res = elm->fe->report_process_event(0, NULL, JVX_EVENTLOOP_DATAFORMAT_NONE,
				JVX_EVENTLOOP_EVENT_SHUTDOWN, JVX_EVENTLOOP_REQUEST_CALL_BLOCKING, JVX_EVENTLOOP_PRIORITY_NORMAL, false);
			if (res != JVX_NO_ERROR)
			{
				return res;
			}
			elm->st = JVX_STATE_SELECTED;
		}
	}

	assert(linkedPriFrontend.fe);
	if (linkedPriFrontend.st == JVX_STATE_ACTIVE)
	{
		linkedPriFrontend.fe->report_process_event(0, NULL, JVX_EVENTLOOP_DATAFORMAT_NONE,
			JVX_EVENTLOOP_EVENT_SHUTDOWN, JVX_EVENTLOOP_REQUEST_CALL_BLOCKING, JVX_EVENTLOOP_PRIORITY_NORMAL, false);
		if (res != JVX_NO_ERROR)
		{
			return res;
		}
		linkedPriFrontend.st = JVX_STATE_SELECTED;
	}
	
	// We need to make sure there is no lingering message and no more messages are no more
	// messages entering the queue. It must be completed before call of terminateHost since
	// the components may request some blocking calls on deactivate!
	//evLop->force_clear_end_event_loop();
	evLop->backend_block(this);
	evLop->event_clear(JVX_SIZE_UNSELECTED, NULL, static_cast<IjvxEventLoop_backend*>(this));
	
	strResponse = "Shutting down backend..";
	strResponse += "\n";
	outputResult(strResponse, "\n\t\t", '\n', '\t');

	terminateHost();
#endif

	return JVX_NO_ERROR;
}

// ========================================================================================

void
CjvxConsoleHost_be_drivehost::postMessage_inThread(std::string message)
{
	std::cout << message << std::endl;
}

void
CjvxConsoleHost_be_drivehost::postMessage_outThread(std::string message)
{
	// Put this string into the event loop queue
	TjvxEventLoopElement evelm;

	evelm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
	evelm.priv_fe = NULL;
	evelm.target_be = static_cast<IjvxEventLoop_backend*>(this);
	evelm.priv_be = NULL;

	evelm.param = (jvxHandle*)(&message);
	evelm.paramType = JVX_EVENTLOOP_DATAFORMAT_STDSTRING;

	evelm.eventType = JVX_EVENTLOOP_EVENT_TEXT_SHOW;
	evelm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
	evelm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
	evelm.delta_t = JVX_SIZE_UNSELECTED;
	evelm.autoDeleteOnProcess = c_true;
	jvxErrorType resL = evLop->event_schedule(&evelm, NULL, NULL);
}

void 
CjvxConsoleHost_be_drivehost::fatalStop(std::string message1, std::string message2)
{
	std::cout << message1 << "::" << message2 << std::endl;
	JVX_SLEEP_S(5);
	exit(0);
}

void
CjvxConsoleHost_be_drivehost::addDefaultSequence(bool	onlyIfNoSequence, jvxOneSequencerStepDefinition* defaultSteps_run, jvxSize numSteps_run,
	jvxOneSequencerStepDefinition* defaultSteps_leave, jvxSize numSteps_leave, const std::string& nm_def)
{
	IjvxSequencer* sequencer = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numSeq = 0;
	jvxSize numStepsRun = 0;
	jvxSize numStepsLeave = 0;
	jvxSize i, k;

	jvxSize idSeq = 0;
	jvxSequencerQueueType seqType = JVX_SEQUENCER_QUEUE_TYPE_RUN;
	jvxSequencerElementType tpElm = JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE;
	jvxComponentIdentification tpComp(JVX_COMPONENT_AUDIO_NODE, 0, 0);
	std::string descr = "description";
	jvxSize funcId = JVX_SIZE_UNSELECTED;
	jvxInt64 timeout = 10000;
	std::string label_step;
	std::string label_cond1;
	std::string label_cond2;
	jvxSize idSeqActivate = JVX_SIZE_UNSELECTED;
	jvxBool foundDefaultWithinDefinedSequences = false;

	// If so, go ahead
	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if ((res == JVX_NO_ERROR) && sequencer)
	{
		res = jvx_add_default_sequence_sequencer(sequencer,
			onlyIfNoSequence, defaultSteps_run, numSteps_run,
			defaultSteps_leave, numSteps_leave, nm_def);
		
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
	}
}
  
jvxErrorType 
CjvxConsoleHost_be_drivehost::report_register_fe_commandline(IjvxCommandLine* comLine)
{
	if (comLine)
	{
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::report_readout_fe_commandline(IjvxCommandLine* comLine)
{
	jvxSize num = 0;
	if (comLine)
	{
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::report_register_be_commandline(IjvxCommandLine* comLine)
{	
	jvxErrorType res = CjvxConsoleHost_be_print::report_register_be_commandline(comLine);
	if (res == JVX_NO_ERROR)
	{
		comLine->register_option("--startExe", "", "Pass execuitable to be started at startup to connect UI.", "", true, JVX_DATAFORMAT_STRING);
		comLine->register_option("--startArgs", "", "Pass exectutable start args.","", true, JVX_DATAFORMAT_STRING, JVX_SIZE_UNSELECTED);
		comLine->register_option("--autostart", "", "Start sequencer immediately.");
	}
	return res;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::report_readout_be_commandline(IjvxCommandLine* comLine)
{
	
	jvxErrorType res = CjvxConsoleHost_be_print::report_readout_be_commandline(comLine);
	if (res == JVX_NO_ERROR)
	{		
		jvxApiString astr;
		jvxSize num = 0;
		comLine->number_entries_option("--startExe", &num);
		if (num)
		{
			comLine->content_entry_option("--startExe", 0, &astr, JVX_DATAFORMAT_STRING);
			startAppExe = astr.std_str();
		}

		num = 0;
		astr.clear();
		comLine->number_entries_option("--startArgs", &num);
		for (jvxSize i = 0; i< num; i++)
		{
			comLine->content_entry_option("--startArgs", i, &astr, JVX_DATAFORMAT_STRING);
			startAppArgs.push_back(astr.std_str());
		}

		comLine->number_entries_option("--autostart", &num);
		if (num)
		{
			config.cmdline_autostart = true;
		}
	}
	return res;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::pre_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp)
{
	CjvxReportCommandRequest_ss elm(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH, tp, ss,
		jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION);
	elm.set_immediate(true);
	switch (ss)
	{
	case JVX_STATE_SWITCH_UNSELECT:
	case JVX_STATE_SWITCH_DEACTIVATE:
	case JVX_STATE_SWITCH_POSTPROCESS:

		this->request_command(elm);
		break;

	default:
		break;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConsoleHost_be_drivehost::post_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp, jvxErrorType suc)
{
	CjvxReportCommandRequest_ss elm(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH, tp, ss,
		jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION);
	elm.set_immediate(true);
	if (suc == JVX_NO_ERROR)
	{
		switch (ss)
		{
		case JVX_STATE_SWITCH_SELECT:
		case JVX_STATE_SWITCH_ACTIVATE:
		case JVX_STATE_SWITCH_PREPARE:

			this->request_command(elm);
			break;

		default:
			break;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxConsoleHost_be_drivehost::one_step_sequencer(IjvxSequencer* sequencer, jvxSize timeout_msec)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxTick tstampNow = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.myTimerRef);
	res = sequencer->trigger_step_process_extern(tstampNow, NULL);
	if (res == JVX_NO_ERROR)
	{
		// Re-trigger timer event in timeout 
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
		evelm.delta_t = timeout_msec;
		evelm.autoDeleteOnProcess = c_false;
		evelm.message_id = JVX_SIZE_UNSELECTED;

		res = evLop->event_schedule(&evelm, NULL, NULL);
		assert(res == JVX_NO_ERROR);

		runtime.sequencer.currentTimerId = evelm.message_id;
	}
	else
	{
		handle_step_process_non_usual_return(sequencer, res);
	}
	return res;
}

void
CjvxConsoleHost_be_drivehost::handle_step_process_non_usual_return(IjvxSequencer* sequencer, jvxErrorType res)
{
	if ((res == JVX_ERROR_PROCESS_COMPLETE) || (res == JVX_ERROR_ABORT))
	{
		//runtime.myState = JVX_STATE_COMPLETE;
		//runtime.acceptNewTimerEvents = false;
		jvxTick tstampNow = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.myTimerRef);
		res = sequencer->trigger_complete_process_extern(tstampNow);
		assert(res == JVX_NO_ERROR);

		res = sequencer->acknowledge_completion_process();
		assert(res == JVX_NO_ERROR);

		runtime.myState = JVX_STATE_ACTIVE;
		std::string txt = jvx_textMessagePrioToString("Sequencer Run Completed!", JVX_REPORT_PRIORITY_SUCCESS);
		postMessage_outThread(txt);
	}
	else
	{
		std::cout << "UNEXPECTED:" << jvxErrorType_descr(res) << std::endl;
		assert(0);
	}
}

jvxErrorType
CjvxConsoleHost_be_drivehost::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	std::vector<std::string> warns;
	genConsoleHost::put_configuration_all(callMan, processor, sectionToContainAllSubsectionsForMe, &warns);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	genConsoleHost::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxConsoleHost_be_drivehost::backend_status(jvxCBitField* curr_status)
{
	jvxErrorType res = CjvxConsoleHost_be_print::backend_status(curr_status);
	if (res == JVX_NO_ERROR)
	{
		if (curr_status)
		{
			if (runtime.myState == JVX_STATE_PROCESSING)
			{
				jvx_bitSet(*curr_status, jvxBackendStatusShiftOptions::JVX_BACKEND_STATUS_SCHEDULER_RUNNING_SHIFT);
			}
		}
	}
	return res;
}

