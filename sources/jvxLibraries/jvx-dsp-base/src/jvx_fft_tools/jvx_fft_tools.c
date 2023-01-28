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
 
 /**
 * File: jvx_fft_tools.c
 * Author: Hauke Krüger
 * Desccription: Framework for all around FFT processing:
 * Create kernel FFT objects, realize the different FFT processing frameworks:
 * a) Single Window Overlap-Add: SW_OLA
 * b) Double Window Overlap-Add: DW_OLA (sqrt Hann Window)
 * c) Overlap-Save: OLS
 * d) Overla-Save Crossfade: OLS_CF
 *///=================================================

#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_fft_tools.h"
#include "jvx_fft_core_typedefs.h"

 // ===========================================================
 // Macros definitions for compile time configurations
 // ===========================================================

 // Check that the right type of FFT object is passed (fft/ifft/real/complex)
#define ASSERT_FFT_CORE

// If desired the FFT buffer can be set to zero for zero padding before
// FFT is proceded. Only in case that the real input buffer for FFT is overwritten by core
#define SET_FFT_BUFFER_ZERO_BEFORE_TRANSFORM


// ==========================================================================0
// ==========================================================================0

//! Paramaters which FFT and IFFT frameworks have in common
typedef struct
{
	//! Handle to core FFT/IFFT
	jvx_fft_ifft_core_common* hdlFftIfft;

	//! Framework type
	jvxFftTools_fwkType fwkType;

	//! Input (FFT) or output (IFFT) framesize
	jvxInt32 framesize;

	//! FFT size
	jvxInt32 fftSize;

	//! Circular buffer pointer
	struct
	{
		jvxSize idxBufferRW;
	} control;

	//! Reference to spectrum originating from FFT/IFFT kernel
	struct
	{
		jvxDataCplx* spectrum;
	} linkedram;
} jvxFftTools_fwkFftIfft_commonParameters;

//! Common parameters for IFFT frameworks, extended by normalization factor and delay
typedef struct
{
	//! Common parameters for FFT/IFFT frameworks
	jvxFftTools_fwkFftIfft_commonParameters common;

	//! Normalization factor
	jvxData normalization;

	//! Delay related to FFT plus IFFT
	jvxData delay;
} jvxFftTools_fwkIfft_commonParameters;

//! Parameters for SW_OLA and DW_OLA, FFT part
typedef struct
{
	//! Common parameters
	jvxFftTools_fwkFftIfft_commonParameters common;

	//! Buffersize used internally
	jvxInt32 buffersize;

	//! ALlocated additional RAM
	struct
	{
		//! Window function FFT
		jvxData* window;

		//! Buffer to hold multiples frames
		jvxData* buffer;

		//! Buffer to whichthe application writes data prior to processing
		jvxData* inputReal;
	} ram;

	//! Linked data
	struct
	{
		//! Input data buffer originating from FFT kernel
		jvxData* fftbuffer;
	} linkedram;
} jvx_tools_hdl_fft_fwk_dw_sw_ola;

//! Parameters for SW_OLA and DW_OLA, FFT part
typedef struct
{
	//! Common parameters
	jvxFftTools_fwkFftIfft_commonParameters common;

	//! ALlocated additional RAM
	struct
	{
		//! Buffer to whichthe application writes data prior to processing
		jvxData* inputReal;
	} ram;

	//! Linked data
	struct
	{
		//! Input data buffer originating from FFT kernel
		jvxData* fftbuffer;
	} linkedram;
} jvx_tools_hdl_fft_fwk_ola_cf;

//! Parameters for OLS and OLS_CF FFT
typedef struct
{
	//! Common parameters
	jvxFftTools_fwkFftIfft_commonParameters common;

	//! Allocated RAM
	struct
	{
		//! Buffer to allow application to write data
		jvxData* inputReal;
	} ram;

	//! Linked resources
	struct
	{
		//! FFT buffer from FFT kernel
		jvxData* fftbuffer;
	} linkedram;
} jvx_tools_hdl_fft_fwk_ols_ols_cf;

//! Parameters for SW_OLA IFFT
typedef struct
{
	//! Common parameters
	jvxFftTools_fwkIfft_commonParameters common;

	//! Allocated RAM
	struct
	{
		//! Buffer from which application reads output
		jvxData* outputReal;

		//! Overlap buffer to superpose the output (buffer)
		jvxData* olabuffer;
	} ram;

	//! Linked resources from IFFT kernel
	struct
	{
		//! Ifft output
		jvxDataCplx* ifftInput;
		jvxData* ifftOutput;
	} linkedram;

	//! Sizes of buffer which is required for computation of normalization factor
	struct
	{
		jvxInt32 buffersize;
	} sizes;
} jvx_tools_hdl_ifft_fwk_sw_ola;

//! Parameters for DW_OLA IFFT
typedef struct
{
	//! Common parameters
	jvxFftTools_fwkIfft_commonParameters common;

	//! Allocated memory
	struct
	{
		//! Output buffer
		jvxData* outputReal;

		//! Internally used olabuffer (BUFFERSIZE)
		jvxData* olabuffer;

		//! Output window (sqrt Hann), BUFFERSIZE
		jvxData* window;
	} ram;

	//! Linked resources
	struct
	{
		//! IFFT outpbut buffer from IFFT kernel
		jvxData* fftbuffer;
	} linkedram;

	//! Size of olabuffer and window
	struct
	{
		jvxInt32 buffersize;
	} sizes;
} jvx_tools_hdl_ifft_fwk_dw_ola;

//! Parameters for OLS
typedef struct
{
	//! Common parameters
	jvxFftTools_fwkIfft_commonParameters common;

	//! Allocated resources
	struct
	{
		//! Output to application
		jvxData* outputReal;
	} ram;

	struct
	{
		//! Output buffer from IFFT kernel
		jvxData* fftbuffer;
	} linkedram;
} jvx_tools_hdl_ifft_fwk_ols;

typedef struct
{
	jvx_tools_hdl_ifft_fwk_ols ols;
	jvxData* save_data;
	jvxSize save_data_ll;
} jvx_tools_hdl_ifft_fwk_ols_ifcf;

//! Parameters for OLS
typedef struct
{
	//! Common parameters
	jvxFftTools_fwkIfft_commonParameters common;

	//! Allocated memory
	struct
	{
		//! Output to application
		jvxData* outputReal;

		//! Processed spectrum (with new weights)
		jvxDataCplx* inputCplx_new;

		//! Processed spectrum (with old weights)
		jvxDataCplx* inputCplx_old;

		//! Crossfading window function
		jvxData* window;
	} ram;

	//! Linked resources form IFFT kernel
	struct
	{
		//! IFFT output
		jvxDataCplx* ifftInput;
		jvxDataCplx* ifftOutput;
	} linkedram;
} jvx_tools_hdl_ifft_fwk_ols_cf;

//! Parameters for OLA_CF
typedef struct
{
	//! Common parameters
	jvxFftTools_fwkIfft_commonParameters common;

	//! Allocated memory
	struct
	{
		//! Output to application
		jvxData* outputReal;

		//! Processed spectrum (with new weights)
		jvxDataCplx* inputCplx_new;

		//! Processed spectrum (with old weights)
		jvxDataCplx* inputCplx_old;

		//! Crossfading window function
		jvxData* window;

		jvxData* olabuffer;
	} ram;

	//! Linked resources form IFFT kernel
	struct
	{
		//! IFFT output
		jvxDataCplx* ifftInput;
		jvxDataCplx* ifftOutput;
	} linkedram;
} jvx_tools_hdl_ifft_fwk_ola_cf;

//! Struct to store data for the anti-aliasing processing
typedef struct
{
	//! Number of desired filter coefficients
	jvxInt32 desiredNumFilterCoeffs;

	//! FFT size from kernels
	jvxInt32 fftSize;

	//! Kernel IFFT
	jvxIFFT* ifftCore;

	//! Kernel FFT
	jvxFFT* fftCore;

	//! FFT / IFFT normalization
	jvxData normalization;

	//! Relevant, non-zero part of ir
	jvxInt32 ir_size;

	struct
	{
		jvxData* window;
	} ram;

	struct
	{
		//! Complex input data
		jvxDataCplx* inputCplx;

		//! Complex output data
		jvxDataCplx* outputCplx;

		//! Time domain output from IFFT
		jvxData* timeDomainBuffer_beforeMod;

		//! Time domain input from filter truncation and after FFT
		jvxData* timeDomainBuffer_afterMod;
	} linkedram;

	//! Helping variables that can be computed in advance
	struct
	{
		jvxInt32 ll1;
		jvxInt32 ll2;
		jvxInt32 ll3;
	} help;
} jvx_tools_hdl_mod_weights_no_alias;

// ==========================================================================
// ==========================================================================
// ==========================================================================

static jvxData jvxFftTools_ifftNormFactors[] =
{
	(jvxData)(1.0 / 16.0),
	(jvxData)(1.0 / 32.0),
	(jvxData)(1.0 / 64.0),
	(jvxData)(1.0 / 128.0),
	(jvxData)(1.0 / 256.0),
	(jvxData)(1.0 / 512.0),
	(jvxData)(1.0 / 1024.0),
	(jvxData)(1.0 / 2048.0),
	(jvxData)(1.0 / 4096.0),
	(jvxData)(1.0 / 8192.0)
};

// =======================================================================================

