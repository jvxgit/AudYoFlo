#include "flutter_native_local.h"

jvxErrorType ffi_properties_descriptor_property_core(
    struct one_property_full_plus*& propRef,
    IjvxProperties* props, jvxCallManagerProperties& callGate,
    jvx::propertyAddress::IjvxPropertyAddress& ident,
    int descr_depth);

struct one_property_comp* ffi_properties_request_handle(void* opaque_hdl, struct component_ident* cpIdArg)
{
    struct one_property_comp* retPtr = nullptr;
    jvxLibHost* ll = nullptr;
    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)cpIdArg->tp;
    cpId.slotid = cpIdArg->slotid;
    cpId.slotsubid = cpIdArg->slotsubid;
    cpId.uId = cpIdArg->uId;

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

    JRE;

    IjvxObject* obj_ptr = nullptr;
    IjvxProperties* prop_ptr = nullptr;
    jvxErrorType res = ll->property_request_handle(cpId, &obj_ptr, &prop_ptr);
    if (res == JVX_NO_ERROR)
    {
        JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(retPtr, struct one_property_comp);
        retPtr->cpId.tp = (jvxComponentType)cpIdArg->tp;
        retPtr->cpId.slotid = cpIdArg->slotid;
        retPtr->cpId.slotsubid = cpIdArg->slotsubid;
        retPtr->cpId.uId = cpIdArg->uId;

        retPtr->obj_ptr = reinterpret_cast<void*>(obj_ptr);
        retPtr->prop_ptr = reinterpret_cast<void*>(prop_ptr);
    }
    else
    {
        retPtr = nullptr;
        
    }
    JSE;
    return retPtr;
};

int ffi_properties_return_handle(void* opaque_hdl, one_property_comp* prop_hdl)
{
    jvxLibHost* ll = nullptr;
    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;
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
    jvxErrorType res = ll->property_return_handle(cpId, 
        reinterpret_cast<IjvxObject*>(prop_hdl->obj_ptr), 
        reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr));
    
    ffi_host_delete(prop_hdl, JVX_DELETE_DATATYPE_ONE_PROP_HANDLER);
    JSE;
    return res;
}

int ffi_properties_number_properties(void* opaque_hdl, struct one_property_comp* prop_hdl)
{
    jvxLibHost* ll = nullptr;
    jvxSize num = 0;
    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;
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
    jvxErrorType res = ll->verify_property_handle(cpId, (IjvxObject*)prop_hdl->obj_ptr,
        (IjvxProperties*)prop_hdl->prop_ptr);
    if (res == JVX_NO_ERROR)
    {
        IjvxProperties* props = reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr);
        if (props)
        {
            jvxCallManagerProperties callGate;
            props->number_properties(callGate, &num);
        }
    }
    JSE;
    return num;
}

struct one_property_full_plus* ffi_properties_descriptor_property_lin(void* opaque_hdl, struct one_property_comp* prop_hdl, int id_prop, int descr_depth)
{
    jvxLibHost* ll = nullptr;
    jvxSize num = 0;
    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;
    struct one_property_full_plus* propRef = nullptr;
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
    jvxErrorType res = ll->verify_property_handle(cpId, (IjvxObject*)prop_hdl->obj_ptr,
        (IjvxProperties*)prop_hdl->prop_ptr);
    if (res == JVX_NO_ERROR)
    {
		IjvxProperties* props = reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr);
		if (props)
		{
            jvxCallManagerProperties callGate;
            props->number_properties(callGate, &num);
            if (id_prop < num)
            {
                jvx::propertyAddress::CjvxPropertyAddressLinear ident(id_prop);
                res = ffi_properties_descriptor_property_core(propRef, props, callGate, ident, descr_depth);
            }
        }
    }
    JSE;
    return propRef;
}

