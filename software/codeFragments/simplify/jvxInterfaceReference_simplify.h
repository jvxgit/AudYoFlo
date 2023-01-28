virtual jvxErrorType JVX_CALLINGCONVENTION request_reference_object(IjvxObject** obj)override
{
	if (obj)
	{
#ifdef JVX_OBJECT_ZERO_REFERENCE
		*obj = NULL;
#else
		*obj = static_cast<IjvxObject*>(this);
#endif
	}
	return JVX_NO_ERROR;
};

virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_object(IjvxObject* obj) override
{
	return JVX_NO_ERROR;
};
