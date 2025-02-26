#include "CjvxVideoV4L2Technology.h"

#include <linux/videodev2.h>

#define AYF_VIDEO_DEVICE_VERBOSE

#ifdef AYF_VIDEO_DEVICE_VERBOSE
void print_video_device_caps(const struct v4l2_capability& cap)
{
  std::cout << "----> Video Device Capabilities: " << std::endl;
  if ((cap.device_caps & V4L2_CAP_VIDEO_CAPTURE))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_VIDEO_CAPTURE>" << std::endl;
    }

  if ((cap.device_caps & V4L2_CAP_VIDEO_CAPTURE_MPLANE))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_VIDEO_CAPTURE_MPLANE>" << std::endl;
    }

    if ((cap.device_caps & V4L2_CAP_VIDEO_OUTPUT))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_VIDEO_OUTPUT>" << std::endl;
    }

    if ((cap.device_caps & V4L2_CAP_VIDEO_OUTPUT_MPLANE))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_VIDEO_OUTPUT_MPLANE>" << std::endl;
    }

    if ((cap.device_caps & V4L2_CAP_VIDEO_M2M))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_VIDEO_M2M>" << std::endl;
    }

    if ((cap.device_caps & V4L2_CAP_VIDEO_M2M_MPLANE))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_VIDEO_M2M_MPLANE>" << std::endl;
    }

    if ((cap.device_caps & V4L2_CAP_VIDEO_OVERLAY))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_VIDEO_OVERLAY>" << std::endl;
    }

    if ((cap.device_caps & V4L2_CAP_VBI_CAPTURE))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_VBI_CAPTURE>" << std::endl;
    }

    if ((cap.device_caps & V4L2_CAP_VBI_OUTPUT))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_VBI_OUTPUT>" << std::endl;
    }

    if ((cap.device_caps & V4L2_CAP_SLICED_VBI_CAPTURE))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_SLICED_VBI_CAPTURE>" << std::endl;
    }

    if ((cap.device_caps & V4L2_CAP_SLICED_VBI_OUTPUT))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_SLICED_VBI_OUTPUT>" << std::endl;
    }

    if ((cap.device_caps & V4L2_CAP_RDS_CAPTURE))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_RDS_CAPTURE>" << std::endl;
    }

    if ((cap.device_caps & V4L2_CAP_VIDEO_OUTPUT_OVERLAY))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_VIDEO_OUTPUT_OVERLAY>" << std::endl;
    }

	if ((cap.device_caps & V4L2_CAP_HW_FREQ_SEEK))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_HW_FREQ_SEEK>" << std::endl;
    }

	if ((cap.device_caps & V4L2_CAP_RDS_OUTPUT))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_RDS_OUTPUT>" << std::endl;
    }
 
  	if ((cap.device_caps & V4L2_CAP_TUNER))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_TUNER>" << std::endl;
    }

  	if ((cap.device_caps & V4L2_CAP_AUDIO))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_AUDIO>" << std::endl;
    }


  	if ((cap.device_caps & V4L2_CAP_RADIO))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_RADIO>" << std::endl;
    }

  	if ((cap.device_caps & V4L2_CAP_MODULATOR))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_MODULATOR>" << std::endl;
    }

  	if ((cap.device_caps & V4L2_CAP_SDR_CAPTURE))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_SDR_CAPTURE>" << std::endl;
    }

  	if ((cap.device_caps & V4L2_CAP_EXT_PIX_FORMAT ))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_EXT_PIX_FORMAT >" << std::endl;
    }

  	if ((cap.device_caps & V4L2_CAP_SDR_OUTPUT))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_SDR_OUTPUT>" << std::endl;
    }

  	if ((cap.device_caps & V4L2_CAP_READWRITE))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_READWRITE>" << std::endl;
    }

  	if ((cap.device_caps & V4L2_CAP_ASYNCIO))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_ASYNCIO>" << std::endl;
    }
 
  	if ((cap.device_caps & V4L2_CAP_STREAMING))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_STREAMING>" << std::endl;
    }
 
  	if ((cap.device_caps & V4L2_CAP_TOUCH))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_TOUCH>" << std::endl;
    }

  	if ((cap.device_caps & V4L2_CAP_DEVICE_CAPS))
    {
      std::cout << "||----> Flag indicates device caps <V4L2_CAP_DEVICE_CAPS>" << std::endl;
    }
}

