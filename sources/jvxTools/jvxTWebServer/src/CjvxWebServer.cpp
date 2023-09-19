#include "CjvxWebServer.h"
#include "jvx_connect_server_st.h"

#define IP_ADDRESS_CHAR_SIZE 1024

CjvxWebServer::CjvxWebServer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_WEBSERVER);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxWebServer*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);
	_common_set_min.theState = JVX_STATE_NONE;
}


CjvxWebServer::~CjvxWebServer()
{
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		//this->stop();
		stop();
	}

	if(_common_set_min.theState == JVX_STATE_INIT)
	{
		this->terminate();
	}
}

jvxErrorType 
CjvxWebServer::initialize()
{
	if(_common_set_min.theState == JVX_STATE_NONE)
	{
		genWebserver::init__properties_init();
		genWebserver::allocate__properties_init();
		genWebserver::register__properties_init(static_cast<CjvxProperties*>(this));

		_common_set_min.theState = JVX_STATE_INIT;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}
	
jvxErrorType 
CjvxWebServer::register_co_handler(jvxSize* unique_handler_id, 
									jvxCBitField purp, jvxHandle* identification, 									
									IjvxWebServerCo_report* bwd)
{

	if(_common_set_min.theState == JVX_STATE_INIT)
	{
		std::string expr = (const char*)identification;

		jvxOneCallbackEndpoint* newEndpoint = NULL;
		JVX_SAFE_NEW_OBJ(newEndpoint, jvxOneCallbackEndpoint(expr, uniqueId, bwd, purp));
		*unique_handler_id = uniqueId;
		uniqueId ++;
		coendpoints.push_back(newEndpoint);
		

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxWebServer::register_ws_handler(jvxSize* unique_handler_id,
	jvxCBitField purp, jvxHandle* identification,
	IjvxWebServerWs_report* bwd)
{

	if (_common_set_min.theState == JVX_STATE_INIT)
	{
		std::string expr = (const char*)identification;

		jvxOneWsCallbackEndpoint* newEndpoint = NULL;
		JVX_SAFE_NEW_OBJ(newEndpoint, jvxOneWsCallbackEndpoint(expr, uniqueId, bwd, purp));
		*unique_handler_id = uniqueId;
		uniqueId++;
		wsendpoints.push_back(newEndpoint);


		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxWebServer::start()
{
	jvxSize i;
	std::vector<std::string> lst;
	char whoami_name[IP_ADDRESS_CHAR_SIZE] = { 0 };
	jvx_connect_server_family* myFam = NULL;
	jvx_connect_server_if* myServ = NULL;
	std::string commandline;

	if(_common_set_min.theState == JVX_STATE_INIT)
	{
		if (!genWebserver::properties_init.wwwRootDirectory.value.empty())
		{
			commandline = "document_root ";
			commandline += genWebserver::properties_init.wwwRootDirectory.value;
		}
		if (JVX_CHECK_SIZE_SELECTED(genWebserver::properties_init.wwwListeningPort.value))
		{
			commandline += " listening_ports ";
			commandline += jvx_size2String(genWebserver::properties_init.wwwListeningPort.value);
		}
		if (JVX_CHECK_SIZE_SELECTED(genWebserver::properties_init.wwwNumberThreads.value))
		{
			commandline += " num_threads ";
			commandline += jvx_size2String(genWebserver::properties_init.wwwNumberThreads.value);
		}
//commandline += " num_threads 1";

//#ifdef VERBOSE_MODE
	
		std::cout << "** Starting webserver with options " << commandline << " **" <<std::endl;
//#endif
		
		jvx_parseCommandLineOneToken(commandline, lst);


		//const char *options[] = {"document_root", "./www", "listening_ports", "8000", 0};
		const char **options = NULL;
		JVX_SAFE_NEW_FLD(options, const char*, lst.size()+1);
		for(i = 0; i < lst.size(); i++)
		{
			options[i] = lst[i].c_str();
		}
		options[i] = NULL;

		myServer = NULL;
		JVX_SAFE_NEW_OBJ(myServer, CivetServer(options));
		assert(myServer);

		for(i = 0; i < coendpoints.size(); i++)
		{
			if((jvx_cbitTest(coendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_GET_HANDLER_SHIFT)) ||
				(jvx_cbitTest(coendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_POST_HANDLER_SHIFT)) ||
				(jvx_cbitTest(coendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_PUT_HANDLER_SHIFT)))
			{
				myServer->addHandler(coendpoints[i]->reg_expression_cb.c_str(), coendpoints[i]);
				//myServer->addRequestHandler(endpoints[i]->reg_expression_cb.c_str(), endpoints[i]);
			}
			
		}

		for (i = 0; i < wsendpoints.size(); i++)
		{
			if (
				(jvx_cbitTest(wsendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_SHIFT)) ||
				(jvx_cbitTest(wsendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_SHIFT)) ||
				(jvx_cbitTest(wsendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_SHIFT)) ||
				(jvx_cbitTest(wsendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_SHIFT))
				)
			{
				myServer->addWebSocketHandler(wsendpoints[i]->reg_expression_cb.c_str(), wsendpoints[i]);
				//myServer->addWebsocketHandler(endpoints[i]->reg_expression_cb.c_str(), endpoints[i]);
			}
		}
		 
		jvx_connect_server_family_initialize_st(&myFam, JVX_CONNECT_SOCKET_TYPE_TCP);
		assert(myFam);
		jvx_connect_server_initialize_st(myFam, &myServ, NULL, NULL, 0, 0);
		assert(myServ);
		jvx_connect_server_whoami_st(myServ, whoami_name, IP_ADDRESS_CHAR_SIZE, JVX_CONNECT_SOCKET_NAME_ALL_IP_ADDR);
		std::cout << "** Ip Address: " << whoami_name << " **" << std::endl;
		jvx_connect_server_terminate_st(myServ);
		myServ = NULL;
		jvx_connect_server_family_terminate_st(myFam);
		myFam = NULL;

		_common_set_min.theState = JVX_STATE_PROCESSING;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

void
CjvxWebServer::in_connect_url_decode(const char* inPtr, jvxApiString& onReturn)
{
	if (inPtr)
	{
		std::string inStr = inPtr;
		if (inStr.size())
		{
			char* tmp = nullptr;
			JVX_SAFE_ALLOCATE_FIELD_CPP_Z(tmp, char, inStr.size()+1);
			mg_url_decode(inStr.c_str(), inStr.size(), tmp, inStr.size()+1, 1);
			inStr = std::string(tmp, inStr.size());
			JVX_DSP_SAFE_DELETE_FIELD(tmp);
		}
		onReturn = inStr;
	}
}
jvxErrorType 
CjvxWebServer::in_connect_request_specifiers(jvxHandle* server, jvxHandle* conn,
	jvxApiString* request_uri_on_return, jvxApiString* query_on_return,
	jvxApiString* local_uri_on_return, jvxApiString* origin_request_on_return,
	jvxApiString* origin_user_on_return)
{
	const char* ret = NULL;
	
	std::string local_uri;
	std::string request_uri;
	std::string queri_uri;
	std::string origin_request;
	std::string origin_user;

	const char* queryRet = NULL;
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		mg_connection* theConnection = reinterpret_cast<mg_connection*>(conn);
		ret = mg_get_request_info(theConnection)->local_uri;
		if (ret)
		{
			local_uri = ret;
		}

		ret = mg_get_request_info(theConnection)->request_uri;
		if (ret)
		{
			request_uri = ret;
		}

		ret = mg_get_request_info(theConnection)->query_string;
		if (ret)
		{						
			queri_uri = ret;			
		}

		ret = mg_get_request_info(theConnection)->remote_addr;
		if (ret)
		{
			origin_request = ret;
		}

		ret = mg_get_request_info(theConnection)->remote_user;
		if (ret)
		{
			origin_user = ret;
		}

#ifdef VERBOSE_MODE
		std::cout << "REQUEST URL: " << local_uri << ":" << request_uri << ":" << queri_uri << ":" << origin_request << ":" << origin_user << std::endl;
#endif
		if (request_uri_on_return)
		{
			request_uri_on_return->assign(request_uri);
		}
		if (query_on_return)
		{
			query_on_return->assign(queri_uri);
		}

		if (local_uri_on_return)
		{
			local_uri_on_return->assign(local_uri);
		}
		if (origin_request_on_return)
		{
			origin_request_on_return->assign(origin_request);
		}
		if (origin_user_on_return)
		{
			origin_user_on_return->assign(origin_user);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxWebServer::in_connect_write_response(jvxHandle* server, jvxHandle* conn, const char* response_text)
{
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		mg_connection* theConnection = reinterpret_cast<mg_connection*>(conn);
		
#ifdef VERBOSE_MODE
		std::cout << response_text << std::flush;
#endif

		mg_printf(theConnection, "%s", response_text);

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}
	
jvxErrorType 
CjvxWebServer::in_connect_write_header_response(jvxHandle* server, jvxHandle* conn, jvxWebServerResponseType resp)
{
	jvxErrorType res = JVX_NO_ERROR;

	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
#ifdef VERBOSE_MODE
		std::cout << "RETURN CONTENT: "<< std::endl;
#endif

		std::string response = "";
		switch(resp)
		{
		case JVX_WEB_SERVER_RESPONSE_JSON:
			response = "HTTP/1.1 200 OK\r\nContent-Type: ";
			response += "text/json; charset=utf-8\r\nConnection: close\r\n\r\n";
			
			break;
		default:
			res = JVX_ERROR_UNSUPPORTED;
		}
		if(res == JVX_NO_ERROR)
		{
			in_connect_write_response(server, conn, response.c_str());
		}
		return(res);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxWebServer::in_connect_write_packet_websocket(jvxHandle* server, jvxHandle* conn, int opcode, const jvxHandle* dField, jvxSize szField)
{
	jvxErrorType res = JVX_NO_ERROR;

	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
#ifdef VERBOSE_MODE
		std::cout << "WEB SOCKET WRITE: "<< std::endl;
#endif
		mg_websocket_write(reinterpret_cast<struct mg_connection *>(conn), opcode, reinterpret_cast<const char*>(dField), szField);
		return(res);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxWebServer::in_connect_read_content(jvxHandle* server, jvxHandle* conn, jvxHandle* readFld, jvxSize* numBytes)
{
	jvxErrorType res = JVX_NO_ERROR;
	int retVal;

	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		mg_connection* theConnection = reinterpret_cast<mg_connection*>(conn);

		retVal = mg_read(theConnection, readFld, *numBytes);
		*numBytes = retVal;
		if(retVal <= 0)
		{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		}
		else
		{
#ifdef VERBOSE_MODE
			std::cout << "READ CONTENT: " << (const char*)readFld << std::endl;
#endif
		}

		return(res);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxWebServer::in_connect_extract_var_content(const char* fldStringIn, jvxSize numBytesIn, const char* var_name, char* readFldOut, jvxSize* numBytesOut)
{
	jvxErrorType res = JVX_NO_ERROR;
	int retVal;

	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		retVal = mg_get_var(fldStringIn, numBytesIn, var_name, readFldOut, *numBytesOut);
		*numBytesOut = retVal;
		switch(retVal)
		{
		case -1:
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
			break;
		case -2:
			res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		default:
			break;
		}
		return(res);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxWebServer::in_connect_url_decode(const char *src, int src_len, jvxApiString* result)
{
	jvxErrorType res = JVX_NO_ERROR;
	int retVal;
	char token[JVX_MAXSTRING];
	std::string out;

	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		memset(token, 0, JVX_MAXSTRING);

		retVal = mg_url_decode(src, src_len, token, JVX_MAXSTRING-1, 1);

		if(retVal >= 0)
		{
			out = token;
			if (result)
			{
				result->assign(out);
			}
		}
		else
		{
			res = JVX_ERROR_INTERNAL;
		}
		return(res);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxWebServer::in_connect_url_encode(const char *src, int src_len, jvxApiString* result)
{
	jvxErrorType res = JVX_NO_ERROR;
	int retVal;
	char token[JVX_MAXSTRING];
	std::string out;

	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		memset(token, 0, JVX_MAXSTRING);

		retVal = mg_url_encode(src, token, JVX_MAXSTRING - 1);

		if (retVal >= 0)
		{
			out = token;
			if (result)
			{
				result->assign(out);
			}
		}
		else
		{
			res = JVX_ERROR_INTERNAL;
		}
		return(res);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxWebServer::in_connect_write_error(jvxHandle* conn, int errCode, const char* token)
{
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		mg_connection* theConnection = reinterpret_cast<mg_connection*>(conn);
		int retVal = mg_send_http_error(theConnection, errCode, "%s", token);
		if (retVal < 0)
		{
			return JVX_ERROR_INTERNAL;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxWebServer::unregister_handler(jvxSize unique_handler_id)
{
	if(_common_set_min.theState == JVX_STATE_INIT)
	{
		std::vector<jvxOneCallbackEndpoint*>::iterator elm = coendpoints.begin();
		for(; elm != coendpoints.end(); elm++)
		{
			if((*elm)->reg_id == unique_handler_id)
			{
				JVX_SAFE_DELETE_OBJ(*elm);
				break;
			}
		}
		if(elm != coendpoints.end())
		{
			coendpoints.erase(elm);
		}

		std::vector<jvxOneWsCallbackEndpoint*>::iterator elmws = wsendpoints.begin();
		for (; elmws != wsendpoints.end(); elmws++)
		{
			if ((*elmws)->reg_id == unique_handler_id)
			{
				JVX_SAFE_DELETE_OBJ(*elmws);
				break;
			}
		}
		if (elmws != wsendpoints.end())
		{
			wsendpoints.erase(elmws);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/*
 * No longer needed
jvxErrorType 
CjvxWebServer::trigger_stop_single_websocket(jvxHandle* server, jvxHandle* conn )
{
	jvxSize i;
	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		mg_connection* theConnection = reinterpret_cast<mg_connection*>(conn);
		//myServer->webSocketCloseHandler(theConnection, )
		//mg_close_connection(theConnection);
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}
*/

/*
jvxErrorType 
CjvxWebServer::stop()
{
	jvxSize i;
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		for(i = 0; i < endpoints.size(); i++)
		{
			if((jvx_cbitTest(endpoints[i]->purp_cb, JVX_WEB_SERVER_URI_GET_HANDLER_SHIFT)) || (jvx_cbitTest(endpoints[i]->purp_cb, JVX_WEB_SERVER_URI_POST_HANDLER_SHIFT)))
			{
				myServer->removeRequestHandler(endpoints[i]->reg_expression_cb);
			}
			if(
				(jvx_cbitTest(endpoints[i]->purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_SHIFT)) ||
				(jvx_cbitTest(endpoints[i]->purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_SHIFT)) ||
				(jvx_cbitTest(endpoints[i]->purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_SHIFT)) ||
				(jvx_cbitTest(endpoints[i]->purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_SHIFT))
				)			
			{
				myServer->removeWebsocketHandler(endpoints[i]->reg_expression_cb);
			}
		}
		JVX_SAFE_DELETE_OBJ(myServer);
		myServer = NULL;
		_common_set_min.theState = JVX_STATE_INIT;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}
*/

/*
 * I think no longer used
jvxErrorType 
CjvxWebServer::trigger_stop()
{
	jvxSize i;
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		//myServer->triggerStop();		
		_common_set_min.theState = JVX_STATE_ACTIVE;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}
*/

jvxErrorType 
CjvxWebServer::stop()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		//if(!myServer->waitStopComplete())
		//{
		//	return(JVX_ERROR_NOT_READY);
		//}

		for(i = 0; i < coendpoints.size(); i++)
		{
			if((jvx_cbitTest(coendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_GET_HANDLER_SHIFT)) || 
				(jvx_cbitTest(coendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_POST_HANDLER_SHIFT)))
			{
				myServer->removeHandler(coendpoints[i]->reg_expression_cb);
			}
		}

		for (i = 0; i < wsendpoints.size(); i++)
		{
			if (
				(jvx_cbitTest(wsendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_SHIFT)) ||
				(jvx_cbitTest(wsendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_SHIFT)) ||
				(jvx_cbitTest(wsendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_SHIFT)) ||
				(jvx_cbitTest(wsendpoints[i]->purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_SHIFT))
				)
			{
				myServer->removeWebSocketHandler(wsendpoints[i]->reg_expression_cb);
			}
		}

		JVX_SAFE_DELETE_OBJ(myServer);
		myServer = NULL;
		_common_set_min.theState = JVX_STATE_INIT;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxWebServer::terminate()
{
	jvxSize i;
	//stop();
	stop();

	if(_common_set_min.theState == JVX_STATE_INIT)
	{
		genWebserver::unregister__properties_init(static_cast<CjvxProperties*>(this));
		genWebserver::deallocate__properties_init();

		for(i = 0; i < coendpoints.size(); i++)
		{
			JVX_SAFE_DELETE_OBJ(coendpoints[i]);
		}
		coendpoints.clear();

		for (i = 0; i < wsendpoints.size(); i++)
		{
			JVX_SAFE_DELETE_OBJ(wsendpoints[i]);
		}
		wsendpoints.clear();

		_common_set_min.theState = JVX_STATE_NONE;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxWebServer::in_connect_send_ws_packet(struct mg_connection* Connection, jvxInt32 Opcode, const char* Data, jvxSize data_len)
{
	jvxSize numSent = mg_websocket_write(Connection, Opcode, Data, data_len);
	if (numSent == 0)
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	else if (numSent == data_len)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INTERNAL;
}

jvxErrorType 
CjvxWebServer::in_connect_drop_connection(struct mg_connection* Connection)
{
	mg_close_connection(Connection);
	// mg_stop(myServer->context);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxWebServer::in_connect_get_ws_ip_addr(struct mg_connection* Connection, jvxApiString* astr)
{
	const struct mg_request_info *request_info = mg_get_request_info(Connection);
	if (astr)
		astr->assign(request_info->remote_addr);
		
	return JVX_NO_ERROR;
}
