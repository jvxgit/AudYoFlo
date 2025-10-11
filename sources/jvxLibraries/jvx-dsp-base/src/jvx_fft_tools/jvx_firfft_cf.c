/*															  		*
 *  ***********************************************************		*
 *  ***********************************************************		*
 *  																*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Copyright (C) Javox Solutions GmbH - All Rights Reserved		*
 *  Unauthorized copying of this file, via any medium is			*
 *  strictly prohibited. 											*
 *  																*
 *  Proprietary and confidential									*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Written by Hauke Krüger <hk@javox-solutions.com>, 2012-2020		*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Javox Solutions GmbH, Gallierstr. 33, 52074 Aachen				*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Contact: info@javox-solutions.com, www.javox-solutions.com		*
 *  																*
 *  ********************************************************   		*/

#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_firfft.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_dsp_base.h"
#include "jvx_allocators/jvx_allocators.h"

#include "jvx_fft_tools/jvx_firfft_prv.h"

/*
 * This function realizes an fft domain based time domain filter. It provides 2 
 * FFTs and 2 IIFTs:
 * - fft 1: transform the input signal into the frequency domain
 * - fft 2: Transform the new filter coefficients into the frequency domain to run with new weights
 * - ifft 1: Transform to time domain, old weights
 * - ifft 2: Transform to time domain, new weights
 * 
 * NORMAL MODE: jvx_firfft_cf_process
 * 
 * in-signal ---> fft ---> x weights ---> ifft  ---> out td
 * 
 * FILTER SWITCH MODE: jvx_firfft_cf_process_update_weights
 * 
 * in-signal ---> fft ---> x old weights ---> ifft  |
 *					   |							| crossfade ---> out td
 *                     |-> x new weights ---> ifft	|
 *								^
 *								|
 * new weights td  -------------
 * 
 */
#include "jvx_firfft_cf_prv.h"

void 
jvx_firfft_cf_compute_weights(jvx_firfft* hdl, jvxData* fir, jvxSize lFir)
{
	jvxSize i;
	jvx_firfft_cf_prv* nHdl = (jvx_firfft_cf_prv*)hdl->prv;
	assert(lFir == nHdl->firfft.init_cpy.lFir);

	if (nHdl->firfft.init_cpy.type == JVX_FIRFFT_SYMMETRIC_FIR)
	{
		// Copy such that we get a zero phase filter
		nHdl->ram_cf.in_weights[0] = nHdl->firfft.init_cpy.fir[nHdl->firfft.ram.outoffset];
		for (i = 0; i < nHdl->firfft.ram.outoffset; i++)
		{
			nHdl->ram_cf.in_weights[nHdl->firfft.derived_cpy.szFftValue - i - 1] = nHdl->firfft.init_cpy.fir[nHdl->firfft.ram.outoffset - i - 1];
			nHdl->ram_cf.in_weights[i + 1] = nHdl->firfft.init_cpy.fir[nHdl->firfft.ram.outoffset + i + 1];
		}
	}
	else
	{
		memcpy(nHdl->ram_cf.in_weights, nHdl->firfft.init_cpy.fir, sizeof(jvxData) * nHdl->firfft.init_cpy.lFir);
	}

	jvx_execute_fft(nHdl->ram_cf.corefft); // -> nHdl->ram_cf.spec[1]
}

void
jvx_firfft_cf_copy_weights(jvx_firfft* hdl, jvxDataCplx* firW, jvxSize lFirW)
{
	jvxSize i;
	jvx_firfft_cf_prv* nHdl = (jvx_firfft_cf_prv*)hdl->prv;
	assert(lFirW == nHdl->firfft.derived_cpy.lFirW);
	for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
	{
		firW[i].re = nHdl->ram_cf.spec_ifft_in[1][i].re;
		firW[i].im = nHdl->ram_cf.spec_ifft_in[1][i].im;
	}
}

