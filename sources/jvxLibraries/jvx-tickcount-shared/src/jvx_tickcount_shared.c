
#include "jvx_tickcount_shared.h"

JVX_DECLARE_TIMESTAMP_DATA_RESET(tStamp);
jvxSize referenceCnt = 0;

jvxErrorType jvx_tickcount_shared_request_reference(jvxCBool forceSet)
{
	jvxErrorType res = JVX_ERROR_ALREADY_IN_USE;
	if ((referenceCnt == 0) || forceSet)
	{
		JVX_GET_TICKCOUNT_US_SETREF(&tStamp);
		res = JVX_NO_ERROR;
	}
	referenceCnt++;
	return res;
}

jvxErrorType jvx_tickcount_shared_return_reference()
{
	if (referenceCnt > 0)
	{
		referenceCnt--;
		if (referenceCnt == 0)
		{
			JVX_DECLARE_TIMESTAMP_DATA_RESET(tStampNew);
			tStamp = tStampNew;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType jvx_tickcount_shared_status(jvxSize* refCnt, jvxTimeStampData* tStRet)
{
	if(refCnt) *refCnt = referenceCnt;
	if (tStRet) *tStRet = tStamp;
	return JVX_NO_ERROR;
}

jvxErrorType jvx_tickcount_shared_get_current_usecs(jvxTick* timeNow_usec)
{
	if (referenceCnt)
	{
		if(timeNow_usec) *timeNow_usec = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_NOT_READY;
}

jvxErrorType jvx_tickcount_shared_get_abs_usecs(jvxTick* timeNow_usec)
{
	if (referenceCnt)
	{
		JVX_GET_TICKCOUNT_US_ABS_CURRENT(&tStamp);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_NOT_READY;
}
