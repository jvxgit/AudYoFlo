#ifndef __JVX_CONNECT_SERVER_ST_H__
#define __JVX_CONNECT_SERVER_ST_H__

#include "jvx.h"
#include "jvxLsocket_common.h"

#ifndef JVX_OS_WINDOWS
#define JVX_PCAP_DIRECT_SEND
#endif


struct jvx_connect_server_if;
struct jvx_connect_server_family;

typedef struct
{

	jvx_connect_callback_type common;
	jvxErrorType (*callback_report_denied_incoming)(jvxHandle* privateData, const char* description, jvxSize ifidx);
	jvxErrorType (*callback_report_connect_incoming)(jvxHandle* privateData, jvxSize channelId, const char* description, jvxSize ifidx);
	jvxErrorType (*callback_report_disconnect_incoming)(jvxHandle* privateData, jvxSize channelId, jvxSize ifidx);

} jvx_connect_server_callback_type;


/** API FUNCTIONS **/
jvxErrorType jvx_connect_server_family_initialize_st(jvx_connect_server_family** hdlFamilyReturn,
	jvx_connect_socket_type socket_type, jvxHandle* config_specific = NULL, jvxConnectionPrivateTypeEnum whatsthis = JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);

jvxErrorType jvx_connect_server_family_number_interfaces_st(jvx_connect_server_family* hdlFamily, jvxSize* num);

jvxErrorType jvx_connect_server_family_name_interfaces_st(jvx_connect_server_family* hdlFamily, char* fld, jvxSize szFld, jvxSize idx);

jvxErrorType jvx_connect_server_family_description_interfaces_st(jvx_connect_server_family* hdlFamily, char* fld, jvxSize szFld, jvxSize idx);

jvxErrorType jvx_connect_server_family_terminate_st(jvx_connect_server_family* hdlFamily);


jvxErrorType jvx_connect_server_initialize_st(jvx_connect_server_family* hdlFamily, 
	jvx_connect_server_if** hdlServerOnReturn,
	jvx_connect_server_callback_type* report_callback,
	jvxHandle* privData, 
	jvxInt32 numConnectionsMax, 
	jvxSize idx);

jvxErrorType jvx_connect_server_terminate_st(jvx_connect_server_if* hdlServer);

jvxErrorType jvx_connect_server_start_listening_st(jvx_connect_server_if* hdlServer,
								   jvxHandle* config_specific, jvxConnectionPrivateTypeEnum whatsthis);

jvxErrorType jvx_connect_server_whoami_st(jvx_connect_server_if* hdlServer, char* whoami_name, jvxSize szText_name, jvx_connect_descriptor_get whattoget);

jvxErrorType jvx_connect_server_identify_st(jvx_connect_server_if* hdlServer, char* whoami_name, jvxSize szText_name, const char* whoami_ip);

jvxErrorType jvx_connect_server_field_constraint_st(jvx_connect_server_if* hdlServer, jvxSize* bytesPrepend, jvxSize* fldMinSize, jvxSize* fldMaxSize);

jvxErrorType jvx_connect_server_prepare_udp_target_st(jvx_connect_server_if* hdlServer, const char* server, int port, jvx_udp_target* theTarget, jvxConnectionIpType ipProt);


jvxErrorType jvx_connect_server_send_st(jvx_connect_server_if* hdlServer, jvxSize idChannel, char* buf, jvxSize* szFld, jvxHandle* wheretosendto, jvxConnectionPrivateTypeEnum whatsthis);

jvxErrorType jvx_connect_server_set_thread_priority(jvx_connect_server_if* hdlServer, JVX_THREAD_PRIORITY prio);


jvxErrorType jvx_connect_server_start_queue_st(jvx_connect_server_if* hdlServer, jvxSize idChannel, jvxSize szFld, jvxSize numFldsQ);

jvxErrorType jvx_connect_server_collect_st(jvx_connect_server_if* hdlServer, jvxSize idChannel, char* buf, jvxSize* szFld, jvxHandle* wheretosendto, jvxConnectionPrivateTypeEnum whatsthis);

jvxErrorType jvx_connect_server_transmit_st(jvx_connect_server_if* hdlServer, jvxSize idChannel);

jvxErrorType jvx_connect_server_stop_queue_st(jvx_connect_server_if* hdlServer, jvxSize idChannel);


jvxErrorType jvx_connect_server_stop_one_connection_st(jvx_connect_server_if* hdlServer, jvxSize idConnection);

jvxErrorType jvx_connect_server_optional_wait_for_shutdown_st(jvx_connect_server_if* hdlServer);

jvxErrorType jvx_connect_server_trigger_shutdown_st(jvx_connect_server_if* hdlServer);

jvxErrorType jvx_connect_server_get_state_st(jvx_connect_server_if* hdlServer, jvx_connect_state* state_on_return);

// jvxErrorType jvx_connect_server_terminate_st(jvx_connect_server_if* hdlServer);

jvxErrorType jvx_connect_server_lock_state_st(jvx_connect_server_if* hdlServer);

jvxErrorType jvx_connect_server_unlock_state_st(jvx_connect_server_if* hdlServer);

