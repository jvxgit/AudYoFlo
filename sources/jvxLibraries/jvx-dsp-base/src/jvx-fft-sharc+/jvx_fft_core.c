#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_core_typedefs.h"

#include "jvx_dsp_base.h"

#ifdef JVX_DATA_FORMAT_DOUBLE
#error "Double-precision is not implemented yet because there is no direct equivalent to TI's DSPF_sp_fftSPxSP single-precision function."
#endif

// Alignment note: malloc() aligns to 8 bytes (see https://e2e.ti.com/support/dsp/c6000_multi-core_dsps/f/639/t/203638)
// NOTE TI C67x DSPLIB FFTs are modify the input buffers, even though they are not inplace!
// TODO benchmark asm vs C versions of FFT


// ####################################################################

static jvxDspBaseErrorType jvx_core_fft_init_common(jvx_fft_ifft_core_common* hdl,
							jvxFftTools_coreFftType corefftType,
							jvxFFTSize fftType,
							jvxFftIfftOperate operate,
							jvxFFTGlobal* glob_ptr,
							jvxSize fftSizeArbitrary)
{
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)glob_ptr;

	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;

	if(hdl && glob_ptr)
	{
		assert(0);
	}
	else
	{
		res = JVX_DSP_ERROR_INVALID_ARGUMENT;
	}
	return res;
}


// =======================================================================================

jvxDspBaseErrorType jvx_create_fft_ifft_global(jvxFFTGlobal** glob_hdl, jvxFFTSize fftType_max, jvxHandle* fftGlobalPrv)
{
	jvx_fft_ifft_core_global_common** global_hdl = (jvx_fft_ifft_core_global_common**)glob_hdl;
	jvx_fft_ifft_core_global_common* ptr = NULL;

	if(fftType_max > JVX_FFT_TOOLS_FFT_SIZE_8192)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	if(!global_hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(ptr, jvx_fft_ifft_core_global_common);

	ptr->fftSizeLog = (int)fftType_max + JVX_OFFSET_FFT_TYPE_MIN;
	ptr->refCount = 0;

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
						  jvxSize* N,
						  jvxFftIfftOperate operate,
						  jvxData* input,
						  jvxDataCplx* output,
						  jvxSize fftSizeArbitrary)
{
	jvx_fft_ifft_core_common** hdl = (jvx_fft_ifft_core_common**)hdlRef;
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;

	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvx_fft_core_real_2_complex* ptr = NULL;

	if(!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(ptr, jvx_fft_core_real_2_complex);
	if(!ptr)
		return JVX_DSP_ERROR_INTERNAL;

	// Specify purpose of fft transform
	res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX, fftType, operate, global_hdl, fftSizeArbitrary);
	ptr->common.ref_global_hdl = global_hdl;
	ptr->common.ref_global_hdl->refCount++;

	if(res != JVX_DSP_NO_ERROR)
		return JVX_DSP_ERROR_INTERNAL;

	assert(0);

	if(in_ptr_fld_N)
	{
		*in_ptr_fld_N = ptr->input;
	}
	if(out_ptr_fld_N2P1)
	{
		*out_ptr_fld_N2P1 = ptr->output;
	}
	if(N)
	{
		*N = ptr->common.fftParameters.fftSize;
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
							 jvxSize* N,
							 jvxFftIfftOperate operate,
							 jvxDataCplx* input,
							 jvxDataCplx* output,
							 jvxSize fftSizeArbitrary)
{
	jvx_fft_ifft_core_common** hdl = (jvx_fft_ifft_core_common**)hdlRef;
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;

	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvx_fft_core_complex_2_complex* ptr = NULL;

	if(!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(ptr, jvx_fft_core_complex_2_complex);
	if(!ptr)
		return JVX_DSP_ERROR_INTERNAL;

	// Specify purpose of fft transform
	res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX, fftType, operate, global_hdl, fftSizeArbitrary);
	ptr->common.ref_global_hdl = global_hdl;
	ptr->common.ref_global_hdl->refCount++;

	if(res != JVX_DSP_NO_ERROR)
		return JVX_DSP_ERROR_INTERNAL;

	// Allocate input
	assert(0);

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

	if(!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(ptr, jvx_ifft_core_complex_2_real);
	if(!ptr)
		return JVX_DSP_ERROR_INTERNAL;


	// Specify purpose of fft transform
	res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL, fftType, operate, global_hdl, fftSizeArbitrary);

	ptr->common.ref_global_hdl = global_hdl;
	ptr->common.ref_global_hdl->refCount++;

	if(res != JVX_DSP_NO_ERROR)
		return JVX_DSP_ERROR_INTERNAL;

	assert(0);

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

	if(!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(ptr, jvx_ifft_core_complex_2_complex);
	if(!ptr)
		return JVX_DSP_ERROR_INTERNAL;


	// Specify purpose of fft transform
	res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX, fftType, operate, global_hdl, fftSizeArbitrary);

	ptr->common.ref_global_hdl = global_hdl;
	ptr->common.ref_global_hdl->refCount++;

	if(res != JVX_DSP_NO_ERROR)
		return JVX_DSP_ERROR_INTERNAL;

	// Allocate input
	assert(0);

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

	if(res == JVX_DSP_NO_ERROR)
	{
		*hdl = (jvx_fft_ifft_core_common*) ptr;
	}

	return res;
}

jvxDspBaseErrorType jvx_execute_fft(jvxFFT* hdlRef)
{
	jvx_fft_ifft_core_common* hdl = (jvx_fft_ifft_core_common*)hdlRef;
	jvx_fft_core_complex_2_complex* hdl_fft_c2c = (jvx_fft_core_complex_2_complex*)hdl;
	jvx_fft_core_real_2_complex* hdl_fft_r2c = (jvx_fft_core_real_2_complex*)hdl;
	void* input;
	int inputSize;

	if(hdl->operate==JVX_FFT_IFFT_PRESERVE_INPUT)
	{
		assert(hdl->shadow_buffer!=0);
		if (hdl->fftParameters.coreFftType==JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)
			inputSize = hdl_fft_c2c->common.fftParameters.fftSize*sizeof(float);
		else
			inputSize = hdl_fft_c2c->common.fftParameters.fftSize*2*sizeof(float);
		memcpy(hdl->shadow_buffer, hdl_fft_c2c->input, inputSize);
		input = hdl->shadow_buffer;
	}
	else
	{
		input = hdl_fft_c2c->input;
	}

	if(hdl->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX)
	{
		assert(0);
	}
	else if(hdl->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)
	{
		assert(0);
	}

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_execute_ifft(jvxIFFT* hdlRef)
{
	jvx_fft_ifft_core_common* hdl = (jvx_fft_ifft_core_common*)hdlRef;
	jvx_ifft_core_complex_2_complex* hdl_ifft_c2c = (jvx_ifft_core_complex_2_complex*)hdl;
	jvx_ifft_core_complex_2_real* hdl_ifft_c2r = (jvx_ifft_core_complex_2_real*)hdl;

	if(hdl->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX)
	{
		assert(0);
	}
	else if(hdl->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL)
	{
		assert(0);
	}

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_destroy_fft(jvxFFT* hdlRef)
{
	jvx_fft_ifft_core_common* hdl = (jvx_fft_ifft_core_common*)hdlRef;
	jvx_fft_core_complex_2_complex* hdl_fft_c2c = (jvx_fft_core_complex_2_complex*)hdlRef;
	jvx_fft_core_real_2_complex* hdl_fft_r2c = (jvx_fft_core_real_2_complex*)hdlRef;

	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;

	if(!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	switch(hdl->fftParameters.coreFftType)
	{
	case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX:
		if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_r2c->input);
		}
		if(hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_r2c->output);
		}

		hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl_fft_r2c->input = NULL;
		hdl_fft_r2c->output = NULL;

		JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_r2c->temp);
		hdl_fft_r2c->temp = NULL;

		break;
	case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX:
		if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_c2c->input);
		}
		if(hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_c2c->output);
		}
		hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl_fft_c2c->input = NULL;
		hdl_fft_c2c->output = NULL;

		break;
	default:
		assert(0);
	}

	hdl->ref_global_hdl->refCount--;

	return res;
}

