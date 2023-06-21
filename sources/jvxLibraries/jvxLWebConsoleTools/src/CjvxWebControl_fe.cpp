#include "CjvxWebControl_fe.h"
#include "jvxTWebServer.h"
#include "CjvxJson.h"
#include "interfaces/console/IjvxEventLoop_backend_ctrl.h"

//#define JVX_FE_CONSOLE_VERBOSE
#define JVX_TIMOUT_BLOCKING_CALL_MSEC 50000
#define JVX_RTST_BUFFER_SIZE 128
#define JVX_WEBCONTROL_SILENT_MODE "JVX_WEBCONTROL_CONSOLE_SILENT_MODE"
#define JVX_WEBCONTROL_SECTION "JVX_WEBCONTROL_CONSOLE_SECTION"

#define JVX_CREATE_ERROR_NO_RETURN(lstelmr, err, txt) \
	{ \
		CjvxJsonElement jelm; \
		jelm.makeAssignmentString("error_description", txt); \
		lstelmr.addConsumeElement(jelm); \
	} \

CjvxWebControl_fe::CjvxWebControl_fe(): JVX_INIT_RT_ST_INSTANCES
{
	  timerCount = 0;
	JVX_INITIALIZE_MUTEX(safeAccessMemList);

	evLop = NULL;
	theState = JVX_STATE_INIT;
	myHostRef = NULL;
	myWebServer.hdl = NULL;
	myWebServer.obj = NULL;
	myWebServer.closeProcedureState = 0;

	webSocketPeriodic.theCtxt.context_conn = NULL;
	webSocketPeriodic.theCtxt.context_server = NULL;
	webSocketPeriodic.timeout_msec = 100;
	webSocketPeriodic.wsMessId = JVX_SIZE_UNSELECTED;
	JVX_GET_TICKCOUNT_US_SETREF(&tStamp);

	wsUniqueId = 1;
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
		resL = start_webserver(myWebServer.hdl, myHostRef, static_cast<IjvxWebServerHost_hooks*>(this));
		assert(resL == JVX_NO_ERROR);

		// Request log file stream
		jvx_init_text_log(jvxrtst_bkp);
		jvxrtst_bkp.theModuleName = "CjvxWebControl_fe";
		jvxrtst_bkp.theToolsHost = myToolsHost;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(jvxrtst_bkp.jvxlst_buf, char, JVX_RTST_BUFFER_SIZE);
		jvxrtst_bkp.jvxlst_buf_sz = JVX_RTST_BUFFER_SIZE;
		jvx_request_text_log(jvxrtst_bkp);

		jvx_lock_text_log(jvxrtst_bkp);
		jvxrtst << "Started Host." << std::endl;
		jvx_unlock_text_log(jvxrtst_bkp);

		res = myHostRef->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle**>(&config.hostRefConfigExtender));
		if ((res == JVX_NO_ERROR) && config.hostRefConfigExtender)
		{
			res = config.hostRefConfigExtender->register_extension(static_cast<IjvxConfigurationExtender_report*>(this), JVX_WEBCONTROL_SECTION);
		}

		break;
	case JVX_EVENTLOOP_EVENT_DEACTIVATE:
		
		if (config.hostRefConfigExtender)
		{
			res = config.hostRefConfigExtender->unregister_extension(JVX_WEBCONTROL_SECTION);
		}
		res = myHostRef->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle*>(config.hostRefConfigExtender));
		config.hostRefConfigExtender = NULL;

		jvx_return_text_log(jvxrtst_bkp);
		JVX_DSP_SAFE_DELETE_FIELD(jvxrtst_bkp.jvxlst_buf);
		jvx_terminate_text_log(jvxrtst_bkp);

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

	case JVX_EVENTLOOP_EVENT_SPECIFIC + 1:

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
	case JVX_EVENTLOOP_EVENT_SPECIFIC + 1:

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
		NULL, 0, command, requiresInterpretation, errorDetected, &jvxrtst, 
		static_cast<jvx_lock*>(&jvxrtst_bkp.jvxos), (config.silent_mode!= c_false));
	if (requiresInterpretation)
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
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "Web socket connected" << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}
	}
	else if (jvx_bitTest(purp, JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_SHIFT))
	{
		TjvxEventLoopElement elm;
		jvxWebContext* ctxt = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(ctxt, jvxWebContext);
		ctxt->context_conn = context_conn;
		ctxt->context_server = context_server;

		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "Web socket ready" << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}

		elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
		elm.priv_fe = NULL;
		elm.target_be = static_cast<IjvxEventLoop_backend*>(this);
		elm.priv_be = NULL;

		elm.param = ctxt;
		elm.paramType = JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 1;

		elm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 1;
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
		jvxUInt16* ptr16 = NULL;
		jvxProtocolHeader* protheader = NULL;
		jvxByte opcode = headerbyte & 0xF;
		switch(opcode)
		{
		case 0x2: // Binary message
		
			protheader = (jvxProtocolHeader*)payload;
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

					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvx_lock_text_log(jvxrtst_bkp);
						jvxrtst << "Incoming request web socket configuration:" << std::endl;
						jvxrtst << "Ticktime [msec] = " << pconfheader->tick_msec << std::endl;
						jvxrtst << "Ping count = " << pconfheader->ping_count << std::endl;
						jvxrtst << "User [int32] = " << pconfheader->user_specific << std::endl;
						jvx_unlock_text_log(jvxrtst_bkp);
					}
					
					// Schedule removal of timeout thread followed by new timeout thread
					elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
					elm.priv_fe = NULL;
					elm.target_be = static_cast<IjvxEventLoop_backend*>(this);
					elm.priv_be = NULL;

					elm.param = (jvxHandle*)pconfheader;
					elm.paramType = JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 3;

					elm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 4;
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

					elm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 5;
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

					elm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 6;
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

					elm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 7;
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
			break;
		case 0x8:
			ptr16 = (jvxUInt16*)payload;
			
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "Web Socket Close Event received, load = " << *ptr16 << std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}
			break;
		case 0x9:
			// Ping
			assert(0);
			break;
		case 0xA:
			// Pong
			this->webSocketPeriodic.current_ping_count = 0;
			break;
		default:
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "Unknown Web Socket Data received received" << std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);
			}
			break;
		}
	}
	else if (jvx_bitTest(purp, JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_SHIFT))
	{
		TjvxEventLoopElement elm;

		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "Web socket closed" << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}

		elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
		elm.priv_fe = NULL;
		elm.target_be = static_cast<IjvxEventLoop_backend*>(this);
		elm.priv_be = NULL;

		elm.param = NULL;
		elm.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;

		elm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 2;
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
			auto elm = lstUpdateProperties.begin();
			for (; elm != lstUpdateProperties.end(); elm++)
			{
				deactivate_property(elm->second);

				if (elm->second.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
				{
					disconnect_udp_port(elm->second);
				}
			}
			lstUpdateProperties.clear();
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
		assert(myPrivateMem);

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
// JVX_EVENTLOOP_EVENT_SPECIFIC + 1 -> Web socket connect
// JVX_EVENTLOOP_EVENT_SPECIFIC + 2 -> Web socket disconnect
// JVX_EVENTLOOP_EVENT_SPECIFIC + 3 -> Web socket thread timeout
// JVX_EVENTLOOP_EVENT_SPECIFIC + 4 -> Timeout reconfigure message
// JVX_EVENTLOOP_EVENT_SPECIFIC + 5 -> Add a new property to list of push properties
// JVX_EVENTLOOP_EVENT_SPECIFIC + 6 -> Remove a property from list of push properties
// JVX_EVENTLOOP_EVENT_SPECIFIC + 7 -> Flow control indicator from transferred property

// Types passed:
// JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 1 -> jvxWebContext*
// JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 3 -> jvxPropertyConfigurePropertySend*
// JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 2 -> jvxPropertyPropertyObserveHeader*
// JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 4 -> jvxPropertyPropertyObserveHeader_response*

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
	onePropertyWebSocketDefinition newProp;
	jvxPropertyPropertyObserveHeader_response theRespA;
	jvxPropertyConfigurePropertySend_response theRespC;
	jvxPropertyPropertyObserveHeader* paddheader = NULL;
	jvxPropertyPropertyObserveHeader* premheader = NULL;
	jvxPropertyPropertyObserveHeader_response* pheader_resp = NULL;
	jvxPropertyConfigurePropertySend* passed = NULL;
	jvxRawStreamHeader_response*raw_pheader_resp = NULL;
	std::string errTxt;
	CjvxJsonElementList jlstresp;
	std::string command;
	std::string f_expr;
	std::vector<std::string> args;
	jvxErrorType resParse;
	CjvxJsonElement jelm_result;
	TjvxEventLoopElement evLElm;
	std::string* errtxt = NULL;
	jvxBool hasDisconnected = false;

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

	case (JVX_EVENTLOOP_EVENT_SPECIFIC + 1): // web socket connected

		 // ========================================================
		 // Web socket connect
		 // ========================================================
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 1);
		if (webSocketPeriodic.theCtxt.context_conn == NULL)
		{
			assert(param);
			webSocketPeriodic.theCtxt = *((jvxWebContext*)param);

			// Get the originating IP address 
			request_event_information(webSocketPeriodic.theCtxt.context_server, webSocketPeriodic.theCtxt.context_conn,
				&uri, &query, &local_uri, &url_origin, &user);
			webSocketPeriodic.uri = uri.std_str();
			webSocketPeriodic.query = query.std_str();
			webSocketPeriodic.local_uri = local_uri.std_str();
			webSocketPeriodic.url_origin = url_origin.std_str();
			webSocketPeriodic.user = user.std_str();
			if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
			{
				jvx_lock_text_log(jvxrtst_bkp);
				jvxrtst << "Web socket connection from " << webSocketPeriodic.url_origin << ":" << webSocketPeriodic.user << std::endl;
				jvx_unlock_text_log(jvxrtst_bkp);

			}
		}
		else
		{
			std::cout << "Failed to activate web socket connection: web socket already in use!" << std::endl;
			std::cout << "Sending immediate terminate signal!" << std::endl;
			myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, 0x8, NULL, 0);
		}
		return JVX_NO_ERROR;
		break;

	case (JVX_EVENTLOOP_EVENT_SPECIFIC + 2): // web socket disconnected

		web_socket_disconnect();

		return JVX_NO_ERROR;
		break;

	case (JVX_EVENTLOOP_EVENT_SPECIFIC + 3): // web socket update timeout

		 // ========================================================
		 // Web socket thread timeout
		 // ========================================================

