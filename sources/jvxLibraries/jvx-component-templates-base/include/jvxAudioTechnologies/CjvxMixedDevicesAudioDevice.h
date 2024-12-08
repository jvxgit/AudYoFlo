#ifndef __CJVXMIXDEVICESAUDIODEVICE_H__
#define __CJVXMIXDEVICESAUDIODEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioMasterDevice.h"

template <class T> class CjvxMixDevicesAudioDevice: public CjvxAudioMasterDevice
{
public:
	jvxSize idxProxyOnCreate = JVX_SIZE_UNSELECTED;
protected:
	CjvxMixedDevicesAudioTechnology<T>* parentTech = nullptr;

public:
	CjvxMixDevicesAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{};

	void initializeDevice(jvxBool actAsProxy, jvxSize idx)
	{
		this->_common_set_device.caps.proxy = actAsProxy;
		this->idxProxyOnCreate = idx;
	}

	virtual jvxErrorType  activate_device_api() = 0;
	virtual jvxErrorType  deactivate_device_api() = 0;

	jvxErrorType activate()
 	{
		jvxErrorType res = JVX_NO_ERROR;
		res = CjvxAudioMasterDevice::activate();
		if(res == JVX_NO_ERROR)
		{
			res = activate_device_api();
		}
		return res;		
 	};

	jvxErrorType deactivate()
 	{
		jvxErrorType res = JVX_NO_ERROR;
		res = CjvxAudioMasterDevice::deactivate();
		if(res == JVX_NO_ERROR)
		{
			res = deactivate_device_api();
		}
		return res;		
 	};
};

#endif
