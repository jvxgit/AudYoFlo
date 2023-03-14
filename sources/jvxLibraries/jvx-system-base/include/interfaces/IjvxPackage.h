#ifndef __IJVXPACKAGE_H__
#define __IJVXPACKAGE_H__

JVX_INTERFACE IjvxPackage
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxPackage() {};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION number_components(jvxSize* numOnReturn) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_entries_component(jvxSize idx,
		jvxApiString* description, jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm) = 0;	
	virtual jvxErrorType JVX_CALLINGCONVENTION release_entries_component(jvxSize idx) = 0;	

};

#endif


