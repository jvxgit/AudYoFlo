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

#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_fft_tools.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_core_typedefs.h"

// =============================================================================
// =============================================================================
#ifdef JVX_FFT_APPLE

#define JVX_FFT_BUFFERS_ALLOCATE(varName_buf, varName_type, type, type_comp, numElements) \
	assert(sizeof(type_comp) == sizeof(type));			\
	if(varName_buf == NULL)						\
		{							\
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->varName_buf, type, numElements); \
			ptr->common.varName_type = JVX_FFT_IFFT_BUFFER_ALLOCATED; \
		}							\
	else								\
		{							\
			ptr->varName_buf = varName_buf;			\
			ptr->common.varName_type = JVX_FFT_IFFT_BUFFER_EXTERNAL; \
		}							\
	memset(ptr->varName_buf, 0, sizeof(type) * (numElements));

#else

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE

#define JVX_FFT_BUFFERS_ALLOCATE(varName_buf, varName_type, type, type_comp, numElements) \
	assert(sizeof(type_comp) == sizeof(type));			\
	if(varName_buf == NULL)						\
		{							\
			ptr->varName_buf = (type*) fftw_malloc(sizeof(type) * (numElements)); \
			ptr->common.varName_type = JVX_FFT_IFFT_BUFFER_ALLOCATED; \
		}							\
	else								\
		{							\
			ptr->varName_buf = varName_buf;			\
			ptr->common.varName_type = JVX_FFT_IFFT_BUFFER_EXTERNAL; \
		}							\
	memset(ptr->varName_buf, 0, sizeof(type) * (numElements));

#else

#define JVX_FFT_BUFFERS_ALLOCATE(varName_buf, varName_type, type, type_comp, numElements) \
	assert(sizeof(type_comp) == sizeof(type));			\
	if(varName_buf == NULL)						\
		{							\
			ptr->varName_buf = (type*) fftwf_malloc(sizeof(type) * (numElements)); \
			ptr->common.varName_type = JVX_FFT_IFFT_BUFFER_ALLOCATED; \
		}							\
	else								\
		{							\
			ptr->varName_buf = varName_buf;			\
			ptr->common.varName_type = JVX_FFT_IFFT_BUFFER_EXTERNAL; \
		}							\
	memset(ptr->varName_buf, 0, sizeof(type) * (numElements));

#endif
#endif

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
#define JVX_FFTW_FREE fftw_free
#else
#define JVX_FFTW_FREE fftwf_free
#endif


// ####################################################################

static jvxDspBaseErrorType jvx_core_fft_init_common(jvx_fft_ifft_core_common* hdl,
						    jvxFftTools_coreFftType corefftType,
						    jvxFFTSize fftType,
						    jvxFftIfftOperate operate,
							jvxFFTGlobal* glob_ptr,
						    jvxSize fftSizeArbitrary)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;


	if(hdl && glob_ptr)
	{
		hdl->fftParameters.coreFftType = corefftType;

		if(fftType == JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE)
		{
			if (fftSizeArbitrary != 0)
			{
				hdl->fftParameters.fftType = JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE;
				hdl->fftParameters.fftSizeLog = (int)ceil(log((jvxData)fftSizeArbitrary) / log(2.0));
				hdl->fftParameters.fftSize = (jvxInt32)fftSizeArbitrary;
				hdl->fftParameters.acceleratedFft = false;

				// If size matches radix 2, override setting
				if (hdl->fftParameters.fftSize == ((jvxInt32)1 << hdl->fftParameters.fftSizeLog))
				{
					hdl->fftParameters.acceleratedFft = true;
				}
			}
			else
			{
				return JVX_DSP_ERROR_INVALID_SETTING;
			}
		}
		else
		{
			hdl->fftParameters.fftType = fftType;
			hdl->fftParameters.fftSizeLog = (int)fftType + JVX_OFFSET_FFT_TYPE_MIN;
			hdl->fftParameters.fftSize = (jvxInt32) 1 << (hdl->fftParameters.fftSizeLog );
			hdl->fftParameters.acceleratedFft = true;
			assert(hdl->fftParameters.fftSizeLog <= ((jvx_fft_ifft_core_global_common*)glob_ptr)->fftSizeLog);
		}

#ifdef JVX_FFT_APPLE

		// High speed Apple FFT only for specific FFT sizes
		if(hdl->fftParameters.acceleratedFft)
		{
			if(hdl->fftParameters.fftSizeLog > 20)
			{
				hdl->fftParameters.acceleratedFft = false;
			}
		}
#endif

		hdl->glob_ptr = glob_ptr;
		hdl->operate = operate;
		hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
	}
	else
	{
		res = JVX_DSP_ERROR_INVALID_ARGUMENT;
	}
	return res;
}


// =======================================================================================

