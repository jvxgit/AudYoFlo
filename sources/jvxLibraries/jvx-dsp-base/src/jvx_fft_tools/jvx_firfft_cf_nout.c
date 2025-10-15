#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_firfft.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_dsp_base.h"
#include "jvx_allocators/jvx_allocators.h"

#include "jvx_fft_tools/jvx_firfft_prv.h"

#include "jvx_firfft_cf_prv.h"
#include "jvx_fft_tools/jvx_firfft_cf_nout.h"

jvxDspBaseErrorType 
jvx_firfft_cf_nout_init(jvx_firfft* hdl, jvxHandle* fftCfgHdl, jvxSize nChannels)
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

		jvx_firfft_cf_nout_prmSync* nChans = jvx_allocator->alloc(sizeof(jvx_firfft_cf_nout_prmSync), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);
		nChans->N = nChannels;
		nChans->firWN = jvx_allocator->alloc(sizeof(jvxDataCplx), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), nChans->N);
		for (i = 0; i < nChannels; i++)
		{
			nChans->firWN[i] = jvx_allocator->alloc(sizeof(jvxDataCplx), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), nHdl->firfft.derived_cpy.lFirW);			
		}

		hdl->sync.firW = NULL;
		hdl->sync.ext = nChans;

		nHdl->firfft.ram.phase = 0;
		nHdl->firfft.ram.normFactor = 1.0 / (jvxData)nHdl->firfft.derived_cpy.szFftValue;
		nHdl->ram_cf.cf_inc = 1.0 / hdl->init.bsize;

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

			jvx_firfft_cf_nout_prmSync* nChans = hdl->sync.ext;
			for (i = 0; i < nChans->N; i++)
			{
				jvx_allocator->dealloc((jvxHandle**)&nChans->firWN[i], (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
			}
			jvx_allocator->dealloc((jvxHandle**)&nChans->firWN, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
			jvx_allocator->dealloc((jvxHandle**)&nChans, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
			
			hdl->sync.ext = NULL;
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

/*
void jvx_firfft_cf_nout_compute_weights(jvx_firfft* hdl, jvxData* fir, jvxSize lFir)
{
	jvxSize i;
	jvx_firfft_cf_prv* nHdl = (jvx_firfft_cf_prv*)hdl->prv;
	assert(lFir == nHdl->firfft.init_cpy.lFir);
	jvx_firfft_cf_nout_prmSync* nChannels = hdl->sync.ext;

	if (nHdl->firfft.init_cpy.type == JVX_FIRFFT_SYMMETRIC_FIR)
	{
		// Copy such that we get a zero phase filter
		nHdl->ram_cf.in_weights[0] = nHdl->firfft.init_cpy.fir[nHdl->firfft.ram.outoffset];
		for (i = 0; i < nHdl->firfft.ram.outoffset; i++)
		{
			nHdl->ram_cf.in_weights[nHdl->firfft.derived_cpy.szFftValue - i - 1] = fir[nHdl->firfft.ram.outoffset - i - 1];
			nHdl->ram_cf.in_weights[i + 1] = fir[nHdl->firfft.ram.outoffset + i + 1];
		}
	}
	else
	{
		memcpy(nHdl->ram_cf.in_weights, fir, sizeof(jvxData) * lFir);
	}

	jvx_execute_fft(nHdl->ram_cf.corefft); // -> nHdl->ram_cf.spec[1]
}

void
jvx_firfft_cf_nout_copy_weights(jvx_firfft* hdl, jvxDataCplx* firW, jvxSize lFirW)
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
*/


jvxDspBaseErrorType jvx_firfft_cf_nout_process(jvx_firfft* hdl, jvxData* inArg, jvxData** outArg, jvxCBool addOnOut)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize ll1 = 0;
	jvxSize ll2 = 0;
	jvxData* ptrIn = NULL;
	jvxData* ptrOut = NULL;
	jvxSize outphase = 0;

	jvxData* in = inArg;	
	jvxData accu;

	jvxDataCplx* spec_out = NULL;
	jvxDataCplx* firW = NULL;
	jvxIFFT* coreifft = NULL;
	jvxData* out_src = NULL;

	if (hdl->prv)
	{
		jvx_firfft_cf_prv* nHdl = (jvx_firfft_cf_prv*)hdl->prv;
		jvx_firfft_cf_nout_prmSync* nChannels = hdl->sync.ext;

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
		for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue; i++)
		{
			ptrIn[i] = 0;
		}
		ptrIn[1] = 1.0;
		ptrIn[2] = 1.0;
		/**/
#endif

		jvx_execute_fft(nHdl->firfft.ram.corefft);

		assert(nChannels);

		jvxSize iC = 0;
		for (iC = 0; iC < nChannels->N; iC++)
		{
			jvxDataCplx* spec_out_copy = nHdl->ram_cf.spec_ifft_in[0];

			// Secondary ifft input
			spec_out = nHdl->ram_cf.spec_ifft_in[1];

			// Channel specific transfer function
			firW = nChannels->firWN[iC];
			coreifft = nHdl->ram_cf.coreifft[1];
			out_src = nHdl->ram_cf.ifft_out[1];
			jvxData* out = outArg[iC];

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
					spec_out[i].re = spec_out_copy[i].re * firW[i].re;
					spec_out[i].im = spec_out_copy[i].im * firW[i].re;
				}
				ll1 = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - outphase, nHdl->firfft.init_cpy.bsize);
				ll2 = nHdl->firfft.init_cpy.bsize - ll1;

				break;
			default:

				for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
				{
					// Be careful, this MIGHT be an inplace operation!!
					accu = spec_out_copy[i].re * firW[i].re - spec_out_copy[i].im * firW[i].im;
					spec_out[i].im = spec_out_copy[i].re * firW[i].im + spec_out_copy[i].im * firW[i].re;
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
						*out++ += *ptrOut++;
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
					memcpy(out, out_src, sizeof(jvxData) * ll2);
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

		// Forward phase
		nHdl->firfft.ram.phase = (nHdl->firfft.ram.phase + nHdl->firfft.init_cpy.bsize) % nHdl->firfft.derived_cpy.szFftValue;

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

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

jvxDspBaseErrorType jvx_firfft_cf_nout_process_update_weights(jvx_firfft* hdl, jvxData* inArg, jvxData** outArg, jvxDataCplx** newWeights, jvxCBool addOnOut)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize ll1 = 0;
	jvxSize ll2 = 0;
	jvxData* ptrIn = NULL;
	jvxData* ptrOut_old_new = NULL;
	jvxSize outphase = 0;

	jvxData* in = inArg;
	jvxData accu;

	jvxDataCplx* spec_out_old_new = NULL;

	jvxDataCplx* firW_old = NULL;

	jvxIFFT* coreifft_old_new = NULL;


	jvxData* out_src_old_new = NULL;

	jvxSize idxNew = 0;
	jvxData weightCf_old = 0;
	jvxData weightCf_new = 0;

	if (hdl->prv)
	{
		jvx_firfft_cf_prv* nHdl = (jvx_firfft_cf_prv*)hdl->prv;
		jvx_firfft_cf_nout_prmSync* nChannels = hdl->sync.ext;

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

		jvxDataCplx* spec_out_copy = nHdl->ram_cf.spec_ifft_in[0];
		assert(nChannels);

		jvxSize iC = 0;
		for (iC = 0; iC < nChannels->N; iC++)
		{
			jvxData* out = outArg[iC];

			jvxDataCplx* firW_new = newWeights[iC];
			firW_old = nChannels->firWN[iC];

			// Channel specific transfer function
			spec_out_copy = nHdl->ram_cf.spec_ifft_in[0];
			spec_out_old_new = nHdl->ram_cf.spec_ifft_in[1];
			coreifft_old_new = nHdl->ram_cf.coreifft[1];
			out_src_old_new = nHdl->ram_cf.ifft_out[1];
			outphase = (nHdl->firfft.ram.phase + nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.outoffset) %
				nHdl->firfft.derived_cpy.szFftValue;

			// Process "old" filtering
			switch (nHdl->firfft.init_cpy.type)
			{
			case JVX_FIRFFT_SYMMETRIC_FIR:
				for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
				{
					spec_out_old_new[i].re = spec_out_copy[i].re * firW_old[i].re;
					spec_out_old_new[i].im = spec_out_copy[i].im * firW_old[i].re;
				}

				// Modify read-out phase
				ll1 = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - outphase, nHdl->firfft.init_cpy.bsize);
				ll2 = nHdl->firfft.init_cpy.bsize - ll1;
				break;
			default:

				for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
				{
					spec_out_old_new[i].im = spec_out_copy[i].re * firW_old[i].im + spec_out_copy[i].im * firW_old[i].re;
					spec_out_old_new[i].re = spec_out_copy[i].re * firW_old[i].re - spec_out_copy[i].im * firW_old[i].im;
				}
				break;
			}			
			jvx_execute_ifft(coreifft_old_new);
			jvx_local_out_old_new(1.0, -nHdl->ram_cf.cf_inc, out_src_old_new, outphase, out, addOnOut, ll1, ll2, nHdl->firfft.ram.normFactor);

			// Process "new" filtering
			switch (nHdl->firfft.init_cpy.type)
			{
			case JVX_FIRFFT_SYMMETRIC_FIR:
				for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
				{
					spec_out_old_new[i].re = spec_out_copy[i].re * firW_new[i].re;
					spec_out_old_new[i].im = spec_out_copy[i].im * firW_new[i].re;
				}
				break;
			default:

				for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
				{
					spec_out_old_new[i].im = spec_out_copy[i].re * firW_new[i].im + spec_out_copy[i].im * firW_new[i].re;
					spec_out_old_new[i].re = spec_out_copy[i].re * firW_new[i].re - spec_out_copy[i].im * firW_new[i].im;
				}
				break;
			}
			jvx_execute_ifft(coreifft_old_new);
			jvx_local_out_old_new(0.0, nHdl->ram_cf.cf_inc, out_src_old_new, outphase, out, true, ll1, ll2, nHdl->firfft.ram.normFactor);

			// Take over the new weights
			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				firW_old[i].re = firW_new[i].re;
				firW_old[i].im = firW_new[i].im;
			}
		}
		nHdl->firfft.ram.phase = (nHdl->firfft.ram.phase + nHdl->firfft.init_cpy.bsize) % nHdl->firfft.derived_cpy.szFftValue;

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;


	return JVX_DSP_NO_ERROR;
}
