#include "CjvxWebControl_fe.h"
#include "jvxTWebServer.h"
#include "CjvxJson.h"
#include "interfaces/console/IjvxEventLoop_backend_ctrl.h"
#include "CjvxWebControl_fe_private.h"

// =================================================================================================================
// =================================================================================================================

#define JVX_CREATE_ERROR_NO_RETURN(lstelmr, err, txt) \
	{ \
		CjvxJsonElement jelm; \
		jelm.makeAssignmentString("error_description", txt); \
		lstelmr.addConsumeElement(jelm); \
	} \

CjvxWebControl_fe::CjvxWebControl_fe()
{
	  timerCount = 0;
	JVX_INITIALIZE_MUTEX(safeAccessMemList);

	evLop = NULL;
	theState = JVX_STATE_INIT;
	myHostRef = NULL;
	myWebServer.hdl = NULL;
	myWebServer.obj = NULL;
	myWebServer.closeProcedureState = 0;

	binWs.initialize(this);
	txtWs.initialize(this);

	config.silent_mode = true;
	config.hostRefConfigExtender = NULL;
	config.numThreads = 8;
}

CjvxWebControl_fe::~CjvxWebControl_fe()
{
	JVX_TERMINATE_MUTEX(safeAccessMemList);
}

jvxErrorType
CjvxWebControl_fe::add_reference_event_loop(IjvxEventLoopObject* eventLoop)
{
	if (theState == JVX_STATE_INIT)
	{
		if (evLop == NULL)
		{
			evLop = eventLoop;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ALREADY_IN_USE;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxWebControl_fe::clear_reference_event_loop(IjvxEventLoopObject* eventLoop)
{
	if (theState == JVX_STATE_INIT)
	{
		if (evLop == eventLoop)
		{
			evLop = NULL;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return JVX_ERROR_WRONG_STATE;
}

// ===========================================================================

jvxErrorType
CjvxWebControl_fe::set_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* thePriBackend)
{
	if (thePriBackend)
	{
		if (linkedPriBackend.be == NULL)
		{
			IjvxReport* this_report = static_cast<IjvxReport*>(this);
			linkedPriBackend.be = thePriBackend;
			linkedPriBackend.st = JVX_STATE_SELECTED;
			linkedPriBackend.be->add_external_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(this_report));
			linkedPriBackend.be->add_sec_reference_backend(static_cast<IjvxEventLoop_backend*>(this));
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxWebControl_fe::clear_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend)
{
	if (theBackend)
	{
		if (linkedPriBackend.be == theBackend)
		{
			IjvxReport* this_report = static_cast<IjvxReport*>(this);
			assert(linkedPriBackend.st == JVX_STATE_SELECTED);
			linkedPriBackend.be->clear_sec_reference_backend(static_cast<IjvxEventLoop_backend*>(this));
			linkedPriBackend.be->remove_external_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(this_report));
			linkedPriBackend.be = NULL;
			linkedPriBackend.st = JVX_STATE_NONE;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

// ===========================================================================

jvxErrorType
CjvxWebControl_fe::add_sec_reference_event_backend(IjvxEventLoop_backend* backend)
{
	std::list<jvxOneBackendAndState>::iterator elm = std::find(linkedSecBackends.begin(), linkedSecBackends.end(), backend);
	if (elm != linkedSecBackends.end())
	{
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	jvxOneBackendAndState newElm;
	newElm.be = backend;
	newElm.st = JVX_STATE_SELECTED;
	linkedSecBackends.push_back(newElm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControl_fe::clear_sec_reference_event_backend(IjvxEventLoop_backend* backend)
{
	std::list<jvxOneBackendAndState>::iterator elm = std::find(linkedSecBackends.begin(), linkedSecBackends.end(), backend);
	if (elm == linkedSecBackends.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	linkedSecBackends.erase(elm);
	return JVX_NO_ERROR;
}

// ==============================================================================
// ==============================================================================

/*
void
CjvxWebControl_fe::start_timer(jvxSize time_diff_msec)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize mId = 0;
	std::string inputText = "This is the timer thread" + jvx_size2String(timerCount);
	timerCount++;
	JVX_LOCK_MUTEX(safeAccessMemList);

	jvxByte* newStr = NULL;
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(newStr, jvxByte, inputText.size() + 1);
	memcpy(newStr, inputText.c_str(), inputText.size());

	res = evLop->event_schedule(
		static_cast<IjvxEventLoop_frontend*>(this), NULL,
		evBackend, NULL,
		&mId, newStr, JVX_EVENTLOOP_DATAFORMAT_STRING,
		JVX_EVENTLOOP_EVENT_TEXT_INPUT, JVX_EVENTLOOP_REQUEST_TRIGGER_RESPONSE,
		JVX_EVENTLOOP_PRIORITY_TIMER, time_diff_msec);
	if (res == JVX_NO_ERROR)
	{
		oneAllocatedField theFld;
		theFld.buf = newStr;
		theFld.selector = mId;

		flds.push_back(theFld);
	}

	JVX_UNLOCK_MUTEX(safeAccessMemList);
}
*/

jvxErrorType
CjvxWebControl_fe::returns_from_start(jvxBool* doesReturn)
{
	if (doesReturn)
		*doesReturn = true;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControl_fe::start(int argc, char* argv[])
{
	std::string inputText;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize mId = JVX_SIZE_UNSELECTED; 
	if (theState == JVX_STATE_INIT)
	{
		// Start the web services
		theState = JVX_STATE_PROCESSING;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxWebControl_fe::stop()
{
	if (theState == JVX_STATE_PROCESSING)
	{
		theState = JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxWebControl_fe::wants_restart(jvxBool *wRet)
{
	if (wRet)
	{
		*wRet = false;
	}
	return JVX_NO_ERROR;
}

// =========================================================================
	
void
CjvxWebControl_fe::right_before_start()
{
	jvxErrorType resL = JVX_NO_ERROR;
	IjvxProperties* cfgHdl = NULL;
	jvxApiString tmpStr;
	jvxCallManagerProperties callGate;
	myWebServer.hdl->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&cfgHdl));
	if (cfgHdl)
	{
		tmpStr.assign_const(config.docRoot.c_str(), config.docRoot.size());

		resL = jvx_set_property(cfgHdl, (jvxHandle*)&tmpStr, 0, 1, JVX_DATAFORMAT_STRING, true, "/wwwRootDirectory", callGate);
		resL = jvx_set_property(cfgHdl, (jvxHandle*)&config.listeningPort, 0, 1, JVX_DATAFORMAT_SIZE, true, "/wwwListeningPort", callGate);
		resL = jvx_set_property(cfgHdl, (jvxHandle*)&config.numThreads, 0, 1, JVX_DATAFORMAT_SIZE, true, "/wwwNumberThreads", callGate);
		myWebServer.hdl->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(cfgHdl));
	}
}

jvxErrorType 
CjvxWebControl_fe::report_process_event(TjvxEventLoopElement* theQueueElement)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType resStart = JVX_NO_ERROR;
	oneThreadReturnType* myPrivateMem = NULL;
	jvxWebContext* ctxt = NULL;
	
	jvxSize message_id = theQueueElement->message_id;
	jvxHandle* param = theQueueElement->param;
	jvxSize paramType = theQueueElement->paramType;
	jvxSize event_type = theQueueElement->eventType;
	jvxOneEventClass event_class = theQueueElement->eventClass;
	jvxOneEventPriority event_priority = theQueueElement->eventPriority;
	jvxCBool autoDeleteOnProcess = theQueueElement->autoDeleteOnProcess;
	jvxHandle* privBlock = NULL;
	oneAddedStaticComponent comp;

	if (theQueueElement->oneHdlBlock)
	{
		privBlock = theQueueElement->oneHdlBlock->priv;
	}

	//JVX_THREAD_ID myThread;
	switch (event_type)
	{
	case JVX_EVENTLOOP_EVENT_SELECT:

		assert(myHostRef == NULL);
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_IJVXHOST_REF);

		myHostRef = reinterpret_cast<IjvxHost*>(param);
		resL = myHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&myToolsHost));

		LOAD_ONE_MODULE_LIB_FULL(jvxTWebServer_init, jvxTWebServer_terminate, "Web Server", addedStaticObjects, myHostRef);
		myHostRef->add_component_load_blacklist(JVX_COMPONENT_WEBSERVER);
		break;

	case JVX_EVENTLOOP_EVENT_ACTIVATE:

		// Request web server
		if ((resL == JVX_NO_ERROR) && myToolsHost)
		{
			resL = myToolsHost->reference_tool(JVX_COMPONENT_WEBSERVER, &myWebServer.obj, 0, NULL);
			if ((resL == JVX_NO_ERROR) && myWebServer.obj)
			{
				myWebServer.obj->request_specialization(reinterpret_cast<jvxHandle**>(&myWebServer.hdl), NULL, NULL);
			}
		}

		// Prepare shutdown at this point already
		myWebServer.closeProcedureState = 0;

		// Start web server
		if (myWebServer.hdl == NULL)
		{
			std::cout << "Web server handle not available. In most cases, this error is due to a worng option --jvxdir in the startup command!" << std::endl;
		}

		assert(myWebServer.hdl);
		resStart = start_webserver(myWebServer.hdl, myHostRef, static_cast<IjvxWebServerHost_hooks*>(this));

		// Request log file stream
		jvx_init_text_log(embLog);
		embLog.jvxrtst_bkp.theModuleName = "CjvxWebControl_fe";
		embLog.jvxrtst_bkp.theToolsHost = myToolsHost;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(embLog.jvxrtst_bkp.jvxlst_buf, char, JVX_RTST_BUFFER_SIZE);
		embLog.jvxrtst_bkp.jvxlst_buf_sz = JVX_RTST_BUFFER_SIZE;
		jvx_request_text_log(embLog);

		jvx_lock_text_log(embLog, 0 JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
		embLog.jvxrtst << "Started Host." << std::endl;
		jvx_unlock_text_log(embLog JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);

		res = myHostRef->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle**>(&config.hostRefConfigExtender));
		if ((res == JVX_NO_ERROR) && config.hostRefConfigExtender)
		{
			res = config.hostRefConfigExtender->register_extension(static_cast<IjvxConfigurationExtender_report*>(this), JVX_WEBCONTROL_SECTION);
		}

		// Report the failed startup to caller
		if (resStart != JVX_NO_ERROR)
		{
			res =JVX_ERROR_INVALID_SETTING;
		}

		break;
	case JVX_EVENTLOOP_EVENT_DEACTIVATE:
		
		if (config.hostRefConfigExtender)
		{
			res = config.hostRefConfigExtender->unregister_extension(JVX_WEBCONTROL_SECTION);
		}
		res = myHostRef->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle*>(config.hostRefConfigExtender));
		config.hostRefConfigExtender = NULL;

		jvx_return_text_log(embLog);
		JVX_DSP_SAFE_DELETE_FIELD(embLog.jvxrtst_bkp.jvxlst_buf);
		jvx_terminate_text_log(embLog);

		// From here, do not accept any events from this frontend
		evLop->frontend_block(this);

		// Then, delete all pending events
		evLop->event_clear(JVX_SIZE_UNSELECTED, static_cast<IjvxEventLoop_frontend*>(this), NULL);

		// Stop web server
		if (myWebServer.hdl)
		{
			resL = myWebServer.hdl->stop();
		}
		resL = stop_webserver();
		assert(resL == JVX_NO_ERROR);

		break;

	case JVX_EVENTLOOP_EVENT_UNSELECTED:
		myHostRef->remove_component_load_blacklist(JVX_COMPONENT_WEBSERVER);
		for (i = 0; i < addedStaticObjects.size(); i++)
		{
			UNLOAD_ONE_MODULE_LIB_FULL(addedStaticObjects[i], myHostRef);
		}
		addedStaticObjects.clear();
		myHostRef = NULL;
		break;

	case JVX_EVENT_LOOP_EVENT_WS_READY:

		ctxt = (jvxWebContext*)param;
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 1);
		JVX_DSP_SAFE_DELETE_OBJECT(ctxt);

	default:
		res = cleanup(message_id, param, paramType, event_type, event_class, event_priority, autoDeleteOnProcess);
		break;
	}
	return res;
}

jvxErrorType
CjvxWebControl_fe::report_cancel_event(TjvxEventLoopElement* theQueueElement)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxWebContext* ctxt = NULL;
	//std::cout << "Message cancelled." << std::endl;
	switch (theQueueElement->eventType)
	{
	case JVX_EVENT_LOOP_EVENT_WS_READY:

		ctxt = (jvxWebContext*)theQueueElement->param;
		assert(theQueueElement->paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 1);
		JVX_DSP_SAFE_DELETE_OBJECT(ctxt);
	default:
		res = cleanup(theQueueElement->message_id, theQueueElement->param, theQueueElement->paramType, 
			theQueueElement->eventType, theQueueElement->eventClass, theQueueElement->eventPriority, 
			theQueueElement->autoDeleteOnProcess);
	}
	return res;
}

jvxErrorType
CjvxWebControl_fe::cleanup(
	jvxSize message_id,
	jvxHandle* param, jvxSize paramType,
	jvxSize event_type, jvxOneEventClass event_class, 
	jvxOneEventPriority event_priority,
	jvxCBool autoDeleteOnProcess)
{
	std::string* newStr = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	if (!autoDeleteOnProcess)
	{
		JVX_LOCK_MUTEX(safeAccessMemList);
		std::vector<oneAllocatedField>::iterator elm = flds.end();
		elm = jvx_findItemSelectorInList < oneAllocatedField, jvxSize>(flds, message_id);
		if (elm != flds.end())
		{
			switch (paramType)
			{
			case JVX_EVENTLOOP_DATAFORMAT_STDSTRING:
				newStr = (std::string*)elm->buf;
				JVX_DSP_SAFE_DELETE_OBJECT(newStr);
				break;
			default:
				res = JVX_ERROR_INVALID_SETTING;
				break;
			}
			flds.erase(elm);
		}
		JVX_UNLOCK_MUTEX(safeAccessMemList);
	}
	return res;
}

// ======================================================================================

jvxErrorType
CjvxWebControl_fe::synchronizeWebServerCoEvents(jvxHandle* context_server, 
	jvxHandle* context_conn, jvxWebServerHandlerPurpose purp, 
	jvxSize uniqueId, jvxBool strictConstConnection, 
	const char* uriprefix)
{
	std::string command;
	jvxBool requiresInterpretation = false;
	jvxBool errorDetected = false;
	std::string errorDescription;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize myId = 0;
	jvxApiString str;
	jvxBool was_registered = false;
	//jvxSize messId = JVX_SIZE_UNSELECTED;
	oneThreadReturnType myPrivateMemReturn;

	//if(!theWebserver.closeProcedureStarted)
	//{
	// -> TODO:: emit emit_report_event_request(context_server, context_conn, purp, uniqueId, strictConstConnection, header, payload, szFld);
	//}
	report_event_request_translate(
		context_server, context_conn, purp, uniqueId, strictConstConnection, uriprefix, 0,
		NULL, 0, command, requiresInterpretation, errorDetected, errorDescription, &embLog.jvxrtst,
		static_cast<jvx_lock_log*>(&embLog.jvxrtst_bkp.jvxos), (config.silent_mode!= c_false));

	// Error priority is higher than the interpretation prio!!
	if (errorDetected)
	{
		CjvxJsonElementList jsec;
		CjvxJsonElement jelm; 
		std::string strOut;
		jelm.makeAssignmentString("error_description", errorDescription);
		jsec.addConsumeElement(jelm); 
		jelm.makeAssignmentString("return_code", jvxErrorType_str[JVX_ERROR_INVALID_ARGUMENT].friendly);
		jsec.insertConsumeElementFront(jelm);

		jsec.printString(strOut, JVX_JSON_PRINT_JSON, 0, "", "", "", false);
		this->hdl->in_connect_write_header_response(context_server, context_conn, JVX_WEB_SERVER_RESPONSE_JSON);
		hdl->in_connect_write_response(context_server, context_conn, strOut.c_str());
		if (!config.silent_mode)
		{
			std::cout << "Return: " << strOut << std::endl;
		}
	}
	else if (requiresInterpretation)
	{
	    // std::cout << __FUNCTION__ << " entering blocking loop" << std::endl;
		TjvxEventLoopElement elm;

	    myPrivateMemReturn.res_mthread = JVX_ERROR_UNSUPPORTED;
		myPrivateMemReturn.ret_mthread = "";
		evLop->reserve_message_id(&elm.message_id);
		str.assign_const(command.c_str(), command.size());

		// Transfer command to the primary backend
		
		elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
		elm.priv_fe = NULL;
		elm.target_be = linkedPriBackend.be;
		elm.priv_be = NULL;

		elm.param = (jvxHandle*)(&str);
		elm.paramType = JVX_EVENTLOOP_DATAFORMAT_JVXAPISTRING;
		
		elm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
		elm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
		elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		elm.delta_t = JVX_SIZE_UNSELECTED;
		elm.autoDeleteOnProcess = c_false;

		res = evLop->event_schedule(&elm, NULL,&myPrivateMemReturn);
		switch (res)
		{
		case JVX_ERROR_END_OF_FILE:
			std::cout << "Disconnected websocket not reported since host shutdown deadline was missed!" << std::endl;
			break;
		case JVX_ERROR_ABORT:
			std::cout << "Disconnected websocket not reported since host does not accept events from this frontend currently!" << std::endl;
			break;
		case JVX_ERROR_WRONG_STATE:
			std::cout << "Event scheduler currently in wrong state!" << std::endl;
			break;
		case JVX_NO_ERROR:
			break;
		default:
			assert(0);
		}
		// After execution of reaction in other thread..
		// Process the response
		this->hdl->in_connect_write_header_response(context_server, context_conn, JVX_WEB_SERVER_RESPONSE_JSON);
	    hdl->in_connect_write_response(context_server, context_conn, myPrivateMemReturn.ret_mthread.c_str());
		if (!config.silent_mode)
		{
			std::cout << "Return: " << myPrivateMemReturn.ret_mthread << std::endl;
		}
	    //std::cout << __FUNCTION__ << " leaving blocking loop" << std::endl;
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return(res);
}

	void
		CjvxWebControl_fe::process_incoming_text(jvxHandle* ctxt, char* payload, jvxSize szFld)
	{
		TjvxEventLoopElement elm;
		jvxOneWsTextRequest fwdRequest;
		std::string cmd(payload, szFld);
		fwdRequest.cmd = cmd;
		fwdRequest.ctxt = ctxt;

		// Schedule removal of timeout thread followed by new timeout thread
		elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
		elm.priv_fe = NULL;
		elm.target_be = static_cast<IjvxEventLoop_backend*>(this);
		elm.priv_be = NULL;

		elm.param = (jvxHandle*)&fwdRequest;
		elm.paramType = JVX_EVENTLOOP_DATAFORMAT_SPEC_JVXONEWSTEXTREQUEST;

		elm.eventType = JVX_EVENT_LOOP_EVENT_WS_TEXT_INPUT;
		elm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
		elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		elm.delta_t = JVX_TIMOUT_BLOCKING_CALL_MSEC;
		elm.autoDeleteOnProcess = c_false;

		jvxErrorType res = this->evLop->event_schedule(&elm, NULL, NULL);
		switch (res)
		{
		case JVX_ERROR_END_OF_FILE:
			std::cout << "Disconnected websocket not reported since host shutdown deadline was missed!" << std::endl;
			break;
		case JVX_ERROR_ABORT:
			std::cout << "Disconnected websocket not reported since host does not accept events from this frontend currently!" << std::endl;
			break;
		case JVX_NO_ERROR:
			break;
		default:
			assert(0);
		}
	}

	void 
	CjvxWebControl_fe::process_incoming_binary(char* payload, size_t szFld)
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxUInt16* ptr16 = NULL;
		jvxProtocolHeader* protheader = (jvxProtocolHeader*)payload;
		if (protheader->fam_hdr.proto_family == JVX_PROTOCOL_TYPE_PROPERTY_STREAM)
		{
			/* ====================================================*/
			/* Incoming request to (re-)configure web socket timer */
			/* ====================================================*/

			if (
				((protheader->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
				((protheader->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PS_CONFIGURE_PROPERTY_OBSERVATION))
			{
				TjvxEventLoopElement elm;

				// TODO: Check that state is correct
				assert(protheader->paketsize == szFld);
				assert(szFld >= sizeof(jvxPropertyConfigurePropertySend));
				jvxPropertyConfigurePropertySend* pconfheader = (jvxPropertyConfigurePropertySend*)protheader;

				// Start timer "thread"

				jvxSize locId = JVX_SIZE_UNSELECTED;

				jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
				if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
				{
					jvx_lock_text_log(embLog, logLev JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
					embLog.jvxrtst << "Incoming request web socket configuration:" << std::endl;
					embLog.jvxrtst << "Ticktime [msec] = " << pconfheader->tick_msec << std::endl;
					embLog.jvxrtst << "Ping count = " << pconfheader->ping_count << std::endl;
					embLog.jvxrtst << "User [int32] = " << pconfheader->user_specific << std::endl;
					jvx_unlock_text_log(embLog JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
				}

				// Schedule removal of timeout thread followed by new timeout thread
				elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
				elm.priv_fe = NULL;
				elm.target_be = static_cast<IjvxEventLoop_backend*>(this);
				elm.priv_be = NULL;

				elm.param = (jvxHandle*)pconfheader;
				elm.paramType = JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 3;

				elm.eventType = JVX_EVENT_LOOP_EVENT_WS_TIMEOUT_RECONF;
				elm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
				elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
				elm.delta_t = JVX_TIMOUT_BLOCKING_CALL_MSEC;
				elm.autoDeleteOnProcess = c_false;

				res = this->evLop->event_schedule(&elm, NULL, NULL);
				switch (res)
				{
				case JVX_ERROR_END_OF_FILE:
					std::cout << "Disconnected websocket not reported since host shutdown deadline was missed!" << std::endl;
					break;
				case JVX_ERROR_ABORT:
					std::cout << "Disconnected websocket not reported since host does not accept events from this frontend currently!" << std::endl;
					break;
				case JVX_NO_ERROR:
					break;
				default:
					assert(0);
				}
			}
			else if (
				((protheader->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
				((protheader->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PS_ADD_PROPERTY_TO_OBSERVE))
			{
				/* ====================================================*/
				/* Add a property update request to list of properties */
				/* ====================================================*/
				TjvxEventLoopElement elm;

				// TODO: Check that state is correct
				assert(protheader->paketsize == szFld);
				assert(szFld >= sizeof(jvxPropertyPropertyObserveHeader));

				jvxPropertyPropertyObserveHeader* paddheader = (jvxPropertyPropertyObserveHeader*)protheader;
				elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
				elm.priv_fe = NULL;
				elm.target_be = static_cast<IjvxEventLoop_backend*>(this);
				elm.priv_be = NULL;

				elm.param = (jvxHandle*)paddheader;
				elm.paramType = JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 2;

				elm.eventType = JVX_EVENT_LOOP_EVENT_WS_ADD_PROP_LIST ;
				elm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
				elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
				elm.delta_t = JVX_TIMOUT_BLOCKING_CALL_MSEC;
				elm.autoDeleteOnProcess = c_false;

				res = this->evLop->event_schedule(&elm, NULL, NULL);
				switch (res)
				{
				case JVX_ERROR_END_OF_FILE:
					std::cout << "Disconnected websocket not reported since host shutdown deadline was missed!" << std::endl;
					break;
				case JVX_ERROR_ABORT:
					std::cout << "Disconnected websocket not reported since host does not accept events from this frontend currently!" << std::endl;
					break;
				case JVX_NO_ERROR:
					break;
				default:
					assert(0);
				}
			}
			else if (
				((protheader->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST) &&
				((protheader->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PS_REMOVE_PROPERTY_TO_OBSERVE))
			{
				/* ====================================================*/
				/* Add a property update request to list of properties */
				/* ====================================================*/
				TjvxEventLoopElement elm;

				// TODO: Check that state is correct
				assert(protheader->paketsize == szFld);
				assert(szFld >= sizeof(jvxPropertyPropertyObserveHeader));

				jvxPropertyPropertyObserveHeader* paddheader = (jvxPropertyPropertyObserveHeader*)protheader;
				elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
				elm.priv_fe = NULL;
				elm.target_be = static_cast<IjvxEventLoop_backend*>(this);
				elm.priv_be = NULL;

				elm.param = (jvxHandle*)paddheader;
				elm.paramType = JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 2;

				elm.eventType = JVX_EVENT_LOOP_EVENT_WS_REM_PROP_LIST;
				elm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
				elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
				elm.delta_t = JVX_TIMOUT_BLOCKING_CALL_MSEC;
				elm.autoDeleteOnProcess = c_false;

				res = this->evLop->event_schedule(&elm, NULL, NULL);
				switch (res)
				{
				case JVX_ERROR_END_OF_FILE:
					std::cout << "Disconnected websocket not reported since host shutdown deadline was missed!" << std::endl;
					break;
				case JVX_ERROR_ABORT:
					std::cout << "Disconnected websocket not reported since host does not accept events from this frontend currently!" << std::endl;
					break;
				case JVX_NO_ERROR:
					break;
				default:
					assert(0);
				}
			}
			else if (
				((protheader->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
				((protheader->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PS_SEND_LINEARFIELD))
			{
				/* ====================================================*/
					/* Add a property update request to list of properties */
					/* ====================================================*/
				TjvxEventLoopElement elm;

				// TODO: Check that state is correct
				assert(protheader->paketsize == szFld);
				assert(szFld >= sizeof(jvxRawStreamHeader_response));

				//jvxPropertyPropertyObserveHeader_response* paddheader = (jvxPropertyPropertyObserveHeader_response*)protheader;
				elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
				elm.priv_fe = NULL;
				elm.target_be = static_cast<IjvxEventLoop_backend*>(this);
				elm.priv_be = NULL;

				elm.param = (jvxHandle*)protheader;
				elm.paramType = JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 4;

				elm.eventType = JVX_EVENT_LOOP_EVENT_WS_FLOW_EVENT;
				elm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
				elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
				elm.delta_t = JVX_TIMOUT_BLOCKING_CALL_MSEC;
				elm.autoDeleteOnProcess = c_false;

				res = this->evLop->event_schedule(&elm, NULL, NULL);
				switch (res)
				{
				case JVX_ERROR_END_OF_FILE:
					std::cout << "Disconnected websocket not reported since host shutdown deadline was missed!" << std::endl;
					break;
				case JVX_ERROR_ABORT:
					std::cout << "Disconnected websocket not reported since host does not accept events from this frontend currently!" << std::endl;
					break;
				case JVX_NO_ERROR:
					break;
				default:
					assert(0);
				}
			}

			if (
				((protheader->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
				((protheader->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PROTOCOL_TYPE_RAW_PROPERTY_STREAM))
			{
				std::cout << "Accepted incoming property stream response" << std::endl;
			}
		}
	}

jvxErrorType
CjvxWebControl_fe::synchronizeWebServerWsEvents(jvxHandle* context_server,
	jvxHandle* context_conn, jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
	jvxBool strictConstConnection, const char* uriprefix, int headerbyte, char *payload, size_t szFld)
{
	jvxBool dataHandled = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	if (jvx_bitTest(purp, JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_SHIFT))
	{
		jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
		if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
		{
			jvx_lock_text_log(embLog, logLev JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
			embLog.jvxrtst << "Web socket connected" << std::endl;
			jvx_unlock_text_log(embLog JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
		}
	}
	else if (jvx_bitTest(purp, JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_SHIFT))
	{
		TjvxEventLoopElement elm;
		jvxWebContext* ctxt = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ctxt, jvxWebContext);
		ctxt->context_conn = context_conn;
		ctxt->context_server = context_server;
		ctxt->uri = uriprefix;

		jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);
		if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
		{
			jvx_lock_text_log(embLog, logLev JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
			embLog.jvxrtst << "Web socket ready" << std::endl;
			jvx_unlock_text_log(embLog JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
		}

		elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
		elm.priv_fe = NULL;
		elm.target_be = static_cast<IjvxEventLoop_backend*>(this);
		elm.priv_be = NULL;

		elm.param = ctxt;
		elm.paramType = JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 1;

		elm.eventType = JVX_EVENT_LOOP_EVENT_WS_READY;
		elm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER_RESPONSE;
		elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		elm.delta_t = JVX_TIMOUT_BLOCKING_CALL_MSEC;
		elm.autoDeleteOnProcess = c_false;

		res = this->evLop->event_schedule(&elm, NULL, NULL); // Wait for a may of 1 second for response
		switch (res)
		{
		case JVX_ERROR_END_OF_FILE:
			std::cout << "Disconnected websocket not reported since host shutdown deadline was missed!" << std::endl;
			break;
		case JVX_ERROR_ABORT:
			std::cout << "Disconnected websocket not reported since host does not accept events from this frontend currently!" << std::endl;
			break;
		case JVX_NO_ERROR:
			break;
		default:
			assert(0);
		}
	}
	else if (jvx_bitTest(purp, JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_SHIFT))
	{
		/* Handle incoming data */
		jvxUInt16* ptr16 = nullptr;
		jvxByte opcode = headerbyte & 0xF;
		jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);

		switch(opcode)
		{

		case 0x1: // Binary message

			process_incoming_text(context_conn, payload, szFld);
			break;

		case 0x2: // Binary message
				
			if (uriprefix == JVX_URI_BINARY_SOCKET)
			{
				// Binary data is treated immediately!!
				process_incoming_binary(payload, szFld);
			}
			else
			{
				JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
				log << "Wrong websocket mode: all binary messages need to be associated to socket </jvx/host/socket>." << std::endl;
				JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
			}
			break;

		case 0x8:
			ptr16 = (jvxUInt16*)payload;
			
			if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
			{
				jvx_lock_text_log(embLog, logLev JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
				embLog.jvxrtst << "Web Socket Close Event received, load = " << *ptr16 << std::endl;
				jvx_unlock_text_log(embLog JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
			}
			break;
		case 0x9:
			
			// Ping
			assert(0);
			break;
		case 0xA:
			// Pong
			if (uriprefix == JVX_URI_BINARY_SOCKET)
			{
				this->binWs.webSocketPeriodic.current_ping_count = 0;
			}
			else
			{
				JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
				log << "Wrong websocket mode: all binary messages need to be associated to socket </jvx/host/socket>." << std::endl;
				JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
			}
			break;
		default:
			if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
			{
				jvx_lock_text_log(embLog, logLev JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
				embLog.jvxrtst << "Unknown Web Socket Data received received" << std::endl;
				jvx_unlock_text_log(embLog JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
			}
			break;
		}
	}
	else if (jvx_bitTest(purp, JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_SHIFT))
	{
		TjvxEventLoopElement elm;
		jvxSize logLev = jvxLogLevel2Id(jvxLogLevel::JVX_LOGLEVEL_4_DEBUG_OPERATION_WITH_AVRG_DEGREE_DEBUG);

		if (embLog.jvxrtst_bkp.theTextLogger_hdl && embLog.jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev))
		{
			jvx_lock_text_log(embLog, logLev JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
			embLog.jvxrtst << "Web socket closed" << std::endl;
			jvx_unlock_text_log(embLog JVX_TEXT_LOG_LOCK_ORIGIN_DEFAULT_ADD);
		}

		elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
		elm.priv_fe = NULL;
		elm.target_be = static_cast<IjvxEventLoop_backend*>(this);
		elm.priv_be = NULL;

		elm.param = context_conn;
		elm.paramType = JVX_EVENTLOOP_DATAFORMAT_HANDLE_PTR;

		elm.eventType = JVX_EVENT_LOOP_EVENT_WS_CLOSE;
		elm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER_RESPONSE;
		elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		elm.delta_t = JVX_TIMOUT_BLOCKING_CALL_MSEC;
		elm.autoDeleteOnProcess = c_false;

		res = this->evLop->event_schedule(&elm, NULL, NULL); // Wait for a may of 1 second for response
		switch (res)
		{
		case JVX_ERROR_END_OF_FILE:
			std::cout << "Disconnected websocket not reported since host shutdown deadline was missed!" << std::endl;
			break;
		case JVX_ERROR_ABORT:
			std::cout << "Disconnected websocket not reported since host does not accept events from this frontend currently!" << std::endl;
			break;
		case JVX_NO_ERROR:
			break;
		default:
			assert(0);
		}

		if (res != JVX_NO_ERROR)
		{
			binWs.force_stop_properties(context_conn);
		}
	}

	return(res);
}

jvxErrorType
CjvxWebControl_fe::report_assign_output(TjvxEventLoopElement* theQueueElement, jvxErrorType sucOperation, jvxHandle* priv)
{
	jvxErrorType res = JVX_ERROR_NO_ACCESS;
	oneThreadReturnType* myPrivateMem = (oneThreadReturnType*)priv;
	if (theQueueElement->origin_fe == static_cast<IjvxEventLoop_frontend*>(this))
	{
		// If we reach here, this callback found its way via this frontend. If not, 
		// we must decide wether or not to update the connected UI - maybe via websocket?
		assert(theQueueElement->paramType == JVX_EVENTLOOP_DATAFORMAT_JVXJSONMULTFIELDS);
		CjvxJsonElementList* jlst = (CjvxJsonElementList*)theQueueElement->param;
		CjvxJsonElementList cLst;
		assert(myPrivateMem);

		// We can add a context to better identify a request
		if (!myPrivateMem->ctx_mthread.empty())
		{
			std::string tmp;
			std::vector<std::string> errs;
			jlst->printString(tmp, JVX_JSON_PRINT_JSON, 0, "", "", "", false);
			if (CjvxJsonElementList::stringToRepresentation(tmp, cLst, errs) == JVX_NO_ERROR)
			{
				CjvxJsonElement addCtc;
				addCtc.makeAssignmentString("call_context", myPrivateMem->ctx_mthread);
				cLst.addConsumeElement(addCtc);
				jlst = &cLst;
			}
		}

		myPrivateMem->ret_mthread.clear();
		jlst->printString(myPrivateMem->ret_mthread, JVX_JSON_PRINT_JSON, 0, "", "", "", false);
		myPrivateMem->res_mthread = sucOperation;
		res = JVX_NO_ERROR;
	}
	return res;
}

jvxErrorType
CjvxWebControl_fe::hook_safeConfigFile()
{
	std::cout << __FUNCTION__ << std::endl;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxWebControl_fe::hook_startSequencer()
{
	std::cout << __FUNCTION__ << std::endl;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxWebControl_fe::hook_stopSequencer()
{
	std::cout << __FUNCTION__ << std::endl;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxWebControl_fe::hook_operationComplete()
{
	std::cout << __FUNCTION__ << std::endl;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxWebControl_fe::hook_ws_started()
{
	// TODO: Start the timer based processing
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxWebControl_fe::hook_ws_stopped()
{
	// TODO: Stop the timer based processing
	return(JVX_NO_ERROR);
}

/*
jvxErrorType
CjvxWebControl_fe::thread_cleanup(JVX_THREAD_ID threadId, jvxHandle* priv)
{
	if (priv)
	{

	}
}
*/
/*
void
CjvxWebControl_fe::slot_postPropertyStreams()
{
	postPropertyStreams();
}

void
CjvxWebControl_fe::inthread_report_event_request(jvxHandle* context_server, jvxHandle* context_conn, jvxWebServerHandlerPurpose purp, jvxSize uniqueId, jvxBool strictConstConnection, int header, char *payload, size_t szFld)
{
	theWebserver.res_mthread = JVX_ERROR_UNSUPPORTED;
	if (theWebserver.hdl)
	{
		theWebserver.res_mthread = report_event_request_core(context_server, context_conn, purp, uniqueId, strictConstConnection, header, payload, szFld);
	}
}
*/

// All functions are blocking!!!

// Events:
// JVX_EVENT_LOOP_EVENT_WS_READY => JVX_EVENTLOOP_EVENT_SPECIFIC + 1 -> Web socket connect
// JVX_EVENT_LOOP_EVENT_WS_CLOSE => JVX_EVENTLOOP_EVENT_SPECIFIC + 2 -> Web socket disconnect
// JVX_EVENT_LOOP_EVENT_WS_TIMEOUT => JVX_EVENTLOOP_EVENT_SPECIFIC + 3 -> Web socket thread timeout
// JVX_EVENT_LOOP_EVENT_WS_TIMEOUT_RECONF => JVX_EVENTLOOP_EVENT_SPECIFIC + 4 -> Timeout reconfigure message
// JVX_EVENT_LOOP_EVENT_WS_REQ_PROP_LIST => JVX_EVENTLOOP_EVENT_SPECIFIC + 5 -> Add a new property to list of push properties
// JVX_EVENT_LOOP_EVENT_WS_REM_PROP_LIST => JVX_EVENTLOOP_EVENT_SPECIFIC + 6 -> Remove a property from list of push properties
// JVX_EVENT_LOOP_EVENT_WS_FLOW_EVENT => JVX_EVENTLOOP_EVENT_SPECIFIC + 7 -> Flow control indicator from transferred property

// Types passed:
// JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 1 -> jvxWebContext*
// JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 3 -> jvxPropertyConfigurePropertySend*
// JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 2 -> jvxPropertyPropertyObserveHeader*
// JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 4 -> jvxPropertyPropertyObserveHeader_response*
// JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 5 -> jvxOneWsTextRequest*

jvxErrorType
CjvxWebControl_fe::process_event(TjvxEventLoopElement* theQueueElement)
{
	/*
	theQueueElement.message_id,
	theQueueElement.origin_fe, theQueueElement.priv_fe,
	theQueueElement.param, theQueueElement.paramType, theQueueElement.eventType,
	theQueueElement.eventClass, theQueueElement.eventPriority, theQueueElement.autoDeleteOnProcess,
	theQueueElement.oneHdlBlock->priv, &theQueueElement.rescheduleEvent);
	*/

	jvxSize message_id = theQueueElement->message_id;
	IjvxEventLoop_frontend* origin = theQueueElement->origin_fe;
	jvxHandle* priv_fe = theQueueElement->priv_fe;
	jvxHandle* param = theQueueElement->param;
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
	jvxErrorType resL = JVX_NO_ERROR;
	std::map<jvxSize, onePropertyWebSocketDefinition>::iterator elm;
#if 0 
	static jvxInt64 tStop = 0;
#endif
	jvxApiString uri;
	jvxApiString query;
	jvxApiString local_uri;
	jvxApiString url_origin;
	jvxApiString user;
	TjvxEventLoopElement evLElm;
	jvxPropertyPropertyObserveHeader* paddheader = NULL;
	jvxPropertyPropertyObserveHeader* premheader = NULL;
	jvxPropertyPropertyObserveHeader_response* pheader_resp = NULL;
	jvxPropertyConfigurePropertySend* passed = NULL;
	jvxRawStreamHeader_response*raw_pheader_resp = NULL;
	jvxOneWsTextRequest* refWsText = nullptr;
	std::string errTxt;
	CjvxJsonElementList jlstresp;
	std::string command;
	std::string f_expr;
	std::vector<std::string> args;
	jvxErrorType resParse;
	CjvxJsonElement jelm_result;

	std::string* errtxt = NULL;
	jvxBool hasDisconnected = false;
	jvxWebContext* ctxt = nullptr;

	switch (event_type)
	{
	case JVX_EVENTLOOP_EVENT_TEXT_INPUT:
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_STDSTRING);
		command = *((std::string*)param);
		if (command == "help")
		{
			std::cout << "\tweb(<address>, <varargs>) : Commando to address the web backend." << std::endl;
			std::cout << "\t\t<address>: config: address the web backend config parameters" << std::endl;
			std::cout << "\t\t\t<varags>: empty: return all config parameters" << std::endl;
			std::cout << "\t\t\t<varags>: silent_mode: return current value for silent_mode configuration" << std::endl;
			std::cout << "\t\t\t<varags>: silent_mode, yes/no: set current value for silent_mode configuration" << std::endl;
			// help comand is a special request which ignores the return value
		}
		else
		{
			resParse = jvx_parseCommandIntoToken(command, f_expr, args);
			if (resParse == JVX_NO_ERROR)
			{
				if (f_expr == "web")
				{
					if (args.size() > 0)
					{
						if ((args[0] == "config"))// || (args[0] == "no"))
						{
							if (args.size() == 1)
							{
								if (config.silent_mode)
								{
									jelm_result.makeAssignmentString("silent_mode", "yes");
								}
								else
								{
									jelm_result.makeAssignmentString("silent_mode", "no");
								}
								jlstresp.insertConsumeElementFront(jelm_result);

								jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resL].friendly);
								jlstresp.insertConsumeElementFront(jelm_result);
							}
							else
							{
								if (args[1] == "silent_mode")
								{
									if (args.size() > 2)
									{
										if (args[2] == "yes")
										{
											config.silent_mode = true;
											jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resL].friendly);
											jlstresp.insertConsumeElementFront(jelm_result);
										}
										else if (args[2] == "no")
										{
											config.silent_mode = false;
											jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resL].friendly);
											jlstresp.insertConsumeElementFront(jelm_result);
										}
										else
										{
											resL = JVX_ERROR_PARSE_ERROR;
											errTxt = "Specification of silent_mode expects <yes> or <no>. Instead, ";
											errTxt += args[2];
											errTxt += " was specified.";
											JVX_CREATE_ERROR_NO_RETURN(jlstresp, res, errTxt);
										}
									}
									else
									{
										if (config.silent_mode)
										{
											jelm_result.makeAssignmentString("silent_mode", "yes");
										}
										else
										{
											jelm_result.makeAssignmentString("silent_mode", "no");
										}
										jlstresp.insertConsumeElementFront(jelm_result);

										jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resL].friendly);
										jlstresp.insertConsumeElementFront(jelm_result);
									}
								}
								else
								{
									resL = JVX_ERROR_INVALID_ARGUMENT;
									errTxt = "Specification of config command <";
									errTxt += args[1];
									errTxt += "> is invalid.";
									JVX_CREATE_ERROR_NO_RETURN(jlstresp, res, errTxt);
									jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resL].friendly);
									jlstresp.insertConsumeElementFront(jelm_result);
								}
							}
						}
						else
						{
							resL = JVX_ERROR_INVALID_ARGUMENT;
							errTxt = "Specification of command <";
							errTxt += args[0];
							errTxt += "> is invalid.";
							JVX_CREATE_ERROR_NO_RETURN(jlstresp, res, errTxt);
							jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resL].friendly);
							jlstresp.insertConsumeElementFront(jelm_result);
						}
					}
					else
					{
						resL = JVX_ERROR_INVALID_ARGUMENT;
						errTxt = "No command specified in call.";
						JVX_CREATE_ERROR_NO_RETURN(jlstresp, res, errTxt);
						jelm_result.makeAssignmentString("return_code", jvxErrorType_str[resL].friendly);
						jlstresp.insertConsumeElementFront(jelm_result);
					}

					TjvxEventLoopElement qel;
					qel.message_id = message_id;
					qel.param = &jlstresp;
					qel.paramType = JVX_EVENTLOOP_DATAFORMAT_JVXJSONMULTFIELDS;
					qel.eventClass = event_class;
					qel.eventPriority = event_priority;
					qel.origin_fe = origin;

					origin->report_assign_output(&qel, res, privBlock);
					/*
					origin->report_assign_output(message_id, &jlstresp,
						JVX_EVENTLOOP_DATAFORMAT_JVXJSONMULTFIELDS,
						event_class, event_priority, res, origin,
						privBlock);
						*/
					return JVX_NO_ERROR;
				} // if (f_expr == "web")
				else
				{
					resParse = JVX_ERROR_INVALID_ARGUMENT;
				}
			} // if (resParse == JVX_NO_ERROR)
			return resParse; // Here, the token parser has failed, hence, this command was not for this backend 
		}
		break;

	case JVX_EVENT_LOOP_EVENT_WS_READY: // web socket connected

		ctxt = (jvxWebContext*)param;

		 // ========================================================
		 // Web socket connect
		 // ========================================================
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 1);
		if (ctxt->uri == JVX_URI_BINARY_SOCKET)
		{
			binWs.register_binary_socket_main_loop(ctxt);
		}
		else if (ctxt->uri == JVX_URI_TEXT_SOCKET)
		{
			txtWs.register_text_socket_main_loop(ctxt);
		}
		return JVX_NO_ERROR;
		break;

	case JVX_EVENT_LOOP_EVENT_WS_CLOSE: // web socket disconnected

		web_socket_disconnect(param);
		return JVX_NO_ERROR;
		break;

	case JVX_EVENT_LOOP_EVENT_WS_TIMEOUT: // web socket update timeout

		 // ========================================================
		 // Web socket thread timeout - only targeting binary socket!
		 // ========================================================

#if 0 
		std::cout << "Timer expired - delta start @ " << (jvxData)(JVX_GET_TICKCOUNT_US_GET(&tStamp) - tStop)* 0.001 << "--" << webSocketPeriodic.wsMessId << std::endl;
#endif

		/*
		Do what is to be done in web socket callback
		*/

		binWs.step_update_properties_websocket(JVX_PROP_STREAM_UPDATE_TIMEOUT, &hasDisconnected);

		// Reschedule timer event

		/*
		std::cout << "Reschedule timer event, timeout [msec] = " << webSocketPeriodic.timeout_msec << std::endl;

		I needed to modify this: if a disconnect occurred due to connection timeout, we need to 
		simly NOT reschedule the timeout. Otherwhise the latest message must be canceled from queue
		*/
		if (!hasDisconnected)
		{
			evLElm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
			evLElm.priv_fe = NULL;
			evLElm.target_be = static_cast<IjvxEventLoop_backend*>(this);
			evLElm.priv_be = NULL;

			evLElm.param = NULL;
			evLElm.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;

			evLElm.eventType = JVX_EVENT_LOOP_EVENT_WS_TIMEOUT;
			evLElm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
			evLElm.eventPriority = JVX_EVENTLOOP_PRIORITY_TIMER;
			evLElm.delta_t = binWs.webSocketPeriodic.timeout_msec;
			evLElm.autoDeleteOnProcess = c_false;

			res = this->evLop->event_schedule(&evLElm, NULL, NULL);

			binWs.webSocketPeriodic.wsMessId = evLElm.message_id;

			switch (res)
			{
			case JVX_ERROR_END_OF_FILE:
				std::cout << "Disconnected websocket not reported since host shutdown deadline was missed!" << std::endl;
				break;
			case JVX_ERROR_ABORT:
				std::cout << "Disconnected websocket not reported since host does not accept events from this frontend curently!" << std::endl;
				break;
			case JVX_NO_ERROR:
				break;
			default:
				assert(0);
			}
		}
#if 0 
		tStop = JVX_GET_TICKCOUNT_US_GET(&tStamp);
		//std::cout << "Timer expired - stop @ " << (jvxData)tStop * 0.001 << std::endl;
#endif
		return JVX_NO_ERROR;
		break;

	case JVX_EVENT_LOOP_EVENT_WS_TIMEOUT_RECONF: // web socket timeout reconfigure

		// Is only addressed by binary websocket
		binWs.timeout_reconfigure(param, paramType);

		return JVX_NO_ERROR;
		break;
	case JVX_EVENT_LOOP_EVENT_WS_ADD_PROP_LIST:

		// Is only addressed by binary websocket
		binWs.add_property_observer_list(param, paramType);

		return JVX_NO_ERROR; 
		break;

	case JVX_EVENT_LOOP_EVENT_WS_REM_PROP_LIST:

		// Is only addressed by binary websocket
		binWs.rem_property_observer_list(param, paramType);

		return JVX_NO_ERROR;
		break;

	case JVX_EVENT_LOOP_EVENT_WS_FLOW_EVENT:

		// Is only addressed by binary websocket
		binWs.process_flow_event(param, paramType);

		return JVX_NO_ERROR;
		break;
	case JVX_EVENT_LOOP_EVENT_WS_COMM_ERROR:
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_STDSTRING);
		errtxt = (std::string*)param;
		std::cout << "Communication error reported, err description: <" << *errtxt << ">." << std::endl;
		assert(0); // Better handling in future here to drop the web socket and do a clean recovery
		break;

	case JVX_EVENT_LOOP_EVENT_WS_TEXT_INPUT:
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_SPEC_JVXONEWSTEXTREQUEST);
		refWsText = (jvxOneWsTextRequest*)param;
		txtWs.process_incoming_text_message(refWsText);
		break;
	default:
		break;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

// ========================================================================================
// ===============================================================================================




// =======================================================================================

jvxErrorType
CjvxWebControl_fe::report_simple_text_message(const char* txt, jvxReportPriority prio)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxWebControl_fe::report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
	jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize offs, jvxSize num, const jvxComponentIdentification& tpTo,
	jvxPropertyCallPurpose callpurposeC)
{
	jvxBool hasDisconnected = false;
	binWs.step_update_properties_websocket(JVX_PROP_STREAM_UPDATE_ON_CHANGE, &hasDisconnected, thisismytype, cat, propId);

	if (tpTo == JVX_COMPONENT_UNKNOWN)
	{
		// This always comes from within main thread
		if (binWs.webSocketPeriodic.theCtxt.context_conn)
		{
			jvxPropertyChangedHeader theHeader;
			jvxErrorType resL = JVX_NO_ERROR;

			theHeader.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
			theHeader.loc_header.purpose = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER | JVX_PS_SEND_INFORM_PROPERTY_CHANGE;
			theHeader.loc_header.paketsize = JVX_SIZE_INT32(sizeof(jvxPropertyChangedHeader));

			theHeader.component_slot = JVX_SIZE_UINT16(thisismytype.slotid);
			theHeader.component_subslot = JVX_SIZE_UINT16(thisismytype.slotsubid);
			theHeader.component_type = JVX_SIZE_UINT16(thisismytype.tp);
			theHeader.cat = JVX_SIZE_UINT16(cat);
			theHeader.prop_id = JVX_SIZE_UINT32(propId);

			resL = myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)binWs.webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
				(const char*)&theHeader, theHeader.loc_header.paketsize);
		}
	}

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControl_fe::report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
	jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
	jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose )
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxWebControl_fe::report_command_request(
	jvxCBitField request, 
	jvxHandle* caseSpecificData, 
	jvxSize szSpecificData)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxWebControl_fe::request_command(const CjvxReportCommandRequest& request)
{	
	std::cout << __FUNCTION__ << "Warning: Unexpected direct call to function that need to be synchronized!" << std::endl;
	jvx::helper::debug_out_command_request(request, std::cout, (std::string)"-- <" + __FUNCTION__ + ">");
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxWebControl_fe::interface_sub_report(IjvxSubReport** subReport) 
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxWebControl_fe::report_os_specific(jvxSize commandId, void* context) 
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxWebControl_fe::get_configuration_ext(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxValue val;
	jvxConfigData* theDat = NULL;
	if (processor)
	{
		if (config.silent_mode)
		{
			val.assign((jvxInt16)c_true);
		}
		else
		{
			val.assign((jvxInt16)c_false);
		}

		processor->createAssignmentValue(&theDat, JVX_WEBCONTROL_SILENT_MODE, val);
		if (theDat)
		{
			processor->addSubsectionToSection(sectionWhereToAddAllSubsections, theDat);
		}
	}
	
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxWebControl_fe::put_configuration_ext(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename, jvxInt32 lineno)
{
	jvxConfigData* theDat = NULL;
	jvxValue val;
	if (processor)
	{
		processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &theDat, JVX_WEBCONTROL_SILENT_MODE);
		if (theDat)
		{
			processor->getAssignmentValue(theDat, &val);
			val.toContent(&config.silent_mode);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControl_fe::report_register_fe_commandline(IjvxCommandLine* comLine)
{
	if (comLine)
	{
		comLine->register_option("--web-document-root", "", "File entry for web files to deploy", "./www", true, JVX_DATAFORMAT_STRING);
		comLine->register_option("--web-num-threads", "", "Number of threads to run for web traffic.", "8", true, JVX_DATAFORMAT_SIZE);
		comLine->register_option("--web-listening_ports", "", "Port to have the web server listen to.", "8000", true, JVX_DATAFORMAT_SIZE);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControl_fe::report_readout_fe_commandline(IjvxCommandLine* comLine)
{
	jvxApiString tmpStr;
	jvxSize num = 0;
	jvxErrorType res;
	if (comLine)
	{
		std::string txtTok = "--web-num-threads";
		res = comLine->content_entry_option(txtTok.c_str(), 0, &config.numThreads, JVX_DATAFORMAT_SIZE);
		if (res != JVX_NO_ERROR)
		{
			std::cout << "Failed to read specification for command line option <" << txtTok << ">, reason: " << jvxErrorType_txt(res) << "." << std::endl;
		}

		res = comLine->content_entry_option("--web-listening_ports", 0, &config.listeningPort, JVX_DATAFORMAT_SIZE);
		if (res != JVX_NO_ERROR)
		{
			std::cout << "Failed to read specification for command line option <" << txtTok << ">, reason: " << jvxErrorType_txt(res) << "." << std::endl;
		}

		res = comLine->content_entry_option("--web-document-root", 0, &tmpStr, JVX_DATAFORMAT_STRING);
		if (res != JVX_NO_ERROR)
		{
			std::cout << "Failed to read specification for command line option <" << txtTok << ">, reason: " << jvxErrorType_txt(res) << "." << std::endl;
		}

		config.docRoot = tmpStr.std_str();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControl_fe::report_register_be_commandline(IjvxCommandLine* comLine)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControl_fe::report_readout_be_commandline(IjvxCommandLine* comLine)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControl_fe::report_want_to_shutdown_ext(jvxBool restart)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxWebControl_fe::query_property(jvxFrontendSupportQueryType tp, jvxHandle* load)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxWebControl_fe::trigger_sync(jvxFrontendTriggerType tp, jvxHandle* load, jvxBool blockedRun)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxWebControl_fe::request_if_command_forward(IjvxReportSystemForward** fwdCalls)
{
	if (fwdCalls)
	{
		*fwdCalls = this;
	}
	return JVX_NO_ERROR;
}

void 
CjvxWebControl_fe::request_command_in_main_thread(CjvxReportCommandRequest* request, jvxBool removeObject)
{
	if (request)
	{
		jvxErrorType res = txtWs.fwd_command_request_main_loop(*request);
		if (res != JVX_NO_ERROR)
		{
			jvx::helper::debug_out_command_request(*request, std::cout, (std::string)"-- <" + __FUNCTION__ + ">");
		}
	}
}

jvxErrorType
CjvxWebControl_fe::report_special_event(TjvxEventLoopElement* theQueueElement, jvxHandle* priv)
{
	assert(theQueueElement->paramType == JVX_EVENTLOOP_DATAFORMAT_SPECIAL_EVENT);
	assert(theQueueElement->param);
	jvxProtocolHeader theHeader;
	jvxSequencerEventHeader theSeqHeader;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSpecialEventType_seq* seqEv = NULL;
	jvxSpecialEventType* theEvent = (jvxSpecialEventType*)theQueueElement->param;
	seqEv = (jvxSpecialEventType_seq*)theEvent->dat;

	switch (theEvent->ev)
	{
	case JVX_EVENTLOOP_SPECIAL_EVENT_SYSTEM_UPDATE:
		theHeader.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
		theHeader.purpose = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER | JVX_PS_SEND_INFORM_SYSTEM_CHANGE;
		theHeader.paketsize = JVX_SIZE_INT32(sizeof(jvxProtocolHeader));

		// This always comes from within main thread
		if (binWs.webSocketPeriodic.theCtxt.context_conn)
		{
			resL = myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)binWs.webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
				(const char*)&theHeader, theHeader.paketsize);
		}
		break;
	case JVX_EVENTLOOP_SPECIAL_EVENT_SEQUENCER_UPDATE:
		memset(&theSeqHeader, 0, sizeof(theSeqHeader));
		theSeqHeader.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
		theSeqHeader.loc_header.purpose = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER | JVX_PS_SEND_INFORM_SEQUENCER_EVENT;
		theSeqHeader.loc_header.paketsize = JVX_SIZE_INT32(sizeof(jvxSequencerEventHeader));

		jvx_fillCharStr(theSeqHeader.seq_step_description, JVX_SEQUENCER_EVENT_DESRIPTION_STR_LENGTH, (std::string)seqEv->description);
		theSeqHeader.seq_event_mask = (jvxUInt64)seqEv->event_mask;
		theSeqHeader.seq_id = (jvxUInt16)seqEv->sequenceId;
		theSeqHeader.seq_step_id = (jvxUInt16)seqEv->stepId;
		theSeqHeader.seq_elm_tp = (jvxUInt16)seqEv->etp;
		theSeqHeader.seq_queue_tp = (jvxUInt16)seqEv->qtp;
		theSeqHeader.seq_step_fid = (jvxUInt16)seqEv->fId;
		theSeqHeader.seq_oper_state = (jvxUInt16)seqEv->oState;

		/*
			TODO
		 */
		// This always comes from within main thread
		if (binWs.webSocketPeriodic.theCtxt.context_conn)
		{
			resL = myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)binWs.webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
				(const char*)&theSeqHeader, theSeqHeader.loc_header.paketsize);
		}
		break;
	default:
		std::cout << "Unknown Special Event Report." << std::endl;
	}
	return JVX_NO_ERROR;
}

void
CjvxWebControl_fe::web_socket_disconnect(jvxHandle* hdl)
{
	jvxErrorType res = txtWs.unregister_text_socket_main_loop(hdl);
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		res = binWs.unregister_binary_socket_main_loop(hdl);
	}

	if (res != JVX_NO_ERROR)
	{
		std::cout << __FUNCTION__ << ": Error: Invalid reference in websocket disconnect!" << std::endl;
	}
}