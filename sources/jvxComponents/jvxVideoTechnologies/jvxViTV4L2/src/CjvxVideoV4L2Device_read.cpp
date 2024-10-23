#include "CjvxVideoV4L2Device.h"

#include <cerrno>
#include <linux/videodev2.h>

#include <poll.h>
#include <tracy/Tracy.hpp>

#define xioctl CjvxVideoV4L2Device::xioctl

static const char * const sl_read_frame_poll = "VideoV4L2Device::poll";
static const char * const sl_read_frame_read = "VideoV4L2Device::read";

void
CjvxVideoV4L2Device::read_frame_thread_main()
{
	int res;
	res = this->init_buffers();
	assert(res == 0);
	res = this->start_capturing();
	assert(res == 0);
	FrameMarkStart(sl_read_frame_poll);
	while(this->runtime.streamState == JVX_STATE_PROCESSING)
	{
		if (auto res = this->read_frame())
		{
			std::cerr << "CjvxVideoV4L2Device::read_frame: " << std::strerror(res) << std::endl;
		}
	}
	FrameMarkEnd(sl_read_frame_poll);
	this->stop_capturing();
	this->free_buffers();
	if (runtime.streamState == JVX_STATE_COMPLETE)
	{
		// We need to stop here
		this->runtime.streamState = JVX_STATE_PREPARED;
	}
	else
	{
		assert(0);
	}
}

