jvxErrorType
number_tools(const jvxComponentIdentification& tp, jvxSize* num) override
{
	return this->_number_tools(tp, num);
}

jvxErrorType
identification_tool(const jvxComponentIdentification& tp, 
	jvxSize idx, jvxApiString* description, jvxApiString* descriptor, 
	jvxBool* multipleInstances) override
{
	return this->_identification_tool(tp, idx, 
		description, descriptor,
		multipleInstances);
}

/*
 * Options to call this function for NODES and simple objects :
 * 1) Specify a valid slotid : return reference of the associated element
 * 2) Specify slotid JVX_SIZE_UNSELECTED :
 * Return the single offside object
 * 3) Specify slotid JVX_SIZE_DONTCARE :
 * Search the currently selected objects of the type to find among those objects that have the right state(filter_stateMask)
 * -filter_id = 0 : Description matches the string in filter_token
 * -filter_id = 1 : Descriptor matches the string in filter_token
 * -otherwise : Module name matches the string in filter_token
 * */
jvxErrorType
reference_tool(const jvxComponentIdentification& tp,
	IjvxObject** theObject, jvxSize filter_id,
	const char* filter_descriptor,
	jvxBitField filter_stateMask,
	IjvxReferenceSelector* decider) override
{
	if (tp == JVX_COMPONENT_HOST)
	{
		if (theObject) *theObject = static_cast<IjvxHost*>(this);
		return JVX_NO_ERROR;
	}

	return this->_reference_tool(tp, theObject, filter_id,
		filter_descriptor, filter_stateMask,
		decider);
}

jvxErrorType
return_reference_tool(const jvxComponentIdentification& tp, IjvxObject* theObject) override
{
	if (tp == JVX_COMPONENT_HOST)
	{
		if (theObject == static_cast<IjvxHost*>(this))
		{
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_ARGUMENT;
	}
	return this->_return_reference_tool(tp, theObject);
}

jvxErrorType
instance_tool(jvxComponentType tp, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor) override
{
	return this->_instance_tool(tp, theObject, filter_id, filter_descriptor);
}

jvxErrorType
return_instance_tool(jvxComponentType tp, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor) override
{
	return this->_return_instance_tool(tp, theObject, filter_id, filter_descriptor);
}