jvxDspBaseErrorType jvx_create_fft_ifft_global(jvxFFTGlobal** glob_hdl,
					       jvxFFTSize fftType_max, jvxHandle* fftGlobCfg)
{
	jvx_fft_ifft_core_global_common** global_hdl = (jvx_fft_ifft_core_global_common**)glob_hdl;
	jvx_fft_ifft_core_global_common* ptr = NULL;

	if(!global_hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(ptr, jvx_fft_ifft_core_global_common);

	ptr->fftSizeLog = (int)fftType_max + JVX_OFFSET_FFT_TYPE_MIN;
	ptr->refCount = 0;

	*global_hdl = ptr;

#ifdef JVX_FFT_APPLE

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
        ptr->setupFFT = vDSP_create_fftsetupD(ptr->fftSizeLog, FFT_RADIX2);
#else
	ptr->setupFFT = vDSP_create_fftsetup(ptr->fftSizeLog, FFT_RADIX2);
#endif

	return JVX_DSP_NO_ERROR;
#else

	// No global stuff required
	return JVX_DSP_NO_ERROR;
#endif
}

// =======================================================================================

jvxDspBaseErrorType jvx_create_fft_real_2_complex(jvxFFT** hdlRef,
						  jvxFFTGlobal* g_hdl,
						  jvxFFTSize fftType,
						  jvxData** in_ptr_fld_N,
						  jvxDataCplx** out_ptr_fld_N2P1,
						  jvxSize* NfftSize,
						  jvxFftIfftOperate operate,
						  jvxData* input,
						  jvxDataCplx* output,
						  jvxSize fftSizeArbitrary)
{
	jvx_fft_ifft_core_common** hdl = (jvx_fft_ifft_core_common**)hdlRef;
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;

	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvx_fft_core_real_2_complex* ptr = NULL;

	if(hdl)
	{
		if (fftType == JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE)
		{
			if ((fftSizeArbitrary % 2) != 0)
			{
				return(JVX_DSP_ERROR_INVALID_SETTING);
			}
		}

		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(ptr, jvx_fft_core_real_2_complex);
		memset(ptr, 0, sizeof(jvx_fft_core_real_2_complex));

		if(ptr)
		{
			// Specify purpose of fft transform
			res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX, fftType, operate, global_hdl, fftSizeArbitrary);
			assert(res == JVX_DSP_NO_ERROR); // We may setup false settings 

			ptr->common.ref_global_hdl = global_hdl;
			ptr->common.ref_global_hdl->refCount++;

			if(res == JVX_DSP_NO_ERROR)
			{
				// ==============================================================
				// Allocate input and output
				// ==============================================================
#ifdef JVX_FFT_APPLE

				if(!hdl->fftParameters.isPureRadix2)
				{
					assert(0);
				}
				// FOR MAC ACCELERATE FFT
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->splitComplex.realp, jvxData, (ptr->common.fftParameters.fftSize/2));
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->splitComplex.imagp, jvxData, (ptr->common.fftParameters.fftSize/2));

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, DSPDoubleComplex, (ptr->common.fftParameters.fftSize/2+1));
#else
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, DSPComplex, (ptr->common.fftParameters.fftSize/2+1));
#endif
				JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxData, jvxData, ptr->common.fftParameters.fftSize);

#else

				// FOR FFTW
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, fftw_complex, (ptr->common.fftParameters.fftSize/2+1));
#else
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, fftwf_complex, (ptr->common.fftParameters.fftSize/2+1));
#endif
				JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxData, jvxData, ptr->common.fftParameters.fftSize);


				// ==============================================================
				// Create fft handles
				// ==============================================================
				switch(ptr->common.operate)
				{
				case JVX_FFT_IFFT_EFFICIENT:
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
					ptr->common.planFFT = fftw_plan_dft_r2c_1d(ptr->common.fftParameters.fftSize, ptr->input, (fftw_complex*)ptr->output, FFTW_ESTIMATE );
#else
					ptr->common.planFFT = fftwf_plan_dft_r2c_1d(ptr->common.fftParameters.fftSize, ptr->input, (fftwf_complex*)ptr->output, FFTW_ESTIMATE);
#endif
					break;
				case JVX_FFT_IFFT_PRESERVE_INPUT:
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
					ptr->common.planFFT = fftw_plan_dft_r2c_1d(ptr->common.fftParameters.fftSize, ptr->input, (fftw_complex*)ptr->output, FFTW_ESTIMATE| FFTW_PRESERVE_INPUT);
#else
					ptr->common.planFFT= fftwf_plan_dft_r2c_1d(ptr->common.fftParameters.fftSize, ptr->input, (fftwf_complex*)ptr->output, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
#endif
					break;
				default:
					assert(0);
				}

#endif

				if(in_ptr_fld_N)
				{
					*in_ptr_fld_N = ptr->input;
				}
				if(out_ptr_fld_N2P1)
				{
					*out_ptr_fld_N2P1 = ptr->output;
				}
				if(NfftSize)
				{
					*NfftSize = ptr->common.fftParameters.fftSize;
				}
			}
			else
			{
				res = JVX_DSP_ERROR_INTERNAL;
			}
		}
		else
		{
			res = JVX_DSP_ERROR_INTERNAL;
		}
	}
	else
	{
		res = JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	if(res == JVX_DSP_NO_ERROR)
	{
		*hdl = (jvx_fft_ifft_core_common*) ptr;
	}

	return res;
}

