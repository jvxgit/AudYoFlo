#ifndef _IJVXPROPERTYEXTENDERHEADTRACKERPROVIDER_H_
#define _IJVXPROPERTYEXTENDERHEADTRACKERPROVIDER_H_

enum class jvxHeadtrackerDataType
{
	JVX_ROTATION_FORMAT_QUAT_NONE,
	// This is data from an X->Y->Z headtracker with X as the axis in direction of look and y to the left, z upwards
	JVX_ROTATION_FORMAT_QUAT_SYSTEM_XYZ
};

JVX_INTERFACE IjvxPropertyExtenderHeadTrackerProvider;

JVX_INTERFACE IjvxPropertyExtenderHeadTrackerProvider_report
{
public:
	virtual ~IjvxPropertyExtenderHeadTrackerProvider_report() {};
	virtual jvxErrorType report_rotation_angle(IjvxPropertyExtenderHeadTrackerProvider* origin, jvxHandle* current_rotation_data, jvxHeadtrackerDataType data_format, jvxSize slotId = 0) = 0;
	virtual jvxErrorType report_quality(IjvxPropertyExtenderHeadTrackerProvider* origin, jvxRemoteConnectionQuality qual, jvxData qualValue = -1, jvxSize slotId = 0) = 0;
};

JVX_INTERFACE IjvxPropertyExtenderHeadTrackerProvider
{
public:
	virtual ~IjvxPropertyExtenderHeadTrackerProvider() {};
	virtual jvxErrorType register_listener(IjvxPropertyExtenderHeadTrackerProvider_report* ptr, const char* tag) = 0;
	virtual jvxErrorType unregister_listener(IjvxPropertyExtenderHeadTrackerProvider_report* ptr) = 0;
};

#endif
		