#include "CjvxConsoleHost_be_drivehost.h"
#include "CjvxHostJsonDefines.h"

// =============================================================================================
// =============================================================================================
// =============================================================================================
// =============================================================================================
// =============================================================================================
// =============================================================================================

typedef struct
{
	jvxCBitField request;
	jvxHandle* caseSpecificData;
	jvxSize szCaseSpecificData;
} jvxRequestCommand_fwd;


// =============================================================================================
// =============================================================================================

jvxErrorType 
CjvxConsoleHost_be_drivehost::report_simple_text_message(const char* txtptr, jvxReportPriority prio)
{
	assert(this->evLop);

	std::string txt = jvx_textMessagePrioToString(txtptr, prio);
	postMessage_outThread(txt);
	return JVX_ERROR_UNSUPPORTED; 
}

jvxErrorType
CjvxConsoleHost_be_drivehost::report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
	jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize offs_start, jvxSize numE, 
	const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose)
{
	jvxErrorType res = JVX_NO_ERROR;
	report_property_changed* summarizeit = NULL;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(summarizeit, report_property_changed);

	summarizeit->cat = cat;
	summarizeit->onlyContent = onlyContent;
	summarizeit->origin = thisismytype;
	summarizeit->propId = propId;
	summarizeit->offs_start = offs_start;
	summarizeit->numE = numE;


	TjvxEventLoopElement evelm;

	evelm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
	evelm.priv_fe = NULL;
	evelm.target_be = static_cast<IjvxEventLoop_backend*>(this);
	evelm.priv_be = NULL;

	evelm.param = (jvxHandle*)(summarizeit);
	evelm.paramType = JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC;

	evelm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 1;
	evelm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
	evelm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
	evelm.delta_t = JVX_SIZE_UNSELECTED;
	evelm.autoDeleteOnProcess = c_false;
	res = evLop->event_schedule(&evelm, NULL, NULL);

	return res;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
	jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
	jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose )
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::request_command(const CjvxReportCommandRequest& request)
{
	TjvxEventLoopElement evElm;
	jvxByte* memfld = NULL;
	jvxBool deallocatethis = false;
	jvxCBitField p1 = 0;
	jvxErrorType res = JVX_NO_ERROR;

	JVX_THREAD_ID tIdLocal = JVX_GET_CURRENT_THREAD_ID();
	
	if (request.request() == jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN)
	{
		assert(tIdLocal == threadIdMainLoop);
		const CjvxReportCommandRequest_uid* ptrReq = castCommandRequest<CjvxReportCommandRequest_uid>(request);
		if (ptrReq)
		{
			IjvxDataConnections* theDataConnections = NULL;
			if (involvedComponents.theHost.hFHost)
			{
				theDataConnections = reqInterfaceObj<IjvxDataConnections>(involvedComponents.theHost.hFHost);
				if (theDataConnections)
				{
					jvxSize num = 0;
					jvxSize uId = JVX_SIZE_UNSELECTED;
					ptrReq->uid(&uId);

					res = theDataConnections->add_process_test(uId, &num, request.immediate());
					if (res == JVX_ERROR_POSTPONE)
					{
						// In case we receive a POSTPONE, this indicates that we need to change thread
						// Post this event to be rescheduled
						evElm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
						evElm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
						evElm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 3; // This is the reschedule id - to be checked!
						evElm.origin_fe = this;
						evElm.target_be = this;

						// The data element will be copied in scheduler
						evElm.autoDeleteOnProcess = true;
						evElm.param = (jvxHandle*)&request;
						evElm.paramType = JVX_EVENTLOOP_DATAFORMAT_REQUEST_COMMAND_REQUEST;
						res = this->evLop->event_schedule(&evElm, NULL, NULL);
					}
					else
					{
						if (num)
						{
							jvxCBitField requestStart = 0;
							jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
							jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);
							jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);

							this->report_command_request(requestStart, NULL, 0);
						}
					}
					retInterfaceObj<IjvxDataConnections>(involvedComponents.theHost.hFHost, theDataConnections);
				}
			}
		}
		return res;
	}


	if (request.immediate())
	{
		assert(tIdLocal == threadIdMainLoop);
		switch (request.request())
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE:
			
			switch (request.request())
			{
			case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE:
				
				// We need to trigger the peridodic sequencer step immediately!!
				/*
				if (subWidgets.theSequencerWidget)
				{
					res = subWidgets.theSequencerWidget->immediate_sequencer_step();
				}
				*/
				break;
			}

			if (this->theHostFeatures.automation.if_report_automate)
			{
				jvxErrorType resL = JVX_NO_ERROR;
				jvxReportCommandBroadcastType broad = request.broadcast();
				switch (broad)
				{
				case jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION:
					resL = this->theHostFeatures.automation.if_report_automate->request_command(request);
					break;
				}
			}
			return res;
		}

		if (this->theHostFeatures.automation.if_report_automate)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			jvxReportCommandBroadcastType broad = request.broadcast();
			switch (broad)
			{
			case jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION:
				resL = this->theHostFeatures.automation.if_report_automate->request_command(request);
				break;
			}
		}
	}
	else
	{
		// Post this event to be rescheduled
		evElm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		evElm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
		evElm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 3; // This is the reschedule id - to be checked!
		evElm.origin_fe = this;
		evElm.target_be = this;

		// The data element will be copied in scheduler
		evElm.autoDeleteOnProcess = true;
		evElm.param = (jvxHandle*)&request;
		evElm.paramType = JVX_EVENTLOOP_DATAFORMAT_REQUEST_COMMAND_REQUEST;
		res = this->evLop->event_schedule(&evElm, NULL, NULL);

	}
	return res;
}

