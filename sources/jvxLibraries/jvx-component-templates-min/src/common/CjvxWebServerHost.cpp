#include "common/CjvxWebServerHost.h"

CjvxWebServerHost::CjvxWebServerHost()
{
	hdl = NULL;
	/*
	for(i = 0; i < JVX_NUMBER_WEBSERVER_CO_HOST_DEFINITIONS; i++)
	{
		notifyCoIds.theIds[i] = -1;
	}
	for (i = 0; i < JVX_NUMBER_WEBSERVER_WS_HOST_DEFINITIONS; i++)
	{
		notifyWsIds.theIds[i] = -1;
	}*/
	hostHdl = NULL;
	theHooks = NULL;
	theWebserver.uniqueId = 1;
};

void
CjvxWebServerHost::right_before_start()
{

}

jvxErrorType
CjvxWebServerHost::start_webserver(IjvxWebServer* server, IjvxHost* hostRef,
	IjvxWebServerHost_hooks* theHooksPass)
{
	jvxSize i;
	jvxSize j;
	oneEntryReference entry;

	jvxSize Uid = 0;
	jvxErrorType resL = JVX_NO_ERROR;
	if ((hdl == NULL) && (server))
	{
		hdl = server;
		hostHdl = hostRef;

		resL = hdl->initialize();

		if (resL == JVX_NO_ERROR)
		{
			// Set synchronous reference
			theHooks = theHooksPass;

			// Regular expression match
			for (i = 0; i < JVX_NUMBER_WEBSERVER_CO_HOST_DEFINITIONS; i++)
			{
				entry.identify = i;
				std::string expressionSeed = theJvxWebServerCoHostDefinitions[i].expression;
				std::string token1 = expressionSeed;
				std::string token2 = "";
				std::string theComponentName;
				size_t replace_start = expressionSeed.find(JVX_HTTP_REPLACE_TOKEN);
				if (replace_start != std::string::npos)
				{
					token1 = expressionSeed.substr(0, replace_start);
					token2 = expressionSeed.substr(replace_start + (std::string(JVX_HTTP_REPLACE_TOKEN)).size(), std::string::npos);
				}

				switch (theJvxWebServerCoHostDefinitions[i].repl)
				{
				case JVX_WEB_SERVER_REPLACE_NONE:

					std::cout << "Register CO URI " << expressionSeed << "[" << std::flush;
					if (theJvxWebServerCoHostDefinitions[i].purp & JVX_WEB_SERVER_URI_PUT_HANDLER_PURP)
					{
						std::cout << "Pu" << std::flush;
					}
					if (theJvxWebServerCoHostDefinitions[i].purp & JVX_WEB_SERVER_URI_GET_HANDLER_PURP)
					{
						std::cout << "G" << std::flush;
					}
					if (theJvxWebServerCoHostDefinitions[i].purp & JVX_WEB_SERVER_URI_POST_HANDLER_PURP)
					{
						std::cout << "Po" << std::flush;
					}
					std::cout << "]" << std::endl;

					resL = hdl->register_co_handler(&Uid,
						theJvxWebServerCoHostDefinitions[i].purp,
						(jvxHandle*)expressionSeed.c_str(),
						static_cast<IjvxWebServerCo_report*>(this));
					assert(resL == JVX_NO_ERROR);

					entry.linkToHelper = JVX_SIZE_UNSELECTED;
					notifyCoIds.theIds[Uid] = entry;
					break;

				case JVX_WEB_SERVER_REPLACE_COMPONENT_TYPE:
					for (j = JVX_COMPONENT_UNKNOWN + 1; j < JVX_COMPONENT_ALL_LIMIT; j++)
					{
						theComponentName = token1 + jvxComponentType_txt(j) + token2;
						theComponentName = jvx_replaceCharacter(theComponentName, ' ', '_');

						std::cout << "Register CO URI " << theComponentName << "[" << std::flush;
						if (theJvxWebServerCoHostDefinitions[i].purp & JVX_WEB_SERVER_URI_PUT_HANDLER_PURP)
						{
							std::cout << "Pu" << std::flush;
						}
						if (theJvxWebServerCoHostDefinitions[i].purp & JVX_WEB_SERVER_URI_GET_HANDLER_PURP)
						{
							std::cout << "G" << std::flush;
						}
						if (theJvxWebServerCoHostDefinitions[i].purp & JVX_WEB_SERVER_URI_POST_HANDLER_PURP)
						{
							std::cout << "Po" << std::flush;
						}
						std::cout << "]" << std::endl;

						resL = hdl->register_co_handler(&Uid,
							theJvxWebServerCoHostDefinitions[i].purp,
							(jvxHandle*)theComponentName.c_str(),
							static_cast<IjvxWebServerCo_report*>(this));
						assert(resL == JVX_NO_ERROR);

						entry.linkToHelper = j;
						notifyCoIds.theIds[Uid] = entry;
					}
					break;
				default:
					assert(0);
					break;
				}
			}


			// Regular expression match
			for (i = 0; i < JVX_NUMBER_WEBSERVER_WS_HOST_DEFINITIONS; i++)
			{
				entry.identify = i;
				std::string expressionSeed = theJvxWebServerWsHostDefinitions[i].expression;
				std::string token1 = expressionSeed;
				std::string token2 = "";
				std::string theComponentName;
				size_t replace_start = expressionSeed.find(JVX_HTTP_REPLACE_TOKEN);
				if (replace_start != std::string::npos)
				{
					token1 = expressionSeed.substr(0, replace_start);
					token2 = expressionSeed.substr(replace_start + (std::string(JVX_HTTP_REPLACE_TOKEN)).size(), std::string::npos);
				}

				switch (theJvxWebServerWsHostDefinitions[i].repl)
				{
				case JVX_WEB_SERVER_REPLACE_NONE:

					std::cout << "Register WS URI " << expressionSeed << "[" << std::flush;
					if (theJvxWebServerWsHostDefinitions[i].purp & JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_PURP)
					{
						std::cout << "Co" << std::flush;
					}
					if (theJvxWebServerWsHostDefinitions[i].purp & JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_PURP)
					{
						std::cout << "R" << std::flush;
					}
					if (theJvxWebServerWsHostDefinitions[i].purp & JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_PURP)
					{
						std::cout << "D" << std::flush;
					}
					if (theJvxWebServerWsHostDefinitions[i].purp & JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_PURP)
					{
						std::cout << "Cl" << std::flush;
					}
					std::cout << "]" << std::endl;

					resL = hdl->register_ws_handler(&Uid,
						theJvxWebServerWsHostDefinitions[i].purp,
						(jvxHandle*)expressionSeed.c_str(),
						static_cast<IjvxWebServerWs_report*>(this));
					assert(resL == JVX_NO_ERROR);

					entry.linkToHelper = JVX_SIZE_UNSELECTED;
					notifyWsIds.theIds[Uid] = entry;
					break;

				case JVX_WEB_SERVER_REPLACE_COMPONENT_TYPE:
					for (j = JVX_COMPONENT_UNKNOWN + 1; j < JVX_COMPONENT_ALL_LIMIT; j++)
					{
						theComponentName = token1 + jvxComponentType_txt(j) + token2;
						theComponentName = jvx_replaceCharacter(theComponentName, ' ', '_');

						std::cout << "Register WS URI " << theComponentName << "[" << std::flush;
						if (theJvxWebServerWsHostDefinitions[i].purp & JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_PURP)
						{
							std::cout << "Co" << std::flush;
						}
						if (theJvxWebServerWsHostDefinitions[i].purp & JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_PURP)
						{
							std::cout << "R" << std::flush;
						}
						if (theJvxWebServerWsHostDefinitions[i].purp & JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_PURP)
						{
							std::cout << "D" << std::flush;
						}
						if (theJvxWebServerWsHostDefinitions[i].purp & JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_PURP)
						{
							std::cout << "Cl" << std::flush;
						}
						std::cout << "]" << std::endl;


						resL = hdl->register_ws_handler(&Uid,
							theJvxWebServerWsHostDefinitions[i].purp,
							(jvxHandle*)theComponentName.c_str(),
							static_cast<IjvxWebServerWs_report*>(this));
						assert(resL == JVX_NO_ERROR);

						entry.linkToHelper = j;
						notifyWsIds.theIds[Uid] = entry;
					}
					break;
				default:
					assert(0);
					break;
				}
			}

			// Do  latest adaptations
			right_before_start();

			resL = hdl->start();
		}
	}
	return resL;
};

jvxErrorType CjvxWebServerHost::stop_webserver()
{
	jvxErrorType resL = JVX_NO_ERROR;
	if (hdl)
	{
		resL = hdl->stop();
		// Typically, stop() should have been called before

		resL = hdl->terminate();
		assert(resL == JVX_NO_ERROR);

		hdl = NULL;
	}
	return resL;
};

jvxErrorType
CjvxWebServerHost::report_server_co_event(
	jvxHandle* context_server, jvxHandle* context_conn,
	jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
	jvxBool strictConstConnection, const char* uriprefix)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theHooks)
	{
		res = theHooks->synchronizeWebServerCoEvents(context_server, context_conn, purp, uniqueId,
			strictConstConnection, uriprefix);
	}
	else
	{
		res = report_event_request_core(context_server, context_conn, purp, uniqueId,
			strictConstConnection, uriprefix, 0, NULL, 0);
	}
	return(res);
};

jvxErrorType
CjvxWebServerHost::report_server_ws_event(
	jvxHandle* context_server, jvxHandle* context_conn,
	jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
	jvxBool strictConstConnection, const char* uriprefix, int header, char* payload, size_t szFld)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theHooks)
	{
		res = theHooks->synchronizeWebServerWsEvents(context_server, context_conn, purp, uniqueId,
			strictConstConnection, uriprefix, header, payload, szFld);
	}
	else
	{
		res = report_event_request_core(context_server, context_conn, purp, uniqueId,
			strictConstConnection, uriprefix, header, payload, szFld);
	}
	return(res);
};

