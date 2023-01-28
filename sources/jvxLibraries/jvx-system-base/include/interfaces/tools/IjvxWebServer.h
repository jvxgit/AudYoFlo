#ifndef __IJVXWEBSERVER_H__
#define __IJVXWEBSERVER_H__

JVX_INTERFACE IjvxWebServerCo_report
{
	
public:

	virtual JVX_CALLINGCONVENTION ~IjvxWebServerCo_report(){};
	
	// Callbacks for managing web server events
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_co_event(
		jvxHandle* context_server, jvxHandle* context_conn,
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId, jvxBool strictConstConnection,
		const char* uri_prefix) = 0;

};

JVX_INTERFACE IjvxWebServerWs_report
{

public:

	virtual JVX_CALLINGCONVENTION ~IjvxWebServerWs_report() {};

	// Callbacks for managing web server events
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_ws_event(
		jvxHandle* context_server, jvxHandle* context_conn,
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId, jvxBool strictConstConnection,		
		const char* uri_prefix, int header = 0, char *payload = NULL, size_t szFld = 0) = 0;

};

JVX_INTERFACE IjvxWebServer: public IjvxInterfaceFactory
{
	
public:

	virtual JVX_CALLINGCONVENTION ~IjvxWebServer(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize() = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION register_co_handler(jvxSize* unique_handler_id, 
									jvxCBitField purp, jvxHandle* identification, 									
									IjvxWebServerCo_report* bwd) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION register_ws_handler(jvxSize* unique_handler_id,
		jvxCBitField purp, jvxHandle* identification,
		IjvxWebServerWs_report* bwd) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION start() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_request_specifiers(jvxHandle* server, jvxHandle* conn,
		jvxApiString* request_uri_on_return, jvxApiString* query_on_return,
		jvxApiString* local_uri_on_return, jvxApiString* origin_request_on_return,
		jvxApiString* origin_user) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_write_response(jvxHandle* server, jvxHandle* conn, const char* response_text) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_write_header_response(jvxHandle* server, jvxHandle* conn, jvxWebServerResponseType resp) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_write_packet_websocket(jvxHandle* server, jvxHandle* conn, int opcode, const jvxHandle* dField, jvxSize szField) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_read_content(jvxHandle* server, jvxHandle* conn, jvxHandle* readFld, jvxSize* numBytes) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_extract_var_content(const char* fldStringIn, jvxSize numBytesIn, const char* var_name, char* readFldOut, jvxSize* numBytesOut) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_url_decode(const char *src, int src_len, jvxApiString* result) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_url_encode(const char *src, int src_len, jvxApiString* result) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_send_ws_packet(struct mg_connection* Connection, jvxInt32 Opcode, const char* Data, jvxSize data_len) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_get_ws_ip_addr(struct mg_connection* Connection, jvxApiString* astr) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_drop_connection(struct mg_connection* Connection) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION in_connect_write_error(jvxHandle* conn, int errCode, const char* token) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_handler(jvxSize unique_handler_id) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;

};

#endif