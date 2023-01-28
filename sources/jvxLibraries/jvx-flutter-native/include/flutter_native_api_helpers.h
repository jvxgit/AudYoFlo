#ifndef __FLUTTER_NATIVE_API_HELPERS_H__
#define __FLUTTER_NATIVE_API_HELPERS_H__

int ffi_translate_enum_string(void* opaque_hdl, const char* typeName, const char* typeSelect);
int ffi_lookup_component_class(void* opaque_hdl, int cpType);

#endif
