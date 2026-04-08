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

		struct
		{
			jvxSize cBufferFadeLength;
			jvxData** out_cf_bufs;
			jvxData pSampleCrossFade;
		} multiOutChannels;

		// Cross fade increment value
		jvxData cf_inc;

		struct jvx_fft_ifft_core_global_attach* ptrAttach;
	} ram_cf_nout;

	jvxCBool allocatedViaAllocator;
} jvx_firfft_cf_nout_prv;

typedef struct
{
	jvxSize ll1_in;
	jvxSize ll2_in;
	jvxSize ll1_out;
	jvxSize ll2_out;
	jvxSize newPhase;
	jvxSize outphase;
} jvx_firfft_cf_nout_rt_args;

jvxDspBaseErrorType 
jvx_firfft_cf_nout_init(jvx_firfft* hdl, jvxSize nStride, jvxSize nChannelsIn, jvxFFTGlobal* fftGlobCfg)
{
	if (hdl->prv == NULL)
	{
		struct jvx_fft_ifft_core_global_attach* ptrAttachFromGlobal = NULL;
		jvx_firfft_cf_nout_reuse elmBufferSources;
		memset(&elmBufferSources, 0, sizeof(elmBufferSources));

		jvx_firfft_cf_nout_prv* nHdl = NULL;
		jvx_firfft_cf_nout_reuse* ptrReuse = NULL;

		jvxSize N = 0, i = 0;
		jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;

		assert(jvx_allocator != NULL);

		nHdl = (jvx_firfft_cf_nout_prv*)jvx_allocator->alloc(sizeof(jvx_firfft_cf_nout_prv), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);
		memset(nHdl, 0, sizeof(jvx_firfft_cf_nout_prv));

		nHdl->firfft.tp = JVX_FIRFFT_PRV_TYPE_CF_NOUT;
		// nHdl->ram_cf_nout.ptrAttach = NULL;

		hdl->prv = nHdl;

		jvx_firfft_update(hdl, JVX_DSP_UPDATE_INIT, true); // this sets all derived values

		jvx_allocator->purpose = JVX_ALLOCATOR_OBJECT_PURPOSE_TIME_CRITICAL;

		resL = jvx_create_fft_ifft_global(&nHdl->firfft.ram.fftGlob, nHdl->firfft.derived_cpy.szFft, fftGlobCfg);
		assert(resL == JVX_DSP_NO_ERROR);

		// Check if current fft implementation requires normalization
		nHdl->firfft.ram.normOut = jvx_fft_requires_normalization(nHdl->firfft.ram.fftGlob);		
		
		// Try to get data from global handle
		if (nHdl->firfft.init_cpy.prepareReuse)
		{
			ptrAttachFromGlobal = (struct jvx_fft_ifft_core_global_attach*)jvx_attached_tag_fft_ifft_global(nHdl->firfft.ram.fftGlob, "jvx_firfft_cf_nout_reuse");
			if (ptrAttachFromGlobal)
			{
				ptrReuse = ptrAttachFromGlobal->attached_data;
				if (ptrReuse->fft_size >= jvx_get_fft_size(nHdl->firfft.derived_cpy.szFft))
				{
					elmBufferSources = *ptrReuse;
				}
			}
		}

		// Backward compatibility		
		// Secondary ifft
		resL = jvx_create_ifft_complex_2_real(&nHdl->firfft.ram.coreifft,
			nHdl->firfft.ram.fftGlob, nHdl->firfft.derived_cpy.szFft,
			&nHdl->ram_cf_nout.spec_ifft_in, &nHdl->firfft.ram.out, &N,
			JVX_FFT_IFFT_PRESERVE_INPUT,
			elmBufferSources.ifft_in, elmBufferSources.ifft_out, 0);
		assert(resL == JVX_DSP_NO_ERROR);		

		//assert(N == nHdl->derived_cpy.szFftValue);

		// Reuse input from ifft[0] for fft output, first realization
		resL = jvx_create_fft_real_2_complex(&nHdl->firfft.ram.corefft,
			nHdl->firfft.ram.fftGlob, nHdl->firfft.derived_cpy.szFft,
			&nHdl->firfft.ram.in, &nHdl->firfft.ram.spec,
			&N, JVX_FFT_IFFT_PRESERVE_INPUT,
			NULL, elmBufferSources.fft_out, 0);
		
		// Attach shared data
		if (nHdl->firfft.init_cpy.prepareReuse && (ptrReuse == NULL))
		{
			ptrReuse = (jvx_firfft_cf_nout_reuse*)jvx_allocator->alloc(sizeof(jvx_firfft_cf_nout_reuse), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);
			nHdl->ram_cf_nout.ptrAttach = (struct jvx_fft_ifft_core_global_attach*)jvx_allocator->alloc(sizeof(struct jvx_fft_ifft_core_global_attach), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);

			ptrReuse->fft_size = nHdl->firfft.derived_cpy.szFftValue;
			ptrReuse->fft_out = nHdl->firfft.ram.spec;
			ptrReuse->ifft_in = nHdl->ram_cf_nout.spec_ifft_in;
			ptrReuse->ifft_out = nHdl->firfft.ram.out;

			nHdl->ram_cf_nout.ptrAttach->next = NULL;
			nHdl->ram_cf_nout.ptrAttach->tag = "jvx_firfft_cf_nout_reuse";
			nHdl->ram_cf_nout.ptrAttach->attached_data = ptrReuse;

			jvx_attach_fft_ifft_global(nHdl->firfft.ram.fftGlob, nHdl->ram_cf_nout.ptrAttach);	
		}

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

		nChans->firWN = jvx_allocator->alloc(sizeof(jvxDataCplx*), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), nChans->N);
		for (i = 0; i < nChans->N; i++)
		{
			nChans->firWN[i] = jvx_allocator->alloc(sizeof(jvxDataCplx), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_FAST_SLOW ), nHdl->firfft.derived_cpy.lFirW);
		}

		nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength = hdl->init.cBufferFadeLength;
		nHdl->ram_cf_nout.multiOutChannels.out_cf_bufs = NULL;
		nHdl->ram_cf_nout.multiOutChannels.pSampleCrossFade = 0.0;
		if (nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength > 0)
		{
			// We need to create a little more overhead
			nHdl->firfft.ram.outoffset += nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength;

			nHdl->ram_cf_nout.multiOutChannels.pSampleCrossFade = 1.0 / (jvxData)nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength;
			nHdl->ram_cf_nout.multiOutChannels.out_cf_bufs = jvx_allocator->alloc(sizeof(jvxData*), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), nChans->N);
			for (i = 0; i < nChans->N; i++)
			{
				nHdl->ram_cf_nout.multiOutChannels.out_cf_bufs[i] = jvx_allocator->alloc(sizeof(jvxDataCplx), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_FAST_SLOW), nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength);
			}
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
jvx_firfft_cf_nout_terminate(jvx_firfft* hdl, jvxFFTGlobal* fftGlobCfg)
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

			if (nHdl->ram_cf_nout.ptrAttach)
			{
				// This is very dangerous but I do not see any other chance: Remove the pointers from global handle.
				// These pointers may be referenced at other places!
				jvx_firfft_cf_nout_reuse* ptrReuse = NULL;
				struct jvx_fft_ifft_core_global_attach* ptrDetach = NULL;
				jvx_detach_fft_ifft_global(nHdl->firfft.ram.fftGlob, &ptrDetach);
				assert(ptrDetach == nHdl->ram_cf_nout.ptrAttach);

				ptrReuse = (jvx_firfft_cf_nout_reuse*)ptrDetach->attached_data;
				ptrReuse->fft_out = NULL;
				ptrReuse->ifft_in = NULL;
				ptrReuse->ifft_out = NULL;
				ptrReuse->fft_size = 0;
				jvx_allocator->dealloc((jvxHandle**)&ptrReuse, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));

				ptrDetach->attached_data = NULL;
				ptrDetach->tag = NULL;
				ptrDetach->next = NULL;
				jvx_allocator->dealloc((jvxHandle**)&ptrDetach, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
				
				nHdl->ram_cf_nout.ptrAttach = NULL;
			}

			jvx_destroy_fft(nHdl->firfft.ram.corefft);
			nHdl->firfft.ram.corefft = NULL;
			nHdl->firfft.ram.in = NULL;
			nHdl->firfft.ram.spec = NULL;

			jvx_destroy_ifft(nHdl->firfft.ram.coreifft);
			nHdl->firfft.ram.coreifft = NULL;
			nHdl->ram_cf_nout.spec_ifft_in = NULL;
			nHdl->firfft.ram.out = NULL;			

			jvx_destroy_fft_ifft_global(nHdl->firfft.ram.fftGlob, fftGlobCfg);
			nHdl->firfft.ram.fftGlob = NULL;

			jvx_firfft_cf_nout_prmSync* nChans = hdl->sync.ext;
			for (i = 0; i < nChans->N; i++)
			{
				jvx_allocator->dealloc((jvxHandle**)&nChans->firWN[i], (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
			}
			jvx_allocator->dealloc((jvxHandle**)&nChans->firWN, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
			
			if (nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength > 0)
			{
				for (i = 0; i < nChans->N; i++)
				{
					jvx_allocator->dealloc((jvxHandle**)&nHdl->ram_cf_nout.multiOutChannels.out_cf_bufs[i], (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_FAST_SLOW));
				}
				jvx_allocator->dealloc((jvxHandle**)&nHdl->ram_cf_nout.multiOutChannels.out_cf_bufs, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
			}
			nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength = 0;

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

// ========================================================================================================================

jvxDspBaseErrorType jvx_firfft_cf_nout_process_core(jvx_firfft_cf_nout_prv* nHdl, jvx_firfft_cf_nout_prmSync* nChannels, 
	jvxData* inArg, jvxData** outArg, jvxCBool* addOnOut_nout, jvxData* fftin,
	jvx_firfft_cf_nout_rt_args* args, jvxFFT*corefft, jvxSize outOffs, jvxSize outSz)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxData* ptrIn = NULL;
	jvxData* ptrOut = NULL;

	jvxData* in = inArg;	
	jvxData accu;

	jvxDataCplx* firW = NULL;
	// jvxData* out_src = NULL;

	assert(nHdl->firfft.tp == JVX_FIRFFT_PRV_TYPE_CF_NOUT);

	// jvxDataCplx* spec_out = nHdl->ram_cf.spec_ifft_in[1];


	ptrIn = fftin;
	ptrIn += nHdl->firfft.ram.phase;

	for (i = 0; i < args->ll1_in; i++)
	{
		*ptrIn++ = *in++;
	}
	ptrIn = nHdl->firfft.ram.in;
	for (i = 0; i < args->ll2_in; i++)
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

		// Process 
		switch (nHdl->firfft.init_cpy.type)
		{
		case JVX_FIRFFT_SYMMETRIC_FIR:
			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				nHdl->ram_cf_nout.spec_ifft_in[i].re = nHdl->firfft.ram.spec[i].re * firW[i].re;
				nHdl->ram_cf_nout.spec_ifft_in[i].im = nHdl->firfft.ram.spec[i].im * firW[i].re;
			}
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

		ptrOut = nHdl->firfft.ram.out;
		ptrOut += args->outphase;

		if (nHdl->firfft.ram.normOut)
		{
			if (addOnOut_nout[iC])
			{
				for (i = 0; i < args->ll1_out; i++)
				{
					*out++ += *ptrOut++ * nHdl->firfft.ram.normFactor;
				}

				ptrOut = nHdl->firfft.ram.out;
				for (i = 0; i < args->ll2_out; i++)
				{
					*out++ += *ptrOut++ * nHdl->firfft.ram.normFactor;
				}

			}
			else
			{
				for (i = 0; i < args->ll1_out; i++)
				{
					*out++ = *ptrOut++ * nHdl->firfft.ram.normFactor;
				}

				ptrOut = nHdl->firfft.ram.out;
				for (i = 0; i < args->ll2_out; i++)
				{
					*out++ = *ptrOut++ * nHdl->firfft.ram.normFactor;
				}
			}
		}
		else
		{
			if (addOnOut_nout[iC])
			{
				for (i = 0; i < args->ll1_out; i++)
				{
					*out++ += *ptrOut++;
				}

				ptrOut = nHdl->firfft.ram.out;
				for (i = 0; i < args->ll2_out; i++)
				{
					*out++ += *ptrOut++;
				}

			}
			else
			{
				memcpy(out, ptrOut, args->ll1_out * sizeof(jvxData));
				/*
				for (i = 0; i < ll1; i++)
				{
					*out++ = *ptrOut++;
				}
				*/

				out += args->ll1_out;
				memcpy(out, nHdl->firfft.ram.out, sizeof(jvxData) * args->ll2_out);
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
	
	jvx_firfft_cf_nout_rt_args funcArgs;

	jvxDataCplx* firW = NULL;
	// jvxData* out_src = NULL;

	if (hdl->prv)
	{
		jvx_firfft_cf_nout_prv* nHdl = (jvx_firfft_cf_nout_prv*)hdl->prv;
		assert(nHdl->firfft.tp == JVX_FIRFFT_PRV_TYPE_CF_NOUT);
		assert(nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength == 0);

		jvx_firfft_cf_nout_prmSync* nChannels = hdl->sync.ext;

		funcArgs.outphase = (nHdl->firfft.ram.phase + nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.outoffset) %
			nHdl->firfft.derived_cpy.szFftValue;
		funcArgs.newPhase = 0; // not in use

		funcArgs.ll1_in = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.phase, nHdl->firfft.init_cpy.bsize);
		funcArgs.ll2_in = nHdl->firfft.init_cpy.bsize - funcArgs.ll1_in;

		// Pass readout pointer <nHdl->firfft.ram.outoffset> to the "left" in the circbuffer.
		// This is due to the symmetrie of the fft when converting an absolute buffer (phase 0) to the time domain
		funcArgs.ll1_out = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - funcArgs.outphase, nHdl->firfft.init_cpy.bsize);
		funcArgs.ll2_out = nHdl->firfft.init_cpy.bsize - funcArgs.ll1_out;


		if (nHdl->ram_cf_nout.multiInChannels.nChannelsIn > 1)
		{
			jvxSize cntOut = 0;
			for (i = 0; i < nHdl->ram_cf_nout.multiInChannels.nChannelsIn; i++)
			{
				jvx_firfft_cf_nout_process_core(nHdl, nChannels, inArg[i], outArg, addOnOut_nout, nHdl->ram_cf_nout.multiInChannels.in_bufs[i],
					&funcArgs, nHdl->ram_cf_nout.multiInChannels.in_ffts[i], cntOut, cntOut + nChannels->stride);
				cntOut += nChannels->stride;
			}
		}
		else
		{
			jvx_firfft_cf_nout_process_core(nHdl, nChannels, inArg[0], outArg, addOnOut_nout, nHdl->firfft.ram.in, &funcArgs, nHdl->firfft.ram.corefft, 0, nChannels->N);
		}

		// Forward phase
		nHdl->firfft.ram.phase = (nHdl->firfft.ram.phase + nHdl->firfft.init_cpy.bsize) % nHdl->firfft.derived_cpy.szFftValue;

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

// ==========================================================================================================================================
// ==========================================================================================================================================
// ==========================================================================================================================================
// ==========================================================================================================================================

jvxDspBaseErrorType jvx_firfft_cf_nout_process_core_ifcf(jvx_firfft_cf_nout_prv* nHdl, jvx_firfft_cf_nout_prmSync* nChannels,
	jvxData* inArg, jvxData** outArg, jvxCBool* addOnOut_nout, jvxData* fftin,
	jvx_firfft_cf_nout_rt_args* args, jvxFFT* corefft, jvxSize outOffs, jvxSize outSz)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxData* ptrIn = NULL;

	jvxData* in = inArg;
	jvxData accu;

	jvxDataCplx* firW = NULL;
	// jvxData* out_src = NULL;

	assert(nHdl->firfft.tp == JVX_FIRFFT_PRV_TYPE_CF_NOUT);
	assert(nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength);

	// jvxDataCplx* spec_out = nHdl->ram_cf.spec_ifft_in[1];


	ptrIn = fftin;
	ptrIn += nHdl->firfft.ram.phase;

	for (i = 0; i < args->ll1_in; i++)
	{
		*ptrIn++ = *in++;
	}
	ptrIn = nHdl->firfft.ram.in;
	for (i = 0; i < args->ll2_in; i++)
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

		// Process 
		switch (nHdl->firfft.init_cpy.type)
		{
		case JVX_FIRFFT_SYMMETRIC_FIR:
			for (i = 0; i < nHdl->firfft.derived_cpy.szFftValue / 2 + 1; i++)
			{
				nHdl->ram_cf_nout.spec_ifft_in[i].re = nHdl->firfft.ram.spec[i].re * firW[i].re;
				nHdl->ram_cf_nout.spec_ifft_in[i].im = nHdl->firfft.ram.spec[i].im * firW[i].re;
			}
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

		// IFFT output buffer
		jvxData* ptrFromIfft = nHdl->firfft.ram.out;
		ptrFromIfft += args->outphase;

		// CF Buffer pointer
		jvxData* ptrFromCfBuffer = nHdl->ram_cf_nout.multiOutChannels.out_cf_bufs[iC];

		jvxSize cfLength = nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength;

		jvxSize ll1_0 = JVX_MIN(args->ll1_out, cfLength);
		jvxSize ll1_1 = args->ll1_out - ll1_0;
		cfLength -= ll1_0;

		jvxData cfNorm_old = 1.0;
		jvxData cfNorm_new = 0.0;
		jvxData normFac = nHdl->firfft.ram.normFactor;

		/**************************************************************************************
		 * Probably not the most efficient code design: we may run fft with or without normalized 
		 * output. And also, we may add the output to a buffer OR replace bufer content. We could also 
		 * involve this with a more compact code structure BUT we do not do that here since the 
		 * code is meant to operate most efficiently on a DSP.
		 ************************************************************************************* */
		if (nHdl->firfft.ram.normOut)
		{			
			// Here, the ifft output is assumed to be not normalized - normalization mandatory
			if (addOnOut_nout[iC])
			{
				// Here, we add the output value to the buffer which is passed
				// 
				// Crossfade area I
				for (i = 0; i < ll1_0; i++)
				{
					jvxData valNewFromIfft = *ptrFromIfft++;
					jvxData valNewFromCfBuf = *ptrFromCfBuffer++;
					jvxData valNew = valNewFromIfft * cfNorm_new + valNewFromCfBuf * cfNorm_old;
					valNew *= normFac;
					*out++ += valNew;
					cfNorm_new += nHdl->ram_cf_nout.multiOutChannels.pSampleCrossFade;
					cfNorm_old = 1.0 - cfNorm_new;
				}

				// Non-crossfade area
				for (i = 0; i < ll1_1; i++)
				{
					*out++ += *ptrFromIfft++ * normFac;
				}

				// Wrap around
				jvxSize ll2_0 = JVX_MIN(args->ll2_out, cfLength);
				jvxSize ll2_1 = args->ll2_out - ll2_0;

				// Reset the pointer for input to beginning of buffer
				ptrFromIfft = nHdl->firfft.ram.out;

				for (i = 0; i < ll2_0; i++)
				{
					jvxData valNewFromIfft = *ptrFromIfft++;
					jvxData valNewFromCfBuf = *ptrFromCfBuffer++;
					jvxData valNew = valNewFromIfft * cfNorm_new + valNewFromCfBuf * cfNorm_old;
					valNew *= normFac;
					*out++ += valNew;
					cfNorm_new += nHdl->ram_cf_nout.multiOutChannels.pSampleCrossFade;
					cfNorm_old = 1.0 - cfNorm_new;
				}

				// Non cf buffer
				for (i = 0; i < ll2_1; i++)
				{
					*out++ += *ptrFromIfft++ * normFac;
				}
			}
			else
			{
				// Here, we write the output value to the buffer which is passed - no addition
				// Crossfade area I
				for (i = 0; i < ll1_0; i++)
				{
					jvxData valNewFromIfft = *ptrFromIfft++;
					jvxData valNewFromCfBuf = *ptrFromCfBuffer++;
					jvxData valNew = valNewFromIfft * cfNorm_new + valNewFromCfBuf * cfNorm_old;
					valNew *= normFac;
					*out++ = valNew;
					cfNorm_new += nHdl->ram_cf_nout.multiOutChannels.pSampleCrossFade;
					cfNorm_old = 1.0 - cfNorm_new;
				}

				// Non-crossfade area
				for (i = 0; i < ll1_1; i++)
				{
					*out++ = *ptrFromIfft++ * normFac;
				}

				// Wrap around
				jvxSize ll2_0 = JVX_MIN(args->ll2_out, cfLength);
				jvxSize ll2_1 = args->ll2_out - ll2_0;

				// Reset the pointer for input to beginning of buffer
				ptrFromIfft = nHdl->firfft.ram.out;

				for (i = 0; i < ll2_0; i++)
				{
					jvxData valNewFromIfft = *ptrFromIfft++;
					jvxData valNewFromCfBuf = *ptrFromCfBuffer++;
					jvxData valNew = valNewFromIfft * cfNorm_new + valNewFromCfBuf * cfNorm_old;
					valNew *= normFac;
					*out++ = valNew;
					cfNorm_new += nHdl->ram_cf_nout.multiOutChannels.pSampleCrossFade;
					cfNorm_old = 1.0 - cfNorm_new;
				}

				// Non cf buffer
				for (i = 0; i < ll2_1; i++)
				{
					*out++ = *ptrFromIfft++ * normFac;
				}
			}
		}
		else
		{
			// Here, the ifft output is assumed to be already normalized - no normalization required and added
			if (addOnOut_nout[iC])
			{
				// Crossfade area I
				for (i = 0; i < ll1_0; i++)
				{
					jvxData valNewFromIfft = *ptrFromIfft++;
					jvxData valNewFromCfBuf = *ptrFromCfBuffer++;
					jvxData valNew = valNewFromIfft * cfNorm_new + valNewFromCfBuf * cfNorm_old;
					*out++ += valNew;
					cfNorm_new += nHdl->ram_cf_nout.multiOutChannels.pSampleCrossFade;
					cfNorm_old = 1.0 - cfNorm_new;
				}

				// Non-crossfade area
				for (i = 0; i < ll1_1; i++)
				{
					*out++ += *ptrFromIfft++;
				}

				// Wrap around
				jvxSize ll2_0 = JVX_MIN(args->ll2_out, cfLength);
				jvxSize ll2_1 = args->ll2_out - ll2_0;

				// Reset the pointer for input to beginning of buffer
				ptrFromIfft = nHdl->firfft.ram.out;

				for (i = 0; i < ll2_0; i++)
				{
					jvxData valNewFromIfft = *ptrFromIfft++;
					jvxData valNewFromCfBuf = *ptrFromCfBuffer++;
					jvxData valNew = valNewFromIfft * cfNorm_new + valNewFromCfBuf * cfNorm_old;
					*out++ += valNew;
					cfNorm_new += nHdl->ram_cf_nout.multiOutChannels.pSampleCrossFade;
					cfNorm_old = 1.0 - cfNorm_new;
				}

				// Non cf buffer
				for (i = 0; i < ll2_1; i++)
				{
					*out++ += *ptrFromIfft++ ;
				}
			}
			else
			{

				// Crossfade area I
				for (i = 0; i < ll1_0; i++)
				{
					jvxData valNewFromIfft = *ptrFromIfft++;
					jvxData valNewFromCfBuf = *ptrFromCfBuffer++;
					jvxData valNew = valNewFromIfft * cfNorm_new + valNewFromCfBuf * cfNorm_old;
					*out++ = valNew;
					cfNorm_new += nHdl->ram_cf_nout.multiOutChannels.pSampleCrossFade;
					cfNorm_old = 1.0 - cfNorm_new;
				}

				// Non-crossfade area
				for (i = 0; i < ll1_1; i++)
				{
					*out++ = *ptrFromIfft++;
				}

				// Wrap around
				jvxSize ll2_0 = JVX_MIN(args->ll2_out, cfLength);
				jvxSize ll2_1 = args->ll2_out - ll2_0;

				// Reset the pointer for input to beginning of buffer
				ptrFromIfft = nHdl->firfft.ram.out;

				for (i = 0; i < ll2_0; i++)
				{
					jvxData valNewFromIfft = *ptrFromIfft++;
					jvxData valNewFromCfBuf = *ptrFromCfBuffer++;
					jvxData valNew = valNewFromIfft * cfNorm_new + valNewFromCfBuf * cfNorm_old;
					*out++ = valNew;
					cfNorm_new += nHdl->ram_cf_nout.multiOutChannels.pSampleCrossFade;
					cfNorm_old = 1.0 - cfNorm_new;
				}

				// Non cf buffer
				for (i = 0; i < ll2_1; i++)
				{
					*out++ = *ptrFromIfft++;
				}
			}
		}

		// Prepare crossfade part for next frame
		cfLength = nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength;

		// Here, copy the sample overhead to temp buffer								
		jvxSize ll3 = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - args->newPhase, cfLength);
		jvxSize ll4 = cfLength - ll3;
		ptrFromIfft = nHdl->firfft.ram.out;
		ptrFromIfft = ptrFromIfft + args->newPhase;
		jvxData* ptrToCfBuffer = nHdl->ram_cf_nout.multiOutChannels.out_cf_bufs[iC];

		// Non-crossfade area - already add the ft decoder constant
		for (i = 0; i < ll3; i++)
		{
			*ptrToCfBuffer++ = *ptrFromIfft++;
		}

		// Second part if necessary
		ptrFromIfft = nHdl->firfft.ram.out;
		for (i = 0; i < ll4; i++)
		{
			*ptrToCfBuffer++ = *ptrFromIfft++;
		}

	}

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_firfft_cf_nin_nout_process_ifcf(jvx_firfft* hdl, jvxData** inArg, jvxData** outArg, jvxCBool* addOnOut_nout)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	
	jvxDataCplx* firW = NULL;
	// jvxData* out_src = NULL;
	jvx_firfft_cf_nout_rt_args funcArgs;

	if (hdl->prv)
	{
		jvx_firfft_cf_nout_prv* nHdl = (jvx_firfft_cf_nout_prv*)hdl->prv;
		assert(nHdl->firfft.tp == JVX_FIRFFT_PRV_TYPE_CF_NOUT);
		
		// This version to be run only in case of crossfade
		// assert(nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength);

		jvx_firfft_cf_nout_prmSync* nChannels = hdl->sync.ext;

		funcArgs.newPhase = (nHdl->firfft.ram.phase + nHdl->firfft.init_cpy.bsize - nHdl->firfft.ram.outoffset) % nHdl->firfft.derived_cpy.szFftValue;
		
		// Phase shift between output and input - causing a delay!!
		funcArgs.outphase = (nHdl->firfft.ram.phase + nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.outoffset) % nHdl->firfft.derived_cpy.szFftValue;

		funcArgs.ll1_in = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - nHdl->firfft.ram.phase, nHdl->firfft.init_cpy.bsize);
		funcArgs.ll2_in = nHdl->firfft.init_cpy.bsize - funcArgs.ll1_in;
		funcArgs.ll1_out = JVX_MIN(nHdl->firfft.derived_cpy.szFftValue - funcArgs.outphase, nHdl->firfft.init_cpy.bsize);
		funcArgs.ll2_out = nHdl->firfft.init_cpy.bsize - funcArgs.ll1_out;

		if (nHdl->ram_cf_nout.multiOutChannels.cBufferFadeLength)
		{
			if (nHdl->ram_cf_nout.multiInChannels.nChannelsIn > 1)
			{
				jvxSize cntOut = 0;
				for (i = 0; i < nHdl->ram_cf_nout.multiInChannels.nChannelsIn; i++)
				{
					jvx_firfft_cf_nout_process_core_ifcf(nHdl, nChannels, inArg[i], outArg, addOnOut_nout, nHdl->ram_cf_nout.multiInChannels.in_bufs[i],
						&funcArgs, nHdl->ram_cf_nout.multiInChannels.in_ffts[i], cntOut, cntOut + nChannels->stride);
					cntOut += nChannels->stride;
				}
			}
			else
			{
				jvx_firfft_cf_nout_process_core_ifcf(nHdl, nChannels, inArg[0], outArg, addOnOut_nout, nHdl->firfft.ram.in, &funcArgs, nHdl->firfft.ram.corefft, 0, nChannels->N);
			}
		}
		else
		{
			if (nHdl->ram_cf_nout.multiInChannels.nChannelsIn > 1)
			{
				jvxSize cntOut = 0;
				for (i = 0; i < nHdl->ram_cf_nout.multiInChannels.nChannelsIn; i++)
				{
					jvx_firfft_cf_nout_process_core(nHdl, nChannels, inArg[i], outArg, addOnOut_nout, nHdl->ram_cf_nout.multiInChannels.in_bufs[i],
						&funcArgs, nHdl->ram_cf_nout.multiInChannels.in_ffts[i], cntOut, cntOut + nChannels->stride);
					cntOut += nChannels->stride;
				}
			}
			else
			{
				jvx_firfft_cf_nout_process_core(nHdl, nChannels, inArg[0], outArg, addOnOut_nout, nHdl->firfft.ram.in, &funcArgs, nHdl->firfft.ram.corefft, 0, nChannels->N);
			}
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