jvxDspBaseErrorType jvx_firfft_cf_init(jvx_firfft* hdl, jvxHandle* fftCfgHdl)
{
	if (hdl->prv == NULL)
	{
		jvx_firfft_cf_prv* nHdl = NULL;
		jvxSize N = 0, i = 0;
		jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;

		assert(jvx_allocator != NULL);

		nHdl = (jvx_firfft_cf_prv*)jvx_allocator->alloc(sizeof(jvx_firfft_cf_prv), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);
		
		hdl->prv = nHdl;

		jvx_firfft_update(hdl, JVX_DSP_UPDATE_INIT, true); // this sets all derived values

		jvx_allocator->purpose = JVX_ALLOCATOR_OBJECT_PURPOSE_TIME_CRITICAL;

		resL = jvx_create_fft_ifft_global(&nHdl->firfft.ram.fftGlob, nHdl->firfft.derived_cpy.szFft, fftCfgHdl);
		assert(resL == JVX_DSP_NO_ERROR);

		// Check if current fft implementation requires normalization
		nHdl->firfft.ram.normOut = jvx_fft_requires_normalization(nHdl->firfft.ram.fftGlob);

		// Let us allocate input and output - 2 iffts here
		resL = jvx_create_ifft_complex_2_real(&nHdl->ram_cf.coreifft[0],
			nHdl->firfft.ram.fftGlob, nHdl->firfft.derived_cpy.szFft,
			&nHdl->ram_cf.spec_ifft_in[0], &nHdl->ram_cf.ifft_out[0], &N,
			JVX_FFT_IFFT_PRESERVE_INPUT,
			NULL, NULL, 0);
		assert(resL == JVX_DSP_NO_ERROR);

		// Backward compatibility
		nHdl->firfft.ram.coreifft = nHdl->ram_cf.coreifft[0];
		nHdl->firfft.ram.spec = nHdl->ram_cf.spec_ifft_in[0];
		nHdl->firfft.ram.out = nHdl->ram_cf.ifft_out[0];

		// Secondary ifft
		resL = jvx_create_ifft_complex_2_real(&nHdl->ram_cf.coreifft[1],
			nHdl->firfft.ram.fftGlob, nHdl->firfft.derived_cpy.szFft,
			&nHdl->ram_cf.spec_ifft_in[1], &nHdl->ram_cf.ifft_out[1], &N,
			JVX_FFT_IFFT_PRESERVE_INPUT,
			NULL, NULL, 0);
		assert(resL == JVX_DSP_NO_ERROR);

		//assert(N == nHdl->derived_cpy.szFftValue);

		// Reuse input from ifft[0] for fft output, first realization
		resL = jvx_create_fft_real_2_complex(&nHdl->firfft.ram.corefft,
			nHdl->firfft.ram.fftGlob, nHdl->firfft.derived_cpy.szFft,
			&nHdl->firfft.ram.in,
			NULL, &N, JVX_FFT_IFFT_PRESERVE_INPUT,
			NULL, nHdl->ram_cf.spec_ifft_in[0], 0);
		assert(resL == JVX_DSP_NO_ERROR);
		//assert(N == nHdl->derived_cpy.szFftValue);

		// Add another fft here for update of the filter weights. The output is always spec[1] as this is overridden anyway
		resL = jvx_create_fft_real_2_complex(&nHdl->ram_cf.corefft,
			nHdl->firfft.ram.fftGlob, nHdl->firfft.derived_cpy.szFft,
			&nHdl->ram_cf.in_weights,
			NULL, &N, JVX_FFT_IFFT_PRESERVE_INPUT,
			NULL, nHdl->ram_cf.spec_ifft_in[1], 0);
		assert(resL == JVX_DSP_NO_ERROR);
		//assert(N == nHdl->derived_cpy.szFftValue);

		if (nHdl->firfft.init_cpy.type == JVX_FIRFFT_SYMMETRIC_FIR)
		{
			assert((nHdl->firfft.init_cpy.lFir % 2) == 1); // Make sure it is symmetric
			nHdl->firfft.ram.outoffset = nHdl->firfft.derived_cpy.delay;
		}
		else
		{
			nHdl->firfft.ram.outoffset = 0;
		}

		jvx_firfft_cf_compute_weights(hdl, nHdl->firfft.init_cpy.fir, nHdl->firfft.init_cpy.lFir);

		hdl->sync.firW = NULL;
		hdl->sync.firW = jvx_allocator->alloc(sizeof(jvxDataCplx), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), nHdl->firfft.derived_cpy.lFirW);
		
		jvx_firfft_cf_copy_weights(hdl, hdl->sync.firW, nHdl->firfft.derived_cpy.lFirW);

		nHdl->firfft.ram.phase = 0;
		nHdl->firfft.ram.normFactor = 1.0 / (jvxData)nHdl->firfft.derived_cpy.szFftValue;
		nHdl->ram_cf.cf_inc = 1.0 / hdl->init.bsize;

		jvx_allocator->purpose = JVX_ALLOCATOR_OBJECT_PURPOSE_UNSPECIFIC;
		

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;	
}