struct one_property_full_plus* ffi_properties_descriptor_property_descr(void* opaque_hdl, struct one_property_comp* prop_hdl, const char* descror, int descr_depth)
{
    jvxLibHost* ll = nullptr;
    jvxSize num = 0;
    struct one_property_full_plus* propRef = nullptr;
    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;
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
    jvxErrorType res = ll->verify_property_handle(cpId, (IjvxObject*)prop_hdl->obj_ptr,
        (IjvxProperties*)prop_hdl->prop_ptr);
    if (res == JVX_NO_ERROR)
    {
        IjvxProperties* props = reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr);
        if (props)
        {

            jvxCallManagerProperties callGate;
            jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descror);
            res = ffi_properties_descriptor_property_core(propRef, props, callGate, ident, descr_depth);
        }
    }
    JSE;
    return propRef;
}

int ffi_properties_string_to_array(char* txt, void* fldBuffer, int offset, int numElements, int formatElements)
{
    jvxSize num = 0;
    jvxDataFormat format = JVX_DATAFORMAT_NONE;
    jvxSize reqBytes = 0;
    JRE;
    jvxErrorType res = jvx_binString2ValueList(txt, "", fldBuffer, (jvxDataFormat)formatElements, numElements, &reqBytes);
    JSE;
    return res;
}

// ===============================================================================================================================
// PROPERTY TYPES PROPERTY TYPES PROPERTY TYPES PROPERTY TYPES PROPERTY TYPES PROPERTY TYPES PROPERTY TYPES PROPERTY TYPES
// ===============================================================================================================================

struct one_property_string_list* ffi_properties_get_string_list(void* opaque_hdl, struct one_property_comp* prop_hdl, const char* descror,
    int contentOnly, int offset)
{
    struct one_property_string_list* retVal = nullptr;
    jvxLibHost* ll = nullptr;

    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;
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
    jvxErrorType res = ll->verify_property_handle(cpId, (IjvxObject*)prop_hdl->obj_ptr,
        (IjvxProperties*)prop_hdl->prop_ptr);
    if (res == JVX_NO_ERROR)
    {
        IjvxProperties* props = reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr);
        if (props)
        {
            jvxApiStringList strLst;
            jvxCallManagerProperties callGate;
            jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descror);
            jPD trans(contentOnly != 0, offset);
            res = props->get_property(callGate,
                jvx::propertyRawPointerType::CjvxRawPointerTypeObject<jvxApiStringList>(strLst),
                ident, trans);
            if (res == JVX_NO_ERROR)
            {
                ffi_host_allocate_property_string_list(&retVal, strLst);
            }
        }
    }
    JSE;
    return retVal;
}

struct one_property_string_list* ffi_properties_get_selection_list(
    void* opaque_hdl, struct one_property_comp* prop_hdl, const char* descror,
    int contentOnly, int offset, jvxUInt32* fillSelected,
    jvxUInt32* fillSelectable, jvxUInt32* fillExclusive, jvxSize numEntries, jvxSize numOneEntry)
{
    struct one_property_string_list* retVal = nullptr;
    jvxLibHost* ll = nullptr;

    assert(numOneEntry == JVX_NUMBER_32BITS_BITFIELD);
    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;
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
    jvxErrorType res = ll->verify_property_handle(cpId, (IjvxObject*)prop_hdl->obj_ptr,
        (IjvxProperties*)prop_hdl->prop_ptr);
    if (res == JVX_NO_ERROR)
    {
        IjvxProperties* props = reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr);
        if (props)
        {
            jvxSelectionList selLst(0);

            // Hui, is this typecast really ok?
            selLst.assign((jvxBitField*)fillSelected, numEntries);

            jvxCallManagerProperties callGate;
            jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descror);
            jPD trans(contentOnly != 0, offset);
            res = props->get_property(callGate,
                jvx::propertyRawPointerType::CjvxRawPointerTypeObject<jvxSelectionList>(selLst),
                ident, trans);
            if (res == JVX_NO_ERROR)
            {
                if (!contentOnly)
                {
                    ffi_host_allocate_property_string_list(&retVal, selLst.strList);
                }

                /*
                if (fillSelected)
                {
                    // -> By assignuing the field, there is no longer need to copy!!
                    const jvxUInt32* from = selLst.bitFieldSelected().raw();
                    if (from)
                    {
                        memcpy(fillSelected, from, minElms*sizeof(jvxUInt32));
                    }
                }
                */

				if (fillSelectable)
				{
					const jvxUInt32* from = selLst.bitFieldSelectable.raw();
					if (from)
					{
						memcpy(fillSelectable, from, JVX_NUMBER_32BITS_BITFIELD * sizeof(jvxUInt32));
					}
				}
				if (fillExclusive)
				{
					const jvxUInt32* from = selLst.bitFieldExclusive.raw();
					if (from)
					{
						memcpy(fillExclusive, from, JVX_NUMBER_32BITS_BITFIELD * sizeof(jvxUInt32));
					}
				}

            }
        }
    }
    JSE;
    return retVal;
}

