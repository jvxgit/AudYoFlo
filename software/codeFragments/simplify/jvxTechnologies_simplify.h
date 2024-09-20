	virtual jvxErrorType JVX_CALLINGCONVENTION number_devices(jvxSize* num) override
	{
		return(CjvxTechnology::_number_devices(num));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION name_device(jvxSize idx, jvxApiString* name, jvxApiString* fName) override
	{
		return(CjvxTechnology::_name_device(idx, name, fName));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION description_device(jvxSize idx, jvxApiString* str) override
	{
		return(CjvxTechnology::_description_device(idx, str));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_device(jvxSize idx, jvxApiString* str, jvxApiString* substr)override
	{
		return(CjvxTechnology::_descriptor_device(idx, str, substr));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION module_name_device(jvxSize idx, jvxApiString* str) override
	{
		return(CjvxTechnology::_module_name_device(idx, str));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION capabilities_device(jvxSize idx, jvxDeviceCapabilities& caps) override
	{
		return(CjvxTechnology::_capabilities_device(idx, caps));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION location_info_device(jvxSize idx, jvxComponentIdentification& tpId) override
	{
		return(CjvxTechnology::_location_info_device(idx, tpId));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION status_device(jvxSize idx, jvxState* stat) override
	{
		return(CjvxTechnology::_status_device(idx, stat));
	};
