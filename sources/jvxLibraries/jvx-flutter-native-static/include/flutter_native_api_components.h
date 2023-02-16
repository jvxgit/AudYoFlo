#ifndef __FLUTTER_NATIVE_API_COMPONENTS_H__
#define __FLUTTER_NATIVE_API_COMPONENTS_H__

// Refer to file flutter_native_alloc.cpp
char* ffi_component_description(void* opaque_hdl, struct component_ident* address);

int ffi_component_number_components(void* opaque_hdl, struct component_ident* address);
int ffi_component_number_devices(void* opaque_hdl, struct component_ident* address);
struct device_capabilities* ffi_component_capabilities_device(void* opaque_hdl, struct component_ident* address, int idx);
struct selection_option* ffi_component_selection_option(void* opaque_hdl, struct component_ident* address, int idx);
struct ss_list* ffi_component_slot_allocation(void* opaque_hdl, struct component_ident* address);

// Select a new node/technology or device. The new component will be signaled but we also return the 
// slotid/ slotsubid immediately. Hence, the return value is NOT an errorcode. The caller can 
// use the returned slotid/slotsubid to immediately run the activate afterwards without waiting for the 
// select to be reported.
struct component_ident* ffi_select_component(void* opaque_hdl, int cpTp, int slotid, int slotSubId, jvxSize idx);

// These functions return an error code
int ffi_activate_selected_component(void* opaque_hdl, struct component_ident* address);
int ffi_deactivate_selected_component(void* opaque_hdl, struct component_ident* address);
int ffi_unselect_selected_component(void* opaque_hdl, struct component_ident* address);

#endif