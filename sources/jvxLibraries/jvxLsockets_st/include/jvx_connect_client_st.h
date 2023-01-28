#ifndef __JVX_CONNECT_CLIENT_ST_H__
#define __JVX_CONNECT_CLIENT_ST_H__

#include "jvx.h"
#include "jvxLsocket_common.h"

#ifndef JVX_OS_WINDOWS
#define JVX_PCAP_DIRECT_SEND
#endif

#define JVX_PCAP_SEND_QUEUE_SIZE 10000

struct jvx_connect_client_if;
struct jvx_connect_client_family;

typedef struct
{
	jvx_connect_callback_type common;
	jvxErrorType (*callback_report_unsuccesful_outgoing)( jvxHandle* privateData, jvx_connect_fail_reason reason, jvxSize ifidx);
	jvxErrorType (*callback_report_connect_outgoing)( jvxHandle* privateData, jvxSize ifidx);
	jvxErrorType (*callback_report_disconnect_outgoing)(jvxHandle* privateData, jvxSize ifidx);

} jvx_connect_client_callback_type;


/** API FUNCTIONS **/
jvxErrorType jvx_connect_client_family_initialize_st(jvx_connect_client_family** hdlFamilyReturn, 
								   jvx_connect_socket_type socket_type);

jvxErrorType jvx_connect_client_family_number_interfaces_st(jvx_connect_client_family* hdlFamily, jvxSize* num);

jvxErrorType jvx_connect_client_family_name_interfaces_st(jvx_connect_client_family* hdlFamily, char* fld, jvxSize szFld, jvxSize idx);

jvxErrorType jvx_connect_client_family_description_interfaces_st(jvx_connect_client_family* hdlFamily, char* fld, jvxSize szFld, jvxSize idx);

jvxErrorType jvx_connect_client_family_terminate_st(jvx_connect_client_family* hdlFamily);

// ==================================================================================

jvxErrorType jvx_connect_client_interface_initialize_st(jvx_connect_client_family* hdlFamily,
										   jvx_connect_client_if** hdlClientOut, 
										   jvx_connect_client_callback_type* report_callback,
										   jvxHandle* priv, 
										   jvxSize timeout_ms, 
										   jvxSize idx); 

jvxErrorType jvx_connect_client_interface_connect_st(jvx_connect_client_if* hdlClient, jvxHandle* config_spec, jvxConnectionPrivateTypeEnum whatsthis);

jvxErrorType jvx_connect_client_interface_terminate_st(jvx_connect_client_if* hdlClient);


jvxErrorType jvx_connect_client_set_thread_priority_st(jvx_connect_client_if* hdlClient, JVX_THREAD_PRIORITY prio);

jvxErrorType jvx_connect_client_field_constraint_st(jvx_connect_client_if* hdlClient, jvxSize* bytesPrepend, jvxSize* fldMinSize, jvxSize* fldMaxSize);

jvxErrorType jvx_connect_client_prepare_udp_target_st(jvx_connect_client_if* hdlClient, const char* server, int port, jvx_udp_target* theTarget, jvxConnectionIpType ipProt);

jvxErrorType jvx_connect_client_send_st(jvx_connect_client_if* hdlClient, char* buf, jvxSize* szFld, jvxHandle* wheretosendto, jvxConnectionPrivateTypeEnum whatsthis);


jvxErrorType jvx_connect_client_start_queue_st(jvx_connect_client_if* hdlClient, jvxSize szFld, jvxSize numFldsQ);

jvxErrorType jvx_connect_client_collect_st(jvx_connect_client_if* hdlClient, char* buf, jvxSize* szFld, jvxHandle* wheretosendto, jvxConnectionPrivateTypeEnum whatsthis);

jvxErrorType jvx_connect_client_transmit_st(jvx_connect_client_if* hdlClient);

jvxErrorType jvx_connect_client_stop_queue_st(jvx_connect_client_if* hdlClient);


jvxErrorType jvx_connect_client_whoami_st(jvx_connect_client_if* hdlClient, char* whoami_name, jvxSize szText_name, jvx_connect_descriptor_get whattoget);

jvxErrorType jvx_connect_client_identify_st(jvx_connect_client_if* hdlClient, char* thatis, jvxSize szText, const char* whoisthat);


jvxErrorType jvx_connect_client_trigger_shutdown_st(jvx_connect_client_if* hdlClient);

jvxErrorType jvx_connect_client_optional_wait_for_shutdown_st(jvx_connect_client_if* hdlClient);


jvxErrorType jvx_connect_client_get_state_st(jvx_connect_client_if* hdlClient, jvx_connect_state* state_on_return);

jvxErrorType jvx_connect_client_lock_state_st(jvx_connect_client_if* hdlClient);

jvxErrorType jvx_connect_client_unlock_state_st(jvx_connect_client_if* hdlClient);

