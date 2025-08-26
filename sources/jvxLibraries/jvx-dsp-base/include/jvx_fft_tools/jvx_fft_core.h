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
 
 #ifndef __JVX_FFT_CORE__H__
#define __JVX_FFT_CORE__H__

//#include "jvx_fft_tools/jvx_fft_tools.h"
#include "jvx_dsp_base.h"



// =====================================================================
JVX_DSP_LIB_BEGIN

typedef jvxHandle jvxFFTGlobal;
typedef jvxHandle jvxFFT;
typedef jvxHandle jvxIFFT;

typedef enum
{
	JVX_FFT_IFFT_PRESERVE_INPUT,
	JVX_FFT_IFFT_EFFICIENT
} jvxFftIfftOperate;

typedef enum
{
	JVX_FFT_ROUND_NEAREST = 0,
	JVX_FFT_ROUND_DOWN = 1,
	JVX_FFT_ROUND_UP = 2
}jvxFftRoundType;

#define JVX_OFFSET_FFT_TYPE_MIN 4

//! Typedefs for FFT purpose type.
typedef enum
{
	JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX = 0,
	JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX,
	JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL,
	JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX
} jvxFftTools_coreFftType;

//! Typedefs for FFT sizes, only power of 2 allowed.
typedef enum
{
	JVX_FFT_TOOLS_FFT_SIZE_16 = 0, // <- must be 2^JVX_OFFSET_FFT_TYPE_MIN
	JVX_FFT_TOOLS_FFT_SIZE_32,
	JVX_FFT_TOOLS_FFT_SIZE_64,
	JVX_FFT_TOOLS_FFT_SIZE_128,
	JVX_FFT_TOOLS_FFT_SIZE_256,
	JVX_FFT_TOOLS_FFT_SIZE_512,
	JVX_FFT_TOOLS_FFT_SIZE_1024,
	JVX_FFT_TOOLS_FFT_SIZE_2048,
	JVX_FFT_TOOLS_FFT_SIZE_4096,
	JVX_FFT_TOOLS_FFT_SIZE_8192,
	JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE
} jvxFFTSize;

#define JVX_FFT_TOOLS_DEFINE_FFT_SIZES static jvxInt32 jvxFFTSize_sizes[] = \
{ \
	16, \
	32, \
	64, \
	128, \
	256, \
	512, \
	1024, \
	2048, \
	4096, \
	8192, \
	-1 \
} ;


// =====================================================================

/**
 * Create global handle for FFT: fftType_max specifies maximal possible length of FFT
 */
jvxDspBaseErrorType jvx_create_fft_ifft_global(jvxFFTGlobal** global_hdl,
					       jvxFFTSize fftType_max, jvxHandle* cfgFftGlobal);

/** 
* Create handle for real to complex FFT
* @param global_hdl: preexisting handle input parameter (see above)
* @param fftType: specifies length of FFT if an enum type between JVX_FFT_TOOLS_FFT_SIZE_16 and JVX_FFT_TOOLS_FFT_SIZE_8192 is chosen;
*      for enum type JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE, the length of the FFT is defined by the last parameter fftSizeArbitrary.
* @param in_ptr_fld_N: return value with pointer to input data field.
* @param out_ptr_fld_N2P1: return value with pointer to output data field.
* @param N: return value with length of FFT.
* @param operate: operating mode of FFT, for JVX_FFT_IFFT_EFFICIENT input data might not be valid anymore after processing
* @param output
* @param input: if set to NULL, memory is handled internally; in_ptr_fld_N and out_ptr_fld_N2P1 return pointers to allocated memory;
*           if output and input point to allocated fields, memory is handled externally, in_ptr_fld_N and out_ptr_fld_N2P1 return pointers output and input.
*			field input and output are overwritten with zeros.
* @param fftSizeArbitrary: is only evaluated, if fftType is set to JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE; in this case it defines the length of the FFT. Is ignored otherwise.
*/
jvxDspBaseErrorType jvx_create_fft_real_2_complex(jvxFFT** hdl,
						  jvxFFTGlobal* global_hdl,
						  jvxFFTSize fftType,
						  jvxData** in_ptr_fld_N,
						  jvxDataCplx** out_ptr_fld_N2P1,
						  jvxSize* NfftSize,
						  jvxFftIfftOperate operate,
						  jvxData* input,
						  jvxDataCplx* output,
						  jvxSize fftSizeArbitrary);

jvxDspBaseErrorType jvx_create_fft_complex_2_complex(jvxFFT** hdl,
						     jvxFFTGlobal* global_hdl,
						     jvxFFTSize fftType,
						     jvxDataCplx** in_ptr_fld_N,
						     jvxDataCplx** out_ptr_fld_N,
						     jvxSize* NfftSize,
						     jvxFftIfftOperate operate,
						     jvxDataCplx* input,
						     jvxDataCplx* output,
							 jvxSize fftSizeArbitrary);

jvxDspBaseErrorType jvx_create_ifft_complex_2_real(jvxIFFT** hdl,
						   jvxFFTGlobal* global_hdl,
						   jvxFFTSize fftType,
						   jvxDataCplx** in_ptr_fld_N2P1,
						   jvxData** out_ptr_fld_N,
						   jvxSize* NfftSize,
						   jvxFftIfftOperate operate,
						   jvxDataCplx* input,
						   jvxData* output,
						   jvxSize fftSizeArbitrary);

jvxDspBaseErrorType jvx_create_ifft_complex_2_complex(jvxIFFT** hdl,
						      jvxFFTGlobal* global_hdl,
						      jvxFFTSize fftType,
						      jvxDataCplx** in_ptr_fld_N,
						      jvxDataCplx** out_ptr_fld_N,
						      jvxSize* NfftSize,
						      jvxFftIfftOperate operate,
						      jvxDataCplx* input,
						      jvxDataCplx* output,
							  jvxSize fftSizeArbitrary);



jvxDspBaseErrorType jvx_execute_fft(jvxFFT* hdl);
jvxDspBaseErrorType jvx_execute_ifft(jvxIFFT* hdl);

jvxDspBaseErrorType jvx_destroy_fft(jvxFFT* hdl);
jvxDspBaseErrorType jvx_destroy_ifft(jvxIFFT* hdl);

jvxDspBaseErrorType jvx_destroy_fft_ifft_global(jvxFFTGlobal* global_hdl);

// =======================================================================================

jvxData* jvx_allocate_fft_buffer_real(jvxSize num_elements);
jvxDataCplx* jvx_allocate_fft_buffer_complex(jvxSize num_elements);
void jvx_free_fft_buffer(void* buffer);

/**
* One shot IFFT for offline use.
*  @param fft_size the length of the FFT.
*  @param in input data array of length fft_size/2+1.
*  @param out output data array of length fft_size.
*  @return error code.
*/
jvxDspBaseErrorType jvx_oneshot_ifft_complex_2_real(jvxSize fft_size, jvxDataCplx* in, jvxData* out, jvxHandle* fftCfgHdl);


// =======================================================================================
jvxDspBaseErrorType jvx_get_nearest_size_fft(jvxFFTSize* szTypeOnReturn, jvxSize n, jvxFftRoundType tp, jvxSize* szOnReturn);
jvxSize jvx_get_fft_size(jvxFFTSize fftSz);

JVX_DSP_LIB_END

#endif
