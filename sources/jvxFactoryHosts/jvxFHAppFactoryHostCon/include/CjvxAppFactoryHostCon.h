#ifndef CJVXCONNECTIONFHOST_H__
#define CJVXCONNECTIONFHOST_H__

#include "CjvxAppFactoryHost.h"
#include "jvxHosts/CjvxDataConnections.h"
	
class CjvxFactoryHostConnections : public CjvxInterfaceHostTplConnections < IjvxFactoryHost, CjvxHostInteractionTools<CjvxHostInteraction> >
{
public:
	JVX_CALLINGCONVENTION CjvxFactoryHostConnections(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxInterfaceHostTplConnections <IjvxFactoryHost, CjvxHostInteractionTools<CjvxHostInteraction> >(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
	};

	virtual JVX_CALLINGCONVENTION ~CjvxFactoryHostConnections()
	{
	};
};

class CjvxAppFactoryHostCon : public CjvxAppFactoryHost,
	public IjvxDataConnections, public CjvxDataConnections
{
public:
	JVX_CALLINGCONVENTION CjvxAppFactoryHostCon(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAppFactoryHostCon();

#include "codeFragments/simplify/jvxDataConnections_simplify.h"

	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;
};

#endif