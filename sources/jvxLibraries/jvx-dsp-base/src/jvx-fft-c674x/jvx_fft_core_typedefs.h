#ifndef __JVX_FFT_CORE_TYPEDEFS__H__
#define __JVX_FFT_CORE_TYPEDEFS__H__

#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "jvx_dsp_base.h"

#include <ti/dsplib/dsplib.h>

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
#define NUM_TWIDDLE_ARRAYS (JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE)
typedef struct 
{
	jvxInt32 fftSizeLog;
	jvxInt32 refCount;
	jvxData *twiddleFactors[NUM_TWIDDLE_ARRAYS];
	jvxDataCplx *A[NUM_TWIDDLE_ARRAYS];
	jvxDataCplx *B[NUM_TWIDDLE_ARRAYS];
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
    
    // Sometimes necessary when JVX_FFT_IFFT_PRESERVE_INPUT is set.
    void* shadow_buffer;

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
	jvxData* input;
	jvxDataCplx* output;
	jvxDataCplx* temp;

} jvx_fft_core_real_2_complex;

typedef struct
{
	jvx_fft_ifft_core_common common;

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
	jvxDataCplx* input;
	jvxData* output;
	jvxDataCplx* temp;
} jvx_ifft_core_complex_2_real;

typedef struct
{
	jvx_fft_ifft_core_common common;

#ifdef JVX_FFT_IFFT_INPLACE
	jvxDataCplx* inputOutput;
#else
	jvxDataCplx* input;
	jvxDataCplx* output;
#endif
} jvx_ifft_core_complex_2_complex;



#endif