jvxDspBaseErrorType jvx_create_fft_complex_2_complex(jvxFFT** hdlRef,
						     jvxFFTGlobal* g_hdl,
						     jvxFFTSize fftType,
						     jvxDataCplx** in_ptr_fld_N,
						     jvxDataCplx** out_ptr_fld_N,
						     jvxSize* NfftSize,
						     jvxFftIfftOperate operate,
						     jvxDataCplx* input,
						     jvxDataCplx* output,
						     jvxSize fftSizeArbitrary)
{
	jvx_fft_ifft_core_common** hdl = (jvx_fft_ifft_core_common**)hdlRef;
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;

	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvx_fft_core_complex_2_complex* ptr = NULL;

	if(hdl)
	{
		if((fftSizeArbitrary % 2) != 0)
		{
			return(JVX_DSP_ERROR_INVALID_SETTING);
		}

		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(ptr, jvx_fft_core_complex_2_complex);
		memset(ptr, 0, sizeof(jvx_fft_core_complex_2_complex));

		if(ptr)
		{
			// Specify purpose of fft transform
			res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX, fftType, operate, global_hdl, fftSizeArbitrary);
			ptr->common.ref_global_hdl = global_hdl;
			ptr->common.ref_global_hdl->refCount++;

			if(res == JVX_DSP_NO_ERROR)
			{
				// Allocate input
#ifdef JVX_FFT_APPLE

				if(!hdl->fftParameters.isPureRadix2)
				{
					assert(0);
				}

				// FOR MAC ACCELERATE FFT
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->splitComplex.realp, jvxData, (ptr->common.fftParameters.fftSize));
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->splitComplex.imagp, jvxData, (ptr->common.fftParameters.fftSize));

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
				JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, DSPDoubleComplex, ptr->common.fftParameters.fftSize);
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, DSPDoubleComplex, ptr->common.fftParameters.fftSize);
#else
				JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, DSPComplex, ptr->common.fftParameters.fftSize);
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, DSPComplex, ptr->common.fftParameters.fftSize);
#endif


#else

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE

				JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, fftw_complex, ptr->common.fftParameters.fftSize);
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, fftw_complex, ptr->common.fftParameters.fftSize);

#else
				JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, fftwf_complex, ptr->common.fftParameters.fftSize);
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, fftwf_complex, ptr->common.fftParameters.fftSize);
#endif

				memset(ptr->input, 0, sizeof(jvxDataCplx)* (ptr->common.fftParameters.fftSize));
				memset(ptr->output, 0, sizeof(jvxDataCplx)* (ptr->common.fftParameters.fftSize));

				switch(ptr->common.operate)
				{
				case JVX_FFT_IFFT_EFFICIENT:
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
					ptr->common.planFFT = fftw_plan_dft_1d(ptr->common.fftParameters.fftSize, (fftw_complex*)ptr->input, (fftw_complex*)ptr->output, FFTW_FORWARD, FFTW_ESTIMATE);
#else
					ptr->common.planFFT = fftwf_plan_dft_1d(ptr->common.fftParameters.fftSize, (fftwf_complex*)ptr->input, (fftwf_complex*)ptr->output, FFTW_FORWARD, FFTW_ESTIMATE);
#endif
					break;
				case JVX_FFT_IFFT_PRESERVE_INPUT:
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
					ptr->common.planFFT = fftw_plan_dft_1d(ptr->common.fftParameters.fftSize, (fftw_complex*)ptr->input, (fftw_complex*)ptr->output, FFTW_FORWARD, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
#else
					ptr->common.planFFT = fftwf_plan_dft_1d(ptr->common.fftParameters.fftSize, (fftwf_complex*)ptr->input, (fftwf_complex*)ptr->output, FFTW_FORWARD, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
#endif
					break;
				default:
					assert(0);
				}
#endif

				if(in_ptr_fld_N)
				{
					*in_ptr_fld_N = ptr->input;
				}
				if(out_ptr_fld_N)
				{
					*out_ptr_fld_N = ptr->output;
				}
				if(NfftSize)
				{
					*NfftSize = ptr->common.fftParameters.fftSize;
				}

			}
			else
			{
				res = JVX_DSP_ERROR_INTERNAL;
			}
		}
		else
		{
			res = JVX_DSP_ERROR_INTERNAL;
		}
	}
	else
	{
		res = JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	if(res == JVX_DSP_NO_ERROR)
	{
		*hdl = (jvx_fft_ifft_core_common*) ptr;
	}

	return res;
}

jvxDspBaseErrorType jvx_create_ifft_complex_2_real(jvxIFFT** hdlRef,
						   jvxFFTGlobal* g_hdl,
						   jvxFFTSize fftType,
						   jvxDataCplx** in_ptr_fld_N2P1,
						   jvxData** out_ptr_fld_N, jvxSize* N,
						   jvxFftIfftOperate operate,
						   jvxDataCplx* input,
						   jvxData* output,
						   jvxSize fftSizeArbitrary)
{
	jvx_fft_ifft_core_common** hdl = (jvx_fft_ifft_core_common**)hdlRef;
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;

	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvx_ifft_core_complex_2_real* ptr = NULL;

	if(hdl)
	{
		if((fftSizeArbitrary % 2) != 0)
		{
			return(JVX_DSP_ERROR_INVALID_SETTING);
		}

		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(ptr, jvx_ifft_core_complex_2_real);
		memset(ptr, 0, sizeof(jvx_ifft_core_complex_2_real));

		if(ptr)
		{
			// Specify purpose of fft transform
			res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL, fftType, operate, global_hdl, fftSizeArbitrary);
			ptr->common.ref_global_hdl = global_hdl;
			ptr->common.ref_global_hdl->refCount++;

			if(res == JVX_DSP_NO_ERROR)
			{
				// Allocate input
#ifdef JVX_FFT_APPLE

				if(!hdl->fftParameters.isPureRadix2)
				{
					assert(0);
				}

				// FOR MAC ACCELERATE FFT
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->splitComplex.realp, jvxData, (ptr->common.fftParameters.fftSize/2));
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->splitComplex.imagp, jvxData, (ptr->common.fftParameters.fftSize/2));

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
				JVX_FFT_BUFFERS_ALLOCATE(input, outputOrigin, jvxDataCplx, DSPDoubleComplex, (ptr->common.fftParameters.fftSize/2+1));
#else
				JVX_FFT_BUFFERS_ALLOCATE(input, outputOrigin, jvxDataCplx, DSPComplex, (ptr->common.fftParameters.fftSize/2+1));
#endif
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxData, jvxData, ptr->common.fftParameters.fftSize);