jvxDspBaseErrorType
jvx_fft_tools_init_framework_dw_ola(jvxFFTfwk** hdlOut_fft,
	jvxIFFTfwk** hdlOut_ifft,
	jvxInt32 framesize,
	jvxInt32 buffersize,
	jvxFFT* coreFft,
	jvxIFFT* coreIfft,
	jvxData** inputReal,
	jvxData** outputReal,
	jvxDataCplx** inputComplex,
	jvxDataCplx** outputComplex,
	jvxInt32* numElements_timeDomain,
	jvxInt32* numElements_Spectrum,
	jvxData* delay)
{
	jvxDspBaseErrorType errFft = JVX_DSP_NO_ERROR;
	jvxDataCplx* dummyPtr = NULL;
	jvxDspBaseErrorType errIfft = JVX_DSP_NO_ERROR;

	if (hdlOut_fft)
	{
		jvx_fft_tools_init_framework_fft(hdlOut_fft, JVX_FFT_TOOLS_FWK_DW_OLA,
			framesize, buffersize, coreFft, inputReal,
			outputComplex, numElements_timeDomain, numElements_Spectrum);
	}

	if (hdlOut_ifft)
	{
		jvx_fft_tools_init_framework_ifft(hdlOut_ifft, JVX_FFT_TOOLS_FWK_DW_OLA,
			framesize, buffersize, coreIfft, &dummyPtr,
			inputComplex, outputReal, numElements_Spectrum, numElements_timeDomain,
			delay, 0);
	}

	if ((errFft == JVX_DSP_NO_ERROR) && (errIfft == JVX_DSP_NO_ERROR))
	{
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

/**
 * Allocate two handles: One for the FFT part, one for the IFFT part. Pass in framesize,
 * buffersize, handles for FFT and IFFT. The returned data are buffers for input and output as well as
 * the buffersizes and the handles for FFT and IFFT.
 *///====================================================================================
jvxDspBaseErrorType
jvx_fft_tools_init_framework_sw_ola(jvxFFTfwk** hdlOut_fft,
	jvxIFFTfwk** hdlOut_ifft,
	jvxInt32 framesize,
	jvxInt32 buffersize,
	jvxFFT* coreFft,
	jvxIFFT* coreIfft,
	jvxData** inputReal,
	jvxData** outputReal,
	jvxDataCplx** inputComplex,
	jvxDataCplx** outputComplex,
	jvxInt32* numElements_timeDomain,
	jvxInt32* numElements_Spectrum,
	jvxData* delay)
{
	jvxDspBaseErrorType errFft = JVX_DSP_NO_ERROR;
	jvxDataCplx* dummyPtr = NULL;
	jvxDspBaseErrorType errIfft = JVX_DSP_NO_ERROR;

	if (hdlOut_fft)
	{
		jvx_fft_tools_init_framework_fft(hdlOut_fft, JVX_FFT_TOOLS_FWK_SW_OLA,
			framesize, buffersize, coreFft, inputReal,
			outputComplex, numElements_timeDomain, numElements_Spectrum);
	}

	if (hdlOut_ifft)
	{
		jvx_fft_tools_init_framework_ifft(hdlOut_ifft, JVX_FFT_TOOLS_FWK_SW_OLA,
			framesize, buffersize, coreIfft, &dummyPtr,
			inputComplex, outputReal, numElements_Spectrum, numElements_timeDomain, delay, 0);
	}

	if ((errFft == JVX_DSP_NO_ERROR) && (errIfft == JVX_DSP_NO_ERROR))
	{
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

/**
 * Allocate two handles: One for the FFT part, one for the IFFT part. Pass in framesize,
 * buffersize, handles for FFT and IFFT. The returned data are buffers for input and output as well as
 * the buffersizes and the handles for FFT and IFFT.
 *///====================================================================================
jvxDspBaseErrorType
jvx_fft_tools_init_framework_ola_cf(jvxFFTfwk** hdlOut_fft,
	jvxIFFTfwk** hdlOut_ifft,
	jvxInt32 framesize,
	jvxFFT* coreFft,
	jvxIFFT* coreIfft,
	jvxData** inputReal,
	jvxData** outputReal,
	jvxDataCplx** inputComplex_newWeights,
	jvxDataCplx** inputComplex_oldWeights,
	jvxDataCplx** outputComplex,
	jvxInt32* numElements_timeDomain,
	jvxInt32* numElements_Spectrum,
	jvxData* delay)
{
	jvxDspBaseErrorType errFft = JVX_DSP_NO_ERROR;
	jvxDataCplx* dummyPtr = NULL;
	jvxDspBaseErrorType errIfft = JVX_DSP_NO_ERROR;

	if (hdlOut_fft)
	{
		jvx_fft_tools_init_framework_fft(hdlOut_fft, JVX_FFT_TOOLS_FWK_OLA_CF,
			framesize, 0, coreFft, inputReal, outputComplex,
			numElements_timeDomain, numElements_Spectrum);
	}

	if (hdlOut_ifft)
	{
		jvx_fft_tools_init_framework_ifft(hdlOut_ifft, JVX_FFT_TOOLS_FWK_OLA_CF,
			framesize, 0, coreIfft, inputComplex_oldWeights,
			inputComplex_newWeights,
			outputReal, numElements_Spectrum,
			numElements_timeDomain, delay, 0);
	}

	if ((errFft == JVX_DSP_NO_ERROR) && (errIfft == JVX_DSP_NO_ERROR))
	{
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

/**
 * Allocate two handles: One for the FFT part, one for the IFFT part. Pass in framesize,
 * buffersize, handles for FFT and IFFT. The returned data are buffers for input and output as well as
 * the buffersizes and the handles for FFT and IFFT.
 *///====================================================================================
jvxDspBaseErrorType
jvx_fft_tools_init_framework_ols(jvxFFTfwk** hdlOut_fft,
	jvxIFFTfwk** hdlOut_ifft,
	jvxInt32 framesize,
	jvxFFT* coreFft,
	jvxIFFT* coreIfft,
	jvxData** inputReal,
	jvxData** outputReal,
	jvxDataCplx** inputComplex,
	jvxDataCplx** outputComplex,
	jvxInt32* numElements_timeDomain,
	jvxInt32* numElements_Spectrum,
	jvxData* delay)
{
	jvxDspBaseErrorType errFft = JVX_DSP_NO_ERROR;
	jvxDataCplx* dummyPtr = NULL;
	jvxDspBaseErrorType errIfft = JVX_DSP_NO_ERROR;

	if (hdlOut_fft)
	{
		jvx_fft_tools_init_framework_fft(hdlOut_fft, JVX_FFT_TOOLS_FWK_OLS,
			framesize, 0, coreFft, inputReal, outputComplex,
			numElements_timeDomain, numElements_Spectrum);
	}

	if (hdlOut_ifft)
	{
		jvx_fft_tools_init_framework_ifft(hdlOut_ifft, JVX_FFT_TOOLS_FWK_OLS,
			framesize, 0, coreIfft, &dummyPtr, inputComplex,
			outputReal, numElements_Spectrum, numElements_timeDomain, delay, 0);
	}

	if ((errFft == JVX_DSP_NO_ERROR) && (errIfft == JVX_DSP_NO_ERROR))
	{
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

/**
 * Allocate two handles: One for the FFT part, one for the IFFT part. Pass in framesize,
 * buffersize, handles for FFT and IFFT. The returned data are buffers for input and output as well as
 * the buffersizes and the handles for FFT and IFFT.
 *///====================================================================================
jvxDspBaseErrorType
jvx_fft_tools_init_framework_ols_ifcf(jvxFFTfwk** hdlOut_fft,
	jvxIFFTfwk** hdlOut_ifft,
	jvxInt32 framesize,
	jvxFFT* coreFft,
	jvxIFFT* coreIfft,
	jvxData** inputReal,
	jvxData** outputReal,
	jvxDataCplx** inputComplex,
	jvxDataCplx** outputComplex,
	jvxInt32* numElements_timeDomain,
	jvxInt32* numElements_Spectrum,
	jvxData* delay,
	jvxSize ifcf_length)
{
	jvxDspBaseErrorType errFft = JVX_DSP_NO_ERROR;
	jvxDataCplx* dummyPtr = NULL;
	jvxDspBaseErrorType errIfft = JVX_DSP_NO_ERROR;

	if (hdlOut_fft)
	{
		jvx_fft_tools_init_framework_fft(hdlOut_fft, JVX_FFT_TOOLS_FWK_OLS,
			framesize, 0, coreFft, inputReal, outputComplex,
			numElements_timeDomain, numElements_Spectrum);
	}

	if (hdlOut_ifft)
	{
		jvx_fft_tools_init_framework_ifft(hdlOut_ifft, JVX_FFT_TOOLS_FWK_OLS_IFCF,
			framesize, 0, coreIfft, &dummyPtr, inputComplex,
			outputReal, numElements_Spectrum, numElements_timeDomain, delay, 
			ifcf_length);
	}

	if ((errFft == JVX_DSP_NO_ERROR) && (errIfft == JVX_DSP_NO_ERROR))
	{
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

/**
 * Allocate two handles: One for the FFT part, one for the IFFT part. Pass in framesize,
 * buffersize, handles for FFT and IFFT. The returned data are buffers for input and output as well as
 * the buffersizes and the handles for FFT and IFFT.
 *///====================================================================================
jvxDspBaseErrorType
jvx_fft_tools_init_framework_ols_cf(jvxFFTfwk** hdlOut_fft,
	jvxIFFTfwk** hdlOut_ifft,
	jvxInt32 framesize,
	jvxFFT* coreFft,
	jvxIFFT* coreIfft,
	jvxData** inputReal,
	jvxData** outputReal,
	jvxDataCplx** inputComplex_newWeights,
	jvxDataCplx** inputComplex_oldWeights,
	jvxDataCplx** outputComplex,
	jvxInt32* numElements_timeDomain,
	jvxInt32* numElements_Spectrum,
	jvxData* delay)
{
	jvxDspBaseErrorType errFft = JVX_DSP_NO_ERROR;
	jvxDataCplx* dummyPtr = NULL;
	jvxDspBaseErrorType errIfft = JVX_DSP_NO_ERROR;

	if (hdlOut_fft)
	{
		jvx_fft_tools_init_framework_fft(hdlOut_fft, JVX_FFT_TOOLS_FWK_OLS_CF,
			framesize, 0, coreFft, inputReal, outputComplex,
			numElements_timeDomain, numElements_Spectrum);
	}


	if (hdlOut_ifft)
	{
		jvx_fft_tools_init_framework_ifft(hdlOut_ifft, JVX_FFT_TOOLS_FWK_OLS_CF,
			framesize, 0, coreIfft, inputComplex_oldWeights,
			inputComplex_newWeights, outputReal, numElements_Spectrum,
			numElements_timeDomain, delay, 0);
	}

	if ((errFft == JVX_DSP_NO_ERROR) && (errIfft == JVX_DSP_NO_ERROR))
	{
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// TERMINATE FRAMEWORKS TERMINATE FRAMEWORKS TERMINATE FRAMEWORKS TERMINATE FRAMEWORKS
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/**
 * Deallocate handles for FFT and IFFT for OLS_CF. Note that also the buffers returned
 * on init will no longer be valid. Framework type will be derived from handle.
 *///====================================================================================
jvxDspBaseErrorType
jvx_fft_tools_terminate_framework(jvxFFTfwk* hdl_fft,
	jvxIFFTfwk* hdl_ifft)
{
	jvxFftTools_fwkFftIfft_commonParameters* handle_fft = (jvxFftTools_fwkFftIfft_commonParameters*)hdl_fft;
	jvxFftTools_fwkFftIfft_commonParameters* handle_ifft = (jvxFftTools_fwkFftIfft_commonParameters*)hdl_ifft;

	if (hdl_fft == hdl_ifft)
	{
		if (handle_fft->fwkType != handle_ifft->fwkType)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}
	}

	if (handle_fft)
	{
		jvx_fft_tools_terminate_framework_fft(handle_fft);
	}
	if (handle_ifft)
	{
		jvx_fft_tools_terminate_framework_ifft(handle_ifft);
	}
	return JVX_DSP_NO_ERROR;
}

// ==============================================================================
// ==============================================================================
// ==============================================================================

jvxDspBaseErrorType
jvx_fft_tools_init_framework_fft(jvxFFTfwk** hdlOut,
	jvxFftTools_fwkType fwkType,
	jvxInt32 framesize,
	jvxInt32 buffersize,
	jvxFFT* hdlCoreFftRef,
	jvxData** inputReal,
	jvxDataCplx** outputComplex,
	jvxInt32* numElements_input,
	jvxInt32* numElements_output)
{
	jvxDspBaseErrorType err = JVX_DSP_NO_ERROR;

	jvx_fft_ifft_core_common* hdlCoreFft = (jvx_fft_ifft_core_common*)hdlCoreFftRef;

	jvx_fft_core_real_2_complex* ptrCore_fft_r2c = (jvx_fft_core_real_2_complex*)hdlCoreFft;
	jvx_fft_core_complex_2_complex* ptrCore_fft_c2c = (jvx_fft_core_complex_2_complex*)hdlCoreFft;
	jvx_ifft_core_complex_2_real* ptrCore_ifft_c2r = (jvx_ifft_core_complex_2_real*)hdlCoreFft;
	jvx_ifft_core_complex_2_complex* ptrCore_ifft_c2c = (jvx_ifft_core_complex_2_complex*)hdlCoreFft;

	if (!(hdlOut && outputComplex && numElements_input && numElements_output))
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	switch (fwkType)
	{
	case JVX_FFT_TOOLS_FWK_SW_OLA:
	{
		jvx_tools_hdl_fft_fwk_dw_sw_ola* handle;

		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_fft_fwk_dw_sw_ola);

		handle->common.fwkType = JVX_FFT_TOOLS_FWK_SW_OLA;

		if (!inputReal)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}
		if (hdlCoreFft->fftParameters.coreFftType != JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		// Common parameters
		handle->common.hdlFftIfft = hdlCoreFft;
		handle->common.framesize = framesize;
		handle->common.control.idxBufferRW = 0;

		// Specific buffersize
		handle->buffersize = buffersize;

		// RAM

		// Ram for window function
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.window, jvxData, handle->buffersize);
		jvx_compute_window(handle->ram.window, handle->buffersize,
			handle->common.framesize, 0, JVX_WINDOW_FLATTOP_HANN, NULL);

		// Ram for buffer
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.buffer, jvxData, handle->buffersize);
		memset(handle->ram.buffer, 0, sizeof(jvxData) * handle->buffersize);

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.inputReal, jvxData, handle->common.framesize);
		memset(handle->ram.inputReal, 0, sizeof(jvxData) * handle->common.framesize);

		handle->linkedram.fftbuffer = ptrCore_fft_r2c->input;
		handle->common.linkedram.spectrum = ptrCore_fft_r2c->output;
		handle->common.fftSize = hdlCoreFft->fftParameters.fftSize;

		// Set the pointer references
		*inputReal = handle->ram.inputReal;
		*numElements_input = handle->common.framesize;

		*outputComplex = handle->common.linkedram.spectrum;
		*numElements_output = handle->common.fftSize / 2 + 1;
		*hdlOut = handle;
	}
	break;
	case JVX_FFT_TOOLS_FWK_DW_OLA:
	{
		jvx_tools_hdl_fft_fwk_dw_sw_ola* handle;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_fft_fwk_dw_sw_ola);

		handle->common.fwkType = JVX_FFT_TOOLS_FWK_DW_OLA;

		if (!inputReal)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		if (hdlCoreFft->fftParameters.coreFftType != JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		// Common parameters
		handle->common.hdlFftIfft = hdlCoreFft;
		handle->common.framesize = framesize;
		handle->common.control.idxBufferRW = 0;

		// Specific buffersize
		handle->buffersize = buffersize;

		// RAM

		// Ram for window function
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.window, jvxData, handle->buffersize);
		jvx_compute_window(handle->ram.window, handle->buffersize,
			handle->common.framesize, 0, JVX_WINDOW_SQRT_FLATTOP_HANN, NULL);

		// Ram for buffer
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.buffer, jvxData, handle->buffersize);
		memset(handle->ram.buffer, 0, sizeof(jvxData) * handle->buffersize);

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.inputReal, jvxData, handle->common.framesize);
		memset(handle->ram.inputReal, 0, sizeof(jvxData) * handle->common.framesize);

		handle->linkedram.fftbuffer = ptrCore_fft_r2c->input;
		handle->common.linkedram.spectrum = ptrCore_fft_r2c->output;
		handle->common.fftSize = hdlCoreFft->fftParameters.fftSize;

		// Set the pointer references
		*inputReal = handle->ram.inputReal;
		*numElements_input = handle->common.framesize;

		*outputComplex = handle->common.linkedram.spectrum;
		*numElements_output = handle->common.fftSize / 2 + 1;
		*hdlOut = handle;
	}
	break;
	case JVX_FFT_TOOLS_FWK_OLA_CF:
	{
		jvx_tools_hdl_fft_fwk_ola_cf* handle;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_fft_fwk_ola_cf);

		handle->common.fwkType = JVX_FFT_TOOLS_FWK_OLA_CF;

		if (!inputReal)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}
		if (hdlCoreFft->fftParameters.coreFftType != JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		// Common parameters
		handle->common.hdlFftIfft = hdlCoreFft;
		handle->common.framesize = framesize;
		handle->common.control.idxBufferRW = 0;

		// RAM
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.inputReal, jvxData, handle->common.framesize);
		memset(handle->ram.inputReal, 0, sizeof(jvxData) * handle->common.framesize);

		handle->linkedram.fftbuffer = ptrCore_fft_r2c->input;
		handle->common.linkedram.spectrum = ptrCore_fft_r2c->output;
		handle->common.fftSize = hdlCoreFft->fftParameters.fftSize;

		// Set the pointer references
		*inputReal = handle->ram.inputReal;
		*numElements_input = handle->common.framesize;

		*outputComplex = handle->common.linkedram.spectrum;
		*numElements_output = handle->common.fftSize / 2 + 1;
		*hdlOut = handle;
	}
	break;
	case JVX_FFT_TOOLS_FWK_OLS:
	{
		jvx_tools_hdl_fft_fwk_ols_ols_cf* handle;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_fft_fwk_ols_ols_cf);

		// Special rule for OLS: we may operate directly on the input buffer for higher efficiency
		handle->common.fwkType = JVX_FFT_TOOLS_FWK_OLS;
		if (hdlCoreFft->fftParameters.coreFftType != JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}
		if (hdlCoreFft->operate != JVX_FFT_IFFT_PRESERVE_INPUT)
		{
			// OLS FFT processing requires to save buffer contents between calls!
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		// Common parameters
		handle->common.hdlFftIfft = hdlCoreFft;
		handle->common.framesize = framesize;
		handle->common.control.idxBufferRW = 0;

		// If a zero buffer is passed, user expects that buffer is allocated, otherwise,
		// the buffer comes from the outside
		if (inputReal)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.inputReal, jvxData, handle->common.framesize);
			memset(handle->ram.inputReal, 0, sizeof(jvxData) * handle->common.framesize);
			*inputReal = handle->ram.inputReal;
		}

		handle->linkedram.fftbuffer = ptrCore_fft_r2c->input;
		handle->common.linkedram.spectrum = ptrCore_fft_r2c->output;
		handle->common.fftSize = hdlCoreFft->fftParameters.fftSize;

		*numElements_input = handle->common.framesize;

		*outputComplex = handle->common.linkedram.spectrum;
		*numElements_output = handle->common.fftSize / 2 + 1;
		*hdlOut = handle;
	}
	break;
	case JVX_FFT_TOOLS_FWK_OLS_CF:
	{
		jvx_tools_hdl_fft_fwk_ols_ols_cf* handle;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_fft_fwk_ols_ols_cf);

		handle->common.fwkType = JVX_FFT_TOOLS_FWK_OLS_CF;

		if (!inputReal)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		if (hdlCoreFft->fftParameters.coreFftType != JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		// Common parameters
		handle->common.hdlFftIfft = hdlCoreFft;
		handle->common.framesize = framesize;
		handle->common.control.idxBufferRW = 0;

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.inputReal, jvxData, handle->common.framesize);
		memset(handle->ram.inputReal, 0, sizeof(jvxData) * handle->common.framesize);

		handle->linkedram.fftbuffer = ptrCore_fft_r2c->input;
		handle->common.linkedram.spectrum = ptrCore_fft_r2c->output;
		handle->common.fftSize = hdlCoreFft->fftParameters.fftSize;

		*inputReal = handle->ram.inputReal;
		*numElements_input = handle->common.framesize;

		*outputComplex = handle->common.linkedram.spectrum;
		*numElements_output = handle->common.fftSize / 2 + 1;
		*hdlOut = handle;
	}
	break;
	default:
		assert(0);
	}
	return JVX_DSP_NO_ERROR;
}

