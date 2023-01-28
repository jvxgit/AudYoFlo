virtual jvxErrorType JVX_CALLINGCONVENTION obtain_unique_id(jvxSize* idOnReturn, const char* desc) override
{
	return _obtain_unique_id(idOnReturn, desc);
}
	
virtual jvxErrorType JVX_CALLINGCONVENTION release_unique_id(jvxSize idOnReturn) override
{
	return _release_unique_id(idOnReturn);
}

virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_unique_id(jvxSize idOnReturn, jvxApiString* onRet) 
{
	return _descriptor_unique_id(idOnReturn, onRet);
}
