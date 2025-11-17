#include "ayfmixerlib.h"
#ifdef USE_ORC
#include "orc/orc.h"
#include "volume.orc.h"
#endif

#include "jvx_system.h"

struct ayf_mixer_prv
{
	struct ayf_mixer_init prmInitCopy;
	struct ayf_mixer_async prmAsyncCopy;
};

jvxDspBaseErrorType ayf_mixer_init(struct ayf_mixer* hdl, jvxSize bsize)
{
	if (hdl->prv == NULL)
	{
		hdl->prmInit.bsize = bsize;
		hdl->prmAsync.volume = 1.0;
		hdl->prmAsync.runorc = c_true;

		hdl->prmAsync.orcTokenBackend_ip = NULL;
		hdl->prmAsync.orcTokenBackend_op = NULL;
		hdl->prmAsync.orcTokenDebug = NULL;

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

// ============================================================================================================
jvxDspBaseErrorType ayf_mixer_prepare(struct ayf_mixer* hdl)
{
	if (hdl->prv == NULL)
	{
		struct ayf_mixer_prv* ptr = NULL;
		JVX_SAFE_ALLOCATE_OBJECT_Z(ptr, struct ayf_mixer_prv);

		ptr->prmAsyncCopy = hdl->prmAsync;
		ptr->prmInitCopy = hdl->prmInit;

#ifdef USE_ORC
		if (hdl->prmAsync.orcTokenDebug)
		{
			JVX_SETENVIRONMENTVARIABLE("ORC_DEBUG", hdl->prmAsync.orcTokenDebug, JVX_SETENV_OVERWRITE);
		}

		// Initialize the first call - the backend is fixed after that
		if (hdl->prmAsync.orcTokenBackend_ip)
		{
			// Initialize backend - will only work at the first time
			JVX_SETENVIRONMENTVARIABLE("ORC_BACKEND", hdl->prmAsync.orcTokenBackend_ip, JVX_SETENV_OVERWRITE);
		}
		volume_orc_double_ip(NULL, 1.0, 0);

		if (hdl->prmAsync.orcTokenBackend_op)
		{
			JVX_SETENVIRONMENTVARIABLE("ORC_BACKEND", hdl->prmAsync.orcTokenBackend_op, JVX_SETENV_OVERWRITE);
		}
		volume_orc_double(NULL, NULL,  1.0, 0);
#endif

		hdl->prv = ptr;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}


jvxDspBaseErrorType ayf_mixer_process(struct ayf_mixer* hdl, jvxData** inputs, jvxData** outputs, jvxSize numChannelsIn, jvxSize numChannelsOut, jvxSize bufferSize)
{
	struct ayf_mixer_prv* hdlLocal = (struct ayf_mixer_prv*)hdl->prv;

	if (hdlLocal)
	{
		if ((numChannelsIn > 0) && (numChannelsOut > 0))
		{
#ifdef USE_ORC
			if (hdlLocal->prmAsyncCopy.runorc == c_true)
			{
				if (inputs == outputs && numChannelsOut <= numChannelsIn) // in-place
				{
					for (jvxSize channelIdx = 0; channelIdx < numChannelsOut; channelIdx++)
					{
#ifdef JVX_DATA_FORMAT_FLOAT
						volume_orc_float_ip(inputs[channelIdx], hdlLocal->prmAsyncCopy.volume, bufferSize);
#elif defined JVX_DATA_FORMAT_DOUBLE
						volume_orc_double_ip(inputs[channelIdx], hdlLocal->prmAsyncCopy.volume, bufferSize);
#else
#error ayf_starter_process: no orc support implemented for current data format
#endif
					}
				}
				else
				{
					for (jvxSize channelIdxOut = 0; channelIdxOut < numChannelsOut; channelIdxOut++)
					{
						jvxSize channelIdxIn = channelIdxOut % numChannelsIn;
#ifdef JVX_DATA_FORMAT_FLOAT
						volume_orc_float(outputs[channelIdxOut], inputs[channelIdxIn], hdlLocal->prmAsyncCopy.volume, bufferSize);
#elif defined JVX_DATA_FORMAT_DOUBLE
						volume_orc_double(outputs[channelIdxOut], inputs[channelIdxIn], hdlLocal->prmAsyncCopy.volume, bufferSize);
#else
#error ayf_starter_process: no orc support implemented for current data format
#endif
						if (channelIdxOut == 0)
						{
							// Add profiling data
							JVX_DATA_OUT_DBG_TP_STR(hdl->develop.dbgHandler, prv, JVX_MIXER_DATA_DEBUG_TP0_SHIFT,
								TP0, outputs[channelIdxOut], hdlLocal->prmInitCopy.bsize);

						}
					}
				}
			}
			else
			{
#else // USE_ORC
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
						JVX_DATA_OUT_DBG_TP_STR(hdl->develop.dbgHandler, prv, JVX_MIXER_DATA_DEBUG_TP0_SHIFT,
							TP0, outputs[channelIdxOut], hdlLocal->prmInitCopy.bsize);

					}
				}
			}
#endif // USE_ORC
#ifdef USE_ORC
			}
#endif			
		}
		return JVX_NO_ERROR;

	}
	return JVX_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType ayf_mixer_postprocess(struct ayf_mixer* hdl)
{
	if (hdl->prv)
	{
		struct ayf_mixer_prv* ptr = (struct ayf_mixer_prv*)hdl->prv;
		JVX_SAFE_DELETE_OBJECT(ptr);
		hdl->prv = NULL;

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;

}

jvxDspBaseErrorType ayf_mixer_update(struct ayf_mixer* hdl, jvxUInt16 operation, jvxCBool wantToSet)
{
	if (hdl->prv)
	{
		struct ayf_mixer_prv* ptr = (struct ayf_mixer_prv*)hdl->prv;
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
