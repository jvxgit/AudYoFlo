#ifndef __CJVXMIXDEVICESAUDIODEVICE_H__
#define __CJVXMIXDEVICESAUDIODEVICE_H__

template <class T> class CjvxMixDevicesAudioDevice: public CjvxAudioDevice
{
public:
	jvxSize idxProxyOnCreate = JVX_SIZE_UNSELECTED;
protected:
	CjvxMixedDevicesAudioTechnology<T>* parentTech = nullptr;

public:
	CjvxMixDevicesAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
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
		res = CjvxAudioDevice::activate();
		if(res == JVX_NO_ERROR)
		{
			res = activate_device_api();
		}
		return res;		
 	};

	jvxErrorType deactivate()
 	{
		jvxErrorType res = JVX_NO_ERROR;
		res = CjvxAudioDevice::deactivate();
		if(res == JVX_NO_ERROR)
		{
			res = deactivate_device_api();
		}
		return res;		
 	};
};

#endif
