#ifndef __IJVXHTTPAPI_H__
#define __IJVXHTTPAPI_H__

JVX_INTERFACE IjvxHttpApi_httpreport
{
public:
	virtual ~IjvxHttpApi_httpreport() {};
	virtual jvxErrorType complete_transfer_http(jvxSize uniqueIdGroup, jvxSize uniqueId_ingroup, const char* response, jvxHandle* priv, jvxCBitField reqId) = 0;
	virtual jvxErrorType failed_transfer_http(jvxSize uniqueIdGroup, jvxSize uniqueId_ingroup, jvxErrorType errRes, jvxHandle* priv, jvxCBitField reqId) = 0;
};

JVX_INTERFACE IjvxHttpApi_wsreport
{
public:
	virtual ~IjvxHttpApi_wsreport() {};
	virtual jvxErrorType websocket_connected(jvxSize uId, jvxHandle* priv) = 0;
	virtual jvxErrorType websocket_error(jvxSize uId, jvxHandle* priv, const char* descrErr) = 0;
	virtual jvxErrorType websocket_disconnected(jvxSize uId, jvxHandle* priv) = 0;
	virtual jvxErrorType websocket_data_binary(jvxSize uId, jvxByte* payload, jvxSize szFld, jvxHandle* priv) = 0;
};

JVX_INTERFACE IjvxHttpApi
{
public:
	virtual ~IjvxHttpApi() {};

	virtual jvxErrorType request_transfer_get(const std::string& uri, jvxSize GuniqueId, jvxSize IGuniqueId, IjvxHttpApi_httpreport* rep, jvxHandle* priv) = 0;
	virtual jvxErrorType request_transfer_put(const std::string& uri, const std::string& content, jvxSize GuniqueId, jvxSize IGuniqueId, IjvxHttpApi_httpreport* rep, jvxHandle* priv) = 0;
	virtual jvxErrorType request_transfer_post(const std::string& uri, const std::string& content, jvxSize GuniqueId, jvxSize IGuniqueId, IjvxHttpApi_httpreport* rep, jvxHandle* priv) = 0;

	virtual jvxErrorType request_connect_websocket(const std::string& uri, jvxSize* uId, IjvxHttpApi_wsreport* rep, jvxHandle* priv) = 0;
	virtual jvxErrorType request_disconnect_websocket(jvxSize uId) = 0;
	virtual jvxErrorType request_transfer_websocket(jvxSize uId, jvxWebServerWebSocketFrameType opcode, const char* hdr) = 0;

	virtual jvxErrorType request_kill_all() = 0;
	
	virtual jvxErrorType blockCheckPending(jvxBool* isPending) = 0; 
	virtual jvxErrorType unblockCheckPending() = 0; 
};

#endif
