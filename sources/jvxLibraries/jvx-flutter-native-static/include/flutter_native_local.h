#ifndef __FLUTTER_NATIVE_LOCAL_H__
#define __FLUTTER_NATIVE_LOCAL_H__

#include "jvxLibHost.h"
#include "flutter_native_host_config.h"

extern "C"
{
#include "flutter_native_api.h"
}

void ffi_get_libhost_pointer(void* opaque_hdl, jvxLibHost*& ll);
void assign_report_functions(callbacks_capi* capi);
bool send_async_dart(report_callback* cbk);
jvxErrorType send_sync_dart(report_callback* cbk);
int ffi_host_allocate_char_array(const std::string& txt, char** txtOnReturn);
void ffi_host_allocate_property_string_list(struct one_property_string_list** ptrRet, const jvxApiStringList& lst);
void ffi_host_allocate_device_capabilities(
	struct device_capabilities** ptrRet,
	const jvxApiString& descror,
	const jvxApiString& description,
	const jvxDeviceCapabilities& caps,
	const jvxComponentIdentification& tpId, 
	const jvxState& stat);
void ffi_host_allocate_selection_option(
	struct selection_option** ptrRet,
	const jvxApiString& descror,
	const jvxApiString& description);

void ffi_host_allocate_val_range(const jvxValueInRange& val, one_property_value_in_range** valArrOnReturn);

void ffi_host_allocate_ss_list(
	struct ss_list** ptrRet,
	jvxSize sz, jvxBool isDevice);

void ffi_host_allocate_component_ident(struct component_ident** ptrRet, const jvxComponentIdentification& cpId);

extern jvxErrorType __last_error;
#define JRE __last_error = JVX_NO_ERROR
#define JSE __last_error = res
;
#endif