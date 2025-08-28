#ifndef __JVX_FFT_CORE_TYPEDEFS__H__
#define __JVX_FFT_CORE_TYPEDEFS__H__

#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "jvx_dsp_base.h"

#include <filter.h>
#include <complex.h>

/** Define if FFT is inplace or out of place
 * inplace currently not an option.
 */
/* #define JVX_FFT_IFFT_INPLACE */

// ===========================================================================
// Core FFT functionality - global handle
// ===========================================================================
typedef struct 
{
	jvxInt32 fftSizeLog;
	jvxInt32 refCount;

	complex_float pm* twiddle_facs;
	jvxSize twiddle_fft_size;

	jvxSize max_fft_size;
	jvxData* work_buffer; // There are FFT_SIZE float values forMAX size FFT
	jvxCBool allocatedAllocator;

} jvx_fft_ifft_core_global_common;


// ===========================================================================
// Common storage of FFT parameters
// ===========================================================================
typedef struct
{
	jvx_fft_ifft_core_global_common* glob_ptr;
	jvxFftIfftOperate operate;

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
	jvxCBool allocatedAllocator;
	jvxSize twStride;
} jvx_fft_ifft_core_common;

// ===========================================================================
// Specific storage of FFT runtime data
// ===========================================================================
typedef struct
{
	jvx_fft_ifft_core_common common;
	jvxData* input;
	jvxDataCplx* output;
} jvx_fft_core_real_2_complex;

typedef struct
{
	jvx_fft_ifft_core_common common;
	jvxDataCplx* input;
	jvxDataCplx* output;
} jvx_fft_core_complex_2_complex;

typedef struct
{
	jvx_fft_ifft_core_common common;
	jvxDataCplx* input;
	jvxData* output;
} jvx_ifft_core_complex_2_real;

typedef struct
{
	jvx_fft_ifft_core_common common;
	jvxDataCplx* input;
	jvxDataCplx* output;
} jvx_ifft_core_complex_2_complex;



#endif
