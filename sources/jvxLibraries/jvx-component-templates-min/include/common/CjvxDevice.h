#ifndef __CJVXDEVICE_H__
#define __CJVXDEVICE_H__

#include "jvx.h"

JVX_INTERFACE IjvxDevice_report
{
public:
	virtual ~IjvxDevice_report() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION on_device_caps_changed(IjvxDevice* dev) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION config_device_set(jvxSize subslotid, const char* txt = nullptr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION config_device_get(jvxSize subslotid, jvxApiString* astr) = 0;
};

// =============================================================================
class CjvxDevice
{
protected:
	struct commonSetDevice
	{
		jvxDeviceCapabilities caps;
		IjvxDevice_report* report = nullptr;
	};

	commonSetDevice _common_set_device;

public:
	CjvxDevice() 
	{
	};

	jvxErrorType _capabilities_device(jvxDeviceCapabilities& caps);
};

#endif
