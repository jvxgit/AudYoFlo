#ifndef HJVXNETWORKDEVICES_H__
#define HJVXNETWORKDEVICES_H__

#include "jvx.h"

#ifdef JVX_OS_WINDOWS
#include "platform/windows/HjvxNetworkDevices_os.h"
#endif

enum jvxSupportedTimestampType
{
	TimestampTypeNone = 0,
	TimestampTypeSoftware = 1,
	TimestampTypeHardware = 2
};

class HjvxNetworkDevices
{
private:
	class oneNetDevice
	{
	public:
		jvxNetDeviceHandle nDevHdl = jvxNetDeviceHandleInit;
		std::string friendlyName;
		jvxSize id = JVX_SIZE_UNSELECTED;
	};

	std::list<oneNetDevice> devices;

public:
	HjvxNetworkDevices() {};
	jvxErrorType scanDevices();

	jvxSize numberDevices();
	jvxErrorType descriptionDevice(jvxSize id, jvxNetDeviceHandle* devIdHandle, jvxApiString* friendlyName);
	jvxSupportedTimestampType supportedTimestampType(jvxNetDeviceHandle* devIdHandle);
};

#endif