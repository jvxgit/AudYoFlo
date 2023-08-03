#ifndef __CJVXINTERFACEHOSTTPL_H__
#define __CJVXINTERFACEHOSTTPL_H__

#include "jvx.h"
#include "jvxFactoryHosts/CjvxInterfaceFactory.h"

// Template "H" represents the host type, template "T" represents the hostinteraction specialization
template <class H, class T>
class CjvxInterfaceHostTpl : public CjvxInterfaceFactory<H>, 
	public IjvxConfiguration, public IjvxConfigurationExtender, public T
{
public:
	JVX_CALLINGCONVENTION CjvxInterfaceHostTpl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxInterfaceFactory<H>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{

	};
	virtual JVX_CALLINGCONVENTION ~CjvxInterfaceHostTpl()
	{

	};
};

template <class H, class T>
class CjvxInterfaceHostTplConnections : public CjvxInterfaceHostTpl<H, T>,
	public IjvxDataConnections, public CjvxDataConnections
{
public:
	JVX_CALLINGCONVENTION CjvxInterfaceHostTplConnections(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxInterfaceHostTpl<H, T>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{

	};
	virtual JVX_CALLINGCONVENTION ~CjvxInterfaceHostTplConnections()
	{

	};

#include "codeFragments/simplify/jvxDataConnections_simplify.h"
};
#endif
