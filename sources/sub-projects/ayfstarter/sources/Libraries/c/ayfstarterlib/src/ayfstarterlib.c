#include "ayfstarterlib.h"
#ifdef USE_ORC
#include "orc/orc.h"
#include "volume.orc.h"
#endif

#include "jvx_system.h"

struct ayf_starter_prv
{
	struct ayf_starter_init prm_initCopy;
	struct ayf_starter_async prm_asyncCopy;
};

jvxDspBaseErrorType ayf_starter_init(struct ayf_starter* hdl, jvxSize bsize)
{
	if (hdl->prv == NULL)
	{
		hdl->prm_init.bsize = bsize;
		hdl->prm_async.volume = 1.0;
		hdl->prm_async.runorc = c_true;

		hdl->prm_async.orc_token_backend_ip = NULL;
		hdl->prm_async.orc_token_backend_op = NULL;
		hdl->prm_async.orc_token_debug = NULL;

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

		ptr->prm_asyncCopy = hdl->prm_async;
		ptr->prm_initCopy = hdl->prm_init;

#ifdef USE_ORC
		if (hdl->prm_async.orc_token_debug)
		{
			JVX_SETENVIRONMENTVARIABLE("ORC_DEBUG", hdl->prm_async.orc_token_debug, "DUMMY");
		}

		// Initialize the first call - the backend is fixed after that
		if (hdl->prm_async.orc_token_backend_ip)
		{
			// Initialize backend - will only work at the first time
			JVX_SETENVIRONMENTVARIABLE("ORC_BACKEND", hdl->prm_async.orc_token_backend_ip, "DUMMY");
		}
		volume_orc_double_ip(NULL, 1.0, 0);

		if (hdl->prm_async.orc_token_backend_op)
		{
			JVX_SETENVIRONMENTVARIABLE("ORC_BACKEND", hdl->prm_async.orc_token_backend_op, "DUMMY");
		}
		volume_orc_double(NULL, NULL,  1.0, 0);
#endif

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
#ifdef USE_ORC
			if (hdlLocal->prm_asyncCopy.runorc == c_true)
			{
				if (inputs == outputs && numChannelsOut <= numChannelsIn) // in-place
				{
					for (jvxSize channelIdx = 0; channelIdx < numChannelsOut; channelIdx++)
					{
#ifdef JVX_DATA_FORMAT_FLOAT
						volume_orc_float_ip(inputs[channelIdx], hdlLocal->prm_asyncCopy.volume, bufferSize);
#elif defined JVX_DATA_FORMAT_DOUBLE
						volume_orc_double_ip(inputs[channelIdx], hdlLocal->prm_asyncCopy.volume, bufferSize);
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
						volume_orc_float(outputs[channelIdxOut], inputs[channelIdxIn], hdlLocal->prm_asyncCopy.volume, bufferSize);
#elif defined JVX_DATA_FORMAT_DOUBLE
						volume_orc_double(outputs[channelIdxOut], inputs[channelIdxIn], hdlLocal->prm_asyncCopy.volume, bufferSize);
#else
#error ayf_starter_process: no orc support implemented for current data format
#endif
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
					outputs[channelIdxOut][j] = inputs[channelIdxIn][j] * hdlLocal->prm_asyncCopy.volume;
					if (channelIdxOut == 0)
					{
						// Add profiling data
						JVX_DATA_OUT_DBG_TP_STR(hdl->develop.dbg_handler, prv, JVX_STARTER_DATA_DEBUG_TP0_SHIFT,
							TP0, outputs[channelIdxOut], hdlLocal->prm_initCopy.bsize);

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
				ptr->prm_asyncCopy = hdl->prm_async;
			}
			else
			{
				hdl->prm_async = ptr->prm_asyncCopy;
			}
		}

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}