jvxErrorType jvx_connect_client_get_state_st(jvx_connect_client_if* hdlClient, jvx_connect_state* stateOnReturn);

// ===============================================================================

#define JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_SIMPLE(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData_thread, jvxSize ifidx); \
	jvxErrorType ic_callback_ ## fname(jvxSize ifidx);

#define JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_PROVIDE_DATA(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData_thread, jvxSize channelId, char** bufptr, jvxSize* szbuf, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis); \
	jvxErrorType ic_callback_ ## fname(jvxSize channelId, char** bufptr, jvxSize* szbuf, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);

#define JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_DATA_READ(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData, jvxSize channelId, char* buf, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis); \
	jvxErrorType ic_callback_ ## fname(jvxSize channelId, char* buf, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);

#define JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_ERROR(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx); \
	jvxErrorType ic_callback_ ## fname(jvxInt32 errCode, const char* errDescription, jvxSize ifidx);

#define JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_SPECIFIC(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData, jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx); \
	jvxErrorType ic_callback_ ## fname(jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx);

#define JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_UNSUCCESSFUL_OUT(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData, jvx_connect_fail_reason reason, jvxSize ifidx); \
	jvxErrorType ic_callback_ ## fname(jvx_connect_fail_reason reason, jvxSize ifidx);


#define JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_ALL \
	JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_SIMPLE(startup_complete) \
	JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_SIMPLE(shutdown_complete) \
	JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_SIMPLE(connect_outgoing) \
	JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_SIMPLE(disconnect_outgoing) \
	JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_PROVIDE_DATA(provide_data_and_length) \
	JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_DATA_READ(data_and_read) \
	JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_ERROR(report_error) \
	JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_SPECIFIC(specific) \
	JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_DECLARE_FUNC_UNSUCCESSFUL_OUT(unsuccesful_outgoing)

#define JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_ASSIGN(var, cname, startup_complete, shutdown_complete, \
	connect_outgoing, disconnect_outgoing, provide_data_and_length, data_and_read, \
	report_error, specific, unsuccesful_outgoing) \
	var.common.callback_report_startup_complete = cname::callback_ ## startup_complete; \
	var.common.callback_report_shutdown_complete = cname::callback_ ## shutdown_complete; \
	var.common.callback_provide_data_and_length = cname::callback_ ## provide_data_and_length; \
	var.common.callback_report_data_and_read = cname::callback_ ## data_and_read; \
	var.common.callback_report_error = cname::callback_ ## report_error; \
	var.common.callback_report_specific = cname::callback_ ## specific; \
	var.callback_report_disconnect_outgoing = cname::callback_ ## disconnect_outgoing; \
	var.callback_report_connect_outgoing = cname::callback_ ## connect_outgoing; \
	var.callback_report_unsuccesful_outgoing = cname::callback_ ## unsuccesful_outgoing;

#define JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_ASSIGN_ALL(var, cname) \
	JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_ASSIGN(var, cname, startup_complete, shutdown_complete, connect_outgoing, disconnect_outgoing, provide_data_and_length, data_and_read, report_error, specific, unsuccesful_outgoing)

#define JVX_CLIENT_SOCKET_FORWARD_C_CALLBACK_RESET(var) \
	var.common.callback_report_startup_complete = NULL; \
	var.common.callback_provide_data_and_length = NULL; \
	var.common.callback_report_data_and_read = NULL; \
	var.common.callback_report_error = NULL; \
	var.common.callback_report_specific = NULL; \
	var.callback_report_connect_outgoing = NULL; \
	var.callback_report_disconnect_outgoing = NULL; \
	var.callback_report_unsuccesful_outgoing = NULL; 

// ===============================================================

#define JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_SIMPLE(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvxSize ifidx) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname(ifidx)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvxSize ifidx) 

#define JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_PROVIDE_DATA(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvxSize channelId, char** bufptr, jvxSize* szbuf, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname( channelId,  bufptr,  szbuf, offset, idIf, additionalInfo, whatsthis)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvxSize channelId, char** bufptr, jvxSize* szbuf, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis) 

#define JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_DATA_READ(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvxSize channelId, char* buf, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname( channelId, buf, sz, offset, idIf, additionalInfo, whatsthis)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvxSize channelId, char* buf, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis) 

#define JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_ERROR(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname( errCode, errDescription,  ifidx)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvxInt32 errCode, const char* errDescription, jvxSize ifidx) 

#define JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_SPECIFIC(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname( idWhat,  fld,  szFld,  ifidx)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx) 

#define JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_UNSUCCESSFUL_OUT(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData, jvx_connect_fail_reason reason, jvxSize ifidx) \
	{ \
		cname* this_pointer = (cname*)privateData; \
		return(this_pointer->ic_callback_ ## fname( reason,  ifidx)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvx_connect_fail_reason reason, jvxSize ifidx) 

#endif
