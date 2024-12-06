#ifndef __CJVXAUDIOPWIREDEVICE_H__
#define __CJVXAUDIOPWIREDEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"
#include "jvxAudioTechnologies/CjvxMixedDevicesAudioTechnology.h"
#include "jvxAudioTechnologies/CjvxMixedDevicesAudioDevice.h"
#include "pcg_exports_device.h"

// class CjvxAudioPWireTechnology<CjvxAudioPWireDevice>;

class oneDevice;

class CjvxAudioPWireDevice: public CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>, public genPWire_device
{
protected:
	oneDevice* theDevicehandle = nullptr;

public:
	JVX_CALLINGCONVENTION CjvxAudioPWireDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAudioPWireDevice();

	void setReferencesApi(oneDevice* theDevicehandle);
};

#endif
