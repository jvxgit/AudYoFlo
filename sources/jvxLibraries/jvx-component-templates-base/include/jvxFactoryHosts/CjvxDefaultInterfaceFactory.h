#ifndef __CJVXDEFAULTINTERFACEFACTORY_H__
#define __CJVXDEFAULTINTERFACEFACTORY_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "jvxHosts/CjvxMinHost.h"


#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxDefaultInterfaceFactory: public IjvxFactoryHost, public CjvxObject, public CjvxMinHost
{
public:
	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxDefaultInterfaceFactory(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxDefaultInterfaceFactory();

	// ===================================================================================================

	#include "codeFragments/simplify/jvxObjects_simplify.h"
	

#ifdef JVX_OS_WINDOWS
#pragma warning( disable : 4065)
#endif
	#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#include "codeFragments/simplify/jvxMinHost_simplify.h"

};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif


#endif

