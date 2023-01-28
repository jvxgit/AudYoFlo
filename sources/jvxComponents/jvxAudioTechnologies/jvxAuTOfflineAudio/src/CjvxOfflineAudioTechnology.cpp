#include "CjvxOfflineAudioTechnology.h"


CjvxOfflineAudioTechnology::CjvxOfflineAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

CjvxOfflineAudioTechnology::~CjvxOfflineAudioTechnology()
{
}

jvxErrorType
CjvxOfflineAudioTechnology::activate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioTechnology::activate();
	if(res == JVX_NO_ERROR)
	{
		std::string device_name = "The Offline Device";
		std::string descriptor = "JVX_OFFLINE_AUDIO_DEVICE";
		CjvxOfflineAudioDevice* ptrDev = new CjvxOfflineAudioDevice(device_name.c_str(), false, 
			descriptor.c_str(), _common_set.theFeatureClass, _common_set.theModuleName.c_str(), 
			JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DEVICE, "", NULL);
		CjvxTechnology::oneDeviceWrapper elm;
		elm.hdlDev = ptrDev;
		_common_tech_set.lstDevices.push_back(elm);
	}
	return(res);
}

jvxErrorType
CjvxOfflineAudioTechnology::deactivate()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioTechnology::deactivate();
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

