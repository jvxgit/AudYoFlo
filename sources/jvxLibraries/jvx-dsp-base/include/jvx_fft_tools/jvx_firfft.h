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

typedef struct 
{
	jvxSize lFir;
	jvxSize lFft;
	jvxSize delayFir;
	jvxData* fir;
	jvxSize bsize;
	jvxFirFftOperType type;
	struct jvxMemRefs* allocators;
} jvx_firfft_prmInit;

typedef struct
{
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

jvxDspBaseErrorType jvx_firfft_init(jvx_firfft* hdl, jvxHandle* fftGlobalCfg);

jvxDspBaseErrorType jvx_firfft_process(jvx_firfft* hdl, jvxData* in, jvxData* out);

jvxDspBaseErrorType jvx_firfft_process_iplace(jvx_firfft* hdl, jvxData* inout);

jvxDspBaseErrorType jvx_firfft_process_mix(jvx_firfft* hdl, jvxData* in, jvxData* out, jvxCBool isFirst, jvxData* workbuf);

jvxDspBaseErrorType jvx_firfft_process_mix_iplace(jvx_firfft* hdl, jvxData* in, jvxData* out, jvxCBool isFirst);

jvxDspBaseErrorType jvx_firfft_terminate(jvx_firfft* hdl);

jvxDspBaseErrorType jvx_firfft_update(jvx_firfft* hdl, jvxInt16 whatToUpdate, jvxCBool do_set);

JVX_DSP_LIB_END

#endif
