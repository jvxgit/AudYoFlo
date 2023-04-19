virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_connector(jvxApiString* str) override
{
	return _descriptor_connector(str);
}

virtual jvxErrorType JVX_CALLINGCONVENTION parent_factory(IjvxConnectorFactory** my_parent)override
{
	return _parent_factory(my_parent);
}

virtual jvxErrorType JVX_CALLINGCONVENTION number_next(jvxSize* num) override
{
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	if (num)
		*num = 0;
	return JVX_NO_ERROR;
#else
	return _number_next(num);
#endif
}

virtual jvxErrorType JVX_CALLINGCONVENTION reference_next(jvxSize idx, IjvxConnectionIterator** next) override
{
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
	if (next)
	{
		*next = nullptr;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
#else
	return _reference_next(idx, next);
#endif

}

virtual jvxErrorType JVX_CALLINGCONVENTION reference_component(
	jvxComponentIdentification* cpTp, 
	jvxApiString* modName,
	jvxApiString* lContext) override
{
	return _reference_component(cpTp, modName, lContext);
}