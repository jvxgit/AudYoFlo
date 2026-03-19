#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_firfft.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_dsp_base.h"
#include "jvx_allocators/jvx_allocators.h"

#include "jvx_fft_tools/jvx_firfft_prv.h"

#include "jvx_fft_tools/jvx_firfft_cf_nout.h"

typedef struct
{
	// This struct contais all variables required for a "simple" firfft: one fft, one ifft
	jvx_firfft_prv firfft;

	struct
	{
		// Corresponding spectrum input buffers, should be the result from different weights
		jvxDataCplx* spec_ifft_in;	

		struct
		{
			jvxData** in_bufs;
			jvxFFT** in_ffts;
			jvxSize nChannelsIn;
		} multiInChannels;

		// Cross fade increment value
		jvxData cf_inc;
	} ram_cf_nout;

	jvxCBool allocatedViaAllocator;
} jvx_firfft_cf_nout_prv;

jvxDspBaseErrorType 
jvx_firfft_cf_nout_init(jvx_firfft* hdl, jvxHandle* fftCfgHdl, jvxSize nStride, jvxSize nChannelsIn)
{
	if (hdl->prv == NULL)
	{
		jvx_firfft_cf_nout_prv* nHdl = NULL;
		jvxSize N = 0, i = 0;
		jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;

		assert(jvx_allocator != NULL);

		nHdl = (jvx_firfft_cf_nout_prv*)jvx_allocator->alloc(sizeof(jvx_firfft_cf_nout_prv), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);
		memset(nHdl, 0, sizeof(jvx_firfft_cf_nout_prv));

		nHdl->firfft.tp = JVX_FIRFFT_PRV_TYPE_CF_NOUT;

		hdl->prv = nHdl;

		jvx_firfft_update(hdl, JVX_DSP_UPDATE_INIT, true); // this sets all derived values

		jvx_allocator->purpose = JVX_ALLOCATOR_OBJECT_PURPOSE_TIME_CRITICAL;

		resL = jvx_create_fft_ifft_global(&nHdl->firfft.ram.fftGlob, nHdl->firfft.derived_cpy.szFft, fftCfgHdl);
		assert(resL == JVX_DSP_NO_ERROR);

		// Check if current fft implementation requires normalization
		nHdl->firfft.ram.normOut = jvx_fft_requires_normalization(nHdl->firfft.ram.fftGlob);

		// Backward compatibility
		
		// Secondary ifft
		resL = jvx_create_ifft_complex_2_real(&nHdl->firfft.ram.coreifft,
			nHdl->firfft.ram.fftGlob, nHdl->firfft.derived_cpy.szFft,
			&nHdl->ram_cf_nout.spec_ifft_in, &nHdl->firfft.ram.out, &N,
			JVX_FFT_IFFT_PRESERVE_INPUT,
			NULL, NULL, 0);
		assert(resL == JVX_DSP_NO_ERROR);		

		//assert(N == nHdl->derived_cpy.szFftValue);

		// Reuse input from ifft[0] for fft output, first realization
		resL = jvx_create_fft_real_2_complex(&nHdl->firfft.ram.corefft,
			nHdl->firfft.ram.fftGlob, nHdl->firfft.derived_cpy.szFft,
			&nHdl->firfft.ram.in, &nHdl->firfft.ram.spec,
			&N, JVX_FFT_IFFT_PRESERVE_INPUT,
			NULL, NULL , 0);

		assert(resL == JVX_DSP_NO_ERROR);
		//assert(N == nHdl->derived_cpy.szFftValue);

		nHdl->ram_cf_nout.multiInChannels.nChannelsIn = nChannelsIn;
		if (nHdl->ram_cf_nout.multiInChannels.nChannelsIn > 1)
		{
			nHdl->ram_cf_nout.multiInChannels.nChannelsIn = nChannelsIn;
			nHdl->ram_cf_nout.multiInChannels.in_bufs = (jvxData**)jvx_allocator->alloc(sizeof(jvxData*), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), nChannelsIn);
			nHdl->ram_cf_nout.multiInChannels.in_ffts = (jvxFFT**)jvx_allocator->alloc(sizeof(jvxFFT*), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), nChannelsIn);

			nHdl->ram_cf_nout.multiInChannels.in_bufs[0] = nHdl->firfft.ram.in;
			nHdl->ram_cf_nout.multiInChannels.in_ffts[0] = nHdl->firfft.ram.corefft;

			for (i = 1; i < nChannelsIn; i++)
			{
				resL = jvx_create_fft_real_2_complex(&nHdl->ram_cf_nout.multiInChannels.in_ffts[i],
					nHdl->firfft.ram.fftGlob, nHdl->firfft.derived_cpy.szFft,
					&nHdl->ram_cf_nout.multiInChannels.in_bufs[i], NULL,
					&N, JVX_FFT_IFFT_PRESERVE_INPUT,
					NULL, nHdl->firfft.ram.spec, 0);
			}
		}

		if (nHdl->firfft.init_cpy.type == JVX_FIRFFT_SYMMETRIC_FIR)
		{
			assert((nHdl->firfft.init_cpy.lFir % 2) == 1); // Make sure it is symmetric
			nHdl->firfft.ram.outoffset = nHdl->firfft.derived_cpy.delay;
		}
		else
		{
			nHdl->firfft.ram.outoffset = 0;
		}

		jvx_firfft_cf_nout_prmSync* nChans = jvx_allocator->alloc(sizeof(jvx_firfft_cf_nout_prmSync), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);
		nChans->stride = nStride;
		nChans->N = nStride * nChannelsIn;
		nChans->nW = nHdl->firfft.derived_cpy.lFirW;		

		nChans->firWN = jvx_allocator->alloc(sizeof(jvxDataCplx), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), nChans->N);
		for (i = 0; i < nChans->N; i++)
		{
			nChans->firWN[i] = jvx_allocator->alloc(sizeof(jvxDataCplx), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), nHdl->firfft.derived_cpy.lFirW);			
		}

		hdl->sync.firW = NULL;
		hdl->sync.ext = nChans;

		nHdl->firfft.ram.phase = 0;
		nHdl->firfft.ram.normFactor = 1.0 / (jvxData)nHdl->firfft.derived_cpy.szFftValue;
		nHdl->ram_cf_nout.cf_inc = 1.0 / hdl->init.bsize;

		jvx_allocator->purpose = JVX_ALLOCATOR_OBJECT_PURPOSE_UNSPECIFIC;


		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType 
