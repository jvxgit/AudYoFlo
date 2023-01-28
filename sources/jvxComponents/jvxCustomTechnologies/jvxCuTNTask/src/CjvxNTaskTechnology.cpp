#include "CjvxNTaskTechnology.h"

#include "CjvxNTaskDevice.h"

//#define JVX_DARWIN_VERBOSE

CjvxNTaskTechnology::CjvxNTaskTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxCustomTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
}

CjvxNTaskTechnology::~CjvxNTaskTechnology()
{
}
	
jvxErrorType 
CjvxNTaskTechnology::activate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxCustomTechnology::activate();
	if (res == JVX_NO_ERROR)
	{

		// First device: The frame streamer device
	  CjvxNTaskDevice* newDevice = new CjvxNTaskDevice("Main NTask Device", false,
			(_common_set.theDescriptor + "#" + jvx_size2String(0)).c_str(), _common_set.theFeatureClass,
			_common_set.theModuleName.c_str(), JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_CUSTOM_DEVICE, "", NULL);

		oneDeviceWrapper elm;
		elm.hdlDev = static_cast<IjvxDevice*>(newDevice);
		_common_tech_set.lstDevices.push_back(elm);
	}
	return res;
}

jvxErrorType 
CjvxNTaskTechnology::deactivate()
{

	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxCustomTechnology::deactivate();
	if (res == JVX_NO_ERROR)
	{
		auto elm = _common_tech_set.lstDevices.begin();
		for (; elm != _common_tech_set.lstDevices.end(); elm++)
		{
			delete(elm->hdlDev);
		}

		_common_tech_set.lstDevices.clear();
	}
	return res;
}

