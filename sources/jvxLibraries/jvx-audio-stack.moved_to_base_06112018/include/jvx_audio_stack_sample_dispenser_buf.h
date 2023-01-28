#ifndef __JVX_AUDIO_STACK_SAMPLE_DISPENSER_H__
#define __JVX_AUDIO_STACK_SAMPLE_DISPENSER_H__

#include "jvx_dsp_base.h"
#include "jvx_dataformats.h"

#include "jvx_audio_stack_sample_dispenser_common.h"

typedef struct 
{
	struct
	{
		jvxHandle* prv_callbacks;

		jvxAudioStackDispenser_lock lock_callback;
		jvxAudioStackDispenser_unlock unlock_callback;
	} callbacks;

	jvxSize numBuffers;
	jvxSize numChannelsIn;
	jvxSize numChannelsOut;
	jvxSize buffersize;
	jvxDataFormat form;

	jvxHandle* prv;
} jvxAudioStackDispenserBuf;

// =========================================================================================

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_initCfg(jvxAudioStackDispenserBuf* hdl_stack);

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_initCfg(jvxAudioStackDispenserBuf* hdl_stack);

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_init(jvxAudioStackDispenserBuf* hdl_stack, jvxHandle*** inBufs, jvxSize inOffset, jvxHandle*** outBufs, jvxSize outOffset);

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_terminate(jvxAudioStackDispenserBuf* hdl_stack);

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_prepare(jvxAudioStackDispenserBuf* hdl_stack);

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_postprocess(jvxAudioStackDispenserBuf* hdl_stack);

// Call this function if samples arrive from an external location
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_external_copy(jvxAudioStackDispenserBuf* hdl_stack, jvxHandle** inBufs, jvxSize inOffset, jvxHandle** outBufs, jvxSize outOffset);

// Call this function if samples are required at an internal location
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_internal_copy(jvxAudioStackDispenserBuf* hdl_stack, jvxHandle** inBufs, jvxSize inOffset, jvxHandle** outBufs, jvxSize outOffset);

// Check this function to see if external events may be accepted
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_outside_ready(jvxAudioStackDispenserBuf* hdl_stack, jvxCBool* is_ready);

#endif
