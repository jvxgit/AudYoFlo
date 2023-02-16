#include "flutter_native_local.h"

char* ffi_component_description(void* opaque_hdl, struct component_ident* address)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString astr;
	char* retPtr = nullptr;
	jvxComponentIdentification cpTp(
		(jvxComponentType)address->tp,
		address->slotid, 
		address->slotsubid,
		address->uId);

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
	
	res = ll->description_selected_component(cpTp, &astr);
	if (res == JVX_NO_ERROR)
	{
		ffi_host_allocate_char_array(astr.std_str(), &retPtr);
	}
	JSE;

	return retPtr;
}

// =====================================================================================
int ffi_component_number_components(void* opaque_hdl, struct component_ident* address)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = JVX_SIZE_UNSELECTED;
	jvxComponentIdentification cpTp(
		(jvxComponentType)address->tp,
		address->slotid,
		address->slotsubid,
		address->uId);

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

	res = ll->number_components_system(cpTp, &num);
	JSE;

	return num;
}

int ffi_component_number_devices(void* opaque_hdl, struct component_ident* address)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = JVX_SIZE_UNSELECTED;
	jvxComponentIdentification cpTp(
		(jvxComponentType)address->tp,
		address->slotid,
		address->slotsubid,
		address->uId);

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

	res = ll->number_devices_selected_technology(cpTp, &num);
	JSE;

	return num;
}

struct device_capabilities* ffi_component_capabilities_device(void* opaque_hdl, struct component_ident* address, int idx)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	struct device_capabilities* retVal = nullptr;
	jvxDeviceCapabilities caps;
	jvxComponentIdentification cpId;
	jvxApiString descror;
	jvxApiString description;
	jvxComponentIdentification cpTp(
		(jvxComponentType)address->tp,
		address->slotid,
		address->slotsubid,
		address->uId);

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

	res = ll->capability_device_selected_technology(cpTp, idx, caps, cpId);
	if (res == JVX_NO_ERROR)
	{
		res = ll->desciptor_device_selected_technology(cpTp, idx, &descror);
		if (res == JVX_NO_ERROR)
		{
			ll->desciption_device_selected_technology(cpTp, idx, &description);
			if (res == JVX_NO_ERROR)
			{
				jvxState stat = JVX_STATE_NONE;
				ll->state_device_selected_technology(cpTp, idx, &stat);
				if (res == JVX_NO_ERROR)
				{
					ffi_host_allocate_device_capabilities(&retVal, descror, description, caps, cpId, stat);
				}
			}
		}
	}
	JSE;

	return retVal;
}

struct ss_list* ffi_component_slot_allocation(void* opaque_hdl, struct component_ident* address)
{

	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	struct ss_list* retVal = nullptr;
	jvxSize sz = 0;	
	jvxComponentIdentification cpTp(
		(jvxComponentType)address->tp,
		address->slotid,
		address->slotsubid,
		address->uId);
	jvxComponentTypeClass cls = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;

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

	ll->lookup_component_class(cpTp.tp, cls);
	if (cls == jvxComponentTypeClass::JVX_COMPONENT_TYPE_DEVICE)
	{
		res = ll->number_slots_component_system(cpTp, nullptr, &sz);

		if (res == JVX_NO_ERROR)
		{
			jvxSize idx = JVX_SIZE_UNSELECTED;
			jvxSize i;
			jvxComponentIdentification tpGet = cpTp;
			ffi_host_allocate_ss_list(&retVal, sz, true);
			for (i = 0; i < sz; i++)
			{
				idx = JVX_SIZE_UNSELECTED;
				tpGet.slotsubid = i;
				ll->selection_component(tpGet, &idx);
				if (JVX_CHECK_SIZE_SELECTED(idx))
				{
					retVal->subslots[i] = idx;
				}
			}
		}
	}
	else
	{
		res = ll->number_slots_component_system(cpTp, &sz, nullptr);

		if (res == JVX_NO_ERROR)
		{
			jvxSize idx = JVX_SIZE_UNSELECTED;
			jvxSize i;
			jvxComponentIdentification tpGet = cpTp;
			ffi_host_allocate_ss_list(&retVal, sz, false);
			for (i = 0; i < sz; i++)
			{
				idx = JVX_SIZE_UNSELECTED;
				tpGet.slotid = i;
				tpGet.slotsubid = 0;
				ll->selection_component(tpGet, &idx);
				if (JVX_CHECK_SIZE_SELECTED(idx))
				{
					retVal->slots[i] = idx;
				}
			}
		}
	}
	JSE;

	return retVal;
}

struct selection_option* ffi_component_selection_option(void* opaque_hdl, struct component_ident* address, int idx)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	struct selection_option* retVal = nullptr;
	jvxDeviceCapabilities caps;
	jvxComponentIdentification cpId;
	jvxApiString descror;
	jvxApiString description;
	jvxComponentIdentification cpTp(
		(jvxComponentType)address->tp,
		address->slotid,
		address->slotsubid,
		address->uId);

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

	res = ll->descriptor_component_system(cpTp, idx, &descror);
	if (res == JVX_NO_ERROR)
	{
		ll->description_component_system(cpTp, idx, &description);
		if (res == JVX_NO_ERROR)
		{
			ffi_host_allocate_selection_option(&retVal, descror, description);
		}
	}
	JSE;

	return retVal;
}

// ============================================================================================

struct component_ident* ffi_select_component(void* opaque_hdl, int cpTp, int slotid, int slotSubId, jvxSize idx)
{
	struct component_ident* retVal = nullptr;
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentTypeClass cpClass = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;
	jvxComponentIdentification cpId(
		(jvxComponentType)cpTp,
		slotid,
		slotSubId,
		JVX_SIZE_UNSELECTED);

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
		switch (cpClass)
		{
		case jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY:
		case jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE:

			// Check cpId
			res = ll->select_component(cpId, idx);
			break;
		case jvxComponentTypeClass::JVX_COMPONENT_TYPE_DEVICE:

			// Check cpId
			res = ll->select_component(cpId, idx);
			break;
		default:
			res = JVX_ERROR_UNSUPPORTED;
		}
		if (res == JVX_NO_ERROR)
		{
			ffi_host_allocate_component_ident(&retVal, cpId);
		}
	}
	JSE;
	return retVal;
}

// These functions return an error code
int ffi_activate_selected_component(void* opaque_hdl, struct component_ident* address)
{
	int ssid = -1;
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentTypeClass cpClass = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;
	jvxComponentIdentification cpId(
		(jvxComponentType)address->tp,
		address->slotid,
		address->slotsubid,
		address->uId);

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
	res = ll->activate_selected_component(cpId);
	return res;
}

int 
ffi_deactivate_selected_component(void* opaque_hdl, struct component_ident* address)
{
	int ssid = -1;
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentTypeClass cpClass = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;
	jvxComponentIdentification cpId(
		(jvxComponentType)address->tp,
		address->slotid,
		address->slotsubid,
		address->uId);

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
	res = ll->deactivate_selected_component(cpId);
	return res;
}

int 
ffi_unselect_selected_component(void* opaque_hdl, struct component_ident* address)
{
	int ssid = -1;
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentTypeClass cpClass = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;
	jvxComponentIdentification cpId(
		(jvxComponentType)address->tp,
		address->slotid,
		address->slotsubid,
		address->uId);

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
	res = ll->unselect_selected_component(cpId);
	return res;
}
