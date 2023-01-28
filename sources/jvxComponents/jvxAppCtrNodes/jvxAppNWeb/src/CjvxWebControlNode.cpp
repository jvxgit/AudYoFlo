#include "CjvxWebControlNode.h"

#define JVX_TYPE_HTTP 0x1234
#define JVX_TYPE_WS 0x4321
#define JVX_WS_READY_COMMAND "show(system, compact)"

CjvxWebControlNode::CjvxWebControlNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxApplicationControllerNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	JVX_INITIALIZE_RW_MUTEX(safeAccessWebConnection);
}

CjvxWebControlNode::~CjvxWebControlNode()
{
	JVX_TERMINATE_RW_MUTEX(safeAccessWebConnection);
}

void
CjvxWebControlNode::right_before_start()
{
	jvxCallManagerProperties callGate;
	IjvxProperties* props = reqInterfaceObj<IjvxProperties>(webServ.objPtr);
	if (props)
	{
		
		jvxApiString txt;
		txt.assign(genAppCtrWeb_device::werbserver_config.www_directory.value.c_str());// "./www");
		jvxErrorType res = props->set_property(callGate, jPROS(&txt), jPAD("/wwwRootDirectory"));
		assert(res == JVX_NO_ERROR);
		res = props->set_property(callGate, jPRFS(&genAppCtrWeb_device::werbserver_config.www_port.value), jPAD("/wwwListeningPort"));
		assert(res == JVX_NO_ERROR);
		res = props->set_property(callGate, jPRFS(&genAppCtrWeb_device::werbserver_config.www_numthreads.value), jPAD("/wwwNumberThreads"));
		assert(res == JVX_NO_ERROR);
		retInterfaceObj<IjvxProperties>(webServ.objPtr, props);
	}
}

jvxErrorType
CjvxWebControlNode::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxApplicationControllerNode::select(owner);
	if (res == JVX_NO_ERROR)
	{
		genAppCtrWeb_device::init__werbserver_config();
		genAppCtrWeb_device::allocate__werbserver_config();
		genAppCtrWeb_device::register__werbserver_config(static_cast<CjvxProperties*>(this));
	}

	return(res);
}

jvxErrorType
CjvxWebControlNode::unselect()
{
	jvxErrorType res = CjvxApplicationControllerNode::unselect();
	if (res == JVX_NO_ERROR)
	{
		genAppCtrWeb_device::unregister__werbserver_config(static_cast<CjvxProperties*>(this));
		genAppCtrWeb_device::deallocate__werbserver_config();		
	}

	return(res);
}

// =======================================================================================================

jvxErrorType
CjvxWebControlNode::activate()
{
	jvxErrorType res = CjvxApplicationControllerNode::activate();
	if(res == JVX_NO_ERROR)
	{		
		cmdInterpreter = reqInterface<IjvxCommandInterpreter>(_common_set_min.theHostRef);
		hostRef = reqInterface<IjvxHost>(_common_set_min.theHostRef);

		webServ = reqRefTool<IjvxWebServer>(_common_set.theToolsHost, JVX_COMPONENT_WEBSERVER);
		
		acceptNewEvents = true;
		if (webServ.cpPtr)
		{
			webServ.cpPtr->state(&statWSInit);
			if(statWSInit < JVX_STATE_INIT)
			{

				start_webserver(webServ.cpPtr, hostRef, this);
			}
		}

	}
	return(res);
}

