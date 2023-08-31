#ifndef __CJVXAPPFACTORYHOST_H__
#define __CJVXAPPFACTORYHOST_H__

#include "jvx.h"
#include "jvxHosts/CjvxFactoryHostTpl.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxAppFactoryHost : public CjvxFactoryHostTpl< CjvxInterfaceHostTpl < IjvxFactoryHost, CjvxHostInteractionTools<CjvxHostInteraction> > >
{
public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxAppFactoryHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAppFactoryHost();

};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif


#endif

