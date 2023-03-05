#include "jvxLibHost.h"

jvxErrorType
jvxLibHost::description_selected_component(jvxComponentIdentification cpTp, jvxApiString* astr)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		res = involvedHost.hHost->description_selected_component(cpTp, astr);
	}
	return res;
}

jvxErrorType
jvxLibHost::number_components_system(jvxComponentIdentification cpTp, jvxSize* num)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		res = involvedHost.hHost->number_components_system(cpTp, num);
	}
	return res;
}

jvxErrorType
jvxLibHost::number_devices_selected_technology(jvxComponentIdentification cpTp, jvxSize* num)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		IjvxObject* theObj = nullptr;
		res = involvedHost.hHost->request_object_selected_component(cpTp, &theObj);
		if ((res == JVX_NO_ERROR) && theObj)
		{
			IjvxTechnology* tech = castFromObject<IjvxTechnology>(theObj);
			if (tech)
			{
				res = tech->number_devices(num);
			}
			involvedHost.hHost->return_object_selected_component(cpTp, theObj);
		}
	}
	return res;
}

jvxErrorType
jvxLibHost::desciptor_device_selected_technology(jvxComponentIdentification cpTp, jvxSize idx, jvxApiString* descror)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		IjvxObject* theObj = nullptr;
		res = involvedHost.hHost->request_object_selected_component(cpTp, &theObj);
		if ((res == JVX_NO_ERROR) && theObj)
		{
			IjvxTechnology* tech = castFromObject<IjvxTechnology>(theObj);
			if (tech)
			{
				res = tech->descriptor_device(idx, descror);
			}
			involvedHost.hHost->return_object_selected_component(cpTp, theObj);
		}
	}
	return res;
}

jvxErrorType
jvxLibHost::desciption_device_selected_technology(jvxComponentIdentification cpTp, jvxSize idx, jvxApiString* description)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		IjvxObject* theObj = nullptr;
		res = involvedHost.hHost->request_object_selected_component(cpTp, &theObj);
		if ((res == JVX_NO_ERROR) && theObj)
		{
			IjvxTechnology* tech = castFromObject<IjvxTechnology>(theObj);
			if (tech)
			{
				res = tech->description_device(idx, description);
			}
			involvedHost.hHost->return_object_selected_component(cpTp, theObj);
		}
	}
	return res;
}

jvxErrorType
jvxLibHost::capability_device_selected_technology(jvxComponentIdentification cpTp, jvxSize idx, jvxDeviceCapabilities& caps, jvxComponentIdentification& tpId)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		IjvxObject* theObj = nullptr;
		res = involvedHost.hHost->request_object_selected_component(cpTp, &theObj);
		if ((res == JVX_NO_ERROR) && theObj)
		{
			IjvxTechnology* tech = castFromObject<IjvxTechnology>(theObj);
			if (tech)
			{
				res = tech->capabilities_device(idx, caps);
				if (res == JVX_NO_ERROR)
				{
					res = tech->location_info_device(idx, tpId);
				}
			}
			involvedHost.hHost->return_object_selected_component(cpTp, theObj);
		}
	}
	return res;
}

jvxErrorType
jvxLibHost::state_device_selected_technology(jvxComponentIdentification cpTp, jvxSize idx, jvxState* stat)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		IjvxObject* theObj = nullptr;
		res = involvedHost.hHost->request_object_selected_component(cpTp, &theObj);
		if ((res == JVX_NO_ERROR) && theObj)
		{
			IjvxTechnology* tech = castFromObject<IjvxTechnology>(theObj);
			if (tech)
			{
				res = tech->status_device(idx, stat);
			}
			involvedHost.hHost->return_object_selected_component(cpTp, theObj);
		}
	}
	return res;
}

// ======================================================================================================

jvxErrorType
jvxLibHost::descriptor_component_system(jvxComponentIdentification cpTp, jvxSize idx, jvxApiString* descror)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		IjvxObject* theObj = nullptr;
		res = involvedHost.hHost->descriptor_component_system(cpTp, idx, descror);
	}
	return res;
}

jvxErrorType
jvxLibHost::description_component_system(jvxComponentIdentification cpTp, jvxSize idx, jvxApiString* description)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		IjvxObject* theObj = nullptr;
		res = involvedHost.hHost->description_component_system(cpTp, idx, description);
	}
	return res;
}

jvxErrorType
jvxLibHost::selection_component(const jvxComponentIdentification& cpTp, jvxSize* idx)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		IjvxObject* theObj = nullptr;
		res = involvedHost.hHost->selection_component(cpTp, idx);
	}
	return res;
}

jvxErrorType
jvxLibHost::number_slots_component_system(const jvxComponentIdentification& cpTp, jvxSize* szSlots, jvxSize* szSubslots)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		IjvxObject* theObj = nullptr;
		res = involvedHost.hHost->number_slots_component_system(cpTp, szSlots, szSubslots, nullptr, nullptr);
	}
	return res;
}

// ======================================================================================================

jvxErrorType
jvxLibHost::select_component(jvxComponentIdentification& cpTp, jvxSize idx)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		res = involvedHost.hHost->select_component(cpTp, idx);		
	}
	return res;
}

jvxErrorType 
jvxLibHost::activate_selected_component(const jvxComponentIdentification& cpTp)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		res = involvedHost.hHost->activate_selected_component(cpTp);
	}
	return res;	
}

jvxErrorType
jvxLibHost::deactivate_selected_component(const jvxComponentIdentification& cpTp)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		res = involvedHost.hHost->deactivate_selected_component(cpTp);
	}
	return res;
}

jvxErrorType
jvxLibHost::unselect_selected_component(jvxComponentIdentification& cpTp)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		res = involvedHost.hHost->unselect_selected_component(cpTp);
	}
	return res;
}