#else

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
				JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, fftw_complex, ptr->common.fftParameters.fftSize/2+1);
#else
				JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, fftwf_complex, ptr->common.fftParameters.fftSize/2+1);
#endif

				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxData, jvxData, ptr->common.fftParameters.fftSize);

				switch(ptr->common.operate)
				{
				case JVX_FFT_IFFT_EFFICIENT:
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
					ptr->common.planFFT = fftw_plan_dft_c2r_1d(ptr->common.fftParameters.fftSize, (fftw_complex*)ptr->input, ptr->output, FFTW_ESTIMATE);
#else
					ptr->common.planFFT = fftwf_plan_dft_c2r_1d(ptr->common.fftParameters.fftSize, (fftwf_complex*)ptr->input, ptr->output, FFTW_ESTIMATE);
#endif
					break;
				case JVX_FFT_IFFT_PRESERVE_INPUT:
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
					ptr->common.planFFT = fftw_plan_dft_c2r_1d(ptr->common.fftParameters.fftSize, (fftw_complex*)ptr->input, ptr->output, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
#else
					ptr->common.planFFT = fftwf_plan_dft_c2r_1d(ptr->common.fftParameters.fftSize, (fftwf_complex*)ptr->input, ptr->output, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
#endif
					break;
				default:
					assert(0);
				}
#endif // #ifdef JVX_FFT_APPLE

				if(in_ptr_fld_N2P1)
				{
					*in_ptr_fld_N2P1 = ptr->input;
				}
				if(out_ptr_fld_N)
				{
					*out_ptr_fld_N = ptr->output;
				}
				if(N)
				{
					*N = ptr->common.fftParameters.fftSize;
				}

			}
			else
			{
				res = JVX_DSP_ERROR_INTERNAL;
			}
		}
		else
		{
			res = JVX_DSP_ERROR_INTERNAL;
		}
	}
	else
	{
		res = JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	if(res == JVX_DSP_NO_ERROR)
	{
		*hdl = (jvx_fft_ifft_core_common*) ptr;
	}

	return res;
}

jvxDspBaseErrorType jvx_create_ifft_complex_2_complex(jvxIFFT** hdlRef,
						      jvxFFTGlobal* g_hdl,
						      jvxFFTSize fftType,
						      jvxDataCplx** in_ptr_fld_N,
						      jvxDataCplx** out_ptr_fld_N,
						      jvxSize* N,
						      jvxFftIfftOperate operate,
						      jvxDataCplx* input,
						      jvxDataCplx* output,
						      jvxSize fftSizeArbitrary)
{
	jvx_fft_ifft_core_common** hdl = (jvx_fft_ifft_core_common**)hdlRef;
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;

	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvx_ifft_core_complex_2_complex* ptr = NULL;

	if(hdl)
	{
		if((fftSizeArbitrary % 2) != 0)
		{
			return(JVX_DSP_ERROR_INVALID_SETTING);
		}

		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(ptr, jvx_ifft_core_complex_2_complex);
		memset(ptr, 0, sizeof(jvx_ifft_core_complex_2_complex));

		if(ptr)
		{
			// Specify purpose of fft transform
			res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX, fftType, operate, global_hdl, fftSizeArbitrary);

			ptr->common.ref_global_hdl = global_hdl;
			ptr->common.ref_global_hdl->refCount++;

			if(res == JVX_DSP_NO_ERROR)
			{
				// Allocate input

#ifdef JVX_FFT_APPLE

				if(!hdl->fftParameters.isPureRadix2)
				{
					assert(0);
				}

				// FOR MAC ACCELERATE FFT
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->splitComplex.realp, jvxData, (ptr->common.fftParameters.fftSize));
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->splitComplex.imagp, jvxData, (ptr->common.fftParameters.fftSize));

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
				JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, DSPDoubleComplex, ptr->common.fftParameters.fftSize);
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, DSPDoubleComplex, ptr->common.fftParameters.fftSize);
#else
				JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, DSPComplex, ptr->common.fftParameters.fftSize);
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, DSPComplex, ptr->common.fftParameters.fftSize);
#endif


#else

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
				JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, fftw_complex, ptr->common.fftParameters.fftSize);
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, fftw_complex, ptr->common.fftParameters.fftSize);
#else
				JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, fftwf_complex, ptr->common.fftParameters.fftSize);
				JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, fftwf_complex, ptr->common.fftParameters.fftSize);
