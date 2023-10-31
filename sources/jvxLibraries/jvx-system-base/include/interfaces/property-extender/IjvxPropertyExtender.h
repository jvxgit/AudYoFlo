#ifndef __IJVXPROPERTYEXTENDER_H__
#define __IJVXPROPERTYEXTENDER_H__

JVX_INTERFACE IjvxPropertyExtender
{
	
public:
	virtual JVX_CALLINGCONVENTION ~IjvxPropertyExtender(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION supports_prop_extender_type(jvxPropertyExtenderType tp) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp) = 0;
};

#endif
