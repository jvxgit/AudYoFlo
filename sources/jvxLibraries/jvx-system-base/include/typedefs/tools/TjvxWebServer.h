#ifndef __TJVXWEBSERVER_H__
#define __TJVXWEBSERVER_H__
	
typedef enum 
{
	JVX_WEB_SERVER_RESPONSE_UNKNOWN = 0,
	JVX_WEB_SERVER_RESPONSE_JSON = 1
} jvxWebServerResponseType;

typedef enum
{
	JVX_WEB_SERVER_UNKNOWN_HANDLER_SHIFT = 0,
	JVX_WEB_SERVER_SYSTEM_REQUEST_HANDLER_SHIFT = 0,
	JVX_WEB_SERVER_URI_GET_HANDLER_SHIFT = 1,
	JVX_WEB_SERVER_URI_POST_HANDLER_SHIFT = 2,
	JVX_WEB_SERVER_URI_PUT_HANDLER_SHIFT = 3,

	JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_SHIFT = 4,
	JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_SHIFT = 5,
	JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_SHIFT = 6,
	JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_SHIFT = 7
}jvxWebServerHandlerPurposeShift;

typedef enum
{
	JVX_WEB_SERVER_UNKNOWN_HANDLER_PURP = 0,
	JVX_WEB_SERVER_SYSTEM_REQUEST_HANDLER_PURP = (1 << JVX_WEB_SERVER_SYSTEM_REQUEST_HANDLER_SHIFT),
	JVX_WEB_SERVER_URI_GET_HANDLER_PURP = (1 << JVX_WEB_SERVER_URI_GET_HANDLER_SHIFT),
	JVX_WEB_SERVER_URI_POST_HANDLER_PURP = (1 << JVX_WEB_SERVER_URI_POST_HANDLER_SHIFT),
	JVX_WEB_SERVER_URI_PUT_HANDLER_PURP = (1 << JVX_WEB_SERVER_URI_PUT_HANDLER_SHIFT),

	JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_PURP = (1 << JVX_WEB_SERVER_URI_WEBSOCKET_CONNECT_HANDLER_SHIFT),
	JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_PURP = (1 << JVX_WEB_SERVER_URI_WEBSOCKET_READY_HANDLER_SHIFT),
	JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_PURP = (1 << JVX_WEB_SERVER_URI_WEBSOCKET_DATA_HANDLER_SHIFT),
	JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_PURP = (1 << JVX_WEB_SERVER_URI_WEBSOCKET_CLOSE_HANDLER_SHIFT)
}jvxWebServerHandlerPurpose;

typedef enum 
{
	JVX_WEBSOCKET_OPCODE_CONTINUATION = 0x0,
	JVX_WEBSOCKET_OPCODE_TEXT = 0x1,
	JVX_WEBSOCKET_OPCODE_BINARY = 0x2,
	JVX_WEBSOCKET_OPCODE_CONNECTION_CLOSE = 0x8,
	JVX_WEBSOCKET_OPCODE_PING = 0x9,
	JVX_WEBSOCKET_OPCODE_PONG = 0xa
} jvxWebServerWebSocketFrameType;

#endif
