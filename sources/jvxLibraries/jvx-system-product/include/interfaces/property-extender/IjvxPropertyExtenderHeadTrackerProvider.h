#ifndef _IJVXPROPERTYEXTENDERHEADTRACKERPROVIDER_H_
#define _IJVXPROPERTYEXTENDERHEADTRACKERPROVIDER_H_

// Forward declaration of quaternion type
struct jvx_quat;

JVX_INTERFACE IjvxPropertyExtenderHeadTrackerProvider;

JVX_INTERFACE IjvxPropertyExtenderHeadTrackerProvider_report
{
public:
	virtual ~IjvxPropertyExtenderHeadTrackerProvider_report() {};	

	// Report a head rotation and a center of head displacement. Displacement may also be nullptr if no displacement is available (pure rotation tracker)
	virtual jvxErrorType report_rotation_displacement(IjvxPropertyExtenderHeadTrackerProvider* origin, jvx_quat* rot_data, jvxData* center_displacement_3d, jvxSize slotId = 0) = 0;
	virtual jvxErrorType report_quality(IjvxPropertyExtenderHeadTrackerProvider* origin, jvxRemoteConnectionQuality qual, jvxData qualValue = -1, jvxSize slotId = 0) = 0;
};

JVX_INTERFACE IjvxPropertyExtenderHeadTrackerProvider
{
public:
	virtual ~IjvxPropertyExtenderHeadTrackerProvider() {};
	virtual jvxErrorType register_listener(IjvxPropertyExtenderHeadTrackerProvider_report* ptr, const char* tag) = 0;
	virtual jvxErrorType unregister_listener(IjvxPropertyExtenderHeadTrackerProvider_report* ptr) = 0;
	virtual jvxErrorType request_trigger_zero_calibration(jvxBool reset = false) = 0;
};

#endif
		