int ffi_properties_get_content_property_fld(
    void* opaque_hdl, struct one_property_comp* prop_hdl, const char* descror,
    int contentOnly, int offset, void* fld, int format_int, jvxSize numEntries)
{
    struct one_property_string_list* retVal = nullptr;
    jvxLibHost* ll = nullptr;

    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;

    jvxDataFormat format = (jvxDataFormat)format_int;
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
    jvxErrorType res = ll->verify_property_handle(cpId, (IjvxObject*)prop_hdl->obj_ptr,
        (IjvxProperties*)prop_hdl->prop_ptr);
    if (res == JVX_NO_ERROR)
    {
        IjvxProperties* props = reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr);
        if (props)
        {
            jvxCallManagerProperties callGate;
            jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descror);
            jPD trans(contentOnly != 0, offset);
            res = props->get_property(callGate,
                jPRG(fld, numEntries, format), ident, trans);
        }
    }
    JSE;
    return res;
}

char* ffi_properties_get_string(void* opaque_hdl, struct one_property_comp* prop_hdl, const char* descror)
{
    char* retPtr = nullptr;
    jvxLibHost* ll = nullptr;

    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;
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
    jvxErrorType res = ll->verify_property_handle(cpId, (IjvxObject*)prop_hdl->obj_ptr,
        (IjvxProperties*)prop_hdl->prop_ptr);
    if (res == JVX_NO_ERROR)
    {
        IjvxProperties* props = reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr);
        if (props)
        {
            jvxApiString astr;
            jvxCallManagerProperties callGate;
            jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descror);
            jPD trans;
            res = props->get_property(callGate,
                jvx::propertyRawPointerType::CjvxRawPointerTypeObject<jvxApiString>(astr), ident, trans);
            if (res == JVX_NO_ERROR)
            {
                ffi_host_allocate_char_array(astr.std_str(), &retPtr);
            }
        }
    }
    JSE;
    return retPtr;
}

one_property_value_in_range* ffi_properties_get_value_in_range(
    void* opaque_hdl, struct one_property_comp* opaque_prop, const char* desror,
    int contentOnly, jvxSize offset, void* fld, jvxSize numEntries)
{
    jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
    jvxLibHost* ll = nullptr;
    one_property_value_in_range* retPtr = nullptr;

    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)opaque_prop->cpId.tp;
    cpId.slotid = opaque_prop->cpId.slotid;
    cpId.slotsubid = opaque_prop->cpId.slotsubid;
    cpId.uId = opaque_prop->cpId.uId;
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
    jvxValueInRange valRg(0);
    valRg.assign((jvxData*)fld, numEntries);
    
    jvxCallManagerProperties callGate;
    jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(desror);
    jPD trans(contentOnly != 0, offset);
    IjvxProperties* props = reinterpret_cast<IjvxProperties*>(opaque_prop->prop_ptr);
    if (props)
    {
        res = props->get_property(callGate, jPROVR(valRg), ident, trans);
        if (!contentOnly)
        {
            if (res == JVX_NO_ERROR)
            {
                ffi_host_allocate_val_range(valRg, &retPtr);
            }
        }
    }
    JSE;
    return retPtr;
}

// ==================================================================================================================================

