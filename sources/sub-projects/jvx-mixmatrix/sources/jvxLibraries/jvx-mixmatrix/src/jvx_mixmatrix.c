#include "jvx_mixmatrix.h"
#include "jvx_dsp_base.h"

#define JVX_MIXMATRIX_THRESHOLD_DO_NOT_MIX 1e-3
#define JVX_MIXMATRIX_SMOOTH_LEVEL 0.95

typedef struct
{
	jvxMixMatrix_init prm_init;

	jvxMixMatrix_sync prm_sync;

	jvxMixMatrix_async prm_async;

} jvxMixMatrix_prv;

jvxDspBaseErrorType 
jvx_mixmatrix_initConfig(jvxMixMatrix* handle)
{
	if (handle)
	{
		handle->prm_init.buffersize = 128;
		handle->prm_init.numInputChannels = 1;
		handle->prm_init.numOutputChannels = 1;

		handle->prm_sync.lin_array_callbacks = NULL;
		handle->prm_sync.lin_array_contexts = NULL;
		handle->prm_sync.lin_array_gain_factors = NULL;
		handle->prm_sync.ll_flds = 0;

		handle->prm_sync.out_levels = NULL;
		handle->prm_sync.in_levels = NULL;

		handle->prm_async.threshold_mix = JVX_MIXMATRIX_THRESHOLD_DO_NOT_MIX;
		handle->prm_async.smooth_alpha = JVX_MIXMATRIX_SMOOTH_LEVEL;

		handle->prv = NULL;
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType jvx_mixmatrix_prepare(jvxMixMatrix* handleOnReturn)
{
	if (handleOnReturn)
	{
		if (handleOnReturn->prv == NULL)
		{
			jvxMixMatrix_prv* newPtr = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newPtr, jvxMixMatrix_prv);
			memset(newPtr, 0, sizeof(jvxMixMatrix_prv));

			newPtr->prm_init = handleOnReturn->prm_init;
			newPtr->prm_async.threshold_mix = JVX_MIXMATRIX_THRESHOLD_DO_NOT_MIX;

			handleOnReturn->prv = (jvxHandle*)newPtr;
			return(JVX_DSP_NO_ERROR);
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_mixmatrix_update(jvxMixMatrix* handle, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	if (handle)
	{
		jvxMixMatrix_prv* myPtr = (jvxMixMatrix_prv*)handle->prv;
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

jvxDspBaseErrorType jvx_mixmatrix_process(jvxMixMatrix* handle, jvxHandle** inputs, jvxHandle** outputs)
{
	jvxSize i, j, k, cnt;
	jvxData val;
	jvxData* fldOut;
	jvxData* fldIn;
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	if (handle)
	{
		jvxMixMatrix_prv* myPtr = (jvxMixMatrix_prv*)handle->prv;
		if (myPtr)
		{
			for (j = 0; j < myPtr->prm_init.numOutputChannels; j++)
			{
				fldOut = outputs[j];
				memset(fldOut, 0, sizeof(jvxData) * myPtr->prm_init.buffersize);
			}

			if (myPtr->prm_sync.lin_array_gain_factors && myPtr->prm_sync.lin_array_mute)
			{
				if (myPtr->prm_sync.lin_array_callbacks)
				{
					cnt = 0;
					for (i = 0; i < myPtr->prm_init.numInputChannels; i++)
					{
						for (j = 0; j < myPtr->prm_init.numOutputChannels; j++)
						{
							if (cnt < myPtr->prm_sync.ll_flds)
							{
								if (myPtr->prm_sync.lin_array_gain_factors[cnt] > myPtr->prm_async.threshold_mix && (!myPtr->prm_sync.lin_array_mute[cnt]))
								{
									if (myPtr->prm_sync.lin_array_callbacks[cnt])
									{
										res = myPtr->prm_sync.lin_array_callbacks[cnt](myPtr->prm_sync.lin_array_contexts[cnt], inputs[i], outputs[j],
											myPtr->prm_sync.lin_array_gain_factors[cnt], myPtr->prm_init.buffersize);
									}
									else
									{
										fldIn = inputs[i];
										fldOut = outputs[j];
										for (k = 0; k < myPtr->prm_init.buffersize; k++)
										{
											*fldOut++ += myPtr->prm_sync.lin_array_gain_factors[cnt] * *fldIn++;
										}
									}
								}
							}
							cnt++;
						}
					}
				}
				else
				{
					cnt = 0;
					for (i = 0; i < myPtr->prm_init.numInputChannels; i++)
					{
						for (j = 0; j < myPtr->prm_init.numOutputChannels; j++)
						{
							if (cnt < myPtr->prm_sync.ll_flds)
							{
								if (myPtr->prm_sync.lin_array_gain_factors[cnt] > myPtr->prm_async.threshold_mix)
								{
									fldIn = inputs[i];
									fldOut = outputs[j];
									for (k = 0; k < myPtr->prm_init.buffersize; k++)
									{
										*fldOut++ += myPtr->prm_sync.lin_array_gain_factors[cnt] * *fldIn++;
									}
								}
							}
							cnt++;
						}
					}
				}
			}

			// Determine input level
			if (myPtr->prm_sync.in_levels)
			{
				for (j = 0; j < myPtr->prm_init.numInputChannels; j++)
				{
					fldIn = inputs[j];
					for (k = 0; k < myPtr->prm_init.buffersize; k++)
					{
						val = *fldIn++;
						val *= val;
						myPtr->prm_sync.in_levels[j] = myPtr->prm_sync.in_levels[j] * myPtr->prm_async.smooth_alpha +
							val * (1 - myPtr->prm_async.smooth_alpha);
					}
				}
			}

			// Determine output level
			if (myPtr->prm_sync.out_levels && myPtr->prm_sync.out_levels_max)
			{
				for (j = 0; j < myPtr->prm_init.numOutputChannels; j++)
				{
					fldOut = outputs[j];
					for (k = 0; k < myPtr->prm_init.buffersize; k++)
					{
						*fldOut *= *myPtr->prm_sync.main_out;
						val = *fldOut;
						val *= val;
						myPtr->prm_sync.out_levels[j] = myPtr->prm_sync.out_levels[j] * myPtr->prm_async.smooth_alpha +
							val * (1 - myPtr->prm_async.smooth_alpha);
						if (val > myPtr->prm_sync.out_levels_max[j])
						{
							myPtr->prm_sync.out_levels_max[j] = val;
						}
						fldOut++;
					}
				}
			}
			return(JVX_DSP_NO_ERROR);
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_mixmatrix_postprocess(jvxMixMatrix* handle)
{
	if (handle)
	{
		jvxMixMatrix_prv* myPtr = (jvxMixMatrix_prv*)handle->prv;
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
