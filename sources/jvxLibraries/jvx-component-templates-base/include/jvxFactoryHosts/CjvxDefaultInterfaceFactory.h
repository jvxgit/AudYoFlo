#ifndef __CJVXDEFAULTINTERFACEFACTORY_H__
#define __CJVXDEFAULTINTERFACEFACTORY_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "jvxHosts/CjvxMinHost.h"


#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

// T = IjvxFactoryHost or IjvxHost
template <class T>
class CjvxDefaultInterfaceFactory: public T, public CjvxObject, public CjvxMinHost,
	public IjvxUniqueId, public CjvxUniqueId
{
public:
	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxDefaultInterfaceFactory(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
		CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
		_common_set.theComponentType.unselected(JVX_COMPONENT_FACTORY_HOST);
		_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxInterfaceFactory*>(this));
		_common_set.thisisme = static_cast<IjvxObject*>(this);
	};

	virtual JVX_CALLINGCONVENTION ~CjvxDefaultInterfaceFactory() {};

	// ===================================================================================================

#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#include "codeFragments/simplify/jvxMinHost_simplify.h"

#include "codeFragments/simplify/jvxUniqueId_simplify.h"

};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif

