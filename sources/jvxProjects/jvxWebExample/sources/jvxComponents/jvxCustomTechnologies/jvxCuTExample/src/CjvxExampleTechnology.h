#ifndef __CJVXEXAMPLETECHNOLOGY_H__
#define __CJVXEXAMPLETECHNOLOGY_H__

#include "jvxCustomTechnologies/CjvxCustomTechnology.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxExampleTechnology: public CjvxCustomTechnology
{
protected:


public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxExampleTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxExampleTechnology();
	
	// ======================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION activate();

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate();

};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif