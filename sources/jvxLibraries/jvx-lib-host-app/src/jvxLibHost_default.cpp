#include "jvxLibHost.h"

extern "C"
{
#define FUNC_CORE_PROT_DECLARE jvx_init_before_start
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_init_before_start_local
#define FUNC_CORE_PROT_ARGS
#define FUNC_CORE_PROT_RETURNVAL void

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_init_before_start=jvx_init_before_start_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_init_before_start=_jvx_init_before_start_local")
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

// ==================================================================================
