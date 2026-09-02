#include "flutter_native_local.h"

struct one_process* ffi_process_handle_for_uid(void* opaque_hdl, int uId)
{
	struct one_process* retPtr = nullptr;
	jvxLibHost* ll = nullptr;
	jvxApiString astr;
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnectionIterator* it = nullptr;
	jvxSize catId = JVX_SIZE_UNSELECTED;

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

	ffi_host_allocate_process_handle(&retPtr);

	if (retPtr)
	{
		retPtr->uId = uId;
		res = ll->process_access(retPtr->uId, &catId, &astr, &it);
		retPtr->category = catId;
		retPtr->iterator = it;
		if (res == JVX_NO_ERROR)
		{
			ffi_host_allocate_char_array(astr.std_str(), &retPtr->name);
		}
	}
	if (res != JVX_NO_ERROR)
	{
		ffi_host_delete(retPtr, JVX_DELETE_DATATYPE_ONE_PROCESS);
		retPtr = nullptr;
	}
	return retPtr;
}


int ffi_process_decode_iterator_number_branches(void* opaque_hdl, void* it_handle)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	IjvxConnectionIterator* it = nullptr;
	jvxSize numBranches = 0;

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

	if (opaque_hdl)
	{
		it = (IjvxConnectionIterator*)it_handle;
		ll->process_decode_iterator(it, nullptr, &numBranches, nullptr, nullptr, nullptr);		
	}
	return numBranches;
}

/*
char* ffi_process_iterator_next_ocon_name(void* opaque_hdl, void* it_handle, size_t idNext)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	IjvxConnectionIterator* it = nullptr;
	jvxApiString nmVia;
	char* retVal;

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

	if (opaque_hdl)
	{
		it = (IjvxConnectionIterator*)it_handle;
		res = ll->process_next_iterator_ocon_name(it, &nmVia, idNext);
		if (res == JVX_NO_ERROR)
		{
			ffi_host_allocate_char_array(nmVia.std_str(), &retVal);
		}
	}
	return retVal;
}
*/

// void*
struct connector_bridge* ffi_process_iterator_next_handle(void* opaque_hdl, void* it_handle, size_t idNext)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	IjvxConnectionIterator* it = nullptr;	
	struct connector_bridge* retBrdge = nullptr;

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

	if (opaque_hdl)
	{
		jvxApiString nmOcon;
		jvxApiString nmIcon;
		it = (IjvxConnectionIterator*)it_handle;
		ffi_host_allocate_connector_bridge(&retBrdge);
		IjvxConnectionIterator* next_ptr = nullptr;
		res = ll->process_next_iterator_handle(it, &next_ptr, idNext, &nmOcon, &nmIcon);
		retBrdge->next = next_ptr;
		ffi_host_allocate_char_array(nmOcon.std_str(), &retBrdge->nmOconFrom);
		ffi_host_allocate_char_array(nmIcon.std_str(), &retBrdge->nmIconTo);
	}
	return retBrdge;
}

struct component_ident* ffi_process_decode_iterator_component(void* opaque_hdl, void* it_handle)
{
	jvxLibHost* ll = nullptr;
	IjvxConnectionIterator* it = nullptr;
	jvxComponentIdentification cpTp;
	jvxSize numBranches = 0;
	struct component_ident* retPtr = nullptr;

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

	if (opaque_hdl)
	{
		it = (IjvxConnectionIterator*)it_handle;
		ll->process_decode_iterator(it, &cpTp, nullptr, nullptr, nullptr, nullptr);
		ffi_host_allocate_component_ident(&retPtr, cpTp);
	}
	return retPtr;
}


char* ffi_process_decode_iterator_module(void* opaque_hdl, void* it_handle)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	IjvxConnectionIterator* it = nullptr;
	jvxComponentIdentification cpTp;
	jvxSize numBranches = 0;
	char* retPtr = nullptr;
	jvxApiString astr;

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

	if (opaque_hdl)
	{
		it = (IjvxConnectionIterator*)it_handle;
		ll->process_decode_iterator(it, nullptr, nullptr, &astr, nullptr, nullptr);
		ffi_host_allocate_char_array(astr.std_str(), &retPtr);
	}
	return retPtr;
}

char* ffi_process_decode_iterator_descriptor(void* opaque_hdl, void* it_handle)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	IjvxConnectionIterator* it = nullptr;
	jvxComponentIdentification cpTp;
	jvxSize numBranches = 0;
	char* retPtr = nullptr;
	jvxApiString astr;

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

	if (opaque_hdl)
	{
		it = (IjvxConnectionIterator*)it_handle;
		ll->process_decode_iterator(it, nullptr, nullptr, nullptr, &astr, nullptr);
		ffi_host_allocate_char_array(astr.std_str(), &retPtr);
	}
	return retPtr;
}

/*
char* ffi_process_decode_iterator_connector(void* opaque_hdl, void* it_handle)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	IjvxConnectionIterator* it = nullptr;
	jvxComponentIdentification cpTp;
	jvxSize numBranches = 0;
	char* retPtr = nullptr;
	jvxApiString astr;

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

	if (it_handle)
	{
		it = (IjvxConnectionIterator*)it_handle;
		ll->process_decode_iterator(it, nullptr, nullptr, nullptr, nullptr, &astr);
		ffi_host_allocate_char_array(astr.std_str(), &retPtr);
	}
	return retPtr;
}
*/

struct process_ready* ffi_process_system_ready(void* opaque_hdl)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	struct process_ready* retPtr = nullptr;
	jvxApiString astr;

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
		res = ll->process_system_ready(&astr);

		JVX_DSP_SAFE_ALLOCATE_OBJECT(retPtr, struct process_ready);
		retPtr->err = res;
		ffi_host_allocate_char_array(astr.std_str(), &retPtr->reason_if_not);
	}
	return retPtr;
}

/*
char* ffi_processes_decode_name(void* opaque_hdl, struct one_process* dec_hdl)
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
	res = ll->process_name(dec_hdl->uId, &astr);
	if (res == JVX_NO_ERROR)
	{
		ffi_host_allocate_char_array(astr.std_str(), &retPtr);
	}
	return retPtr;
}*/