jvxErrorType
CjvxWebServerHost::report_event_request_core(
	jvxHandle* context_server, jvxHandle* context_conn,
	jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
	jvxBool strictConstConnection, const char* uriprefix, int header, char* payload, size_t szFld)

{
	jvxSize identify = JVX_SIZE_UNSELECTED;
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxApiString  uri;
	jvxApiString  query;
	char oneToken[JVX_MAXSTRING];

	std::string response;
	std::string in_params;
	std::string purpose;
	std::string comp_target;
	std::string descriptor;
	std::string stateSwitch;
	std::string value;

	std::string contentonly = "no";
	jvxComponentIdentification cpType;
	jvxSize sz;
	jvxStateSwitch operTp = JVX_STATE_SWITCH_NONE;
	jvxInt32 idx = -1;
	std::string url;
	std::string url_query;

	hdl->in_connect_request_specifiers(context_server, context_conn, &uri, &query, NULL, NULL, NULL);
	url = uri.std_str();
	url_query = query.std_str();

	identify = notifyCoIds.theIds[uniqueId].identify;

	switch (purp)
	{
	case JVX_WEB_SERVER_URI_GET_HANDLER_PURP:

		resL = JVX_NO_ERROR;
		while (resL == JVX_NO_ERROR)
		{
			memset(oneToken, 0, JVX_MAXSTRING);
			sz = JVX_MAXSTRING - 1;
			resL = hdl->in_connect_read_content(context_server, context_conn, oneToken, &sz);
			in_params += oneToken;
		}

		//std::cout << "Get request on " << url << ", query:" << url_query << ", content: " << in_params << std::endl;

		switch (identify)
		{
		case 0:

			// Address "/jvx/host/commands/"
			break;
		case 1:

			// Address "/jvx/host/components/" JVX_HTTP_REPLACE_TOKEN
			break;

		case 2:

			// Address  "/jvx/host/sequencer/"
			break;

		case 3:

			// Address "/jvx/host/connections/"
			break;
		}

		break;

	case JVX_WEB_SERVER_URI_PUT_HANDLER_PURP:

		resL = JVX_NO_ERROR;
		while (resL == JVX_NO_ERROR)
		{
			memset(oneToken, 0, JVX_MAXSTRING);
			sz = JVX_MAXSTRING - 1;
			resL = hdl->in_connect_read_content(context_server, context_conn, oneToken, &sz);
			in_params += oneToken;
		}

		// std::cout << "Put request on " << url << ", query:" << url_query << ", content: " << in_params << std::endl;

		switch (identify)
		{
		case 0:

			// Address "/jvx/host/commands/"
			break;
		case 1:

			// Address "/jvx/host/components/" JVX_HTTP_REPLACE_TOKEN
			break;

		case 2:

			// Address  "/jvx/host/sequencer/"
			break;

		case 3:

			// Address "/jvx/host/connections/"
			break;
		}

		break;

	case JVX_WEB_SERVER_URI_POST_HANDLER_PURP:

		resL = JVX_NO_ERROR;
		while (resL == JVX_NO_ERROR)
		{
			memset(oneToken, 0, JVX_MAXSTRING);
			sz = JVX_MAXSTRING - 1;
			resL = hdl->in_connect_read_content(context_server, context_conn, oneToken, &sz);
			in_params += oneToken;
		}

		// std::cout << "Post request on " << url << ", query:" << url_query << ", content: " << in_params << std::endl;

		if (!in_params.empty())
		{
#if 0
			purpose = this->extractStringParameters(in_params, "purpose");
			if (uniqueId == notifyCoIds.theIds[0])
			{
				// Address /jvx/host/general/
				if (purpose == "state")
				{
					this->response_host__state(context_server, context_conn);
					break;
				}
				if (purpose == "safe_config")
				{
					this->response_host__safe_config(context_server, context_conn);
					break;
				}
				std::cout << "Failed to match input parameters " << in_params << "!" << std::endl;
			}
			else if (uniqueId == notifyCoIds.theIds[1])
			{
				// Address /jvx/host/components/
				comp_target = extractStringParameters(in_params, "component_target");
				componentTargetStringToComponentType(comp_target, cpType);
				if (purpose == "description")
				{
					this->response_components__description(context_server, context_conn, cpType);
					break;
				}
				if (purpose == "state")
				{
					this->response_components__state(context_server, context_conn, cpType);
					break;
				}
				if (purpose == "switch_state")
				{
					stateSwitch = extractStringParameters(in_params, "operation");
					operationStringToStateSwitchType(stateSwitch, operTp);
					this->response_components__switch_state(context_server, context_conn, cpType, operTp, idx);
					break;
				}
				std::cout << "Failed to match input parameters " << in_params << "!" << std::endl;
			}
			else if (uniqueId == notifyCoIds.theIds[2])
			{
				// Address /jvx/host/sequencer/
				if (purpose == "state")
				{
					this->response_sequencer__start(context_server, context_conn);
					break;
				}
				if (purpose == "start")
				{
					this->response_sequencer__start(context_server, context_conn);
					break;
				}
				if (purpose == "stop")
				{
					this->response_sequencer__stop(context_server, context_conn);
					break;
				}
				std::cout << "Failed to match input parameters " << in_params << "!" << std::endl;
			}
			else if (uniqueId == notifyCoIds.theIds[3])
			{
				comp_target = extractStringParameters(in_params, "component_target");
				componentTargetStringToComponentType(comp_target, cpType);

				if (purpose == "description")
				{
					this->response_properties__description(context_server, context_conn, cpType);
					break;
				}
				if (purpose == "get-content")
				{
					descriptor = extractStringParameters(in_params, "property_descriptor");
					contentonly = extractStringParameters(in_params, "content_only");
					this->response_properties__get_content(context_server, context_conn, cpType, descriptor, (contentonly == "yes"));
					break;
				}
				if (purpose == "set-content")
				{
					descriptor = extractStringParameters(in_params, "property_descriptor");
					contentonly = extractStringParameters(in_params, "content_only");
					value = extractStringParameters(in_params, "value");
					this->response_properties__set_content(context_server, context_conn, cpType, descriptor, value);
				}
				std::cout << "Failed to match input parameters " << in_params << "!" << std::endl;
			}
			else
			{
				// Unmatched
			}
#endif
		}
		break;
	case JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_PURP:
		res = hook_wsConnect(context_server, context_conn);
		break;
	case JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_PURP:
		res = hook_wsData(context_server, context_conn, header, payload, szFld);
		break;
	case JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_PURP:
		res = hook_wsReady(context_server, context_conn);
		break;
	case JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_PURP:
		res = hook_wsClose(context_server, context_conn);
		break;
	default:
		assert(0);
	}
	return(JVX_NO_ERROR);
};

jvxErrorType
CjvxWebServerHost::request_event_information(
	jvxHandle* context_server, jvxHandle* context_conn, jvxApiString* uri,
	jvxApiString* query, jvxApiString* local_uri, jvxApiString* origin, jvxApiString* user)
{
	hdl->in_connect_request_specifiers(context_server, context_conn, uri, query, local_uri, origin, user);
	return(JVX_NO_ERROR);
};

jvxErrorType
CjvxWebServerHost::report_event_request_translate(
	jvxHandle* context_server, jvxHandle* context_conn,
	jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
	jvxBool strictConstConnection, const char* uriprefix, int header,
	char* payload_ws, size_t szFld,
	std::string& command, jvxBool& requiresInterpretation,
	jvxBool& errorDetected, std::ostream* os, jvx_lock* jvxlock, jvxBool silentMode)

