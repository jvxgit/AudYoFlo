#ifndef __TJVXCONNECTION_H__
#define __TJVXCONNECTION_H__

typedef enum
{
	JVX_CONNECTION_REPORT_STARTUP_COMPLETE_SHFT = 0,
	JVX_CONNECTION_REPORT_SHUTDOWN_COMPLETE_SHFT,
	JVX_CONNECTION_REPORT_ERROR_SHFT,

	JVX_CONNECTION_REPORT_SHFT_OFFSET
} jvxConnectionEventEnum;

#define JVX_CONNECT_PRIV_TYPE_ENUM_SUB_OFFSET 128
typedef enum
{
	JVX_CONNECT_PRIVATE_ARG_TYPE_NONE = 0,
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_WHAT_AND_HOW, /* <- jvxSocketAddressType* from TjvxSocket.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SERVER_PRIVATE, /* <- jvx_socket_server_private* from TjvxSocket.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CLIENT_PRIVATE, /* <- jvx_socket_client_private* from TjvxSocket.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SEND_PRIVATE, /* <- jvx_socket_send_private from TjvxSocket.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_SEND_PRIVATE_UDP, /* <- jvx_udp_target from jvxLSocket_common.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_HOSTNAME_PRIVATE,  /* <- jvx_socket_get_host_name_private* from TjvxSocket.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_ERROR, /* <- jvx_socket_error_private* from TjvxSocket.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONNECTION_TCP, /* <- jvx_socket_connection_tcp_private* from TjvxSocket.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_TCP, /* <- jvx_connect_spec_tcp* from jvxLSocket_common.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_UDP, /* <- jvx_connect_spec_udp* from jvxLSocket_common.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_PCAP, /* <- jvx_connect_spec_pcap* from jvxLSocket_common.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_UDP_TARGET, /* <- jvx_udp_target* from jvxLSocket_common.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_PCAP_TARGET, /* <- jvx_connect_send_spec_pcap* from jvxLSocket_common.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_PCAP_DST_SRC_MAC, /* <- jvxByte[12] from containing 6 Bytes src mac + 6 bytes dst mac */

	JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_CONFIG, /* <- jvxRs232Config* from TjvxRs232.h */
	JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_MODEM_STATUS, /* <- jvxUInt16* */
	JVX_CONNECT_PRIVATE_ARG_TYPE_RS232_MODEM_EVENT, /* <- jvxUInt16* */

	JVX_CONNECT_PRIVATE_ARG_TYPE_OUTPUT_STRING, /*<- jvxApiString* */
	JVX_CONNECT_PRIVATE_ARG_TYPE_INPUT_STRING, /*<- const char* */

	JVX_CONNECT_PRIVATE_ARG_TYPE_SECURE_CHANNEL_SPECIAL_CHAR, /* <- jvxByte* to provide special character in case of secure channel packet handling */
	JVX_CONNECT_PRIVATE_ARG_TYPE_CONNECTION_NUM_PORT, /* <- jvxSize* */

} jvxConnectionPrivateTypeEnum;

typedef enum
{
	JVX_IP_NONE = 0,
	JVX_IP_V4,
	JVX_IP_V6,
	JVX_IP_UNIX 
} jvxConnectionIpType;

typedef enum
{
	JVX_CONNECT_THREAD_SINGLE_THREAD = 0,
	JVX_CONNECT_THREAD_MULTI_THREAD = 1
} jvxConnectionThreadingModel;
#endif


