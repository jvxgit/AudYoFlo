#include <stdio.h>
#include <map>
#include "flutter_native_local.h"
#include "dart_native_api.h"
#include "dart_api_dl.h"
#include "internal/dart_api_dl_impl.h"
#include "dart_version.h"

#include <thread>

// ===================================================================================
// All extern variables declared in flutter_native_alloc.cpp
// ===================================================================================

extern std::map<void*, jvxLibHost*> lst_active_referenes;

extern std::string dllConfigSystem;
extern JVX_HMODULE dllHandleConfig;
extern flutter_config_open_ptr fptr;
extern native_host_configure_func_pointers func_pointer_object;

// ===================================================================================
// ===================================================================================

// Define the DART API functions for backward callbacks
#define DART_API_DL_DEFINITIONS(name, R, A) name##_Type name##_DL = NULL;
DART_API_ALL_DL_SYMBOLS(DART_API_DL_DEFINITIONS)
#undef DART_API_DL_DEFINITIONS

// =========================================================================
// Init code for DART API functions
typedef void (*DartApiEntry_function)();
DartApiEntry_function FindFunctionPointer(const DartApiEntry* entries,
	const char* name) {
	while (entries->name != NULL)
	{
		if (strcmp(entries->name, name) == 0)
			return entries->function;
		entries++;
	}
	return NULL;
}

void InitDartApiDL(void* data)
{
	DartApi* dart_api_data = (DartApi*)data;

	assert(dart_api_data->major == DART_API_DL_MAJOR_VERSION);
	/*
	{
		// If the DartVM we're running on does not have the same version as this
		// file was compiled against, refuse to initialize. The symbols are not
		// compatible.
	}*/

	// Minor versions are allowed to be different.
	// If the DartVM has a higher minor version, it will provide more symbols
	// than we initialize here.
	// If the DartVM has a lower minor version, it will not provide all symbols.
	// In that case, we leave the missing symbols un-initialized. Those symbols
	// should not be used by the Dart and native code. The client is responsible
	// for checking the minor version number himself based on which symbols it
	// is using.
	// (If we would error out on this case, recompiling native code against a
	// newer SDK would break all uses on older SDKs, which is too strict.)
	const DartApiEntry* dart_api_function_pointers = dart_api_data->functions;

#define DART_API_DL_INIT(name, R, A)                                           \
  name##_DL =                                                                  \
      (name##_Type)(FindFunctionPointer(dart_api_function_pointers, #name));
	DART_API_ALL_DL_SYMBOLS(DART_API_DL_INIT)
#undef DART_API_DL_INIT
		const DartApiEntry* it = dart_api_function_pointers;

}

// =========================================================================

struct func_pointers ptrs;
signed long long naport = 0;
bool send_async_dart(report_callback* cbk)
{
	bool retVal = false;
	Dart_CObject dart_object;
	dart_object.type = Dart_CObject_kInt64;
	dart_object.value.as_int64 = (intptr_t)cbk;
	if (Dart_PostCObject_DL)
	{
		retVal = Dart_PostCObject_DL(naport, &dart_object);
	}
	return retVal;
}

jvxErrorType send_sync_dart(report_callback* cbk)
{
	jvxErrorType ret = JVX_ERROR_UNSUPPORTED;
	if (ptrs.cb_sync)
	{
		ret = (jvxErrorType)ptrs.cb_sync(cbk);
	}
	return ret;
}
// ==================================================================================
// ==================================================================================

/*
	va_list nums;
	va_start(nums, argc);
	for (int i = 0; i < argc; i++)
	{
		const char* ptr = va_arg(nums, const char*);
	}
	va_end(nums);
*/

// JVX_GET_CURRENT_MODULE_PATH
/* <- i think, this function is no longer in use!
std::string JVX_GET_MODULE_FILENAME(LPCSTR func)
{
	std::string retVal;
	retVal = "";

	char path[MAX_PATH];
	HMODULE hm = NULL;

	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		func, &hm) == 0)
	{
		int ret = GetLastError();
		fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
		// Return or however you want to handle an error.
		return retVal;
	}
	if (GetModuleFileName(hm, path, sizeof(path)) == 0)
	{
		int ret = GetLastError();
		fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
		// Return or however you want to handle an error.
		return retVal;
	}
	retVal = path;
	return retVal;
}
*/

int ffi_host_initialize(void* opaque_hdl, const char** argv, int argc, func_pointers* funcs, signed long long dart_port, int executableInBinFolder)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	jvxLibHost* ll = nullptr;
	callbacks_capi capi;

#ifdef JVX_SECURE_OPAQUE_PTR
	if (opaque_hdl)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		auto elm = lst_active_referenes.find(opaque_hdl);
		if (elm != lst_active_referenes.end())
		{
		}
	}
#else
	ll = reinterpret_cast<jvxLibHost*>(opaque_hdl);
#endif

	/*
	 * Only for testing: sync callback triggered directly
	 *

	if (funcs)
	{
		if (funcs->cb_sync)
		{
			report_callback cbk;
			cbk.callback_id = 0x12345;
			cbk.callback_subid = 0x54321;
			funcs->cb_sync(&cbk);
		}
	}
	*/

	/*
	 * Only for testing: async callback triggered directly
	 *

	 std::thread t([&]() {
		//bool resultSend = Dart_PostInteger_DL(dart_port, 12345);
		Dart_CObject dart_object;
		report_callback* cbk = nullptr;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(cbk, report_callback);
		cbk->callback_id = 0x12345;
		cbk->callback_subid = 0x54321;
		dart_object.type = Dart_CObject_kInt64;
		dart_object.value.as_int64 = (intptr_t)cbk;
		bool resultSend = Dart_PostCObject_DL(dart_port, &dart_object);
		});
	// bool resultSend = Dart_PostInteger_DL(dart_port, 12345);
	t.join();
	*/

	// ======================================================================
	// Prepare whatever is required for backwards report.
	assign_report_functions(&capi);
	if (funcs)
	{
		ptrs = *funcs;
	}
	naport = dart_port;
	// ======================================================================
	// ======================================================================

	if (ll)
	{
		std::string alternativePath = JVX_GET_CURRENT_MODULE_PATH(&ffi_host_initialize); // JVX_GET_MODULE_FILENAME((LPCSTR)&ffi_host_initialize);
		if (!alternativePath.empty())
		{
			if (argc >= 1)
			{
				argv[0] = alternativePath.c_str();
			}
		}

		mainThreadId = JVX_GET_CURRENT_THREAD_ID();;
		res = ll->initSystem(argv, argc, &capi, (executableInBinFolder != 0));
	}
	return res;
}

int ffi_host_terminate(void* opaque_hdl)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	jvxLibHost* ll = nullptr;
#ifdef JVX_SECURE_OPAQUE_PTR
	if (opaque_hdl)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		auto elm = lst_active_referenes.find(opaque_hdl);
		if (elm != lst_active_referenes.end())
		{
		}
	}
#else
	ll = reinterpret_cast<jvxLibHost*>(opaque_hdl);
#endif
	if (ll)
	{
		res = ll->terminateSystem();
		mainThreadId = JVX_THREAD_ID_INVALID;
	}
	return res;
}
