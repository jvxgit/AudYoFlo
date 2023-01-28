#ifndef __CJVXWEBSERVER_H__
#define __CJVXWEBSERVER_H__

#include "jvx.h"
#include "common/CjvxProperties.h"
#include "common/CjvxObject.h"
#include <CivetServer.h>
#include "pcg_exports_webserver.h"

#ifdef JVX_WEBSERVER_VERBOSE_MODE
#include <iostream>
#endif

class jvxOneCallbackEndpoint: public CivetHandler
{
	friend class CjvxWebServer;

private:

	std::string reg_expression_cb;
	jvxSize reg_id;
	IjvxWebServerCo_report* theReport;
	jvxCBitField purp_cb;

public:

	jvxOneCallbackEndpoint(std::string reg_expr, jvxSize id, IjvxWebServerCo_report* rep, jvxCBitField purp)
	{
		reg_expression_cb = reg_expr;
		reg_id = id;
		theReport = rep;
		purp_cb = purp;
	}

	// ==========================================================================

    virtual bool handleGet(CivetServer *server, struct mg_connection *conn) override
    {
		// Call callback
#ifdef JVX_WEBSERVER_VERBOSE_MODE
		std::cout << "--> GET REQUEST: " << std::endl;
#endif
		if(jvx_cbitTest(purp_cb, JVX_WEB_SERVER_URI_GET_HANDLER_SHIFT))
		{
			return(handleRequest(server, conn, JVX_WEB_SERVER_URI_GET_HANDLER_PURP)); 
		}
		return(true);
    }

    virtual bool handlePost(CivetServer *server, struct mg_connection *conn) 
    {
#ifdef JVX_WEBSERVER_VERBOSE_MODE
			std::cout << "--> POST REQUEST: " << std::endl;
#endif

		// Call callback	
		if(jvx_cbitTest(purp_cb, JVX_WEB_SERVER_URI_POST_HANDLER_SHIFT))
		{
			return(handleRequest(server, conn, JVX_WEB_SERVER_URI_POST_HANDLER_PURP)); 
		}
		return(true);
	};

	virtual bool handlePut(CivetServer *server, struct mg_connection *conn)
	{
#ifdef JVX_WEBSERVER_VERBOSE_MODE
		std::cout << "--> PUT REQUEST: " << std::endl;
#endif

		// Call callback	
		if (jvx_cbitTest(purp_cb, JVX_WEB_SERVER_URI_PUT_HANDLER_SHIFT))
		{
			return(handleRequest(server, conn, JVX_WEB_SERVER_URI_PUT_HANDLER_PURP));
		}
		return(true);
	};
	// ==========================================================================
	// ==========================================================================
	// ==========================================================================

	// Request handler functions
	#if 0
	bool handleRequest(CivetServer *server, const mg_connection *conn, jvxWebServerHandlerPurpose purp, int header = 0, char *payload = NULL, size_t szFld = 0) 
	{
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

		if(theReport)
		{
			// Brute force typecast
			res = theReport->report_server_co_event(reinterpret_cast<jvxHandle*>(server), (jvxHandle*)(reinterpret_cast<const jvxHandle*>(conn)), purp, reg_id, 
				true, reg_expression_cb.c_str());
		}	

#ifdef JVX_WEBSERVER_VERBOSE_MODE
		std::cout << std::endl;
#endif

		if(res == JVX_NO_ERROR)
		{
			return(true);
		}
		return(false);
	};
#endif

	bool handleRequest(CivetServer *server, struct mg_connection *conn, jvxWebServerHandlerPurpose purp, int header = 0, char *payload = NULL, size_t szFld = 0) 
	{
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

		if(theReport)
		{
			res = theReport->report_server_co_event(reinterpret_cast<jvxHandle*>(server), reinterpret_cast<jvxHandle*>(conn), purp, 
				reg_id, false, reg_expression_cb.c_str());
		}	

#ifdef JVX_WEBSERVER_VERBOSE_MODE
		std::cout << std::endl;
#endif

		if(res == JVX_NO_ERROR)
		{
			return(true);
		}
		return(false);
	};
};

class jvxOneWsCallbackEndpoint : public CivetWebSocketHandler
{
	friend class CjvxWebServer;

private:

	std::string reg_expression_cb;
	jvxSize reg_id;
	IjvxWebServerWs_report* theReport;
	jvxCBitField purp_cb;
public:

	jvxOneWsCallbackEndpoint(std::string reg_expr, jvxSize id, IjvxWebServerWs_report* rep, jvxCBitField purp)
	{
		reg_expression_cb = reg_expr;
		reg_id = id;
		theReport = rep;
		purp_cb = purp;
	};

