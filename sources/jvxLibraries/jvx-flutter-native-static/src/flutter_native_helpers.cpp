#include "flutter_native_local.h"

jvxErrorType __last_error = JVX_NO_ERROR;

void
ffi_get_libhost_pointer(void* opaque_hdl, jvxLibHost*& ll)
{
#ifdef JVX_SECURE_OPAQUE_PTR
	if (opaque_hdl)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		auto elm = lst_active_referenes.find(opaque_hdl);
		if (elm == lst_active_referenes.end())
		{
			assert(0);
		}
	}
#endif
	ll = reinterpret_cast<jvxLibHost*>(opaque_hdl);
	assert(ll);
}

char* 
ffi_transfer_command(void* opaque_hdl, const char* cmd, int json_out)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString astr;
	char* retPtr = nullptr;

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

	ll->transfer_command(cmd, &astr, (json_out != 0));
	ffi_host_allocate_char_array(astr.std_str(), &retPtr);	
	return retPtr;
}

int ffi_translate_enum_string(void* opaque_hdl, const char* typeName, const char* typeSelect)
{
	int retVal = -1;
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString astr;
	char* retPtr = nullptr;
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

	retVal = ll->translate_enum_string(typeName, typeSelect);
	return retVal;
}

char* ffi_encode_component_identification(int cpTp, int slotid, int slotsubid)
{
	char* retPtr = nullptr;
	jvxComponentIdentification cpId((jvxComponentType)cpTp, slotid, slotsubid);
	std::string txt = jvxComponentIdentification_txt(cpId);
	ffi_host_allocate_char_array(txt, &retPtr);
	return retPtr;
}

int ffi_lookup_component_class(void* opaque_hdl, int cpTp)
{
	int retVal = -1;
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString astr;
	jvxComponentTypeClass cpClass = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;

	JRE;
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

	res = ll->lookup_component_class((jvxComponentType)cpTp, cpClass);
	if (res == JVX_NO_ERROR)
	{
		retVal = (int)cpClass;
	}
	JSE;
	return retVal;
}

int ffi_last_error()
{
	return __last_error;
}

int ffi_preload_dll(void** dllhandle, const char* dllPath)
{
	JVX_HMODULE mod = JVX_LOADLIBRARY(dllPath);
	if (dllhandle)
	{
		*dllhandle = (void*)mod;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_SETTING;

}