#ifndef __IJVXTOOLSHOST_H__
#define __IJVXTOOLSHOST_H__

JVX_INTERFACE IjvxToolsHost: public IjvxInterfaceReference
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxToolsHost(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION number_tools(const jvxComponentIdentification&, jvxSize* num) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION identification_tool(const jvxComponentIdentification&, jvxSize idx, jvxApiString* description, jvxApiString* descriptor, jvxBool* multipleInstances) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_tool(const jvxComponentIdentification&, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor, jvxBitField filter_stateMask = (jvxBitField)JVX_STATE_DONTCARE) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_tool(const jvxComponentIdentification&, IjvxObject* theObject) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION instance_tool( jvxComponentType, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION return_instance_tool( jvxComponentType tp, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor) = 0;

};

#endif