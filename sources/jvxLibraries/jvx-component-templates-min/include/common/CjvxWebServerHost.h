#ifndef __CJVXWEBSERVERHOST_H__
#define __CJVXWEBSERVERHOST_H__

#include "common/TjvxWebServerHost.h"
#include <iostream>
#include <map>

#ifndef JVX_NUMBER_DIGITS_FLOAT
#define JVX_NUMBER_DIGITS_FLOAT 4
#endif

/*
#define JVX_NUMBER_CHAR_REPLACE 3

static char JVX_CHAR_REPLACE_THIS[JVX_NUMBER_CHAR_REPLACE] =
{
' ', '+', '-'
};

static const char* JVX_CHAR_REPLACE_BY[JVX_NUMBER_CHAR_REPLACE] =
{
"_", "_", "_"
};
*/

class IjvxWebServerHost_hooks
{
public:

	~IjvxWebServerHost_hooks(){};

	virtual jvxErrorType synchronizeWebServerCoEvents(jvxHandle* context_server, jvxHandle* context_conn, 
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
		jvxBool strictConstConnection, const char* uriprefix) = 0;

	virtual jvxErrorType synchronizeWebServerWsEvents(jvxHandle* context_server, jvxHandle* context_conn, 
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
		jvxBool strictConstConnection, const char* uriprefix, int header, char *payload, size_t szFld) = 0;

	virtual jvxErrorType hook_safeConfigFile() = 0;
	virtual jvxErrorType hook_startSequencer() = 0;
	virtual jvxErrorType hook_stopSequencer() = 0;

	virtual jvxErrorType hook_operationComplete() = 0;

	// If support for web sockets...
/*	virtual jvxErrorType hook_wsConnect(jvxHandle* context_server, jvxHandle* context_conn){return(JVX_ERROR_UNSUPPORTED);};
	virtual jvxErrorType hook_wsReady(jvxHandle* context_server, jvxHandle* context_conn){return(JVX_ERROR_UNSUPPORTED);};
	virtual jvxErrorType hook_wsData(jvxHandle* context_server, jvxHandle* context_conn, int header, char *payload, size_t szFld){return(JVX_ERROR_UNSUPPORTED);};
	virtual jvxErrorType hook_wsClose(jvxHandle* context_server, jvxHandle* context_conn){return(JVX_ERROR_UNSUPPORTED);};
	*/
	virtual jvxErrorType hook_ws_started() = 0;
	virtual jvxErrorType hook_ws_stopped() = 0;
};



class CjvxWebServerHost: public IjvxWebServerCo_report, public IjvxWebServerWs_report
{
protected:

	struct oneEntryReference
	{
		jvxSize linkToHelper;
		jvxSize identify;
	};

	IjvxWebServer* hdl;
	IjvxHost* hostHdl;

	struct
	{
		std::map<jvxSize, oneEntryReference> theIds;
	} notifyCoIds;

	struct
	{
		std::map<jvxSize, oneEntryReference> theIds;
	} notifyWsIds;

	IjvxWebServerHost_hooks* theHooks;

	typedef struct
	{
		std::string prop_descr;
		jvxComponentIdentification selector[1];
		jvxSize idClient;
		jvxBool validPropertyHandle;
		jvxPropertyDescriptor descriptorPropertyHandle;
		jvxHandle* bufSend;
		jvxSize bufSize;
	} oneWsPropertyStream;

	typedef struct
	{
		jvxHandle* theServer;
		jvxHandle* selector[1];
		jvxBool isConnected;
		std::vector<oneWsPropertyStream> theStreams;
	} oneWsConnection;

	struct
	{		
		jvxSize uniqueId;
		std::vector<oneWsConnection> openConnectionsWs;
	} theWebserver;

public:

	CjvxWebServerHost();

	jvxErrorType start_webserver(IjvxWebServer* server, IjvxHost* hostRef,
		IjvxWebServerHost_hooks* theHooksPass = NULL);

	jvxErrorType stop_webserver();

