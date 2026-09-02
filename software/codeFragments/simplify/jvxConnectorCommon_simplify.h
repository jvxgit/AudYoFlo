virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_connector(jvxApiString* str) override
{
	jvxErrorType res = _descriptor_connector(str);

#ifdef JVX_INPUT_CONNECTOR_WITH_UID
	if (!conUid.empty())
	{
		if(str) *str = str->std_str() + "-#" + conUid;
	}
#endif
	return res;
}

virtual jvxErrorType JVX_CALLINGCONVENTION parent_factory(IjvxConnectorFactory** my_parent)override
{
	return _parent_factory(my_parent);
}

