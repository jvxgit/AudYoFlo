#ifndef __JVX_MIXMATRIX_H__
#define __JVX_MIXMATRIX_H__

#include "jvx_dsp_base.h"

typedef jvxDspBaseErrorType (*jvx_mix_matrix_process_samples)(jvxHandle* context, jvxHandle* in, jvxHandle* out, 
						jvxData gainFactor, jvxSize number_samples);
						
typedef struct
{
	jvxData* lin_array_gain_factors;
	jvxCBool* lin_array_mute;
	jvx_mix_matrix_process_samples* lin_array_callbacks;
	jvxHandle** lin_array_contexts;
	jvxData* out_levels;
	jvxData* in_levels;
	jvxData* out_levels_max;
	jvxSize ll_flds;
	jvxData* main_out;
} jvxMixMatrix_sync;

typedef struct
{
	jvxData threshold_mix;
	jvxData smooth_alpha;
} jvxMixMatrix_async;

typedef struct
{
	jvxSize numInputChannels;
	jvxSize numOutputChannels;
	jvxSize buffersize;
} jvxMixMatrix_init;

typedef struct
{
	jvxMixMatrix_init prm_init;

	jvxMixMatrix_sync prm_sync;
	
	jvxMixMatrix_async prm_async;

	jvxHandle* prv;
} jvxMixMatrix;

// ============================================================================

jvxDspBaseErrorType jvx_mixmatrix_initConfig(jvxMixMatrix* handle);

jvxDspBaseErrorType jvx_mixmatrix_prepare(jvxMixMatrix* handleOnReturn);

jvxDspBaseErrorType jvx_mixmatrix_update(jvxMixMatrix* handle, jvxUInt16 whatToUpdate, jvxCBool do_set);

jvxDspBaseErrorType jvx_mixmatrix_process(jvxMixMatrix* handle, jvxHandle** inputs, jvxHandle** outputs);

jvxDspBaseErrorType jvx_mixmatrix_postprocess(jvxMixMatrix* handleOnReturn);


#endif