jvxDspBaseErrorType jvx_firfft_cf_terminate(jvx_firfft* hdl)
{
	jvxErrorType res = JVX_DSP_ERROR_WRONG_STATE;
	if (hdl->prv)
	{
		res = JVX_DSP_ERROR_WRONG_STATE;
		jvx_firfft_cf_prv* nHdl = (jvx_firfft_cf_prv*)hdl->prv;

		if (nHdl->ram_cf.corefft)
		{
			jvx_destroy_fft(nHdl->ram_cf.corefft);
			nHdl->ram_cf.corefft = NULL;
			nHdl->ram_cf.in_weights = NULL;

			jvx_destroy_fft(nHdl->firfft.ram.corefft);
			nHdl->firfft.ram.corefft = NULL;

			jvx_destroy_ifft(nHdl->ram_cf.coreifft[0]);
			nHdl->ram_cf.coreifft[0] = NULL;
			nHdl->ram_cf.spec_ifft_in[0] = NULL;
			nHdl->ram_cf.ifft_out[0] = NULL;

			jvx_destroy_ifft(nHdl->ram_cf.coreifft[1]);
			nHdl->ram_cf.coreifft[1] = NULL;
			nHdl->ram_cf.spec_ifft_in[1] = NULL;
			nHdl->ram_cf.ifft_out[1] = NULL;

			jvx_destroy_fft_ifft_global(nHdl->firfft.ram.fftGlob);
			nHdl->firfft.ram.fftGlob = NULL;

			jvx_allocator->dealloc((jvxHandle**)&hdl->sync.firW, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
			
			hdl->sync.firW = NULL;

			nHdl->firfft.ram.coreifft = NULL;
			nHdl->firfft.ram.spec = NULL;
			nHdl->firfft.ram.out = NULL;

			jvx_allocator->dealloc((jvxHandle**)&nHdl, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));			

			hdl->prv = NULL;
			
			res = JVX_DSP_NO_ERROR;
		}
		if (res == JVX_NO_ERROR)
		{
			jvx_firfft_resetDerived(hdl);
		}
	}
	return res;
}

jvxDspBaseErrorType jvx_firfft_cf_process(jvx_firfft* hdl, jvxData* inArg, jvxData* outArg, jvxCBool addOnOut)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize ll1 = 0;
	jvxSize ll2 = 0;
	jvxData* ptrIn = NULL;
	jvxData* ptrOut = NULL;
	jvxSize outphase = 0;

	jvxData* in = inArg;
	jvxData* out = outArg;
	jvxData accu;

	jvxDataCplx* spec_out = NULL;
	jvxDataCplx* firW = NULL;
	jvxIFFT* coreifft = NULL;
	jvxData* out_src = NULL;

	if (hdl->prv)
	{
		jvx_firfft_cf_prv* nHdl = (jvx_firfft_cf_prv*)hdl->prv;

		ll1 = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.phase, nHdl->firfft.init_cpy.bsize);
		ll2 = nHdl->firfft.init_cpy.bsize - ll1;

		ptrIn = nHdl->firfft.ram.in;
		ptrIn += nHdl->firfft.ram.phase;

		for (i = 0; i < ll1; i++)
		{
			*ptrIn++ = *in++;
		}
		ptrIn = nHdl->firfft.ram.in;
		for (i = 0; i < ll2; i++)
		{
			*ptrIn++ = *in++;
		}

#if 0
		/* Add values here to check fundamental functionality of fft */
		ptrIn = nHdl->firfft.ram.in;
		for(i = 0; i < nHdl->firfft.derived_cpy.szFftValue; i++)
		{
			ptrIn[i] = 0;
		}
		ptrIn[1] = 1.0;
		ptrIn[2] = 1.0;
		/**/
