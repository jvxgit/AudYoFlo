#ifndef _IJVXPROPERTYEXTENDERNFTFPROVIDER_H_
#define _IJVXPROPERTYEXTENDERNFTFPROVIDER_H_

// Forward declaration
struct jvx_quat;

JVX_INTERFACE IjvxPropertyExtenderNftfProvider;

JVX_INTERFACE IjvxPropertyExtenderNftfProvider_report
{
public:
	virtual ~IjvxPropertyExtenderNftfProvider_report() {};
	virtual jvxErrorType report_position(IjvxPropertyExtenderNftfProvider* origin, jvxData* xyzPosiLeftEar_3, jvxData* xyzPosiRightEar_3, jvx_quat* rot_data_rel, jvxSize slotId = 0) = 0;
	virtual jvxErrorType report_nftf_fname(IjvxPropertyExtenderNftfProvider* origin, const char* fNameNftf, jvxSize slotId = 0) = 0;
	virtual jvxErrorType report_quality(IjvxPropertyExtenderNftfProvider* origin, jvxRemoteConnectionQuality qual, jvxData qualValue = -1, jvxSize slotId = 0) = 0;
};

JVX_INTERFACE IjvxPropertyExtenderNftfProvider
{
public:
	virtual ~IjvxPropertyExtenderNftfProvider() {};
	virtual jvxErrorType register_listener(IjvxPropertyExtenderNftfProvider_report* ptr, const char* tag) = 0;
	virtual jvxErrorType unregister_listener(IjvxPropertyExtenderNftfProvider_report* ptr) = 0;
};

#endif