jvxErrorType
CjvxWebControlNode::deactivate()
{
	jvxSize i;
	jvxErrorType res = CjvxApplicationControllerNode::_pre_check_deactivate();
	if(res == JVX_NO_ERROR)
	{
		acceptNewEvents = false;
		jvxBool runAnother = true;
		
restart_end_loop_conn:
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(safeAccessWebConnection);
		for (auto elm : workerHdls)
		{
			if (elm.second->stat > JVX_STATE_NONE)
			{
				if (elm.second->stat > JVX_STATE_INIT)
				{
					// Unlock this list to allow the other threads to continue
					JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(safeAccessWebConnection);
					JVX_SLEEP_MS(200);
					elm.second->endCnt++;
					assert(elm.second->endCnt < 10);
					goto restart_end_loop_conn;
				}
				else
				{
					// All clean, worker handles can be removed
					elm.second->stat = JVX_STATE_NONE;
					JVX_TERMINATE_NOTIFICATION(elm.second->waitHdl);
					break;
				}			
			}
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(safeAccessWebConnection);		
		workerHdls.clear();

		while (wsConnections.size())
		{
		restart_end_loop_ws:
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(safeAccessWebConnection);
			for (auto elmws : wsConnections)
			{
				if (elmws.second->stat > JVX_STATE_INIT)
				{
					elmws.second->stat = JVX_STATE_INIT;

					/*
					 * So here is the stop code: first send out the stop byte and then set flag in civetweb backend.
					 * There will be now acknowledge since the connection is closed after a timeout. 
					 * We remove the object here and leave it to civetweb to handle it after the timeout
					 */
					// Send the leave opcode for the other side
					webServ.cpPtr->in_connect_send_ws_packet((struct mg_connection*)elmws.first, 0x8, nullptr, 0);

					// Run shutdown
					webServ.cpPtr->in_connect_drop_connection((struct mg_connection*)elmws.first);

					elmws.second->clear_ws_container();
					JVX_DSP_SAFE_DELETE_OBJECT(elmws.second);
					wsConnections.erase(elmws.first);
					JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(safeAccessWebConnection);

					// We need to jump back since the iterator is broken if we erase
					goto restart_end_loop_ws;
				}
			}
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(safeAccessWebConnection);
		}

		if (statWSInit == JVX_STATE_NONE)
		{
			stop_webserver();
		}
		if (webServ.cpPtr)
		{
			retRefTool<IjvxWebServer>(_common_set.theToolsHost, JVX_COMPONENT_WEBSERVER, webServ);
		}
		if (hostRef)
		{
			retInterface<IjvxHost>(_common_set_min.theHostRef, hostRef);
			hostRef = nullptr;
		}
		if (cmdInterpreter)
		{
			retInterface<IjvxCommandInterpreter>(_common_set_min.theHostRef, cmdInterpreter);
			cmdInterpreter = nullptr;
		}
		res = CjvxApplicationControllerNode::deactivate();
		
	}
	return(res);
}

jvxErrorType 
CjvxWebControlNode::synchronizeWebServerCoEvents(
	jvxHandle* context_server, jvxHandle* context_conn,
	jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
	jvxBool strictConstConnection, const char* uriprefix)
{
	std::string command;
	jvxBool requiresInterpretation = false;
	jvxBool errorDetected = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize myId = 0;
	jvxApiString str;
	jvxBool was_registered = false;

	// Interpret the requested operation and respond with response
	report_event_request_translate(
		context_server, context_conn, purp, uniqueId, strictConstConnection, uriprefix, 0,
		NULL, 0, command, requiresInterpretation, errorDetected, &jvxrtst,
		static_cast<jvx_lock*>(&jvxrtst_bkp.jvxos), silentMode);
	if (requiresInterpretation)
	{
		// After execution of reaction in other thread..
		// Process the response
		JVX_THREAD_ID threadId = JVX_GET_CURRENT_THREAD_ID();

		oneThreadWorker* threadObj = nullptr;
		if (acceptNewEvents)
		{
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(safeAccessWebConnection);
			auto hdl = workerHdls.find(threadId);
			if (hdl == workerHdls.end())
			{
				JVX_DSP_SAFE_ALLOCATE_OBJECT(threadObj, oneThreadWorker);
				JVX_INITIALIZE_NOTIFICATION(threadObj->waitHdl);

				workerHdls[threadId] = threadObj;
				threadObj->stat = JVX_STATE_INIT;
			}
			else
			{
				threadObj = hdl->second;
			}

			if (threadObj->stat == JVX_STATE_INIT)
			{
				threadObj->stat = JVX_STATE_ACTIVE;
				threadObj->command = command;
			}
			else
			{
				threadObj = nullptr;
				assert(false);
			}
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(safeAccessWebConnection);
		}

		JVX_WAIT_FOR_NOTIFICATION_I(threadObj->waitHdl);

		CjvxReportCommandRequest_rm mess(_common_set.theComponentType, threadId);
		_common_set.theReport->request_command(mess);
		JVX_WAIT_RESULT resW = JVX_WAIT_FOR_NOTIFICATION_II_S(threadObj->waitHdl, 500);
		if (resW == JVX_WAIT_SUCCESS)
		{			
			// This part is not in a lock. We can do so since we never delete an object which is not in state
			// INIT
			if (
				(threadObj->resL == JVX_NO_ERROR) && threadObj->stat == JVX_STATE_COMPLETE)
			{
				webServ.cpPtr->in_connect_write_header_response(context_server, context_conn, JVX_WEB_SERVER_RESPONSE_JSON);
				webServ.cpPtr->in_connect_write_response(context_server, context_conn, threadObj->response.c_str());

				if (!silentMode)
				{
					std::cout << "Return: " << threadObj->response << std::endl;
				}
			}
			else
			{
				webServ.cpPtr->in_connect_write_error(context_conn, 503, threadObj->response.c_str());
			}

			// Reset the status of this handler
			threadObj->command.clear();
			threadObj->response.clear();
			threadObj->resL = JVX_NO_ERROR;
			threadObj->stat = JVX_STATE_INIT;
			// From here on, a removal may happen - not before
			return res;
		}
		else
		{
			// Here we have a problem: the message is still pending but we can not remove since it may come back later
			// We should catch it where else but at first we indicate an unepxted error
			assert(false);
		}
		//std::cout << __FUNCTION__ << " leaving blocking loop" << std::endl;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControlNode::schedule_main_loop(jvxSize rescheduleId, jvxHandle* user_data)
{
	if (rescheduleId == 0)
	{
		// This request comes from the websocket part
		jvxHandle* conn = user_data;
		CjvxWebControlNodeWsHandle* wsObject = nullptr;

		// We need to access the websocket part only in shared mode: we do not change the list
		JVX_LOCK_RW_MUTEX_SHARED(safeAccessWebConnection);
		auto elm = wsConnections.find(conn);
		if (elm != wsConnections.end())
		{
			wsObject = elm->second;
		}

		if(wsObject)
		{
			std::string token;
			CjvxWebControlNodeWsEventType wsEvent = CjvxWebControlNodeWsEventType::AYF_EVENT_NONE;
			std::string transToken;

			// However, we need a secondary lock for the command lst
			wsObject->lock();
			auto tokenElm = wsObject->commands.begin();
			token = tokenElm->txt;
			wsEvent = tokenElm->eventType;
			wsObject->commands.pop_front();
			wsObject->unlock();

			if (
				(wsEvent == CjvxWebControlNodeWsEventType::AYF_EVENT_INIT_INTERPRET) ||
				(wsEvent == CjvxWebControlNodeWsEventType::AYF_EVENT_INTERPRET))
			{
				jvxApiString respTxt;
				CjvxJsonElement jelm;
				CjvxJsonElementList jelml;
				if (cmdInterpreter)
				{

					cmdInterpreter->interpret_command(token.c_str(), &respTxt, true);
					std::vector<std::string> errs;
					jvxErrorType resL = CjvxJsonElementList::stringToRepresentation(respTxt.std_str(), jelml, errs);
				}
				else
				{
					jelm.makeAssignmentString("error_code", jvxErrorType_txt(JVX_ERROR_UNSUPPORTED));
					jelml.addConsumeElement(jelm);
				}

				jelm.makeSection("out", jelml);
				jelml.addConsumeElement(jelm);

				jelm.makeAssignmentString("in", token);
				jelml.addConsumeElement(jelm);

				jelm.makeSection("cmd", jelml);
				jelml.addConsumeElement(jelm);

				jelml.printToJsonView(transToken);
				if (wsEvent == CjvxWebControlNodeWsEventType::AYF_EVENT_INIT_INTERPRET)
				{

					_common_set.theReport->interface_sub_report(&wsObject->subReport);
					if (wsObject->subReport)
					{
						wsObject->subReport->register_sub_report(static_cast<IjvxReportSystem*>(wsObject));
					}
				}
			}
			if (
				(wsEvent == CjvxWebControlNodeWsEventType::AYF_EVENT_DIRECT_EMIT))
			{
				transToken = token;
			}
			// The websocket sender has an internal lock
			webServ.cpPtr->in_connect_send_ws_packet(reinterpret_cast<struct mg_connection*>(elm->first), 0x1 /* text mode */, transToken.c_str(), transToken.size());
		}
		JVX_UNLOCK_RW_MUTEX_SHARED(safeAccessWebConnection);
	}
	else
	{
		// This request comes from the webserver part
		oneThreadWorker* threadObj = nullptr;
		JVX_LOCK_RW_MUTEX_SHARED(safeAccessWebConnection);
		auto elm = workerHdls.find(rescheduleId);
		if (elm != workerHdls.end())
		{
			if (elm->second->stat == JVX_STATE_ACTIVE)
			{
				elm->second->stat = JVX_STATE_PROCESSING;
				threadObj = elm->second;
			}
		}
		JVX_UNLOCK_RW_MUTEX_SHARED(safeAccessWebConnection);

		// This not in a lock: the state indicates that is will not be removed
		if (threadObj)
		{
			threadObj->resL = JVX_ERROR_UNSUPPORTED;

			// Run the backend command here!!
			if (cmdInterpreter)
			{
				jvxApiString respTxt;
				threadObj->resL = cmdInterpreter->interpret_command(threadObj->command.c_str(), &respTxt, true);
				threadObj->response = respTxt.std_str();
			}

			// This will release the state for the original thread!
			threadObj->stat = JVX_STATE_COMPLETE;
			JVX_SET_NOTIFICATION(threadObj->waitHdl);
			// JVX_UNLOCK_MUTEX(safeAccessWorkers);
		}
	}
	

	return JVX_NO_ERROR;
}


jvxErrorType
CjvxWebControlNode::synchronizeWebServerWsEvents(jvxHandle* context_server,
	jvxHandle* context_conn, jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
	jvxBool strictConstConnection, const char* uriprefix, int headerbyte, char* payload, size_t szFld)
{
	if (jvx_bitTest(purp, JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_SHIFT))
	{
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "Web socket connected" << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}

		JVX_LOCK_RW_MUTEX_EXCLUSIVE(safeAccessWebConnection);
		auto elm = wsConnections.find(context_conn);
		if (elm == wsConnections.end())
		{
			CjvxWebControlNodeWsHandle* newElm = nullptr;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(newElm, CjvxWebControlNodeWsHandle(webServ.cpPtr, context_conn));
			newElm->stat = JVX_STATE_SELECTED;
			wsConnections[context_conn] = newElm;
		}
		else
		{
			assert(false);
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(safeAccessWebConnection);
	}
	else if (jvx_bitTest(purp, JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_SHIFT))
	{
		JVX_LOCK_RW_MUTEX_SHARED(safeAccessWebConnection);
		auto elm = wsConnections.find(context_conn);
		if (elm != wsConnections.end())
		{
			elm->second->stat = JVX_STATE_ACTIVE;
			elm->second->lock();
			elm->second->commands.push_back(CjvxWebControlNodeWsEvent(JVX_WS_READY_COMMAND, CjvxWebControlNodeWsEventType::AYF_EVENT_INIT_INTERPRET));
			elm->second->unlock();

			// Reschedue event into main loop
			CjvxReportCommandRequest_rm mess(_common_set.theComponentType, 0,
				jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER,
				reinterpret_cast<jvxHandle*>((intptr_t)context_conn));
			_common_set.theReport->request_command(mess);
		}
		else
		{
			assert(false);
		}
		JVX_UNLOCK_RW_MUTEX_SHARED(safeAccessWebConnection);
		/*
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
		*/
	}
	else if (jvx_bitTest(purp, JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_SHIFT))
	{

		/* Handle incoming data */
		std::string inTxt;
		jvxUInt16* ptr16 = NULL;
		jvxProtocolHeader* protheader = NULL;
		jvxByte opcode = headerbyte & 0xF;
		std::map<jvxHandle*, CjvxWebControlNodeWsHandle*>::iterator elm;
		switch (opcode)
		{
		case 0x1: // Text message

			JVX_LOCK_RW_MUTEX_SHARED(safeAccessWebConnection);
			elm = wsConnections.find(context_conn);
			if (elm != wsConnections.end())
			{
				inTxt = std::string(payload, szFld);
				std::cout << "Incoming text message: <" << inTxt << ">." << std::endl;

				elm->second->lock();
				elm->second->commands.push_back(CjvxWebControlNodeWsEvent(inTxt, CjvxWebControlNodeWsEventType::AYF_EVENT_INTERPRET));
				elm->second->unlock();

				// Reschedue event into main loop
				CjvxReportCommandRequest_rm mess(_common_set.theComponentType, 0,
					jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER,
					reinterpret_cast<jvxHandle*>((intptr_t)context_conn));
				_common_set.theReport->request_command(mess);
			}
			else
			{
				assert(0);		
			}
			JVX_UNLOCK_RW_MUTEX_SHARED(safeAccessWebConnection);
			break;

		case 0x2: // Binary message

#if 0
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
#endif
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
			// this->webSocketPeriodic.current_ping_count = 0;
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
		// TjvxEventLoopElement elm;
		JVX_LOCK_RW_MUTEX_EXCLUSIVE(safeAccessWebConnection);
		auto elm = wsConnections.find(context_conn);
		if (elm != wsConnections.end())
		{
			elm->second->clear_ws_container();
			JVX_DSP_SAFE_DELETE_OBJECT(elm->second);
			wsConnections.erase(elm);
		}
		JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(safeAccessWebConnection);
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "Web socket closed" << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}

		/*
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
		*/
	}
	return JVX_NO_ERROR;
}

// in_connect_send_ws_packet

jvxErrorType
CjvxWebControlNode::hook_safeConfigFile()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControlNode::hook_startSequencer()
{
	return JVX_NO_ERROR;
}


jvxErrorType
CjvxWebControlNode::hook_stopSequencer()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControlNode::hook_operationComplete() 
{
	return JVX_NO_ERROR;
}

// If support for web sockets...
/*	virtual jvxErrorType hook_wsConnect(jvxHandle* context_server, jvxHandle* context_conn){return(JVX_ERROR_UNSUPPORTED);};
	virtual jvxErrorType hook_wsReady(jvxHandle* context_server, jvxHandle* context_conn){return(JVX_ERROR_UNSUPPORTED);};
	virtual jvxErrorType hook_wsData(jvxHandle* context_server, jvxHandle* context_conn, int header, char *payload, size_t szFld){return(JVX_ERROR_UNSUPPORTED);};
	virtual jvxErrorType hook_wsClose(jvxHandle* context_server, jvxHandle* context_conn){return(JVX_ERROR_UNSUPPORTED);};
	*/
jvxErrorType
CjvxWebControlNode::hook_ws_started()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebControlNode::hook_ws_stopped()
{
	return JVX_NO_ERROR;
}

