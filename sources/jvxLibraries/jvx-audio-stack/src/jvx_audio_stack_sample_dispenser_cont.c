#include "jvx_audio_stack_sample_dispenser_cont.h"
#include "jvx_dsp_base.h"

#include "jvx_buffer_estimation/jvx_estimate_buffer_fillheight.h"

typedef struct
{
	jvxAudioStackDispenserCont init;
	struct
	{
		jvxHandle** fromExtBufs;
		jvxHandle** toExtBufs;
		jvxSize numBytesElement;
		jvxData* tstampsFromExt;
		jvxData* tstampsToExt;
	} buffers;

	struct
	{
		jvxSize fill_height;
		jvxSize inout_idx;
		jvxAudioDispenser_status state;
	}admin;

	jvxHandle* fHeightEstimator;
	jvxTimeStampData tstamp_ref;
} jvxAudioStackDispenser_cont_prv;


// ==================================================================================================
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_initCfg(jvxAudioStackDispenserCont* hdl_stack)
{
	hdl_stack->buffersize = 1280;
	hdl_stack->start_threshold = 1280;
	hdl_stack->callbacks.lock_callback = NULL;
	hdl_stack->callbacks.unlock_callback = NULL;
	hdl_stack->callbacks.prv_callbacks = NULL;
	hdl_stack->form = JVX_DATAFORMAT_16BIT_LE;
	hdl_stack->numChannelsFromExternal = 1;
	hdl_stack->numChannelsToExternal = 1;
	hdl_stack->imm_start = c_false;
	hdl_stack->switch_on_buffer_full = c_true;
	hdl_stack->prv = NULL;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_init(jvxAudioStackDispenserCont* hdl_stack, jvxHandle** fromExtBufs,
	jvxHandle** toExtBufs,
	jvxData* buf_tstamps_fromExt,
	jvxData* buf_tstamps_ToExt)
{
	if (hdl_stack->prv == NULL)
	{
		jvxAudioStackDispenser_cont_prv* newHdl = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newHdl, jvxAudioStackDispenser_cont_prv);

		if (
			(hdl_stack->callbacks.lock_callback == NULL) ||
			(hdl_stack->callbacks.unlock_callback == NULL))
		{
			return JVX_DSP_ERROR_INVALID_SETTING;
		}

		newHdl->init = *hdl_stack;

		newHdl->buffers.fromExtBufs = fromExtBufs;
		newHdl->buffers.toExtBufs = toExtBufs;
		newHdl->buffers.numBytesElement = jvxDataFormat_size[newHdl->init.form];
		newHdl->buffers.tstampsFromExt = buf_tstamps_fromExt;
		newHdl->buffers.tstampsToExt = buf_tstamps_ToExt;

		newHdl->admin.fill_height = 0;
		newHdl->admin.inout_idx = 0;
		newHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_OFF;

		newHdl->fHeightEstimator = NULL;
		hdl_stack->prv = newHdl;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_terminate(jvxAudioStackDispenserCont* hdl_stack)
{
	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* oldHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;
		JVX_DSP_SAFE_DELETE_OBJECT(oldHdl);

		hdl_stack->prv = NULL;

		// Reset all values as efore init
		jvx_audio_stack_sample_dispenser_cont_initCfg(hdl_stack);

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_prepare(jvxAudioStackDispenserCont* hdl_stack, jvxHandle* fheightEstimator)
{
	jvxCBool retError = false;
	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* myHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;

		myHdl->fHeightEstimator = fheightEstimator;
		JVX_GET_TICKCOUNT_US_SETREF(&myHdl->tstamp_ref);
		myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
		if (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_OFF)
		{
			myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_CHARGING_HELD;
			myHdl->admin.fill_height = 0;
			myHdl->admin.inout_idx = 0;
		}
		else
		{
			retError = c_true;
		}
		myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);

		if (retError)
		{
			return JVX_DSP_ERROR_ABORT;
		}

		if (myHdl->init.imm_start)
		{
			myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_CHARGING;
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_postprocess(jvxAudioStackDispenserCont* hdl_stack)
{
	jvxCBool retError = false;
	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* myHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;

		myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
		if (myHdl->admin.state != JVX_AUDIO_DISPENSER_STATE_OFF)
		{
			myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_OFF;
		}
		else
		{
			retError = c_true;
		}
		myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);

		if (retError)
		{
			return JVX_DSP_ERROR_ABORT;
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

/* Following is broken
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_external_copy(jvxAudioStackDispenserCont* hdl_stack, jvxHandle** inBufs, jvxSize inOffset, jvxHandle** outBufs, jvxSize outOffset, jvxSize bsize)
{
	jvxSize i;
	jvxByte* ptrTo = NULL;
	jvxByte* ptrFrom = NULL;
	jvxCBool retError = false;

	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* myHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;

		jvxSize idx = 0;
		jvxSize fheight = 0;
		myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
		idx = myHdl->admin.inout_idx;
		fheight = myHdl->admin.fill_height;
		if (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_OFF)
		{
			retError = c_true;
		}
		myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);

		if (retError)
		{
			return JVX_DSP_ERROR_ABORT;
		}

		if ((fheight + bsize )< myHdl->init.buffersize)
		{
			jvxSize posi = (idx + fheight) % myHdl->init.buffersize;

			jvxSize l1 = JVX_MIN(bsize, myHdl->init.buffersize - posi);
			jvxSize l2 = bsize - l1;

            for (i = 0; i < myHdl->init.numChannelsOut; i++)
            {
                ptrFrom = (jvxByte*)myHdl->buffers.outBufs[i];
                ptrFrom += posi;

                ptrTo = (jvxByte*)outBufs[i];
                ptrTo += outOffset;

                if (l1)
                {
                    memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l1);
                    ptrFrom = myHdl->buffers.inBufs[i];
                    ptrTo += myHdl->buffers.numBytesElement * l1;
                }
                if (l2)
                {
                    memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l2);
                }
            }

            for (i = 0; i < myHdl->init.numChannelsIn; i++)
			{
				ptrTo = (jvxByte*)myHdl->buffers.inBufs[i];
				ptrTo += posi;

				ptrFrom = (jvxByte*)inBufs[i];
				ptrFrom += inOffset;

				if (l1)
				{
					memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l1);
					ptrTo = myHdl->buffers.inBufs[i];
					ptrFrom += myHdl->buffers.numBytesElement * l1;
				}
				if (l2)
				{
					memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l2);
				}
			}

			myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
			myHdl->admin.inout_idx += bsize;
			myHdl->admin.fill_height += bsize;
			if (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_CHARGING)
			{
				if (myHdl->admin.fill_height >= myHdl->init.start_threshold)
				{
					myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_OPERATION;
				}
			}
			myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}
*/

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_external_copy_sc_start(jvxAudioStackDispenserCont* hdl_stack, jvxSize*posi, jvxSize bsize)
{
	jvxCBool retError = false;

	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* myHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;

		jvxSize idx = 0;
		jvxSize fheight = 0;
		myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
		idx = myHdl->admin.inout_idx;
		fheight = myHdl->admin.fill_height;
		if (
		        (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_OFF)||
		        (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_CHARGING_HELD) ||
		        (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_UNCHARGING)
		)
		{
		    retError = c_true;
		}
		myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);

		if (retError)
		{
			return JVX_DSP_ERROR_ABORT;
		}

		if ((myHdl->init.buffersize - fheight ) >= bsize)
		{
			 jvxSize locpos = (idx + fheight) % myHdl->init.buffersize;
			 locpos *= myHdl->buffers.numBytesElement ;
			 *posi = locpos;
			return JVX_DSP_NO_ERROR;
		}

		myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
		myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_UNCHARGING;
		myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType 
jvx_audio_stack_sample_dispenser_cont_external_copy_sc_from_ext(jvxAudioStackDispenserCont* hdl_stack, jvxHandle* fromExt, jvxSize inOffset, jvxSize bsize, jvxSize idChan, jvxSize posi)
{
	jvxByte* ptrTo = NULL;
	jvxByte* ptrFrom = NULL;

	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* myHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;

		assert(posi + bsize * myHdl->buffers.numBytesElement <= (myHdl->init.buffersize * myHdl->buffers.numBytesElement));

		jvxSize l1 = JVX_MIN(bsize, myHdl->init.buffersize - posi);
		jvxSize l2 = bsize - l1;
		
		assert(idChan < myHdl->init.numChannelsFromExternal);

		ptrTo = (jvxByte*)myHdl->buffers.fromExtBufs[idChan];
		ptrTo += posi;

		ptrFrom = (jvxByte*)fromExt;
		ptrFrom += inOffset;

		if (l1)
		{
			memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l1);
			ptrTo = myHdl->buffers.fromExtBufs[idChan];
			ptrFrom += myHdl->buffers.numBytesElement * l1;
		}
		if (l2)
		{
			memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l2);
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType 
jvx_audio_stack_sample_dispenser_cont_external_copy_sc_to_ext(jvxAudioStackDispenserCont* hdl_stack, jvxHandle* toExtBuf, jvxSize outOffset, jvxSize bsize, jvxSize idChan, jvxSize posi)
{
	jvxByte* ptrTo = NULL;
	jvxByte* ptrFrom = NULL;

	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* myHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;

        assert(posi + bsize * myHdl->buffers.numBytesElement <= (myHdl->init.buffersize * myHdl->buffers.numBytesElement));

		jvxSize l1 = JVX_MIN(bsize, myHdl->init.buffersize - posi);
		jvxSize l2 = bsize - l1;

		assert(idChan < myHdl->init.numChannelsToExternal);

		ptrFrom = (jvxByte*)myHdl->buffers.toExtBufs[idChan];
		ptrFrom += posi;

		ptrTo = (jvxByte*)toExtBuf;
		ptrTo += outOffset;

		if (l1)
		{
			memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l1);
			ptrTo = myHdl->buffers.toExtBufs[idChan];
			ptrFrom += myHdl->buffers.numBytesElement * l1;
		}
		if (l2)
		{
			memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l2);
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType 
jvx_audio_stack_sample_dispenser_cont_external_copy_sc_terminate(jvxAudioStackDispenserCont* hdl_stack, jvxSize bsize)
{
	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* myHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;

		myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
		myHdl->admin.fill_height += bsize;
		if (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_CHARGING)
		{
			if (myHdl->admin.fill_height >= myHdl->init.start_threshold)
			{
				myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_OPERATION;
			}
		}
		myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}






jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_external_copy(jvxAudioStackDispenserCont* hdl_stack,
	jvxHandle** fromExtBufs, jvxSize fromExtOffset,
	jvxHandle** toExtBufs, jvxSize toExtOffset, jvxSize bsize,
	jvxSize idTask, jvxAudioStackDispenserProfile* prof, jvxInt64* tStamp_ext, 
	jvxData* cntTimestamp_fromExt, jvxData* cntTimestamp_toExt)
{
	jvxSize i;
	jvxByte* ptrTo = NULL;
	jvxByte* ptrFrom = NULL;
	jvxCBool retError = false;
	jvxTick tStamp;
	jvxSize * ptrTstampTo = NULL;

	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* myHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;

		jvxSize posi_samples;
		jvxSize posi_bytes;
		jvxSize fheight = 0;
		myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
		posi_samples = myHdl->admin.inout_idx;
		fheight = myHdl->admin.fill_height;
		if (
			(myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_OFF)||
			(myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_CHARGING_HELD) ||
			(myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_UNCHARGING)
			)
		{
			if (tStamp_ext)
			{
				tStamp = *tStamp_ext;
			}
			else
			{
				tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myHdl->tstamp_ref);
			}
			if (prof)
			{
				prof->tStamp = tStamp;
				prof->fHeightOnEnter = myHdl->admin.fill_height;
				prof->state_onEnter = myHdl->admin.state;
			}
			retError = c_true;
			if (prof)
			{
				prof->fHeightOnLeave = myHdl->admin.fill_height;
				prof->state_onLeave = myHdl->admin.state;
			}
		}
		myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);

		if (retError)
		{
			return JVX_DSP_ERROR_ABORT;
		}

		if ((myHdl->init.buffersize - fheight) >= bsize)
		{
			posi_samples = (posi_samples + fheight) % myHdl->init.buffersize;
			jvxSize l1 = JVX_MIN(bsize, myHdl->init.buffersize - posi_samples);
			jvxSize l2 = bsize - l1;
			
			if (myHdl->buffers.tstampsToExt && cntTimestamp_toExt)
			{
				jvxData* ptrTStamp_from = myHdl->buffers.tstampsToExt + posi_samples;
				memcpy(cntTimestamp_toExt, ptrTStamp_from, sizeof(jvxData) * l1);
				cntTimestamp_toExt += l1;
				if (l2)
				{
					ptrTStamp_from = myHdl->buffers.tstampsToExt;
					memcpy(cntTimestamp_toExt, ptrTStamp_from, sizeof(jvxData) * l2);
				}
			}

			// Transform into offset in bytes
			posi_bytes = posi_samples * myHdl->buffers.numBytesElement; // pointer shift is in bytes
			for (i = 0; i < myHdl->init.numChannelsToExternal; i++)
			{
				ptrFrom = (jvxByte*)myHdl->buffers.toExtBufs[i];
				ptrFrom += posi_bytes;
				ptrTo = (jvxByte*)toExtBufs[i];
				ptrTo += toExtOffset;

				if (l1)
				{
					memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l1);
					ptrFrom = (jvxByte*)myHdl->buffers.toExtBufs[i];
					ptrTo += myHdl->buffers.numBytesElement * l1;
				}
				if (l2)
				{
					memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l2);
				}
			}

			// =============================================================

			if (myHdl->buffers.tstampsFromExt && cntTimestamp_fromExt)
			{
				jvxData* ptrTStamp_to = myHdl->buffers.tstampsFromExt + posi_samples;
				memcpy(ptrTStamp_to, cntTimestamp_fromExt, sizeof(jvxData) * l1);
				cntTimestamp_fromExt += l1;
				if (l2)
				{
					ptrTStamp_to = myHdl->buffers.tstampsFromExt;
					memcpy(ptrTStamp_to, cntTimestamp_fromExt, sizeof(jvxData) * l2);
				}
			}
			
			for (i = 0; i < myHdl->init.numChannelsFromExternal; i++)
			{
				ptrTo = (jvxByte*)myHdl->buffers.fromExtBufs[i];
				ptrTo += posi_bytes;
				ptrFrom = (jvxByte*)fromExtBufs[i];
				ptrFrom += fromExtOffset;
				if (l1)
				{
					memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l1);
					ptrTo = (jvxByte*)myHdl->buffers.fromExtBufs[i];
					ptrFrom += myHdl->buffers.numBytesElement * l1;
				}
				if (l2)
				{
					memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l2);
				}
			}


			myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
			if (tStamp_ext)
			{
				tStamp = *tStamp_ext;
			}
			else
			{
				tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myHdl->tstamp_ref);
			}
			if (prof)
			{
				prof->tStamp = tStamp;
				prof->fHeightOnEnter = myHdl->admin.fill_height;
				prof->state_onEnter = myHdl->admin.state;
			}
			myHdl->admin.fill_height += bsize;
			if (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_CHARGING)
			{
				if (myHdl->admin.fill_height >= myHdl->init.start_threshold)
				{
					myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_OPERATION;
				}
			}

			if (myHdl->fHeightEstimator)
			{
				jvx_estimate_buffer_fillheight_process_time_ext(myHdl->fHeightEstimator, myHdl->admin.fill_height, idTask, tStamp);
			}
			if (prof)
			{
				prof->fHeightOnLeave = myHdl->admin.fill_height;
				prof->state_onLeave = myHdl->admin.state;
			}
			myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);
			return JVX_DSP_NO_ERROR;
		}

		myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
		if (tStamp_ext)
		{
			tStamp = *tStamp_ext;
		}
		else
		{
			tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myHdl->tstamp_ref);
		}
		if (prof)
		{
			prof->tStamp = tStamp;
			prof->fHeightOnEnter = myHdl->admin.fill_height;
			prof->state_onEnter = myHdl->admin.state;
		}
		if (myHdl->init.switch_on_buffer_full)
		{
			myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_UNCHARGING;
		}
		if (prof)
		{
			prof->fHeightOnLeave = myHdl->admin.fill_height;
			prof->state_onLeave = myHdl->admin.state;
		}
		myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}




jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_internal_copy(jvxAudioStackDispenserCont* hdl_stack,
	jvxHandle** fromExtBufs, jvxSize fromExtOffset,
	jvxHandle** toExtBufs, jvxSize toExtOffset, jvxSize bsize,
	jvxSize idTask, jvxAudioStackDispenserProfile* prof, jvxInt64* tStamp_ext,
	jvxData* cntTimestamp_toExt, jvxData* cntTimestamp_fromExt)
{
	jvxSize i;
	jvxByte* ptrTo = NULL;
	jvxByte* ptrFrom = NULL;
	jvxCBool retError = false;
	jvxTick tStamp;

	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* myHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;

		jvxSize posi_samples;
		jvxSize posi_bytes;
		jvxSize fheight = 0;
		myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
		posi_samples = myHdl->admin.inout_idx;
		fheight = myHdl->admin.fill_height;
		if (!
			((myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_OPERATION)||
			(myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_UNCHARGING)))
		{
			if (tStamp_ext)
			{
				tStamp = *tStamp_ext;
			}
			else
			{
				tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myHdl->tstamp_ref);
			}
			if (prof)
			{
				prof->tStamp = tStamp;
				prof->fHeightOnEnter = myHdl->admin.fill_height;
				prof->state_onEnter = myHdl->admin.state;
			}
			if (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_CHARGING_HELD)
			{
				myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_CHARGING;
			}
			retError = c_true;
			if (prof)
			{
				prof->fHeightOnLeave = myHdl->admin.fill_height;
				prof->state_onLeave = myHdl->admin.state;
			}
		}

		myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);

		if (retError)
		{
			return JVX_DSP_ERROR_ABORT;
		}

		if (fheight >= bsize)
		{
			jvxSize l1 = JVX_MIN(bsize, myHdl->init.buffersize - posi_samples);
			jvxSize l2 = bsize - l1;
			posi_bytes = posi_samples * myHdl->buffers.numBytesElement ; // pointer shift is in bytes

			// =============================================================
			if (myHdl->buffers.tstampsToExt && cntTimestamp_toExt)
			{
				jvxData* ptrTStamp_to = myHdl->buffers.tstampsToExt + posi_samples;
				memcpy(ptrTStamp_to, cntTimestamp_toExt, sizeof(jvxData) * l1);
				cntTimestamp_toExt += l1;
				if (l2)
				{
					ptrTStamp_to = myHdl->buffers.tstampsToExt;
					memcpy(ptrTStamp_to, cntTimestamp_toExt, sizeof(jvxData) * l2);
				}
			}

            for (i = 0; i < myHdl->init.numChannelsToExternal; i++)
            {
                ptrTo = (jvxByte*)myHdl->buffers.toExtBufs[i];
                ptrTo += posi_bytes;
                ptrFrom = (jvxByte*)toExtBufs[i];
                ptrFrom += toExtOffset;

                if (l1)
                {
                    memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l1);
                    ptrTo = (jvxByte*)myHdl->buffers.toExtBufs[i];
                    ptrFrom += myHdl->buffers.numBytesElement * l1;
                }
                if (l2)
                {
                    memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l2);
                }
            }

			// =============================================================
			if (myHdl->buffers.tstampsFromExt && cntTimestamp_fromExt)
			{
				jvxData* ptrTStamp_from = myHdl->buffers.tstampsFromExt + posi_samples;
				memcpy(cntTimestamp_fromExt, ptrTStamp_from, sizeof(jvxData) * l1);
				cntTimestamp_fromExt += l1;
				if (l2)
				{
					ptrTStamp_from = myHdl->buffers.tstampsFromExt;
					memcpy(cntTimestamp_fromExt, ptrTStamp_from, sizeof(jvxData) * l2);
				}
			}

            for (i = 0; i < myHdl->init.numChannelsFromExternal; i++)
			{
				ptrFrom = (jvxByte*)myHdl->buffers.fromExtBufs[i];
				ptrFrom += posi_bytes;
				ptrTo = (jvxByte*)fromExtBufs[i];
				ptrTo += fromExtOffset;

				if (l1)
				{
					memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l1);
					ptrFrom = (jvxByte*)myHdl->buffers.fromExtBufs[i];
					ptrTo += myHdl->buffers.numBytesElement * l1;
				}
				if (l2)
				{
					memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesElement * l2);
				}
			}


			myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
			if (tStamp_ext)
			{
				tStamp = *tStamp_ext;
			}
			else
			{
				tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myHdl->tstamp_ref);
			}
			if (prof)
			{
				prof->tStamp = tStamp;
				prof->fHeightOnEnter = myHdl->admin.fill_height;
				prof->state_onEnter = myHdl->admin.state;
			}
			myHdl->admin.fill_height -= bsize;
			myHdl->admin.inout_idx = (myHdl->admin.inout_idx + bsize) % myHdl->init.buffersize;
			if (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_UNCHARGING)
			{
				if (myHdl->admin.fill_height < myHdl->init.start_threshold)
				{
					myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_OPERATION;
				}
			}
			if (myHdl->fHeightEstimator)
			{
				jvx_estimate_buffer_fillheight_process_time_ext(myHdl->fHeightEstimator, myHdl->admin.fill_height, idTask, tStamp);
			}
			if (prof)
			{
				prof->fHeightOnLeave = myHdl->admin.fill_height;
				prof->state_onLeave = myHdl->admin.state;
			}
			myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);
			return JVX_DSP_NO_ERROR;
		}

		myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
		if (tStamp_ext)
		{
			tStamp = *tStamp_ext;
		}
		else
		{
			tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myHdl->tstamp_ref);
		}
		if (prof)
		{
			prof->tStamp = tStamp;
			prof->fHeightOnEnter = myHdl->admin.fill_height;
			prof->state_onEnter = myHdl->admin.state;
		}
		myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_CHARGING;
		if (prof)
		{
			prof->fHeightOnLeave = myHdl->admin.fill_height;
			prof->state_onLeave = myHdl->admin.state;
		}
		myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_outside_ready(jvxAudioStackDispenserCont* hdl_stack, jvxCBool* is_ready, jvxSize size)
{
	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* myHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;
		*is_ready = c_false;
		if (
		        (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_CHARGING)||
		        (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_OPERATION))
		{
			if (myHdl->admin.fill_height + size <= myHdl->init.buffersize)
				*is_ready = c_true;
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_update_fillheight(jvxAudioStackDispenserCont* hdl_stack, 
	jvxSize idTask, jvxData* fHeight,
	jvxData* out_min, jvxData* out_max, 
	jvxData* out_operate_average, jvxData* out_operate_min,
	jvxData* out_operate_max)
{
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* myHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;
		if (myHdl->fHeightEstimator)
		{
			myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
			resL = jvx_estimate_buffer_fillheight_read(myHdl->fHeightEstimator, idTask, fHeight, out_min, out_max, out_operate_average, out_operate_min, out_operate_max);
			myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);
		}
		else
		{
			if (fHeight)
			{
				*fHeight = (jvxData)myHdl->admin.fill_height;
			}
			if (out_min)
			{
				*out_min = 0;
			}
			if (out_max)
			{
				*out_max = 0;
			}
			if (out_operate_average)
			{
				*out_operate_average = 0;
			}
			if (out_operate_min)
			{
				*out_operate_min = 0;
			}

			if (out_operate_max)
			{
				*out_operate_max = 0;
			}
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_buffer_status(
	jvxAudioStackDispenserCont* hdl_stack,
	jvxSize* fHeight,
	jvxSize* space)
{
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_cont_prv* myHdl = (jvxAudioStackDispenser_cont_prv*)hdl_stack->prv;

		if (fHeight)
		{
			*fHeight = myHdl->admin.fill_height;
		}
		if (space)
		{
			*space = myHdl->init.buffersize - myHdl->admin.fill_height;
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}