#endif

				switch(ptr->common.operate)
				{
				case JVX_FFT_IFFT_EFFICIENT:
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
					ptr->common.planFFT = fftw_plan_dft_1d(ptr->common.fftParameters.fftSize, (fftw_complex*)ptr->input, (fftw_complex*)ptr->output, FFTW_BACKWARD, FFTW_ESTIMATE);
#else
					ptr->common.planFFT = fftwf_plan_dft_1d(ptr->common.fftParameters.fftSize, (fftwf_complex*)ptr->input, (fftwf_complex*)ptr->output, FFTW_BACKWARD, FFTW_ESTIMATE);
#endif
					break;
				case JVX_FFT_IFFT_PRESERVE_INPUT:
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
					ptr->common.planFFT = fftw_plan_dft_1d(ptr->common.fftParameters.fftSize, (fftw_complex*)ptr->input, (fftw_complex*)ptr->output, FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
#else
					ptr->common.planFFT = fftwf_plan_dft_1d(ptr->common.fftParameters.fftSize, (fftwf_complex*)ptr->input, (fftwf_complex*)ptr->output, FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
#endif
					break;
				default:
					assert(0);
				}
#endif // #ifdef JVX_FFT_APPLE

				if(in_ptr_fld_N)
				{
					*in_ptr_fld_N = ptr->input;
				}
				if(out_ptr_fld_N)
				{
					*out_ptr_fld_N = ptr->output;
				}
				if(N)
				{
					*N = ptr->common.fftParameters.fftSize;
				}

			}
			else
			{
				res = JVX_DSP_ERROR_INTERNAL;
			}
		}
		else
		{
			res = JVX_DSP_ERROR_INTERNAL;
		}
	}
	else
	{
		res = JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	if(res == JVX_DSP_NO_ERROR)
	{
		*hdl = (jvx_fft_ifft_core_common*) ptr;
	}

	return res;
}





