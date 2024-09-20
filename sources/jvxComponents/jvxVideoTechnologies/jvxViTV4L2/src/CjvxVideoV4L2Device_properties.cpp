#include "CjvxVideoV4L2Device.h"

#include <linux/videodev2.h>
#include <iostream>

// ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ##

#define xioctl CjvxVideoV4L2Device::xioctl

void
CjvxVideoV4L2Device::scanProperties()
{
	struct v4l2_fmtdesc vid_format = {0};
	vid_format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // TODO maybe better V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE (multi-planar)?
	while (xioctl(this->dev_fd, VIDIOC_ENUM_FMT, &vid_format) == 0)
	{
		// TODO check if V4L2_FMT_FLAG_EMULATED mask in vid_format.flags is set; it's software emulated in that case
		if (vid_format.pixelformat != V4L2_PIX_FMT_YUYV &&
			vid_format.pixelformat != V4L2_PIX_FMT_NV12 &&
			vid_format.pixelformat != V4L2_PIX_FMT_RGB24)
		{
			std::cout << "Unsupported pixel format: " << (const char*)vid_format.description << std::endl;
			vid_format.index++;
			continue;
		}
		struct v4l2_frmsizeenum vid_size = {0};
		vid_size.pixel_format = vid_format.pixelformat;
		while (xioctl(this->dev_fd, VIDIOC_ENUM_FRAMESIZES, &vid_size) == 0)
		{
			if (vid_size.type == V4L2_FRMSIZE_TYPE_DISCRETE) // TODO (maybe) implement stepwise
			{
				struct v4l2_frmivalenum vid_interval = {0};
				vid_interval.pixel_format = vid_format.pixelformat;
				vid_interval.width = vid_size.discrete.width;
				vid_interval.height = vid_size.discrete.height;
				jvxData fps_min = -1, fps_max = -1;
				while (xioctl(this->dev_fd, VIDIOC_ENUM_FRAMEINTERVALS, &vid_interval) == 0)
				{
					if (vid_interval.type == V4L2_FRMIVAL_TYPE_DISCRETE)
					{
						jvxData fps = (jvxData)vid_interval.discrete.denominator / vid_interval.discrete.numerator;
						if (fps_min == -1 || fps < fps_min)
							fps_min = fps;
						if (fps_max == -1 || fps > fps_max)
							fps_max = fps;
					}
					else // V4L2_FRMIVAL_TYPE_CONTINUOUS and V4L2_FRMIVAL_TYPE_STEPWISE
					{
						jvxData fps_min_i = (jvxData)vid_interval.stepwise.min.denominator / vid_interval.stepwise.min.numerator;
						if (fps_min == -1 || fps_min_i < fps_min)
							fps_min = fps_min_i;
						jvxData fps_max_i = (jvxData)vid_interval.stepwise.max.denominator / vid_interval.stepwise.max.numerator;
						if (fps_max == -1 || fps_max_i > fps_max)
							fps_max = fps_max_i;
						break; // only for index 0
					}
					vid_interval.index++;
				}

				jvxVideoSetting oneForm = {0};
				oneForm.form = JVX_DATAFORMAT_BYTE;
				oneForm.subform = JVX_DATAFORMAT_GROUP_NONE;
				oneForm.height = vid_size.discrete.height;
				oneForm.width = vid_size.discrete.width;
				oneForm.id = lstModes.size();
				oneForm.fps_min = fps_min;
				oneForm.fps_max = fps_max;
				oneForm.fps = fps_max; // TODO ?

				if (vid_format.pixelformat == V4L2_PIX_FMT_YUYV)
				{
					oneForm.subform = JVX_DATAFORMAT_GROUP_VIDEO_YUYV;
				}
				else if (vid_format.pixelformat == V4L2_PIX_FMT_NV12)
				{
					oneForm.subform = JVX_DATAFORMAT_GROUP_VIDEO_NV12;
				}
				else if (vid_format.pixelformat == V4L2_PIX_FMT_RGB24)
				{
					oneForm.subform = JVX_DATAFORMAT_GROUP_VIDEO_RGB24;
				}
				oneForm.descriptor = "Mode #" + jvx_size2String(lstModes.size()) + ": " + jvx_size2String(oneForm.width) + "x" + jvx_size2String(oneForm.height) + ", ";
				oneForm.descriptor += jvxDataFormatGroup_txt(oneForm.subform);

#ifdef VERBOSE_1
				std::cout << "Mode #" << lstModes.size() << ": " << oneForm.width << "x" << oneForm.height << ", " << std::flush;
				std::cout << jvxDataFormatGroup_txt(oneForm.subform) << std::flush;
				std::cout << "Rate: " << oneForm.fps_min << "::" << oneForm.fps_max << "--" << oneForm.fps << std::endl;
#endif
			        lstModes.push_back(oneForm);
			}
			else
			{
				std::cerr << "Pixel size is of type " << vid_size.type << " which is not implemented!" << std::endl;
			}
			vid_size.index++;
		}
		vid_format.index++;
	}
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxVideoV4L2Device, on_mode_selected)
{
	// If we changed the mode, we need to run the test again
	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genV4L2_device::configuration_mf.mode_selection))
	{
		if (JVX_CHECK_SIZE_SELECTED(_common_set_ocon.theData_out.con_link.uIdConn))
		{
			_request_test_chain_master(_common_set_ocon.theData_out.con_link.uIdConn);
		}
	}
	return JVX_NO_ERROR;
}
