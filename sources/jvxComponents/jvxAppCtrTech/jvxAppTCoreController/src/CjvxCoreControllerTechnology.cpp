#include "CjvxCoreControllerTechnology.h"
#include "CjvxCoreControllerDevice.h"

CjvxCoreControllerTechnology::CjvxCoreControllerTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxApplicationControllerTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);	
}

CjvxCoreControllerTechnology::~CjvxCoreControllerTechnology()
{
	terminate();
}

jvxErrorType
CjvxCoreControllerTechnology::activate()
{
	jvxErrorType res = _activate();
	if(res == JVX_NO_ERROR)
	{
		// Do whatever is required
		CjvxCoreControllerDevice* newDevice = new CjvxCoreControllerDevice("Core Controller Device", false, _common_set.theDescriptor.c_str(), _common_set.theFeatureClass, _common_set.theModuleName.c_str(), 
			JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE, "device/application_controller_device", NULL);

		// Whatever to be done for initialization
		oneDeviceWrapper elm;
		elm.hdlDev= static_cast<IjvxDevice*>(newDevice);

		_common_tech_set.lstDevices.push_back(elm);

	}
	return(res);
}

jvxErrorType
CjvxCoreControllerTechnology::deactivate()
{
	jvxSize i;
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
		auto elm = _common_tech_set.lstDevices.begin();
		for (; elm != _common_tech_set.lstDevices.end(); elm++)
		{
			delete(elm->hdlDev);
		}
		_common_tech_set.lstDevices.clear();
	}
	return(res);
}
