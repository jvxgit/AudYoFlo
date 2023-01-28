#ifndef __JVX_AUDIO_STACK_SAMPLE_DISPENSER_CONT_H__
#define __JVX_AUDIO_STACK_SAMPLE_DISPENSER_CONT_H__

#include "jvx_dsp_base.h"
#include "jvx_dataformats.h"

#include "jvx_audio_stack_sample_dispenser_common.h"

/*                        ---------------------------------------------------
 *	   		  fromExt --> |		     fromExtBufs[                     ]		| --> fromExt
 *                        |												    |
 *                        |                                                 |
 * EXTERNAL               |                                                 |
 *                        |                                                 |
 *                        |                                                 |
 *              toExt <-- |          toExtBufs[                    ]        | <-- toExt
 *                        ---------------------------------------------------
 */
typedef struct 
{
	struct
	{
		jvxHandle* prv_callbacks;

		jvxAudioStackDispenser_lock lock_callback;
		jvxAudioStackDispenser_unlock unlock_callback;
	} callbacks;

	jvxSize numChannelsFromExternal;
	jvxSize numChannelsToExternal;
	jvxSize buffersize;
	jvxDataFormat form;
	jvxSize start_threshold;

	jvxHandle* prv;
} jvxAudioStackDispenserCont;

// =========================================================================================

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_initCfg(jvxAudioStackDispenserCont* hdl_stack);
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_init(jvxAudioStackDispenserCont* hdl_stack,
                                                               jvxHandle** fromExtBufs, jvxHandle** toExtBufs);
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_terminate(jvxAudioStackDispenserCont* hdl_stack);
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_prepare(jvxAudioStackDispenserCont* hdl_stack, jvxHandle* fheightEstimator); // fheightEstimator may be NULL pointer to deactivate estimation
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_postprocess(jvxAudioStackDispenserCont* hdl_stack);

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_external_copy(jvxAudioStackDispenserCont* hdl_stack,
                                                                        jvxHandle** fromExtBufs, jvxSize fromExtOffset,
                                                                        jvxHandle** toExtBufs, jvxSize toExtOffset, jvxSize bsize, jvxSize idTask);

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_internal_copy(jvxAudioStackDispenserCont* hdl_stack, jvxHandle** fromExtBufs,
                                                                        jvxSize fromExtOffset, jvxHandle** toExtBufs,
                                                                        jvxSize toExtOffset, jvxSize bsize, jvxSize idTask);

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_outside_ready(jvxAudioStackDispenserCont* hdl_stack, jvxCBool* is_ready,
                                                                        jvxSize size);

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_external_copy_sc_start(jvxAudioStackDispenserCont* hdl_stack, jvxSize*posi, jvxSize bsize);
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_external_copy_sc_from_ext(jvxAudioStackDispenserCont* hdl_stack,
                                                                                    jvxHandle* fromExtBuf, jvxSize fromExtOffset,
                                                                                    jvxSize bsize, jvxSize idChan, jvxSize posi);
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_external_copy_sc_to_ext(jvxAudioStackDispenserCont* hdl_stack, jvxHandle* toExtBuf,
                                                                                  jvxSize toExtOffset, jvxSize bsize, jvxSize idChan,
                                                                                  jvxSize posi);
jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_cont_external_copy_sc_terminate(jvxAudioStackDispenserCont* hdl_stack, jvxSize bsize);

jvxDspBaseErrorType jvx_audio_stack_sample_dispenser_update_fillheight(jvxAudioStackDispenserCont* hdl_stack, 
	jvxSize idTask, jvxData* fHeight,
	jvxData* out_min, jvxData* out_max, jvxData* out_operate_average, jvxData* out_operate_min,
	jvxData* out_operate_max);

#endif
