jvxErrorType JVX_CALLINGCONVENTION prepare_connect(jvxLinkDataDescriptor* theData) override
{
	return _prepare_connect(theData);
};
 
jvxErrorType JVX_CALLINGCONVENTION postprocess_connect(jvxLinkDataDescriptor* theData)override
{
	return _postprocess_connect(theData);
};
	
jvxErrorType JVX_CALLINGCONVENTION start_connect(jvxLinkDataDescriptor* theData)override
{
	return _start_connect(theData);
};

jvxErrorType JVX_CALLINGCONVENTION process_start(jvxLinkDataDescriptor* theData)override
{
	return _process_start(theData);
};

jvxErrorType JVX_CALLINGCONVENTION process_buffers(jvxLinkDataDescriptor* theData)override
{
	return _process_buffers(theData);
};

jvxErrorType JVX_CALLINGCONVENTION process_stop(jvxLinkDataDescriptor* theData)override
{
	return _process_stop(theData);
};

jvxErrorType JVX_CALLINGCONVENTION stop_connect(jvxLinkDataDescriptor* theData)override
{
	return _stop_connect(theData);
};

jvxErrorType JVX_CALLINGCONVENTION link_request_reference_object(IjvxObject** obj)override
{
	return _link_request_reference_object(obj);
};

jvxErrorType JVX_CALLINGCONVENTION link_return_reference_object(IjvxObject* obj)override
{
	return _link_return_reference_object(obj);
};

