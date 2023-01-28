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
 
 #ifndef __JVX_FFT_CORE_TYPEDEFS__H__
#define __JVX_FFT_CORE_TYPEDEFS__H__

#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "jvx_dsp_base.h"

#ifdef JVX_FFT_APPLE
#include <Accelerate/Accelerate.h>
#else
#include "fftw3.h"
#endif

/** Define if FFT is inplace or out of place
 * inplace currently not an option.
 */
/* #define JVX_FFT_IFFT_INPLACE */

typedef enum
{
	JVX_FFT_IFFT_BUFFER_NOTSET,
	JVX_FFT_IFFT_BUFFER_EXTERNAL,
	JVX_FFT_IFFT_BUFFER_ALLOCATED
} jvxFftIfftBufferOrigin;

// ===========================================================================
// Core FFT functionality - global handle
// ===========================================================================
typedef struct 
{
#ifdef JVX_FFT_APPLE

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
	FFTSetupD setupFFT;
#else
	FFTSetup setupFFT;
#endif

#endif

	jvxInt32 fftSizeLog;
	jvxInt32 refCount;
} jvx_fft_ifft_core_global_common;


// ===========================================================================
// Common storage of FFT parameters
// ===========================================================================
typedef struct
{
	jvx_fft_ifft_core_global_common* glob_ptr;

	jvxFftIfftOperate operate;

	jvxFftIfftBufferOrigin inputOrigin;

	jvxFftIfftBufferOrigin outputOrigin;

	//! FFT specific paramteters
	struct
	{
		//! Store FFT type (fft/ifft/real/complex)
		jvxFftTools_coreFftType coreFftType;

		//! FFT type is only related to FFT length. Only power of 2 allowed
		jvxFFTSize fftType;

		//! The fft size as derived from fftType
		jvxInt32 fftSize;

        //! The log (base 2) of the fftSize
        jvxInt32 fftSizeLog;

		// Indicate if fft is real radix 2 (2^N)
		jvxCBool acceleratedFft;

	} fftParameters;

#ifndef JVX_FFT_APPLE

	//! The FFT handle for fftw
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
	fftw_plan planFFT;
#else
	fftwf_plan planFFT;
#endif

#endif

	jvx_fft_ifft_core_global_common* ref_global_hdl;

} jvx_fft_ifft_core_common;

// ===========================================================================
// Specific storage of FFT runtime data
// ===========================================================================
typedef struct
{
	jvx_fft_ifft_core_common common;

#ifdef JVX_FFT_APPLE

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
		DSPDoubleSplitComplex splitComplex;
#else
        DSPSplitComplex splitComplex;
#endif

#endif

	jvxData* input;
	jvxDataCplx* output;

} jvx_fft_core_real_2_complex;

typedef struct
{
	jvx_fft_ifft_core_common common;

#ifdef JVX_FFT_APPLE

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
		DSPDoubleSplitComplex splitComplex;
#else
        DSPSplitComplex splitComplex;
#endif

#endif

#ifdef JVX_FFT_IFFT_INPLACE
	jvxDataCplx* inputOutput;
#else
	jvxDataCplx* input;
	jvxDataCplx* output;
#endif
} jvx_fft_core_complex_2_complex;

typedef struct
{
	jvx_fft_ifft_core_common common;

#ifdef JVX_FFT_APPLE

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
		DSPDoubleSplitComplex splitComplex;
#else
        DSPSplitComplex splitComplex;
#endif

#endif

	jvxDataCplx* input;
	jvxData* output;
} jvx_ifft_core_complex_2_real;

typedef struct
{
	jvx_fft_ifft_core_common common;

#ifdef JVX_FFT_APPLE

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
		DSPDoubleSplitComplex splitComplex;
#else
        DSPSplitComplex splitComplex;
#endif

#endif

#ifdef JVX_FFT_IFFT_INPLACE
	jvxDataCplx* inputOutput;
#else
	jvxDataCplx* input;
	jvxDataCplx* output;
#endif
} jvx_ifft_core_complex_2_complex;



#endif
