#ifndef __FLUTTER_NATIVE_API_PROPERTIES_H__
#define __FLUTTER_NATIVE_API_PROPERTIES_H__

// Refer to file flutter_native_alloc.cpp
struct one_property_comp* ffi_properties_request_handle(void* opaque_hdl, struct component_ident* cpIdArg);
int ffi_properties_return_handle(void* opaque_hdl, struct one_property_comp* prop_hdl);

int ffi_properties_number_properties(void* opaque_hdl, struct one_property_comp* prop_hdl);
struct one_property_full_plus* ffi_properties_descriptor_property_lin(void* opaque_hdl, struct one_property_comp* opaque_prop, int id_prop, int descr_depth);
struct one_property_full_plus* ffi_properties_descriptor_property_descr(void* opaque_hdl, struct one_property_comp* opaque_prop, const char* descror, int descr_depth);

struct one_property_string_list* ffi_properties_get_string_list(
	void* opaque_hdl, struct one_property_comp* opaque_prop, const char* desror, 
	int contentOnly, int offset);

struct one_property_string_list* ffi_properties_get_selection_list(
	void* opaque_hdl, struct one_property_comp* opaque_prop, const char* desror, 
	int contentOnly, int offset, jvxUInt32* fillSelected, 
	jvxUInt32* fillSelectable, jvxUInt32* fillExclusive, jvxSize numEntries,
	jvxSize numFill);
int ffi_properties_get_content_property_fld(
	void* opaque_hdl, struct one_property_comp* opaque_prop, const char* desror,
	int contentOnly, int offset, void* fld, int format, jvxSize numEntries);

char* ffi_properties_get_string(void* opaque_hdl, struct one_property_comp* opaque_prop, const char* desror);

struct one_property_value_in_range* ffi_properties_get_value_in_range(
	void* opaque_hdl, struct one_property_comp* opaque_prop, const char* desror,  
	int contentOnly, jvxSize offset, void* fld, jvxSize numEntries);

// =================================================================================
int ffi_properties_set_content_property_fld(
	void* opaque_hdl, struct one_property_comp* opaque_prop, const char* desror,
	int contentOnly, int offset, void* fld, int format, jvxSize numEntries, int reportSetMode);

int ffi_properties_set_selection_list(
	void* opaque_hdl, struct one_property_comp* opaque_prop, const char* desror,
	int contentOnly, int offset, jvxUInt32* passSelected, jvxSize numEntries, jvxSize numOneEntry,
	int reportSetMode);

int ffi_properties_set_value_range(void* opaque_hdl, struct one_property_comp* opaque_prop, const char* desror,
	int contentOnly, int offset, void* fld, jvxSize numEntries, double minVal, double maxVal, int reportSetMode);

int ffi_properties_set_single_string(void* opaque_hdl, struct one_property_comp* opaque_prop, const char* desror,
	char* txt, int reportSetMode);

int ffi_properties_set_collect_start(void* opaque_hdl, struct one_property_comp* prop_hdl);

int ffi_properties_set_collect_stop(void* opaque_hdl, struct one_property_comp* prop_hdl);

// ===================================================================================

int ffi_properties_string_to_array(char* txt, void* fldBuffer, int offset, int numElements, int formatElements);

#endif