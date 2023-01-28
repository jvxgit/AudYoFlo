#ifndef __IJVXCONFIGURATIONATTACH_H__
#define __IJVXCONFIGURATIONATTACH_H__

JVX_INTERFACE IjvxConfigurationAttach
{
 public:
  virtual JVX_CALLINGCONVENTION ~IjvxConfigurationAttach(){};

	// this function must be called from within the main thread!! You need to reschedule main if in doubt!
	virtual jvxErrorType JVX_CALLINGCONVENTION attach_configuration_submodule(const char* prefix, IjvxConfiguration* props) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION detach_configuration_submodule(IjvxConfiguration* props) = 0;
};

#endif