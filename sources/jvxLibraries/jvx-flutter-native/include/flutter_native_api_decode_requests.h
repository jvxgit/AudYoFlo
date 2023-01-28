#ifndef __FLUTTER_NATIVE_API_DECODE_REQUESTS_H__
#define __FLUTTER_NATIVE_API_DECODE_REQUESTS_H__

// Refer to file flutter_native_alloc.cpp
int ffi_req_command_decode_origin(void* hdl_class, struct component_ident* retCp);
int ffi_req_command_decode_sswitch(void* hdl_class, int* sswitchRet);
int ffi_req_command_decode_broadcast(void* hdl_class, int* sswitchRet);
int ffi_req_command_decode_uid(void* hdl_class, size_t* uid);
int ffi_req_command_decode_request_type(void* hdl_class, int* reqRetPtr);
char* ffi_req_command_decode_ident_allocate_char_array(void* hdl_class);
struct one_sequencer_event* ffi_req_command_decode_sequencer_event(void* hdl_class);

#endif