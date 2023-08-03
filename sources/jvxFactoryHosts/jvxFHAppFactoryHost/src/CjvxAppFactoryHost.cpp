#include "CjvxAppFactoryHost.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

	CjvxAppFactoryHost::CjvxAppFactoryHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
		CjvxFactoryHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS

	// Do not load components from dlls
	config.use_only_static_objects = true;
#endif
}

CjvxAppFactoryHost::~CjvxAppFactoryHost()
{
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
