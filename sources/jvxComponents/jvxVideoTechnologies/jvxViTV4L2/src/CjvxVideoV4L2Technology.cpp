#include "CjvxVideoV4L2Technology.h"

#include <linux/videodev2.h>

CjvxVideoV4L2Technology::CjvxVideoV4L2Technology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxTemplateTechnology<CjvxVideoV4L2Device>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	this->numberDevicesInit = 0;
	this->deviceNamePrefix = "VideoDevice";
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

jvxErrorType
CjvxVideoV4L2Technology::activate()
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxVideoV4L2Device>::activate();

	if (res == JVX_NO_ERROR)
	{
		char device[20];

		for (int i = 0; i < 64; i++)
		{
			snprintf(device, sizeof(device), "/dev/video%d", i);
			int fd = open(device, O_RDONLY);
			if (fd == -1)
				continue;

			struct v4l2_capability cap;
			if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
			{
				// Not a valid video device
				close(fd);
				continue;
			}

			close(fd);

			if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
			{
				// Not a video capture device
				continue;
			}

			this->numDevices += 1;

			// Allocate new device handle in user space
			CjvxVideoV4L2Device* newDevice = new CjvxVideoV4L2Device((const char*)cap.card, false, _common_set.theDescriptor.c_str(), _common_set.theFeatureClass,
				_common_set.theModuleName.c_str(), JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_VIDEO_DEVICE, "device/video_device", NULL);

			newDevice->init(this, device, (i == 0));

			// Whatever to be done for initialization
			oneDeviceWrapper elm;
			elm.hdlDev = static_cast<IjvxDevice*>(newDevice);

			_common_tech_set.lstDevices.push_back(elm);
		}
		return JVX_NO_ERROR;
	}
	return res;
}


jvxErrorType
CjvxVideoV4L2Technology::deactivate()
{
	jvxErrorType res = _deactivate();
	if (res == JVX_NO_ERROR)
	{
		for (auto&& elm : _common_tech_set.lstDevices)
			delete(elm.hdlDev);
		_common_tech_set.lstDevices.clear();

		numDevices = 0;
	}
	return res;
}

