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

#include "jvx_fft_tools/jvx_firfft_prv.h"	

jvxDspBaseErrorType jvx_firfft_initCfg(jvx_firfft* init)
{
	jvxDspBaseErrorType resL = JVX_DSP_ERROR_UNSUPPORTED;
	init->init.bsize = 128;
	init->init.fir = NULL;
	init->init.lFir = 1;
	init->init.lFft = JVX_SIZE_UNSELECTED;
	init->init.delayFir = JVX_SIZE_UNSELECTED;
	init->init.type = JVX_FIRFFT_SYMMETRIC_FIR;
	init->init.allocators = NULL;

	jvx_firfft_resetDerived(init);

	init->sync.firW = NULL;

	init->prv = NULL;

	resL = jvx_firfft_update(init, JVX_DSP_UPDATE_INIT, true);

	return resL;
}

jvxDspBaseErrorType jvx_firfft_init(jvx_firfft* hdl, jvxHandle* fftGlobalCfg)
{
	if (hdl->prv == NULL)
	{
		jvx_firfft_prv* nHdl = NULL;
		jvxSize N = 0, i = 0;
		jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;

		if (hdl->init.allocators)
		{
			nHdl = hdl->init.allocators->alloc(sizeof(jvx_firfft_prv), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);
		}
		else
		{
			JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(nHdl, jvx_firfft_prv);
		}

		hdl->prv = nHdl;

		jvx_firfft_update(hdl, JVX_DSP_UPDATE_INIT, true); // this sets all derived values

		resL = jvx_create_fft_ifft_global(&nHdl->ram.fftGlob, nHdl->derived_cpy.szFft, fftGlobalCfg);
		assert(resL == JVX_DSP_NO_ERROR);

		// Let us allocate input and output
		resL = jvx_create_ifft_complex_2_real(&nHdl->ram.coreifft,
			nHdl->ram.fftGlob, nHdl->derived_cpy.szFft,
			&nHdl->ram.spec, &nHdl->ram.out, &N,
			JVX_FFT_IFFT_PRESERVE_INPUT,
			NULL, NULL, 0);
		assert(resL == JVX_DSP_NO_ERROR);
		//assert(N == nHdl->derived_cpy.szFftValue);

		// Reuse input from ifft for fft output
		resL = jvx_create_fft_real_2_complex(&nHdl->ram.corefft,
			nHdl->ram.fftGlob, nHdl->derived_cpy.szFft,
			&nHdl->ram.in,
			NULL, &N, JVX_FFT_IFFT_PRESERVE_INPUT,
			NULL, nHdl->ram.spec, 0);
		assert(resL == JVX_DSP_NO_ERROR);
		//assert(N == nHdl->derived_cpy.szFftValue);

		if (nHdl->init_cpy.type == JVX_FIRFFT_SYMMETRIC_FIR)
		{
			assert((nHdl->init_cpy.lFir % 2) == 1); // Make sure it is symmetric
			nHdl->ram.outoffset = nHdl->derived_cpy.delay;

			// Copy such that we get a zero phase filter
			nHdl->ram.in[0] = nHdl->init_cpy.fir[nHdl->ram.outoffset];
			for (i = 0; i < nHdl->ram.outoffset; i++)
			{
				nHdl->ram.in[nHdl->derived_cpy.szFftValue - i - 1] = nHdl->init_cpy.fir[nHdl->ram.outoffset - i - 1];
				nHdl->ram.in[i + 1] = nHdl->init_cpy.fir[nHdl->ram.outoffset + i + 1];
			}
		}
		else
		{
			nHdl->ram.outoffset = 0;
			memcpy(nHdl->ram.in, nHdl->init_cpy.fir, sizeof(jvxData) * nHdl->init_cpy.lFir);
		}

		jvx_execute_fft(nHdl->ram.corefft);

		hdl->sync.firW = NULL;		
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(hdl->sync.firW, jvxDataCplx, hdl->derived.lFirW);
		for (i = 0; i < hdl->derived.lFirW; i++)
		{
			hdl->sync.firW[i].re = nHdl->ram.spec[i].re;
			hdl->sync.firW[i].im = nHdl->ram.spec[i].im;
		}
		nHdl->ram.phase = 0;

		nHdl->ram.normFactor = 1.0 / (jvxData)nHdl->derived_cpy.szFftValue;

		// Cleanup the fft buffer for processing
		memset(nHdl->ram.in, 0, sizeof(jvxData) * nHdl->derived_cpy.szFftValue);

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;	
}

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

jvxDspBaseErrorType jvx_firfft_terminate(jvx_firfft* hdl)
{
	if (hdl->prv)
	{
		jvx_firfft_prv* nHdl = (jvx_firfft_prv*)hdl->prv;

		if (nHdl->ram.corefft)
		{
			jvx_destroy_fft(nHdl->ram.corefft);
			nHdl->ram.corefft = NULL;
			jvx_destroy_ifft(nHdl->ram.coreifft);
			nHdl->ram.coreifft = NULL;

			jvx_destroy_fft_ifft_global(nHdl->ram.fftGlob);
			nHdl->ram.fftGlob = NULL;

			JVX_DSP_SAFE_DELETE_FIELD(hdl->sync.firW);
			hdl->sync.firW = NULL;

			if (hdl->init.allocators)
			{
				hdl->init.allocators->dealloc((jvxHandle**)&nHdl, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
			}
			else
			{
				JVX_DSP_SAFE_DELETE_OBJECT(nHdl);
			}
			hdl->prv = NULL;

			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType jvx_firfft_update(jvx_firfft* hdl, jvxInt16 whatToUpdate, jvxCBool do_set)
{
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvx_firfft_prv* nHdl = (jvx_firfft_prv*)hdl->prv;
	jvxSize fftszmin = 0;

	switch (whatToUpdate)
	{
	case JVX_DSP_UPDATE_INIT:
		if (do_set)
		{
			fftszmin = (hdl->init.lFir + hdl->init.bsize - 1);
			if (JVX_CHECK_SIZE_SELECTED(hdl->init.lFft))
			{
				fftszmin = JVX_MAX(hdl->init.lFft, fftszmin);
			}

			resL = jvx_get_nearest_size_fft(&hdl->derived.szFft,
				fftszmin,
				JVX_FFT_ROUND_UP, &hdl->derived.szFftValue);
			assert(resL == JVX_DSP_NO_ERROR);

			hdl->derived.lFirW = hdl->derived.szFftValue / 2 + 1;

			switch (hdl->init.type)
			{
			case JVX_FIRFFT_SYMMETRIC_FIR:
				assert((hdl->init.lFir % 2) == 1);
				hdl->derived.delay = hdl->init.lFir / 2;
				break;
			default:
				hdl->derived.delay = 0;
				if (JVX_CHECK_SIZE_SELECTED(hdl->init.delayFir))
				{
					hdl->derived.delay = hdl->init.delayFir;
				}
			}

			if (nHdl)
			{
				nHdl->init_cpy = hdl->init;
				nHdl->derived_cpy = hdl->derived;
			}
		}
		else
		{
			if (nHdl)
			{
				hdl->init = nHdl->init_cpy;
				hdl->derived = nHdl->derived_cpy;
			}
		}
		break;
	}
	return JVX_DSP_NO_ERROR;
}

void
jvx_firfft_resetDerived(jvx_firfft* init)
{
	init->derived.delay = 0;
	init->derived.szFft = JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE;
	init->derived.szFftValue = 0;
	init->derived.lFirW = 0;
}
