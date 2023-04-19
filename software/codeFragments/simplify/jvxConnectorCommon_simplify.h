virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_connector(jvxApiString* str) override
{
	return _descriptor_connector(str);
}

virtual jvxErrorType JVX_CALLINGCONVENTION parent_factory(IjvxConnectorFactory** my_parent)override
{
	return _parent_factory(my_parent);
}

