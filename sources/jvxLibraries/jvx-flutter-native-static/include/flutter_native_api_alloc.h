#ifndef __FLUTTER_NATIVE_API_ALLOC_H__
#define __FLUTTER_NATIVE_API_ALLOC_H__

// Refer to file flutter_native_alloc.cpp
struct retOpaqueHandle ffi_allocate_backend_handle(); 
int ffi_deallocate_backend_handle(void* opaque_hdl);

#endif
