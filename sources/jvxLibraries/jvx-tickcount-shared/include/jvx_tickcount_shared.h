#ifndef __JVX_TICKCOUNT_SHARED_H__
#define __JVX_TICKCOUNT_SHARED_H__

#include "jvx_dsp_base.h"

jvxErrorType jvx_tickcount_shared_request_reference(jvxCBool forceSet);
jvxErrorType jvx_tickcount_shared_return_reference();
jvxErrorType jvx_tickcount_shared_status(jvxSize* refCnt, jvxTimeStampData* tStRet);
jvxErrorType jvx_tickcount_shared_get_current_usecs(jvxTick* timeNow_usec);
jvxErrorType jvx_tickcount_shared_get_abs_usecs(jvxTick* timeNow_usec);

#endif