#if 0 
		std::cout << "Timer expired - delta start @ " << (jvxData)(JVX_GET_TICKCOUNT_US_GET(&tStamp) - tStop)* 0.001 << "--" << webSocketPeriodic.wsMessId << std::endl;
#endif

		/*
		Do what is to be done in web socket callback
		*/

		step_update_properties_websocket(JVX_PROP_STREAM_UPDATE_TIMEOUT, &hasDisconnected);

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

			evLElm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 3;
			evLElm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
			evLElm.eventPriority = JVX_EVENTLOOP_PRIORITY_TIMER;
			evLElm.delta_t = webSocketPeriodic.timeout_msec;
			evLElm.autoDeleteOnProcess = c_false;

			res = this->evLop->event_schedule(&evLElm, NULL, NULL);

			webSocketPeriodic.wsMessId = evLElm.message_id;

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

	case (JVX_EVENTLOOP_EVENT_SPECIFIC + 4): // web socket timeout reconfigure

		// ========================================================
		// Timeout reconfigure message
		// ========================================================

		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 3);
		passed = (jvxPropertyConfigurePropertySend*)param;
		webSocketPeriodic.timeout_msec = passed->tick_msec;
		webSocketPeriodic.ping_cnt_trigger = passed->ping_count;
		webSocketPeriodic.ping_cnt_close = webSocketPeriodic.ping_cnt_trigger * 2;
		webSocketPeriodic.ping_cnt_close_failed = webSocketPeriodic.ping_cnt_trigger * 3;
		webSocketPeriodic.current_ping_count = 0;

		// Restart timer with a different timeout
		if (JVX_CHECK_SIZE_SELECTED(webSocketPeriodic.wsMessId))
		{
			res = this->evLop->event_clear(webSocketPeriodic.wsMessId, NULL, NULL);
			webSocketPeriodic.wsMessId = JVX_SIZE_UNSELECTED;
		}

		// Reschedule timer event
		evLElm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
		evLElm.priv_fe = NULL;
		evLElm.target_be = static_cast<IjvxEventLoop_backend*>(this);
		evLElm.priv_be = NULL;

		evLElm.param = NULL;
		evLElm.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;

		evLElm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 3;
		evLElm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
		evLElm.eventPriority = JVX_EVENTLOOP_PRIORITY_TIMER;
		evLElm.delta_t = webSocketPeriodic.timeout_msec;
		evLElm.autoDeleteOnProcess = c_false;

		res = this->evLop->event_schedule(&evLElm, NULL, NULL);

		webSocketPeriodic.wsMessId = evLElm.message_id;

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

		// Positive response
		theRespC.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
		theRespC.loc_header.purpose = JVX_PS_CONFIGURE_PROPERTY_OBSERVATION | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
		theRespC.loc_header.paketsize = sizeof(jvxPropertyConfigurePropertySend_response);
		theRespC.errcode = JVX_NO_ERROR;
		theRespC.user_specific = passed->user_specific;
		myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
			(const char*)&theRespC, theRespC.loc_header.paketsize);

		return JVX_NO_ERROR;
		break;
	case (JVX_EVENTLOOP_EVENT_SPECIFIC + 5):

		// ========================================================
		// Add a new property to list of push properties
		// ========================================================

		/*
			jvxExternalBuffer* jvx_allocate1DCircExternalBuffer(jvxSize bLength, 
			jvxSize numChannels, jvxDataFormat form, jvxDataFormatGroup subform,
			lock_buffer lockf, try_lock_buffer try_lockf, unlock_buffer unlockf,
			jvxSize* szFld)
		*/

		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 2);
		paddheader = (jvxPropertyPropertyObserveHeader*)param;

		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3) 
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "Incoming request web socket property add:" << std::endl;
			jvxrtst << "Component identification = " << jvxComponentIdentification_txt(jvxComponentIdentification((jvxComponentType)paddheader->component_type, paddheader->component_slot, paddheader->component_subslot)) << std::endl;
			jvxrtst << "Update Condition = " << jvxPropertyStreamCondUpdate_txt(paddheader->cond_update) << std::endl;
			jvxrtst << "Update Condition Param = " << paddheader->param_cond_update << std::endl;
			jvxrtst << "Property format = " << jvxDataFormat_txt(paddheader->property_format) << std::endl;
			jvxrtst << "Property number elements = " << paddheader->property_num_elements << std::endl;
			jvxrtst << "Property offset = " << paddheader->property_offset << std::endl;
			jvxrtst << "Property state active = " << jvxState_dec(paddheader->state_active) << std::endl;
			jvxrtst << "Property decoder hint type = " << jvxPropertyDecoderHintType_txt(paddheader->property_dec_hint_tp) << std::endl;
			jvxrtst << "Param0 = " << paddheader->param0 << std::endl;
			jvxrtst << "Param1 = " << paddheader->param1 << std::endl;
			jvxrtst << "User [int32] = " << paddheader->user_specific << std::endl;
			jvxrtst << "Priority [uint16] = " << paddheader->priority << std::endl;
			jvxrtst << "Port [uint32] = " << paddheader->port << std::endl;
			jvxrtst << "Number emit min [uint32] = " << paddheader->num_emit_min << std::endl;
			jvxrtst << "Number emit max [uint32] = " << paddheader->num_emit_max << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}

		assert(paddheader->component_type < JVX_COMPONENT_ALL_LIMIT);
		newProp.cpId = jvxComponentIdentification((jvxComponentType)paddheader->component_type, paddheader->component_slot, paddheader->component_subslot);
		newProp.format_spec = (jvxDataFormat)paddheader->property_format;
		newProp.htTp = (jvxPropertyDecoderHintType)paddheader->property_dec_hint_tp;
		newProp.numElms_spec = JVX_UINT32_SIZE(paddheader->property_num_elements);
		newProp.offset = JVX_UINT16_SIZE(paddheader->property_offset);
		newProp.param0 = paddheader->param0;
		newProp.param1 = paddheader->param1;
		newProp.state_active = (jvxState)paddheader->state_active;
		newProp.cond_update = (jvxPropertyStreamCondUpdate)paddheader->cond_update;
		newProp.param_cond_update = paddheader->param_cond_update;
		//newProp.streamPropertyInTransferStateMax = paddheader->cnt_report_disconnect;
		newProp.requiresFlowControl = (paddheader->requires_flow_control != 0);
		newProp.prio = (jvxPropertyTransferPriority)paddheader->priority;
		newProp.high_prio.port = paddheader->port;
		newProp.num_emit_min = JVX_UINT32_SIZE(paddheader->num_emit_min);
		newProp.num_emit_max = JVX_UINT32_SIZE(paddheader->num_emit_max);
		newProp.propertyName = std::string((char*)paddheader + sizeof(jvxPropertyPropertyObserveHeader), (paddheader->loc_header.paketsize - sizeof(jvxPropertyPropertyObserveHeader)));
		newProp.uniqueId = wsUniqueId++;

		newProp.tStamp = 0;
		newProp.state_transfer = JVX_PROPERTY_WEBSOCKET_STATE_NONE;
		newProp.streamPropertyInTransferState = 0;
		newProp.runtime.allocatedRawBuffer = NULL;
		jvx_initPropertyDescription(newProp.runtime.descr);
		newProp.runtime.referencedPayloadBuffer = NULL;
		newProp.runtime.szRawBuffer = 0;
		jvx_initPropertyReferenceTriple(&newProp.runtime.theTriple);
		newProp.runtime.transferState = JVX_STATE_NONE;
		newProp.runtime.cntTicks = 0;
		newProp.runtime.serious_fail = false;
		newProp.runtime.last_send_msec = 0;

		if (newProp.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
		{
			jvxApiString astr;
			myWebServer.hdl->in_connect_get_ws_ip_addr((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, &astr);
			newProp.high_prio.target = astr.std_str();

			// Create a UDP socket and open!
			auto elm = high_prio_transfer.theUdpSockets.find(newProp.high_prio.port);
			if (elm == high_prio_transfer.theUdpSockets.end())
			{
				// Allocate new socket
				HjvxPropertyStreamUdpSocket* theNewUdpSocket = NULL;
				JVX_DSP_SAFE_ALLOCATE_OBJECT(theNewUdpSocket, HjvxPropertyStreamUdpSocket);
				theNewUdpSocket->initialize(
					static_cast<HjvxPropertyStreamUdpSocket_report*>(this),
					myToolsHost, myHostRef);
				theNewUdpSocket->register_property(newProp.uniqueId, newProp.high_prio.port, newProp.high_prio.target);
				high_prio_transfer.theUdpSockets[newProp.high_prio.port] = theNewUdpSocket;
				newProp.runtime.prio_high.udpSocket = theNewUdpSocket;
			}
			else
			{
				(elm->second)->register_property(newProp.uniqueId, newProp.high_prio.port, astr.std_str());
				newProp.runtime.prio_high.udpSocket = elm->second;
			}
		}

		lstUpdateProperties[newProp.uniqueId] = newProp;
		
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "Registered property via web socket <" << newProp.uniqueId << ">: " << jvxComponentIdentification_txt(newProp.cpId) << "--" << newProp.propertyName << "||" <<
				jvxDataFormat_txt(newProp.format_spec) << "--" << jvxPropertyDecoderHintType_txt(newProp.htTp) << "--" << newProp.offset << newProp.numElms_spec << std::flush;
			switch(newProp.prio)
			{
			case JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO:
				jvxrtst << "--" << "prio_normal." << std::flush;
				break;
			case JVX_PROP_CONNECTION_TYPE_HIGH_PRIO:
				jvxrtst << "--" << "prio_high, udp target: <" << newProp.high_prio.target << ":" << newProp.high_prio.port << ">." << std::flush;
				break;
			}
			jvxrtst << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);

		}

		// Send response
		theRespA.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
		theRespA.loc_header.purpose = JVX_PS_ADD_PROPERTY_TO_OBSERVE | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
		theRespA.loc_header.paketsize = sizeof(jvxPropertyPropertyObserveHeader_response);
		theRespA.errcode = JVX_NO_ERROR;
		theRespA.component_type = paddheader->component_type;
		theRespA.component_slot = paddheader->component_slot;
		theRespA.component_subslot = paddheader->component_subslot;
		theRespA.stream_id = JVX_SIZE_INT16(newProp.uniqueId);
		theRespA.user_specific = paddheader->user_specific;

		myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
			(const char*)&theRespA, theRespA.loc_header.paketsize);

		return JVX_NO_ERROR; 
		break;

	case (JVX_EVENTLOOP_EVENT_SPECIFIC + 6):

		// ========================================================
		// Remove a property from list of push properties
		// ========================================================

		/*
		jvxExternalBuffer* jvx_allocate1DCircExternalBuffer(jvxSize bLength,
		jvxSize numChannels, jvxDataFormat form, jvxDataFormatGroup subform,
		lock_buffer lockf, try_lock_buffer try_lockf, unlock_buffer unlockf,
		jvxSize* szFld)
		*/

		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 2);
		premheader = (jvxPropertyPropertyObserveHeader*)param;

		while(1)
		{
			std::map<jvxSize, onePropertyWebSocketDefinition>::iterator elm = lstUpdateProperties.begin();
			for (; elm != lstUpdateProperties.end(); elm++)
			{
				if (
					(elm->second.cpId.tp == premheader->component_type) &&
					(elm->second.cpId.slotid == premheader->component_slot) &&
					(elm->second.cpId.slotsubid == premheader->component_subslot))
				{
					if (elm->second.runtime.transferState == JVX_STATE_ACTIVE)
					{
						deactivate_property(elm->second);
					}
					break;
				}
			}
			if (elm == lstUpdateProperties.end())
			{
				// No more found
				break;
			}
			else
			{
				if (elm->second.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
				{
					disconnect_udp_port(elm->second);
				}
				lstUpdateProperties.erase(elm);
			}
		}

		theRespA.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
		theRespA.loc_header.purpose = JVX_PS_REMOVE_PROPERTY_TO_OBSERVE | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
		theRespA.loc_header.paketsize = sizeof(jvxPropertyPropertyObserveHeader_response);
		theRespA.errcode = JVX_NO_ERROR;
		theRespA.component_type = premheader->component_type;
		theRespA.component_slot = premheader->component_slot;
		theRespA.component_subslot = premheader->component_subslot;

		theRespA.stream_id = JVX_SIZE_INT16(0);
		theRespA.user_specific = premheader->user_specific;

		myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
			(const char*)&theRespA, theRespA.loc_header.paketsize);
		break;

	case (JVX_EVENTLOOP_EVENT_SPECIFIC + 7):

		// ========================================================
		// Response from data consumer to indicate flow control
		// ========================================================

		/*
		jvxExternalBuffer* jvx_allocate1DCircExternalBuffer(jvxSize bLength,
		jvxSize numChannels, jvxDataFormat form, jvxDataFormatGroup subform,
		lock_buffer lockf, try_lock_buffer try_lockf, unlock_buffer unlockf,
		jvxSize* szFld)
		*/

		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 4);
		raw_pheader_resp = (jvxRawStreamHeader_response*)param;
		{
			// No protection required here since only reading and within main thread
			auto elm = lstUpdateProperties.find(raw_pheader_resp->raw_header.stream_id);
			if (elm != lstUpdateProperties.end())
			{
				if (elm->second.requiresFlowControl)
				{
					if (elm->second.state_transfer == JVX_PROPERTY_WEBSOCKET_STATE_IN_TRANSFER)
					{
#ifdef JVX_VERBOSE_FLOW_CONTROL
						std::cout << "R[" << raw_pheader_resp->raw_header.stream_id << "--" << raw_pheader_resp->raw_header.packet_tstamp << "]" << std::endl;
#endif
						elm->second.state_transfer = JVX_PROPERTY_WEBSOCKET_STATE_NONE;
					}
				}
			}
		}
		break;
	case JVX_EVENTLOOP_EVENT_SPECIFIC + 8:
		assert(paramType == JVX_EVENTLOOP_DATAFORMAT_STDSTRING);
		errtxt = (std::string*)param;
		std::cout << "Communication error reported, err description: <" << *errtxt << ">." << std::endl;
		assert(0); // Better handling in future here to drop the web socket and do a clean recovery
		break;

	default:
		break;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

