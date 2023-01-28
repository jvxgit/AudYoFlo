#ifndef __JVX_ADAPTIVE_RESAMPLER_H__
#define __JVX_ADAPTIVE_RESAMPLER_H__

#include "jvx_dsp_base.h"

jvxDspBaseErrorType jvxAdaptiveResampler_variantI_initialize(jvxHandle** H, jvxData* delay_samples, int lagrange_toLeft, int lagrange_toRight, int lFilter, int buffersizeInMax, int oversamplingFactor, jvxDataFormat format);
jvxDspBaseErrorType jvxAdaptiveResampler_variantI_terminate(jvxHandle* H);
jvxDspBaseErrorType jvxAdaptiveResampler_variantI_process(void* in, int bSizeIn, void* out, int bSizeOut, jvxHandle* H);

jvxDspBaseErrorType jvxAdaptiveResampler_variantII_initialize(jvxHandle** H, jvxData* delay_samples, int lagrange_toLeft, int lagrange_toRight, int lFilter, int buffersizeInMax, int oversamplingFactor, jvxDataFormat format);
jvxDspBaseErrorType jvxAdaptiveResampler_variantII_terminate(jvxHandle* H);
jvxDspBaseErrorType jvxAdaptiveResampler_variantII_process(void* in, int bSizeIn, void* out, int bSizeOut, jvxHandle* H);

jvxDspBaseErrorType jvxAdaptiveResampler_variantIII_initialize(jvxHandle** H, jvxData* delay_samples, int lagrange_toLeft, int lagrange_toRight, int lFilter, int buffersizeInMax, int oversamplingFactor, jvxDataFormat format);
jvxDspBaseErrorType jvxAdaptiveResampler_variantIII_terminate(jvxHandle* H);
jvxDspBaseErrorType jvxAdaptiveResampler_variantIII_process(void* in, int bSizeIn, void* out, int bSizeOut, jvxHandle* H);

#endif
