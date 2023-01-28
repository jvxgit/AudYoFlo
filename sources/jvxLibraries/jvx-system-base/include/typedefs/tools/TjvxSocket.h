#ifndef __TJVXSOCKET_H__
#define __TJVXSOCKET_H__

typedef enum
{
	JVX_SOCKET_ADDRESS_NONE = 0,
	JVX_SOCKET_ADDRESS_SERVER = 1,
	JVX_SOCKET_ADDRESS_CLIENT = 2
} jvxSocketAddress;

typedef enum
{
	JVX_SOCKET_OPERATE_VIA_NONE = 0,
	JVX_SOCKET_OPERATE_VIA_TCP = 1,
	JVX_SOCKET_OPERATE_VIA_UDP = 2,
	JVX_SOCKET_OPERATE_VIA_PCAP = 3
} jvxSocketType;

typedef struct
{
	jvxSocketAddress whatToAddress;
	jvxSocketType addressViaWhatProtocol;
} jvxSocketAddressType;

#define JVX_CONNECTION_TAG_SOCKET  "JVX_CONNECTION_SOCKET_COMPONENT"

typedef struct
{
	const char* name;
	int port;
	jvxConnectionIpType family;
} jvx_socket_udp_private;

// ===================================================

typedef struct
{
	jvxSize idConnection;
	const char* origin;
} jvx_socket_connection_tcp_private;

typedef struct
{
	jvxSize idConnection;
	jvxHandle* target;
} jvx_socket_send_private;

// ===================================================

typedef struct
{
	jvxSize numConnectionsMax;
	jvxHandle* priv_start;
	jvxConnectionPrivateTypeEnum priv_type;

} jvx_socket_server_private;


// ===================================================

typedef struct
{
	jvxSize timeOutConnect;
	jvxHandle* priv_start;
	jvxConnectionPrivateTypeEnum priv_type;
} jvx_socket_client_private;

// ===================================================

typedef enum
{
	JVX_SOCKET_SERVER_REPORT_DENIED_INCOMING_SHFT = JVX_CONNECTION_REPORT_SHFT_OFFSET + 0,
	JVX_SOCKET_SERVER_REPORT_CONNECT_INCOMING_SHFT = JVX_CONNECTION_REPORT_SHFT_OFFSET + 1,
	JVX_SOCKET_SERVER_REPORT_DISCONNECT_INCOMING_SHFT = JVX_CONNECTION_REPORT_SHFT_OFFSET + 2
} jvxSocketReportTypes;

typedef enum
{
	JVX_SOCKET_CLIENT_REPORT_UNSUCCESSFUL_OUTGOING_SHFT = JVX_CONNECTION_REPORT_SHFT_OFFSET + 0,
	JVX_SOCKET_CLIENT_REPORT_CONNECT_OUTGOING_SHFT = JVX_CONNECTION_REPORT_SHFT_OFFSET + 1,
	JVX_SOCKET_CLIENT_REPORT_DISCONNECT_OUTGOING_SHFT = JVX_CONNECTION_REPORT_SHFT_OFFSET + 2
} jvxClientReportTypes;

typedef struct
{
	jvxInt32 errCode;
	const char* errDescr;
} jvx_socket_error_private;

typedef enum
{
	JVX_SOCKET_SERVER_CONTROL_NONE = 0,
	JVX_SOCKET_SERVER_CONTROL_GET_FIRST_IP_ADDRESS,
	JVX_SOCKET_SERVER_CONTROL_GET_ALL_IP_ADDRESSES,
	JVX_SOCKET_SERVER_CONTROL_GET_HOSTNAME_FOR_IP_ADDRESS,
	JVX_SOCKET_SERVER_CONTROL_GET_MAC_ADDRESS
} jvxSocketControlCommands;

typedef struct
{
	const char* whoisthat;
	jvxApiString thatis;
} jvx_socket_get_host_name_private;
#endif
