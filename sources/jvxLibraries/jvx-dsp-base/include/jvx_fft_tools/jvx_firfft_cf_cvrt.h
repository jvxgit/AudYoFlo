#ifndef __JVX_FIRFFT_FLTER_CF_CVRT_H__
#define __JVX_FIRFFT_FLTER_CF_CVRT_H__

#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_tools/jvx_firfft.h"

typedef struct
{
	jvx_firfft_prmInit init;
	jvx_firfft_prmDerived derived;
	jvxHandle* prv;
} jvx_firfft_cvrt;


JVX_DSP_LIB_BEGIN

jvxDspBaseErrorType jvx_firfft_cf_cvrt_init(jvx_firfft* hdl, jvxHandle* fftCfgHdl, jvxData** fir, jvxHandle** cvrtHdl);
jvxDspBaseErrorType jvx_firfft_cf_cvrt_terminate(jvx_firfft* hdl, jvxHandle** cvrtHdl);
void jvx_firfft_cf_cvrt_compute_weights_and_copy(jvxHandle* cvrtHdl, jvxData* firIn, jvxSize lFir, jvxDataCplx* firW, jvxSize lFirW, jvxCBool resetBuffer);

JVX_DSP_LIB_END

#endif