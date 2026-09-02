#ifndef __FLUTTER_NATIVE_API_PROCESSES_H__
#define __FLUTTER_NATIVE_API_PROCESSES_H__

// Refer to file flutter_native_alloc.cpp
struct one_process* ffi_process_handle_for_uid(void* opaque_hdl, int uId);

struct connector_bridge* ffi_process_iterator_next_handle(void* opaque_hdl, void* it_handle, size_t idNext);

int ffi_process_decode_iterator_number_branches(void* opaque_hdl, void* it_handle);
struct component_ident* ffi_process_decode_iterator_component(void* opaque_hdl, void* it_handle);
char* ffi_process_decode_iterator_module(void* opaque_hdl, void* it_handle);
char* ffi_process_decode_iterator_descriptor(void* opaque_hdl, void* it_handle);

struct process_ready* ffi_process_system_ready(void* opaque_hdl);

#endif