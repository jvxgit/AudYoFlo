#ifndef CJVXCONNECTIONFHOST_H__
#define CJVXCONNECTIONFHOST_H__

#include "CjvxAppFactoryHost.h"
#include "jvxHosts/CjvxDataConnections.h"
	
class CjvxConnectionFHost : public CjvxAppFactoryHost,
	public IjvxDataConnections, public CjvxDataConnections
{
public:
	JVX_CALLINGCONVENTION CjvxConnectionFHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxConnectionFHost();

#include "codeFragments/simplify/jvxDataConnections_simplify.h"

	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;
};

#endif