#ifndef __CJVXINTERFACEFACTORY_H__
#define __CJVXINTERFACEFACTORY_H__

#include "jvxFactoryHosts/CjvxDefaultInterfaceFactory.h"

template <class T>
class CjvxInterfaceFactory : public CjvxDefaultInterfaceFactory<T>,
		public IjvxToolsHost, public IjvxUniqueId, public CjvxUniqueId,
		public IjvxProperties, public CjvxProperties
{
public:
	JVX_CALLINGCONVENTION CjvxInterfaceFactory(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
		CjvxDefaultInterfaceFactory<T>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL),
		CjvxProperties(module_name, *this)
	{
	};

	virtual JVX_CALLINGCONVENTION ~CjvxInterfaceFactory() 
	{
	};

#include "codeFragments/simplify/jvxUniqueId_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"

	// Interface <IjvxToolsHost>: open

};

#endif
