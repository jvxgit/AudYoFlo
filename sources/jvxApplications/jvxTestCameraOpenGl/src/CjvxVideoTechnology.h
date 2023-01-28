#ifndef __CJVX_VIDEO_TECHNOLOGY__
#define __CJVX_VIDEO_TECHNOLOGY__

#include "jvx.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include "CjvxVideoDevice.h"

// ==========================================================================

class CjvxVideoTechnology
{
private:
	std::vector<CjvxVideoDevice*> allDevices;
	
	IMFActivate** lstDevices;
	UINT32 numDevices;
	jvxState status;

public:
	CjvxVideoTechnology();
	
	jvxErrorType init();
	jvxErrorType terminate();

	jvxErrorType activateDevice(CjvxVideoDevice**retDevice, jvxSize id);
	jvxErrorType deactivateDevice(CjvxVideoDevice* dev);

	jvxErrorType number_devices(jvxSize* sz);
	jvxErrorType name_device(jvxSize id, std::string& nm);

};

#endif
	