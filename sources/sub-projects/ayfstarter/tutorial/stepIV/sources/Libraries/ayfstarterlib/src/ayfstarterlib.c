#include "ayfstarterlib.h"

struct ayf_starter_prv
{
	struct ayf_starter_init prmInitCopy;
	struct ayf_starter_async prmAsyncCopy;
};

jvxDspBaseErrorType ayf_starter_init(struct ayf_starter* hdl, jvxSize bsize)
{
	if (hdl->prv == NULL)
	{
		hdl->prmInit.bsize = bsize;
		hdl->prmAsync.volume = 1.0;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

// ============================================================================================================
jvxDspBaseErrorType ayf_starter_prepare(struct ayf_starter* hdl)
{
	if (hdl->prv == NULL)
	{
		struct ayf_starter_prv* ptr = NULL;
		JVX_SAFE_ALLOCATE_OBJECT_Z(ptr, struct ayf_starter_prv);

		ptr->prmAsyncCopy = hdl->prmAsync;
		ptr->prmInitCopy = hdl->prmInit;

		hdl->prv = ptr;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}


jvxDspBaseErrorType ayf_starter_process(struct ayf_starter* hdl, jvxData** inputs, jvxData** outputs, jvxSize numChannelsIn, jvxSize numChannelsOut, jvxSize bufferSize)
{
	struct ayf_starter_prv* hdlLocal = (struct ayf_starter_prv*)hdl->prv;

	if (hdlLocal)
	{
		if ((numChannelsIn > 0) && (numChannelsOut > 0))
		{
			for (jvxSize j = 0; j < bufferSize; j++)
			{
				for (jvxSize channelIdxOut = 0; channelIdxOut < numChannelsOut; channelIdxOut++)
				{
					jvxSize channelIdxIn = channelIdxOut % numChannelsIn;

					// We move from start to end within the buffer - it is all fine even if we run in-place
					outputs[channelIdxOut][j] = inputs[channelIdxIn][j] * hdlLocal->prmAsyncCopy.volume;
					if (channelIdxOut == 0)
					{
						// Add profiling data
						JVX_DATA_OUT_DBG_TP_STR(hdl->develop.dbgHandler, prv, JVX_STARTER_DATA_DEBUG_TP0_SHIFT,
							TP0, outputs[channelIdxOut], hdlLocal->prmInitCopy.bsize);

					}
				}
			}
			
		}
		return JVX_NO_ERROR;
	
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType ayf_starter_postprocess(struct ayf_starter* hdl)
{
	if (hdl->prv)
	{
		struct ayf_starter_prv* ptr = (struct ayf_starter_prv*)hdl->prv;
		JVX_SAFE_DELETE_OBJECT(ptr);
		hdl->prv = NULL;
		
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;

}

jvxDspBaseErrorType ayf_starter_update(struct ayf_starter* hdl, jvxUInt16 operation, jvxCBool wantToSet)
{
	if (hdl->prv)
	{
		struct ayf_starter_prv* ptr = (struct ayf_starter_prv*)hdl->prv;
		if (operation & JVX_DSP_UPDATE_ASYNC)
		{
			if (wantToSet)
			{
				ptr->prmAsyncCopy = hdl->prmAsync;
			}
			else
			{
				hdl->prmAsync = ptr->prmAsyncCopy;
			}
		}

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}
