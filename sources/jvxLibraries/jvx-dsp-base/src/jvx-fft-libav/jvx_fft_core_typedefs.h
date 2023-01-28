#ifndef __JVX_FFT_CORE_TYPEDEFS__H__
#define __JVX_FFT_CORE_TYPEDEFS__H__

#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "jvx_helpers.h"
#include "jvx_allocate.h"

#define FFT_FLOAT 1
#pragma message "WARNING: LGPL source code included"
#include "libavcodec/avfft.h"

// libav fft is inplace
#define JVX_FFT_IFFT_INPLACE

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

	jvx_fft_ifft_core_global_common* ref_global_hdl;

} jvx_fft_ifft_core_common;

// ===========================================================================
// Specific storage of FFT runtime data
// ===========================================================================
typedef struct
{
	jvx_fft_ifft_core_common common;
	RDFTContext* av_fft_hdl;

    jvxData* input;
    jvxDataCplx* output;
} jvx_fft_core_real_2_complex;

typedef struct
{
	jvx_fft_ifft_core_common common;
	FFTContext* av_fft_hdl;

	jvxDataCplx* input;
	jvxDataCplx* output;
} jvx_fft_core_complex_2_complex;

typedef struct
{
	jvx_fft_ifft_core_common common;
	RDFTContext* av_fft_hdl;

    jvxDataCplx* input;
    jvxData* output;
} jvx_ifft_core_complex_2_real;

typedef struct
{
	jvx_fft_ifft_core_common common;
	FFTContext* av_fft_hdl;

	jvxDataCplx* input;
	jvxDataCplx* output;
} jvx_ifft_core_complex_2_complex;

#endif
