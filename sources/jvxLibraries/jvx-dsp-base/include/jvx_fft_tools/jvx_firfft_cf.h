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
 
#ifndef __JVX_FIRFFT_FLTER_CF_H__
#define __JVX_FIRFFT_FLTER_CF_H__

#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_tools/jvx_firfft.h"

JVX_DSP_LIB_BEGIN

jvxDspBaseErrorType jvx_firfft_cf_init(jvx_firfft* hdl);
jvxDspBaseErrorType jvx_firfft_cf_terminate(jvx_firfft* hdl);

jvxDspBaseErrorType jvx_firfft_cf_process(jvx_firfft* hdl, jvxData* inArg, jvxData* outArg, jvxCBool addOnOut);
jvxDspBaseErrorType jvx_firfft_cf_process_update_weights(jvx_firfft* hdl, jvxData* inArg, jvxData* outArg, jvxDataCplx* newWeights, jvxCBool addOnOut);

void jvx_firfft_cf_compute_weights(jvx_firfft* hdl, jvxData* fir, jvxSize lFir);
void jvx_firfft_cf_copy_weights(jvx_firfft* hdl, jvxDataCplx* firW, jvxSize lFirW);

/*
jvxDspBaseErrorType jvx_firfft_process(jvx_firfft* hdl, jvxData* in, jvxData* out);

jvxDspBaseErrorType jvx_firfft_process_iplace(jvx_firfft* hdl, jvxData* inout);

jvxDspBaseErrorType jvx_firfft_process_mix(jvx_firfft* hdl, jvxData* in, jvxData* out, jvxCBool isFirst, jvxData* workbuf);

jvxDspBaseErrorType jvx_firfft_process_mix_iplace(jvx_firfft* hdl, jvxData* in, jvxData* out, jvxCBool isFirst);
*/

// jvxDspBaseErrorType jvx_firfft_update(jvx_firfft* hdl, jvxInt16 whatToUpdate, jvxCBool do_set);

JVX_DSP_LIB_END

#endif
