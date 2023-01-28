#include "jvx.h"
#include "jvxFactoryHosts/CjvxDefaultInterfaceFactory.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxDefaultInterfaceFactory::CjvxDefaultInterfaceFactory(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_FACTORY_HOST);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxInterfaceFactory*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

CjvxDefaultInterfaceFactory::~CjvxDefaultInterfaceFactory()
{
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
