#ifndef __CJVXSIGNALPROCESSINGTECHNOLOGYDEPLOY__H__
#define __CJVXSIGNALPROCESSINGTECHNOLOGYDEPLOY__H__

#include "jvxTechnologies/CjvxTemplateTechnology.h"
#include "CjvxSignalProcessingDeviceDeploy.h"

class CjvxSignalProcessingTechnologyDeploy : public CjvxTemplateTechnology<CjvxSignalProcessingDeviceDeploy>
{
public: 
	JVX_CALLINGCONVENTION CjvxSignalProcessingTechnologyDeploy(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxSignalProcessingTechnologyDeploy();
};

#endif
