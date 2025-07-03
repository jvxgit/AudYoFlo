#include "HjvxNetworkDevices.h"

jvxSize 
HjvxNetworkDevices::numberDevices()
{
	return devices.size();
}

jvxErrorType 
HjvxNetworkDevices::descriptionDevice(jvxSize id, jvxNetDeviceHandle* devIdHandle, jvxApiString* friendlyName)
{
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	auto elm = devices.begin();
	if (id < devices.size())
	{
		std::advance(elm, id);
		if (devIdHandle)
		{
			*devIdHandle = elm->nDevHdl;
		}
		if (friendlyName)
		{
			friendlyName->assign(elm->friendlyName);
		}
		res = JVX_NO_ERROR;
	}
	return res;
}

// =======================================================================================================
