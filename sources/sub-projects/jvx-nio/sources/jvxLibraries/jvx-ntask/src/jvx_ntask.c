#include "jvx_ntask.h"
#include "jvx_dsp_base.h"

typedef struct
{
	jvxNtask_init prm_init;

	jvxNtask_sync prm_sync;

	jvxNtask_async prm_async;

} jvxNtask_prv;

jvxDspBaseErrorType 
jvx_ntask_initConfig(jvxNtask* handle)
{
	if (handle)
	{
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_NO_ERROR); 
}

jvxDspBaseErrorType jvx_ntask_prepare(jvxNtask* handleOnReturn)
{
	if (handleOnReturn)
	{
		if (handleOnReturn->prv == NULL)
		{
			jvxNtask_prv* newPtr = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newPtr, jvxNtask_prv);
			memset(newPtr, 0, sizeof(jvxNtask_prv));

			newPtr->prm_init = handleOnReturn->prm_init;
			handleOnReturn->prv = (jvxHandle*)newPtr;

			return(JVX_DSP_NO_ERROR);
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_ntask_update(jvxNtask* handle, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	if (handle)
	{
		jvxNtask_prv* myPtr = (jvxNtask_prv*)handle->prv;
		if(myPtr)
		{
			if (whatToUpdate == JVX_DSP_UPDATE_SYNC)
			{
				if (do_set)
				{
					myPtr->prm_sync = handle->prm_sync;
				}
				else
				{
					handle->prm_sync = myPtr->prm_sync;
				}
			}
			if (whatToUpdate == JVX_DSP_UPDATE_ASYNC)
			{
				if (do_set)
				{
					myPtr->prm_async = handle->prm_async;
				}
				else
				{
					handle->prm_async = myPtr->prm_async;
				}
			}

			return(JVX_DSP_NO_ERROR);			
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_ntask_process(jvxNtask* handle, jvxHandle** inputs, jvxHandle** outputs)
{
	jvxSize i, j, k, cnt;
	jvxData val;
	jvxData* fldOut;
	jvxData* fldIn;
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	if (handle)
	{
		jvxNtask_prv* myPtr = (jvxNtask_prv*)handle->prv;
		if (myPtr)
		{
			return(JVX_DSP_NO_ERROR);
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_ntask_postprocess(jvxNtask* handle)
{
	if (handle)
	{
		jvxNtask_prv* myPtr = (jvxNtask_prv*)handle->prv;
		if (myPtr)
		{
			JVX_DSP_SAFE_DELETE_OBJECT(myPtr);
			handle->prv = NULL;
			return(JVX_DSP_NO_ERROR);
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}
