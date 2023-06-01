#ifndef _IJVXPROPERTYEXTENDERHEADTRACKERPROVIDER_H_
#define _IJVXPROPERTYEXTENDERHEADTRACKERPROVIDER_H_

JVX_INTERFACE IjvxPropertyExtenderHeadTrackerProvider;

JVX_INTERFACE IjvxPropertyExtenderHeadTrackerProvider_report
{
public:
	virtual ~IjvxPropertyExtenderHeadTrackerProvider_report() {};
	virtual jvxErrorType report_rotation_angle(IjvxPropertyExtenderHeadTrackerProvider* origin, jvxData newValDegree) = 0;
};

JVX_INTERFACE IjvxPropertyExtenderHeadTrackerProvider
{
public:
	virtual ~IjvxPropertyExtenderHeadTrackerProvider() {};
	virtual jvxErrorType register_listener(IjvxPropertyExtenderHeadTrackerProvider_report* ptr, const char* tag) = 0;
	virtual jvxErrorType unregister_listener(IjvxPropertyExtenderHeadTrackerProvider_report* ptr) = 0;
};

#endif
		