jvxDspBaseErrorType jvx_destroy_ifft(jvxIFFT* hdlRef)
{
	jvx_fft_ifft_core_common* hdl = (jvx_fft_ifft_core_common*)hdlRef;
	jvx_ifft_core_complex_2_complex* hdl_ifft_c2c = (jvx_ifft_core_complex_2_complex*)hdlRef;
	jvx_ifft_core_complex_2_real* hdl_ifft_c2r = (jvx_ifft_core_complex_2_real*)hdlRef;

	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;

	if(!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	switch(hdl->fftParameters.coreFftType)
	{
	case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL:
		if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2r->input);
		}
		if(hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2r->output);
		}
		hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl_ifft_c2r->input = NULL;
		hdl_ifft_c2r->output = NULL;

		JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2r->temp);
		hdl_ifft_c2r->temp = NULL;

		break;
	case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX:
		if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2c->input);
		}
		if(hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2c->output);
		}
		hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl_ifft_c2c->input = NULL;
		hdl_ifft_c2c->output = NULL;

		break;
	default:
		assert(0);
	}

	hdl->ref_global_hdl->refCount--;

	return res;
}

jvxDspBaseErrorType jvx_destroy_fft_ifft_global(jvxFFTGlobal* g_hdl)
{
	int i;
	jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;

	if(!global_hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	// all ffts released?
	assert(global_hdl->refCount == 0);

	for (i = 0; i < NUM_TWIDDLE_ARRAYS; ++i)
	{
		JVX_DSP_SAFE_DELETE_FIELD(global_hdl->twiddleFactors[i]);
		JVX_DSP_SAFE_DELETE_FIELD(global_hdl->A[i]);
		JVX_DSP_SAFE_DELETE_FIELD(global_hdl->B[i]);
	}

	JVX_DSP_SAFE_DELETE_OBJECT(global_hdl);

	return JVX_DSP_NO_ERROR;
}

JVX_FFT_TOOLS_DEFINE_FFT_SIZES

jvxDspBaseErrorType 
jvx_get_nearest_size_fft(jvxFFTSize* szTypeOnReturn, jvxSize n, jvxFftRoundType tp, jvxSize* szOnReturn)
{
	// first valid size is 16 -> subtract 4 to address enum correctly
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
			*szTypeOnReturn = (jvxFFTSize)idxLow;
			if(szOnReturn)
			{
				*szOnReturn = fftSLow;
			}

		}
		else
		{
			*szTypeOnReturn = (jvxFFTSize)idxHigh;
			if(szOnReturn)
			{
				*szOnReturn = fftSHigh;
			}
		}
		return JVX_DSP_NO_ERROR;
	case JVX_FFT_ROUND_UP:
		if (n <= fftSHigh)
		{
			*szTypeOnReturn = (jvxFFTSize)idxHigh;
			if(szOnReturn)
			{
				*szOnReturn = fftSHigh;
			}
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_INVALID_SETTING;
	case JVX_FFT_ROUND_DOWN:
		if (n >= fftSLow)
		{
			*szTypeOnReturn = (jvxFFTSize)idxLow;
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
