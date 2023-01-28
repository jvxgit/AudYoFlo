#ifndef __CJVXSOCKETSAPI_H__
#define __CJVXSOCKETSAPI_H__

enum class jvxSocketsReportType
{
	JVX_SOCKET_REPORT_NONE,
	JVX_SOCKET_REPORT_PCAP_RAW
};

enum class jvxSocketsFamily
{
	JVX_SOCKET_IP_V4,
	JVX_SOCKET_IP_V6,
	JVX_SOCKET_IP_UNIX
};

enum class jvxSocketsErrorType
{
	JVX_SOCKET_NO_ERROR,
	JVX_SOCKET_ERROR_INIT_FAILED,
	JVX_SOCKET_ERROR_SETOPT_I_FAILED,
	JVX_SOCKET_ERROR_SETOPT_II_FAILED,
	JVX_SOCKET_ERROR_BIND_FAILED,
	JVX_SOCKET_ERROR_LISTEN_FAILED,
	JVX_SOCKET_ERROR_ACCEPT_FAILED,
	JVX_SOCKET_ERROR_RESOLVE_HOSTNAME_FAILED,
	JVX_SOCKET_ERROR_GENERAL_CONNECTION_FAILED,
	JVX_SOCKET_ERROR_TIMEOUT,
	JVX_SOCKET_ERROR_REFUSED,
	JVX_SOCKET_ERROR_ENDED,
	JVX_SOCKET_ERROR_RESOLVE_HOSTNAME,
	JVX_SOCKET_ERROR_GENERAL_CONNECTION,
	JVX_SOCKET_ERROR_CONNECTION_RESET,
	JVX_SOCKET_ERROR_MISC
};

enum class jvxSocketsConnectionType
{
	JVX_SOCKET_TYPE_TCP,
	JVX_SOCKET_TYPE_UDP,
	JVX_SOCKET_TYPE_UNIX,
#ifdef JVX_WITH_PCAP
	JVX_SOCKET_TYPE_PCAP,
#endif
	JVX_SOCKET_TYPE_BTH
};

enum class jvxSocketsConnectionTransfer
{
	JVX_SOCKET_TRANSFER_NORMAL,
	JVX_SOCKET_TRANSFER_UDP
};

enum class jvxSocketsConnectionState
{
	JVX_STATE_CONNECTION_INIT,
	JVX_STATE_CONNECTION_STARTED,
	JVX_STATE_CONNECTION_CONNECTED,
	JVX_STATE_CONNECTION_DISCONNECTED
};

// ================================================================

JVX_INTERFACE IjvxSocketsConnection_transfer
{
public:
	~IjvxSocketsConnection_transfer() {};
	virtual jvxErrorType get_data(jvxByte** buf_send, jvxSize* numElements) = 0;
	virtual jvxSocketsConnectionTransfer type() = 0;
	virtual jvxErrorType specialization(jvxHandle** ptr, jvxSocketsConnectionTransfer tp) = 0;
};

JVX_INTERFACE IjvxSocketsConnection_transfer_udp
{
public:
	virtual ~IjvxSocketsConnection_transfer_udp() {};
	virtual jvxErrorType udp_force_new_target(jvxBool* forceUpdate) = 0;
	virtual jvxErrorType udp_get_new_target(jvxApiString* target, jvxInt32* portid) = 0;
};

// ================================================================

JVX_INTERFACE IjvxSocketsConnection_report;

JVX_INTERFACE IjvxSocketsConnection
{
public:
	virtual ~IjvxSocketsConnection() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION register_report(IjvxSocketsConnection_report* report) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_report(IjvxSocketsConnection_report* report) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer(IjvxSocketsConnection_transfer* oneBuf, jvxSize* num_sent) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect() = 0;
};

JVX_INTERFACE IjvxSocketsServer
{
public:
	virtual ~IjvxSocketsServer() {};
};

// ================================================================

JVX_INTERFACE IjvxSocketsAdditionalInfoPacket
{
public:
	virtual ~IjvxSocketsAdditionalInfoPacket() {};
};

// ================================================================

JVX_INTERFACE IjvxSocketsConnection_report
{
public:
	virtual ~IjvxSocketsConnection_report() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION provide_data_and_length(char** fld_use, jvxSize* lenField, jvxSize* offset, IjvxSocketsAdditionalInfoPacket* additionalInfo) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_data_and_read(char* fld_filled, jvxSize lenField, jvxSize offset, 
		IjvxSocketsAdditionalInfoPacket* additionalInfo) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_connection_error(jvxSocketsErrorType errCode, const char* errDescription) = 0;
	// virtual jvxErrorType JVX_CALLINGCONVENTION report_connection_specific(jvxSocketReportType tp, jvxHandle* fld, jvxSize szFld) = 0;
};

JVX_INTERFACE IjvxSocketsClient_report
{
public:
	virtual ~IjvxSocketsClient_report() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION report_client_connect(IjvxSocketsConnection* connection) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_client_disconnect(IjvxSocketsConnection* connection) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_client_error(jvxSocketsErrorType err, const char* description) = 0;
};

JVX_INTERFACE IjvxSocketsServer_report
{
public:
	virtual ~IjvxSocketsServer_report() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_denied(const char* description) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_connect(const char* description, IjvxSocketsConnection* newConnection) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_disconnect(const char* description, IjvxSocketsConnection* newConnection) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_error(jvxSocketsErrorType err, const char* description) = 0;
};

#endif
