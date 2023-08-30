#ifndef __IJVXEXTERNAL_MODULE_FACTORY_H__
#define __IJVXEXTERNAL_MODULE_FACTORY_H__

JVX_INTERFACE IjvxExternalModuleFactory
{
 public:
  virtual JVX_CALLINGCONVENTION ~IjvxExternalModuleFactory(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION invite_load_components_active(IjvxHost* hostRef) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION finalize_unload_components_active(IjvxHost* hostRef) = 0;
};

#endif
