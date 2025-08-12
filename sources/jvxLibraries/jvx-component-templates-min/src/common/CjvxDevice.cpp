#include "common/CjvxDevice.h"

jvxErrorType
CjvxDevice::_capabilities_device(jvxDeviceCapabilities& caps)
{
	caps = _common_set_device.caps;
	return JVX_NO_ERROR;
}