jvxErrorType CjvxConsoleHost_be_drivehost::interface_sub_report(IjvxSubReport** subReport)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::report_command_request(
	jvxCBitField request, jvxHandle* caseSpecificData ,
	jvxSize szSpecificData)
{
	TjvxEventLoopElement evElm;
	jvxByte* memfld = NULL;
	jvxBool deallocatethis = false;
	jvxCBitField p1 = 0;
	jvxRequestCommand_fwd p2;

	evElm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
	evElm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
	evElm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 2;

	evElm.origin_fe = this;
	evElm.target_be = this;

	p2.request = request;
	p2.caseSpecificData = caseSpecificData;
	p2.szCaseSpecificData = szSpecificData;

	evElm.sz_param = sizeof(jvxRequestCommand_fwd);
	evElm.sz_param += szSpecificData;

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(memfld, jvxByte, evElm.sz_param);
	memcpy(memfld, &p2, sizeof(jvxRequestCommand_fwd));

	if (szSpecificData && caseSpecificData)
	{
		memcpy(memfld + sizeof(jvxRequestCommand_fwd), caseSpecificData, szSpecificData);
	}

/*
	if (caseSpecificData == NULL)
	{
		evElm.sz_param = sizeof(p1);
		p1 = request;
		memfld = (jvxByte*)&p1;
	}
	else
	{
		if (szSpecificData == 0)
		{
			evElm.sz_param = sizeof(p2);
			p2.request = request;
			p2.caseSpecificData = caseSpecificData; // the VALUE of the pointer is the important issue!!
			memfld = (jvxByte*)&p2;
		}
		else
		{
			evElm.sz_param = sizeof(jvxCBitField) + szSpecificData;

			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(memfld, jvxByte, evElm.sz_param);
			jvxCBitField* ptrReq = (jvxCBitField*)memfld;
			*ptrReq = request;
			if (caseSpecificData)
			{
				memcpy(memfld + sizeof(jvxCBitField), caseSpecificData, szSpecificData);
			}
			deallocatethis = true;
		}
	}
*/
	evElm.autoDeleteOnProcess = true;
	evElm.param = memfld;
	evElm.paramType = JVX_EVENTLOOP_DATAFORMAT_MEMORY_BUF;

	this->evLop->event_schedule(&evElm, NULL, NULL);
/*
	if (deallocatethis)
	{
		JVX_DSP_SAFE_DELETE_FIELD(memfld);
	}
*/
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::report_os_specific(jvxSize commandId, void* context)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::report_want_to_shutdown_ext(jvxBool restart)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::request_property(jvxFrontendSupportRequestType tp, jvxHandle* load)
{
	return JVX_ERROR_UNSUPPORTED;

}
// =============================================================================================
// =============================================================================================

jvxErrorType
CjvxConsoleHost_be_drivehost::report_component_select_on_config(const jvxComponentIdentification& tp)
{
	/*
	jvx_componentIdPropsCombo newRefElm;
	jvx_componentIdentification_properties_direct_add(
		involvedHost.hHost,
		theRegisteredComponentReferenceTriples,
		tp,
		newRefElm);*/
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::report_component_unselect_on_config(const jvxComponentIdentification& tp)
{
	/*
	jvx_componentIdPropsCombo newRefElm;
	jvx_componentIdentification_properties_erase(
		involvedHost.hHost,
		theRegisteredComponentReferenceTriples,
		tp);
		*/
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::report_component_active_on_config(const jvxComponentIdentification& tp)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::report_component_inactive_on_config(const jvxComponentIdentification& tp)
{
	return JVX_ERROR_UNSUPPORTED;
}

// =============================================================================================
// =============================================================================================

jvxErrorType
CjvxConsoleHost_be_drivehost::report_process_event(TjvxEventLoopElement* theQueueElement)
{
	jvxSize message_id = theQueueElement->message_id;
	jvxHandle* param = theQueueElement->param;
	jvxSize paramType = theQueueElement->paramType;
	jvxSize event_type = theQueueElement->eventType;
	jvxOneEventClass event_class = theQueueElement->eventClass;
	jvxOneEventPriority event_priority = theQueueElement->eventPriority;
	jvxCBool autoDeleteOnProcess = theQueueElement->autoDeleteOnProcess;
	jvxHandle* privBlock = NULL;
	if (theQueueElement->oneHdlBlock)
	{
		privBlock = theQueueElement->oneHdlBlock->priv;
	}
	if (paramType == JVX_EVENTLOOP_DATAFORMAT_STDSTRING)
	{
		std::string* newStr = (std::string*)param;
		JVX_DSP_SAFE_DELETE_OBJECT(newStr);
	}
	else if (event_type == JVX_EVENTLOOP_EVENT_SPECIFIC + 1)
	{
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC);
		report_property_changed* summarizeit = (report_property_changed*)param;
		JVX_DSP_SAFE_DELETE_OBJECT(summarizeit);
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::report_cancel_event(TjvxEventLoopElement* theQueueElement)
{
	if ( theQueueElement->paramType == JVX_EVENTLOOP_DATAFORMAT_STDSTRING)
	{
		std::string* newStr = (std::string*)theQueueElement->param;
		JVX_DSP_SAFE_DELETE_OBJECT(newStr);
	}
	else if (theQueueElement->eventType == JVX_EVENTLOOP_EVENT_SPECIFIC + 1)
	{
		assert(theQueueElement->paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC);
		report_property_changed* summarizeit = (report_property_changed*)theQueueElement->param;
		JVX_DSP_SAFE_DELETE_OBJECT(summarizeit);
	}
	return JVX_ERROR_UNSUPPORTED;
}

/* 
 * ID JVX_EVENTLOOP_EVENT_SPECIFIC -> sequencer timer event
 * JVX_EVENTLOOP_EVENT_SPECIFIC + 1 -> Property changed event
 */
jvxErrorType 
CjvxConsoleHost_be_drivehost::process_event(TjvxEventLoopElement* theQueueElement)
{
	jvxSize message_id = theQueueElement->message_id;
	IjvxEventLoop_frontend* origin = theQueueElement->origin_fe;
	jvxHandle* priv_fe = theQueueElement->priv_fe;
	jvxHandle* param = theQueueElement->param;
	jvxSize sz_param = theQueueElement->sz_param;
	jvxSize paramType = theQueueElement->paramType;
	jvxSize event_type = theQueueElement->eventType;
	jvxOneEventClass event_class = theQueueElement->eventClass;
	jvxOneEventPriority event_priority = theQueueElement->eventPriority;
	jvxCBool autoDeleteOnProcess = theQueueElement->autoDeleteOnProcess;
	jvxBool* reschedule = &theQueueElement->rescheduleEvent;


	jvxHandle* privBlock = NULL;
	if (theQueueElement->oneHdlBlock)
	{
		privBlock = theQueueElement->oneHdlBlock->priv;
	}

	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxSize mID = 0;
	if (event_type == (JVX_EVENTLOOP_EVENT_SPECIFIC))
	{
		assert(event_priority == JVX_EVENTLOOP_PRIORITY_TIMER);
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_NONE);

		// No need to remove timer since we are in timer callback
			// Do some processing...
		res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
		assert(res == JVX_NO_ERROR);
		assert(sequencer);
		res = one_step_sequencer(sequencer, runtime.timeout_msec);
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
		return res;
	}
	else if (event_type == JVX_EVENTLOOP_EVENT_SPECIFIC + 1)
	{
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC);
		report_property_changed* summarizeit = (report_property_changed*)param;

		std::map<jvxInterfaceType, oneRegisteredInterface>::iterator elm = registeredInterfaceTypes.find(JVX_INTERFACE_REPORT);
		if (elm != registeredInterfaceTypes.end())
		{
			std::list<jvxHandle*>::iterator elmp = elm->second.theIfs.begin();
			for (; elmp != elm->second.theIfs.end(); elmp++)
			{
				IjvxReport* theRep = (IjvxReport*)(*elmp);
				theRep->report_internals_have_changed(summarizeit->origin, NULL, summarizeit->cat, summarizeit->propId, 
					summarizeit->onlyContent, summarizeit->offs_start, summarizeit->numE, JVX_COMPONENT_UNKNOWN);
			}
		}
		return JVX_NO_ERROR;
	}
	else if (event_type == JVX_EVENTLOOP_EVENT_SPECIFIC + 2)
	{
		jvxRequestCommand_fwd* param_fwd = (jvxRequestCommand_fwd*)param;
		jvxCBitField request = 0;
		jvxSize szFld = 0;
		jvxHandle* ptr = NULL;
		
		assert(sz_param >= sizeof(jvxRequestCommand_fwd));
		request = param_fwd->request;
		if (param_fwd->szCaseSpecificData)
		{
			param_fwd->caseSpecificData = (jvxByte*)param_fwd + sizeof(jvxRequestCommand_fwd);
		}
/*
		if (jvx_bitTest(request, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT))
		{
			assert(szFld == sizeof(jvxHandle*));
			ptr = *((jvxHandle**)((jvxByte*)param + sizeof(jvxCBitField)));
			szFld = 0;
		}
		else
		{
			ptr = (jvxByte*)param + sizeof(jvxCBitField);
		}
*/
		report_command_request_inThread(request, param_fwd->caseSpecificData, param_fwd->szCaseSpecificData);		
		return JVX_NO_ERROR;
	}
	else if (event_type == JVX_EVENTLOOP_EVENT_SPECIFIC + 3)
	{
		const CjvxReportCommandRequest* request = (const CjvxReportCommandRequest*)param;

		jvxReportCommandDataType tp = request->datatype();
		const CjvxReportCommandRequest_rm* iface_rm = NULL;
		jvxReportCommandRequest req = request->request();
		jvxComponentIdentification cp;
		jvxCBitField prio = 0;
		IjvxObject* theObj = NULL;
		IjvxInterfaceFactory* theFac = NULL;
		jvxComponentIdentification cpId = request->origin();
		jvxSize schedId = JVX_SIZE_UNSELECTED;
		jvxErrorType resL = JVX_NO_ERROR;
		switch (tp)
		{
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_BASE:
			switch (req)
			{
			case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST:
				cp = request->origin();
				
				std::cout << __FUNCTION__ << "Reported new component list for component <" << jvxComponentIdentification_txt(cp) << "> - need to forward this request via web socket later" << std::endl;
				break;

			case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_ALL_PROPERTIES:
				cp = request->origin();
				std::cout << __FUNCTION__ << "Reported update all components for component <" << jvxComponentIdentification_txt(cp) << "> - need to forward this request via web socket later" << std::endl;
				break;
			case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_SYSTEM_STATUS_CHANGED:

				std::cout << __FUNCTION__ << "Reported system status changed - need to forward this request via web socket later" << std::endl;
				break;
			}
			break;

		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE:
			request->specialization(reinterpret_cast<const jvxHandle**>(&iface_rm), tp);
			if (iface_rm)
			{
				jvxHandle* userData = request->user_data();
				schedId = iface_rm->schedule_id();
				if (involvedHost.hHost)
				{
					involvedHost.hHost->request_object_selected_component(cpId, &theObj);
					if (theObj)
					{
						theObj->interface_factory(&theFac);
						if (theFac)
						{
							IjvxSchedule* theSchedule = NULL;
							theFac->request_hidden_interface(JVX_INTERFACE_SCHEDULE, reinterpret_cast<jvxHandle**>(&theSchedule));
							if (theSchedule)
							{
								theSchedule->schedule_main_loop(schedId, userData);
								theFac->return_hidden_interface(JVX_INTERFACE_SCHEDULE, reinterpret_cast<jvxHandle*>(theSchedule));
							}
							else
							{
								jvxApiString astr;
								theObj->description(&astr);
								std::cout << __FUNCTION__ << " : Request to re-schedule in main thread failed for object <" << astr.std_str() << ">, reason: JVX_INTERFACE_SCHEDULE not supported." << std::endl;
							}
						}
						involvedHost.hHost->return_object_selected_component(cpId, theObj);
					}
				}
			}
			
			break;
		default:
			break;
		}

		if (this->theHostFeatures.automation.if_report_automate)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			jvxReportCommandBroadcastType broad = request->broadcast();
			switch (broad)
			{
			case jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION:
				resL = this->theHostFeatures.automation.if_report_automate->request_command(*request);
				break;
			}
		}
		return JVX_NO_ERROR;
	}

	return CjvxConsoleHost_be_print::process_event(theQueueElement
		/*
		message_id, origin, priv_fe,
		param, paramType, event_type, event_class,
		event_priority, deleteOnProcess, privBlock, reschedule*/
	);
}

// =============================================================================================
// =============================================================================================

jvxErrorType
CjvxConsoleHost_be_drivehost::report_event(
	jvxCBitField event_mask, const char* descriptionin, jvxSize sequenceId,
	jvxSize stepId, jvxSequencerQueueType qtp, jvxSequencerElementType etp, 
	jvxSize fId, jvxSize oper_state, jvxBool indicateFirstError)
{
	jvxTick tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.myTimerRef);
	jvxData tt_msec = (jvxData)tt * 0.001; // in msecs
	std::string txt = "[" + jvx_data2String(tt_msec, 2) + " msec]";
	std::string description_str = descriptionin;
	if (event_mask & JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE)
	{
		txt += "Startup process complete, message: ";
		txt += description_str;
		txt = jvx_textMessagePrioToString(txt, JVX_REPORT_PRIORITY_SUCCESS);
		postMessage_outThread(txt);	
	}

	if (event_mask & JVX_SEQUENCER_EVENT_SEQUENCE_STARTUP_COMPLETE)
	{
		txt += "Startup sequence complete, message: ";
		txt += description_str;
		txt = jvx_textMessagePrioToString(txt, JVX_REPORT_PRIORITY_SUCCESS);
		postMessage_outThread(txt);
	}

	if (event_mask & JVX_SEQUENCER_EVENT_REPORT_OPERATION_STATE)
	{
		txt = jvx_create_text_seq_report("Report Operation State while Waiting", sequenceId, stepId, qtp, etp, fId, oper_state, 
			description_str);
		postMessage_outThread(txt);
	}

	if (event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP)
	{
		txt = jvx_create_text_seq_report("Successful completion sequence step", sequenceId, stepId, qtp, etp, fId, oper_state, 
			description_str);
		txt = jvx_textMessagePrioToString(txt, JVX_REPORT_PRIORITY_SUCCESS);
		postMessage_outThread(txt);

		if (etp == JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER)
		{
			// What can we do here?
		}
	}
	if (event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR)
	{
		txt = jvx_create_text_seq_report("Incomplete completion sequence step", sequenceId, stepId, qtp, etp, fId, oper_state, 
			description_str);
		txt = jvx_textMessagePrioToString(txt, JVX_REPORT_PRIORITY_ERROR);
		postMessage_outThread(txt);

		if (indicateFirstError)
		{
			if (runtime.firstError.empty())
			{
				runtime.firstError = description_str;
			}
		}
	}

	if (event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE)
	{
		txt += "Successful completion sequence " + jvx_size2String(sequenceId) + ", message ";
		txt += description_str;
		txt = jvx_textMessagePrioToString(txt, JVX_REPORT_PRIORITY_SUCCESS);
		postMessage_outThread(txt);

	}
	if (event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ERROR)
	{
		txt += "Incomplete completion sequence " + jvx_size2String(sequenceId) +
			" due to error, message: ";
		txt += description_str;
		txt = jvx_textMessagePrioToString(txt, JVX_REPORT_PRIORITY_ERROR);
		postMessage_outThread(txt);
	}

	if (event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT)
	{
		txt += "Incomplete completion sequence " + jvx_size2String(sequenceId) + 
			" due to abort, message: ";
		txt += description_str;
		txt = jvx_textMessagePrioToString(txt, JVX_REPORT_PRIORITY_INFO);
		postMessage_outThread(txt);
	}

	if (event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS)
	{
		txt += "Successful completion process.";
		txt = jvx_textMessagePrioToString(txt, JVX_REPORT_PRIORITY_SUCCESS);
		linestart = JVX_DEFINE_PROCESSING_OFF;
		postMessage_outThread(txt);

	}
	if (event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR)
	{
		description_str = runtime.firstError; // <- in this case, we report the first error that occurred in processing
		txt += "Incomplete completion process error, message: ";
		txt += description_str;
		txt = jvx_textMessagePrioToString(txt, JVX_REPORT_PRIORITY_ERROR);
		linestart = JVX_DEFINE_PROCESSING_OFF;
		postMessage_outThread(txt);

	}
	if (event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT)
	{
		txt += "Process aborted.";
		txt = jvx_textMessagePrioToString(txt, JVX_REPORT_PRIORITY_WARNING);
		linestart = JVX_DEFINE_PROCESSING_OFF;
		postMessage_outThread(txt);
	}
	if (event_mask & JVX_SEQUENCER_EVENT_PROCESS_TERMINATED)
	{
		txt += "Process terminated.";		
		txt = jvx_textMessagePrioToString(txt, JVX_REPORT_PRIORITY_SUCCESS);
		postMessage_outThread(txt);		
	}

	if (event_mask & JVX_SEQUENCER_EVENT_DEBUG_MESSAGE)
	{
		/*
		if (checkBox_debug_sequencer->isChecked())
		{
			txt += description;
			runtime.report->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);
		}*/
	}

	if (event_mask & JVX_SEQUENCER_EVENT_INFO_TEXT)
	{
		txt += description_str;
		txt = jvx_textMessagePrioToString(txt, JVX_REPORT_PRIORITY_INFO);
		postMessage_outThread(txt);
	}

	if (event_mask & ~JVX_SEQUENCER_EVENT_DEBUG_MESSAGE)
	{
		jvxSpecialEventType specEvent;
		specEvent.ev = JVX_EVENTLOOP_SPECIAL_EVENT_SEQUENCER_UPDATE;
		jvxSpecialEventType_seq seqEv;
		seqEv.event_mask = event_mask;
		seqEv.description = description_str.c_str(); // Can work with no copy since call is IMMEDIATE
		seqEv.sequenceId = sequenceId;
		seqEv.stepId = stepId;
		seqEv.qtp = qtp;
		seqEv.etp = etp; 
		seqEv.fId = fId;
		seqEv.oState = oper_state;
		specEvent.dat = (jvxHandle*)&seqEv;

		if (linkedPriFrontend.fe)
		{
			TjvxEventLoopElement qel;
			qel.message_id = JVX_SIZE_UNSELECTED;
			qel.param = &specEvent;
			qel.paramType = JVX_EVENTLOOP_DATAFORMAT_SPECIAL_EVENT;
			qel.eventClass = JVX_EVENTLOOP_REQUEST_IMMEDIATE;
			qel.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
			qel.origin_fe = NULL;

			linkedPriFrontend.fe->report_special_event(&qel, NULL);
		}

		std::list<jvxOneFrontendAndState>::iterator elm = this->linkedSecFrontends.begin();
		for (; elm != linkedSecFrontends.end(); elm++)
		{
			TjvxEventLoopElement qel;
			qel.message_id = JVX_SIZE_UNSELECTED;
			qel.param = &specEvent;
			qel.paramType = JVX_EVENTLOOP_DATAFORMAT_SPECIAL_EVENT;
			qel.eventClass = JVX_EVENTLOOP_REQUEST_IMMEDIATE;
			qel.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
			qel.origin_fe = NULL;

			(*elm).fe->report_special_event(&qel, NULL);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::sequencer_callback(jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSize functionId)
{
	// What to do here?
	return JVX_NO_ERROR;
}

// =============================================================================================
// =============================================================================================

jvxErrorType
CjvxConsoleHost_be_drivehost::get_configuration_ext(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxValue val;
	jvxConfigData* theDat = NULL;
	if (processor)
	{
		jvxValue val;
		jvxConfigData* datTmp = NULL;
		jvxConfigData* datSec = NULL;

		val.assign(config.auto_start);
		processor->createAssignmentValue(&datTmp, JVX_COMMON_PROPERTIES_HOST_AUTOSTART, val);
		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);
		datTmp = NULL;

		val.assign(config_show.numdigits);
		processor->createAssignmentValue(&datTmp, "JVX_CONSOLE_HOST_NUM_DIGITS", val);
		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);
		datTmp = NULL;

		val.assign(config.timeout_seq_msec);
		processor->createAssignmentValue(&datTmp, "JVX_CONSOLE_HOST_TIMEOUT_SEQ", val);
		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);
		datTmp = NULL;

		val.assign(config_act.silent_out);
		processor->createAssignmentValue(&datTmp, "JVX_CONSOLE_HOST_SILENT_OUT", val);
		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);
		datTmp = NULL;

		val.assign(config_show.outputPropertyFieldsBase64);
		processor->createAssignmentValue(&datTmp, "JVX_CONSOLE_HOST_PROP_OUT_B64", val);
		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);
		datTmp = NULL;

		val.assign(config_show.get_property_compact);
		processor->createAssignmentValue(&datTmp, "JVX_CONSOLE_HOST_PROP_COMPACT", val);
		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);
		datTmp = NULL;
		
	}

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::put_configuration_ext(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename, jvxInt32 lineno)
{
	jvxConfigData* datTmp = NULL;
	jvxValue val;
	jvxErrorType res;
	if (processor)
	{
		res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, JVX_COMMON_PROPERTIES_HOST_AUTOSTART);
		if (res == JVX_NO_ERROR)
		{
			processor->getAssignmentValue(datTmp, &val);
			val.toContent(&config.auto_start);
		}

		res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, "JVX_CONSOLE_HOST_NUM_DIGITS");
		if (res == JVX_NO_ERROR)
		{
			processor->getAssignmentValue(datTmp, &val);
			val.toContent(&config_show.numdigits);
		}

		res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, "JVX_CONSOLE_HOST_TIMEOUT_SEQ");
		if (res == JVX_NO_ERROR)
		{
			processor->getAssignmentValue(datTmp, &val);
			val.toContent(&config.timeout_seq_msec);
		}

		res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, "JVX_CONSOLE_HOST_SILENT_OUT");
		if (res == JVX_NO_ERROR)
		{
			processor->getAssignmentValue(datTmp, &val);
			val.toContent(&config_act.silent_out);
		}

		res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, "JVX_CONSOLE_HOST_PROP_OUT_B64");
		if (res == JVX_NO_ERROR)
		{
			processor->getAssignmentValue(datTmp, &val);
			val.toContent(&config_show.outputPropertyFieldsBase64);
		}

		res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, "JVX_CONSOLE_HOST_PROP_COMPACT");
		if (res == JVX_NO_ERROR)
		{
			processor->getAssignmentValue(datTmp, &val);
			val.toContent(&config_show.get_property_compact);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::boot_initialize_product()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = JVX_APPHOST_PRODUCT_CLASSNAME::boot_initialize_product();
	return res;
}


