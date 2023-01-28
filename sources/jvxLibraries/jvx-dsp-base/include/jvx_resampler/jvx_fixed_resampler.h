#ifndef __JVX_FIXED_RESAMPLER_H__
#define __JVX_FIXED_RESAMPLER_H__

#include "jvx_dsp_base.h"
#include "jvx_circbuffer/jvx_circbuffer.h"

JVX_DSP_LIB_BEGIN

typedef struct 
{
	jvxSize lFilter;
	jvxSize buffersizeIn;
	jvxSize buffersizeOut;
	jvxDataFormat format;
	jvxData stopbandAtten_dB;
	jvxData socketVal;
} jvx_fixed_resampler_prmInit;

typedef struct 
{
	jvxData delay;
} jvx_fixed_resampler_prmDerived;

typedef struct 
{
	jvx_fixed_resampler_prmInit prmInit;
	jvx_fixed_resampler_prmDerived prmDerived;

	jvxHandle* prv;
} jvx_fixed_resampler;

jvxDspBaseErrorType jvx_fixed_resampler_initConfig(jvx_fixed_resampler* theField);
jvxDspBaseErrorType jvx_fixed_resampler_prepare(jvx_fixed_resampler* H);
jvxDspBaseErrorType jvx_fixed_resampler_postprocess(jvx_fixed_resampler* H);
jvxDspBaseErrorType jvx_fixed_resampler_process(jvx_fixed_resampler* H, void* in, void* out);

/*
 * Special implementation which takes int16 input values and converts into float output.
 * The conversion is done as part of the resampler for higher efficiency
 */
jvxDspBaseErrorType jvx_fixed_resampler_process_convert(jvx_fixed_resampler* H, jvxInt16* in, jvxData* out);

/*
 * Special implementation which takes int16 input values and converts into float output.
 * This implementation requires that only a downsampling is performed, hence upsampling factor isThe conversion is done as part of the resampler for higher efficiency
 * "1". It is not checked, hence, a crash is likely to happen if used in a wrong way.
 */
jvxDspBaseErrorType jvx_fixed_resampler_process_convert_downonly(jvx_fixed_resampler* H, jvxInt16* in, jvxData* out);

/*
 * Special implementation which takes int16 input values and converts into float output.
 * Also, this function rquires that only downsampling is applied AND the length of the
 * states buffer is a power of 2 (e.g. 64) as the modulo operation is realized by means of
 * an "AND".
 */
jvxDspBaseErrorType jvx_fixed_resampler_process_convert_downonly_modand(jvx_fixed_resampler* H, jvxInt16* in, jvxData* out);

jvxDspBaseErrorType jvx_fixed_resampler_prepare_direct(jvx_fixed_resampler* H,
	jvxSize lFilter,jvxSize bsizein, jvxSize bsizeout, jvxDataFormat form,
	jvxSize osampling, jvxSize downsampling, jvx_circbuffer* state, jvxData* impResp);
jvxDspBaseErrorType jvx_fixed_resampler_postprocess_direct(jvx_fixed_resampler* H);

JVX_DSP_LIB_END

#endif