{
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxApiString  uri;
	jvxApiString  query;
	jvxApiString local_uri;
	jvxApiString origin;
	jvxApiString user;
	char oneToken[JVX_MAXSTRING];
	std::vector<std::string> query_list;
	std::string response;
	std::string in_params;
	std::string purpose;
	std::string comp_target;
	std::string descriptor;
	std::string stateSwitch;
	std::string value;
	std::string suburl;
	std::string contentonly = "no";
	jvxComponentIdentification cpType;
	jvxSize sz;
	jvxStateSwitch operTp = JVX_STATE_SWITCH_NONE;
	jvxInt32 idx = -1;
	std::string url;
	std::string url_query;
	oneEntryReference theRef;
	jvxSize identify = JVX_SIZE_UNSELECTED;
	std::string token;
	requiresInterpretation = false;
	errorDetected = false;
	jvxBool isErr = false;
	jvxBool contentOnly = false;
	jvxSize offset = 0;
	jvxSize num_elements = JVX_SIZE_UNSELECTED;
	std::string context_token;
	std::string pretoken;
	hdl->in_connect_request_specifiers(context_server, context_conn, &uri, &query, &local_uri, &origin, &user);
	url = uri.std_str();
	url_query = query.std_str();

	theRef = notifyCoIds.theIds[uniqueId];
	identify = notifyCoIds.theIds[uniqueId].identify;

	switch (purp)
	{
	case JVX_WEB_SERVER_URI_GET_HANDLER_PURP:

		resL = JVX_NO_ERROR;
		while (resL == JVX_NO_ERROR)
		{
			memset(oneToken, 0, JVX_MAXSTRING);
			sz = JVX_MAXSTRING - 1;
			resL = hdl->in_connect_read_content(context_server, context_conn, oneToken, &sz);
			in_params += oneToken;
		}

		if (os)
		{
			if (jvxlock)
			{
				jvxlock->lock();
			}
			*os << "Get request on " << url << ", query:" << url_query << ", content: " << in_params << std::endl;
			if (!silentMode)
			{
				std::cout << "Get request on " << url << ", query:" << url_query << ", content: " << in_params << std::endl;
			}

			if (jvxlock)
			{
				jvxlock->unlock();
			}
		}

		switch (identify)
		{
		case 0:

			// Address "/jvx/host/commands/"
			break;
		case 1:

			// Address "/jvx/host/components/" JVX_HTTP_REPLACE_TOKEN

			// This location accepts requests to dedicated prperties
			// The query may contain:
			// - "slotid"=<PARAM> with <PARAM> an integer
			// - "slotsubid"=<PARAM> with <PARAM> an integer
			// - "purpose"=<PARAM> with <PARAM> either "description" or "extended_info"
			suburl = url.substr(((std::string)uriprefix).size(), std::string::npos);

			jvx_parseHttpQueryIntoPieces(query_list, url_query);


			cpType.tp = (jvxComponentType)theRef.linkToHelper;
			cpType.slotid = 0;
			cpType.slotsubid = 0;

			if (suburl == "/multi")
			{
				resL = jvx_findValueHttpQuery(query_list, token, "multi");
				if (resL == JVX_NO_ERROR)
				{
					suburl = token;
				}
			}

			resL = jvx_findValueHttpQuery(query_list, token, "slotid");
			if (resL == JVX_NO_ERROR)
			{
				cpType.slotid = jvx_string2Size(token, isErr);
				if (isErr)
				{
					std::cout << __FUNCTION__ << "Error" << std::endl;
				}
			}
			resL = jvx_findValueHttpQuery(query_list, token, "slotsubid");
			if (resL == JVX_NO_ERROR)
			{
				cpType.slotsubid = jvx_string2Size(token, isErr);
				if (isErr)
				{
					std::cout << __FUNCTION__ << "Error" << std::endl;
				}
			}

			resL = jvx_findValueHttpQuery(query_list, token, "purpose");
			if (resL != JVX_NO_ERROR)
			{
				resL = jvx_findValueHttpQuery(query_list, token, "content_only");
				if (resL == JVX_NO_ERROR)
				{
					if (token == "yes")
					{
						contentOnly = true;
					}
				}
				resL = jvx_findValueHttpQuery(query_list, token, "offset");
				if (resL == JVX_NO_ERROR)
				{
					offset = jvx_string2Size(token, isErr);
					if (isErr)
					{
						std::cout << __FUNCTION__ << "Error" << std::endl;
					}
				}
				resL = jvx_findValueHttpQuery(query_list, token, "num_elements");
				if (resL == JVX_NO_ERROR)
				{
					num_elements = jvx_string2Size(token, isErr);
					if (isErr)
					{
						std::cout << __FUNCTION__ << "Error" << std::endl;
					}
				}
				resL = jvx_findValueHttpQuery(query_list, token, "call_context");
				if (resL == JVX_NO_ERROR)
				{
					context_token = token;
				}


				// Function to get the value of a property
				command = "act(";
				command += jvxComponentIdentification_txt(cpType);
				command += ", get_property, ";
				command += suburl;
				command += ", " + jvx_size2String(offset);
				command += ", " + jvx_size2String(num_elements);
				if (contentOnly)
				{
					command += ", yes";
				}

				if (!context_token.empty())
				{
					// If there is a context, add the "missing" contentOnly
					if (!contentOnly)
					{
						command += ", no";
					}

					// And add the context token
					command += ", " + context_token;
				}
				command += ")";
			}
			else 
			{
				// Replace hastag by | to match the command syntax
				token = jvx_replaceCharacter(token, ':', '|');
				command = "show(";
				command += jvxComponentIdentification_txt(cpType);
				command += ", properties,";
				command += token;
				command += ",";
				command += suburl;
				command += ")";
			}

			requiresInterpretation = true;
			break;

		case 2:

			// Address  "/jvx/host/sequencer/"
			// This location accepts requests to dedicated prperties
			// - "purpose"=<PARAM> with <PARAM> either "start" or "stop" 
			// - If no "purpose", info is requested
			suburl = url.substr(((std::string)uriprefix).size(), std::string::npos);
			if (suburl == "status")
			{
				// Function to get the value of a property
				command = "show(sequencer, status);";
				requiresInterpretation = true;
			}
			else if (suburl == "list")
			{
				// Function to get the value of a property
				command = "show(sequencer);";
				requiresInterpretation = true;
				/*
				jvx_parseHttpQueryIntoPieces(query_list, url_query);
				resL = jvx_findValueHttpQuery(query_list, token, "purpose");
				if (resL == JVX_NO_ERROR)
				{
				}*/
			}
			else
			{
				// Nothing specific
			}

			break;

		case 3:

			suburl = url.substr(((std::string)uriprefix).size(), std::string::npos);
			if (suburl == "list")
			{
				// Address "/jvx/host/connections/"
				command = "show(connections);";
				requiresInterpretation = true;
			}
			else if (suburl == "path")
			{
				// Address "/jvx/host/connections/"
				command = "show(connections, path);";
				requiresInterpretation = true;
			}
			break;

		case 4:
			// Address "/jvx/host/web/"
			break;

		case 5:
			// Address "/jvx/host/system/"

			// This location accepts requests to dedicated prperties
			// The query may contain:
			// - "slotid"=<PARAM> with <PARAM> an integer
			// - "slotsubid"=<PARAM> with <PARAM> an integer
			// - "purpose"=<PARAM> with <PARAM> either "description" or "extended_info"
			suburl = url.substr(((std::string)uriprefix).size(), std::string::npos);

			if (suburl == "status")
			{

				jvx_parseHttpQueryIntoPieces(query_list, url_query);

				resL = jvx_findValueHttpQuery(query_list, token, "purpose");
				if ((resL != JVX_NO_ERROR) || (token == "full"))
				{
					command = "show(system)";
					requiresInterpretation = true;
				}
				else if (token == "components")
				{
					command = "show(components)";
					requiresInterpretation = true;
				}
				else if (token == "compact")
				{
					command = "show(system, compact)";
					requiresInterpretation = true;
				}
				else
				{
					jvxComponentIdentification cpTp;
					jvxApiString strRet;
					hdl->in_connect_url_decode(token.c_str(), JVX_SIZE_INT32(token.size()), &strRet);
					token = strRet.std_str();
					token = jvx_replaceCharacter(token, '_', ' ');
					resL = jvxComponentIdentification_decode(cpTp, token);
					if (resL == JVX_NO_ERROR)
					{
						command = "show(";
						command += token;
						command += ")";
						requiresInterpretation = true;
					}
					else
					{
						std::cout << "Invalid request on " << uriprefix << ": " << token << std::endl;
					}
				}

			}
			else if (suburl == "version")
			{
				command = "show(version)";
				requiresInterpretation = true;
			}
			break;
		}
#if 0
		if (uniqueId == notifyCoIds.theIds[0])
		{
			// Address /jvx/host/general/
			command = "show(whatever)";
			requiresInterpretation = true;
		}
		else if (uniqueId == notifyCoIds.theIds[1])
		{
			// Address /jvx/host/components/
			command = "show(whatever)";
			requiresInterpretation = true;
		}
		else if (uniqueId == notifyCoIds.theIds[2])
		{
			// Address /jvx/host/sequencer/
			command = "show(whatever)";
			requiresInterpretation = true;
		}
		else if (uniqueId == notifyCoIds.theIds[3])
		{
			// Address /jvx/host/properties/
			command = "show(whatever)";
			requiresInterpretation = true;
		}
		else
		{
			// Unmatched
			errorDetected = true;
		}
#endif
		break;

	case JVX_WEB_SERVER_URI_POST_HANDLER_PURP:

		resL = JVX_NO_ERROR;
		while (resL == JVX_NO_ERROR)
		{
			memset(oneToken, 0, JVX_MAXSTRING);
			sz = JVX_MAXSTRING - 1;
			resL = hdl->in_connect_read_content(context_server, context_conn, oneToken, &sz);
			in_params += oneToken;
		}

		if (os)
		{
			if (jvxlock)
			{
				jvxlock->lock();
			}

			*os << "Post request on " << url << ", query:" << url_query << ", content: " << in_params << std::endl;
			if (!silentMode)
			{
				std::cout << "Post request on " << url << ", query:" << url_query << ", content: " << in_params << std::endl;
			}
			if (jvxlock)
			{
				jvxlock->unlock();
			}
		}

		switch (identify)
		{
		case 0:

			// Address "/jvx/host/commands/"
			command = in_params;
			requiresInterpretation = true;
			break;
		case 1:

			// Address "/jvx/host/components/" JVX_HTTP_REPLACE_TOKEN

			// This location accepts requests to dedicated prperties
			// The query may contain:
			// - "slotid"=<PARAM> with <PARAM> an integer
			// - "slotsubid"=<PARAM> with <PARAM> an integer
			suburl = url.substr(((std::string)uriprefix).size(), std::string::npos);

			jvx_parseHttpQueryIntoPieces(query_list, url_query);

			cpType.tp = (jvxComponentType)theRef.linkToHelper;
			cpType.slotid = 0;
			cpType.slotsubid = 0;

			resL = jvx_findValueHttpQuery(query_list, token, "slotid");
			if (resL == JVX_NO_ERROR)
			{
				cpType.slotid = jvx_string2Size(token, isErr);
				if (isErr)
				{
					std::cout << __FUNCTION__ << "Error" << std::endl;
				}
			}
			resL = jvx_findValueHttpQuery(query_list, token, "slotsubid");
			if (resL == JVX_NO_ERROR)
			{
				cpType.slotsubid = jvx_string2Size(token, isErr);
				if (isErr)
				{
					std::cout << __FUNCTION__ << "Error" << std::endl;
				}
			}
			resL = jvx_findValueHttpQuery(query_list, token, "offset");
			if (resL == JVX_NO_ERROR)
			{
				offset = jvx_string2Size(token, isErr);
				if (isErr)
				{
					std::cout << __FUNCTION__ << "Error" << std::endl;
				}
			}
			resL = jvx_findValueHttpQuery(query_list, token, "report");
			if (resL == JVX_NO_ERROR)
			{
				if (token == "yes")
				{
					pretoken = "[r]";
				}
			}

			if (in_params.size())
			{
				// Function to set the value of a property
				command = "act" + pretoken + "(";
				command += jvxComponentIdentification_txt(cpType);
				command += ", set_property, ";
				command += suburl;
				command += ", ";
				command += in_params;
				if (offset > 0)
				{
					command += ", " + jvx_size2String(offset);
				}
				command += ")";
			}
			else
			{
				std::cout << __FUNCTION__ << "Error: Empty command in PUT request" << std::endl;
				errorDetected = true;
			}

			requiresInterpretation = true;
			break;

		case 2:

			// Address  "/jvx/host/sequencer/"
			// This location accepts requests to dedicated prperties
			// - "purpose"=<PARAM> with <PARAM> either "start" or "stop" 
			// - If no "purpose", info is requested
			suburl = url.substr(((std::string)uriprefix).size(), std::string::npos);
			if ((suburl == "start") || (suburl == "stop") || (suburl == "stops"))
			{
				// Function to get the value of a property
				command = "act(sequencer, ";
				command += suburl;
				command += ")";
				requiresInterpretation = true;
			}
			else
			{
				/*
				jvx_parseHttpQueryIntoPieces(query_list, url_query);
				resL = jvx_findValueHttpQuery(query_list, token, "purpose");
				if (resL == JVX_NO_ERROR)
				{
					if ((token == "start") || (token == "stop"))
					{
						// Function to get the value of a property
						command = "act(sequencer, ";
						command += token;
						command += ")";
						requiresInterpretation = true;
					}
				}*/
			}

			break;

		case 3:

			// Address "/jvx/host/connections/"
			break;
		}
#if 0 
		if (uniqueId == notifyCoIds.theIds[0])
		{
			// Address /jvx/host/general/

		}
		else if (uniqueId == notifyCoIds.theIds[1])
		{
			// Address /jvx/host/components/
			command = in_params;
			requiresInterpretation = true;
		}
		else if (uniqueId == notifyCoIds.theIds[2])
		{
			// Address /jvx/host/sequencer/
			command = in_params;
			requiresInterpretation = true;
		}
		else if (uniqueId == notifyCoIds.theIds[3])
		{
			// Address /jvx/host/properties/
			command = in_params;
			requiresInterpretation = true;
		}
		else
		{
			// Unmatched
			errorDetected = true;
		}
#endif
		break;

	case JVX_WEB_SERVER_URI_PUT_HANDLER_PURP:

		resL = JVX_NO_ERROR;
		while (resL == JVX_NO_ERROR)
		{
			memset(oneToken, 0, JVX_MAXSTRING);
			sz = JVX_MAXSTRING - 1;
			resL = hdl->in_connect_read_content(context_server, context_conn, oneToken, &sz);
			in_params += oneToken;
		}

		//std::cout << "Put request on " << url << ", query:" << url_query << ", content: " << in_params << std::endl;

		if (!in_params.empty())
		{
			switch (identify)
			{
			case 0:

				// Address "/jvx/host/commands/"
				// Address "/jvx/host/commands/"
				command = in_params;
				requiresInterpretation = true;
				break;
			case 1:

				// Address "/jvx/host/components/" JVX_HTTP_REPLACE_TOKEN

				// This location accepts requests to dedicated prperties
				// The query may contain:
				// - "slotid"=<PARAM> with <PARAM> an integer
				// - "slotsubid"=<PARAM> with <PARAM> an integer
				suburl = url.substr(((std::string)uriprefix).size(), std::string::npos);

				jvx_parseHttpQueryIntoPieces(query_list, url_query);

				cpType.tp = (jvxComponentType)theRef.linkToHelper;
				cpType.slotid = 0;
				cpType.slotsubid = 0;

				resL = jvx_findValueHttpQuery(query_list, token, "slotid");
				if (resL == JVX_NO_ERROR)
				{
					cpType.slotid = jvx_string2Size(token, isErr);
					if (isErr)
					{
						std::cout << __FUNCTION__ << "Error" << std::endl;
					}
				}
				resL = jvx_findValueHttpQuery(query_list, token, "slotsubid");
				if (resL == JVX_NO_ERROR)
				{
					cpType.slotsubid = jvx_string2Size(token, isErr);
					if (isErr)
					{
						std::cout << __FUNCTION__ << "Error" << std::endl;
					}
				}
				resL = jvx_findValueHttpQuery(query_list, token, "offset");
				if (resL == JVX_NO_ERROR)
				{
					offset = jvx_string2Size(token, isErr);
					if (isErr)
					{
						std::cout << __FUNCTION__ << "Error" << std::endl;
					}
				}

				if (in_params.size())
				{
					// Function to set the value of a property
					command = "act(";
					command += jvxComponentIdentification_txt(cpType);
					command += ", set_property, ";
					command += suburl;
					command += ", ";
					command += in_params;
					if (offset > 0)
					{
						command += ", " + jvx_size2String(offset);
					}
					//command += ", only_if_taken_unaltered";
					command += ")";
				}
				else
				{
					std::cout << __FUNCTION__ << "Error: Empty command in PUT request" << std::endl;
					errorDetected = true;
				}

				requiresInterpretation = true;
				break;

			case 2:

				// Address  "/jvx/host/sequencer/"
			// This location accepts requests to dedicated prperties
			// - "purpose"=<PARAM> with <PARAM> either "start" or "stop" 
			// - If no "purpose", info is requested
				suburl = url.substr(((std::string)uriprefix).size(), std::string::npos);
				if ((suburl == "start") || (suburl == "stop") || (suburl == "stops"))
				{
					// Function to get the value of a property
					command = "act(sequencer, ";
					command += suburl;
					command += ")";
					requiresInterpretation = true;
				}
				else
				{
					/*
					jvx_parseHttpQueryIntoPieces(query_list, url_query);
					resL = jvx_findValueHttpQuery(query_list, token, "purpose");
					if (resL == JVX_NO_ERROR)
					{
						if ((token == "start") || (token == "stop"))
						{
							// Function to get the value of a property
							command = "act(sequencer, ";
							command += token;
							command += ")";
							requiresInterpretation = true;
						}
					}*/
				}
				break;

			case 3:

				// Address "/jvx/host/connections/"
				break;
			}
#if 0
			purpose = this->extractStringParameters(in_params, "purpose");
			if (uniqueId == notifyCoIds.theIds[0])
			{
				// Address /jvx/host/general/
				command = "act(whatever)";
				requiresInterpretation = true;
			}
			else if (uniqueId == notifyCoIds.theIds[1])
			{
				// Address /jvx/host/components/
				comp_target = extractStringParameters(in_params, "component_target");
				command = "act(whatever)";

				componentTargetStringToComponentType(comp_target, cpType);
				requiresInterpretation = true;
			}
			else if (uniqueId == notifyCoIds.theIds[2])
			{
				// Address /jvx/host/sequencer/
				command = "act(whatever)";
				requiresInterpretation = true;
			}
			else if (uniqueId == notifyCoIds.theIds[3])
			{
				// Address /jvx/host/properties/
				command = "act(whatever)";
				requiresInterpretation = true;
			}
			else
			{
				// Unmatched
				errorDetected = true;
			}
#endif
		}
		break;
	case JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_PURP:
		res = hook_wsConnect(context_server, context_conn);
		break;
	case JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_PURP:
		res = hook_wsData(context_server, context_conn, header, payload_ws, szFld);
		break;
	case JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_PURP:
		res = hook_wsReady(context_server, context_conn);
		break;
	case JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_PURP:
		res = hook_wsClose(context_server, context_conn);
		break;
	default:
		assert(0);
	}
	return(JVX_NO_ERROR);
};

