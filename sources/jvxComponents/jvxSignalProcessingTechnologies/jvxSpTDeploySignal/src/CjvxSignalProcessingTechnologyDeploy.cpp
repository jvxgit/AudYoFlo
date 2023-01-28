#include "CjvxSignalProcessingTechnologyDeploy.h"

CjvxSignalProcessingTechnologyDeploy::CjvxSignalProcessingTechnologyDeploy(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxTemplateTechnology<CjvxSignalProcessingDeviceDeploy>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	this->numberDevicesInit = 1;
	this->deviceNamePrefix = "Signal Processing Deploy Device";
	allowMultipleInstances = true;
}


CjvxSignalProcessingTechnologyDeploy::~CjvxSignalProcessingTechnologyDeploy()
{

}