int
CjvxVideoV4L2Device::start_capturing()
{
	unsigned int i;
	enum v4l2_buf_type type;

	for (i = 0; i < this->buffers.size(); ++i)
	{
		struct v4l2_buffer buf = {0};

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (xioctl(this->dev_fd, VIDIOC_QBUF, &buf) == -1)
			return errno;
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (xioctl(this->dev_fd, VIDIOC_STREAMON, &type) == -1)
		return errno;

	return 0;
}

void
CjvxVideoV4L2Device::stop_capturing()
{
	if (this->dev_fd < 0)
		return;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	xioctl(this->dev_fd, VIDIOC_STREAMOFF, &type);
}

jvxErrorType
CjvxVideoV4L2Device::set_device_mode(jvxSize idModeSelect)
{
	const jvxVideoSetting &mode = this->lstModes[idModeSelect];
	struct v4l2_format fmt = {0};
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	switch (mode.subform)
	{
	case JVX_DATAFORMAT_GROUP_VIDEO_YUYV:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		break;
	case JVX_DATAFORMAT_GROUP_VIDEO_NV12:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
		break;
	case JVX_DATAFORMAT_GROUP_VIDEO_RGB24:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
		break;
	}
	fmt.fmt.pix.width = mode.width;
	fmt.fmt.pix.height = mode.height;
	fmt.fmt.pix.field = V4L2_FIELD_NONE; // Progressive scan

	// Set the format using the ioctl
	if (xioctl(this->dev_fd, VIDIOC_S_FMT, &fmt) == -1)
		return JVX_ERROR_NO_ACCESS;

	struct v4l2_streamparm streamparm = {0};
	streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	streamparm.parm.capture.timeperframe.numerator = 1;
	streamparm.parm.capture.timeperframe.denominator = mode.fps;

	// Set the new stream parameters
	if (xioctl(this->dev_fd, VIDIOC_S_PARM, &streamparm) == -1)
		return JVX_ERROR_NO_ACCESS;

	return JVX_NO_ERROR;
}

int
CjvxVideoV4L2Device::init_buffers()
{
	struct v4l2_requestbuffers req = {0};

	req.count  = 4;
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (xioctl(this->dev_fd, VIDIOC_REQBUFS, &req) == -1)
	{
		if (EINVAL == errno)
		{
			std::cerr << "CjvxVideoV4L2Device: " << this->device << " does not support memory mapping" << std::endl;
			return errno;
		}
		else
		{
			return errno;
		}
	}

	if (req.count < 2)
	{
		std::cerr << "CjvxVideoV4L2Device: Insufficient buffer memory on " << this->device << std::endl;
		return ENOMEM;
	}

	this->buffers.clear();
	this->buffers.reserve(req.count);
	for (auto i = 0u; i < req.count; ++i)
	{
		struct v4l2_buffer buf = {0};

		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = i;

		if (xioctl(this->dev_fd, VIDIOC_QUERYBUF, &buf) == -1)
		{
			std::cerr << "CjvxVideoV4L2Device: VIDIOC_QUERYBUF" << std::endl;
			return errno;
		}

		auto& buffer = this->buffers.emplace_back();
		buffer.length = buf.length;
		buffer.ptr = mmap(NULL /* start anywhere */,
				  buf.length,
				  PROT_READ | PROT_WRITE /* required */,
				  MAP_SHARED /* recommended */,
				  this->dev_fd, buf.m.offset);

		if (buffer.ptr == MAP_FAILED) {
			std::cerr << "CjvxVideoV4L2Device: mmap failed" << std::endl;
			return ENODEV;
		}
	}

	return 0;
}

void
CjvxVideoV4L2Device::free_buffers()
{
	for (const auto& buffer : this->buffers)
	{
		munmap(buffer.ptr, buffer.length);
	}
	this->buffers.clear();
}

int
CjvxVideoV4L2Device::read_frame()
{
	struct pollfd fds[1];
	int r;

	jvxTick startt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
	jvxTick stoptt;
	jvxTick deltat;

	fds[0].fd = dev_fd;
	fds[0].events = POLLIN;

	r = poll(fds, 1, 200); // 200ms timeout
	if (r == -1)
		return EIO;

	if (!(r > 0 && (fds[0].revents & POLLIN))) {
		// Timeout occurred
		return 0;
	}

	struct v4l2_buffer buf = {0};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (xioctl(this->dev_fd, VIDIOC_DQBUF, &buf) == -1)
	{
		switch (errno) {
		case EAGAIN:
			return 0;
		case EIO:
			// Could ignore EIO, see spec.
			// fall through
		default:
			std::cerr << "CjvxVideoV4L2Device: VIDIOC_DQBUF: " << std::strerror(errno) << std::endl;
			return errno;
		}
	}

	FrameMarkEnd(sl_read_frame_poll);
	FrameMarkStart(sl_read_frame_read);

	if (runtime.lastTime > 0)
	{
		runtime.avrgT = runtime.avrgT * (ALPHA)+(1 - ALPHA) * (startt - runtime.lastTime);
	}
	runtime.lastTime = startt;
	CjvxVideoDevice_genpcg::rt_info_vd.detected_rate.value = 1000000.0 / runtime.avrgT;

	if (buf.index >= this->buffers.size())
	{
		std::cerr << "CjvxVideoV4L2Device::read_frame: index >= #buffers" << std::endl;
	}

	// In this function, we get the desired buffer target index
	jvxErrorType res = JVX_NO_ERROR;
	res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
	assert(res == JVX_NO_ERROR);

	jvxByte** ptrBytes = jvx_process_icon_extract_output_buffers<jvxByte>(_common_set_ocon.theData_out);
	jvxUInt8* dest = (jvxUInt8*)ptrBytes[0]; // <- channel #0

	jvxSize sz = _common_set_ocon.theData_out.con_params.buffersize * jvxDataFormat_getsize(_common_set_ocon.theData_out.con_params.format);
	memcpy(dest, this->buffers[buf.index].ptr, sz);

	runtime.numFrames++;
	FrameMarkEnd(sl_read_frame_read);

	// Process buffer
	res = _common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
	assert(res == JVX_NO_ERROR);

	// Processing complete
	res = _common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
	assert(res == JVX_NO_ERROR);

	if (xioctl(this->dev_fd, VIDIOC_QBUF, &buf) == -1)
	{
		std::cerr << "CjvxVideoV4L2Device: VIDIOC_QBUF: " << std::strerror(errno) << std::endl;
		return errno;
	}

	stoptt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
	deltat = stoptt - startt;
	CjvxVideoDevice::rt_info_vd.delta_t_copy.value = CjvxVideoDevice::rt_info_vd.delta_t_copy.value * ALPHA +
		(1 - ALPHA) * deltat;

	FrameMarkStart(sl_read_frame_poll);

	return 0;
}