// ==============================================================================

jvxDspBaseErrorType
jvx_fft_tools_init_framework_fft_analysis(jvxFFTfwk** hdlOut,
	jvxInt32 framesize,
	jvxFFT* hdlCoreFftRef,
	jvxData** inputReal,
	jvxDataCplx** outputComplex,
	jvxInt32* numElements_input,
	jvxInt32* numElements_output)
{
	jvxDspBaseErrorType err = JVX_DSP_NO_ERROR;

	jvx_fft_ifft_core_common* hdlCoreFft = (jvx_fft_ifft_core_common*)hdlCoreFftRef;

	jvx_fft_core_real_2_complex* ptrCore_fft_r2c = (jvx_fft_core_real_2_complex*)hdlCoreFft;
	jvx_fft_core_complex_2_complex* ptrCore_fft_c2c = (jvx_fft_core_complex_2_complex*)hdlCoreFft;

	jvx_tools_hdl_fft_fwk_dw_sw_ola* handle;

	if (!(hdlOut && inputReal && outputComplex && numElements_input && numElements_output))
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_fft_fwk_dw_sw_ola);

	handle->common.fwkType = JVX_FFT_TOOLS_FWK_ANALYSIS;

	if (hdlCoreFft->fftParameters.coreFftType != JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)
	{
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	// Common parameters
	handle->common.hdlFftIfft = hdlCoreFft;
	handle->common.framesize = framesize;
	handle->common.control.idxBufferRW = 0;

	// Specific buffersize
	handle->buffersize = -1;

	// RAM
	handle->ram.window = NULL;

	handle->linkedram.fftbuffer = ptrCore_fft_r2c->input;
	handle->common.linkedram.spectrum = ptrCore_fft_r2c->output;
	handle->common.fftSize = hdlCoreFft->fftParameters.fftSize;

	// Ram for buffer
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.buffer, jvxData, handle->common.fftSize);
	memset(handle->ram.buffer, 0, sizeof(jvxData) * handle->common.fftSize);

	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.inputReal, jvxData, handle->common.framesize);
	memset(handle->ram.inputReal, 0, sizeof(jvxData) * handle->common.framesize);

	// Set the pointer references
	*inputReal = handle->ram.inputReal;
	*numElements_input = handle->common.framesize;

	*outputComplex = handle->common.linkedram.spectrum;
	*numElements_output = handle->common.fftSize / 2 + 1;
	*hdlOut = handle;
	return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType
jvx_fft_tools_init_framework_ifft(jvxIFFTfwk** hdlOut,
	jvxFftTools_fwkType fwkType,
	jvxInt32 framesize,
	jvxInt32 buffersize,
	jvxIFFT* hdlCoreFftRef,
	jvxDataCplx** inputComplex_oldWeights,
	jvxDataCplx** inputComplex_newWeights,
	jvxData** outputReal,
	jvxInt32* numElements_input,
	jvxInt32* numElements_output,
	jvxData* delay,
	jvxSize ifcf_length_max)
{
	jvxDspBaseErrorType err = JVX_DSP_NO_ERROR;

	jvx_fft_ifft_core_common* hdlCoreFft = (jvx_fft_ifft_core_common*)hdlCoreFftRef;

	jvx_ifft_core_complex_2_real* ptrCore_ifft_c2r = (jvx_ifft_core_complex_2_real*)hdlCoreFft;
	jvx_ifft_core_complex_2_complex* ptrCore_ifft_c2c = (jvx_ifft_core_complex_2_complex*)hdlCoreFft;

	if (!(hdlOut && inputComplex_oldWeights && inputComplex_newWeights && numElements_input && numElements_output))
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	switch (fwkType)
	{
	case JVX_FFT_TOOLS_FWK_SW_OLA:
	{
		jvx_tools_hdl_ifft_fwk_sw_ola* handle;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_ifft_fwk_sw_ola);

		handle->common.common.fwkType = JVX_FFT_TOOLS_FWK_SW_OLA;

		if (!outputReal)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}
		if (hdlCoreFft->fftParameters.coreFftType != JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		// Common parameters
		handle->common.common.hdlFftIfft = hdlCoreFft;
		handle->common.common.framesize = framesize;
		handle->common.common.control.idxBufferRW = 0;

		handle->sizes.buffersize = buffersize;

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.outputReal, jvxData, handle->common.common.framesize);
		memset(handle->ram.outputReal, 0, sizeof(jvxData)*handle->common.common.framesize);

		handle->linkedram.ifftInput = ptrCore_ifft_c2r->input;
		handle->linkedram.ifftOutput = ptrCore_ifft_c2r->output;
		handle->common.common.linkedram.spectrum = ptrCore_ifft_c2r->input;
		handle->common.common.fftSize = hdlCoreFft->fftParameters.fftSize;

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.olabuffer, jvxData, handle->common.common.fftSize);
		memset(handle->ram.olabuffer, 0, sizeof(jvxData) * handle->common.common.fftSize);

		jvx_compute_window(NULL, handle->sizes.buffersize, handle->common.common.framesize, 0,
			JVX_WINDOW_FLATTOP_HANN, &handle->common.normalization);
		handle->common.delay = (jvxData)(handle->sizes.buffersize - handle->common.common.framesize);
		/*
		  handle->common.normalization = (jvxData)
		  (floor((ceil((jvxData)handle->sizes.buffersize/(jvxData)handle->common.common.framesize))/2.0) +
		  ((int)ceil((jvxData)handle->sizes.buffersize/(jvxData)handle->common.common.framesize))%2);
		*/

		handle->common.normalization *= jvxFftTools_ifftNormFactors[hdlCoreFft->fftParameters.fftType];

		*outputReal = handle->ram.outputReal;
		*inputComplex_newWeights = handle->linkedram.ifftInput;
		*inputComplex_oldWeights = NULL;
		*numElements_input = hdlCoreFft->fftParameters.fftSize / 2 + 1;

		if (delay)
		{
			*delay = (jvxData)handle->sizes.buffersize - handle->common.common.framesize;
		}
		*hdlOut = handle;
	}
	break;
	case JVX_FFT_TOOLS_FWK_DW_OLA:
	{
		jvx_tools_hdl_ifft_fwk_dw_ola* handle;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_ifft_fwk_dw_ola);

		handle->common.common.fwkType = JVX_FFT_TOOLS_FWK_DW_OLA;

		if (!outputReal)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}
		if (hdlCoreFft->fftParameters.coreFftType != JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		// Common parameters
		handle->common.common.hdlFftIfft = hdlCoreFft;
		handle->common.common.framesize = framesize;
		handle->common.common.control.idxBufferRW = 0;

		handle->sizes.buffersize = buffersize;

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.outputReal, jvxData, handle->common.common.framesize);
		memset(handle->ram.outputReal, 0, sizeof(jvxData)*handle->common.common.framesize);

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.olabuffer, jvxData, handle->sizes.buffersize);
		memset(handle->ram.olabuffer, 0, sizeof(jvxData)*handle->sizes.buffersize);

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.window, jvxData, handle->sizes.buffersize);
		jvx_compute_window(handle->ram.window, handle->sizes.buffersize,
			handle->common.common.framesize, 0,
			JVX_WINDOW_SQRT_FLATTOP_HANN, &handle->common.normalization);

		handle->linkedram.fftbuffer = ptrCore_ifft_c2r->output;
		handle->common.common.linkedram.spectrum = ptrCore_ifft_c2r->input;
		handle->common.common.fftSize = hdlCoreFft->fftParameters.fftSize;

		handle->common.delay = (jvxData)(handle->sizes.buffersize - handle->common.common.framesize);

		*outputReal = handle->ram.outputReal;
		*inputComplex_newWeights = (jvxDataCplx*)handle->common.common.linkedram.spectrum;
		*inputComplex_oldWeights = NULL;
		*numElements_input = handle->common.common.fftSize / 2 + 1;

		handle->common.normalization *= jvxFftTools_ifftNormFactors[hdlCoreFft->fftParameters.fftType];
		if (delay)
		{
			*delay = handle->common.delay;
		}

		*hdlOut = handle;
	}
	break;

	case JVX_FFT_TOOLS_FWK_OLA_CF:
	{
		jvx_tools_hdl_ifft_fwk_ola_cf* handle;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_ifft_fwk_ola_cf);

		handle->common.common.fwkType = JVX_FFT_TOOLS_FWK_OLA_CF;

		if (!outputReal)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}
		if (hdlCoreFft->fftParameters.coreFftType != JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		// Common parameters
		handle->common.common.hdlFftIfft = hdlCoreFft;
		handle->common.common.framesize = framesize;
		handle->common.common.control.idxBufferRW = 0;

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.outputReal, jvxData, handle->common.common.framesize);
		memset(handle->ram.outputReal, 0, sizeof(jvxData)*handle->common.common.framesize);

		handle->linkedram.ifftInput = ptrCore_ifft_c2c->input;
		handle->linkedram.ifftOutput = ptrCore_ifft_c2c->output;
		handle->common.common.linkedram.spectrum = ptrCore_ifft_c2c->input;
		handle->common.common.fftSize = hdlCoreFft->fftParameters.fftSize;

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.olabuffer, jvxData, hdlCoreFft->fftParameters.fftSize);
		memset(handle->ram.olabuffer, 0, sizeof(jvxData) * hdlCoreFft->fftParameters.fftSize);

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.window, jvxData, handle->common.common.fftSize);
		memset(handle->ram.window, 0, sizeof(jvxData) * handle->common.common.fftSize);

		// Option I: framesize valid values, rest is 0
		jvx_compute_window(handle->ram.window, handle->common.common.framesize,
			handle->common.common.framesize, 0,
			JVX_WINDOW_HALF_HANN, &handle->common.normalization);
		handle->common.delay = 0;

		handle->common.normalization *= jvxFftTools_ifftNormFactors[hdlCoreFft->fftParameters.fftType];

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.inputCplx_new, jvxDataCplx, handle->common.common.fftSize / 2 + 1);
		memset(handle->ram.inputCplx_new, 0, sizeof(jvxDataCplx) * (handle->common.common.fftSize / 2 + 1));

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.inputCplx_old, jvxDataCplx, handle->common.common.fftSize / 2 + 1);
		memset(handle->ram.inputCplx_old, 0, sizeof(jvxDataCplx) * (handle->common.common.fftSize / 2 + 1));

		*outputReal = handle->ram.outputReal;
		*inputComplex_newWeights = handle->ram.inputCplx_new;
		*inputComplex_oldWeights = handle->ram.inputCplx_old;
		*numElements_input = handle->common.common.fftSize / 2 + 1;

		if (delay)
		{
			*delay = 0;
		}
		*hdlOut = handle;
	}
	break;

	case JVX_FFT_TOOLS_FWK_OLS:
	{
		jvx_tools_hdl_ifft_fwk_ols* handle;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_ifft_fwk_ols);

		// Special rule for OLS: we may operate directly on the input buffer for higher efficiency
		handle->common.common.fwkType = JVX_FFT_TOOLS_FWK_OLS;
		if (hdlCoreFft->fftParameters.coreFftType != JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}
		if (hdlCoreFft->operate != JVX_FFT_IFFT_PRESERVE_INPUT)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		// Common parameters
		handle->common.common.hdlFftIfft = hdlCoreFft;
		handle->common.common.framesize = framesize;
		handle->common.common.control.idxBufferRW = 0;

		// If pointer for output is NULL, user expects that buffer is allocated
		if (outputReal)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.outputReal, jvxData, handle->common.common.framesize);
			memset(handle->ram.outputReal, 0, sizeof(jvxData)*handle->common.common.framesize);
			*outputReal = handle->ram.outputReal;
		}

		handle->linkedram.fftbuffer = ptrCore_ifft_c2r->output;
		handle->common.common.linkedram.spectrum = ptrCore_ifft_c2r->input;
		handle->common.common.fftSize = hdlCoreFft->fftParameters.fftSize;

		handle->common.delay = 0;
		handle->common.normalization = 1.0;

		*inputComplex_newWeights = (jvxDataCplx*)handle->common.common.linkedram.spectrum;
		*inputComplex_oldWeights = NULL;
		*numElements_input = handle->common.common.fftSize / 2 + 1;

		handle->common.normalization *= jvxFftTools_ifftNormFactors[hdlCoreFft->fftParameters.fftType];
		if (delay)
		{
			*delay = 0;
		}

		*hdlOut = handle;
	}
	break;
	case JVX_FFT_TOOLS_FWK_OLS_CF:
	{
		jvx_tools_hdl_ifft_fwk_ols_cf* handle;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_ifft_fwk_ols_cf);

		handle->common.common.fwkType = JVX_FFT_TOOLS_FWK_OLS_CF;

		if (!outputReal)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}
		if (hdlCoreFft->fftParameters.coreFftType != JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		// Common parameters
		handle->common.common.hdlFftIfft = hdlCoreFft;
		handle->common.common.framesize = framesize;
		handle->common.common.control.idxBufferRW = 0;

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.outputReal, jvxData, handle->common.common.framesize);
		memset(handle->ram.outputReal, 0, sizeof(jvxData)*handle->common.common.framesize);

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.window, jvxData, handle->common.common.framesize);
		jvx_compute_window(handle->ram.window, handle->common.common.framesize, 0, 0,
			JVX_WINDOW_HALF_HANN, &handle->common.normalization);

		handle->linkedram.ifftInput = ptrCore_ifft_c2c->input;
		handle->linkedram.ifftOutput = ptrCore_ifft_c2c->output;
		handle->common.common.fftSize = hdlCoreFft->fftParameters.fftSize;

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.inputCplx_new, jvxDataCplx, handle->common.common.fftSize / 2 + 1);
		memset(handle->ram.inputCplx_new, 0, sizeof(jvxDataCplx) * (handle->common.common.fftSize / 2 + 1));

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.inputCplx_old, jvxDataCplx, handle->common.common.fftSize / 2 + 1);
		memset(handle->ram.inputCplx_old, 0, sizeof(jvxDataCplx) * (handle->common.common.fftSize / 2 + 1));

		handle->common.delay = 0;
		handle->common.normalization = 1.0;

		*outputReal = handle->ram.outputReal;
		*inputComplex_newWeights = handle->ram.inputCplx_new;
		*inputComplex_oldWeights = handle->ram.inputCplx_old;
		*numElements_input = handle->common.common.fftSize / 2 + 1;

		handle->common.normalization *= jvxFftTools_ifftNormFactors[hdlCoreFft->fftParameters.fftType];
		if (delay)
		{
			*delay = 0.0;
		}

		*hdlOut = handle;
	}
	break;

	case JVX_FFT_TOOLS_FWK_OLS_IFCF:
	{
		jvx_tools_hdl_ifft_fwk_ols_ifcf* handle;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_ifft_fwk_ols_ifcf);

		// Special rule for OLS: we may operate directly on the input buffer for higher efficiency
		handle->ols.common.common.fwkType = JVX_FFT_TOOLS_FWK_OLS_IFCF;
		if (hdlCoreFft->fftParameters.coreFftType != JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}
		if (hdlCoreFft->operate != JVX_FFT_IFFT_PRESERVE_INPUT)
		{
			return JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		// Common parameters
		handle->ols.common.common.hdlFftIfft = hdlCoreFft;
		handle->ols.common.common.framesize = framesize;
		handle->ols.common.common.control.idxBufferRW = 0;

		// If pointer for output is NULL, user expects that buffer is allocated
		if (outputReal)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ols.ram.outputReal, jvxData, handle->ols.common.common.framesize);
			memset(handle->ols.ram.outputReal, 0, sizeof(jvxData)*handle->ols.common.common.framesize);
			*outputReal = handle->ols.ram.outputReal;
		}

		handle->ols.linkedram.fftbuffer = ptrCore_ifft_c2r->output;
		handle->ols.common.common.linkedram.spectrum = ptrCore_ifft_c2r->input;
		handle->ols.common.common.fftSize = hdlCoreFft->fftParameters.fftSize;

		handle->ols.common.delay = 0;
		handle->ols.common.normalization = 1.0;

		*inputComplex_newWeights = (jvxDataCplx*)handle->ols.common.common.linkedram.spectrum;
		*inputComplex_oldWeights = NULL;
		*numElements_input = handle->ols.common.common.fftSize / 2 + 1;

		handle->ols.common.normalization *= jvxFftTools_ifftNormFactors[hdlCoreFft->fftParameters.fftType];
		if (delay)
		{
			*delay = 0;
		}

		handle->save_data_ll = 0;
		handle->save_data = NULL;
		if (ifcf_length_max)
		{
			handle->save_data_ll = ifcf_length_max;
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->save_data, jvxData, handle->save_data_ll);
		}
		*hdlOut = handle;
	}
	break;
	default:
		assert(0);
	}
	return JVX_DSP_NO_ERROR;
}


// =======================================================================================

