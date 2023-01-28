#ifndef JVX_STATEMACHINE_FULL_SKIP_INIT_TERM
virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef)override
	{
		return(CjvxObject::_initialize(hostRef));
	};

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
#endif

#ifndef JVX_STATEMACHINE_FULL_SKIP_SELECT_UNSELECT
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override
	{
		return(CjvxObject::_select(owner));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override
	{
		if(_common_set_min.theState == JVX_STATE_PROCESSING)
		{
			stop();
		}
		if(_common_set_min.theState == JVX_STATE_PREPARED)
		{
			postprocess();
		}
		if(_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			deactivate();
		}
		return(CjvxObject::_unselect());
	};
#endif

	// ====================================================================
	// ====================================================================
	// ====================================================================

#ifdef JVX_STATE_MACHINE_DEFINE_ACTIVATEDEACTIVATE

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override
	{
		return(_activate());
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override
	{
		return(_deactivate());
	}

#endif

	// ====================================================================
	// ====================================================================
	// ====================================================================

#ifdef JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override
	{
		return(_postprocess());
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override
	{
		return(_prepare());
	};

#endif

	// ====================================================================
	// ====================================================================
	// ====================================================================

#ifdef JVX_STATE_MACHINE_DEFINE_STARTSTOP

	virtual jvxErrorType JVX_CALLINGCONVENTION start()override
	{
		return(_start());
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override
	{
		return(_stop());
	}

#endif

#ifdef JVX_STATE_MACHINE_DEFINE_READY
	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override
	{
		return(_is_ready(suc, reasonIfNot));
	}
#endif

	virtual jvxErrorType JVX_CALLINGCONVENTION is_alive(jvxBool* alive)override
	{
		return(_is_alive(alive));
	}

	// ====================================================================
	// ====================================================================
	// ====================================================================

	// ====================================================================
	// ====================================================================
	// ====================================================================

#ifdef JVX_STATE_MACHINE_DEFINE_STARTSTOP_UNSUPPORTED

	virtual jvxErrorType JVX_CALLINGCONVENTION start()override
	{
		return(JVX_ERROR_UNSUPPORTED);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override
	{
		return(JVX_ERROR_UNSUPPORTED);
	}
#endif

#ifdef JVX_STATE_MACHINE_DEFINE_READY_UNSUPPORTED
	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override
	{
		return(JVX_ERROR_UNSUPPORTED);
	}
#endif

	// ====================================================================
	// ====================================================================
	// ====================================================================

#ifdef JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS_UNSUPPORTED


	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override
	{
		return(JVX_ERROR_UNSUPPORTED);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override
	{
		return(JVX_ERROR_UNSUPPORTED);
	};

#endif

virtual jvxErrorType JVX_CALLINGCONVENTION owner(IjvxObject** ownerOnRet) override
{
	return(CjvxObject::_owner(ownerOnRet));
};

