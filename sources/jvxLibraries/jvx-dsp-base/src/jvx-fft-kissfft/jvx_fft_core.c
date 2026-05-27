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
#include "jvx_allocators/jvx_allocators.h"

// =============================================================================
// =============================================================================

#define JVX_FFTK_BUFFERS_ALLOCATE(varName_buf, varName_type, type, type_comp, numElements) \
	assert(sizeof(type_comp) == sizeof(type));			\
	if(varName_buf == NULL)						\
		{							\
			ptr->varName_buf = NULL; \
			JVX_SAFE_ALLOCATE_FIELD_Z(ptr->varName_buf, type, numElements); \
			ptr->common.varName_type = JVX_FFT_IFFT_BUFFER_ALLOCATED; \
		}							\
	else								\
		{							\
			ptr->varName_buf = varName_buf;			\
			ptr->common.varName_type = JVX_FFT_IFFT_BUFFER_EXTERNAL; \
		}							\
	memset(ptr->varName_buf, 0, sizeof(type) * (numElements));


#define JVX_FFTK_BUFFERS_DEALLOCATE(buf) \
	JVX_SAFE_DELETE_FIELD(buf)

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
void jvx_fft_ifft_global_set_flags(jvxFFTGlobal* g_hdl, jvxCBitField flagsSet)
{
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;
	global_hdl->globFlags = flagsSet;
}

jvxCBitField jvx_fft_ifft_global_get_flags(jvxFFTGlobal* g_hdl)
{
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;
	return global_hdl->globFlags;
}

