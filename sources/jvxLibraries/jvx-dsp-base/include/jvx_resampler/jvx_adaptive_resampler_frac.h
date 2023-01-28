#ifndef __JVX_ADAPTIVE_RESAMPLER_FRAC_H__
#define __JVX_ADAPTIVE_RESAMPLER_FRAC_H__

#include "jvx_dsp_base.h"

bool jvxAdaptiveResampler_frac_variantI_initialize(jvxHandle** H, jvxData* delay_samples, int lagrange_toLeft, int lagrange_toRight, int lFilter, int buffersizeInMax, int oversamplingFactor, jvxDataFormat format);

bool jvxAdaptiveResampler_frac_variantI_terminate(jvxHandle* H);

bool jvxAdaptiveResampler_frac_variantI_process(void* in, int bSizeIn, void* out, int* bSizeOut, jvxData desiredBSizeOut_frac, jvxHandle* H);


bool jvxAdaptiveResampler_frac_variantII_initialize(jvxHandle** H, jvxData* delay_samples, int lagrange_toLeft, int lagrange_toRight, int lFilter, int buffersizeInMax, int oversamplingFactor, jvxDataFormat format);

bool jvxAdaptiveResampler_frac_variantII_terminate(jvxHandle* H);

bool jvxAdaptiveResampler_frac_variantII_process(void* in, int bSizeIn, void* out, int* bSizeInOut, jvxData desiredBSizeOut_frac, jvxHandle* H);


bool jvxAdaptiveResampler_frac_variantIII_initialize(jvxHandle** H, jvxData* delay_samples, int lagrange_toLeft, int lagrange_toRight, int lFilter, int buffersizeInMax, int oversamplingFactor, jvxDataFormat format);

bool jvxAdaptiveResampler_frac_variantIII_terminate(jvxHandle* H);

bool jvxAdaptiveResampler_frac_variantIII_process(void* in, int bSizeIn, void* out, int* bSizeInOut, jvxData desiredBSizeOut_frac, jvxHandle* H);

#endif