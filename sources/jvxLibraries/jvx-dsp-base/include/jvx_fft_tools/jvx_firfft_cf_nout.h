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
 
#ifndef __JVX_FIRFFT_FLTER_CF_NOUT_H__
#define __JVX_FIRFFT_FLTER_CF_NOUT_H__

#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_tools/jvx_firfft.h"

JVX_DSP_LIB_BEGIN

typedef struct
{
	jvxDataCplx** firWN;
	jvxSize N;
} jvx_firfft_cf_nout_prmSync;

jvxDspBaseErrorType jvx_firfft_cf_nout_init(jvx_firfft* hdl, jvxHandle* fftCfgHdl, jvxSize nChannels);
jvxDspBaseErrorType jvx_firfft_cf_nout_terminate(jvx_firfft* hdl);

//void jvx_firfft_cf_nout_compute_weights(jvx_firfft* hdl, jvxData* fir, jvxSize lFir);
//void jvx_firfft_cf_nout_copy_weights(jvx_firfft* hdl, jvxDataCplx* firW, jvxSize lFirW);

jvxDspBaseErrorType jvx_firfft_cf_nout_process(jvx_firfft* hdl, jvxData* inArg, jvxData** outArg, jvxCBool addOnOut);
jvxDspBaseErrorType jvx_firfft_cf_nout_process_update_weights(jvx_firfft* hdl, jvxData* inArg, jvxData** outArg, jvxDataCplx** newWeights, jvxCBool addOnOut);

JVX_DSP_LIB_END

#endif