jvxDspBaseErrorType jvx_create_fft_ifft_global(
	jvxFFTGlobal** glob_hdl, jvxFFTSize fftType_max, 
	jvxFFTGlobal* fftGlobCfg JVX_FFT_GLOBAL_CONFIG_ADD_ARGUMENT)
{
	jvx_fft_ifft_core_global_common** global_hdl = (jvx_fft_ifft_core_global_common**)glob_hdl;
	jvx_fft_ifft_core_global_common* ptr = NULL;

	if(!global_hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	
	// Check this here
	assert(sizeof(kiss_fft_cpx) == sizeof(jvxDataCplx));

	assert(jvx_allocator != NULL);
	if (fftGlobCfg)
	{
		ptr = fftGlobCfg;
	}
	else
	{
		ptr = jvx_allocator->alloc(sizeof(jvx_fft_ifft_core_global_common), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);
		ptr->fftType = fftType_max;
		ptr->fftSizeLog = (int)fftType_max + JVX_OFFSET_FFT_TYPE_MIN;
		ptr->refCount = 0;
		ptr->attached = NULL;
		ptr->globFlags = 0;
	}

	*global_hdl = ptr;

	// No global stuff required
	return JVX_DSP_NO_ERROR;

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

		assert(jvx_allocator != NULL);

		ptr = jvx_allocator->alloc(sizeof(jvx_fft_core_real_2_complex), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);		

		if(ptr)
		{
			ptr->common.kFFTr = NULL;
			ptr->common.kFFT = NULL;

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
				
				// FOR FFTW
				JVX_FFTK_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, kiss_fft_cpx, (ptr->common.fftParameters.fftSize/2+1));
				JVX_FFTK_BUFFERS_ALLOCATE(input, inputOrigin, jvxData, kiss_fft_scalar, ptr->common.fftParameters.fftSize);
				
				// ==============================================================
				// Create fft handles
				// ==============================================================
				// It is all for <JVX_FFT_IFFT_PRESERVE_INPUT>
				ptr->common.kFFTr = kiss_fftr_alloc(ptr->common.fftParameters.fftSize, 0, NULL, 0);
				ptr->common.purp = JVX_FFT_LOCAL_PURPOSE_FFT_R2C;

				if (output == NULL)
				{
					jvx_allocator->inc(sizeof(jvxDataCplx) * (ptr->common.fftParameters.fftSize / 2 + 1));
				}
				if (input == NULL)
				{
					jvx_allocator->inc(sizeof(jvxData) * ptr->common.fftParameters.fftSize);
				}

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

jvxDspBaseErrorType jvx_create_fft_complex_2_complex(
	jvxFFT** hdlRef,
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

		assert(jvx_allocator != NULL);

		ptr = jvx_allocator->alloc(sizeof(jvx_fft_core_complex_2_complex), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);

		if(ptr)
		{
			ptr->common.kFFTr = NULL;
			ptr->common.kFFT = NULL;

			// Specify purpose of fft transform
			res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX, fftType, operate, global_hdl, fftSizeArbitrary);
			ptr->common.ref_global_hdl = global_hdl;
			ptr->common.ref_global_hdl->refCount++;

			if(res == JVX_DSP_NO_ERROR)
			{
				// Allocate input

				JVX_FFTK_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, kiss_fft_cpx, ptr->common.fftParameters.fftSize);
				JVX_FFTK_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, kiss_fft_cpx, ptr->common.fftParameters.fftSize);

				memset(ptr->input, 0, sizeof(jvxDataCplx)* (ptr->common.fftParameters.fftSize));
				memset(ptr->output, 0, sizeof(jvxDataCplx)* (ptr->common.fftParameters.fftSize));

				// It is all <JVX_FFT_IFFT_PRESERVE_INPUT>
				ptr->common.kFFT = kiss_fft_alloc(ptr->common.fftParameters.fftSize, 0, NULL, 0);
				ptr->common.purp = JVX_FFT_LOCAL_PURPOSE_FFT_C2C;

				if (input == NULL)
				{
					jvx_allocator->inc(sizeof(jvxDataCplx) * ptr->common.fftParameters.fftSize);
				}
				if (output == NULL)
				{
					jvx_allocator->inc(sizeof(jvxDataCplx) * ptr->common.fftParameters.fftSize);
				}

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

		assert(jvx_allocator != NULL);
		ptr = jvx_allocator->alloc(sizeof(jvx_ifft_core_complex_2_real), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);
		
		if(ptr)
		{
			// Specify purpose of fft transform
			res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL, fftType, operate, global_hdl, fftSizeArbitrary);
			ptr->common.ref_global_hdl = global_hdl;
			ptr->common.ref_global_hdl->refCount++;			

			if(res == JVX_DSP_NO_ERROR)
			{
				// Allocate input
				JVX_FFTK_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, kiss_fft_cpx, ptr->common.fftParameters.fftSize/2+1);
				JVX_FFTK_BUFFERS_ALLOCATE(output, outputOrigin, jvxData, kiss_fft_scalar, ptr->common.fftParameters.fftSize);

				// It is always <JVX_FFT_IFFT_PRESERVE_INPUT>
				ptr->common.kFFTr = kiss_fftr_alloc(ptr->common.fftParameters.fftSize, 1, NULL, 0);
				ptr->common.purp = JVX_FFT_LOCAL_PURPOSE_IFFT_C2R;

				if (input == NULL)
				{
					jvx_allocator->inc(sizeof(jvxDataCplx) * (ptr->common.fftParameters.fftSize / 2 + 1));
				}
				if (output == NULL)
				{
					jvx_allocator->inc(sizeof(jvxData) * ptr->common.fftParameters.fftSize);
				}

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

		assert(jvx_allocator != NULL);

		ptr = jvx_allocator->alloc(sizeof(jvx_ifft_core_complex_2_complex), (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);

		if(ptr)
		{
			// Specify purpose of fft transform
			res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX, fftType, operate, global_hdl, fftSizeArbitrary);

			ptr->common.ref_global_hdl = global_hdl;
			ptr->common.ref_global_hdl->refCount++;

			if(res == JVX_DSP_NO_ERROR)
			{
				// Allocate input

				JVX_FFTK_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, kiss_fft_cpx, ptr->common.fftParameters.fftSize);
				JVX_FFTK_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, kiss_fft_cpx, ptr->common.fftParameters.fftSize);

				// It is always JVX_FFT_IFFT_PRESERVE_INPUT
				ptr->common.kFFT = kiss_fft_alloc(ptr->common.fftParameters.fftSize, 1, NULL, 0);
				ptr->common.purp = JVX_FFT_LOCAL_PURPOSE_IFFT_C2C;
	
				if (input == NULL)
				{
					jvx_allocator->inc(sizeof(jvxDataCplx) * ptr->common.fftParameters.fftSize);
				}
				if (output == NULL)
				{
					jvx_allocator->inc(sizeof(jvxDataCplx) * ptr->common.fftParameters.fftSize);
				}

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

// =================================================================================================
// =================================================================================================
// =================================================================================================

jvxDspBaseErrorType jvx_execute_fft(jvxFFT* hdlRef)
{
	jvx_fft_ifft_core_common* hdl = (jvx_fft_ifft_core_common*)hdlRef;
	jvx_fft_core_real_2_complex* hdl_fft_r2c = (jvx_fft_core_real_2_complex*)hdl;
	jvx_fft_core_complex_2_complex* hdl_fft_c2c = (jvx_fft_core_complex_2_complex*)hdl;
	jvx_ifft_core_complex_2_complex* hdl_ifft_c2c = (jvx_ifft_core_complex_2_complex*)hdl;
	jvx_ifft_core_complex_2_real* hdl_ifft_c2r = (jvx_ifft_core_complex_2_real*)hdl;

	switch (hdl->purp)
	{
	case JVX_FFT_LOCAL_PURPOSE_FFT_R2C:
		assert(hdl->kFFTr);
		kiss_fftr(hdl->kFFTr, hdl_fft_r2c->input, (kiss_fft_cpx*)hdl_fft_r2c->output);
		break;
	case JVX_FFT_LOCAL_PURPOSE_FFT_C2C:
		assert(hdl->kFFT);
		kiss_fft(hdl->kFFT, (const kiss_fft_cpx*)hdl_fft_c2c->input, (kiss_fft_cpx*)hdl_fft_r2c->output);
		break;
	case JVX_FFT_LOCAL_PURPOSE_IFFT_C2R:
		assert(hdl->kFFTr);
		kiss_fftri(hdl->kFFTr, (const kiss_fft_cpx*)hdl_ifft_c2r->input, hdl_ifft_c2r->output);
		break;
	case JVX_FFT_LOCAL_PURPOSE_IFFT_C2C:
		assert(hdl->kFFT);
		kiss_fft(hdl->kFFT, (const kiss_fft_cpx*)hdl_ifft_c2c->input, (kiss_fft_cpx*)hdl_ifft_c2c->output);
		break;
	}
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

	switch (hdl->fftParameters.coreFftType)
	{
	case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX:
		if (hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_FFTK_BUFFERS_DEALLOCATE(hdl_fft_r2c->input);

		}
		if (hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{

			JVX_FFTK_BUFFERS_DEALLOCATE(hdl_fft_r2c->output);
		}

		hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl_fft_r2c->input = NULL;
		hdl_fft_r2c->output = NULL;

		break;
	case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX:
		if (hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_FFTK_BUFFERS_DEALLOCATE(hdl_fft_c2c->input);
		}
		if (hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_FFTK_BUFFERS_DEALLOCATE(hdl_fft_c2c->output);
		}
		hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl_fft_c2c->input = NULL;
		hdl_fft_c2c->output = NULL;

		break;
	case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL:
		if (hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_FFTK_BUFFERS_DEALLOCATE(hdl_ifft_c2r->input);
		}
		if (hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_FFTK_BUFFERS_DEALLOCATE(hdl_ifft_c2r->output);
		}
		hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl_ifft_c2r->input = NULL;
		hdl_ifft_c2r->output = NULL;
		break;
	case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX:
		if (hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_FFTK_BUFFERS_DEALLOCATE(hdl_ifft_c2c->input);
		}
		if (hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_FFTK_BUFFERS_DEALLOCATE(hdl_ifft_c2c->output);
		}
		hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl_ifft_c2c->input = NULL;
		hdl_ifft_c2c->output = NULL;

		break;
	default:
		assert(0);
	}


	if(hdl->kFFT) kiss_fft_free(hdl->kFFT);
	hdl->kFFT = NULL;

	if (hdl->kFFTr) kiss_fftr_free(hdl->kFFTr);
	hdl->kFFTr = NULL;

	hdl->ref_global_hdl->refCount--;

	assert(jvx_allocator != NULL);

	jvx_allocator->dealloc((jvxHandle**)&hdl, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
	return res;
}

jvxDspBaseErrorType jvx_destroy_ifft(jvxIFFT* hdlRef)
{
	return jvx_destroy_fft((jvxFFT*)hdlRef);
}

jvxDspBaseErrorType jvx_destroy_fft_ifft_global(jvxFFTGlobal* g_hdl, jvxFFTGlobal* fftGlobCfg)
{
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;

	if (global_hdl)
	{
		assert(jvx_allocator != NULL);

		if (fftGlobCfg)
		{
			global_hdl = NULL;
		}
		else
		{
			// All ffts released?
			assert(global_hdl->refCount == 0);
			assert(global_hdl->attached == NULL);
			jvx_allocator->dealloc((jvxHandle**)&global_hdl, (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW));
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxCBool jvx_fft_requires_normalization(jvxFFTGlobal* global_hdl)
{
	return c_true;
	// return c_false;
}

// Return FFT implementation type - some ffts require special constraints when calling
jvxFftImplementationType jvx_fft_ifft_implementation()
{
	return JVX_FFT_IMPLEMENTATION_FFTW;
}

jvxData * jvx_allocate_fft_buffer_real(jvxSize num_elements)
{
	jvxData* buffer;
	
	JVX_SAFE_ALLOCATE_FIELD_Z(buffer, jvxData, num_elements);
	return buffer;
}

jvxDataCplx * jvx_allocate_fft_buffer_complex(jvxSize num_elements)
{
	jvxDataCplx* buffer;
	JVX_SAFE_ALLOCATE_FIELD_Z(buffer, jvxDataCplx, num_elements);
	return buffer;
}

void jvx_free_fft_buffer(void * buffer)
{
	JVX_FFTK_BUFFERS_DEALLOCATE(buffer);
}

jvxDspBaseErrorType jvx_oneshot_ifft_complex_2_real(jvxSize fft_size, jvxDataCplx * in, jvxData * out, jvxFFTGlobal* fftGlobCfg JVX_FFT_GLOBAL_CONFIG_ADD_ARGUMENT)
{		
	jvxSize dummy = 0;

	jvxFFTGlobal* fft_global;
	jvx_create_fft_ifft_global(&fft_global, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE, fftGlobCfg JVX_FFT_GLOBAL_CONFIG_ADD_ARGUMENT_CALL);

	jvxFFT* fft;

	jvxDataCplx* in_fft;
	jvxData* out_fft;

	jvx_create_ifft_complex_2_real(&fft, fft_global, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE, &in_fft, &out_fft, &dummy, JVX_FFT_IFFT_EFFICIENT, NULL, NULL, fft_size);

	memcpy(in_fft, in, sizeof(jvxDataCplx) * (fft_size / 2 + 1));

	jvx_execute_ifft(fft);

	memcpy(out, out_fft, sizeof(jvxData) * fft_size);

	jvx_destroy_ifft(fft);
	jvx_destroy_fft_ifft_global(fft_global, fftGlobCfg);

	return JVX_DSP_NO_ERROR;
}

jvxFFTSize jvx_fft_ifft_global_max_fft_size(jvxFFTGlobal* global_hdl_arg)
{
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)global_hdl_arg;

	if (global_hdl)
	{
		return global_hdl->fftType;
	}
	return JVX_FFT_TOOLS_FFT_SIZE_INVALID;
}



