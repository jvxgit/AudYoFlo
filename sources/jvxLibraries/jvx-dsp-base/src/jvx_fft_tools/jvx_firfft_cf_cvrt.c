#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_firfft.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_allocators/jvx_allocators.h"

#include "jvx_fft_tools/jvx_firfft_cf_nout.h"
#include "jvx_fft_tools/jvx_firfft_cf_cvrt.h"

#include "jvx_fft_tools/jvx_firfft_prv.h"


typedef struct
{
	struct
	{
		jvxFFT* corefft;
		jvxData* in_weights;
		jvxDataCplx* out_spec;
		jvxFFTGlobal* fftGlob;
		jvxCBool normOut;
		jvxSize outoffset;
		jvxData lFirFull;
	} ram;

	jvx_firfft_prmInit init_cpy;
	jvx_firfft_prmDerived derived_cpy;
} jvx_firfft_cvrt_prv;

jvxDspBaseErrorType jvx_firfft_cf_cvrt_init(jvx_firfft* hdl, jvxHandle* fftCfgHdl, jvxData** fir, jvxHandle** cvrtHdl)
{
	jvx_firfft_cvrt_prv* nHdl = NULL;
	jvxSize N = 0, i = 0;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;

	assert(jvx_allocator != NULL);

	nHdl = (jvx_firfft_cvrt_prv*)jvx_allocator->alloc(sizeof(jvx_firfft_cvrt_prv), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);
	assert(cvrtHdl);
	*cvrtHdl = nHdl;

	nHdl->init_cpy = hdl->init;
	nHdl->derived_cpy = hdl->derived;

	jvx_allocator->purpose = JVX_ALLOCATOR_OBJECT_PURPOSE_TIME_CRITICAL;

	resL = jvx_create_fft_ifft_global(&nHdl->ram.fftGlob, nHdl->derived_cpy.szFft, fftCfgHdl);
	assert(resL == JVX_DSP_NO_ERROR);

	// Check if current fft implementation requires normalization
	nHdl->ram.normOut = jvx_fft_requires_normalization(nHdl->ram.fftGlob);

	// Add another fft here for update of the filter weights. The output is always spec[1] as this is overridden anyway
	resL = jvx_create_fft_real_2_complex(&nHdl->ram.corefft,
		nHdl->ram.fftGlob, nHdl->derived_cpy.szFft,
		&nHdl->ram.in_weights, &nHdl->ram.out_spec,
		&N, JVX_FFT_IFFT_PRESERVE_INPUT,
		NULL, NULL, 0);

	nHdl->ram.lFirFull = N;

	assert(resL == JVX_DSP_NO_ERROR);
	//assert(N == nHdl->derived_cpy.szFftValue);

	if (nHdl->init_cpy.type == JVX_FIRFFT_SYMMETRIC_FIR)
	{
		assert((nHdl->init_cpy.lFir % 2) == 1); // Make sure it is symmetric		
		nHdl->ram.outoffset = nHdl->derived_cpy.delay;
	}
	else
	{
		nHdl->ram.outoffset = 0;
	}

	jvx_allocator->purpose = JVX_ALLOCATOR_OBJECT_PURPOSE_UNSPECIFIC;
	assert(hdl->sync.ext);

	// Fill buffers with init ir
	jvx_firfft_cf_nout_prmSync* nChans = (jvx_firfft_cf_nout_prmSync*)hdl->sync.ext;
	for (i = 0; i < nChans->N; i++)
	{
		jvx_firfft_cf_cvrt_compute_weights_and_copy(nHdl, fir[i], nHdl->init_cpy.lFir, nChans->firWN[i], nHdl->derived_cpy.lFirW, false);
	}

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_firfft_cf_cvrt_terminate(jvx_firfft* hdl, jvxHandle** cvrtHdl)
{
	jvxErrorType res = JVX_DSP_ERROR_WRONG_STATE;
	jvx_firfft_cvrt_prv* nHdl = (jvx_firfft_cvrt_prv*)*cvrtHdl;
	*cvrtHdl = NULL;

	if (nHdl->ram.corefft)
	{
		jvx_destroy_fft(nHdl->ram.corefft);
		nHdl->ram.corefft = NULL;
		nHdl->ram.in_weights = NULL;
		nHdl->ram.out_spec = NULL;

		jvx_destroy_fft_ifft_global(nHdl->ram.fftGlob);
		nHdl->ram.fftGlob = NULL;

		jvx_allocator->dealloc((jvxHandle**)&nHdl, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));

		res = JVX_DSP_NO_ERROR;
	}

	return res;
}

void 
jvx_firfft_cf_cvrt_compute_weights_and_copy(jvxHandle* cvrtHdl, jvxData* firIn, jvxSize lFir, jvxDataCplx* firW, jvxSize lFirW, jvxCBool resetFirBuf)
{
	jvxSize i;
	jvx_firfft_cvrt_prv* nHdl = (jvx_firfft_cvrt_prv*)cvrtHdl;
	assert(lFir == nHdl->init_cpy.lFir);

	if (resetFirBuf)
	{
		memset(nHdl->ram.in_weights, 0, sizeof(jvxData) * nHdl->ram.lFirFull);
	}

	if (nHdl->init_cpy.type == JVX_FIRFFT_SYMMETRIC_FIR)
	{
		// Copy such that we get a zero phase filter
		nHdl->ram.in_weights[0] = firIn[nHdl->ram.outoffset];
		for (i = 0; i < nHdl->ram.outoffset; i++)
		{
			nHdl->ram.in_weights[nHdl->derived_cpy.szFftValue - i - 1] = firIn[nHdl->ram.outoffset - i - 1];
			nHdl->ram.in_weights[i + 1] = firIn[nHdl->ram.outoffset + i + 1];
		}
	}
	else
	{
		memcpy(nHdl->ram.in_weights, firIn, sizeof(jvxData) * lFir);
	}

	jvx_execute_fft(nHdl->ram.corefft); // -> nHdl->ram.out_spec

	assert(lFirW == nHdl->derived_cpy.lFirW);
	for (i = 0; i < nHdl->derived_cpy.szFftValue / 2 + 1; i++)
	{
		firW[i].re = nHdl->ram.out_spec[i].re;
		firW[i].im = nHdl->ram.out_spec[i].im;
	}
}
