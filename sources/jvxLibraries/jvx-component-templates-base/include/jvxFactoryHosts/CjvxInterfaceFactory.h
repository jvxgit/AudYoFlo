#ifndef __CJVXINTERFACEFACTORY_H__
#define __CJVXINTERFACEFACTORY_H__

#include "jvxFactoryHosts/CjvxDefaultInterfaceFactory.h"

#include "common/CjvxProperties.h"
#include "pcg_CjvxHost_pcg.h"

template <class T>
class CjvxInterfaceFactory : public CjvxDefaultInterfaceFactory<T>,
		public IjvxToolsHost, public IjvxProperties, public CjvxProperties,
		public CjvxHost_genpcg
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

#include "codeFragments/simplify/jvxProperties_simplify.h"

	// Interface <IjvxToolsHost>: open

};

#endif
