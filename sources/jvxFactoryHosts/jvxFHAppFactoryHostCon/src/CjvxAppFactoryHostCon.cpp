#include "CjvxAppFactoryHostCon.h"

CjvxAppFactoryHostCon::CjvxAppFactoryHostCon(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) : 
	CjvxFactoryHost< CjvxInterfaceHostTplConnections < IjvxFactoryHost, CjvxHostInteractionTools<CjvxHostInteraction> > >(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
}

CjvxAppFactoryHostCon::~CjvxAppFactoryHostCon()
{
}

