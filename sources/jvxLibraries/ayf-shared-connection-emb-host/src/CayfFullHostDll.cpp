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

#include "interfaces/all-hosts/configHostFeatures_common.h"
extern "C"
{
	jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
	{
		configureHost_features* theFeaturesH = NULL;

		features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesH), JVX_HOST_IMPLEMENTATION_HOST);
		if (theFeaturesH)
		{
			if (theFeaturesH->hHost)
			{
				theFeaturesH->hHost->add_external_factory(static_cast<IjvxExternalModuleFactory*>(&globalHostInstance));
			}
		}
		return(JVX_NO_ERROR);
	}

	jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
	{
		configureHost_features* theFeaturesH = NULL;

		features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesH), JVX_HOST_IMPLEMENTATION_HOST);
		if (theFeaturesH)
		{
			if (theFeaturesH->hHost)
			{
				theFeaturesH->hHost->remove_external_factory(static_cast<IjvxExternalModuleFactory*>(&globalHostInstance));
			}
		}
		return(JVX_NO_ERROR);
	}
}

/*
virtual jvxErrorType JVX_CALLINGCONVENTION add_external_factory(IjvxExternalModuleFactory* oneModFactory) = 0;
virtual jvxErrorType JVX_CALLINGCONVENTION remove_external_factory(IjvxExternalModuleFactory* oneModFactory) = 0;
*/