	jvxErrorType report_server_co_event(
		jvxHandle* context_server, jvxHandle* context_conn,
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
		jvxBool strictConstConnection, const char* uriprefix);

	jvxErrorType report_server_ws_event(
		jvxHandle* context_server, jvxHandle* context_conn,
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
		jvxBool strictConstConnection, const char* uriprefix, int header, char *payload, size_t szFld);

	jvxErrorType report_event_request_core(
		jvxHandle* context_server, jvxHandle* context_conn,
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
		jvxBool strictConstConnection, const char* uriprefix, int header, char *payload, size_t szFld);

	jvxErrorType request_event_information(
		jvxHandle* context_server, jvxHandle* context_conn, jvxApiString*  uri,
		jvxApiString* query, jvxApiString* local_uri, jvxApiString* origin, jvxApiString* user);

	jvxErrorType report_event_request_translate(
		jvxHandle* context_server, jvxHandle* context_conn,
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
		jvxBool strictConstConnection, const char* uriprefix, int header,
		char *payload_ws, size_t szFld,
		std::string& command, jvxBool& requiresInterpretation,
		jvxBool& errorDetected, std::ostream* os, jvx_lock* jvxlock, jvxBool silentMode);

	// Web socket handling	
	jvxErrorType hook_wsConnect(jvxHandle* context_server, jvxHandle* context_conn);
	
	jvxErrorType hook_wsReady(jvxHandle* context_server, jvxHandle* context_conn);

	jvxErrorType hook_wsData(jvxHandle* context_server, jvxHandle* context_conn, int flags, char *payload, size_t szFld);

	jvxErrorType hook_wsClose(jvxHandle* context_server, jvxHandle* context_conn);

	virtual void right_before_start();

	void postPropertyStreams();

	// ======================================================================

	jvxErrorType componentTargetStringToComponentType(std::string componentToken, jvxComponentIdentification& cpType);

	jvxErrorType operationStringToStateSwitchType(std::string operToken, jvxStateSwitch& ssType);

	void writeResponseLeadIn(jvxHandle* server, jvxHandle* conn, jvxBool callResult = true, std::string err = "--");

	void writeResponseLeadOut(jvxHandle* server, jvxHandle* conn);

	// ======================================================================

	// +++

	virtual jvxErrorType response_host__state(jvxHandle* server, jvxHandle* conn);

	virtual jvxErrorType response_host__safe_config(jvxHandle* server, jvxHandle* conn);

	// +++

	virtual jvxErrorType response_components__description(jvxHandle* server, jvxHandle* conn, jvxComponentIdentification compTp);

	virtual jvxErrorType response_components__state(jvxHandle* server, jvxHandle* conn, jvxComponentIdentification compTp);

	virtual jvxErrorType response_components__switch_state(jvxHandle* server, jvxHandle* conn, jvxComponentIdentification compTp, 
		jvxStateSwitch operation, jvxInt32 idx);

	virtual jvxErrorType response_sequencer__state(jvxHandle* server, jvxHandle* conn);

	// +++

	virtual jvxErrorType response_sequencer__start(jvxHandle* server, jvxHandle* conn);

	virtual jvxErrorType response_sequencer__stop(jvxHandle* server, jvxHandle* conn);

	// +++

	virtual jvxErrorType response_properties__description(jvxHandle* server, jvxHandle* conn, jvxComponentIdentification compTp);

	virtual jvxErrorType response_properties__get_content(jvxHandle* server, jvxHandle* conn, jvxComponentIdentification compTp, std::string descriptor, jvxBool contentOnly);

	virtual jvxErrorType response_properties__set_content(jvxHandle* server, jvxHandle* conn, jvxComponentIdentification compTp, std::string descriptor, std::string value);

	std::string extractStringParameters(const std::string& in_params, const char* loofor);

	/*
	 * I think no longer needed 
	virtual jvxErrorType trigger_stop_all_websockets()
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

	virtual jvxErrorType check_status_all_websockets_closed();
};

#endif
