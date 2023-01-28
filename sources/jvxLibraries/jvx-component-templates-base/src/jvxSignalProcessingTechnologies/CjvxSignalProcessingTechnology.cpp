#include "jvx.h"

#include "jvxSignalProcessingTechnologies/CjvxSignalProcessingTechnology.h"
#include "jvxSignalProcessingTechnologies/CjvxSignalProcessingDevice.h"

CjvxSignalProcessingTechnology::CjvxSignalProcessingTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

CjvxSignalProcessingTechnology::~CjvxSignalProcessingTechnology()
{
	terminate();
}

jvxErrorType
CjvxSignalProcessingTechnology::activate()
{
	jvxErrorType res = _activate(); 
	if(res == JVX_NO_ERROR)
	{
#ifdef USE_STANDALONE_SKELETON
		// Do whatever is required
		CjvxSignalProcessingDevice* newDevice = new CjvxSignalProcessingDevice("Single Spectrum Processor", false, _common_set.theDescriptor.c_str(), _common_set.theFeatureClass, 
			_common_set.theModuleName.c_str(), JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE, "device/signal_processing_device");

		// Whatever to be done for initialization
		oneDeviceWrapper elm;
		elm.hdlDev = static_cast<IjvxDevice*>(newDevice);
		_common_tech_set.lstDevices.push_back(elm);
#endif
	}
	return(res);
}

jvxErrorType
CjvxSignalProcessingTechnology::deactivate()
{
	jvxSize i;
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
#ifdef USE_STANDALONE_SKELETON
		for(i = 0; i < _common_tech_set.lstDevices.size(); i++)
		{
			delete(_common_tech_set.lstDevices[i].hdlDev);
		}
		_common_tech_set.lstDevices.clear();
#endif
	}
	return(res);
}