// Web socket handling	
jvxErrorType
CjvxWebServerHost::hook_wsConnect(jvxHandle* context_server, jvxHandle* context_conn)
{
	// We should arrive here within the QT thread, therefore no threading problem!!
	oneWsConnection theConn;
	theConn.isConnected = false;
	theConn.selector[0] = context_conn;
	theConn.theServer = context_server;

	if (this->theWebserver.openConnectionsWs.size() == 0)
	{
		if (theHooks)
		{
			theHooks->hook_ws_started();
		}
	}
	this->theWebserver.openConnectionsWs.push_back(theConn);
	return(JVX_NO_ERROR);
};

jvxErrorType
CjvxWebServerHost::hook_wsReady(jvxHandle* context_server, jvxHandle* context_conn)
{
	std::vector<oneWsConnection>::iterator elm = jvx_findItemSelectorInList_one<oneWsConnection, jvxHandle*>(theWebserver.openConnectionsWs, context_conn, 0);
	if (elm != theWebserver.openConnectionsWs.end())
	{
		elm->isConnected = true;
	}
	else
	{
		std::cout << "Unexpected problem in mainCentral::hook_wsReady: unknown confirmed connection handle" << std::endl;
	}
	return(JVX_NO_ERROR);
};

jvxErrorType
CjvxWebServerHost::hook_wsData(jvxHandle* context_server, jvxHandle* context_conn, int flags, char* payload, size_t szFld)
{
	jvxErrorType res = JVX_NO_ERROR;
	char fldIn[JVX_MAXSTRING];
	jvxSize numcopy = 0;
	jvxSize i;
	std::string resp;
	std::string command;
	std::string f_expr;
	std::vector<std::string> args;
	std::string prop_descr;
	jvxComponentIdentification tp;
	jvxBool startit = false;
	jvxBool stopit = false;
	jvxSize idRet = 0;

	// We should find (http://tools.ietf.org/html/rfc6455):
	// - (flags & 0x80) == 1
	// - flags &= 0x7f => Opcode
	// Translations in Opcode meaning -> jvxWebServerWebSocketFrameType in TjvxWebServer.h
	if (flags & 0x80)
	{
		jvxInt8 opcode = (jvxInt8)(flags & 0x7f);
		switch (opcode)
		{
		case JVX_WEBSOCKET_OPCODE_CONNECTION_CLOSE:
			res = JVX_ERROR_ABORT;
			break;
		case JVX_WEBSOCKET_OPCODE_TEXT:
			memset(fldIn, 0, JVX_MAXSTRING);
			numcopy = JVX_MIN(szFld, JVX_MAXSTRING - 1);
			if (numcopy == szFld)
			{
				memcpy(fldIn, payload, numcopy);
				command = fldIn;

				jvx_parseCommandIntoToken(command, f_expr, args);
				if (f_expr == "keepalive")
				{
					// This message is ok, respond with "ok"
					resp = "ok:keepalive";
				}
				else
				{
					if (args.size() == 2)
					{
						// Find component type
						for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
						{
							std::string compTo = jvxComponentType_txt(i);
							if (args[0] == compTo)
							{
								tp = jvxComponentIdentification((jvxComponentType)i, 0, 0);
								prop_descr = args[1];
								break;
							}
						}
					}

					if (tp.tp != JVX_COMPONENT_UNKNOWN)
					{
						if (f_expr == "start")
						{
							startit = true;
						}
						else if (f_expr == "stop")
						{
							stopit = true;
						}
						else
						{
							res = JVX_ERROR_INVALID_ARGUMENT;
						}
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}

					if (res == JVX_NO_ERROR)
					{
						if (startit)
						{
							std::vector<oneWsConnection>::iterator elm = jvx_findItemSelectorInList_one<oneWsConnection, jvxHandle*>(theWebserver.openConnectionsWs, context_conn, 0);
							if (elm != theWebserver.openConnectionsWs.end())
							{
								std::vector<oneWsPropertyStream>::iterator elmT = jvx_findItemSelectorInList_one<oneWsPropertyStream, jvxComponentIdentification>(elm->theStreams, tp, 0);
								if (elmT == elm->theStreams.end())
								{
									oneWsPropertyStream newStream;
									newStream.prop_descr = prop_descr;
									newStream.selector[0] = tp;
									newStream.idClient = theWebserver.uniqueId;
									theWebserver.uniqueId++;
									idRet = newStream.idClient;
									newStream.validPropertyHandle = false;
									jvx_initPropertyDescription(newStream.descriptorPropertyHandle);
									newStream.bufSend = NULL;
									newStream.bufSize = 0;

									elm->theStreams.push_back(newStream);
								}
								else
								{
									res = JVX_ERROR_DUPLICATE_ENTRY;
								}
							}
							else
							{
								res = JVX_ERROR_ELEMENT_NOT_FOUND;
							}
						}

						if (stopit)
						{
							std::vector<oneWsConnection>::iterator elm = jvx_findItemSelectorInList_one<oneWsConnection, jvxHandle*>(theWebserver.openConnectionsWs, context_conn, 0);
							if (elm != theWebserver.openConnectionsWs.end())
							{
								std::vector<oneWsPropertyStream>::iterator elmT = jvx_findItemSelectorInList_one<oneWsPropertyStream, jvxComponentIdentification>(elm->theStreams, tp, 0);
								if (elmT != elm->theStreams.end())
								{
									JVX_SAFE_DELETE_FLD(elmT->bufSend, jvxByte);
									elmT->bufSize = 0;
									idRet = elmT->idClient;
									elm->theStreams.erase(elmT);
								}
								else
								{
									res = JVX_ERROR_ELEMENT_NOT_FOUND;
								}
							}
							else
							{
								res = JVX_ERROR_ELEMENT_NOT_FOUND;
							}
						}
					}

					if (res == JVX_NO_ERROR)
					{
						resp = "ok:" + jvx_size2String(idRet);
					}
					else
					{
						resp = "error: ";
						resp += jvxErrorType_descr(res);
						res = JVX_NO_ERROR; // Keep link open
					}
				}// else: if(command == "keepalive")
				if (res == JVX_NO_ERROR)
				{
					hdl->in_connect_write_packet_websocket(context_server, context_conn, JVX_WEBSOCKET_OPCODE_TEXT, reinterpret_cast<const jvxHandle*>(resp.c_str()), resp.size());
				}
			}
			else
			{
				std::cout << "mainCentral::hook_wsData: Too many input characters, skipping message" << std::endl;
				res = JVX_ERROR_SIZE_MISMATCH;
			}


			break;
		default:
			// Do nothing
			break;
		}
	}

	// Do nothing, just return the info to keep 
	return(res);
};

