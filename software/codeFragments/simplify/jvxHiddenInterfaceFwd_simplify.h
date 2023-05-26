// --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 
// Interface CjvxHiddenInterfacet
// --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 

virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override
{
	return(_request_hidden_interface(tp, hdl));
};

virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override
{
	return(_return_hidden_interface(tp, hdl));
};

virtual jvxErrorType JVX_CALLINGCONVENTION object_hidden_interface(IjvxObject** objRef)override
{
	if (objRef)
	{
		*objRef = static_cast<IjvxObject*>(this);
	}
	return JVX_NO_ERROR;
}