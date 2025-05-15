#ifndef __FLUTTER_NATIVE_API_H__
#define __FLUTTER_NATIVE_API_H__

#include "flutter_native_macros.h"

JVX_FLUTTER_LIB_BEGIN

#include "flutter_native_types.h"

// ==============================================================
// Allocate and deallocate the backendengine
// ==============================================================
#include "flutter_native_api_alloc.h"

// ==============================================================
// Initialize and terminate the main backend engine
// ==============================================================
#include "flutter_native_api_boot_shutdown.h"

// ==============================================================
// ==============================================================
#include "flutter_native_api_decode_requests.h"

// ==============================================================
// ==============================================================
#include "flutter_native_api_processes.h"

// ==============================================================
// ==============================================================
#include "flutter_native_api_sequencer.h"

// ==============================================================
// ==============================================================
#include "flutter_native_api_periodic.h"

// ==============================================================
// ==============================================================
#include "flutter_native_api_properties.h"

// ==============================================================
// ==============================================================
#include "flutter_native_api_components.h"

// ==============================================================
// ==============================================================
#include "flutter_native_api_config.h"

// 
// =============================================================
#include "flutter_native_api_helpers.h"

// ==============================================================
// ==============================================================
//! Sync and asynchronous callback function to re-enter the host
	int ffi_host_request_command_fwd(void* opaque_hdl, void* request_command, int async);
	int ffi_host_request_command_reschedule(void* request_command);

	// ==============================================================
	// Miscfunction calls for administration
	// ==============================================================
	char* ffi_transfer_command(void* opaque_hdl, const char* cmd, int json_out);


	char* ffi_encode_component_identification(int cpTp, int slotid, int slotsubid);

	int ffi_last_error();

	//! Delete a buffer. The tp argument specifies the type
	int ffi_host_delete(void* ptr, int tp);

	//! Test function to run a test command via ffi bridge
	int ffi_simple_call();

	// The Dart init function is not exposed via symbol file as we get access from within the flutter manually.
	// We might change this in the future
	// void InitDartApiDL(void* data);

	JVX_FLUTTER_LIB_END

//! Dll main default implementation
#define NATIVE_DLL_DEFINE_DLL_MAIN \
	extern bool flutternative_dllmain(DWORD fwdReason); \
	BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdReason, LPVOID lpReserved) \
	{ \
		return flutternative_dllmain(fwdReason); \
	}

#endif
