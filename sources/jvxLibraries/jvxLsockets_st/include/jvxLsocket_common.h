#ifndef __JVXLSOCKET_COMMON_H__
#define __JVXLSOCKET_COMMON_H__

typedef enum
{
	JVX_CONNECT_SOCKET_TYPE_NONE = 0,
	JVX_CONNECT_SOCKET_TYPE_TCP,
	JVX_CONNECT_SOCKET_TYPE_UDP,
#ifdef JVX_WITH_PCAP
	JVX_CONNECT_SOCKET_TYPE_PCAP,
#endif
	JVX_CONNECT_SOCKET_TYPE_BTH
} jvx_connect_socket_type;

typedef enum
{
	JVX_ERROR_NO_ERROR = 0,
	JVX_ERROR_ID_CREATE_SOCKET_FAILED = 1,
	JVX_ERROR_ID_CREATE_SOCKET_SETOPT_I = 2,
	JVX_ERROR_ID_CREATE_SOCKET_SETOPT_II = 3,
	JVX_ERROR_ID_RESOLVE_HOSTNAME = 4,
	JVX_ERROR_ID_CREATE_SOCKET_BIND = 5,
	JVX_ERROR_ID_CREATE_SOCKET_LISTEN = 6,
	JVX_ERROR_ID_CREATE_SOCKET_ACCEPT = 7,
	JVX_ERROR_ID_GENERAL_CONNECTION_ERROR = 8,
	JVX_ERROR_ID_GENERAL_CONNECTION_RESET = 9
} jvx_connect_error_codes;

typedef enum
{
	JVX_CONNECT_STATE_NONE = 0,
	JVX_CONNECT_STATE_INIT = 1,
	JVX_CONNECT_STATE_STARTED = 2,
	JVX_CONNECT_STATE_CONNECTED = 3,
	JVX_CONNECT_STATE_LISTENING = 4,
	JVX_CONNECT_STATE_WAIT_FOR_SHUTDOWN = 5 /* must be larger than JVX_CONNECT_STATE_STARTED */
} jvx_connect_state;

typedef enum
{
	JVX_CONNECT_CONNECTION_SUCCESS = 0,
	JVX_CONNECT_CONNECTION_FAILED_TIMEOUT = 1,
	JVX_CONNECT_CONNECTION_FAILED_REFUSED = 2,
	JVX_CONNECT_CONNECTION_FAILED_ENDED = 3,
	JVX_CONNECT_CONNECTION_FAILED_MISC = 4
} jvx_connect_fail_reason;

typedef enum
{
	JVX_CONNECT_SPECIFIC_SUBTYPE_NONE = 0,
	JVX_CONNECT_SPECIFIC_SUBTYPE_PCAP_RAW
}jvx_connect_specific_sub_type;

typedef enum
{
	JVX_CONNECT_SOCKET_NAME_NONE = 0,
	JVX_CONNECT_SOCKET_NAME_ALL_IP_ADDR = 1,
	JVX_CONNECT_SOCKET_NAME_ONE_IP_ADDR = 2,
	JVX_CONNECT_SOCKET_NAME_MY_NAME = 3
} jvx_connect_descriptor_get;

typedef enum
{
	JVX_PCAP_RECEIVE_FILTER_INCOMING_MAC_SHIFT = 0,			// Establish filter to filter out only packets addressing this devices MAC
	JVX_PCAP_RECEIVE_OUTPUT_RAW_SHIFT = 1,					// Output all packets as is - in raw mode - via a "report_specific" callback with id JVX_CONNECT_SPECIFIC_SUBTYPE_PCAP_RAW
	JVX_PCAP_RECEIVE_OUTPUT_REPORT_JVX_PACKETS_SHIFT = 2,	// Report only packets with the Javox Ethernet protocol bytes (J and X)
	JVX_PCAP_RECEIVE_FILTER_INCOMING_ONLY_SHIFT = 3,		// works only in Linux - I suppose
	JVX_PCAP_RECEIVE_FILTER_NO_SELF_CAPTURE_SHIFT = 4		// works only with npcap
} jvx_connect_specific_pcap_receive_mode;

typedef struct
{
	int localPort; /* specify a -1 if no port is specified */
	int connectToPort; /* -1 is not a valid option */
	std::string targetName; 
	jvxConnectionIpType family; 
} jvx_connect_spec_udp;

typedef struct
{
	jvx_connect_spec_udp connect;
	int disableNagleAlgorithmTcp;
} jvx_connect_spec_tcp;

typedef sockaddr_in jvx_udp_target;

typedef struct
{
	jvxUInt8 dest_mac[6];
	jvxSize timeoutConnect_msec;
	jvxBitField receive_mode;
	jvxBitField send_mode;
} jvx_connect_spec_pcap;

typedef struct
{
	jvxUInt8 dest_mac[6];
	jvxBool sendRaw;
} jvx_connect_send_spec_pcap;

typedef struct
{
	jvxErrorType(*callback_report_startup_complete)(jvxHandle* privateData, jvxSize ifidx);
	jvxErrorType(*callback_report_shutdown_complete)(jvxHandle* privateData, jvxSize ifidx);

	jvxErrorType(*callback_provide_data_and_length)(jvxHandle* privateData, jvxSize channelId, char**, jvxSize*, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);
	jvxErrorType(*callback_report_data_and_read)(jvxHandle* privateData, jvxSize channelId, char*, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);

	jvxErrorType(*callback_report_error)(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx);

	jvxErrorType(*callback_report_specific)(jvxHandle* privateData, jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx);
} jvx_connect_callback_type;

typedef enum
{
	JVX_CONNECT_LIB_PRIVATE_ARG_TYPE_NONE = JVX_CONNECT_PRIV_TYPE_ENUM_SUB_OFFSET
} jvx_connect_common_spec_type;

#ifdef OS_SUPPORTS_BTH
typedef struct
{
	const JVX_GUID* guidPtr;
} jvx_connect_spec_bth;
#endif

#endif
