#include "CjvxExampleTechnology.h"

#include "CjvxExampleDevice.h"

//#define JVX_DARWIN_VERBOSE

CjvxExampleTechnology::CjvxExampleTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxCustomTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
#ifdef JVX_DARWIN_VERBOSE
  std::cout << "::" << __FUNCTION__ << ": " << "constructor " << _common_set.theDescriptor << " technology component" << std::endl;
#endif
}

CjvxExampleTechnology::~CjvxExampleTechnology()
{
#ifdef JVX_DARWIN_VERBOSE
  std::cout << "::" << __FUNCTION__ << ": " << "destructor " << _common_set.theDescriptor << " technology component" << std::endl;
#endif
}
	
jvxErrorType 
CjvxExampleTechnology::activate()
{
#ifdef JVX_DARWIN_VERBOSE
  std::cout << "::" << __FUNCTION__ << ": " << "activate " << _common_set.theDescriptor << " technology component" <<std::endl;
#endif
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxCustomTechnology::activate();
	if (res == JVX_NO_ERROR)
	{
#ifdef JVX_DARWIN_VERBOSE
	  std::cout << "::" << __FUNCTION__ << ": " << "Adding device " << " to " << _common_set.theDescriptor << " technology component" <<std::endl;
#endif
		// First device: The frame streamer device
		CjvxExampleDevice* newDevice = new CjvxExampleDevice("Inspire Device", false, 
			(_common_set.theDescriptor + "#" + jvx_size2String(0)).c_str(), _common_set.theFeatureClass,
			_common_set.theModuleName.c_str(), JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_CUSTOM_DEVICE, "", NULL);

		oneDeviceWrapper elm;
		elm.hdlDev = static_cast<IjvxDevice*>(newDevice);
		_common_tech_set.lstDevices.push_back(elm);
	}
	return res;
}

jvxErrorType 
CjvxExampleTechnology::deactivate()
{
#ifdef JVX_DARWIN_VERBOSE
  std::cout << "::" << __FUNCTION__ << ": " << "deactivate " << _common_set.theDescriptor << " technology component" << std::endl;
#endif
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

