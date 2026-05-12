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
 
#ifndef __JVX_FIRFFT_FLTER_H__
#define __JVX_FIRFFT_FLTER_H__

#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_fft_core.h"

typedef enum
{
	// This function expects a symmetric FIR filter
	JVX_FIRFFT_SYMMETRIC_FIR = 0,

	// This function allows all kinds of transfer functions
	JVX_FIRFFT_FLEXIBLE_FIR = 1,

} jvxFirFftOperType;

typedef enum
{
	JVX_FIRFFT_IMPLEMENTATION_SIMPLE,
	JVX_FIRFFT_IMPLEMENTATION_CF
} jvxFirFftImplementationType;

typedef struct 
{
	jvxSize lFir;
	jvxSize lFft;
	jvxSize delayFir;
	jvxData* fir;
	jvxSize bsize;
	jvxFirFftOperType type;
	jvxSize cBufferFadeLength;
	struct jvxMemRefs* allocators;
	jvxCBool prepareReuse;
	jvxCBool prio_low_mem_gains;
} jvx_firfft_prmInit;

typedef struct
{
	jvxSize lFirEff;
	jvxFFTSize szFft;
	jvxSize szFftValue;
	jvxSize delay;
	jvxSize lFirW;
} jvx_firfft_prmDerived;

typedef struct
{
	jvxDataCplx* firW;
	jvxHandle* ext;
} jvx_firfft_prmSync;

typedef struct
{
	jvx_firfft_prmInit init;
	jvx_firfft_prmDerived derived;
	jvx_firfft_prmSync sync;
	jvxHandle* prv;
} jvx_firfft;

JVX_DSP_LIB_BEGIN

jvxDspBaseErrorType jvx_firfft_initCfg(jvx_firfft* init);

jvxDspBaseErrorType jvx_firfft_init(jvx_firfft* hdl, jvxFFTGlobal* fftGlobCfg);

jvxDspBaseErrorType jvx_firfft_process(jvx_firfft* hdl, jvxData* in, jvxData* out);

jvxDspBaseErrorType jvx_firfft_process_iplace(jvx_firfft* hdl, jvxData* inout);

jvxDspBaseErrorType jvx_firfft_process_mix(jvx_firfft* hdl, jvxData* in, jvxData* out, jvxCBool isFirst, jvxData* workbuf);

jvxDspBaseErrorType jvx_firfft_process_mix_iplace(jvx_firfft* hdl, jvxData* in, jvxData* out, jvxCBool isFirst);

jvxDspBaseErrorType jvx_firfft_terminate(jvx_firfft* hdl, jvxFFTGlobal* fftGlobCfg);

jvxDspBaseErrorType jvx_firfft_update(jvx_firfft* hdl, jvxInt16 whatToUpdate, jvxCBool do_set);

jvxDspBaseErrorType jvx_firfft_get_global_handle(jvx_firfft* hdl, jvxFFTGlobal** fftGlobCfg);

// Helper
jvxSize jvx_firfft_precompute_firl(jvxSize lFirMin, jvxSize bsize, jvxSize lFft, jvxSize cBufferFadeLength, jvxCBool allowLargerFir, jvxFFTSize* fftSizeReturn);

JVX_DSP_LIB_END

#endif