jvxDspBaseErrorType
jvx_fft_tools_terminate_framework_fft(jvxFFTfwk* hdl)
{
	jvxDspBaseErrorType err = JVX_DSP_NO_ERROR;
	jvxFftTools_fwkFftIfft_commonParameters* ptrType;
	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	ptrType = (jvxFftTools_fwkFftIfft_commonParameters*)hdl;

	switch (ptrType->fwkType)
	{
	case JVX_FFT_TOOLS_FWK_SW_OLA:
	{
		jvx_tools_hdl_fft_fwk_dw_sw_ola* handle = (jvx_tools_hdl_fft_fwk_dw_sw_ola*)hdl;

		// Common parameters
		handle->common.hdlFftIfft = NULL;
		handle->common.framesize = 0;
		handle->common.control.idxBufferRW = 0;

		// Specific buffersize
		handle->buffersize = 0;

		// RAM

		// Ram for window function
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.window);
		handle->ram.window = NULL;

		// Ram for buffer
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.buffer);
		handle->ram.buffer = NULL;

		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.inputReal);
		handle->ram.inputReal = NULL;

		handle->linkedram.fftbuffer = NULL;
		handle->common.linkedram.spectrum = NULL;

		// Set the pointer references
		JVX_DSP_SAFE_DELETE_OBJECT(handle);
	}
	break;
	case JVX_FFT_TOOLS_FWK_DW_OLA:
	{
		jvx_tools_hdl_fft_fwk_dw_sw_ola* handle = (jvx_tools_hdl_fft_fwk_dw_sw_ola*)hdl;

		// Common parameters
		handle->common.hdlFftIfft = NULL;
		handle->common.framesize = 0;
		handle->common.control.idxBufferRW = 0;

		// Specific buffersize
		handle->buffersize = 0;

		// RAM

		// Ram for window function
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.window);
		handle->ram.window = NULL;

		// Ram for buffer
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.buffer);
		handle->ram.buffer = NULL;

		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.inputReal);
		handle->ram.inputReal = NULL;

		handle->linkedram.fftbuffer = NULL;
		handle->common.linkedram.spectrum = NULL;
		JVX_DSP_SAFE_DELETE_OBJECT(handle);
	}
	break;
	case JVX_FFT_TOOLS_FWK_OLA_CF:
	{
		jvx_tools_hdl_fft_fwk_ola_cf* handle = (jvx_tools_hdl_fft_fwk_ola_cf*)hdl;

		// Common parameters
		handle->common.hdlFftIfft = NULL;
		handle->common.framesize = 0;
		handle->common.control.idxBufferRW = 0;

		// RAM
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.inputReal);
		handle->ram.inputReal = NULL;

		handle->linkedram.fftbuffer = NULL;
		handle->common.linkedram.spectrum = NULL;
		handle->common.fftSize = 0;

		handle->linkedram.fftbuffer = NULL;
		handle->common.linkedram.spectrum = NULL;
		JVX_DSP_SAFE_DELETE_OBJECT(handle);
	}
	break;
	case JVX_FFT_TOOLS_FWK_OLS:
	{
		jvx_tools_hdl_fft_fwk_ols_ols_cf* handle = (jvx_tools_hdl_fft_fwk_ols_ols_cf*)hdl;

		// Common parameters
		handle->common.hdlFftIfft = NULL;
		handle->common.framesize = 0;
		handle->common.control.idxBufferRW = 0;

		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.inputReal);

		handle->linkedram.fftbuffer = NULL;
		handle->common.linkedram.spectrum = NULL;

		JVX_DSP_SAFE_DELETE_OBJECT(handle);
	}
	break;
	case JVX_FFT_TOOLS_FWK_OLS_CF:
	{
		jvx_tools_hdl_fft_fwk_ols_ols_cf* handle = (jvx_tools_hdl_fft_fwk_ols_ols_cf*)hdl;

		// Common parameters
		handle->common.hdlFftIfft = NULL;
		handle->common.framesize = 0;
		handle->common.control.idxBufferRW = 0;

		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.inputReal);

		handle->linkedram.fftbuffer = NULL;
		handle->common.linkedram.spectrum = NULL;

		JVX_DSP_SAFE_DELETE_OBJECT(handle);
	}
	break;
	default:
		assert(0);
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fft_tools_terminate_framework_fft_analysis(jvxFFTfwk* hdl)
{
	jvxDspBaseErrorType err = JVX_DSP_NO_ERROR;
	jvxFftTools_fwkFftIfft_commonParameters* ptrType;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	ptrType = (jvxFftTools_fwkFftIfft_commonParameters*)hdl;

	switch (ptrType->fwkType)
	{
	case JVX_FFT_TOOLS_FWK_ANALYSIS:
	{
		jvx_tools_hdl_fft_fwk_dw_sw_ola* handle = (jvx_tools_hdl_fft_fwk_dw_sw_ola*)hdl;

		// Common parameters
		handle->common.hdlFftIfft = NULL;
		handle->common.framesize = 0;
		handle->common.control.idxBufferRW = 0;

		// Specific buffersize
		handle->buffersize = 0;

		// Ram for buffer
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.buffer);
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.inputReal);

		handle->linkedram.fftbuffer = NULL;
		handle->common.linkedram.spectrum = NULL;

		// Set the pointer references
		JVX_DSP_SAFE_DELETE_OBJECT(handle);
	}
	break;
	default:
		assert(0);
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fft_tools_terminate_framework_ifft(jvxIFFTfwk* hdl)
{
	jvxDspBaseErrorType err = JVX_DSP_NO_ERROR;
	jvxFftTools_fwkFftIfft_commonParameters* ptrType;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	ptrType = (jvxFftTools_fwkFftIfft_commonParameters*)hdl;

	switch (ptrType->fwkType)
	{
	case JVX_FFT_TOOLS_FWK_SW_OLA:
	{
		jvx_tools_hdl_ifft_fwk_sw_ola* handle = (jvx_tools_hdl_ifft_fwk_sw_ola*)hdl;

		// Common parameters
		handle->common.common.hdlFftIfft = NULL;
		handle->common.common.framesize = 0;
		handle->common.common.control.idxBufferRW = 0;

		handle->sizes.buffersize = 0;

		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.outputReal);

		handle->linkedram.ifftInput = NULL;
		handle->linkedram.ifftOutput = NULL;
		handle->common.common.linkedram.spectrum = NULL;
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.olabuffer);

		handle->common.delay = 0;
		handle->common.normalization = 0.0;

		JVX_DSP_SAFE_DELETE_OBJECT(handle);
	}
	break;
	case JVX_FFT_TOOLS_FWK_DW_OLA:
	{
		jvx_tools_hdl_ifft_fwk_dw_ola* handle = (jvx_tools_hdl_ifft_fwk_dw_ola*)hdl;

		// Common parameters
		handle->common.common.hdlFftIfft = NULL;
		handle->common.common.framesize = 0;
		handle->common.common.control.idxBufferRW = 0;

		handle->sizes.buffersize = 0;

		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.outputReal);
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.olabuffer);
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.window);

		handle->linkedram.fftbuffer = NULL;
		handle->common.common.linkedram.spectrum = NULL;

		handle->common.delay = 0;
		handle->common.normalization = 0.0;

		JVX_DSP_SAFE_DELETE_OBJECT(handle);
	}
	break;

	case JVX_FFT_TOOLS_FWK_OLA_CF:
	{
		jvx_tools_hdl_ifft_fwk_ola_cf* handle = (jvx_tools_hdl_ifft_fwk_ola_cf*)hdl;

		// Common parameters
		handle->common.common.hdlFftIfft = NULL;
		handle->common.common.framesize = 0;
		handle->common.common.control.idxBufferRW = 0;

		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.inputCplx_new);
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.inputCplx_old);
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.olabuffer);
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.outputReal);
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.window);

		handle->linkedram.ifftInput = NULL;
		handle->linkedram.ifftOutput = NULL;
		handle->common.common.linkedram.spectrum = NULL;

		handle->common.delay = 0;
		handle->common.normalization = 0.0;

		JVX_DSP_SAFE_DELETE_OBJECT(handle);
	}
	break;

	case JVX_FFT_TOOLS_FWK_OLS:
	{
		jvx_tools_hdl_ifft_fwk_ols* handle = (jvx_tools_hdl_ifft_fwk_ols*)hdl;

		// Common parameters
		handle->common.common.hdlFftIfft = NULL;
		handle->common.common.framesize = 0;
		handle->common.common.control.idxBufferRW = 0;

		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.outputReal);

		handle->linkedram.fftbuffer = NULL;
		handle->common.common.linkedram.spectrum = NULL;

		handle->common.delay = 0;
		handle->common.normalization = 0.0;

		JVX_DSP_SAFE_DELETE_OBJECT(handle);
	}
	break;
	case JVX_FFT_TOOLS_FWK_OLS_CF:
	{
		jvx_tools_hdl_ifft_fwk_ols_cf* handle = (jvx_tools_hdl_ifft_fwk_ols_cf*)hdl;

		// Common parameters
		handle->common.common.hdlFftIfft = NULL;
		handle->common.common.framesize = 0;
		handle->common.common.control.idxBufferRW = 0;

		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.outputReal);
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.window);

		handle->linkedram.ifftInput = NULL;
		handle->linkedram.ifftOutput = NULL;
		handle->common.common.linkedram.spectrum = NULL;

		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.inputCplx_new);
		JVX_DSP_SAFE_DELETE_FIELD(handle->ram.inputCplx_old);

		handle->common.delay = 0;
		handle->common.normalization = 0.0;

		JVX_DSP_SAFE_DELETE_OBJECT(handle);
	}
	break;
	case JVX_FFT_TOOLS_FWK_OLS_IFCF:
	{
		jvx_tools_hdl_ifft_fwk_ols_ifcf* handle = (jvx_tools_hdl_ifft_fwk_ols_ifcf*)hdl;

		if (handle->save_data_ll)
		{
			JVX_DSP_SAFE_DELETE_FIELD(handle->save_data);
		}
		handle->save_data_ll = 0;

		// Common parameters
		handle->ols.common.common.hdlFftIfft = NULL;
		handle->ols.common.common.framesize = 0;
		handle->ols.common.common.control.idxBufferRW = 0;

		JVX_DSP_SAFE_DELETE_FIELD(handle->ols.ram.outputReal);

		handle->ols.linkedram.fftbuffer = NULL;
		handle->ols.common.common.linkedram.spectrum = NULL;

		handle->ols.common.delay = 0;
		handle->ols.common.normalization = 0.0;

		JVX_DSP_SAFE_DELETE_OBJECT(handle);
	}
	break;
	default:
		assert(0);
	}
	return JVX_DSP_NO_ERROR;
}

// =======================================================================================