#endif

		jvx_execute_fft(nHdl->firfft.ram.corefft);

		spec_out = nHdl->ram_cf.spec_ifft_in[0];
		firW = hdl->sync.firW;
		coreifft = nHdl->ram_cf.coreifft[0];
		out_src = nHdl->ram_cf.ifft_out[0];

		ptrOut = out_src;
		outphase = (nHdl->firfft.ram.phase + nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.outoffset) %
			nHdl->firfft.derived_cpy.szFftValue;
		ptrOut += outphase;

		// Process 
		switch (nHdl->firfft.init_cpy.type)
		{
		case JVX_FIRFFT_SYMMETRIC_FIR:
			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				spec_out[i].re *= firW[i].re;
				spec_out[i].im *= firW[i].re;
			}
			ll1 = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - outphase, nHdl->firfft.init_cpy.bsize);
			ll2 = nHdl->firfft.init_cpy.bsize - ll1;

			break;
		default:

			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				// Be careful, this MIGHT be an inplace operation!!
				accu = nHdl->firfft.ram.spec[i].re * firW[i].re - nHdl->firfft.ram.spec[i].im * firW[i].im;
				spec_out[i].im = nHdl->firfft.ram.spec[i].re * firW[i].im + nHdl->firfft.ram.spec[i].im * firW[i].re;
				spec_out[i].re = accu;
			}

			break;
		}

		jvx_execute_ifft(coreifft);

		if (nHdl->firfft.ram.normOut)
		{
			if (addOnOut)
			{
				for (i = 0; i < ll1; i++)
				{
					*out++ += *ptrOut++ * nHdl->firfft.ram.normFactor;
				}

				ptrOut = out_src;
				for (i = 0; i < ll2; i++)
				{
					*out++ += *ptrOut++ * nHdl->firfft.ram.normFactor;
				}

			}
			else
			{
				for (i = 0; i < ll1; i++)
				{
					*out++ = *ptrOut++ * nHdl->firfft.ram.normFactor;
				}

				ptrOut = out_src;
				for (i = 0; i < ll2; i++)
				{
					*out++ = *ptrOut++ * nHdl->firfft.ram.normFactor;
				}
			}
		}
		else
		{
			if (addOnOut)
			{
				for (i = 0; i < ll1; i++)
				{
					*out++ += *ptrOut++ ;
				}

				ptrOut = out_src;
				for (i = 0; i < ll2; i++)
				{
					*out++ += *ptrOut++;
				}

			}
			else
			{
				memcpy(out, ptrOut, ll1 * sizeof(jvxData));
				/*
				for (i = 0; i < ll1; i++)
				{
					*out++ = *ptrOut++;
				}
				*/

				out += ll1;
				memcpy(out, out_src, sizeof(jvxData)* ll2);
				/*
				ptrOut = out_src;
				for (i = 0; i < ll2; i++)
				{
					*out++ = *ptrOut++;
				}
				*/
			}
		}

		nHdl->firfft.ram.phase = (nHdl->firfft.ram.phase + nHdl->firfft.init_cpy.bsize) % nHdl->firfft.derived_cpy.szFftValue;

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_firfft_cf_process_update_weights(jvx_firfft* hdl, jvxData* inArg, jvxData* outArg, 
	jvxDataCplx* newWeights, jvxCBool addOnOut)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize ll1 = 0;
	jvxSize ll2 = 0;
	jvxData* ptrIn = NULL;
	jvxData* ptrOut_old = NULL;
	jvxData* ptrOut_new = NULL;
	jvxSize outphase = 0;

	jvxData* in = inArg;
	jvxData* out = outArg;
	jvxData accu;

	jvxDataCplx* spec_out_old = NULL;
	jvxDataCplx* spec_out_new = NULL;

	jvxDataCplx* firW_old = NULL;
	jvxDataCplx* firW_new = newWeights;

	jvxIFFT* coreifft_old = NULL;
	jvxIFFT* coreifft_new = NULL;

	jvxData* out_src_old = NULL;
	jvxData* out_src_new = NULL;

	jvxSize idxNew = 0;
	jvxData weightCf_old = 0;
	jvxData weightCf_new = 0;

	if (hdl->prv)
	{
		jvx_firfft_cf_prv* nHdl = (jvx_firfft_cf_prv*)hdl->prv;

		ll1 = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.phase, nHdl->firfft.init_cpy.bsize);
		ll2 = nHdl->firfft.init_cpy.bsize - ll1;

		ptrIn = nHdl->firfft.ram.in;
		ptrIn += nHdl->firfft.ram.phase;

		for (i = 0; i < ll1; i++)
		{
			*ptrIn++ = *in++;
		}
		ptrIn = nHdl->firfft.ram.in;
		for (i = 0; i < ll2; i++)
		{
			*ptrIn++ = *in++;
		}

		jvx_execute_fft(nHdl->firfft.ram.corefft);

		spec_out_old = nHdl->ram_cf.spec_ifft_in[0];
		firW_old = hdl->sync.firW;
		coreifft_old = nHdl->ram_cf.coreifft[0];
		out_src_old= nHdl->ram_cf.ifft_out[0];

		spec_out_new = nHdl->ram_cf.spec_ifft_in[1];
		firW_new = newWeights;
		coreifft_new = nHdl->ram_cf.coreifft[1];
		out_src_new = nHdl->ram_cf.ifft_out[1];
		
		ptrOut_old = out_src_old;
		ptrOut_new = out_src_new;

		outphase = (nHdl->firfft.ram.phase + nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.outoffset) %
			nHdl->firfft.derived_cpy.szFftValue;
		ptrOut_old += outphase;
		ptrOut_new += outphase;

		// Process 
		switch (nHdl->firfft.init_cpy.type)
		{
		case JVX_FIRFFT_SYMMETRIC_FIR:
			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				spec_out_old[i].re *= firW_old[i].re;
				spec_out_new[i].re *= firW_new[i].re;
				spec_out_old[i].im *= firW_old[i].re;
				spec_out_new[i].im *= firW_new[i].re;
				
				// Take over the new weights
				firW_old[i].re = firW_new[i].re;
				firW_old[i].im = firW_new[i].im;
			}
			ll1 = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - outphase, nHdl->firfft.init_cpy.bsize);
			ll2 = nHdl->firfft.init_cpy.bsize - ll1;

			break;
		default:

			// Old is inplace!!
			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				spec_out_new[i].im = nHdl->firfft.ram.spec[i].re * firW_new[i].im + nHdl->firfft.ram.spec[i].im * firW_new[i].re;
				spec_out_new[i].re = nHdl->firfft.ram.spec[i].re * firW_new[i].re - nHdl->firfft.ram.spec[i].im * firW_new[i].im;

				accu = nHdl->firfft.ram.spec[i].re * firW_old[i].re - nHdl->firfft.ram.spec[i].im * firW_old[i].im;
				spec_out_old[i].im = nHdl->firfft.ram.spec[i].re * firW_old[i].im + nHdl->firfft.ram.spec[i].im * firW_old[i].re;
				spec_out_old[i].re = accu;

				// Take over the new weights
				firW_old[i].re = firW_new[i].re;
				firW_old[i].im = firW_new[i].im;
			}
			break;
		}

		jvx_execute_ifft(coreifft_old);
		jvx_execute_ifft(coreifft_new);

		weightCf_old = 1.0;
		weightCf_new = 0.0;

		if (addOnOut)
		{
			for (i = 0; i < ll1; i++)
			{
				accu = *ptrOut_old++ * weightCf_old + *ptrOut_new++ * weightCf_new;
				accu *= nHdl->firfft.ram.normFactor;
				*out += accu;

				out++;
				weightCf_old -= nHdl->ram_cf.cf_inc;
				weightCf_new += nHdl->ram_cf.cf_inc;
			}

			ptrOut_old = out_src_old;
			ptrOut_new = out_src_new;
			for (i = 0; i < ll2; i++)
			{
				accu = *ptrOut_old++ * weightCf_old + *ptrOut_new++ * weightCf_new;
				accu *= nHdl->firfft.ram.normFactor;
				*out += accu;

				out++;
				weightCf_old -= nHdl->ram_cf.cf_inc;
				weightCf_new += nHdl->ram_cf.cf_inc;
			}

		}
		else
		{
			for (i = 0; i < ll1; i++)
			{
				*out = *ptrOut_old++ * weightCf_old + *ptrOut_new++ * weightCf_new;
				*out *= nHdl->firfft.ram.normFactor;

				out++;
				weightCf_old -= nHdl->ram_cf.cf_inc;
				weightCf_new += nHdl->ram_cf.cf_inc;
			}

			ptrOut_old = out_src_old;
			ptrOut_new = out_src_new;
			for (i = 0; i < ll2; i++)
			{
				*out = *ptrOut_old++ * weightCf_old + *ptrOut_new++ * weightCf_new;
				*out *= nHdl->firfft.ram.normFactor;

				out++;
				weightCf_old -= nHdl->ram_cf.cf_inc;
				weightCf_new += nHdl->ram_cf.cf_inc;
			}
		}
		nHdl->firfft.ram.phase = (nHdl->firfft.ram.phase + nHdl->firfft.init_cpy.bsize) % nHdl->firfft.derived_cpy.szFftValue;

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}
/*

jvxDspBaseErrorType jvx_firfft_process_mix_place(jvx_firfft* hdl, jvxData* in, jvxData* out, jvxCBool isFirst)
{
	return jvx_firfft_process_mix(hdl, in, out, isFirst, in);
}

jvxDspBaseErrorType jvx_firfft_process_mix(jvx_firfft* hdl, jvxData* in, jvxData* out, jvxCBool isFirst, jvxData* workbuf)
{
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize i;
	jvxData* inPtr = workbuf;
	jvxData* outPtr = out;
	if (hdl->prv)
	{
		jvx_firfft_prv* nHdl = (jvx_firfft_prv*)hdl->prv; 
		if (isFirst)
		{
			memset(out, 0, sizeof(jvxData) * nHdl->init_cpy.bsize);
		}
		jvx_firfft_process(hdl, in, workbuf);

		for (i = 0; i < nHdl->init_cpy.bsize; i++)
		{
			*outPtr += *inPtr;
			outPtr++;
			inPtr++;
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_firfft_process_iplace(jvx_firfft* hdl, jvxData* inout)
{
	return jvx_firfft_process(hdl, inout, inout);
}

jvxDspBaseErrorType jvx_firfft_process(jvx_firfft* hdl, jvxData* inArg, jvxData* outArg)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize ll1 = 0;
	jvxSize ll2 = 0;
	jvxData* ptrIn = NULL;
	jvxData* ptrOut = NULL;
	jvxSize outphase = 0;

	jvxData* in = inArg;
	jvxData* out = outArg;
	jvxData accu;

	if (hdl->prv)
	{
		jvx_firfft_prv* nHdl = (jvx_firfft_prv*)hdl->prv;

		ll1 = JVX_MIN(nHdl->derived_cpy.szFftValue - nHdl->ram.phase, nHdl->init_cpy.bsize);
		ll2 = nHdl->init_cpy.bsize - ll1;

		ptrIn = nHdl->ram.in;
		ptrIn += nHdl->ram.phase;

		for (i = 0; i < ll1; i++)
		{
			*ptrIn++ = *in++;
		}
		ptrIn = nHdl->ram.in;
		for (i = 0; i < ll2; i++)
		{
			*ptrIn++ = *in++;
		}

		jvx_execute_fft(nHdl->ram.corefft);

		ptrOut = nHdl->ram.out;
		outphase = (nHdl->ram.phase + nHdl->derived_cpy.szFftValue - nHdl->ram.outoffset) %
			nHdl->derived_cpy.szFftValue;
		ptrOut += outphase;

		// Process 
		switch (nHdl->init_cpy.type)
		{
		case JVX_FIRFFT_SYMMETRIC_FIR:
			for (i = 0; i < nHdl->derived_cpy.szFftValue / 2 + 1; i++)
			{
				nHdl->ram.spec[i].re *= hdl->sync.firW[i].re;
				nHdl->ram.spec[i].im *= hdl->sync.firW[i].re;
			}
			jvx_execute_ifft(nHdl->ram.coreifft);

			ll1 = JVX_MIN(nHdl->derived_cpy.szFftValue - outphase, nHdl->init_cpy.bsize);
			ll2 = nHdl->init_cpy.bsize - ll1;

			break;
		default:

			for (i = 0; i < nHdl->derived_cpy.szFftValue / 2 + 1; i++)
			{
				accu = nHdl->ram.spec[i].re * hdl->sync.firW[i].re - nHdl->ram.spec[i].im * hdl->sync.firW[i].im;
				nHdl->ram.spec[i].im = nHdl->ram.spec[i].re * hdl->sync.firW[i].im + nHdl->ram.spec[i].im * hdl->sync.firW[i].re;
				nHdl->ram.spec[i].re = accu;
			}
			jvx_execute_ifft(nHdl->ram.coreifft);

			break;
		}

		for (i = 0; i < ll1; i++)
		{
			*out++ = *ptrOut++ * nHdl->ram.normFactor;
		}
		ptrOut = nHdl->ram.out;
		for (i = 0; i < ll2; i++)
		{
			*out++ = *ptrOut++ * nHdl->ram.normFactor;
		}

		nHdl->ram.phase = (nHdl->ram.phase + nHdl->init_cpy.bsize) % nHdl->derived_cpy.szFftValue;

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}
*/