void
CjvxConsoleHost_be_drivehost::report_command_request_inThread(jvxCBitField request, jvxHandle* caseSpecificData, jvxSize szSpecificData)
{
	if (jvx_cbitTest(request, JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_PROCESS_SHIFT))
	{
		//control_stopSequencer();
	}

	if (jvx_cbitTest(request, JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_SUBROUTINE_SHIFT))
	{
		//control_stopSequencer();
	}
	if (jvx_cbitTest(request, JVX_REPORT_REQUEST_TRY_TRIGGER_START_SEQUENCER_SHIFT))
	{
		//control_startSequencer();
	}
	if (jvx_cbitTest(request, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT))
	{
		//this->updateWindow(request);
	}
	if (jvx_cbitTest(request, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT))
	{
		//this->updateWindow(request);
		//this->subWidgets.realtimeViewer.props.theWidget->updateWindow_update(false);
	}
	if (jvx_cbitTest(request, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT))
	{
		//this->updateWindow(request);
		//this->subWidgets.realtimeViewer.props.theWidget->updateWindow_update(true);
	}

	if (
		(jvx_cbitTest(request, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT)))
	{
		IjvxDataConnections* theDataConnections = NULL;
		IjvxDataConnectionProcess* theRef = NULL;
		IjvxConnectionMaster* theMas = NULL;
		JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
		jvxBool testOk = false;

		if (involvedComponents.theHost.hFHost)
		{
			jvxErrorType res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theDataConnections));
			if ((res == JVX_NO_ERROR) && theDataConnections)
			{
				jvxSize uId = (intptr_t)caseSpecificData;
				jvxApiString nmChain;
				JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);

				theDataConnections->reference_connection_process_uid(uId, &theRef);
				if (theRef)
				{
					// Run test on chain, the result is stored internally and will be shown in next "updateWindow"
					jvxErrorType resL = theRef->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A_NULL);

					theRef->status_chain(&testOk JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
					if (!testOk)
					{
						theRef->descriptor_connection(&nmChain);
						this->report_simple_text_message(("Problem testing chain " + nmChain.std_str() + ", please review connection manager.").c_str(), JVX_REPORT_PRIORITY_ERROR);
					}
					theDataConnections->return_reference_connection_process(theRef);

				}
				else
				{
					std::cout << "Failed to find connection process with ID = " << uId << std::endl;
				}
				involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theDataConnections));
				theDataConnections = NULL;
			}
		}
		//this->updateWindow(request);
		//this->subWidgets.realtimeViewer.props.theWidget->updateWindow_update(false);
	}
}
