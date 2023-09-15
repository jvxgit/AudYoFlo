#include "CjvxTextWebSockets.h"
#include "CjvxWebControl_fe.h"
#include "CjvxWebControl_fe_private.h"

void
CjvxTextWebSockets::initialize(CjvxWebControl_fe* hostrefArg)
{
	hostRef = hostrefArg;
}

jvxErrorType
CjvxTextWebSockets::register_text_socket_main_loop(jvxWebContext* ctxt)

{
	jvxApiString uri;
	jvxApiString query;
	jvxApiString local_uri;
	jvxApiString url_origin;
	jvxApiString user;
	std::cout << "Text web socket connect" << std::endl;
	jvxWsTextConnection newConnection;
	newConnection.theCtxt = *ctxt;

	auto elm = connectedTextSockets.find(newConnection.theCtxt.context_conn);
	if (elm == connectedTextSockets.end())
	{
		// Get the originating IP address 
		hostRef->request_event_information(newConnection.theCtxt.context_server,
			newConnection.theCtxt.context_conn,
			&uri, &query, &local_uri, &url_origin, &user);
		newConnection.theCtxt.uri = uri.std_str();
		newConnection.query = query.std_str();
		newConnection.local_uri = local_uri.std_str();
		newConnection.url_origin = url_origin.std_str();
		newConnection.user = user.std_str();
		newConnection.status = ayfTextWebSocketState::AYF_WEBSOCKET_NONE;

		JVX_START_LOCK_LOG_REF(hostRef, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << "Web text socket connection from " << url_origin.std_str() << ":" << user.std_str() << std::endl;
		JVX_STOP_LOCK_LOG_REF(hostRef);
		connectedTextSockets[newConnection.theCtxt.context_conn] = newConnection;
		return JVX_NO_ERROR;
	}
	JVX_START_LOCK_LOG_REF(hostRef, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
	log << "Web text socket connection with handle " << newConnection.theCtxt.context_conn << " was already registered before!" << std::endl;
	JVX_STOP_LOCK_LOG_REF(hostRef);
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CjvxTextWebSockets::unregister_text_socket_main_loop(jvxHandle* refCtxt)
{
	// ========================================================
	// Web socket disconnect
	// ========================================================
	std::cout << "Text web socket disconnect" << std::endl;
	auto elm = connectedTextSockets.find(refCtxt);
	if (elm != connectedTextSockets.end())
	{
		JVX_START_LOCK_LOG_REF(hostRef, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << "Connected web text socket connection from " << elm->second.url_origin << ":" << elm->second.user << " successfully disconnected." << std::endl;
		JVX_STOP_LOCK_LOG_REF(hostRef);
		connectedTextSockets.erase(elm);
		return JVX_NO_ERROR;
	}
	JVX_START_LOCK_LOG_REF(hostRef, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
	log << "Web text socket connection with handle " << refCtxt << " was not registered before!" << std::endl;
	JVX_STOP_LOCK_LOG_REF(hostRef);
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxTextWebSockets::process_incoming_text_message(jvxOneWsTextRequest* mess)
{
	auto elmL = connectedTextSockets.find(mess->ctxt);
	if (elmL != connectedTextSockets.end())
	{
		if (elmL->second.status == ayfTextWebSocketState::AYF_WEBSOCKET_NONE)
		{
			std::string cmd;
			std::string ctxt;
			std::vector<std::string> token;
			jvx_parseCommandLineOneToken(mess->cmd, token, '#');
			if (token.size() > 0)
			{
				cmd = token[0];
			}
			if (token.size() > 1)
			{
				ctxt = token[1];
			}
			if (cmd == "AYF_WS_HOST_CONNECT")
			{
				jvxApiString cmd = "show(system);";
				// std::cout << __FUNCTION__ << " entering blocking loop" << std::endl;
				TjvxEventLoopElement elm;
				CjvxWebControl_fe::oneThreadReturnType myPrivateMemReturn;
				TjvxEventLoopElementSync oneHdlBlock;
				myPrivateMemReturn.res_mthread = JVX_ERROR_UNSUPPORTED;
				myPrivateMemReturn.ret_mthread = "";
				myPrivateMemReturn.ctx_mthread = ctxt;

				hostRef->evLop->reserve_message_id(&elm.message_id);

				// Transfer command to the primary backend

				elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(hostRef);
				elm.priv_fe = NULL;
				elm.target_be = hostRef->linkedPriBackend.be;
				elm.priv_be = NULL;

				elm.param = (jvxHandle*)(&cmd);
				elm.paramType = JVX_EVENTLOOP_DATAFORMAT_JVXAPISTRING;

				elm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
				elm.eventClass = JVX_EVENTLOOP_REQUEST_IMMEDIATE;
				elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
				elm.delta_t = JVX_SIZE_UNSELECTED;
				elm.autoDeleteOnProcess = c_false;

				jvxErrorType res = hostRef->evLop->event_schedule(&elm, NULL, &myPrivateMemReturn);
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

				jvxErrorType rresL = hostRef->myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)(elmL->first), JVX_WEBSOCKET_OPCODE_TEXT,
					(const char*)myPrivateMemReturn.ret_mthread.c_str(), myPrivateMemReturn.ret_mthread.size());
				elmL->second.status = ayfTextWebSocketState::AYF_WEBSOCKET_READY;
			}
			else
			{

			}
		}
	}

	return JVX_NO_ERROR;
}