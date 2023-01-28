#include "jvx_audio_stack_sample_dispenser_buf.h"
#include "jvx_allocate.h"

typedef struct
{
	jvxAudioStackDispenserBuf init;
	struct
	{
		jvxHandle*** inBufs;
		jvxSize inOffset;

		jvxHandle*** outBufs;
		jvxSize outOffset;

		jvxSize numBytesBuf;
	} buffers;

	struct
	{
		jvxSize fill_height;
		jvxSize inout_idx;
		jvxAudioDispenser_status state;
	}admin;

} jvxAudioStackDispenser_buf_prv;


// ==================================================================================================
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_initCfg(jvxAudioStackDispenserBuf* hdl_stack)
{
	hdl_stack->buffersize = 128;
	hdl_stack->callbacks.lock_callback = NULL;
	hdl_stack->callbacks.unlock_callback = NULL;
	hdl_stack->callbacks.prv_callbacks = NULL;
	hdl_stack->form = JVX_DATAFORMAT_16BIT_LE;
	hdl_stack->numBuffers = 2;
	hdl_stack->numChannelsIn = 1;
	hdl_stack->numChannelsOut = 1;
	hdl_stack->prv = NULL;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_init(jvxAudioStackDispenserBuf* hdl_stack, jvxHandle*** inBufs, jvxSize inOffset, jvxHandle*** outBufs, jvxSize outOffset)
{
	if (hdl_stack->prv == NULL)
	{
		jvxAudioStackDispenser_buf_prv* newHdl = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newHdl, jvxAudioStackDispenser_buf_prv);

		newHdl->init = *hdl_stack;

		newHdl->buffers.inBufs = inBufs;
		newHdl->buffers.inOffset = inOffset;

		newHdl->buffers.outBufs = outBufs;
		newHdl->buffers.outOffset = outOffset;

		newHdl->buffers.numBytesBuf = jvxDataFormat_size[newHdl->init.form] * newHdl->init.buffersize;

		newHdl->admin.fill_height = 0;
		newHdl->admin.inout_idx = 0;
		newHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_OFF;

		hdl_stack->prv = newHdl;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_terminate(jvxAudioStackDispenserBuf* hdl_stack)
{
	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_buf_prv* oldHdl = (jvxAudioStackDispenser_buf_prv*)hdl_stack->prv;
		JVX_DSP_SAFE_DELETE_OBJECT(oldHdl);

		hdl_stack->prv = NULL;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_prepare(jvxAudioStackDispenserBuf* hdl_stack)
{
	jvxCBool retError = false;
	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_buf_prv* myHdl = (jvxAudioStackDispenser_buf_prv*)hdl_stack->prv;

		myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
		if (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_OFF)
		{
			myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_CHARGING;
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

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_postprocess(jvxAudioStackDispenserBuf* hdl_stack)
{
	jvxCBool retError = false;
	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_buf_prv* myHdl = (jvxAudioStackDispenser_buf_prv*)hdl_stack->prv;

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

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_external_copy(jvxAudioStackDispenserBuf* hdl_stack, jvxHandle** inBufs, jvxSize inOffset, jvxHandle** outBufs, jvxSize outOffset)
{
	jvxSize i;
	jvxByte* ptrTo = NULL;
	jvxByte* ptrFrom = NULL;
	jvxCBool retError = false;

	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_buf_prv* myHdl = (jvxAudioStackDispenser_buf_prv*)hdl_stack->prv;

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

		if (fheight < myHdl->init.numBuffers)
		{
			for (i = 0; i < myHdl->init.numChannelsIn; i++)
			{
				ptrTo = (jvxByte*)myHdl->buffers.inBufs[idx][i];
				ptrTo += myHdl->buffers.inOffset;
				ptrFrom = (jvxByte*)inBufs[i];
				ptrFrom += inOffset;
				memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesBuf);
			}
			for (i = 0; i < myHdl->init.numChannelsOut; i++)
			{
				ptrFrom = (jvxByte*)myHdl->buffers.outBufs[idx][i];
				ptrFrom += myHdl->buffers.outOffset;
				ptrTo = (jvxByte*)outBufs[i];
				ptrTo += outOffset;
				memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesBuf);
			}
			myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
			myHdl->admin.inout_idx++;
			myHdl->admin.fill_height++;
			if (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_CHARGING)
			{
				if (myHdl->admin.fill_height == myHdl->init.numBuffers)
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

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_internal_copy(jvxAudioStackDispenserBuf* hdl_stack, jvxHandle** inBufs, jvxSize inOffset, jvxHandle** outBufs, jvxSize outOffset)
{
	jvxSize i;
	jvxByte* ptrTo = NULL;
	jvxByte* ptrFrom = NULL;
	jvxCBool retError = false;

	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_buf_prv* myHdl = (jvxAudioStackDispenser_buf_prv*)hdl_stack->prv;

		jvxSize idx = 0;
		jvxSize fheight = 0;
		myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
		idx = myHdl->admin.inout_idx;
		fheight = myHdl->admin.fill_height;
		if (myHdl->admin.state != JVX_AUDIO_DISPENSER_STATE_OPERATION)
		{
			retError = c_true;
		}
		myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);

		if (retError)
		{
			return JVX_DSP_ERROR_ABORT;
		}

		if (fheight)
		{
			idx = (idx + myHdl->init.numBuffers - fheight) % myHdl->init.numBuffers;

			for (i = 0; i < myHdl->init.numChannelsIn; i++)
			{
				ptrFrom = (jvxByte*)myHdl->buffers.inBufs[idx][i];
				ptrFrom += myHdl->buffers.inOffset;
				ptrTo = (jvxByte*)inBufs[i];
				ptrTo += inOffset;
				memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesBuf);
			}
			for (i = 0; i < myHdl->init.numChannelsOut; i++)
			{
				ptrTo = (jvxByte*)myHdl->buffers.outBufs[idx][i];
				ptrTo += myHdl->buffers.outOffset;
				ptrFrom = (jvxByte*)outBufs[i];
				ptrFrom += outOffset;
				memcpy(ptrTo, ptrFrom, myHdl->buffers.numBytesBuf);
			}
			myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
			myHdl->admin.inout_idx++;
			myHdl->admin.fill_height++;
			if (myHdl->admin.state == JVX_AUDIO_DISPENSER_STATE_CHARGING)
			{
				if (myHdl->admin.fill_height == myHdl->init.numBuffers)
				{
					myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_OPERATION;
				}
			}
			myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);
			return JVX_DSP_NO_ERROR;
		}
		else
		{
			myHdl->init.callbacks.lock_callback(myHdl->init.callbacks.prv_callbacks);
			myHdl->admin.state = JVX_AUDIO_DISPENSER_STATE_CHARGING;
			myHdl->init.callbacks.unlock_callback(myHdl->init.callbacks.prv_callbacks);
		}
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_outside_ready(jvxAudioStackDispenserBuf* hdl_stack, jvxCBool* is_ready)
{
	if (hdl_stack->prv)
	{
		jvxAudioStackDispenser_buf_prv* myHdl = (jvxAudioStackDispenser_buf_prv*)hdl_stack->prv;
		*is_ready = c_false;
		if (myHdl->admin.state >= JVX_AUDIO_DISPENSER_STATE_CHARGING)
		{
			if (myHdl->admin.fill_height < myHdl->init.numBuffers)
				*is_ready = c_true;
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

