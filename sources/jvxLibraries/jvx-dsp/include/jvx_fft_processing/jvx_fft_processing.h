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

#ifndef __JVX_FFT_PROCESSING_H__
#define __JVX_FFT_PROCESSING_H__

#include "jvx_fft_tools/jvx_fft_tools.h"

typedef struct
{
	jvxFFTSize fftType;
	jvxFftTools_fwkType	fftFrameworkType;
	jvxSize hopsize;
	jvxSize buffersize;
	jvxSize ifcf_length_max;
	jvxSize ir_length_max;
	jvxCBool allocateProcessingFft;
	jvxCBool allocateAnalysisFft;
	jvxCBool allocateWeightCorrection;
	jvx_windowType desiredWindowTypeWeightCorrection;
} jvx_fft_signal_processing_prmInit;

typedef struct
{
	jvxSize fftsize;
	jvxSize numSpectrumCoeffs;
	jvxSize desiredNumFilterCoeffs;
	jvxDataCplx* outFftBuffer;
	jvxDataCplx* outFftBufferAnalysis;
	jvxDataCplx* weight_correction_in;
	jvxDataCplx* weight_correction_out;
} jvx_fft_signal_processing_prmDerived;

typedef struct
{
	jvxCBool auto_aliasing;
	jvxCBool employ_antialias_weight_processing;
	jvxSize ifcf_length;
} jvx_fft_signal_processing_prmAsync;

typedef struct
{
	jvxInt32 percentAnalysisWinFftSize; 
	jvx_windowType wintype; 
} jvx_fft_signal_processing_prmSync;

typedef struct
{
	jvx_fft_signal_processing_prmInit cfg;
	jvx_fft_signal_processing_prmDerived derived;
	jvx_fft_signal_processing_prmAsync async;
	jvx_fft_signal_processing_prmSync sync;

	jvxHandle* prv;
} jvx_fft_signal_processing;

// ====================================================================================0

JVX_DSP_LIB_BEGIN

jvxDspBaseErrorType jvx_fft_signal_processing_initCfg(jvx_fft_signal_processing* signal_proc);
jvxDspBaseErrorType jvx_fft_signal_processing_updateCfg(jvx_fft_signal_processing_prmDerived* H, jvx_fft_signal_processing_prmInit* cfg);

jvxDspBaseErrorType jvx_fft_signal_processing_update(jvx_fft_signal_processing* signal_proc, jvxUInt16 whatToUpdate, jvxCBool do_set);

jvxDspBaseErrorType jvx_fft_signal_processing_prepare(jvx_fft_signal_processing* signal_proc, jvxHandle* global_hdl);
	
jvxDspBaseErrorType jvx_fft_signal_processing_input(jvx_fft_signal_processing* signal_proc, jvxData* in, jvxSize hopsize);
jvxDspBaseErrorType jvx_fft_signal_processing_output(jvx_fft_signal_processing* signal_proc, jvxData* out, jvxData* weights, jvxSize hopsize);

jvxDspBaseErrorType jvx_fft_signal_processing_postprocess(jvx_fft_signal_processing* signal_proc);

JVX_DSP_LIB_END

#endif