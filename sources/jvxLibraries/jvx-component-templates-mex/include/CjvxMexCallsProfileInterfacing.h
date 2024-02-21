#ifndef __CJVXSUBCLASSMEXCOMMON_H__
#define __CJVXSUBCLASSMEXCOMMON_H__

class CjvxMexCallsProfileInterfacing
{
public:	
	virtual jvxErrorType local_prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType local_postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;
	virtual jvxErrorType local_process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) = 0;
};

#endif