// ========================================================================================

void
CjvxWebControl_fe::web_socket_disconnect()
{
	// ========================================================
	// Web socket disconnect
	// ========================================================
	std::cout << "Web socket disconnect" << std::endl;
	if (webSocketPeriodic.theCtxt.context_conn)
	{
		webSocketPeriodic.theCtxt.context_conn = NULL;
		webSocketPeriodic.theCtxt.context_server = NULL;

		webSocketPeriodic.uri = "";
		webSocketPeriodic.query = "";
		webSocketPeriodic.local_uri = "";
		webSocketPeriodic.url_origin = "";
		webSocketPeriodic.user = "";

		// Cancel timout message
		jvxErrorType res = this->evLop->event_clear(webSocketPeriodic.wsMessId, NULL, NULL);
		assert(res == JVX_NO_ERROR);
		webSocketPeriodic.wsMessId = JVX_SIZE_UNSELECTED;


		auto elm = lstUpdateProperties.begin();
		for (; elm != lstUpdateProperties.end(); elm++)
		{
			deactivate_property(elm->second);

			if (elm->second.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
			{
				disconnect_udp_port(elm->second);
			}
		}
		lstUpdateProperties.clear();
		webSocketPeriodic.ping_cnt_close = 0;
		webSocketPeriodic.ping_cnt_close_failed = 0;
		webSocketPeriodic.ping_cnt_trigger = 0;
		webSocketPeriodic.current_ping_count = 0;
	}
	else
	{
		std::cout << "Failed to deactivate web socket connection: web socket is not in use!" << std::endl;
	}
}

jvxErrorType 
CjvxWebControl_fe::try_activate_property(onePropertyWebSocketDefinition& defOneProperty)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;
	std::string collectStr;
	jvxPropertyStreamHeader* preparedHeader = NULL;
	jvxCallManagerProperties callGate;
	if (defOneProperty.runtime.transferState == JVX_STATE_NONE)
	{
		if (!defOneProperty.runtime.serious_fail)
		{
			myHostRef->state_selected_component(defOneProperty.cpId, &stat);
			if (stat & defOneProperty.state_active)
			{
				// Activate the property
				jvx_initPropertyReferenceTriple(&defOneProperty.runtime.theTriple);
				jvx_getReferencePropertiesObject(myHostRef, &defOneProperty.runtime.theTriple, defOneProperty.cpId);
				if (defOneProperty.runtime.theTriple.theProps)
				{
					jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(defOneProperty.propertyName.c_str());
					
					res = jvx_getPropertyDescription(defOneProperty.runtime.theTriple.theProps, defOneProperty.runtime.descr, ident, callGate);
					if (res == JVX_NO_ERROR)
					{
						switch (defOneProperty.cond_update)
						{
						case JVX_PROP_STREAM_UPDATE_TIMEOUT:
						case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
						case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
							if (
								((defOneProperty.runtime.descr.num == defOneProperty.numElms_spec) || (JVX_CHECK_SIZE_UNSELECTED(defOneProperty.numElms_spec))) &&
								((defOneProperty.runtime.descr.format == defOneProperty.format_spec) || (defOneProperty.format_spec == JVX_DATAFORMAT_NONE)) &&
								(defOneProperty.runtime.descr.decTp == defOneProperty.htTp))
							{
								switch (defOneProperty.runtime.descr.decTp)
								{
								case JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER:
									res = allocateMultichannelCircBuf_propstream(defOneProperty);
									break;

								case JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER:
									// todo: allocate and install switch buffer
									res = JVX_ERROR_UNSUPPORTED;
									break;
								default:

									res = allocateLinearBuffer_propstream(defOneProperty);
									break;
								}
							}
							else
							{
								defOneProperty.runtime.serious_fail = true;
								if (JVX_CHECK_SIZE_SELECTED(defOneProperty.numElms_spec))
								{
									if (defOneProperty.runtime.descr.num != defOneProperty.numElms_spec)
									{
										std::cout << "Failed to activate property " << defOneProperty.propertyName << " in component " << jvxComponentIdentification_txt(defOneProperty.cpId)
											<< ": number of element mismatch, local property provides  <" << defOneProperty.runtime.descr.num
											<< "> elements but external request asked for <" << defOneProperty.numElms_spec << "> elements." << std::endl;
									}
								}
								if (defOneProperty.format_spec != JVX_DATAFORMAT_NONE)
								{
									if (defOneProperty.runtime.descr.format != defOneProperty.format_spec)
									{
										std::cout << "Failed to activate property " << defOneProperty.propertyName << " in component " << jvxComponentIdentification_txt(defOneProperty.cpId)
											<< ": format of element mismatch, local property provides <" << jvxDataFormat_txt(defOneProperty.runtime.descr.format)
											<< "> but external request asked for <" << jvxDataFormat_txt(defOneProperty.format_spec) << ">" << std::endl;
									}
								}
								if (defOneProperty.runtime.descr.decTp != defOneProperty.htTp)
								{
									std::cout << "Failed to activate property " << defOneProperty.propertyName << " in component " << jvxComponentIdentification_txt(defOneProperty.cpId)
										<< ": decoder hint type mismatch, local property provides <" << jvxPropertyDecoderHintType_txt(defOneProperty.runtime.descr.decTp)
										<< "> but external request asked for <" << jvxPropertyDecoderHintType_txt(defOneProperty.htTp) << ">" << std::endl;
								}

								res = JVX_ERROR_INVALID_SETTING;
							}
							break;
						case JVX_PROP_STREAM_UPDATE_ON_CHANGE_REPORT_ORIGIN:
							collectStr = jvxComponentIdentification_txt(defOneProperty.cpId);
							collectStr += defOneProperty.propertyName;
							defOneProperty.runtime.szRawBuffer = collectStr.size();
							defOneProperty.runtime.szRawBuffer += sizeof(jvxPropertyStreamHeader);
							JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(defOneProperty.runtime.allocatedRawBuffer, jvxByte, defOneProperty.runtime.szRawBuffer);
							defOneProperty.runtime.referencedPayloadBuffer = defOneProperty.runtime.allocatedRawBuffer + sizeof(jvxPropertyStreamHeader);
							memcpy(defOneProperty.runtime.referencedPayloadBuffer, collectStr.c_str(), collectStr.size());
							preparedHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;

							/*
								Set streaming parameters
							 */
							preparedHeader->raw_header.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
							preparedHeader->raw_header.loc_header.purpose = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER | JVX_PS_SEND_INFORM_CHANGE;
							preparedHeader->raw_header.loc_header.paketsize = JVX_SIZE_INT32(defOneProperty.runtime.szRawBuffer);
							preparedHeader->raw_header.stream_id = JVX_SIZE_UINT16(defOneProperty.uniqueId);
							preparedHeader->raw_header.packet_tstamp = (jvxUInt16)-1;
							preparedHeader->property_num_elements = JVX_SIZE_UINT32(collectStr.size());
							preparedHeader->property_offset = JVX_SIZE_UINT16(0);
							preparedHeader->property_format = JVX_DATAFORMAT_STRING;
							preparedHeader->property_type = JVX_SIZE_UINT8(JVX_PROPERTY_DECODER_NONE);
							preparedHeader->param0 = JVX_SIZE_UINT32(defOneProperty.param0);
							preparedHeader->param1 = JVX_SIZE_UINT32(defOneProperty.param1);
							preparedHeader->requires_flow_response = 1;
							preparedHeader->reserved0 = 0;
							preparedHeader->reserved1 = 0;

							defOneProperty.runtime.cntTicks = 0;
							break;
						default: 
							assert(0);
						}

						if (res == JVX_NO_ERROR)
						{
							if (defOneProperty.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
							{
								defOneProperty.cond_update = JVX_PROP_STREAM_UPDATE_ON_CHANGE; // high priority only on change!
							}
						}

						if (res != JVX_NO_ERROR)
						{
							jvx_returnReferencePropertiesObject(myHostRef, &defOneProperty.runtime.theTriple, defOneProperty.cpId);
							jvx_initPropertyDescription(defOneProperty.runtime.descr);
							jvx_initPropertyReferenceTriple(&defOneProperty.runtime.theTriple);
						}
						else
						{
							// We need to add a little bit of protection since the high prio properties are transfered
							// in a high speed thread but only if active
							defOneProperty.runtime.transferState = JVX_STATE_ACTIVE;
						}
					} // if (res == JVX_NO_ERROR)
					else
					{
						defOneProperty.runtime.serious_fail = true;
						std::cout << "Failed to activate property " << defOneProperty.propertyName << " in component " << jvxComponentIdentification_txt(defOneProperty.cpId)
							<< ": failed to access property within component, error description: " << jvxErrorType_descr(res) << std::endl;
					}
				} // if (defOneProperty.runtime.theTriple.theProps)
				else
				{
					defOneProperty.runtime.serious_fail = true;
					std::cout << "Failed to activate property " << defOneProperty.propertyName << " in component " << jvxComponentIdentification_txt(defOneProperty.cpId)
						<< ": failed to obtain property handle reference." << std::endl;
				}
			}// if (stat & defOneProperty.state_active)
		}//if (!defOneProperty.runtime.serious_fail)
	}// if (defOneProperty.runtime.transferState == JVX_STATE_NONE)
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}

jvxErrorType
CjvxWebControl_fe::deactivate_property(onePropertyWebSocketDefinition& defOneProperty)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;
	jvxPropertyStreamHeader* preparedHeader = NULL;
	
	if(defOneProperty.runtime.transferState == JVX_STATE_ACTIVE)
	{
		// We need to add a little bit of protection since the high prio properties are transfered
		// in a high speed thread but only if active
		defOneProperty.runtime.transferState = JVX_STATE_NONE;

		// Deactivate the property
		if (defOneProperty.runtime.theTriple.theProps)
		{

			switch (defOneProperty.cond_update)
			{
			case JVX_PROP_STREAM_UPDATE_TIMEOUT:
			case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
			case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
				switch (defOneProperty.runtime.descr.decTp)
				{
				case JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER:

					res = deallocateMultichannelCircBuf_propstream(defOneProperty);
					break;
				case JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER:
					// todo: allocate and install switch buffer
					assert(0);
					break;
				default:
					deallocateLinearBuffer_propstream(defOneProperty);
					break;
				}
				break;
			case JVX_PROP_STREAM_UPDATE_ON_CHANGE_REPORT_ORIGIN:
				JVX_DSP_SAFE_DELETE_FIELD(defOneProperty.runtime.allocatedRawBuffer);
				defOneProperty.runtime.allocatedRawBuffer = NULL;
				break;
			default:
				assert(0);
			}

			jvx_returnReferencePropertiesObject(myHostRef, &defOneProperty.runtime.theTriple, defOneProperty.cpId);
		}
		jvx_initPropertyReferenceTriple(&defOneProperty.runtime.theTriple);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}

jvxPropertyTransferType
CjvxWebControl_fe::check_transfer_property(onePropertyWebSocketDefinition& defOneProperty, jvxPropertyStreamCondUpdate theReason, 
	const jvxComponentIdentification& idCp, jvxPropertyCategoryType cat, jvxSize propId)
{
	jvxPropertyTransferType doTransfer = JVX_PROP_TRANSFER_NONE;

	if (defOneProperty.requiresFlowControl)
	{
		if (defOneProperty.state_transfer != JVX_PROPERTY_WEBSOCKET_STATE_NONE)
		{
			jvxBool fail = true;
			if (defOneProperty.cond_update == JVX_PROP_STREAM_UPDATE_ON_CHANGE)
			{
				if (defOneProperty.param_cond_update > 0)
				{
					jvxTick tick = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
					jvxData tickNow = (jvxData)tick * 0.001;
					if (tickNow - defOneProperty.runtime.last_send_msec > defOneProperty.param_cond_update)
					{
						defOneProperty.state_transfer = JVX_PROPERTY_WEBSOCKET_STATE_NONE;
						fail = false;
						std::cout << "Restarting flow control for property <" << defOneProperty.propertyName << "> due to expired response timeout, " <<
							" missing response for id = " << defOneProperty.runtime.latest_tstamp << "." << std::endl;
					}
				}
			}
			if(fail)
			{
#ifdef JVX_VERBOSE_FLOW_CONTROL
				std::cout << "F" << std::endl;
#endif
				return doTransfer;
			}
		}
	}

	switch (theReason)
	{
	case JVX_PROP_STREAM_UPDATE_TIMEOUT:
		if (defOneProperty.cond_update == theReason)
		{
			if (defOneProperty.runtime.cntTicks == 0)
			{
				doTransfer = JVX_PROP_TRANSFER_CONTENT;
			}
			defOneProperty.runtime.cntTicks = (defOneProperty.runtime.cntTicks + 1) % defOneProperty.param_cond_update;
		}
		break;
	case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
		if (defOneProperty.cond_update == theReason)
		{
			if (
				(defOneProperty.cpId == idCp) &&
				(defOneProperty.runtime.descr.globalIdx == propId) &&
				(defOneProperty.runtime.descr.category == cat))
			{
				doTransfer = JVX_PROP_TRANSFER_CONTENT;
			}
		}
		break;
	case JVX_PROP_STREAM_UPDATE_ON_CHANGE_REPORT_ORIGIN:
		if (defOneProperty.cond_update == theReason)
		{
			if (
				(defOneProperty.cpId == idCp) &&
				(defOneProperty.runtime.descr.globalIdx == propId) &&
				(defOneProperty.runtime.descr.category == cat))
			{
				doTransfer = JVX_PROP_TRANSFER_SELF;
			}
		}
		break;
	case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
		break;
	}
	return doTransfer;
}

jvxErrorType 
CjvxWebControl_fe::transfer_activated_property(onePropertyWebSocketDefinition& defOneProperty, 
	jvxSize & numBytesTransferred)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxPropertyStreamHeader* preparedHeader = NULL;
	jvxSize num_avail = 0;
	jvxSize idxread = 0;
	
	jvxByte* copyFrom = NULL, * copyFrom0 = NULL;
	jvxByte* copyTo = NULL, *copyTo0 = NULL;
	jvxSize i;
	jvxSize szOneSample = 0;
	jvxSize szOneChannel_fh = 0;
	jvxSize szOneChannel_full = 0;
	jvxSize szAllChannels_fh = 0;
	jvxSize szAllChannels_full = 0;
	numBytesTransferred = 0;
	jvxCallManagerProperties callGate;

	jvxPropertyStreamHeader* theHeader = NULL;
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resM = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;

	// We may only transfer data via websocket if normal priority.
	// 
	if (defOneProperty.runtime.transferState == JVX_STATE_ACTIVE)
	{
		// Deactivate the property
		if (defOneProperty.runtime.theTriple.theProps)
		{
			switch (defOneProperty.cond_update)
			{
			case JVX_PROP_STREAM_UPDATE_TIMEOUT:
			case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
				switch (defOneProperty.runtime.descr.decTp)
				{
				case JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER:
					// todo: allocate and install circular buffer
					preparedHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;

					// std::cout << "------" << "Circular buffer pointer on ws send: " << defOneProperty.runtime.specbuffer << std::endl;
					defOneProperty.runtime.specbuffer->safe_access.lockf(defOneProperty.runtime.specbuffer->safe_access.priv);
					num_avail = defOneProperty.runtime.specbuffer->fill_height;
					idxread = defOneProperty.runtime.specbuffer->idx_read;
					defOneProperty.runtime.specbuffer->safe_access.unlockf(defOneProperty.runtime.specbuffer->safe_access.priv);

					// std::cout << __FUNCTION__ << "Circular buffer property via socket: " << defOneProperty.runtime.specbuffer << std::endl;

					if (num_avail > defOneProperty.num_emit_min)
					{
						jvxBool is_last_emit = false;
						while (!is_last_emit)
						{
							jvxSize num_transfer = num_avail;
							
							if (defOneProperty.num_emit_max > 0)
							{
								num_transfer = JVX_MIN(num_transfer, defOneProperty.num_emit_max);
							}

							num_avail -= num_transfer;
							if (num_avail <= defOneProperty.num_emit_min)
							{
								is_last_emit = true;
							}
							
							/*
							std::cout << "------" << "Sending " << num_transfer << " values" << std::flush;
							if (is_last_emit)
							{
								std::cout << " (last emit)." << std::flush;
							}
							else
							{
								std::cout << "." << std::flush;
							}
							std::cout << std::endl;
							*/

							szOneSample = jvxDataFormat_getsize(defOneProperty.runtime.descr.format);
							szOneChannel_fh = szOneSample * num_transfer;
							szOneChannel_full = szOneSample * defOneProperty.param1;
							szAllChannels_fh = szOneChannel_fh * defOneProperty.param0;
							szAllChannels_full = szOneChannel_full * defOneProperty.param0;

							preparedHeader->raw_header.loc_header.paketsize = JVX_SIZE_UINT32(sizeof(jvxPropertyStreamHeader) + szAllChannels_fh);
							preparedHeader->property_num_elements = JVX_SIZE_UINT32(num_transfer);
							preparedHeader->requires_flow_response = 1;

							jvxSize ll1 = defOneProperty.runtime.specbuffer->length - idxread;
							ll1 = JVX_MIN(ll1, num_transfer);
							jvxSize ll2 = num_transfer - ll1;
							// jvxUInt32* cpt = (jvxUInt32*)(defOneProperty.runtime.allocatedRawBuffer + sizeof(jvxPropertyStreamHeader));

							copyTo0 = defOneProperty.runtime.allocatedRawBuffer + sizeof(jvxPropertyStreamHeader);

							for (i = 0; i < defOneProperty.param0; i++)
							{
								// Compute pointer in buffers

								// Target buffers will be of length fHeight
								copyTo = copyTo0 + i * szOneChannel_fh;

								// Source buffers are of full size
								copyFrom0 = defOneProperty.runtime.specbuffer->ptrFld + i * szOneChannel_full;

								if (ll1)
								{
									copyFrom = copyFrom0 + szOneSample * idxread;
									memcpy(copyTo, copyFrom, szOneSample*ll1);
									copyTo += szOneSample * ll1;
								}
								if (ll2)
								{
									copyFrom = copyFrom0;
									memcpy(copyTo, copyFrom, szOneSample*ll2);
									copyTo += szOneSample * ll2;
								}
							}
							/*
							std::cout << defOneProperty.propertyName << " -- First value = " << *cpt << std::endl;
							cpt = (jvxUInt32*)copyTo - 8;
							std::cout << defOneProperty.propertyName << " -- Last value = " << *cpt << std::endl;
							*/
							// Update circular buffer state
							defOneProperty.runtime.specbuffer->safe_access.lockf(defOneProperty.runtime.specbuffer->safe_access.priv);
							defOneProperty.runtime.specbuffer->fill_height -= num_transfer;
							defOneProperty.runtime.specbuffer->idx_read = (defOneProperty.runtime.specbuffer->idx_read +
								num_transfer) % defOneProperty.runtime.specbuffer->length;
							idxread = defOneProperty.runtime.specbuffer->idx_read; // Local update of read index. Do NOT update fill height to prevent endless send at very high speeds
							defOneProperty.runtime.specbuffer->safe_access.unlockf(defOneProperty.runtime.specbuffer->safe_access.priv);

							jvxPropertyStreamHeader* theHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;
							theHeader->raw_header.packet_tstamp = defOneProperty.tStamp++;
							defOneProperty.streamPropertyInTransferState = 0;
							defOneProperty.state_transfer = JVX_PROPERTY_WEBSOCKET_STATE_IN_TRANSFER;

							jvxTick tick = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
							defOneProperty.runtime.last_send_msec = (jvxData)tick * 0.001;
							defOneProperty.runtime.latest_tstamp = theHeader->raw_header.packet_tstamp;

							if (is_last_emit)
							{
								preparedHeader->requires_flow_response = 1;
							}
							else
							{
								preparedHeader->requires_flow_response = 0;
							}

#ifdef JVX_VERBOSE_FLOW_CONTROL
							std::cout << "S[" << theHeader->raw_header.stream_id << "--" << theHeader->raw_header.packet_tstamp << "]" << std::endl;
#endif
							if (defOneProperty.prio == JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO)
							{
								resL = myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
									(const char*)theHeader, theHeader->raw_header.loc_header.paketsize);
							}
							else
							{
								resL = defOneProperty.runtime.prio_high.udpSocket->send_packet((const char*)theHeader, theHeader->raw_header.loc_header.paketsize);
								if (resL != JVX_NO_ERROR)
								{
									std::cout << __FUNCTION__ << ": Error: UDP Send Error, error reason: " << jvxErrorType_txt(resL) << "." << std::endl;
								}
							}
							numBytesTransferred = theHeader->raw_header.loc_header.paketsize;
						}

					}

					break;
				case JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER:
					// todo: allocate and install switch buffer
					res = JVX_ERROR_UNSUPPORTED;
					break;
				default:

					if (JVX_CHECK_SIZE_UNSELECTED(defOneProperty.numElms_spec))
					{
						jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
						jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(defOneProperty.propertyName.c_str());
						if (defOneProperty.runtime.theTriple.theProps)
						{
							resL = jvx_getPropertyDescription(defOneProperty.runtime.theTriple.theProps, theDescr, ident, callGate);
						}
						else
						{
							resL = JVX_ERROR_UNSUPPORTED;
						}
						if (resL != JVX_NO_ERROR)
						{
							res = JVX_ERROR_INTERNAL;
						}
						if (
							(defOneProperty.runtime.descr.num != theDescr.num) ||
							(defOneProperty.runtime.descr.format != theDescr.format))
						{
							defOneProperty.runtime.descr = theDescr;
							resL = deallocateLinearBuffer_propstream(defOneProperty);
							resL = allocateLinearBuffer_propstream(defOneProperty);
						}
					}
					resL = jvx_get_property(defOneProperty.runtime.theTriple.theProps, defOneProperty.runtime.referencedPayloadBuffer,
						defOneProperty.offset, defOneProperty.runtime.descr.num, defOneProperty.runtime.descr.format, true, 
						defOneProperty.propertyName.c_str(),
						callGate);
					if (resL == JVX_NO_ERROR)
					{
						jvxPropertyStreamHeader* theHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;

						theHeader->raw_header.packet_tstamp = defOneProperty.tStamp++;
						theHeader->requires_flow_response = 1;

						defOneProperty.streamPropertyInTransferState = 0;
						defOneProperty.state_transfer = JVX_PROPERTY_WEBSOCKET_STATE_IN_TRANSFER;

						jvxTick tick = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
						defOneProperty.runtime.last_send_msec = (jvxData)tick * 0.001;
						defOneProperty.runtime.latest_tstamp = theHeader->raw_header.packet_tstamp;

#ifdef JVX_VERBOSE_FLOW_CONTROL
						std::cout << "S[" << theHeader->raw_header.stream_id << "--" << theHeader->raw_header.packet_tstamp << "]" << std::endl;
#endif
						if (defOneProperty.prio == JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO)
						{
							resL = myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
								(const char*)theHeader, theHeader->raw_header.loc_header.paketsize);
						}
						else
						{
							resL = defOneProperty.runtime.prio_high.udpSocket->send_packet((const char*)theHeader, theHeader->raw_header.loc_header.paketsize);
							if (resL != JVX_NO_ERROR)
							{
								std::cout << __FUNCTION__ << ": Error: UDP Send Error, error reason: " << jvxErrorType_txt(resL) << "." << std::endl;
							}
						}
						numBytesTransferred = theHeader->raw_header.loc_header.paketsize;
					}

					break;
				}
				break;
			case JVX_PROP_STREAM_UPDATE_ON_CHANGE_REPORT_ORIGIN:

				if (defOneProperty.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
				{
					theHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;

					theHeader->requires_flow_response = 1;
					resL = myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
						(const char*)theHeader, theHeader->raw_header.loc_header.paketsize);
					numBytesTransferred = theHeader->raw_header.loc_header.paketsize;
				}
				break;
			default:
				assert(0);
			} // switch (defOneProperty.cond_update)
		}
		else
		{
			res = JVX_ERROR_INTERNAL;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	return res;
}