void print_video_capabilities(const struct v4l2_capability& cap)
{
   std::cout << "----> Video Capabilities: " << std::endl;
  if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_VIDEO_CAPTURE>" << std::endl;
    }

  if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_VIDEO_CAPTURE_MPLANE>" << std::endl;
    }

    if ((cap.capabilities & V4L2_CAP_VIDEO_OUTPUT))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_VIDEO_OUTPUT>" << std::endl;
    }

    if ((cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_VIDEO_OUTPUT_MPLANE>" << std::endl;
    }

    if ((cap.capabilities & V4L2_CAP_VIDEO_M2M))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_VIDEO_M2M>" << std::endl;
    }

    if ((cap.capabilities & V4L2_CAP_VIDEO_M2M_MPLANE))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_VIDEO_M2M_MPLANE>" << std::endl;
    }

    if ((cap.capabilities & V4L2_CAP_VIDEO_OVERLAY))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_VIDEO_OVERLAY>" << std::endl;
    }

    if ((cap.capabilities & V4L2_CAP_VBI_CAPTURE))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_VBI_CAPTURE>" << std::endl;
    }

    if ((cap.capabilities & V4L2_CAP_VBI_OUTPUT))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_VBI_OUTPUT>" << std::endl;
    }

    if ((cap.capabilities & V4L2_CAP_SLICED_VBI_CAPTURE))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_SLICED_VBI_CAPTURE>" << std::endl;
    }

    if ((cap.capabilities & V4L2_CAP_SLICED_VBI_OUTPUT))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_SLICED_VBI_OUTPUT>" << std::endl;
    }

    if ((cap.capabilities & V4L2_CAP_RDS_CAPTURE))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_RDS_CAPTURE>" << std::endl;
    }

    if ((cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_OVERLAY))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_VIDEO_OUTPUT_OVERLAY>" << std::endl;
    }

	if ((cap.capabilities & V4L2_CAP_HW_FREQ_SEEK))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_HW_FREQ_SEEK>" << std::endl;
    }

	if ((cap.capabilities & V4L2_CAP_RDS_OUTPUT))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_RDS_OUTPUT>" << std::endl;
    }
 
  	if ((cap.capabilities & V4L2_CAP_TUNER))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_TUNER>" << std::endl;
    }

  	if ((cap.capabilities & V4L2_CAP_AUDIO))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_AUDIO>" << std::endl;
    }


  	if ((cap.capabilities & V4L2_CAP_RADIO))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_RADIO>" << std::endl;
    }

  	if ((cap.capabilities & V4L2_CAP_MODULATOR))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_MODULATOR>" << std::endl;
    }

  	if ((cap.capabilities & V4L2_CAP_SDR_CAPTURE))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_SDR_CAPTURE>" << std::endl;
    }

  	if ((cap.capabilities & V4L2_CAP_EXT_PIX_FORMAT ))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_EXT_PIX_FORMAT >" << std::endl;
    }

  	if ((cap.capabilities & V4L2_CAP_SDR_OUTPUT))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_SDR_OUTPUT>" << std::endl;
    }

  	if ((cap.capabilities & V4L2_CAP_READWRITE))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_READWRITE>" << std::endl;
    }

  	if ((cap.capabilities & V4L2_CAP_ASYNCIO))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_ASYNCIO>" << std::endl;
    }
 
  	if ((cap.capabilities & V4L2_CAP_STREAMING))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_STREAMING>" << std::endl;
    }
 
  	if ((cap.capabilities & V4L2_CAP_TOUCH))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_TOUCH>" << std::endl;
    }

  	if ((cap.capabilities & V4L2_CAP_DEVICE_CAPS))
    {
      std::cout << "||----> Flag indicates capability <V4L2_CAP_DEVICE_CAPS>" << std::endl;
    }

}
#endif

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
		   // snprintf(device, sizeof(device), "/dev/media%d", i);

#ifdef AYF_VIDEO_DEVICE_VERBOSE
			std::cout << __FUNCTION__ << ": Opening Device " << device << std::endl;
#endif
			
			int fd = open(device, O_RDONLY);
			if (fd == -1)
				continue;

			struct v4l2_capability cap;
			if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
			{
				// Not a valid video device
#ifdef AYF_VIDEO_DEVICE_VERBOSE
	  			std::cout << __FUNCTION__ << ": --> Not a valid device, unable to query capabilities." << std::endl;
#endif
				
				close(fd);
				continue;
			}
#ifdef AYF_VIDEO_DEVICE_VERBOSE
			else
			{
	  			std::cout << __FUNCTION__ << ": --> Found a valid device." << std::endl;
			}
#endif

			// Close video driver handle here!!
			close(fd);

#ifdef AYF_VIDEO_DEVICE_VERBOSE
			print_video_capabilities(cap);
			print_video_device_caps(cap);
#endif
			
			//if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
			if (!(cap.device_caps & V4L2_CAP_VIDEO_CAPTURE))
			{
				// Not a video capture device
#ifdef AYF_VIDEO_DEVICE_VERBOSE
	  			std::cout << __FUNCTION__ << ": --> Not a video capture device." << std::endl;
#endif
				continue;
			}
#ifdef AYF_VIDEO_DEVICE_VERBOSE
			else
			{
	  			std::cout << __FUNCTION__ << ": --> Found a video capture device." << std::endl;
			}
#endif

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