jvx_firfft_cf_nout_terminate(jvx_firfft* hdl)
{
	jvxSize i;
	jvxErrorType res = JVX_DSP_ERROR_WRONG_STATE;
	if (hdl->prv)
	{
		res = JVX_DSP_ERROR_WRONG_STATE;
		jvx_firfft_cf_nout_prv* nHdl = (jvx_firfft_cf_nout_prv*)hdl->prv;
		assert(nHdl->firfft.tp == JVX_FIRFFT_PRV_TYPE_CF_NOUT);

		if (nHdl->firfft.ram.corefft)
		{
			if(nHdl->ram_cf_nout.multiInChannels.nChannelsIn > 1)
			{
				nHdl->ram_cf_nout.multiInChannels.in_bufs[0] = NULL;
				nHdl->ram_cf_nout.multiInChannels.in_ffts[0] = NULL;

				for (i = 1; i < nHdl->ram_cf_nout.multiInChannels.nChannelsIn; i++)
				{
					jvx_destroy_fft(nHdl->ram_cf_nout.multiInChannels.in_ffts[i]);
					nHdl->ram_cf_nout.multiInChannels.in_ffts[i] = NULL;
					nHdl->ram_cf_nout.multiInChannels.in_bufs[i] = NULL;
				}

				jvx_allocator->dealloc((jvxHandle**) &nHdl->ram_cf_nout.multiInChannels.in_bufs, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
				jvx_allocator->dealloc((jvxHandle**) &nHdl->ram_cf_nout.multiInChannels.in_ffts, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
				nHdl->ram_cf_nout.multiInChannels.nChannelsIn = 0;

			}
			jvx_destroy_fft(nHdl->firfft.ram.corefft);
			nHdl->firfft.ram.corefft = NULL;
			nHdl->firfft.ram.in = NULL;
			nHdl->firfft.ram.spec = NULL;

			jvx_destroy_ifft(nHdl->firfft.ram.coreifft);
			nHdl->firfft.ram.coreifft = NULL;
			nHdl->ram_cf_nout.spec_ifft_in = NULL;
			nHdl->firfft.ram.out = NULL;

			jvx_destroy_fft_ifft_global(nHdl->firfft.ram.fftGlob);
			nHdl->firfft.ram.fftGlob = NULL;

			jvx_firfft_cf_nout_prmSync* nChans = hdl->sync.ext;
			for (i = 0; i < nChans->N; i++)
			{
				jvx_allocator->dealloc((jvxHandle**)&nChans->firWN[i], (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
			}
			jvx_allocator->dealloc((jvxHandle**)&nChans->firWN, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
			jvx_allocator->dealloc((jvxHandle**)&nChans, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
			
			hdl->sync.ext = NULL;
			hdl->sync.firW = NULL;

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

jvxDspBaseErrorType jvx_firfft_cf_nout_process_core(jvx_firfft_cf_nout_prv* nHdl, jvx_firfft_cf_nout_prmSync* nChannels, 
	jvxData* inArg, jvxData** outArg, jvxCBool* addOnOut_nout, jvxData* fftin,
	jvxSize ll1, jvxSize ll2, jvxFFT*corefft, jvxSize outOffs, jvxSize outSz)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxData* ptrIn = NULL;
	jvxData* ptrOut = NULL;
	jvxSize outphase = 0;

	jvxData* in = inArg;	
	jvxData accu;

	jvxDataCplx* firW = NULL;
	// jvxData* out_src = NULL;

	assert(nHdl->firfft.tp == JVX_FIRFFT_PRV_TYPE_CF_NOUT);

	// jvxDataCplx* spec_out = nHdl->ram_cf.spec_ifft_in[1];


	ptrIn = fftin;
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
	for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue; i++)
	{
		ptrIn[i] = 0;
	}
	ptrIn[1] = 1.0;
	ptrIn[2] = 1.0;
	/**/
#endif

	jvx_execute_fft(corefft);

	assert(nChannels);

	jvxSize iC = 0;
	for (iC = outOffs; iC < outSz; iC++)
	{
		// Channel specific transfer function
		firW = nChannels->firWN[iC];
		jvxData* out = outArg[iC];

		ptrOut = nHdl->firfft.ram.out;
		outphase = (nHdl->firfft.ram.phase + nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.outoffset) %
			nHdl->firfft.derived_cpy.szFftValue;
		ptrOut += outphase;

		// Process 
		switch (nHdl->firfft.init_cpy.type)
		{
		case JVX_FIRFFT_SYMMETRIC_FIR:
			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				nHdl->ram_cf_nout.spec_ifft_in[i].re = nHdl->firfft.ram.spec[i].re * firW[i].re;
				nHdl->ram_cf_nout.spec_ifft_in[i].im = nHdl->firfft.ram.spec[i].im * firW[i].re;
			}
			ll1 = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - outphase, nHdl->firfft.init_cpy.bsize);
			ll2 = nHdl->firfft.init_cpy.bsize - ll1;

			break;
		default:

			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				// Be careful, this MIGHT be an inplace operation!!
				accu = nHdl->firfft.ram.spec[i].re * firW[i].re - nHdl->firfft.ram.spec[i].im * firW[i].im;
				nHdl->ram_cf_nout.spec_ifft_in[i].im = nHdl->firfft.ram.spec[i].re * firW[i].im + nHdl->firfft.ram.spec[i].im * firW[i].re;
				nHdl->ram_cf_nout.spec_ifft_in[i].re = accu;
			}
			break;
		}

		jvx_execute_ifft(nHdl->firfft.ram.coreifft);

		if (nHdl->firfft.ram.normOut)
		{
			if (addOnOut_nout[iC])
			{
				for (i = 0; i < ll1; i++)
				{
					*out++ += *ptrOut++ * nHdl->firfft.ram.normFactor;
				}

				ptrOut = nHdl->firfft.ram.out;
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

				ptrOut = nHdl->firfft.ram.out;
				for (i = 0; i < ll2; i++)
				{
					*out++ = *ptrOut++ * nHdl->firfft.ram.normFactor;
				}
			}
		}
		else
		{
			if (addOnOut_nout[iC])
			{
				for (i = 0; i < ll1; i++)
				{
					*out++ += *ptrOut++;
				}

				ptrOut = nHdl->firfft.ram.out;
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
				memcpy(out, nHdl->firfft.ram.out, sizeof(jvxData) * ll2);
				/*
				ptrOut = out_src;
				for (i = 0; i < ll2; i++)
				{
					*out++ = *ptrOut++;
				}
				*/
			}
		}
	}

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_firfft_cf_nin_nout_process(jvx_firfft* hdl, jvxData** inArg, jvxData** outArg, jvxCBool* addOnOut_nout)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize ll1 = 0;
	jvxSize ll2 = 0;

	jvxDataCplx* firW = NULL;
	// jvxData* out_src = NULL;

	if (hdl->prv)
	{
		jvx_firfft_cf_nout_prv* nHdl = (jvx_firfft_cf_nout_prv*)hdl->prv;
		assert(nHdl->firfft.tp == JVX_FIRFFT_PRV_TYPE_CF_NOUT);

		jvx_firfft_cf_nout_prmSync* nChannels = hdl->sync.ext;

		ll1 = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.phase, nHdl->firfft.init_cpy.bsize);
		ll2 = nHdl->firfft.init_cpy.bsize - ll1;

		if (nHdl->ram_cf_nout.multiInChannels.nChannelsIn > 1)
		{
			jvxSize cntOut = 0;
			for (i = 0; i < nHdl->ram_cf_nout.multiInChannels.nChannelsIn; i++)
			{
				jvx_firfft_cf_nout_process_core(nHdl, nChannels, inArg[i], outArg, addOnOut_nout, nHdl->ram_cf_nout.multiInChannels.in_bufs[i],
					ll1, ll2, nHdl->ram_cf_nout.multiInChannels.in_ffts[i], cntOut, cntOut + nChannels->stride);
				cntOut += nChannels->stride;
			}
		}
		else
		{
			jvx_firfft_cf_nout_process_core(nHdl, nChannels, inArg[0], outArg, addOnOut_nout, nHdl->firfft.ram.in, ll1, ll2, nHdl->firfft.ram.corefft, 0, nChannels->N);
		}

		// Forward phase
		nHdl->firfft.ram.phase = (nHdl->firfft.ram.phase + nHdl->firfft.init_cpy.bsize) % nHdl->firfft.derived_cpy.szFftValue;

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

// ==========================================================================================================================
// ==========================================================================================================================

void 
jvx_local_out_old_new(jvxData weightInit, jvxData weightInc, jvxData* ptrSpecInTdStart, jvxSize offsetSpecInTdStart, jvxData* out, jvxCBool addOnOut, jvxSize ll1, jvxSize ll2, jvxData normFact)
{
	jvxSize i = 0;
	jvxData accu = 0;
	jvxData weight = weightInit;
	jvxData* ptrOut_old_new = ptrSpecInTdStart + offsetSpecInTdStart;

	if (addOnOut)
	{
		// First part
		for (i = 0; i < ll1; i++)
		{
			accu = *ptrOut_old_new++ * weight;
			accu *= normFact;
			*out += accu;
			out++;
			weight += weightInc;
		}

		ptrOut_old_new = ptrSpecInTdStart;
		for (i = 0; i < ll2; i++)
		{
			accu = *ptrOut_old_new++ * weight; 
			accu *= normFact;
			*out += accu;
			out++;
			weight += weightInc;
		}
	}
	else
	{
		for (i = 0; i < ll1; i++)
		{
			*out = *ptrOut_old_new++ * weight; // +*ptrOut_new++ * weightCf_new;
			*out *= normFact;
			out++;
			weight += weightInc;
		}

		ptrOut_old_new = ptrSpecInTdStart;
		for (i = 0; i < ll2; i++)
		{
			*out = *ptrOut_old_new++ * weight; // +*ptrOut_new++ * weightCf_new;
			*out *= normFact;
			out++;
			weight += weightInc;
		}
	}
}

jvxDspBaseErrorType jvx_firfft_cf_nout_process_update_weights_core(jvx_firfft_cf_nout_prv* nHdl, jvx_firfft_cf_nout_prmSync* nChannels, 
	jvxData* inArg, jvxData** outArg, jvxDataCplx** newWeights, jvxCBool* addOnOut_nout, jvxData* fftin,
	jvxSize ll1, jvxSize ll2, jvxFFT* corefft, jvxSize outOffs, jvxSize outSz)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxData* ptrIn = NULL;
	jvxSize outphase = 0;
	jvxData* in = inArg;

	jvxSize idxNew = 0;

	ptrIn = fftin;
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

	jvx_execute_fft(corefft);

	// jvxDataCplx* spec_out_copy = nHdl->ram_cf.spec_ifft_in[0];
	assert(nChannels);

	jvxSize iC = 0;
	for (iC = outOffs; iC < outSz; iC++)
	{
		jvxData* out = outArg[iC];

		jvxDataCplx* firW_new = newWeights[iC];
		jvxDataCplx* firW_old = nChannels->firWN[iC];

		outphase = (nHdl->firfft.ram.phase + nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.outoffset) %
			nHdl->firfft.derived_cpy.szFftValue;

		// Process "old" filtering
		switch (nHdl->firfft.init_cpy.type)
		{
		case JVX_FIRFFT_SYMMETRIC_FIR:
			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				nHdl->ram_cf_nout.spec_ifft_in[i].re = nHdl->firfft.ram.spec[i].re * firW_old[i].re;
				nHdl->ram_cf_nout.spec_ifft_in[i].im = nHdl->firfft.ram.spec[i].im * firW_old[i].re;
			}

			// Modify read-out phase
			ll1 = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - outphase, nHdl->firfft.init_cpy.bsize);
			ll2 = nHdl->firfft.init_cpy.bsize - ll1;
			break;
		default:

			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				nHdl->ram_cf_nout.spec_ifft_in[i].im = nHdl->firfft.ram.spec[i].re * firW_old[i].im + nHdl->firfft.ram.spec[i].im * firW_old[i].re;
				nHdl->ram_cf_nout.spec_ifft_in[i].re = nHdl->firfft.ram.spec[i].re * firW_old[i].re - nHdl->firfft.ram.spec[i].im * firW_old[i].im;
			}
			break;
		}

		jvx_execute_ifft(nHdl->firfft.ram.coreifft);
		jvx_local_out_old_new(1.0, -nHdl->ram_cf_nout.cf_inc, nHdl->firfft.ram.out, outphase, out, addOnOut_nout[iC], ll1, ll2, nHdl->firfft.ram.normFactor);


		// Process "new" filtering
		switch (nHdl->firfft.init_cpy.type)
		{
		case JVX_FIRFFT_SYMMETRIC_FIR:
			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				nHdl->ram_cf_nout.spec_ifft_in[i].re = nHdl->firfft.ram.spec[i].re * firW_new[i].re;
				nHdl->ram_cf_nout.spec_ifft_in[i].im = nHdl->firfft.ram.spec[i].im * firW_new[i].re;
			}
			break;
		default:

			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				nHdl->ram_cf_nout.spec_ifft_in[i].im = nHdl->firfft.ram.spec[i].re * firW_new[i].im + nHdl->firfft.ram.spec[i].im * firW_new[i].re;
				nHdl->ram_cf_nout.spec_ifft_in[i].re = nHdl->firfft.ram.spec[i].re * firW_new[i].re - nHdl->firfft.ram.spec[i].im * firW_new[i].im;
			}
			break;
		}
		jvx_execute_ifft(nHdl->firfft.ram.coreifft);
		jvx_local_out_old_new(0.0, nHdl->ram_cf_nout.cf_inc, nHdl->firfft.ram.out, outphase, out, true, ll1, ll2, nHdl->firfft.ram.normFactor);

		// Take over the new weights			
		for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
		{
			firW_old[i].re = firW_new[i].re;
			firW_old[i].im = firW_new[i].im;
		}
	}

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_firfft_cf_nin_nout_process_update_weights(jvx_firfft* hdl, jvxData** inArg, jvxData** outArg, jvxDataCplx** newWeights, jvxCBool* addOnOut_nout)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize ll1 = 0;
	jvxSize ll2 = 0;

	jvxDataCplx* firW = NULL;
	// jvxData* out_src = NULL;

	if (hdl->prv)
	{
		jvx_firfft_cf_nout_prv* nHdl = (jvx_firfft_cf_nout_prv*)hdl->prv;
		assert(nHdl->firfft.tp == JVX_FIRFFT_PRV_TYPE_CF_NOUT);

		jvx_firfft_cf_nout_prmSync* nChannels = hdl->sync.ext;

		ll1 = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.phase, nHdl->firfft.init_cpy.bsize);
		ll2 = nHdl->firfft.init_cpy.bsize - ll1;

		if (nHdl->ram_cf_nout.multiInChannels.nChannelsIn > 1)
		{
			jvxSize cntOut = 0;
			for (i = 0; i < nHdl->ram_cf_nout.multiInChannels.nChannelsIn; i++)
			{
				jvx_firfft_cf_nout_process_update_weights_core(nHdl, nChannels, inArg[i], outArg, newWeights, addOnOut_nout, nHdl->ram_cf_nout.multiInChannels.in_bufs[i],
					ll1, ll2, nHdl->ram_cf_nout.multiInChannels.in_ffts[i], cntOut, cntOut + nChannels->stride);
				cntOut += nChannels->stride;
			}
		}
		else
		{
			jvx_firfft_cf_nout_process_update_weights_core(nHdl, nChannels, inArg[0], outArg, newWeights, addOnOut_nout, nHdl->firfft.ram.in, ll1, ll2, nHdl->firfft.ram.corefft, 0, nChannels->N);
		}

		// Forward phase
		nHdl->firfft.ram.phase = (nHdl->firfft.ram.phase + nHdl->firfft.init_cpy.bsize) % nHdl->firfft.derived_cpy.szFftValue;

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}
