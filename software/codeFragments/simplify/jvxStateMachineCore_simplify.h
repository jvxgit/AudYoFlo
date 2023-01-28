#ifndef JVX_STATEMACHINE_FULL_SKIP_INITIALIZE
virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef)override
{
	return(CjvxObject::_initialize(hostRef));
};
#endif

#ifndef JVX_STATEMACHINE_FULL_SKIP_SELECT_UNSELECT
virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override
{
	return(CjvxObject::_select(owner));
};

virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override
{
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		deactivate();
	}
	return(CjvxObject::_unselect());
};
#endif

virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override
{
	jvxErrorType res = CjvxObject::_terminate();

#ifdef JVX_RESET_PROPERTIES_ON_TERMINATE

	// This option should not be used anymore since properties may be associated in state NONE already!
	assert(0);
	// If we have a static library, the properties need to be reset in case of object termination
	if (res == JVX_NO_ERROR)
	{
		_reset_properties(); // Works only if object is derived from CjvxProperties
	}
#endif
	return(res);
};

// ====================================================================
// ====================================================================
// ====================================================================

#ifndef JVX_STATE_MACHINE_DEFINE_SKIP_ACTIVATE_DEACTIVATE

virtual jvxErrorType JVX_CALLINGCONVENTION activate()override
{
	return(_activate());
}

virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override
{
	return(_deactivate());
}

#endif

virtual jvxErrorType JVX_CALLINGCONVENTION owner(IjvxObject** ownerOnRet) override
{
	return(CjvxObject::_owner(ownerOnRet));
};