int ffi_properties_set_content_property_fld(
    void* opaque_hdl, struct one_property_comp* prop_hdl, const char* descror,
    int contentOnly, int offset, void* fld, int format, jvxSize numEntries, int reportSetMode)
{
    jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
    jvxLibHost* ll = nullptr;

    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;
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
	jvxApiString astr;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descror);
	jPD trans(contentOnly != 0, offset);
	res = ll->set_property(callGate, cpId,
		reinterpret_cast<IjvxObject*>(prop_hdl->obj_ptr),
		reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr),
		jPRG(fld, numEntries, (jvxDataFormat)format), ident, trans,
        (AyfBackendReportPropertySet)reportSetMode);
	JSE;
    return res;
}

int ffi_properties_set_selection_list(
    void* opaque_hdl, struct one_property_comp* prop_hdl, const char* descror,
    int contentOnly, int offset, jvxUInt32* passSelected, jvxSize numEntries,
    jvxSize numOneEntry, int reportSetMode)
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxLibHost* ll = nullptr;
    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;
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
    
	jvxSelectionList selLst(0);
	jvxApiString astr;
	jvxCallManagerProperties callGate;
    selLst.assign((jvxBitField*)passSelected, numEntries);
    assert(numOneEntry == JVX_NUMBER_32BITS_BITFIELD);

	/*
    if (passSelected)
	{
		jvxUInt32* to = selLst.bitFieldSelected().raw();
		if (to)
		{
			memcpy(to, passSelected, minElms * sizeof(jvxUInt32));
		}
	}
    */

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descror);
	jPD trans(true, offset);
	res = ll->set_property(callGate,
		cpId, reinterpret_cast<IjvxObject*>(prop_hdl->obj_ptr),
		reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr),
		jPROSL(selLst), ident, trans, (AyfBackendReportPropertySet)reportSetMode);
    
    JSE;
    return res;
}

int ffi_properties_set_value_range(void* opaque_hdl, struct one_property_comp* prop_hdl, const char* desror,
    int contentOnly, int offset, void* fld, jvxSize numEntries, double minVal, double maxVal, int reportSetMode)
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxLibHost* ll = nullptr;
    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;
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

    jvxValueInRange valRg(0);
    jvxCallManagerProperties callGate;
    valRg.assign((jvxData*)fld, numEntries);

    /*
    if (passSelected)
    {
        jvxUInt32* to = selLst.bitFieldSelected().raw();
        if (to)
        {
            memcpy(to, passSelected, minElms * sizeof(jvxUInt32));
        }
    }
    */

    jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(desror);
    jPD trans(true, offset);
    res = ll->set_property(callGate,
        cpId, reinterpret_cast<IjvxObject*>(prop_hdl->obj_ptr),
        reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr),
        jPROVR(valRg), ident, trans, (AyfBackendReportPropertySet)reportSetMode);

    JSE;
    return res;
}

int ffi_properties_set_single_string(void* opaque_hdl, struct one_property_comp* prop_hdl, const char* descror,
    char* txt, int reportSetMode)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxLibHost* ll = nullptr;
	jvxComponentIdentification cpId;
	cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
	cpId.slotid = prop_hdl->cpId.slotid;
	cpId.slotsubid = prop_hdl->cpId.slotsubid;
	cpId.uId = prop_hdl->cpId.uId;
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

	jvxSelectionList selLst;
	jvxApiString astr;
	jvxCallManagerProperties callGate;
	if (txt)
	{
		astr.assign(txt);
	}
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descror);
	jPD trans(true);
	res = ll->set_property(callGate,
		cpId, reinterpret_cast<IjvxObject*>(prop_hdl->obj_ptr),
		reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr),
		jPROS(astr), ident, trans, (AyfBackendReportPropertySet)reportSetMode);

	JSE;
	return res;
}

// ======================================================================================

