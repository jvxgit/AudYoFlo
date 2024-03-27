#ifndef __IJVXHOSTTYPEHANDLER_H__
#define __IJVXHOSTTYPEHANDLER_H__

JVX_INTERFACE IjvxHostTypeHandler
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxHostTypeHandler(){};

	//! For a component type, return the type class
	/* JVX_COMPONENT_TYPE_TECHNOLOGY,
	 * JVX_COMPONENT_TYPE_NODE,
	 * JVX_COMPONENT_TYPE_HOST,
	 * JVX_COMPONENT_TYPE_CODEC,
	 * JVX_COMPONENT_TYPE_TOOL,
	 * JVX_COMPONENT_TYPE_SIMPLE,
	 * JVX_COMPONENT_TYPE_DEVICE,
	 * JVX_COMPONENT_TYPE_PROCESS
	 */
	virtual jvxErrorType JVX_CALLINGCONVENTION component_class(jvxComponentType tp, jvxComponentTypeClass& tpCls) = 0;

	//! Add a type to be handled in the component organization 
	virtual jvxErrorType JVX_CALLINGCONVENTION add_type_host(jvxComponentType* tp, 
		jvxSize numtp, const char* description, const char* tokenConfig, 
		jvxComponentTypeClass classType, jvxSize numSlotsMax = 1, jvxSize numSubSlotsMax = 1, 
		jvxComponentTypeClass childClassType = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE) = 0;

	virtual jvxErrorType add_numsubslots_type_host(const jvxComponentIdentification& tp, jvxSize numSubSlotsMax) = 0;

	//! Number of different supported types
	virtual jvxErrorType JVX_CALLINGCONVENTION number_types_host(jvxSize* num, jvxComponentTypeClass classType) = 0;

	//! Description of a host type
	virtual jvxErrorType JVX_CALLINGCONVENTION description_type_host(jvxSize idx, jvxApiString* theDescription, jvxApiString* theTokenInConfig, jvxComponentType* tp, jvxSize numtp, jvxComponentTypeClass classType) = 0;

	//! Remove all added host types at the end of life
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_host_types() = 0;
};

#endif