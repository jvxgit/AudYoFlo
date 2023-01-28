#ifndef __IJVXPROPERTYATTACH_H__
#define __IJVXPROPERTYATTACH_H__

JVX_INTERFACE IjvxPropertyAttach
{
 public:
  virtual JVX_CALLINGCONVENTION ~IjvxPropertyAttach(){};

	// this function must be called from within the main thread!! You need to reschedule main if in doubt!
	virtual jvxErrorType JVX_CALLINGCONVENTION attach_property_submodule(const char* prefix, IjvxProperties* props) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION detach_property_submodule(IjvxProperties* props) = 0;
};

#endif

