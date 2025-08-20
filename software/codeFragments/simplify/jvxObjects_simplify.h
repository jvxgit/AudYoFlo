// --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 
// Interface CjvxObject
// --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 
virtual jvxErrorType JVX_CALLINGCONVENTION state(jvxState* stat)override
{
	return(CjvxObject::_state(stat));
};

virtual jvxErrorType JVX_CALLINGCONVENTION check_access(jvxCallManager& callGate) override
{
	return(CjvxObject::_check_access(callGate));
};

#ifndef JVX_OBJECT_SKIP_SET_LOCATION_INFO
virtual jvxErrorType JVX_CALLINGCONVENTION set_location_info(const jvxComponentIdentification& tpMe) override
{
	return(CjvxObject::_set_location_info(tpMe));
};
#endif

virtual jvxErrorType JVX_CALLINGCONVENTION location_info(jvxComponentIdentification& tpMe) override
{
	return(CjvxObject::_location_info(tpMe));
};

virtual jvxErrorType JVX_CALLINGCONVENTION lock_state() override
{
	return(CjvxObject::_lock_state());
};

virtual jvxErrorType JVX_CALLINGCONVENTION try_lock_state() override
{
	return(CjvxObject::_try_lock_state());
};

virtual jvxErrorType JVX_CALLINGCONVENTION unlock_state()override
{
	return(CjvxObject::_unlock_state());
};

virtual jvxErrorType JVX_CALLINGCONVENTION name(jvxApiString* name, jvxApiString* fName)override
{
	return(CjvxObject::_name(name, fName));
};

virtual jvxErrorType JVX_CALLINGCONVENTION description(jvxApiString* str)override
{
	return(CjvxObject::_description(str));
};

virtual jvxErrorType JVX_CALLINGCONVENTION descriptor(jvxApiString* str, jvxApiString* substr)override
{
	return(CjvxObject::_descriptor(str, substr));
};

virtual jvxErrorType JVX_CALLINGCONVENTION module_reference(jvxApiString* str, jvxComponentAccessType* acTp) override
{
	return(CjvxObject::_module_reference(str, acTp));
};

virtual jvxErrorType JVX_CALLINGCONVENTION compile_options(jvxApiString* str) override
{
	return(CjvxObject::_compile_options(str));
};

virtual jvxErrorType JVX_CALLINGCONVENTION feature_class(jvxBitField* ft)override
{
	return(CjvxObject::_feature_class(ft));
};

virtual jvxErrorType JVX_CALLINGCONVENTION version(jvxApiString* str)override
{
	return(CjvxObject::_version(str));
};

virtual jvxErrorType JVX_CALLINGCONVENTION request_specialization(jvxHandle** obj, jvxComponentIdentification* cT, jvxBool* mult, 
jvxApiString* subComponentDescriptor)override
{
	return(CjvxObject::_requestComponentSpecialization(obj, cT, mult, subComponentDescriptor));
};

virtual jvxErrorType JVX_CALLINGCONVENTION lasterror(jvxApiError* err)override
{
	return(CjvxObject::_lasterror(err));
};

virtual jvxErrorType JVX_CALLINGCONVENTION number_info_tokens(jvxSize* num)override
{
	return(_number_info_tokens(num));
};

virtual jvxErrorType JVX_CALLINGCONVENTION info_token(jvxSize idx, jvxApiString* fldStr)override
{
	return(_info_token(idx, fldStr));
};

virtual jvxErrorType JVX_CALLINGCONVENTION set_user_data(jvxSize idUserData, jvxHandle* floating_pointer)override
{
	return(_set_user_data( idUserData, floating_pointer));
};

virtual jvxErrorType JVX_CALLINGCONVENTION user_data(jvxSize idUserData, jvxHandle** floating_pointer)override
{
	return(_user_data( idUserData, floating_pointer));
};

virtual jvxErrorType JVX_CALLINGCONVENTION request_unique_object_id(jvxSize* uId) override
{
	return _request_unique_object_id(uId);
};

virtual jvxErrorType JVX_CALLINGCONVENTION interface_factory(IjvxInterfaceFactory** facOnRet) override
{
	return(_interface_factory(facOnRet));
};

virtual jvxErrorType JVX_CALLINGCONVENTION increment_reference() override
{
	return _increment_reference();
}
	
virtual jvxErrorType JVX_CALLINGCONVENTION current_reference(jvxSize* cnt) override
{
	return _current_reference(cnt);
}

virtual jvxErrorType JVX_CALLINGCONVENTION decrement_reference() override
{
	return _decrement_reference();
}

virtual jvxErrorType JVX_CALLINGCONVENTION register_object(IjvxObject*) override
{
	return JVX_NO_ERROR;
}

virtual jvxErrorType JVX_CALLINGCONVENTION unregister_object(IjvxObject*) override
{
	return JVX_NO_ERROR;
}
