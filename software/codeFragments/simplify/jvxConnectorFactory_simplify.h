virtual jvxErrorType JVX_CALLINGCONVENTION number_input_connectors(jvxSize* num_in_connectors) override
{
	return _number_input_connectors(num_in_connectors);
};
 
virtual jvxErrorType JVX_CALLINGCONVENTION number_output_connectors(jvxSize* num_out_connectors) override
{
	return _number_output_connectors(num_out_connectors);
};

virtual jvxErrorType JVX_CALLINGCONVENTION reference_input_connector(jvxSize idx, IjvxInputConnectorSelect** ref) override
{
	return _reference_input_connector(idx, ref);
};

virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_input_connector(IjvxInputConnectorSelect* ref) override
{
	return _return_reference_input_connector(ref);
};

virtual jvxErrorType JVX_CALLINGCONVENTION reference_output_connector(jvxSize idx, IjvxOutputConnectorSelect** ref) override
{
	return _reference_output_connector(idx, ref);
};

virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_output_connector(IjvxOutputConnectorSelect* ref) override
{
	return _return_reference_output_connector(ref);
};