jvxErrorType
CjvxWebServerHost::hook_wsClose(jvxHandle* context_server, jvxHandle* context_conn)
{
	jvxSize j;
	std::vector<oneWsConnection>::iterator elm = this->theWebserver.openConnectionsWs.begin();
	elm = jvx_findItemSelectorInList_one<oneWsConnection, jvxHandle*>(theWebserver.openConnectionsWs, context_conn, 0);
	if (elm != this->theWebserver.openConnectionsWs.end())
	{
		// Remove all fields present in this connection
		for (j = 0; j < elm->theStreams.size(); j++)
		{

			if (elm->theStreams[j].bufSend)
			{
				JVX_SAFE_DELETE_FLD(elm->theStreams[j].bufSend, jvxByte);
				elm->theStreams[j].bufSize = 0;
			}
		}
		this->theWebserver.openConnectionsWs.erase(elm);
	}

	if (this->theWebserver.openConnectionsWs.size() == 0)
	{
		if (theHooks)
		{
			theHooks->hook_ws_stopped();
		}
	}

	return(JVX_NO_ERROR);
};

void
CjvxWebServerHost::postPropertyStreams()
{
	jvxSize i, j;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool allocateFieldNew = false;
	jvxBool secondPollProperty = false;
	jvxBool sendContent = false;

	jvxCallManagerProperties callGate;

	jvxBool obtainDescriptionProp = false;

	//	if(!theWebserver.closeProcedureState == 0)
	//	{
	for (i = 0; i < this->theWebserver.openConnectionsWs.size(); i++)
	{
		if (this->theWebserver.openConnectionsWs[i].isConnected)
		{
			// Post all properties
			for (j = 0; j < this->theWebserver.openConnectionsWs[i].theStreams.size(); j++)
			{
				allocateFieldNew = false;
				secondPollProperty = false;
				sendContent = false;

				callGate.reset();

				obtainDescriptionProp = false;
				if (this->theWebserver.openConnectionsWs[i].theStreams[j].validPropertyHandle)
				{
					callGate.accessFlags = JVX_ACCESS_ROLE_DEFAULT; // Here, we need to find the right specific role

					resL = jvx_get_property(hostHdl,
						((jvxByte*)this->theWebserver.openConnectionsWs[i].theStreams[j].bufSend + sizeof(jvxPropStreamProtocolHeader)),
						0,
						this->theWebserver.openConnectionsWs[i].theStreams[j].descriptorPropertyHandle.num,
						this->theWebserver.openConnectionsWs[i].theStreams[j].descriptorPropertyHandle.format,
						true,
						this->theWebserver.openConnectionsWs[i].theStreams[j].selector[0],
						this->theWebserver.openConnectionsWs[i].theStreams[j].prop_descr.c_str(),
						callGate);

					if (JVX_CHECK_PROPERTY_ACCESS_OK(resL, callGate.access_protocol, this->theWebserver.openConnectionsWs[i].theStreams[j].prop_descr,
						jvxComponentIdentification_txt(this->theWebserver.openConnectionsWs[i].theStreams[j].selector[0])))
					{
						sendContent = true;

					}
					else
					{
						obtainDescriptionProp = true;
					}
				}
				else
				{
					obtainDescriptionProp = true;
				}


				if (obtainDescriptionProp)
				{
					jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(this->theWebserver.openConnectionsWs[i].theStreams[j].prop_descr.c_str());
					// IjvxHost* theHost, jvxPropertyDescriptor& theStr, jvxComponentIdentification tp, const char* descriptor)
					resL = jvx_getPropertyDescription(hostHdl,
						this->theWebserver.openConnectionsWs[i].theStreams[j].descriptorPropertyHandle,
						this->theWebserver.openConnectionsWs[i].theStreams[j].selector[0],
						ident,
						callGate);
					if (resL == JVX_NO_ERROR)
					{
						switch (this->theWebserver.openConnectionsWs[i].theStreams[j].descriptorPropertyHandle.format)
						{
						case JVX_DATAFORMAT_8BIT:
						case JVX_DATAFORMAT_16BIT_LE:
						case JVX_DATAFORMAT_32BIT_LE:
						case JVX_DATAFORMAT_64BIT_LE:
						case JVX_DATAFORMAT_DATA:
							allocateFieldNew = true;
							break;
						default:
							break;
						}
					}

					if (allocateFieldNew)
					{
						this->theWebserver.openConnectionsWs[i].theStreams[j].validPropertyHandle = true;
						if (this->theWebserver.openConnectionsWs[i].theStreams[j].bufSend)
						{
							JVX_SAFE_DELETE_FLD(this->theWebserver.openConnectionsWs[i].theStreams[j].bufSend, jvxByte);
							this->theWebserver.openConnectionsWs[i].theStreams[j].bufSize = 0;
						}
						this->theWebserver.openConnectionsWs[i].theStreams[j].bufSize = sizeof(jvxPropStreamProtocolHeader) +
							this->theWebserver.openConnectionsWs[i].theStreams[j].descriptorPropertyHandle.num *
							jvxDataFormat_size[this->theWebserver.openConnectionsWs[i].theStreams[j].descriptorPropertyHandle.format];
						JVX_SAFE_NEW_FLD(this->theWebserver.openConnectionsWs[i].theStreams[j].bufSend, jvxByte, this->theWebserver.openConnectionsWs[i].theStreams[j].bufSize);
						memset(this->theWebserver.openConnectionsWs[i].theStreams[j].bufSend, 0, sizeof(jvxByte) * this->theWebserver.openConnectionsWs[i].theStreams[j].bufSize);
						jvxPropStreamProtocolHeader* header = (jvxPropStreamProtocolHeader*)this->theWebserver.openConnectionsWs[i].theStreams[j].bufSend;
						header->fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
						header->propid = JVX_SIZE_INT16(this->theWebserver.openConnectionsWs[i].theStreams[j].idClient);
						header->dataformat = this->theWebserver.openConnectionsWs[i].theStreams[j].descriptorPropertyHandle.format;
						header->numberElements = (jvxUInt32)this->theWebserver.openConnectionsWs[i].theStreams[j].descriptorPropertyHandle.num;
						header->offsetElements = 0;
						secondPollProperty = true;
						sendContent = true;
					}

					if (secondPollProperty)
					{
						callGate.reset();
						resL = jvx_get_property(hostHdl,
							((jvxByte*)this->theWebserver.openConnectionsWs[i].theStreams[j].bufSend + sizeof(jvxPropStreamProtocolHeader)),
							0,
							this->theWebserver.openConnectionsWs[i].theStreams[j].descriptorPropertyHandle.num,
							this->theWebserver.openConnectionsWs[i].theStreams[j].descriptorPropertyHandle.format,
							true,
							this->theWebserver.openConnectionsWs[i].theStreams[j].selector[0],
							this->theWebserver.openConnectionsWs[i].theStreams[j].prop_descr.c_str(),
							callGate);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(resL, callGate.access_protocol, this->theWebserver.openConnectionsWs[i].theStreams[j].prop_descr,
							jvxComponentIdentification_txt(this->theWebserver.openConnectionsWs[i].theStreams[j].selector[0])))
						{
							sendContent = true;
						}
						else
						{
							this->theWebserver.openConnectionsWs[i].theStreams[j].validPropertyHandle = true;
						}
					}
				}
				if (sendContent)
				{
					hdl->in_connect_write_packet_websocket(
						this->theWebserver.openConnectionsWs[i].theServer,
						this->theWebserver.openConnectionsWs[i].selector[0],
						JVX_WEBSOCKET_OPCODE_BINARY,
						this->theWebserver.openConnectionsWs[i].theStreams[j].bufSend,
						this->theWebserver.openConnectionsWs[i].theStreams[j].bufSize);
				}
			}// for(j = 0; j < this->theWebserver.openConnectionsWs[i].theStreams.size(); j++)
		}
	}
	//}
};

