#include "CayfFullHostClass.h"
CayfFullHostClass globalHostInstance;

extern "C"
{
	jvxErrorType ayf_register_factory_host(const char* nm, jvxApiString& nmAsRegistered, IjvxExternalModuleFactory* regMe, int argc, const char* argv[])
	{
		return globalHostInstance.register_factory_host(nm, nmAsRegistered, regMe, argc, argv);
	}

	jvxErrorType ayf_unregister_factory_host(IjvxExternalModuleFactory* regMe)
	{
		return globalHostInstance.unregister_factory_host(regMe);
	}
}