// ==============================================================================

#define JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_SIMPLE(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData_thread, jvxSize ifidx); \
	jvxErrorType ic_callback_ ## fname(jvxSize ifidx);

#define JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_PROVIDE_DATA(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData_thread, jvxSize channelId, char** bufptr, jvxSize* szbuf, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis); \
	jvxErrorType ic_callback_ ## fname(jvxSize channelId, char** bufptr, jvxSize* szbuf, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);

#define JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_DATA_READ(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData, jvxSize channelId, char* buf, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis); \
	jvxErrorType ic_callback_ ## fname(jvxSize channelId, char* buf, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);

#define JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_ERROR(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx); \
	jvxErrorType ic_callback_ ## fname(jvxInt32 errCode, const char* errDescription, jvxSize ifidx);

#define JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_SPECIFIC(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData, jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx); \
	jvxErrorType ic_callback_ ## fname(jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx);

#define JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_CONNECT_IN(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData, jvxSize channelId, const char* description, jvxSize ifidx); \
	jvxErrorType ic_callback_ ## fname(jvxSize channelId, const char* description, jvxSize ifidx);

#define JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_DENIED_IN(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData, const char* description, jvxSize ifidx); \
	jvxErrorType ic_callback_ ## fname(const char* description, jvxSize ifidx);

#define JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_DISCONNECT_IN(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData, jvxSize channelId, jvxSize ifidx); \
	jvxErrorType ic_callback_ ## fname(jvxSize channelId, jvxSize ifidx);


#define JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_ALL \
	JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_SIMPLE(startup_complete) \
	JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_SIMPLE(shutdown_complete) \
	JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_PROVIDE_DATA(provide_data_and_length) \
	JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_DATA_READ(data_and_read) \
	JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_ERROR(report_error) \
	JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_SPECIFIC(specific) \
	JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_CONNECT_IN(connect_incoming) \
	JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_DENIED_IN(denied_incoming) \
	JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_DISCONNECT_IN(disconnect_incoming)

#define JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_ASSIGN(var, cname, startup_complete, shutdown_complete, provide_data_and_length, data_and_read, report_error, specific, \
		connect_incoming, disconnect_incoming, denied_incoming) \
	var.common.callback_report_startup_complete = cname::callback_ ## startup_complete; \
	var.common.callback_report_shutdown_complete = cname::callback_ ## shutdown_complete; \
	var.common.callback_provide_data_and_length = cname::callback_ ## provide_data_and_length; \
	var.common.callback_report_data_and_read = cname::callback_ ## data_and_read; \
	var.common.callback_report_error = cname::callback_ ## report_error; \
	var.common.callback_report_specific = cname::callback_ ## specific; \
	var.callback_report_denied_incoming = cname::callback_ ## denied_incoming; \
	var.callback_report_connect_incoming = cname::callback_ ## connect_incoming; \
	var.callback_report_disconnect_incoming = cname::callback_ ## disconnect_incoming;


// ===============================================================

/*
jvxErrorType(*callback_report_startup_complete)(jvxHandle* privateData, jvxSize ifidx);
jvxErrorType(*callback_report_shutdown_complete)(jvxHandle* privateData, jvxSize ifidx);
jvxErrorType(*callback_provide_data_and_length)(jvxHandle* privateData, jvxSize channelId, char**, jvxSize*, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);
jvxErrorType(*callback_report_data_and_read)(jvxHandle* privateData, jvxSize channelId, char*, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);
jvxErrorType(*callback_report_error)(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx);
jvxErrorType(*callback_report_specific)(jvxHandle* privateData, jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx);


*/
#define JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_SIMPLE(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvxSize ifidx) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname(ifidx)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvxSize ifidx) 

#define JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_PROVIDE_DATA(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvxSize channelId, char** bufptr, jvxSize* szbuf, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname( channelId,  bufptr,  szbuf, offset, idIf, additionalInfo, whatsthis)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvxSize channelId, char** bufptr, jvxSize* szbuf, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis) 

#define JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_DATA_READ(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvxSize channelId, char* buf, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname( channelId, buf, sz, offset, idIf, additionalInfo, whatsthis)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvxSize channelId, char* buf, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis) 

#define JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_ERROR(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname( errCode, errDescription,  ifidx)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvxInt32 errCode, const char* errDescription, jvxSize ifidx) 

#define JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_SPECIFIC(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname( idWhat,  fld,  szFld,  ifidx)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx) 

#define JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_DENIED_IN(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, const char* description, jvxSize ifidx) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname(description,  ifidx)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(const char* description, jvxSize ifidx) 

#define JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_CONNECT_IN(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvxSize channelId, const char* description, jvxSize ifidx) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname( channelId, description, ifidx)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvxSize channelId, const char* description, jvxSize ifidx) 

#define JVX_SERVER_SOCKET_C_CALLBACK_DEFINE_FUNC_DISCONNECT_IN(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvxSize channelId, jvxSize ifidx) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname( channelId, ifidx)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvxSize channelId, jvxSize ifidx) 

#endif