int
ffi_properties_set_collect_start(void* opaque_hdl, struct one_property_comp* prop_hdl)
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxLibHost* ll = nullptr;
    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;
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

    jvxSelectionList selLst;
    jvxApiString astr;
    jvxCallManagerProperties callGate;
    
    jPD trans(true);
    res = ll->start_property_set_collect(callGate,
        cpId, reinterpret_cast<IjvxObject*>(prop_hdl->obj_ptr),
        reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr));

    JSE;
    return res;
}

int
ffi_properties_set_collect_stop(void* opaque_hdl, struct one_property_comp* prop_hdl)
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxLibHost* ll = nullptr;
    jvxComponentIdentification cpId;
    cpId.tp = (jvxComponentType)prop_hdl->cpId.tp;
    cpId.slotid = prop_hdl->cpId.slotid;
    cpId.slotsubid = prop_hdl->cpId.slotsubid;
    cpId.uId = prop_hdl->cpId.uId;
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

    jvxSelectionList selLst;
    jvxApiString astr;
    jvxCallManagerProperties callGate;

    jPD trans(true);
    res = ll->stop_property_set_collect(callGate,
        cpId, reinterpret_cast<IjvxObject*>(prop_hdl->obj_ptr),
        reinterpret_cast<IjvxProperties*>(prop_hdl->prop_ptr));

    JSE;
    return res;
}

// ======================================================================================

// ===============================================================================================================================
// HELPER HELPER HELPER HELPER HELPER HELPER HELPER HELPER HELPER HELPER HELPER HELPER HELPER HELPER HELPER HELPER HELPER HELPER
// ===============================================================================================================================

jvxErrorType ffi_properties_descriptor_property_core(
    struct one_property_full_plus*& propRef,
    IjvxProperties* props, jvxCallManagerProperties& callGate,
    jvx::propertyAddress::IjvxPropertyAddress& ident,
    int descr_depth)
{
    propRef = nullptr;
    jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus theDescr;
    jvxErrorType res = JVX_ERROR_INVALID_SETTING;
    theDescr.descrType = (jvx::propertyDescriptor::descriptorEnum)descr_depth;
    res = props->description_property(callGate, theDescr, ident);

    if (res == JVX_NO_ERROR)
    {
        JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z(propRef, struct one_property_full_plus);

        if (theDescr.valid_parts & (1 << (int)jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_MIN))
        {
            propRef->full.ctrl.core.pmin.valid_parts = theDescr.valid_parts;
            propRef->full.ctrl.core.pmin.category = theDescr.category;
            propRef->full.ctrl.core.pmin.globalIdx = theDescr.globalIdx;
        }

        if (theDescr.valid_parts & (1 << (int)jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_CORE))
        {
            propRef->full.ctrl.core.accessType = theDescr.accessType;
            propRef->full.ctrl.core.ctxt = theDescr.ctxt;
            propRef->full.ctrl.core.decTp = theDescr.decTp;
            propRef->full.ctrl.core.format = theDescr.format;
            propRef->full.ctrl.core.num = theDescr.num;
        }

        if (theDescr.valid_parts & (1 << (int)jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_CONTROL))
        {
            propRef->full.ctrl.allowedStateMask = theDescr.allowedStateMask;
            propRef->full.ctrl.allowedThreadingMask = theDescr.allowedThreadingMask;
            propRef->full.ctrl.installable = theDescr.installable;
            propRef->full.ctrl.invalidateOnStateSwitch = theDescr.invalidateOnStateSwitch;
            propRef->full.ctrl.isValid = theDescr.isValid;
        }

        if (theDescr.valid_parts & (1 << (int)jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_FULL))
        {
            ffi_host_allocate_char_array(theDescr.name.std_str(), &propRef->full.name);
            ffi_host_allocate_char_array(theDescr.description.std_str(), &propRef->full.description);
            ffi_host_allocate_char_array(theDescr.descriptor.std_str(), &propRef->full.descriptor);
        }

        if (theDescr.valid_parts & (1 << (int)jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_FULL_PLUS))
        {
            propRef->accessFlags = theDescr.accessFlags;
            propRef->configModeFlags = theDescr.configModeFlags;
        }
    }
    return res;
}