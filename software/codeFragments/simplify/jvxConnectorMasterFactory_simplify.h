virtual jvxErrorType JVX_CALLINGCONVENTION number_connector_masters(jvxSize* num_in_connectors) override
{
	return _number_connector_masters(num_in_connectors);
};
 
virtual jvxErrorType JVX_CALLINGCONVENTION reference_connector_master(jvxSize idx, IjvxConnectionMaster** ref) override
{
	return _reference_connector_master(idx, ref);
};

virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connector_master( IjvxConnectionMaster* ref) override
{
	return _return_reference_connector_master(ref);
};