jvxDspBaseErrorType
jvx_fft_tools_process_framework_fft(jvxFFTfwk* hdl, jvxData* in_real)
{
	jvxSize i;
	jvxDspBaseErrorType err = JVX_DSP_NO_ERROR;
	jvxFftTools_fwkFftIfft_commonParameters* ptrType;
	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	ptrType = (jvxFftTools_fwkFftIfft_commonParameters*)hdl;

	switch (ptrType->fwkType)
	{
	case JVX_FFT_TOOLS_FWK_SW_OLA:
	{
		jvx_tools_hdl_fft_fwk_dw_sw_ola* handle = (jvx_tools_hdl_fft_fwk_dw_sw_ola*)hdl;

		jvxSize idxW = handle->common.control.idxBufferRW;

		jvxSize ll1 = JVX_MIN(handle->buffersize - idxW, handle->common.framesize);
		jvxSize ll2 = handle->common.framesize - ll1;
		jvxSize ll3 = 0;

		jvxData* ptrIn = handle->ram.inputReal;
		jvxData* ptrOut = handle->ram.buffer + idxW;
		jvxData* ptrWin = NULL;

		if (in_real)
			ptrIn = in_real;

		for (i = 0; i < ll1; i++)
		{
			*ptrOut++ = *ptrIn++;
		}
		ptrOut = handle->ram.buffer;
		for (i = 0; i < ll2; i++)
		{
			*ptrOut++ = *ptrIn++;
		}
		idxW = (idxW + handle->common.framesize) % handle->buffersize;

		ll1 = handle->buffersize - idxW;
		ll2 = handle->buffersize - ll1;
		ll3 = handle->common.fftSize - handle->buffersize;

		ptrIn = handle->ram.buffer + idxW;
		ptrWin = handle->ram.window;
		ptrOut = handle->linkedram.fftbuffer;

		for (i = 0; i < ll1; i++)
		{
			*ptrOut++ = *ptrIn++ * *ptrWin++;
		}
		ptrIn = handle->ram.buffer;
		for (i = 0; i < ll2; i++)
		{
			*ptrOut++ = *ptrIn++ * *ptrWin++;
		}
#ifdef SET_FFT_BUFFER_ZERO_BEFORE_TRANSFORM
		for (i = 0; i < ll3; i++)
		{
			*ptrOut++ = 0.0;
		}
#endif
		jvx_execute_fft(handle->common.hdlFftIfft);
		handle->common.control.idxBufferRW = idxW;
		return JVX_DSP_NO_ERROR;
	}
	break;
	case JVX_FFT_TOOLS_FWK_DW_OLA:
	{
		jvx_tools_hdl_fft_fwk_dw_sw_ola* handle = (jvx_tools_hdl_fft_fwk_dw_sw_ola*)hdl;

		jvxSize idxW = handle->common.control.idxBufferRW;

		jvxSize ll1 = JVX_MIN(handle->buffersize - idxW, handle->common.framesize);
		jvxSize ll2 = handle->common.framesize - ll1;
		jvxSize ll3 = 0;

		jvxData* ptrIn = handle->ram.inputReal;
		jvxData* ptrOut = handle->ram.buffer + idxW;
		jvxData* ptrWin = NULL;

		if (in_real)
			ptrIn = in_real;

		for (i = 0; i < ll1; i++)
		{
			*ptrOut++ = *ptrIn++;
		}
		ptrOut = handle->ram.buffer;
		for (i = 0; i < ll2; i++)
		{
			*ptrOut++ = *ptrIn++;
		}
		idxW = (idxW + handle->common.framesize) % handle->buffersize;

		ll1 = handle->buffersize - idxW;
		ll2 = handle->buffersize - ll1;
		ll3 = handle->common.fftSize - handle->buffersize;

		ptrIn = handle->ram.buffer + idxW;
		ptrWin = handle->ram.window;
		ptrOut = handle->linkedram.fftbuffer;

		for (i = 0; i < ll1; i++)
		{
			*ptrOut++ = *ptrIn++ * *ptrWin++;
		}
		ptrIn = handle->ram.buffer;
		for (i = 0; i < ll2; i++)
		{
			*ptrOut++ = *ptrIn++ * *ptrWin++;
		}
#ifdef SET_FFT_BUFFER_ZERO_BEFORE_TRANSFORM
		for (i = 0; i < ll3; i++)
		{
			*ptrOut++ = 0.0;
		}
#endif
		jvx_execute_fft(handle->common.hdlFftIfft);
		handle->common.control.idxBufferRW = idxW;
		return JVX_DSP_NO_ERROR;
	}
	break;

	case JVX_FFT_TOOLS_FWK_OLA_CF:
	{
		jvx_tools_hdl_fft_fwk_ola_cf* handle = (jvx_tools_hdl_fft_fwk_ola_cf*)hdl;

#ifdef SET_FFT_BUFFER_ZERO_BEFORE_TRANSFORM
		memset(handle->linkedram.fftbuffer, 0, sizeof(jvxData)*handle->common.fftSize);
#endif
		memcpy(handle->linkedram.fftbuffer, handle->ram.inputReal, sizeof(jvxData)*handle->common.framesize);

		jvx_execute_fft(handle->common.hdlFftIfft);
		return JVX_DSP_NO_ERROR;
	}
	break;


	case JVX_FFT_TOOLS_FWK_OLS:
	{
		jvx_tools_hdl_fft_fwk_ols_ols_cf* handle = (jvx_tools_hdl_fft_fwk_ols_ols_cf*)hdl;
		jvxSize idxW = handle->common.control.idxBufferRW;

		jvxSize ll1 = JVX_MIN(handle->common.fftSize - idxW, handle->common.framesize);
		jvxSize ll2 = handle->common.framesize - ll1;

		jvxData* ptrIn = handle->ram.inputReal;
		jvxData* ptrOut = handle->linkedram.fftbuffer + idxW;

		if (in_real)
			ptrIn = in_real;

		assert(ptrIn);

		for (i = 0; i < ll1; i++)
		{
			*ptrOut++ = *ptrIn++;
		}
		ptrOut = handle->linkedram.fftbuffer;
		for (i = 0; i < ll2; i++)
		{
			*ptrOut++ = *ptrIn++;
		}
		handle->common.control.idxBufferRW = (idxW + handle->common.framesize) % handle->common.fftSize;

		jvx_execute_fft(handle->common.hdlFftIfft);
		return JVX_DSP_NO_ERROR;

	}
	break;
	case JVX_FFT_TOOLS_FWK_OLS_CF:
	{
		jvx_tools_hdl_fft_fwk_ols_ols_cf* handle = (jvx_tools_hdl_fft_fwk_ols_ols_cf*)hdl;
		jvxSize idxW = handle->common.control.idxBufferRW;

		jvxSize ll1 = JVX_MIN(handle->common.fftSize - idxW, handle->common.framesize);
		jvxSize ll2 = handle->common.framesize - ll1;

		jvxData* ptrIn = handle->ram.inputReal;
		jvxData* ptrOut = handle->linkedram.fftbuffer + idxW;

		if (in_real)
			ptrIn = in_real;

		for (i = 0; i < ll1; i++)
		{
			*ptrOut++ = *ptrIn++;
		}
		ptrOut = handle->linkedram.fftbuffer;
		for (i = 0; i < ll2; i++)
		{
			*ptrOut++ = *ptrIn++;
		}
		handle->common.control.idxBufferRW = (idxW + handle->common.framesize) % handle->common.fftSize;

		jvx_execute_fft(handle->common.hdlFftIfft);
		return JVX_DSP_NO_ERROR;
	}
	break;
	default:
		assert(0);
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fft_tools_process_framework_fft_analysis(jvxFFTfwk* hdl,
	jvxData* window,
	jvxUInt32 numSamplesValidWindow)
{
	jvxSize i;
	jvxDspBaseErrorType err = JVX_DSP_NO_ERROR;

	jvxFftTools_fwkFftIfft_commonParameters* ptrType;
	jvx_tools_hdl_fft_fwk_dw_sw_ola* handle;

	jvxSize idxW;

	jvxSize ll1;
	jvxSize ll2;
	jvxSize ll3 = 0;
	jvxSize ll3_0 = 0;
	jvxSize ll3_1 = 0;

	jvxData* ptrIn;
	jvxData* ptrOut;
	jvxData* ptrWin = NULL;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	ptrType = (jvxFftTools_fwkFftIfft_commonParameters*)hdl;
	handle = (jvx_tools_hdl_fft_fwk_dw_sw_ola*)hdl;
	idxW = handle->common.control.idxBufferRW;
	ll1 = JVX_MIN(handle->common.fftSize - (jvxInt32)idxW, handle->common.framesize);
	ll2 = handle->common.framesize - ll1;
	ptrIn = handle->ram.inputReal;
	ptrOut = handle->ram.buffer + idxW;

	assert(ptrType->fwkType == JVX_FFT_TOOLS_FWK_ANALYSIS);

	for (i = 0; i < ll1; i++)
	{
		*ptrOut++ = *ptrIn++;
	}
	ptrOut = handle->ram.buffer;
	for (i = 0; i < ll2; i++)
	{
		*ptrOut++ = *ptrIn++;
	}
	idxW = (idxW + handle->common.framesize) % handle->common.fftSize;

	ll1 = JVX_MIN(numSamplesValidWindow, idxW);
	ll2 = numSamplesValidWindow - ll1;
	ll3 = handle->common.fftSize - numSamplesValidWindow;
	ll3_0 = idxW - ll1;
	ll3_1 = ll3 - ll3_0;

	ptrIn = handle->ram.buffer + idxW;
	ptrWin = window + numSamplesValidWindow;
	ptrOut = handle->linkedram.fftbuffer + handle->common.fftSize;

	for (i = 0; i < ll1; i++)
	{
		ptrWin--;
		ptrOut--;
		ptrIn--;
		*ptrOut = *ptrIn * *ptrWin;
	}

#ifdef SET_FFT_BUFFER_ZERO_BEFORE_TRANSFORM
	for (i = 0; i < ll3_0; i++)
	{
		ptrOut--;
		*ptrOut = 0.0;
	}
#endif
	ptrIn = handle->ram.buffer + handle->common.fftSize;
	for (i = 0; i < ll2; i++)
	{
		ptrWin--;
		ptrOut--;
		ptrIn--;
		*ptrOut = *ptrIn * *ptrWin;
	}
#ifdef SET_FFT_BUFFER_ZERO_BEFORE_TRANSFORM
	for (i = 0; i < ll3_1; i++)
	{
		ptrOut--;
		*ptrOut = 0.0;
	}
#endif
	jvx_execute_fft(handle->common.hdlFftIfft);
	handle->common.control.idxBufferRW = idxW;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fft_tools_process_framework_ifft(jvxIFFTfwk* hdl, jvxData* out_real, jvxSize ifcf_length)
{
	int i;
	jvxData accu = 0.0;

	jvxDspBaseErrorType err = JVX_DSP_NO_ERROR;
	jvxFftTools_fwkFftIfft_commonParameters* ptrType;
	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	ptrType = (jvxFftTools_fwkFftIfft_commonParameters*)hdl;

	switch (ptrType->fwkType)
	{
	case JVX_FFT_TOOLS_FWK_SW_OLA:
	{
		jvx_tools_hdl_ifft_fwk_sw_ola* handle = (jvx_tools_hdl_ifft_fwk_sw_ola*)hdl;


		jvxSize idxW;

		jvxData* ptrOutOut;
		jvxData* ptrOut;
		jvxData* ptrIn;

		jvxSize limll;
		jvxSize ll1;
		jvxSize ll1_0;
		jvxSize ll1_1;
		jvxSize ll2;
		jvxSize ll2_0;
		jvxSize ll2_1;

		jvx_execute_ifft(handle->common.common.hdlFftIfft);

		idxW = handle->common.common.control.idxBufferRW;

		ptrOutOut = handle->ram.outputReal;

		if (out_real)
			ptrOutOut = out_real;
		assert(ptrOutOut);

		ptrOut = handle->ram.olabuffer + idxW;
		ptrIn = handle->linkedram.ifftOutput;

		limll = handle->common.common.framesize;
		ll1 = handle->common.common.fftSize - idxW;
		ll1_0 = JVX_MIN(ll1, limll);
		ll1_1 = ll1 - ll1_0;
		limll -= ll1_0;
		ll2 = handle->common.common.fftSize - ll1;
		ll2_0 = JVX_MIN(ll2, limll);
		ll2_1 = ll2 - ll2_0;

		// Output the returned values, set the ola buffer to zero at this place
		for (i = 0; i < ll1_0; i++)
		{
			accu = *ptrIn;
			accu *= handle->common.normalization;
			*ptrOutOut = accu + (*ptrOut);
			*ptrOut = 0.0;
			ptrOut++;
			ptrIn++;
			ptrOutOut++;
		}
		for (i = 0; i < ll1_1; i++)
		{
			accu = *ptrIn;
			accu *= handle->common.normalization;
			*ptrOut += accu;
			ptrOut++;
			ptrIn++;
		}

		ptrOut = handle->ram.olabuffer;

		// Output the returned values, set the ola buffer to zero at this place
		for (i = 0; i < ll2_0; i++)
		{
			accu = *ptrIn;
			accu *= handle->common.normalization;
			*ptrOutOut = accu + *ptrOut;
			*ptrOut = 0.0;
			ptrOut++;
			ptrIn++;
			ptrOutOut++;
		}
		for (i = 0; i < ll2_1; i++)
		{
			accu = *ptrIn;
			accu *= handle->common.normalization;
			*ptrOut += accu;
			ptrOut++;
			ptrIn++;
		}
		handle->common.common.control.idxBufferRW = (idxW + handle->common.common.framesize) % handle->common.common.fftSize;
	}
	break;
	case JVX_FFT_TOOLS_FWK_DW_OLA:
	{
		jvx_tools_hdl_ifft_fwk_dw_ola* handle = (jvx_tools_hdl_ifft_fwk_dw_ola*)hdl;

		jvxSize idxW;

		jvxData* ptrOutOut;
		jvxData* ptrOut;
		jvxData* ptrIn;
		jvxData* ptrWin;

		jvxSize limll;
		jvxSize ll1;
		jvxSize ll1_0;
		jvxSize ll1_1;
		jvxSize ll2;
		jvxSize ll2_0;
		jvxSize ll2_1;

		jvx_execute_ifft(handle->common.common.hdlFftIfft);

		idxW = handle->common.common.control.idxBufferRW;

		ptrOutOut = handle->ram.outputReal;

		if (out_real)
			ptrOutOut = out_real;
		assert(ptrOutOut);

		ptrOut = handle->ram.olabuffer + idxW;
		ptrIn = handle->linkedram.fftbuffer;
		ptrWin = handle->ram.window;

		limll = handle->common.common.framesize;
		ll1 = handle->sizes.buffersize - idxW;
		ll1_0 = JVX_MIN(ll1, limll);
		ll1_1 = ll1 - ll1_0;
		limll -= ll1_0;
		ll2 = handle->sizes.buffersize - ll1;
		ll2_0 = JVX_MIN(ll2, limll);
		ll2_1 = ll2 - ll2_0;

		// Output the returned values, set the ola buffer to zero at this place
		for (i = 0; i < ll1_0; i++)
		{
			accu = *ptrIn;
			accu *= handle->common.normalization;
			*ptrOutOut = (accu * *ptrWin) + (*ptrOut);
			*ptrOut = 0.0;
			ptrOut++;
			ptrIn++;
			ptrOutOut++;
			ptrWin++;
		}
		for (i = 0; i < ll1_1; i++)
		{
			accu = *ptrIn;
			accu *= handle->common.normalization;
			*ptrOut += (accu * *ptrWin);
			ptrOut++;
			ptrIn++;
			ptrWin++;
		}

		ptrOut = handle->ram.olabuffer;

		// Output the returned values, set the ola buffer to zero at this place
		for (i = 0; i < ll2_0; i++)
		{
			accu = *ptrIn;
			accu *= handle->common.normalization;
			*ptrOutOut = (accu * *ptrWin) + *ptrOut;
			*ptrOut = 0.0;
			ptrOut++;
			ptrIn++;
			ptrOutOut++;
			ptrWin++;
		}
		for (i = 0; i < ll2_1; i++)
		{
			accu = *ptrIn;
			accu *= handle->common.normalization;
			*ptrOut += (accu * *ptrWin);
			ptrOut++;
			ptrIn++;
			ptrWin++;
		}
		handle->common.common.control.idxBufferRW = (idxW + handle->common.common.framesize) % handle->sizes.buffersize;
		/*
		  if(dbg)
		  {
		  memcpy(dbg, handle->ram.olabuffer, sizeof(jvxData)* handle->sizes.buffersize);
		  }*/
	}
	break;

	case JVX_FFT_TOOLS_FWK_OLA_CF:
	{
		jvx_tools_hdl_ifft_fwk_ola_cf* handle = (jvx_tools_hdl_ifft_fwk_ola_cf*)hdl;

		jvxDataCplx* ptrTarget_firstHalf = handle->linkedram.ifftInput;
		jvxDataCplx* ptrTarget_secondHalf = handle->linkedram.ifftInput + handle->common.common.fftSize;
		jvxDataCplx* ptrSource_old = handle->ram.inputCplx_old;
		jvxDataCplx* ptrSource_new = handle->ram.inputCplx_new;

		jvxSize idxW;

		jvxData* ptrOutOut;
		jvxDataCplx* ptrIn;
		jvxData* ptrWin_start;
		jvxData* ptrOla;

		jvxSize limll;
		jvxSize ll1;
		jvxSize ll1_0;
		jvxSize ll1_1;
		jvxSize ll2;
		jvxSize ll2_0;
		jvxSize ll2_1;
		jvxData accuWin;

		// Object at position "0"
		ptrTarget_firstHalf->re = ptrSource_old->re - ptrSource_new->im;
		ptrTarget_firstHalf->im = ptrSource_old->im + ptrSource_new->re;
		ptrTarget_firstHalf++;
		ptrSource_old++;
		ptrSource_new++;

		for (i = 0; i < (handle->common.common.fftSize / 2 - 1); i++)
		{
			ptrTarget_firstHalf->re = ptrSource_old->re - ptrSource_new->im;
			ptrTarget_firstHalf->im = ptrSource_old->im + ptrSource_new->re;
			ptrTarget_firstHalf++;

			--ptrTarget_secondHalf;
			ptrTarget_secondHalf->re = ptrSource_old->re + ptrSource_new->im;
			ptrTarget_secondHalf->im = -ptrSource_old->im + ptrSource_new->re;

			ptrSource_new++;
			ptrSource_old++;
		}

		// Object at position "L/2"
		ptrTarget_firstHalf->re = ptrSource_old->re - ptrSource_new->im;
		ptrTarget_firstHalf->im = ptrSource_old->im + ptrSource_new->re;

		// Transform signal
		jvx_execute_ifft(handle->common.common.hdlFftIfft);

		idxW = handle->common.common.control.idxBufferRW;

		ptrOutOut = handle->ram.outputReal;
		
		if (out_real)
			ptrOutOut = out_real;
		assert(ptrOutOut);

		ptrIn = handle->linkedram.ifftOutput;
		ptrWin_start = handle->ram.window;
		ptrOla = handle->ram.olabuffer + idxW;

		limll = handle->common.common.framesize;
		ll1 = handle->common.common.fftSize - idxW;
		ll1_0 = JVX_MIN(ll1, limll);
		ll1_1 = ll1 - ll1_0;
		limll -= ll1_0;
		ll2 = handle->common.common.fftSize - ll1;
		ll2_0 = JVX_MIN(ll2, limll);
		ll2_1 = ll2 - ll2_0;
		accuWin = 0.0;

		for (i = 0; i < ll1_0; i++)
		{
			accuWin = *ptrWin_start;
			accu = ptrIn->re * accuWin + ptrIn->im * (1 - accuWin);// <- real part is from "old" weights
			accu *= handle->common.normalization;
			*ptrOutOut++ = *ptrOla + accu;
			*ptrOla = 0.0;
			ptrIn++;
			ptrWin_start++;
			ptrOla++;
		}
		for (i = 0; i < ll1_1; i++)
		{
			accuWin = *ptrWin_start;
			accu = ptrIn->re * accuWin + ptrIn->im * (1 - accuWin);// <- real part is from "old" weights
			accu *= handle->common.normalization;
			*ptrOla = *ptrOla + accu;
			ptrIn++;
			ptrWin_start++;
			ptrOla++;
		}

		ptrOla = handle->ram.olabuffer;

		for (i = 0; i < ll2_0; i++)
		{
			accuWin = *ptrWin_start;
			accu = ptrIn->re * accuWin + ptrIn->im * (1 - accuWin);// <- real part is from "old" weights
			accu *= handle->common.normalization;
			*ptrOutOut++ = *ptrOla + accu;
			*ptrOla = 0.0;
			ptrIn++;
			ptrWin_start++;
			ptrOla++;
		}
		for (i = 0; i < ll2_1; i++)
		{
			accuWin = *ptrWin_start;
			accu = ptrIn->re * accuWin + ptrIn->im * (1 - accuWin);// <- real part is from "old" weights
			accu *= handle->common.normalization;
			*ptrOla = *ptrOla + accu;
			ptrIn++;
			ptrWin_start++;
			ptrOla++;
		}
		handle->common.common.control.idxBufferRW = (idxW + handle->common.common.framesize) % handle->common.common.fftSize;

	}
	break;
	case JVX_FFT_TOOLS_FWK_OLS:
	{
		jvx_tools_hdl_ifft_fwk_ols* handle = (jvx_tools_hdl_ifft_fwk_ols*)hdl;

		jvxSize idxW;
		jvxData* ptrOutOut;
		jvxData* ptrIn;

		jvxSize ll1;
		jvxSize ll2;

		jvx_execute_ifft(handle->common.common.hdlFftIfft);

		idxW = handle->common.common.control.idxBufferRW;
		ptrOutOut = handle->ram.outputReal;
		ptrIn = handle->linkedram.fftbuffer + idxW;

		if (out_real)
			ptrOutOut = out_real;
		assert(ptrOutOut);

		ll1 = JVX_MIN(handle->common.common.fftSize - idxW, handle->common.common.framesize);
		ll2 = handle->common.common.framesize - ll1;

		for (i = 0; i < ll1; i++)
		{
			*ptrOutOut++ = *ptrIn++ * handle->common.normalization;
		}
		ptrIn = handle->linkedram.fftbuffer;
		for (i = 0; i < ll2; i++)
		{
			*ptrOutOut++ = *ptrIn++  * handle->common.normalization;
		}
		handle->common.common.control.idxBufferRW = (idxW + handle->common.common.framesize) % handle->common.common.fftSize;
	}
	break;
	case JVX_FFT_TOOLS_FWK_OLS_CF:
	{
		jvx_tools_hdl_ifft_fwk_ols_cf* handle = (jvx_tools_hdl_ifft_fwk_ols_cf*)hdl;

		jvxDataCplx* ptrTarget_firstHalf = handle->linkedram.ifftInput;
		jvxDataCplx* ptrTarget_secondHalf = handle->linkedram.ifftInput + handle->common.common.fftSize;
		jvxDataCplx* ptrSource_old = handle->ram.inputCplx_old;
		jvxDataCplx* ptrSource_new = handle->ram.inputCplx_new;

		jvxSize idxW;
		jvxData* ptrOutOut;
		jvxDataCplx* ptrIn;
		jvxData* ptrWin_start;
		jvxData* ptrWin_end;

		jvxSize ll1;
		jvxSize ll2;

		// Object at position "0"

		// What we do here is to make a complex to complex ifft and to reconstruct
		// the output signal for old and new weights at the same time

		// The Matlab code explans what is actually done: we assume that both spectra are
		// the output from a real-2-complex transform.
		// The Matlab code looks as follows (taken from file jvx_IFFT_frame_transformComplex_ols_cf.m):
		/*
		
			% Both fft buffers have a length of FFTSIZE/2+1
			symmetricFft_old = [fftBuf_short_old conj(fftBuf_short_old(end - 1:-1 : 2))];
			asymmetricFft_new = [(1i * fftBuf_short_new) (1i * conj(fftBuf_short_new(end - 1:-1 : 2)))];

			cplxFft = symmetricFft_old + asymmetricFft_new;

			handle.signals.ifftBuffer = ifft(cplxFft);

			ifftBuffer_old = real(handle.signals.ifftBuffer);
			ifftBuffer_new = imag(handle.signals.ifftBuffer);

			% Spectrum to time domain buffer
			idxR = handle.idxRead;
			delta = 1 / handle.lFRAME;

			idxInWindow_fwd = 1;
			idxInWindow_bwd = handle.lFRAME;

			out = zeros(1, handle.lFRAME);

			% Copy new samples to circular buffer first
			ll1 = min(handle.lFFT - idxR, handle.lFRAME);
			ll2 = handle.lFRAME - ll1;

			for ind = 1:ll1
				% out(ind) = ifftBuffer_new(idxR + ind);
				%        out(ind) = out(ind) * handle.normalization;
				out(ind) = handle.windowHalf(idxInWindow_fwd) * ifftBuffer_old(idxR + ind)...
					+ handle.windowHalf(idxInWindow_bwd) * ifftBuffer_new(idxR + ind);
				out(ind) = out(ind) * handle.normalization;
				idxInWindow_fwd = idxInWindow_fwd + 1;
				idxInWindow_bwd = idxInWindow_bwd - 1;
			end

			if (ll2 > 0)
				for ind = 1:ll2
					% out(ll1 + ind) = ifftBuffer_new(ind);
					%            out(ind) = out(ind) * handle.normalization;
					out(ll1 + ind) = handle.windowHalf(idxInWindow_fwd) * ifftBuffer_old(ind)...
						+ handle.windowHalf(idxInWindow_bwd) * ifftBuffer_new(ind);
					out(ind) = out(ind) * handle.normalization;
					idxInWindow_fwd = idxInWindow_fwd + 1;
					idxInWindow_bwd = idxInWindow_bwd - 1;
				end
				idxR = ll2;
			else
				idxR = idxR + handle.lFRAME;
				if (idxR >= handle.lFFT)
					idxR = idxR - handle.lFFT;
				end
			end
		*/

		ptrTarget_firstHalf->re = ptrSource_old->re - ptrSource_new->im;
		ptrTarget_firstHalf->im = ptrSource_old->im + ptrSource_new->re;
		ptrTarget_firstHalf++;
		ptrSource_old++;
		ptrSource_new++;

		for (i = 0; i < (handle->common.common.fftSize / 2 - 1); i++)
		{
			ptrTarget_firstHalf->re = ptrSource_old->re - ptrSource_new->im;
			ptrTarget_firstHalf->im = ptrSource_old->im + ptrSource_new->re;
			ptrTarget_firstHalf++;

			--ptrTarget_secondHalf;
			ptrTarget_secondHalf->re = ptrSource_old->re + ptrSource_new->im;
			ptrTarget_secondHalf->im = -ptrSource_old->im + ptrSource_new->re;

			ptrSource_new++;
			ptrSource_old++;
		}

		// Object at position "L/2"
		ptrTarget_firstHalf->re = ptrSource_old->re - ptrSource_new->im;
		ptrTarget_firstHalf->im = ptrSource_old->im + ptrSource_new->re;

		// Transform signal
		jvx_execute_ifft(handle->common.common.hdlFftIfft);

		idxW = handle->common.common.control.idxBufferRW;
		ptrOutOut = handle->ram.outputReal;
		
		if (out_real)
			ptrOutOut = out_real;
		assert(ptrOutOut);

		ptrIn = handle->linkedram.ifftOutput + idxW;
		ptrWin_start = handle->ram.window;
		ptrWin_end = handle->ram.window + handle->common.common.framesize;

		ll1 = JVX_MIN(handle->common.common.fftSize - idxW, handle->common.common.framesize);
		ll2 = handle->common.common.framesize - ll1;

		for (i = 0; i < ll1; i++)
		{
			--ptrWin_end;
			accu = ptrIn->re * *ptrWin_start + ptrIn->im * *ptrWin_end;// <- real part is from "old" weights
			accu *= handle->common.normalization;
			*ptrOutOut++ = accu;
			ptrIn++;
			ptrWin_start++;
		}
		ptrIn = handle->linkedram.ifftOutput;
		for (i = 0; i < ll2; i++)
		{
			--ptrWin_end;
			accu = ptrIn->re * *ptrWin_start + ptrIn->im * *ptrWin_end;
			accu *= handle->common.normalization;
			*ptrOutOut++ = accu;
			ptrIn++;
			ptrWin_start++;
		}
		handle->common.common.control.idxBufferRW = (idxW + handle->common.common.framesize) % handle->common.common.fftSize;

	}
	break;
	case JVX_FFT_TOOLS_FWK_OLS_IFCF:
	{
		jvx_tools_hdl_ifft_fwk_ols_ifcf* handle = (jvx_tools_hdl_ifft_fwk_ols_ifcf*)hdl;

		jvxSize idxW;
		jvxData* ptrOutOut;
		jvxData* ptrIn;

		jvxSize ll1;
		jvxSize ll2;

		jvx_execute_ifft(handle->ols.common.common.hdlFftIfft);

		idxW = handle->ols.common.common.control.idxBufferRW;
		ptrOutOut = handle->ols.ram.outputReal;
		ptrIn = handle->ols.linkedram.fftbuffer + idxW;

		if (out_real)
			ptrOutOut = out_real;
		assert(ptrOutOut);

		ll1 = JVX_MIN(handle->ols.common.common.fftSize - idxW, handle->ols.common.common.framesize);
		ll2 = handle->ols.common.common.framesize - ll1;

		for (i = 0; i < ll1; i++)
		{
			*ptrOutOut++ = *ptrIn++ * handle->ols.common.normalization;
		}
		ptrIn = handle->ols.linkedram.fftbuffer;
		for (i = 0; i < ll2; i++)
		{
			*ptrOutOut++ = *ptrIn++  * handle->ols.common.normalization;
		}

		ifcf_length = JVX_MIN(ifcf_length, handle->save_data_ll);
		jvxData increment = 1.0 / (jvxData)ifcf_length;
		jvxData fac = 0.0;

		ptrOutOut = handle->ols.ram.outputReal;
		if (out_real)
			ptrOutOut = out_real;

		for(i = 0; i < ifcf_length; i++)
		{ 
			*ptrOutOut = *ptrOutOut * fac + handle->save_data[i] * (1.0 - fac);
			fac += increment;
			ptrOutOut++;
		}

		idxW = (idxW + handle->ols.common.common.framesize) % handle->ols.common.common.fftSize;
		handle->ols.common.common.control.idxBufferRW = idxW;

		ptrIn = handle->ols.linkedram.fftbuffer + idxW;
		ptrOutOut = handle->save_data;

		ll1 = JVX_MIN(handle->ols.common.common.fftSize - idxW, handle->save_data_ll);
		ll2 = handle->save_data_ll - ll1;
		for (i = 0; i < ll1; i++)
		{
			*ptrOutOut++ = *ptrIn++ * handle->ols.common.normalization;
		}
		ptrIn = handle->ols.linkedram.fftbuffer;
		for (i = 0; i < ll2; i++)
		{
			*ptrOutOut++ = *ptrIn++  * handle->ols.common.normalization;
		}
		// 
	}
	break;
	default:
		assert(0);
	}
	return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType
jvx_fft_tools_init_modify_weights_no_aliasing(jvxFFTweightMod** hdlOut,
	jvxFFT* coreFftRef,
	jvxIFFT* coreIfftRef,
	jvxDataCplx** input,
	jvxDataCplx** output,
	jvxInt32* numElements_inputoutput,
	jvxInt32 desiredNumFilterCoefficients,
	jvx_windowType winType)
{
	jvx_fft_ifft_core_common* coreFft = (jvx_fft_ifft_core_common*)coreFftRef;
	jvx_fft_ifft_core_common* coreIfft = (jvx_fft_ifft_core_common*)coreIfftRef;

	jvx_tools_hdl_mod_weights_no_alias* handle;

	jvx_fft_core_real_2_complex* ptrFftCore;
	jvx_ifft_core_complex_2_real* ptrIfftCore;

	if (!(hdlOut && coreFft && coreIfft && input && output && numElements_inputoutput))
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	ptrFftCore = (jvx_fft_core_real_2_complex*)coreFft;
	ptrIfftCore = (jvx_ifft_core_complex_2_real*)coreIfft;

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, jvx_tools_hdl_mod_weights_no_alias);

	if (!(
		(coreIfft->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL) &&
		(coreFft->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)))
	{
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	handle->desiredNumFilterCoeffs = desiredNumFilterCoefficients;
	if (handle->desiredNumFilterCoeffs % 2 == 0)
	{
		handle->desiredNumFilterCoeffs--;
	}
	handle->fftCore = coreFft;
	handle->ifftCore = coreIfft;

	handle->fftSize = coreFft->fftParameters.fftSize;
	if (handle->fftSize != coreIfft->fftParameters.fftSize)
	{
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	handle->help.ll1 = (handle->desiredNumFilterCoeffs - 1) / 2;
	handle->help.ll2 = handle->fftSize - (2 * handle->help.ll1 + 1);
	handle->help.ll3 = handle->fftSize - handle->help.ll1;
	handle->ir_size = (2 * handle->help.ll1 + 1);

	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.window, jvxData, handle->ir_size);
	jvx_compute_window(handle->ram.window, handle->ir_size, 0, 0, winType, NULL);

	handle->linkedram.inputCplx = ptrIfftCore->input;
	handle->linkedram.outputCplx = ptrFftCore->output;

	handle->normalization = jvxFftTools_ifftNormFactors[ptrFftCore->common.fftParameters.fftType];

	handle->linkedram.timeDomainBuffer_beforeMod = ptrIfftCore->output;
	handle->linkedram.timeDomainBuffer_afterMod = ptrFftCore->input;

	*input = handle->linkedram.inputCplx;
	*output = handle->linkedram.outputCplx;
	*numElements_inputoutput = handle->fftSize / 2 + 1;
	*hdlOut = handle;
	return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType
jvx_fft_tools_terminate_modify_weights_no_aliasing(jvxFFTweightMod* hdl)
{
	jvx_tools_hdl_mod_weights_no_alias* handle;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	handle = (jvx_tools_hdl_mod_weights_no_alias*)hdl;

	handle->desiredNumFilterCoeffs = 0;
	handle->fftCore = NULL;
	handle->ifftCore = NULL;

	handle->fftSize = 0;

	handle->linkedram.inputCplx = NULL;
	handle->linkedram.outputCplx = NULL;

	handle->linkedram.timeDomainBuffer_beforeMod = NULL;
	handle->linkedram.timeDomainBuffer_afterMod = NULL;

	JVX_DSP_SAFE_DELETE_FIELD(handle->ram.window);
	handle->ram.window = NULL;

	handle->ir_size = 0;

	JVX_DSP_SAFE_DELETE_OBJECT(handle);

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fft_tools_process_modify_weights_no_aliasing(jvxFFTweightMod* hdl)
{
	int i;
	jvx_tools_hdl_mod_weights_no_alias* handle;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	handle = (jvx_tools_hdl_mod_weights_no_alias*)hdl;

	jvx_execute_ifft(handle->ifftCore);

	// Circular shift plus setting zero
	memcpy(handle->linkedram.timeDomainBuffer_afterMod,
		handle->linkedram.timeDomainBuffer_beforeMod + handle->help.ll3,
		sizeof(jvxData) * handle->help.ll1);
	memcpy(handle->linkedram.timeDomainBuffer_afterMod + handle->help.ll1, handle->linkedram.timeDomainBuffer_beforeMod,
		sizeof(jvxData) * (handle->help.ll1 + 1));
	memset(handle->linkedram.timeDomainBuffer_afterMod + handle->ir_size, 0, sizeof(jvxData) * handle->help.ll2);

	for (i = 0; i < handle->ir_size; i++)
	{
		handle->linkedram.timeDomainBuffer_afterMod[i] *= handle->normalization * handle->ram.window[i];
	}
	jvx_execute_fft(handle->fftCore);

	return JVX_DSP_NO_ERROR;
}

jvxSize
jvx_fft_tools_compute_num_filter_coeffs_frameworks(jvxSize fftSize, jvxSize hopsize, jvxSize buffersize, jvxFftTools_fwkType fftFrameworkType, jvxSize ifcf_length)
{
	jvxSize retVal = 0;
	switch (fftFrameworkType)
	{
	case JVX_FFT_TOOLS_FWK_OLS:
	case JVX_FFT_TOOLS_FWK_OLS_CF:
		retVal = fftSize - hopsize + 1;
		break;
	case JVX_FFT_TOOLS_FWK_OLS_IFCF:
		retVal = fftSize - hopsize - ifcf_length + 1;
		break;
	default:
		retVal = fftSize - buffersize + 1;
	}
	return (retVal);
}

jvxDspBaseErrorType 
jvx_fft_direct_apply_arbitrary_size(jvxData* inBuf_n, jvxData* outBuf_n2p1, jvxSize n, jvxCBool outlog10)
{
	jvxSize i;
	jvxFFTGlobal* globFft = NULL;
	jvxFFT* myFft = NULL;
	jvxData* inFld = NULL;
	jvxDataCplx* outFld = NULL;
	jvxSize N = 0;
	jvxFFTSize fftInt;
	jvxSize N2, N2P1;

	jvxDspBaseErrorType res = jvx_get_nearest_size_fft(&fftInt, n, JVX_FFT_ROUND_UP, NULL);
	assert(res == JVX_DSP_NO_ERROR);

	jvx_create_fft_ifft_global(&globFft, fftInt);
	jvx_create_fft_real_2_complex(&myFft, globFft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
		&inFld, &outFld, &N, JVX_FFT_IFFT_EFFICIENT, NULL, NULL, n);
	N2 = N / 2;
	N2P1 = N2 + 1;

	memcpy(inFld, inBuf_n, sizeof(jvxData) * n);
	jvx_execute_fft(myFft);
	for (i = 0; i < N2P1; i++)
	{
		outBuf_n2p1[i] = outFld[i].re * outFld[i].re + outFld[i].im * outFld[i].im;
	}
	if (outlog10)
	{
		for (i = 0; i < N2P1; i++)
		{
			outBuf_n2p1[i] = 10 * log10(outBuf_n2p1[i]);
		}
	}

	jvx_destroy_fft(myFft);
	jvx_destroy_fft_ifft_global(globFft);
	inFld = NULL;
	outFld = NULL;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType 
jvx_fft_direct_apply_fixed_size(jvxData* inBuf_n, jvxData* outBuf_n2p1, jvxFFTSize szFft, jvxSize n, jvxCBool outlog10)
{
	jvxSize i;
	jvxFFTGlobal* globFft = NULL;
	jvxFFT* myFft = NULL;
	jvxData* inFld = NULL;
	jvxDataCplx* outFld = NULL;
	jvxSize N = 0;
	jvxFFTSize fftInt;
	jvxSize N2, N2P1;

	jvxDspBaseErrorType res = jvx_get_nearest_size_fft(&fftInt, n, JVX_FFT_ROUND_UP, NULL);
	assert(res == JVX_DSP_NO_ERROR);

	jvx_create_fft_ifft_global(&globFft, fftInt);
	jvx_create_fft_real_2_complex(&myFft, globFft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
		&inFld, &outFld, &N, JVX_FFT_IFFT_EFFICIENT, NULL, NULL, n);
	N2 = N / 2;
	N2P1 = N2 + 1;

	memcpy(inFld, inBuf_n, sizeof(jvxData) * n);
	jvx_execute_fft(myFft);
	for (i = 0; i < N2P1; i++)
	{
		outBuf_n2p1[i] = outFld[i].re * outFld[i].re + outFld[i].im * outFld[i].im;
	}
	if (outlog10)
	{
		for (i = 0; i < N2P1; i++)
		{
			outBuf_n2p1[i] = 10 * log10(outBuf_n2p1[i]);
		}
	}

	jvx_destroy_fft(myFft);
	jvx_destroy_fft_ifft_global(globFft);
	inFld = NULL;
	outFld = NULL;
	return JVX_DSP_NO_ERROR;
}