jvxDspBaseErrorType jvx_execute_fft(jvxFFT* hdlRef)
{
	jvx_fft_ifft_core_common* hdl = (jvx_fft_ifft_core_common*)hdlRef;
	jvx_fft_core_real_2_complex* hdl_fft_r2c = (jvx_fft_core_real_2_complex*)hdl;
	jvx_fft_core_complex_2_complex* hdl_fft_c2c = (jvx_fft_core_complex_2_complex*)hdl;
	jvx_ifft_core_complex_2_complex* hdl_ifft_c2c = (jvx_ifft_core_complex_2_complex*)hdl;
	jvx_ifft_core_complex_2_real* hdl_ifft_c2r = (jvx_ifft_core_complex_2_real*)hdl;

#ifdef JVX_FFT_APPLE
	switch(hdl->fftParameters.fftType)
		{
		case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX:

			// This approach always preserves the input (that is: input field) as we do a copy at first

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE

			// Transform linear buffer of real values into a even-odd representation. Use function ctoz sincle it does exactly this
			vDSP_ctozD((DSPDoubleComplex *) hdl_fft_r2c->input, 2, &hdl_fft_r2c->splitComplex, 1, hdl_fft_r2c->common.fftParameters.fftSize/2);

			// Do a real to complex fft
			vDSP_fft_zripD(hdl_fft_r2c->common.glob_ptr->setupFFT, &hdl_fft_r2c->splitComplex, 1, hdl_fft_r2c->common.fftParameters.fftSizeLog, FFT_FORWARD);

			// Re-transform the split vector into a linear array again
			vDSP_ztocD(&hdl_fft_r2c->splitComplex, 1, (DSPDoubleComplex*)hdl_fft_r2c->output, 2, hdl_fft_c2c->common.fftParameters.fftSize/2);

			// The dc and nyquist values are considered separately
			hdl_fft_r2c->output[hdl_fft_r2c->common.fftParameters.fftSize/2].re = hdl_fft_r2c->output[hdl_fft_r2c->common.fftParameters.fftSize/2].im;
			hdl_fft_r2c->output[0].im = 0;

#else // JVX_DSP_DATA_FORMAT_DOUBLE

			// Transform linear buffer of real values into a even-odd representation. Use function ctoz sincle it does exactly this
			vDSP_ctoz((DSPComplex *) hdl_fft_r2c->input, 2, &hdl_fft_r2c->splitComplex, 1, hdl_fft_r2c->common.fftParameters.fftSize/2);

			// Do a real to complex fft
			vDSP_fft_zrip(hdl_fft_r2c->common.glob_ptr->setupFFT, &hdl_fft_r2c->splitComplex, 1, hdl_fft_r2c->common.fftParameters.fftSizeLog, FFT_FORWARD);

			// Re-transform the split vector into a linear array again
			vDSP_ztoc(&hdl_fft_r2c->splitComplex, 1, (DSPComplex*)hdl_fft_r2c->output, 2, hdl_fft_r2c->common.fftParameters.fftSize/2);

			// The dc and nyquist values are considered separately
			hdl_fft_r2c->output[hdl_fft_r2c->common.fftParameters.fftSize/2].re = hdl_fft_r2c->output[hdl_fft_r2c->common.fftParameters.fftSize/2].im;
			hdl_fft_r2c->output[0].im = 0;

#endif // JVX_DSP_DATA_FORMAT_DOUBLE
			break;

		case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX:

			// This approach always preserves the input (that is: input field) as we do a copy at first

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE

			// Transform linear buffer of real values into a even-odd representation. Use function ctoz sincle it does exactly this
			vDSP_ctozD((DSPDoubleComplex *) hdl_fft_c2c->input, 2, &hdl_fft_c2c->splitComplex, 1, hdl_fft_c2c->common.fftParameters.fftSize);

			// Do a real to complex fft
			vDSP_fft_zipD(hdl_fft_c2c->common.glob_ptr->setupFFT, &hdl_fft_c2c->splitComplex, 1, hdl_fft_c2c->common.fftParameters.fftSizeLog, FFT_FORWARD);

			// Re-transform the split vector into a linear array again
			vDSP_ztocD(&hdl_fft_c2c->splitComplex, 1, (DSPDoubleComplex*)hdl_fft_c2c->output, 2, hdl_fft_c2c->common.fftParameters.fftSize);

#else // JVX_DSP_DATA_FORMAT_DOUBLE

			// Transform linear buffer of real values into a even-odd representation. Use function ctoz sincle it does exactly this
			vDSP_ctoz((DSPComplex *) hdl_fft_c2c->input, 2, &hdl_fft_c2c->splitComplex, 1, hdl_fft_c2c->common.fftParameters.fftSize);

			// Do a real to complex fft
			vDSP_fft_zip(hdl_fft_c2c->common.glob_ptr->setupFFT, &hdl_fft_c2c->splitComplex, 1, hdl_fft_c2c->common.fftParameters.fftSizeLog, FFT_FORWARD);

			// Re-transform the split vector into a linear array again
			vDSP_ztoc(&hdl_fft_c2c->splitComplex, 1, (DSPComplex*)hdl_fft_c2c->output, 2, hdl_fft_c2c->common.fftParameters.fftSize);

#endif // JVX_DSP_DATA_FORMAT_DOUBLE
			break;

		case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL:

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE

			// The dc and nyquist values are considered separately

			// Transform linear buffer of real values into a even-odd representation. Use function ctoz sincle it does exactly this
			vDSP_ctozD((DSPDoubleComplex *) hdl_ifft_c2r->input, 2, &hdl_ifft_c2r->splitComplex, 1, hdl_ifft_c2r->common.fftParameters.fftSize/2);

			// Do a real to complex fft
			vDSP_fft_zripD(hdl_ifft_c2r->common.glob_ptr->setupFFT, &hdl_ifft_c2r->splitComplex, 1, hdl_ifft_c2r->common.fftParameters.fftSizeLog, FFT_INVERSE);

			// Re-transform the split vector into a linear array again
			vDSP_ztocD(&hdl_ifft_c2r->splitComplex, 1, (DSPDoubleComplex*)hdl_ifft_c2r->output, 2, hdl_ifft_c2r->common.fftParameters.fftSize/2);


#else

			// The dc and nyquist values are considered separately

			// Transform linear buffer of real values into a even-odd representation. Use function ctoz sincle it does exactly this
			vDSP_ctoz((DSPComplex *) hdl_ifft_c2r->input, 2, &hdl_ifft_c2r->splitComplex, 1, hdl_ifft_c2r->common.fftParameters.fftSize/2);

			// Do a real to complex fft
			vDSP_fft_zrip(hdl_ifft_c2r->common.glob_ptr->setupFFT, &hdl_ifft_c2r->splitComplex, 1, hdl_ifft_c2r->common.fftParameters.fftSizeLog, FFT_INVERSE);

			// Re-transform the split vector into a linear array again
			vDSP_ztoc(&hdl_ifft_c2r->splitComplex, 1, (DSPComplex*)hdl_ifft_c2r->output, 2, hdl_ifft_c2r->common.fftParameters.fftSize/2);


#endif
			break;

		case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX:

			// This approach always preserves the input (that is: input field) as we do a copy at first

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE

			// Transform linear buffer of real values into a even-odd representation. Use function ctoz sincle it does exactly this
			vDSP_ctozD((DSPDoubleComplex *) hdl_ifft_c2c->input, 2, &hdl_ifft_c2c->splitComplex, 1, hdl_ifft_c2c->common.fftParameters.fftSize);

			// Do a real to complex fft
			vDSP_fft_zipD(hdl_ifft_c2c->common.glob_ptr->setupFFT, &hdl_ifft_c2c->splitComplex, 1, hdl_ifft_c2c->common.fftParameters.fftSizeLog, FFT_INVERSE);

			// Re-transform the split vector into a linear array again
			vDSP_ztocD(&hdl_ifft_c2c->splitComplex, 1, (DSPDoubleComplex*)hdl_ifft_c2c->output, 2, hdl_fft_c2c->common.fftParameters.fftSize);

#else

			// Transform linear buffer of real values into a even-odd representation. Use function ctoz sincle it does exactly this
			vDSP_ctoz((DSPComplex *) hdl_ifft_c2c->input, 2, &hdl_ifft_c2c->splitComplex, 1, hdl_ifft_c2c->common.fftParameters.fftSize);

			// Do a real to complex fft
			vDSP_fft_zip(hdl_ifft_c2c->common.glob_ptr->setupFFT, &hdl_ifft_c2c->splitComplex, 1, hdl_ifft_c2c->common.fftParameters.fftSizeLog, FFT_INVERSE);

			// Re-transform the split vector into a linear array again
			vDSP_ztoc(&hdl_ifft_c2c->splitComplex, 1, (DSPComplex*)hdl_ifft_c2c->output, 2, hdl_fft_c2c->common.fftParameters.fftSize);

#endif
			break;
		default:
			assert(0);

		}

#else // JVX_FFT_APPLE

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
	fftw_execute(hdl->planFFT);
#else // JVX_DSP_DATA_FORMAT_DOUBLE
	fftwf_execute(hdl->planFFT);
#endif // JVX_DSP_DATA_FORMAT_DOUBLE

#endif // JVX_FFT_APPLE
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_execute_ifft(jvxIFFT* hdlRef)
{
	return jvx_execute_fft((jvxFFT*)hdlRef); // FIXME: use this for the time being (as long as we do not have real typed handles)
}

jvxDspBaseErrorType jvx_destroy_fft(jvxFFT* hdlRef)
{
	jvx_fft_ifft_core_common* hdl = (jvx_fft_ifft_core_common*)hdlRef;

	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;

	jvx_fft_core_real_2_complex* hdl_fft_r2c = (jvx_fft_core_real_2_complex*)hdl;
	jvx_fft_core_complex_2_complex* hdl_fft_c2c = (jvx_fft_core_complex_2_complex*)hdl;
	jvx_ifft_core_complex_2_complex* hdl_ifft_c2c = (jvx_ifft_core_complex_2_complex*)hdl;
	jvx_ifft_core_complex_2_real* hdl_ifft_c2r = (jvx_ifft_core_complex_2_real*)hdl;

	if(!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	switch(hdl->fftParameters.coreFftType)
		{
		case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX:
			if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
				{
#ifdef JVX_FFT_APPLE
					JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_r2c->input);
#else // JVX_FFT_APPLE

					JVX_FFTW_FREE(hdl_fft_r2c->input);

#endif // JVX_FFT_APPLE
				}
			if(hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
				{
#ifdef JVX_FFT_APPLE
					JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_r2c->output);
#else // JVX_FFT_APPLE

					JVX_FFTW_FREE(hdl_fft_r2c->output);


#endif // JVX_FFT_APPLE

				}

			hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
			hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
			hdl_fft_r2c->input = NULL;
			hdl_fft_r2c->output = NULL;

#ifdef JVX_FFT_APPLE
			JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_r2c->splitComplex.realp);
			hdl_fft_r2c->splitComplex.realp = NULL;
			JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_r2c->splitComplex.imagp);
			hdl_fft_r2c->splitComplex.imagp = NULL;
#endif // JVX_FFT_APPLE
			break;
		case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX:
			if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
				{
#ifdef JVX_FFT_APPLE
					JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_c2c->input);
#else // JVX_FFT_APPLE

					JVX_FFTW_FREE(hdl_fft_c2c->input);

#endif // JVX_FFT_APPLE
				}
			if(hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
				{
#ifdef JVX_FFT_APPLE
					JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_c2c->output);
#else // JVX_FFT_APPLE

					JVX_FFTW_FREE(hdl_fft_c2c->output);

#endif // JVX_FFT_APPLE
				}
			hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
			hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
			hdl_fft_c2c->input = NULL;
			hdl_fft_c2c->output = NULL;

