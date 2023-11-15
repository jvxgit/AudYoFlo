#include "CayfFullHostClass.h"
CayfFullHostClass globalHostInstance;

#include "interfaces/all-hosts/configHostFeatures_common.h"

extern "C"
{
#define FUNC_CORE_PROT_DECLARE jvx_configure_host_features
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_configure_host_features_local
#define FUNC_CORE_PROT_ARGS configureHost_features* theFeaturesH 
#define FUNC_CORE_PROT_RETURNVAL void

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_configure_host_features=jvx_configure_host_features_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_configure_host_features=_jvx_configure_host_features_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		// Default implementation does just nothing
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL

	// ========================================================================================

#define FUNC_CORE_PROT_DECLARE jvx_invalidate_host_features
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_invalidate_host_features_local
#define FUNC_CORE_PROT_ARGS configureHost_features* theFeaturesH 
#define FUNC_CORE_PROT_RETURNVAL void

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_invalidate_host_features=jvx_invalidate_host_features_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_invalidate_host_features=_jvx_invalidate_host_features_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		// Default implementation does just nothing
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL
	
}


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

	jvxErrorType ayf_load_config_content_factory_host(IjvxExternalModuleFactory* priIf, jvxConfigData** datOnReturn, const char* fName)
	{
		return globalHostInstance.load_config_content(priIf, datOnReturn, fName);
	}

	jvxErrorType ayf_release_config_content_factory_host(IjvxExternalModuleFactory* priIf, jvxConfigData* datOnReturn)
	{
		return globalHostInstance.release_config_content(priIf, datOnReturn);
	}

	jvxErrorType ayf_forward_text_command(const char* command, IjvxObject* priObj, jvxApiString& astr)
	{
		return JVX_ERROR_UNSUPPORTED;
	}
}

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
			globalHostInstance.setHost(theFeaturesH->hHost);

			jvx_configure_host_features(theFeaturesH);
		}
		return(JVX_NO_ERROR);
	}

	jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
	{
		configureHost_features* theFeaturesH = NULL;

		features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesH), JVX_HOST_IMPLEMENTATION_HOST);
		if (theFeaturesH)
		{
			jvx_invalidate_host_features(theFeaturesH);
			if (theFeaturesH->hHost)
			{
				theFeaturesH->hHost->remove_external_factory(static_cast<IjvxExternalModuleFactory*>(&globalHostInstance));
				globalHostInstance.setHost(nullptr);
			}
		}
		return(JVX_NO_ERROR);
	}
}

/*
virtual jvxErrorType JVX_CALLINGCONVENTION add_external_factory(IjvxExternalModuleFactory* oneModFactory) = 0;
virtual jvxErrorType JVX_CALLINGCONVENTION remove_external_factory(IjvxExternalModuleFactory* oneModFactory) = 0;
*/