#ifndef __CJVXAPPCORECONTROLLER_H__
#define __CJVXAPPCORECONTROLLER_H__

#include "jvxAppCtrTech/CjvxAppCtrTechnology.h"

class CjvxCoreControllerTechnology: public CjvxApplicationControllerTechnology
{

public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class JVX_INTERFACE member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxCoreControllerTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	
	JVX_CALLINGCONVENTION ~CjvxCoreControllerTechnology();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate();

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate();

};

#endif