// ======================================================================

jvxErrorType componentTargetStringToComponentType(std::string componentToken, jvxComponentIdentification& cpType)
{
	jvxSize i;
	cpType.tp = JVX_COMPONENT_UNKNOWN;
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		if (jvxComponentType_txt(i) == componentToken)
		{
			cpType.tp = (jvxComponentType)i;
			cpType.slotid = 0;
			cpType.slotsubid = 0;
			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxWebServerHost::operationStringToStateSwitchType(std::string operToken, jvxStateSwitch& ssType)
{
	jvxSize i;
	ssType = JVX_STATE_SWITCH_NONE;
	for (i = 0; i < JVX_STATE_SWITCH_LIMIT; i++)
	{
		if (jvxStateSwitch_txt(i) == operToken)
		{
			ssType = (jvxStateSwitch)i;
			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

void
CjvxWebServerHost::writeResponseLeadIn(jvxHandle* server, jvxHandle* conn, jvxBool callResult, std::string err)
{
	std::string token;

	// ****
	token = "{\"result\":";
	hdl->in_connect_write_response(server, conn, token.c_str());
	token.clear();

	if (callResult)
	{
		token = "\"ok\"";
	}
	else
	{
		token = "\"err\"";
	}
	hdl->in_connect_write_response(server, conn, token.c_str());
	token.clear();

	token = ",\"err\":\"" + err + "\"";
	hdl->in_connect_write_response(server, conn, token.c_str());
	token.clear();
};

void
CjvxWebServerHost::writeResponseLeadOut(jvxHandle* server, jvxHandle* conn)
{
	std::string token;

	// ****
	token = "}";
	hdl->in_connect_write_response(server, conn, token.c_str());
	token.clear();
};

// ======================================================================

// +++

jvxErrorType
CjvxWebServerHost::response_host__state(jvxHandle* server, jvxHandle* conn)
{
	jvxState stat = JVX_STATE_NONE;
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;

	if (hostHdl)
	{
		res = hostHdl->state(&stat);

		hdl->in_connect_write_header_response(server, conn, JVX_WEB_SERVER_RESPONSE_JSON);

		if (res == JVX_NO_ERROR)
		{
			writeResponseLeadIn(server, conn, true, "--");
		}
		else
		{
			writeResponseLeadIn(server, conn, false, std::string("Failed to get state from host, error type) ") + jvxErrorType_descr(res));
		}

		token = ",\"state\":\""; token += jvxState_txt(stat);
		token += "\"";
		hdl->in_connect_write_response(server, conn, token.c_str());

		writeResponseLeadOut(server, conn);
	}

	return JVX_NO_ERROR;
};

jvxErrorType
CjvxWebServerHost::response_host__safe_config(jvxHandle* server, jvxHandle* conn)
{
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	if (hostHdl)
	{
		if (theHooks)
		{
			res = theHooks->hook_safeConfigFile();

			hdl->in_connect_write_header_response(server, conn, JVX_WEB_SERVER_RESPONSE_JSON);

			if (res == JVX_NO_ERROR)
			{
				writeResponseLeadIn(server, conn, true, "--");
			}
			else
			{
				writeResponseLeadIn(server, conn, false, "Failed to get state from host");
			}

			writeResponseLeadOut(server, conn);
		}
		else
		{
			writeResponseLeadIn(server, conn, false, "Failed to get state from host");

			writeResponseLeadOut(server, conn);
		}
	}
	return JVX_NO_ERROR;
};

// +++

jvxErrorType
CjvxWebServerHost::response_components__description(jvxHandle* server, jvxHandle* conn, jvxComponentIdentification compTp)
{
	jvxState stat = JVX_STATE_NONE;
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize sz = 0;
	jvxSize i;
	jvxApiString  fldStr;
	jvxApiString strL;

	if (hostHdl)
	{
		res = hostHdl->number_components_system(compTp, &sz);

		hdl->in_connect_write_header_response(server, conn, JVX_WEB_SERVER_RESPONSE_JSON);

		if (res == JVX_NO_ERROR)
		{
			writeResponseLeadIn(server, conn, true, "--");
		}
		else
		{
			writeResponseLeadIn(server, conn, false, std::string("Failed to get number of components from host, error type ") + jvxErrorType_descr(res));
		}

		token = ", \"component_target\":\"";
		token += jvxComponentIdentification_txt(compTp);
		token += "\"";
		hdl->in_connect_write_response(server, conn, token.c_str());

		token = ", \"components\":";
		token += "[";
		hdl->in_connect_write_response(server, conn, token.c_str());

		for (i = 0; i < sz; i++)
		{
			token = "";
			hostHdl->description_component_system(compTp, i, &strL);

			token = strL.std_str();

			if (i > 0)
			{
				token = ", \"" + token + "\"";
			}
			else
			{
				token = "\"" + token + "\"";
			}
			hdl->in_connect_write_response(server, conn, token.c_str());
		}

		token = "]";
		hdl->in_connect_write_response(server, conn, token.c_str());

		writeResponseLeadOut(server, conn);
	}

	return JVX_NO_ERROR;
};

jvxErrorType
CjvxWebServerHost::response_components__state(jvxHandle* server, jvxHandle* conn, jvxComponentIdentification compTp)
{
	jvxState stat = JVX_STATE_NONE;
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	if (hostHdl)
	{
		res = hostHdl->state_selected_component(compTp, &stat);

		hdl->in_connect_write_header_response(server, conn, JVX_WEB_SERVER_RESPONSE_JSON);

		if (res == JVX_NO_ERROR)
		{
			writeResponseLeadIn(server, conn, true, "--");
		}
		else
		{
			writeResponseLeadIn(server, conn, false, std::string("Failed to get state for componentfrom host, error type ") + jvxErrorType_descr(res));
		}

		token = ", \"component_target\":\"";
		token += jvxComponentIdentification_txt(compTp);
		token += "\"";
		hdl->in_connect_write_response(server, conn, token.c_str());

		token = ", \"state\":\"";
		token += jvxState_txt(stat);
		token += "\"";
		hdl->in_connect_write_response(server, conn, token.c_str());

		writeResponseLeadOut(server, conn);
	}

	return JVX_NO_ERROR;
};

jvxErrorType
CjvxWebServerHost::response_components__switch_state(jvxHandle* server, jvxHandle* conn, jvxComponentIdentification compTp,
	jvxStateSwitch operation, jvxInt32 idx)
{
	//jvxStateSwitch_str
	jvxState stat = JVX_STATE_NONE;
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize sz = 0;
	jvxSize slotId = 0;
	jvxSize slotSubId = 0;
	if (hostHdl)
	{
		switch (operation)
		{
		case JVX_STATE_SWITCH_SELECT:
			res = hostHdl->number_components_system(compTp, &sz);
			if ((idx >= 0) && (idx < sz))
			{
				res = hostHdl->select_component(compTp, idx);
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			break;
		case JVX_STATE_SWITCH_ACTIVATE:
			res = hostHdl->activate_selected_component(compTp);
			break;
		case JVX_STATE_SWITCH_DEACTIVATE:
			res = hostHdl->deactivate_selected_component(compTp);
			break;
		case JVX_STATE_SWITCH_UNSELECT:
			res = hostHdl->unselect_selected_component(compTp);
			break;
		default:
			res = JVX_ERROR_UNSUPPORTED;
			break;
		}

		hdl->in_connect_write_header_response(server, conn, JVX_WEB_SERVER_RESPONSE_JSON);

		if (res == JVX_NO_ERROR)
		{
			writeResponseLeadIn(server, conn, true, "--");
		}
		else
		{
			writeResponseLeadIn(server, conn, false, std::string("Failed to switch state for component, error type ") + jvxErrorType_descr(res));
		}

		token = ",\"component_target\":\"";
		token += jvxComponentIdentification_txt(compTp);
		token += "\"";
		hdl->in_connect_write_response(server, conn, token.c_str());

		writeResponseLeadOut(server, conn);
	}

	return JVX_NO_ERROR;
};

jvxErrorType
CjvxWebServerHost::response_sequencer__state(jvxHandle* server, jvxHandle* conn)
{
	jvxState stat = JVX_STATE_NONE;
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* theSeq = NULL;
	jvxSequencerStatus stats = JVX_SEQUENCER_STATUS_NONE;
	jvxSize idxSeq = 0;
	jvxSequencerQueueType qTp;
	jvxSize idxSt = 0;
	jvxBool loop = false;

	if (hostHdl)
	{
		res = hostHdl->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSeq));
		if ((res == JVX_NO_ERROR) && theSeq)
		{
			res = theSeq->status_process(&stats, &idxSeq, &qTp, &idxSt, &loop);
			hostHdl->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSeq));
		}

		hdl->in_connect_write_header_response(server, conn, JVX_WEB_SERVER_RESPONSE_JSON);
		if (res == JVX_NO_ERROR)
		{
			writeResponseLeadIn(server, conn, true, "--");
		}
		else
		{
			writeResponseLeadIn(server, conn, false, std::string("Failed to obtain sequencer status, error type ") + jvxErrorType_descr(res));
		}

		token = ",\"status\":";
		token += "\"";
		token += jvxSequencerStatus_str[stats].full;
		token += "\"";
		hdl->in_connect_write_response(server, conn, token.c_str());

		token = ",\"seq_id\":";
		token += "\"" + jvx_size2String(idxSeq) + "\"";
		hdl->in_connect_write_response(server, conn, token.c_str());

		token = ",\"queue_tp\":";
		token += "\"";
		token += jvxSequencerQueueType_str[stats].full;
		token += "\"";
		hdl->in_connect_write_response(server, conn, token.c_str());

		token = ",\"step_id\":";
		token += "\"";
		token += jvx_size2String(idxSt);
		token += "\"";
		hdl->in_connect_write_response(server, conn, token.c_str());

		writeResponseLeadOut(server, conn);
	}
	return JVX_NO_ERROR;
};

// +++

jvxErrorType
CjvxWebServerHost::response_sequencer__start(jvxHandle* server, jvxHandle* conn)
{
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	if (hostHdl)
	{
		if (theHooks)
		{
			res = theHooks->hook_startSequencer();

			hdl->in_connect_write_header_response(server, conn, JVX_WEB_SERVER_RESPONSE_JSON);

			if (res == JVX_NO_ERROR)
			{
				writeResponseLeadIn(server, conn, true, "--");
			}
			else
			{
				writeResponseLeadIn(server, conn, false, "Failed to start sequencer");
			}

			writeResponseLeadOut(server, conn);
		}
		else
		{
			writeResponseLeadIn(server, conn, false, "No jook provided for start of sequencer");

			writeResponseLeadOut(server, conn);
		}
	}
	return JVX_NO_ERROR;
};

jvxErrorType
CjvxWebServerHost::response_sequencer__stop(jvxHandle* server, jvxHandle* conn)
{
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	if (hostHdl)
	{
		if (theHooks)
		{
			res = theHooks->hook_stopSequencer();

			hdl->in_connect_write_header_response(server, conn, JVX_WEB_SERVER_RESPONSE_JSON);

			if (res == JVX_NO_ERROR)
			{
				writeResponseLeadIn(server, conn, true, "--");
			}
			else
			{
				writeResponseLeadIn(server, conn, false, "Failed to stop sequencer");
			}

			writeResponseLeadOut(server, conn);
		}
		else
		{
			writeResponseLeadIn(server, conn, false, "No jook provided for start of sequencer");

			writeResponseLeadOut(server, conn);
		}
	}
	return JVX_NO_ERROR;
};

// +++

jvxErrorType
CjvxWebServerHost::response_properties__description(jvxHandle* server, jvxHandle* conn, jvxComponentIdentification compTp)
{
	jvxState stat = JVX_STATE_NONE;
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize sz = 0, i = 0;
	jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
	jvx::propertyAddress::CjvxPropertyAddressLinear ident(0);
	/*
	jvxPropertyCategoryType category = JVX_PROPERTY_CATEGORY_UNKNOWN;
	jvxUInt64 settype = 0;
	jvxUInt64 stateAccess = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxSize num = 0;
	jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_WRITE_ONLY;
	jvxPropertyDecoderHintType decTp = JVX_PROPERTY_DECODER_NONE;
	jvxSize globalIdx = 0;
	jvxPropertyContext ctxt = JVX_PROPERTY_CONTEXT_UNKNOWN;
	jvxApiString  name;
	jvxApiString  description;
	jvxApiString  descriptor;
	jvxBool isValid = false;
	*/
	jvxCallManagerProperties callGate;
	jvx_propertyReferenceTriple theTriple;
	jvx_initPropertyReferenceTriple(&theTriple);

	if (hostHdl)
	{
		res = jvx_getReferencePropertiesObject(hostHdl, &theTriple, compTp);
		if (res == JVX_NO_ERROR)
		{
			if (jvx_isValidPropertyReferenceTriple(&theTriple))
			{
				writeResponseLeadIn(server, conn, true, "--");

				token = ",\"component_target\":\"";
				token += jvxComponentIdentification_txt(compTp);
				token += "\"";
				hdl->in_connect_write_response(server, conn, token.c_str());

				theTriple.theProps->number_properties(callGate, &sz);
				token = "\"properties\":[";
				hdl->in_connect_write_response(server, conn, token.c_str());

				for (i = 0; i < sz; i++)
				{
					token = "{";
					hdl->in_connect_write_response(server, conn, token.c_str());

					ident.idx = i;
					theTriple.theProps->description_property(callGate, theDescr, ident);
					/*
					theTriple.theProps->description_property(&callGate, i,
						&category,&settype,&stateAccess,&format,
						&num,&accessType,&decTp,&globalIdx,
						&ctxt,&name,&description,&descriptor, &isValid,
						NULL);
						*/

					token = "\"name\":\"";
					token += theDescr.name.std_str();
					token += "\"";
					hdl->in_connect_write_response(server, conn, token.c_str());

					token = "\"description\":\"";
					token += theDescr.description.std_str();
					token += "\"";
					hdl->in_connect_write_response(server, conn, token.c_str());

					token = "\"descriptor\":\"";
					token += theDescr.descriptor.std_str();
					token += "\"";
					hdl->in_connect_write_response(server, conn, token.c_str());

					token = "\"format\":\"";
					token += jvxDataFormat_str[theDescr.format].full;
					token += "\"";
					hdl->in_connect_write_response(server, conn, token.c_str());

					token = "\"number\":\"";
					token += jvx_size2String(theDescr.num);
					token += "\"";
					hdl->in_connect_write_response(server, conn, token.c_str());

					token = "\"isvalid\":\"";
					if (theDescr.isValid)
					{
						token += "yes";
					}
					else
					{
						token += "no";
					}
					token += "\"";
					hdl->in_connect_write_response(server, conn, token.c_str());

					token = "\"decoder_type\":\"";
					token += jvxPropertyDecoderHintType_str[theDescr.decTp].full;
					token += "\"";
					hdl->in_connect_write_response(server, conn, token.c_str());

					token = "}";
					hdl->in_connect_write_response(server, conn, token.c_str());
				}
				token = "]";
				hdl->in_connect_write_response(server, conn, token.c_str());

				writeResponseLeadOut(server, conn);
			}
			else
			{
				writeResponseLeadIn(server, conn, false, "Failed to obtain valid property handle reference");
				writeResponseLeadOut(server, conn);
			}

			jvx_returnReferencePropertiesObject(hostHdl, &theTriple, compTp);
		}
		else
		{
			writeResponseLeadIn(server, conn, false, "Failed to obtain valid property handle reference");
			writeResponseLeadOut(server, conn);
		}
	}
	return JVX_NO_ERROR;
};

jvxErrorType
CjvxWebServerHost::response_properties__get_content(jvxHandle* server, jvxHandle* conn, jvxComponentIdentification compTp,
	std::string descriptor, jvxBool contentOnly)
{
	jvxState stat = JVX_STATE_NONE;
	std::string token;
	jvxErrorType res = JVX_NO_ERROR;

	jvx::propertyDescriptor::CjvxPropertyDescriptorCore descr;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descriptor.c_str());
	jvx::propertyAddress::CjvxPropertyAddressGlobalId identIdx;
	jvx::propertyDetail::CjvxTranferDetail trans;

	jvxSize sz = 0;
	jvxCallManagerProperties callGate;
	jvxSize i;

	jvxByte* copyPtr = NULL;
	jvxSelectionList lst;
	lst.bitFieldSelected() = 0;
	lst.bitFieldExclusive = 0;

	jvxApiString  fldStr;
	jvxValueInRange fldValRange;

	jvx_propertyReferenceTriple theTriple;
	jvx_initPropertyReferenceTriple(&theTriple);

	if (hostHdl)
	{
		res = jvx_getReferencePropertiesObject(hostHdl, &theTriple, compTp);
		if (res == JVX_NO_ERROR)
		{
			res = theTriple.theProps->description_property(
				callGate, descr, ident);

			if (res == JVX_NO_ERROR)
			{
				res = theTriple.theProps->number_properties(callGate, &sz);
				if (res == JVX_NO_ERROR)
				{
					switch (descr.format)
					{
					case JVX_DATAFORMAT_DATA:
					case JVX_DATAFORMAT_8BIT:
					case JVX_DATAFORMAT_16BIT_LE:
					case JVX_DATAFORMAT_32BIT_LE:
					case JVX_DATAFORMAT_64BIT_LE:

						identIdx.reset(descr.globalIdx, descr.category);
						trans.reset();
						JVX_SAFE_NEW_FLD(copyPtr, jvxByte, jvxDataFormat_size[descr.format] * descr.num);
						res = theTriple.theProps->get_property(callGate, jPRG(copyPtr,
							descr.num, descr.format), identIdx, trans);
						break;

					case JVX_DATAFORMAT_SELECTION_LIST:

						identIdx.reset(descr.globalIdx, descr.category);
						trans.reset(contentOnly);
						res = theTriple.theProps->get_property(
							callGate,
							jPRG(&lst,
								descr.num, descr.format),
							identIdx, trans);
						break;
					case JVX_DATAFORMAT_STRING:

						identIdx.reset(descr.globalIdx, descr.category);
						trans.reset(contentOnly);
						res = theTriple.theProps->get_property(callGate, jPRG(&fldStr,
							descr.num, descr.format), identIdx, trans);
						break;
					case JVX_DATAFORMAT_STRING_LIST:
						identIdx.reset(descr.globalIdx, descr.category);
						trans.reset(contentOnly);
						res = theTriple.theProps->get_property(callGate, jPRG(
							&lst.strList,
							descr.num, descr.format),
							identIdx, trans);
						break;
					case JVX_DATAFORMAT_VALUE_IN_RANGE:

						identIdx.reset(descr.globalIdx, descr.category);
						trans.reset(contentOnly);
						res = theTriple.theProps->get_property(
							callGate, jPRG(&fldValRange,
								descr.num, descr.format),
							identIdx, trans);
						break;
					default:
						break;
					}
					if (res == JVX_NO_ERROR)
					{
						writeResponseLeadIn(server, conn, true, "--");

						token = ",\"component_target\":\"";
						token += jvxComponentIdentification_txt(compTp);
						token += "\"";
						hdl->in_connect_write_response(server, conn, token.c_str());

						token = ",\"property_descriptor\":\"" + descriptor + "\"";
						hdl->in_connect_write_response(server, conn, token.c_str());

						jvxData* fldData = (jvxData*)copyPtr;
						jvxInt8* fldI8 = (jvxInt8*)copyPtr;
						jvxInt16* fldI16 = (jvxInt16*)copyPtr;
						jvxInt32* fldI32 = (jvxInt32*)copyPtr;
						jvxInt64* fldI64 = (jvxInt64*)copyPtr;

						switch (descr.format)
						{
						case JVX_DATAFORMAT_DATA:

							token = ",\"value\":";
							hdl->in_connect_write_response(server, conn, token.c_str());
							if (descr.num > 1)
							{
								token = "[";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}

							for (i = 0; i < descr.num; i++)
							{
								token = "";
								if (i > 0)
								{
									token += ",";
								}
								token += jvx_data2String(fldData[i], JVX_NUMBER_DIGITS_FLOAT);
								hdl->in_connect_write_response(server, conn, token.c_str());
							}

							if (descr.num > 1)
							{
								token = "]";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}
							break;

						case JVX_DATAFORMAT_8BIT:

							token = ",\"value\":";
							hdl->in_connect_write_response(server, conn, token.c_str());
							if (descr.num > 1)
							{
								token = "[";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}

							for (i = 0; i < descr.num; i++)
							{
								token = "";
								if (i > 0)
								{
									token += ",";
								}
								token += jvx_int2String(fldI8[i]);
								hdl->in_connect_write_response(server, conn, token.c_str());
							}

							if (descr.num > 1)
							{
								token = "]";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}
							break;

						case JVX_DATAFORMAT_16BIT_LE:

							token = ",\"value\":";
							hdl->in_connect_write_response(server, conn, token.c_str());
							if (descr.num > 1)
							{
								token = "[";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}

							for (i = 0; i < descr.num; i++)
							{
								token = "";
								if (i > 0)
								{
									token += ",";
								}
								token += jvx_int2String(fldI16[i]);
								hdl->in_connect_write_response(server, conn, token.c_str());
							}
							if (descr.num > 1)
							{
								token = "]";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}
							break;
						case JVX_DATAFORMAT_32BIT_LE:

							token = ",\"value\":";
							hdl->in_connect_write_response(server, conn, token.c_str());
							if (descr.num > 1)
							{
								token = "[";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}

							for (i = 0; i < descr.num; i++)
							{
								token = "";
								if (i > 0)
								{
									token += ",";
								}
								token += jvx_int2String(fldI32[i]);
								hdl->in_connect_write_response(server, conn, token.c_str());
							}

							if (descr.num > 1)
							{
								token = "]";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}
							break;
						case JVX_DATAFORMAT_64BIT_LE:

							token = ",\"value\":";
							hdl->in_connect_write_response(server, conn, token.c_str());
							if (descr.num > 1)
							{
								token = "[";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}

							for (i = 0; i < descr.num; i++)
							{
								token = "";
								if (i > 0)
								{
									token += ",";
								}
								token += "\"" + jvx_int642String(fldI64[i]) + "\"";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}
							if (descr.num > 1)
							{
								token = "]";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}
							break;

						case JVX_DATAFORMAT_SELECTION_LIST:

							token = ",\"entries\":[";
							hdl->in_connect_write_response(server, conn, token.c_str());

							for (i = 0; i < lst.strList.ll(); i++)
							{
								token = "\"";
								token += lst.strList.std_str_at(i);
								token += "\"";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}
							token = "],";
							hdl->in_connect_write_response(server, conn, token.c_str());

							token = "\"value\":\"";
							token += jvx_bitField2String(lst.bitFieldSelected());
							token += "\"";
							hdl->in_connect_write_response(server, conn, token.c_str());
							break;

						case JVX_DATAFORMAT_STRING:

							token = ",\"value\":\"";
							token += fldStr.std_str();
							token += "\"";
							hdl->in_connect_write_response(server, conn, token.c_str());

							break;
						case JVX_DATAFORMAT_STRING_LIST:
							token = ",\"value\":[";
							hdl->in_connect_write_response(server, conn, token.c_str());

							for (i = 0; i < lst.strList.ll(); i++)
							{
								token = "\"";
								token += lst.strList.std_str_at(i);
								token += "\"";
								hdl->in_connect_write_response(server, conn, token.c_str());
							}
							token = "],";
							hdl->in_connect_write_response(server, conn, token.c_str());

							break;
						case JVX_DATAFORMAT_VALUE_IN_RANGE:
							token = ",\"value\":";
							token += "\"" + jvx_data2String(fldValRange.val(), JVX_NUMBER_DIGITS_FLOAT) + "\"";
							hdl->in_connect_write_response(server, conn, token.c_str());

							token = "\",min\":";
							token += "\"" + jvx_data2String(fldValRange.minVal, JVX_NUMBER_DIGITS_FLOAT) + "\"";
							hdl->in_connect_write_response(server, conn, token.c_str());

							token = "\",max\":";
							token += "\"" + jvx_data2String(fldValRange.maxVal, JVX_NUMBER_DIGITS_FLOAT) + "\"";
							hdl->in_connect_write_response(server, conn, token.c_str());

						default:
							assert(0);
						}
						if (copyPtr)
						{
							JVX_SAFE_DELETE_FLD(copyPtr, jvxByte);
						}

						writeResponseLeadOut(server, conn);
					}
					else
					{
						writeResponseLeadIn(server, conn, false, "Failed to get property from component.");
						writeResponseLeadOut(server, conn);
					}
				}
				else
				{
					writeResponseLeadIn(server, conn, false, "Failed to get property number of entries from component.");
					writeResponseLeadOut(server, conn);
				}
			}
			else
			{
				writeResponseLeadIn(server, conn, false, "Failed to get property specification from component.");
				writeResponseLeadOut(server, conn);
			}
		}
		else
		{
			writeResponseLeadIn(server, conn, false, "Failed to get property reference for component.");
			writeResponseLeadOut(server, conn);
		}
	}
	return JVX_NO_ERROR;
};

jvxErrorType
CjvxWebServerHost::response_properties__set_content(jvxHandle* server, jvxHandle* conn, jvxComponentIdentification compTp, std::string descriptor, std::string value)
{
	return JVX_NO_ERROR;
};

std::string
CjvxWebServerHost::extractStringParameters(const std::string& in_params, const char* loofor)
{
	std::string retVal = "notfound";
	jvxSize sz;
	jvxApiString  fldStr;
	jvxErrorType resL = JVX_NO_ERROR;
	char oneToken[JVX_MAXSTRING];
	memset(oneToken, 0, JVX_MAXSTRING);
	sz = JVX_MAXSTRING - 1;
	resL = hdl->in_connect_extract_var_content(in_params.c_str(), in_params.size(), loofor, oneToken, &sz);
	if (resL == JVX_NO_ERROR)
	{
		retVal = oneToken;
		hdl->in_connect_url_decode(retVal.c_str(), JVX_SIZE_INT(retVal.size()), &fldStr);

		retVal = fldStr.std_str();
	}
	return(retVal);
};

/*
 * I think no longer needed
jvxErrorType CjvxWebServerHost::trigger_stop_all_websockets()
{
	jvxSize i;
	// This is a little bit of a hack: we bypass the official civetweb api
	// to only shutdown the socket - the connection is kept.
	// As a result, the recv thread should return and shutdown comes immediately and
	// requires no incoming packet. DOES THIS WORK IN LINUX?
	for(i = 0; i < theWebserver.openConnectionsWs.size(); i++)
	{
		hdl->trigger_stop_single_websocket(theWebserver.openConnectionsWs[i].theServer, theWebserver.openConnectionsWs[i].selector[0]);
	}
	return(JVX_NO_ERROR);
};
*/

jvxErrorType CjvxWebServerHost::check_status_all_websockets_closed()
{
	if (theWebserver.openConnectionsWs.size() == 0)
	{
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_NOT_READY);
};