// ===============================================================================================
jvxErrorType
CjvxWebControl_fe::allocateLinearBuffer_propstream(onePropertyWebSocketDefinition& defOneProperty)
{
	jvxPropertyStreamHeader* preparedHeader = NULL;
	if (defOneProperty.runtime.descr.num == 0)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}
	
	switch(defOneProperty.runtime.descr.format)
	{
	case JVX_DATAFORMAT_DATA:
	case JVX_DATAFORMAT_8BIT:
	case JVX_DATAFORMAT_16BIT_LE:
	case JVX_DATAFORMAT_32BIT_LE:
	case JVX_DATAFORMAT_64BIT_LE:
	case JVX_DATAFORMAT_U16BIT_LE:
	case JVX_DATAFORMAT_U32BIT_LE:
	case JVX_DATAFORMAT_U64BIT_LE:
		break;
	default:
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	std::cout << "-- Property with descriptor <" << defOneProperty.runtime.descr.descriptor.std_str() << "> --" << std::flush;
	switch (defOneProperty.cond_update)
	{
	case JVX_PROP_STREAM_UPDATE_TIMEOUT:
		std::cout << "Update on timeout, update period = " << defOneProperty.param_cond_update << "." << std::endl;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
		std::cout << "Update on change." << std::flush;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
		std::cout << "Update on request." << std::flush;
		break;
	}

	defOneProperty.runtime.szRawBuffer = jvxDataFormat_getsize(defOneProperty.runtime.descr.format) * defOneProperty.runtime.descr.num;
	defOneProperty.runtime.szRawBuffer += sizeof(jvxPropertyStreamHeader);

	std::cout << "--> Allocating linear buffer, length = " << defOneProperty.runtime.descr.num <<
		", format = " << jvxDataFormat_txt(defOneProperty.runtime.descr.format) <<
		"." << std::endl;

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(defOneProperty.runtime.allocatedRawBuffer, jvxByte, defOneProperty.runtime.szRawBuffer);
	defOneProperty.runtime.referencedPayloadBuffer = defOneProperty.runtime.allocatedRawBuffer + sizeof(jvxPropertyStreamHeader);
	preparedHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;

	/*
		Set streaming parameters
	*/
	preparedHeader->raw_header.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
	preparedHeader->raw_header.loc_header.purpose = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER | JVX_PS_SEND_LINEARFIELD;
	preparedHeader->raw_header.loc_header.paketsize = JVX_SIZE_INT32( defOneProperty.runtime.szRawBuffer);
	preparedHeader->raw_header.stream_id = JVX_SIZE_UINT16(defOneProperty.uniqueId);
	preparedHeader->raw_header.packet_tstamp = (jvxUInt16)-1;
	preparedHeader->property_num_elements = JVX_SIZE_UINT32(defOneProperty.runtime.descr.num);
	preparedHeader->property_offset = JVX_SIZE_UINT16(defOneProperty.offset);
	preparedHeader->property_format = defOneProperty.runtime.descr.format;
	preparedHeader->property_type = JVX_SIZE_UINT8(defOneProperty.htTp);
	preparedHeader->param0 = JVX_SIZE_UINT32(defOneProperty.param0);
	preparedHeader->param1 = JVX_SIZE_UINT32(defOneProperty.param1);
	preparedHeader->requires_flow_response = 1;
	preparedHeader->reserved0 = 0;
	preparedHeader->reserved1 = 0;

	defOneProperty.runtime.cntTicks = 0;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControl_fe::deallocateLinearBuffer_propstream(onePropertyWebSocketDefinition& defOneProperty)
{
	std::cout << "-- Property with descriptor <" << defOneProperty.runtime.descr.descriptor.std_str() << "> --" << std::flush;
	switch (defOneProperty.cond_update)
	{
	case JVX_PROP_STREAM_UPDATE_TIMEOUT:
		std::cout << "Update on timeout, update period = " << defOneProperty.param_cond_update << "." << std::endl;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
		std::cout << "Update on change." << std::flush;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
		std::cout << "Update on request." << std::flush;
		break;
	}
	std::cout << "--> Deallocating linear buffer, length = " << defOneProperty.runtime.descr.num <<
		", format = " << jvxDataFormat_txt(defOneProperty.runtime.descr.format) <<
		"." << std::endl;

	JVX_DSP_SAFE_DELETE_FIELD(defOneProperty.runtime.allocatedRawBuffer);
	defOneProperty.runtime.referencedPayloadBuffer = NULL;
	defOneProperty.runtime.szRawBuffer = 0;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControl_fe::allocateMultichannelCircBuf_propstream(onePropertyWebSocketDefinition& defOneProperty)
{
	jvxCallManagerProperties callGate;
	jvxPropertyStreamHeader* preparedHeader = NULL;
	if (defOneProperty.runtime.descr.num == 0)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	switch (defOneProperty.runtime.descr.format)
	{
	case JVX_DATAFORMAT_DATA:
	case JVX_DATAFORMAT_16BIT_LE:
	case JVX_DATAFORMAT_32BIT_LE:
	case JVX_DATAFORMAT_64BIT_LE:
	case JVX_DATAFORMAT_8BIT:
	case JVX_DATAFORMAT_U16BIT_LE:
	case JVX_DATAFORMAT_U32BIT_LE:
	case JVX_DATAFORMAT_U64BIT_LE:
	case JVX_DATAFORMAT_U8BIT:
		break;
	default:
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	/*
	if (
		!(
		(defOneProperty.runtime.descr.format >= ) &&
			(defOneProperty.runtime.descr.format <= JVX_DATAFORMAT_8BIT)))
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}
	*/

	jvxSize szFld_y = JVX_SIZE_UNSELECTED;
	jvxSize szFld_x = JVX_SIZE_UNSELECTED;

	std::cout << "-- Property with descriptor <" << defOneProperty.runtime.descr.descriptor.std_str() << "> --" << std::flush;
	switch (defOneProperty.cond_update)
	{
	case JVX_PROP_STREAM_UPDATE_TIMEOUT:
		std::cout << "Update on timeout, update period = " << defOneProperty.param_cond_update << "." << std::endl;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
		std::cout << "Update on change." << std::flush;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
		std::cout << "Update on request." << std::flush;
		break;
	}

	if (JVX_CHECK_SIZE_UNSELECTED(defOneProperty.param1))
	{
		defOneProperty.runtime.theTriple.theProps->get_property_extended_info(callGate,
			&szFld_y, JVX_PROPERTY_INFO_RECOMMENDED_SIZE_Y,
			jPAGID(defOneProperty.runtime.descr.globalIdx, defOneProperty.runtime.descr.category));
	}
	if (JVX_CHECK_SIZE_UNSELECTED(defOneProperty.param0))
	{
		defOneProperty.runtime.theTriple.theProps->get_property_extended_info(callGate,
			&szFld_x, JVX_PROPERTY_INFO_RECOMMENDED_SIZE_X,
			jPAGID(defOneProperty.runtime.descr.globalIdx, defOneProperty.runtime.descr.category));
	}

	if (JVX_CHECK_SIZE_SELECTED(defOneProperty.param1) && JVX_CHECK_SIZE_SELECTED(defOneProperty.param0))
	{
		std::cout << "--> Allocating multichannel circular buffer, length = " << defOneProperty.param1 <<
			", number channels = " << defOneProperty.param0 <<
			", format = " << jvxDataFormat_txt(defOneProperty.runtime.descr.format) <<
			", subformat = " << jvxDataFormatGroup_txt(JVX_DATAFORMAT_GROUP_GENERIC_NON_INTERLEAVED) << std::flush;
		if (defOneProperty.num_emit_min)
		{
			std::cout << ", minimum length constraint = " << defOneProperty.num_emit_min << std::flush;
		}
		else
		{
			std::cout << ", maximum length constraint = " << defOneProperty.num_emit_max << std::flush;

		}
		std::cout << "." << std::endl;

		defOneProperty.runtime.specbuffer = jvx_allocate1DCircExternalBuffer(
			defOneProperty.param1, defOneProperty.param0,
			defOneProperty.runtime.descr.format,
			JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED,
			jvx_static_lock, jvx_static_try_lock, jvx_static_unlock,
			&defOneProperty.runtime.specbuffer_sz);
		defOneProperty.runtime.specbuffer_valid = true;

		// Specify the property parameters for association
		jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(defOneProperty.runtime.specbuffer, 
			&defOneProperty.runtime.specbuffer_valid,
			defOneProperty.runtime.descr.format);
		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(defOneProperty.runtime.descr.globalIdx, 
			defOneProperty.runtime.descr.category);

		defOneProperty.runtime.theTriple.theProps->install_property_reference(callGate,
			ptrRaw, ident);

		defOneProperty.runtime.szRawBuffer = jvxDataFormat_getsize(defOneProperty.runtime.descr.format) * defOneProperty.param0 * defOneProperty.param1;
		defOneProperty.runtime.szRawBuffer += sizeof(jvxPropertyStreamHeader);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(defOneProperty.runtime.allocatedRawBuffer, jvxByte, defOneProperty.runtime.szRawBuffer);
		defOneProperty.runtime.referencedPayloadBuffer = defOneProperty.runtime.allocatedRawBuffer + sizeof(jvxPropertyStreamHeader);
		preparedHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;

		/*
			Set streaming parameters
		*/
		preparedHeader->raw_header.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
		preparedHeader->raw_header.loc_header.purpose = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER | JVX_PS_SEND_LINEARFIELD;
		preparedHeader->raw_header.loc_header.paketsize = JVX_SIZE_INT32(defOneProperty.runtime.szRawBuffer);
		preparedHeader->raw_header.stream_id = JVX_SIZE_UINT16(defOneProperty.uniqueId);
		preparedHeader->raw_header.packet_tstamp = (jvxUInt16)-1;
		preparedHeader->property_num_elements = JVX_SIZE_UINT32(defOneProperty.runtime.descr.num);
		preparedHeader->property_offset = JVX_SIZE_UINT16(defOneProperty.offset);
		preparedHeader->property_format = defOneProperty.runtime.descr.format;
		preparedHeader->property_type = JVX_SIZE_UINT8(defOneProperty.htTp);
		preparedHeader->param0 = JVX_SIZE_UINT32(defOneProperty.param0);
		preparedHeader->param1 = JVX_SIZE_UINT32(defOneProperty.param1);
		preparedHeader->requires_flow_response = 1;
		preparedHeader->reserved0 = 0;
		preparedHeader->reserved1 = 0;

		defOneProperty.runtime.cntTicks = 0;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxWebControl_fe::deallocateMultichannelCircBuf_propstream(onePropertyWebSocketDefinition& defOneProperty)
{
	std::cout << "-- Property with descriptor <" << defOneProperty.runtime.descr.descriptor.std_str() << "> --" << std::flush;
	switch (defOneProperty.cond_update)
	{
	case JVX_PROP_STREAM_UPDATE_TIMEOUT:
		std::cout << "Update on timeout, update period = " << defOneProperty.param_cond_update << "." << std::endl;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
		std::cout << "Update on change." << std::flush;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
		std::cout << "Update on request." << std::flush;
		break;
	}
	std::cout << "--> Deallocating multichannel circular buffer, length = " << defOneProperty.param1 <<
		", number channels = " << defOneProperty.param0 <<
		", format = " << jvxDataFormat_txt(defOneProperty.runtime.descr.format) <<
		", subformat = " << jvxDataFormatGroup_txt(JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED) <<
		"." << std::endl;

	jvxCallManagerProperties callGate;
	assert(defOneProperty.runtime.specbuffer);

	jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(defOneProperty.runtime.specbuffer,
		&defOneProperty.runtime.specbuffer_valid,
		defOneProperty.runtime.descr.format);
	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(defOneProperty.runtime.descr.globalIdx,
		defOneProperty.runtime.descr.category);

	defOneProperty.runtime.theTriple.theProps->uninstall_property_reference(callGate, ptrRaw, ident);
	jvx_deallocateExternalBuffer(defOneProperty.runtime.specbuffer);
	defOneProperty.runtime.specbuffer = NULL;
	defOneProperty.runtime.specbuffer_sz = 0;
	defOneProperty.runtime.specbuffer_valid = false;
	return deallocateLinearBuffer_propstream(defOneProperty);
}

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
	step_update_properties_websocket(JVX_PROP_STREAM_UPDATE_ON_CHANGE, &hasDisconnected, thisismytype, cat, propId);

	if (tpTo == JVX_COMPONENT_UNKNOWN)
	{
		// This always comes from within main thread
		if (webSocketPeriodic.theCtxt.context_conn)
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

			resL = myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
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
	return JVX_ERROR_UNSUPPORTED;
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

void
CjvxWebControl_fe::step_update_properties_websocket(jvxPropertyStreamCondUpdate theReason, 
	jvxBool* has_disconnected, const jvxComponentIdentification& idCp, jvxPropertyCategoryType cat , jvxSize propId)
{
	jvxErrorType resL = JVX_NO_ERROR;
	std::map<jvxSize, onePropertyWebSocketDefinition>::iterator elm;
	jvxSize cnt = 0;
	jvxSize numBytes = 0;

	*has_disconnected = false;

	if (theReason == JVX_PROP_STREAM_UPDATE_TIMEOUT)
	{
		// std::cout << __FUNCTION__ << " Running property callback - " << webSocketPeriodic.current_ping_count << std::endl;
		webSocketPeriodic.current_ping_count++;
		if (webSocketPeriodic.current_ping_count > webSocketPeriodic.ping_cnt_trigger)
		{
			// resL = myWebServer.hdl->in_connect_drop_connection((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn);
			myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, 0x9, NULL, 0);
		}
		if (webSocketPeriodic.current_ping_count > webSocketPeriodic.ping_cnt_close)
		{
			// resL = myWebServer.hdl->in_connect_drop_connection((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn);
			myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, 0x8, NULL, 0);
		}
		if (webSocketPeriodic.current_ping_count > webSocketPeriodic.ping_cnt_close_failed)
		{
			// Client is not responding - assuming it has gone!
			web_socket_disconnect();
			*has_disconnected = true;
		}
	}
	// Do not need to protect list since only read access in main thread
	elm = lstUpdateProperties.begin();
	if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
	{
		jvx_lock_text_log(jvxrtst_bkp);
		jvxrtst << "Update properties via websocket, reason " << jvxPropertyStreamCondUpdate_txt(theReason) << std::endl;
		if (!config.silent_mode)
		{
			std::cout << "Update properties via websocket, reason " << jvxPropertyStreamCondUpdate_txt(theReason) << std::endl;
		}
		jvx_unlock_text_log(jvxrtst_bkp);		
	}

	cnt = 0;
	for (; elm != lstUpdateProperties.end(); elm++)
	{
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "Check #" << cnt << ": " << jvxComponentIdentification_txt(elm->second.cpId) << "::" << elm->second.propertyName << "(" << elm->second.uniqueId << ")" << "--" << "Status: " << jvxState_txt(elm->second.runtime.transferState)
				<< "--Activate on: " << jvxState_txt(elm->second.state_active) << "--" << jvxPropertyStreamCondUpdate_txt(elm->second.cond_update) << "::" << elm->second.param_cond_update << std::endl;
			if (!config.silent_mode)
			{
				jvxrtst << "Check #" << cnt << ": " << jvxComponentIdentification_txt(elm->second.cpId) << "::" << elm->second.propertyName << "(" << elm->second.uniqueId << ")" << "--" << "Status: " << jvxState_txt(elm->second.runtime.transferState)
					<< "--Activate on: " << jvxState_txt(elm->second.state_active) << "--" << jvxPropertyStreamCondUpdate_txt(elm->second.cond_update) << "::" << elm->second.param_cond_update << std::endl;
			}
			jvx_unlock_text_log(jvxrtst_bkp);
			
		}

		if (elm->second.runtime.transferState == JVX_STATE_NONE)
		{
			resL = try_activate_property(elm->second);
		}

		if (elm->second.runtime.transferState == JVX_STATE_ACTIVE)
		{
			jvxPropertyTransferType doTransfer = check_transfer_property(elm->second, theReason, idCp, cat, propId);
			if (doTransfer != JVX_PROP_TRANSFER_NONE)
			{
				if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
				{
					jvx_lock_text_log(jvxrtst_bkp);
					jvxrtst << "--> Property " << elm->second.propertyName << " is requested to be transfered." << std::endl;
					if (!config.silent_mode)
					{
						std::cout << "--> Property " << elm->second.propertyName << " is requested to be transfered." << std::endl;
					}
					jvx_unlock_text_log(jvxrtst_bkp);
				}
				resL = transfer_activated_property(elm->second, numBytes);
				if (resL != JVX_NO_ERROR)
				{
					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvx_lock_text_log(jvxrtst_bkp);
						jvxrtst << "Failed to transfer property " << elm->second.propertyName << ", deactivating" << std::endl;
						if (!config.silent_mode)
						{
							std::cout << "Failed to transfer property" << elm->second.propertyName << ", deactivating" << std::endl;
						}
						jvx_unlock_text_log(jvxrtst_bkp);

					}
					resL = deactivate_property(elm->second);
				}
				else
				{
					if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
					{
						jvx_lock_text_log(jvxrtst_bkp);
						jvxrtst << "--> For property " << elm->second.propertyName << ", " << numBytes << " bytes were successfully transfered." << std::endl;
						if (!config.silent_mode)
						{
							std::cout << "--> For property " << elm->second.propertyName << ", " << numBytes << " bytes were successfully transfered." << std::endl;
						}
						jvx_unlock_text_log(jvxrtst_bkp);

					}
				}
			}
		}

		cnt++;
	}
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
CjvxWebControl_fe::request_property(jvxFrontendSupportRequestType tp, jvxHandle* load)
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
CjvxWebControl_fe::request_command_in_main_thread(CjvxReportCommandRequest* request, jvxBool)
{
	// Here, we can forward the requests!!!
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
		if (webSocketPeriodic.theCtxt.context_conn)
		{
			resL = myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
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
		if (webSocketPeriodic.theCtxt.context_conn)
		{
			resL = myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
				(const char*)&theSeqHeader, theSeqHeader.loc_header.paketsize);
		}
		break;
	default:
		std::cout << "Unknown Special Event Report." << std::endl;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControl_fe::disconnect_udp_port(const onePropertyWebSocketDefinition& prop_elm)
{
	if (prop_elm.runtime.prio_high.udpSocket)
	{
		jvxInt32 port = 0;
		prop_elm.runtime.prio_high.udpSocket->unregister_property(prop_elm.uniqueId, port);
		if (port != 0)
		{
			auto elms = high_prio_transfer.theUdpSockets.find(port);
			if (elms != high_prio_transfer.theUdpSockets.end())
			{
				HjvxPropertyStreamUdpSocket* to_remove = elms->second;
				high_prio_transfer.theUdpSockets.erase(elms);
				to_remove->terminate();
				JVX_DSP_SAFE_DELETE_OBJECT(to_remove);
			}
			else
			{
				assert(0);
			}
		}
	}
	else
	{
		assert(0);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControl_fe::report_incoming_packet(jvxByte* fld, jvxSize sz, HjvxPropertyStreamUdpSocket* resp_socket)
{
	jvxErrorType res = JVX_NO_ERROR;

	assert(sz >= sizeof(jvxRawStreamHeader_response));
	TjvxEventLoopElement elm;
	//jvxPropertyPropertyObserveHeader_response* paddheader = (jvxPropertyPropertyObserveHeader_response*)fld;
	elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
	elm.priv_fe = NULL;
	elm.target_be = static_cast<IjvxEventLoop_backend*>(this);
	elm.priv_be = NULL;

	elm.param = (jvxHandle*)fld;
	elm.paramType = JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 4;

	elm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 7;
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
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxWebControl_fe::report_connection_error(const char* errorText)
{
	TjvxEventLoopElement elm;
	std::string err = errorText;

	elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(this);
	elm.priv_fe = NULL;
	elm.target_be = static_cast<IjvxEventLoop_backend*>(this);
	elm.priv_be = NULL;

	elm.param = (jvxHandle*)&err;
	elm.paramType = JVX_EVENTLOOP_DATAFORMAT_STDSTRING;

	elm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 8;
	elm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
	elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
	elm.delta_t = JVX_TIMOUT_BLOCKING_CALL_MSEC;
	elm.autoDeleteOnProcess = c_true;
	jvxErrorType res = this->evLop->event_schedule(&elm, NULL, NULL);
	return JVX_NO_ERROR;
}
