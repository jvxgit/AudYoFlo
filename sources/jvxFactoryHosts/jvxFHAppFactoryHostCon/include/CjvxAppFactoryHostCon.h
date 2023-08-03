#ifndef CJVXCONNECTIONFHOST_H__
#define CJVXCONNECTIONFHOST_H__

#include "jvxHosts/CjvxFactoryHostTpl.h"

#include "CjvxAppFactoryHost.h"
#include "jvxHosts/CjvxDataConnections.h"
#include "jvxHosts/CjvxFactoryHostTpl.h"


class CjvxAppFactoryHostCon : public CjvxFactoryHost< CjvxInterfaceHostTplConnections < IjvxFactoryHost, CjvxHostInteractionTools<CjvxHostInteraction> > >
{
public:
	JVX_CALLINGCONVENTION CjvxAppFactoryHostCon(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAppFactoryHostCon();
};

#endif