	virtual bool handleConnection(CivetServer *server, const struct mg_connection *conn) override
	{
#ifdef JVX_WEBSERVER_VERBOSE_MODE
		std::cout << "--> WEB SOCKET CONNECT REQUEST: " << std::endl;
#endif

		if (jvx_cbitTest(purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_SHIFT))
		{
			return(handleRequest(server, conn, JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_PURP));
		}
		return(true);
	};

	void handleReadyState(CivetServer *server, struct mg_connection *conn) override
	{
#ifdef JVX_WEBSERVER_VERBOSE_MODE
		std::cout << "--> WEB SOCKET READY REQUEST: " << std::endl;
#endif

		if (jvx_cbitTest(purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_SHIFT))
		{
			handleRequest(server, conn, JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_PURP);
		}
	};

	bool handleData(CivetServer *server, struct mg_connection *conn, int header, char *payload, size_t szFld) override
	{
#ifdef JVX_WEBSERVER_VERBOSE_MODE
		std::cout << "--> WEB SOCKET DATA REQUEST: " << std::endl;
#endif

		if (jvx_cbitTest(purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_SHIFT))
		{
			return(handleRequest(server, conn, JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_PURP, header, payload, szFld));
		}
		return(true);
	};

	void handleClose(CivetServer *server, const struct mg_connection *conn) override
	{
#ifdef JVX_WEBSERVER_VERBOSE_MODE
		std::cout << "--> WEB SOCKET CLOSE REQUEST: " << std::endl;
#endif

		if (jvx_cbitTest(purp_cb, JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_SHIFT))
		{
			handleRequest(server, conn, JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_PURP);
		}
	};

	// Request handler functions
	bool handleRequest(CivetServer *server, const mg_connection *conn, jvxWebServerHandlerPurpose purp, int header = 0, char *payload = NULL, size_t szFld = 0)
	{
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

		if (theReport)
		{
			// Brute force typecast
			res = theReport->report_server_ws_event(reinterpret_cast<jvxHandle*>(server), (jvxHandle*)(reinterpret_cast<const jvxHandle*>(conn)), 
				purp, reg_id, true, reg_expression_cb.c_str(), header, payload, szFld);
		}

#ifdef JVX_WEBSERVER_VERBOSE_MODE
		std::cout << std::endl;
#endif

		if (res == JVX_NO_ERROR)
		{
			return(true);
		}
		return(false);
	};
};

// ==============================================================================================

class CjvxWebServer: public IjvxWebServer, public IjvxProperties, public CjvxObject, public CjvxProperties, public genWebserver
{
private:
	std::vector<jvxOneCallbackEndpoint*> coendpoints; 
	std::vector<jvxOneWsCallbackEndpoint*> wsendpoints;
	jvxSize uniqueId;
	CivetServer* myServer;

public:

	CjvxWebServer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	
	~CjvxWebServer();
	
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize()override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION start()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION register_co_handler(jvxSize* unique_handler_id, 
									jvxCBitField purp, jvxHandle* identification, 									
									IjvxWebServerCo_report* bwd) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION register_ws_handler(jvxSize* unique_handler_id,
		jvxCBitField purp, jvxHandle* identification,
		IjvxWebServerWs_report* bwd) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_request_specifiers(jvxHandle* server, jvxHandle* conn,
		jvxApiString* request_uri_on_return, jvxApiString* query_on_return,
		jvxApiString* local_uri_on_return, jvxApiString* origin_request_on_return,
		jvxApiString* origin_user)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_write_response(jvxHandle* server, jvxHandle* conn, const char* response_text)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_write_header_response(jvxHandle* server, jvxHandle* conn, jvxWebServerResponseType resp)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_write_packet_websocket(jvxHandle* server, jvxHandle* conn, int opcode, 
		const jvxHandle* dField, jvxSize szField)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_read_content(jvxHandle* server, jvxHandle* conn, jvxHandle* readFld, jvxSize* numBytes)override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_extract_var_content(const char* fldStringIn, jvxSize numBytesIn, 
		const char* var_name, char* readFldOut, jvxSize* numBytesOut)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_url_decode(const char *src, int src_len, jvxApiString* result)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_url_encode(const char *src, int src_len, jvxApiString* result)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_send_ws_packet(struct mg_connection* Connection, jvxInt32 Opcode, const char* Data, jvxSize data_len) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_get_ws_ip_addr(struct mg_connection* Connection, jvxApiString* astr) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_handler(jvxSize unique_handler_id)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_drop_connection(struct mg_connection* Connection) override;
	//virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_write_error(jvxHandle* conn, int errCode, const char* token) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;

#include "codeFragments/simplify/jvxProperties_simplify.h"

#include "codeFragments/simplify/jvxObjects_simplify.h"

// For all non-object interfaces, return object reference
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

#define JVX_INTERFACE_SUPPORT_PROPERTIES
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
};

#endif