#ifndef __FLUTTER_NATIVE_API_BOOT_SHUTDOWN_H__
#define __FLUTTER_NATIVE_API_BOOT_SHUTDOWN_H__

// Refer to file flutter_native_boot_shutdown.cpp
int ffi_host_initialize(void* opaque_hdl, const char** argv, int argc, struct func_pointers* funcs, signed long long dart_port, int executableInBinFolder);
int ffi_host_terminate(void* opaque_hdl);

#endif
