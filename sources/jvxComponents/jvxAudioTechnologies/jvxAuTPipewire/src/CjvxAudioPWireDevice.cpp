#include "CjvxAudioPWireDevice.h"
#include "CjvxAudioPWireTechnology.h"

CjvxAudioPWireDevice::CjvxAudioPWireDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
}


CjvxAudioPWireDevice::~CjvxAudioPWireDevice()
{
	if (parentTech)
	{		
	}
}

