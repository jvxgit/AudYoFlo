virtual jvxErrorType JVX_CALLINGCONVENTION system_ready() override
	{
		return(CjvxObject::_system_ready());
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION system_about_to_shutdown() override
	{
		return(CjvxObject::_system_about_to_shutdown());
	};

	