#ifdef JVX_FFT_APPLE
			JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_c2c->splitComplex.realp);
			hdl_fft_c2c->splitComplex.realp = NULL;
			JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_c2c->splitComplex.imagp);
			hdl_fft_c2c->splitComplex.imagp = NULL;
#endif // JVX_FFT_APPLE
			break;
		case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL:
			if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
				{
#ifdef JVX_FFT_APPLE
					JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2r->input);
#else // JVX_FFT_APPLE
					JVX_FFTW_FREE(hdl_ifft_c2r->input);
#endif // JVX_FFT_APPLE
				}
			if(hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
				{
#ifdef JVX_FFT_APPLE
					JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2r->output);
#else // JVX_FFT_APPLE
					JVX_FFTW_FREE(hdl_ifft_c2r->output);
#endif // JVX_FFT_APPLE
				}
			hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
			hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
			hdl_ifft_c2r->input = NULL;
			hdl_ifft_c2r->output = NULL;

#ifdef JVX_FFT_APPLE
			JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2r->splitComplex.realp);
			hdl_ifft_c2r->splitComplex.realp = NULL;
			JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2r->splitComplex.imagp);
			hdl_ifft_c2r->splitComplex.imagp = NULL;
#endif // JVX_FFT_APPLE
			break;
		case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX:
			if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
				{
#ifdef JVX_FFT_APPLE
					JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2c->input);
#else // JVX_FFT_APPLE
					JVX_FFTW_FREE(hdl_ifft_c2c->input);
#endif //JVX_FFT_APPLE
				}
			if(hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
				{
#ifdef JVX_FFT_APPLE
					JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2c->output);
#else //JVX_FFT_APPLE
					JVX_FFTW_FREE(hdl_ifft_c2c->output);
#endif //JVX_FFT_APPLE
				}
			hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
			hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
			hdl_ifft_c2c->input = NULL;
			hdl_ifft_c2c->output = NULL;

#ifdef JVX_FFT_APPLE
			JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2c->splitComplex.realp);
			hdl_ifft_c2c->splitComplex.realp = NULL;
			JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2c->splitComplex.imagp);
			hdl_ifft_c2c->splitComplex.imagp = NULL;
#endif // JVX_FFT_APPLE
			break;
		default:
			assert(0);
		}

#ifndef JVX_FFT_APPLE

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
	fftw_destroy_plan(hdl->planFFT);
#else // JVX_DSP_DATA_FORMAT_DOUBLE
	fftwf_destroy_plan(hdl->planFFT);
#endif // JVX_DSP_DATA_FORMAT_DOUBLE

#endif // JVX_FFT_APPLE
	hdl->ref_global_hdl->refCount--;

	return res;
}

jvxDspBaseErrorType jvx_destroy_ifft(jvxIFFT* hdlRef)
{
	return jvx_destroy_fft((jvxFFT*)hdlRef);
}

