#ifndef __FLUTTER_NATIVE_API_CONFIG_H__
#define __FLUTTER_NATIVE_API_CONFIG_H__

int ffi_config_save(void* opaque_hdl);
int ffi_get_compile_flags(void* opaque_hdl, int* dataTpFormat, int* num32BitWordsSelection);
int ffi_set_operation_flags(void* opaque_hdl, int reportSet, int reportSyncDirectArg);
// int ffi_get_native_config_parameters(void* opaque_hdl, int* auto_start, int* auto_stop, int* skipstate_selected);
// int ffi_set_native_config_parameters(void* opaque_hdl, int auto_start, int auto_stop, int skipstate_selected);


#endif

// 