jvxDspBaseErrorType jvx_destroy_fft_ifft_global(jvxFFTGlobal* g_hdl)
{
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;

	if(global_hdl)
		{
#ifdef JVX_FFT_APPLE

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
			vDSP_destroy_fftsetupD(global_hdl->setupFFT);
#else
			vDSP_destroy_fftsetup(global_hdl->setupFFT);
#endif

#endif

			// All ffts released?
			assert(global_hdl->refCount == 0);

			JVX_DSP_SAFE_DELETE_OBJECT(global_hdl);

			return JVX_DSP_NO_ERROR;
		}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxData * jvx_allocate_fft_buffer_real(jvxSize num_elements)
{
	jvxData* buffer;
	
#ifdef JVX_FFT_APPLE
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(buffer, jvxData, num_elements);
#else
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
	buffer = (jvxData*)fftw_malloc(sizeof(jvxData) * (num_elements));
#else
	buffer = (jvxData*)fftwf_malloc(sizeof(jvxData) * (num_elements));
#endif
#endif
	return buffer;
}

jvxDataCplx * jvx_allocate_fft_buffer_complex(jvxSize num_elements)
{
	jvxDataCplx* buffer;
#ifdef JVX_FFT_APPLE
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(buffer, jvxDataCplx, num_elements);
#else
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
	buffer = (jvxDataCplx*)fftw_malloc(sizeof(jvxDataCplx) * (num_elements));
#else
	buffer = (jvxDataCplx*)fftwf_malloc(sizeof(jvxDataCplx) * (num_elements));
#endif
#endif
	return buffer;
}

void jvx_free_fft_buffer(void * buffer)
{
#ifdef JVX_FFT_APPLE
	JVX_DSP_SAFE_DELETE_FIELD(buffer);
#else
	JVX_FFTW_FREE(buffer);
#endif
}

jvxDspBaseErrorType jvx_oneshot_ifft_complex_2_real(jvxSize fft_size, jvxDataCplx * in, jvxData * out, jvxHandle* fftCfgHdl)
{		
	jvxSize dummy = 0;

	jvxFFTGlobal* fft_global;
	jvx_create_fft_ifft_global(&fft_global, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE, fftCfgHdl);

	jvxFFT* fft;

	jvxDataCplx* in_fft;
	jvxData* out_fft;

	jvx_create_ifft_complex_2_real(&fft, fft_global, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE, &in_fft, &out_fft, &dummy, JVX_FFT_IFFT_EFFICIENT, NULL, NULL, fft_size);

	memcpy(in_fft, in, sizeof(jvxDataCplx) * (fft_size / 2 + 1));

	jvx_execute_ifft(fft);

	memcpy(out, out_fft, sizeof(jvxData) * fft_size);

	jvx_destroy_ifft(fft);
	jvx_destroy_fft_ifft_global(fft_global);

	return JVX_DSP_NO_ERROR;
}


JVX_FFT_TOOLS_DEFINE_FFT_SIZES

jvxDspBaseErrorType 
jvx_get_nearest_size_fft(jvxFFTSize* szTypeOnReturn, jvxSize n, jvxFftRoundType tp, jvxSize* szOnReturn)
{
	// first valid size is 16 -> subtract 4 to address enum correctly
	jvxSize idxFftSize = JVX_SIZE_UNSELECTED;
	int idxLow = (int)floor(log((jvxData)n) / log(2.)) - JVX_OFFSET_FFT_TYPE_MIN;
	int idxHigh = (int)ceil(log((jvxData)n) / log(2.)) - JVX_OFFSET_FFT_TYPE_MIN;
	idxLow = JVX_MAX(JVX_FFT_TOOLS_FFT_SIZE_16, idxLow);
	idxLow = JVX_MIN(JVX_FFT_TOOLS_FFT_SIZE_8192, idxLow);
	idxHigh = JVX_MAX(JVX_FFT_TOOLS_FFT_SIZE_16, idxHigh);
	idxHigh = JVX_MIN(JVX_FFT_TOOLS_FFT_SIZE_8192, idxHigh);
	jvxSize fftSLow = jvxFFTSize_sizes[idxLow];
	jvxSize fftSHigh = jvxFFTSize_sizes[idxHigh];
	switch (tp)
	{
	case JVX_FFT_ROUND_NEAREST:
		if (abs((int)fftSLow - (int)n) < abs((int)fftSHigh - (int)n))
		{
			*szTypeOnReturn = idxLow;
			if(szOnReturn)
			{
				*szOnReturn = fftSLow;
			}

		}
		else
		{
			*szTypeOnReturn = idxHigh;
			if(szOnReturn)
			{
				*szOnReturn = fftSHigh;
			}
		}
		return JVX_DSP_NO_ERROR;
	case JVX_FFT_ROUND_UP:
		if (n <= fftSHigh)
		{
			*szTypeOnReturn = idxHigh;
			if(szOnReturn)
			{
				*szOnReturn = fftSHigh;
			}
		}
		else
		{
			*szTypeOnReturn = JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE;
			if (szOnReturn)
			{
				*szOnReturn = n;
			}
		}
		return JVX_DSP_NO_ERROR;
	case JVX_FFT_ROUND_DOWN:
		if (n >= fftSLow)
		{
			*szTypeOnReturn = idxLow;
			if(szOnReturn)
			{
				*szOnReturn = fftSLow;
			}
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_INVALID_SETTING;
	default: 
		assert(0);
	}
	return JVX_DSP_NO_ERROR;
}

jvxSize jvx_get_fft_size(jvxFFTSize fftType)
{
	int fftSizeLog = (int)fftType + JVX_OFFSET_FFT_TYPE_MIN;
	jvxSize fftSize = (jvxSize)1 << (fftSizeLog);
	return fftSize;
}