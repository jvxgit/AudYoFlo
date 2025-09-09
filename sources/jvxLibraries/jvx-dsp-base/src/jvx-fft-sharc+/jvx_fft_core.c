#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_core_typedefs.h"
#include "jvx_allocators/jvx_allocators.h"

#include "jvx_dsp_base.h"

#ifdef JVX_DATA_FORMAT_DOUBLE
#error "Double-precision is not implemented yet because there is no direct equivalent to TI's DSPF_sp_fftSPxSP single-precision function."
#endif

#define AYF_USE_OPTIMIZED_IFFT_COMPLEX_2_REAL

struct jvx_fft_core_real_2_complex_sc594
{
	jvx_fft_core_real_2_complex common;
	jvxSize twiddle_stride;
};

// Declare the max size of fft
#define MAX_FFT_SIZE 8192
complex_float pm twiddle_table[MAX_FFT_SIZE/2];
#define MAX_FFT_SIZE_TYPE JVX_FFT_TOOLS_FFT_SIZE_8192


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
		hdl->fftParameters.coreFftType = corefftType;

		if(fftType == JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE)
		{
			assert(0);
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
	jvxCBool onlyFft = c_false;

	// Support only up to FFT size 8192
	if(fftType_max > MAX_FFT_SIZE_TYPE)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	// Multiply specified global handle
	if(!global_hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	jvx_fft_ifft_core_global_cfg* cfg = (jvx_fft_ifft_core_global_cfg*)fftGlobalPrv;

	// Check for existing allocators
	assert(jvx_allocator);
	ptr = jvx_allocator->alloc(sizeof(jvx_fft_ifft_core_global_common),  (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);

	ptr->fftSizeLog = (int)fftType_max + JVX_OFFSET_FFT_TYPE_MIN;
	ptr->refCount = 0;

	if(fftGlobalPrv == NULL)
	{
		twidfft (twiddle_table, MAX_FFT_SIZE);
	}

	ptr->twiddle_facs = twiddle_table;
	ptr->twiddle_fft_size = MAX_FFT_SIZE;

	// Derive maximum fft associated to global fft handle
	ptr->max_fft_size = (jvxInt32) 1 << ptr->fftSizeLog;

	ptr->cfg.mode = (1 << JVX_FFT_GLOBAL_FFT_IFFT_PRESERVE_INPUT_SHIFT) | (1 << JVX_FFT_GLOBAL_FFT_IFFT_COMPLEX_2_REAL_SHIFT);

	// Allocate workbuffer to allow for PRESERVE_INPUT option
	if(cfg)
	{
		ptr->cfg = *cfg;
	}

	ptr->work_buffer_common = NULL;
	ptr->work_buffer_ifft = NULL;

	if(ptr->cfg.mode & (1 << JVX_FFT_GLOBAL_FFT_ONLY_SHIFT))
	{
		if(ptr->cfg.mode & (1 << JVX_FFT_GLOBAL_FFT_IFFT_PRESERVE_INPUT_SHIFT))
		{
			// For FFT in preserve input mode, we need the work buffer
			ptr->work_buffer_common = jvx_allocator->alloc(sizeof(jvxData), (JVX_ALLOCATOR_ALLOCATE_FIELD | JVX_MEMORY_ALLOCATE_FAST_SLOW), (ptr->max_fft_size));
		}
	}
	else
	{
		if(ptr->cfg.mode & (1 << JVX_FFT_GLOBAL_FFT_IFFT_PRESERVE_INPUT_SHIFT))
		{
			// For IFFT in preserve input mode, we need the work buffer
			ptr->work_buffer_common = jvx_allocator->alloc(sizeof(complex_float), (JVX_ALLOCATOR_ALLOCATE_FIELD | JVX_MEMORY_ALLOCATE_FAST_SLOW), (ptr->max_fft_size));
		}

		if(ptr->cfg.mode & (1 <<  JVX_FFT_GLOBAL_FFT_IFFT_COMPLEX_2_REAL_SHIFT))
		{
			// Here, we need a dedicated buffer for complex output
			ptr->work_buffer_ifft = jvx_allocator->alloc(sizeof(complex_float), (JVX_ALLOCATOR_ALLOCATE_FIELD | JVX_MEMORY_ALLOCATE_FAST_SLOW), (ptr->max_fft_size));
		}
	}


	*global_hdl = ptr;

	// No global stuff required
	return JVX_DSP_NO_ERROR;
}

// =======================================================================================

jvxCBool jvx_fft_requires_normalization(jvxFFTGlobal* global_hdl)
{
	return c_false;
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

	assert(jvx_allocator);
	ptr = jvx_allocator->alloc(sizeof(jvx_fft_core_real_2_complex),  (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);

	if(!ptr)
		return JVX_DSP_ERROR_INTERNAL;

	// Specify purpose of fft transform
	res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX, fftType, operate, g_hdl, fftSizeArbitrary);
	ptr->common.ref_global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;
	ptr->common.ref_global_hdl->refCount++;
	ptr->common.twStride = global_hdl->twiddle_fft_size / ptr->common.fftParameters.fftSize;

	if(res != JVX_DSP_NO_ERROR)
		return JVX_DSP_ERROR_INTERNAL;

	assert(jvx_allocator);

	// Associate input buffers
	if(input)
	{
		ptr->input = input;
	}
	else
	{
		ptr->input = jvx_allocator->alloc(sizeof(jvxData),
				(JVX_ALLOCATOR_ALLOCATE_FIELD | JVX_MEMORY_ALLOCATE_FAST_SLOW), ptr->common.fftParameters.fftSize);
	}

	// Associate output buffers
	if(output)
	{
		ptr->output = output;
	}
	else
	{
		complex_float* cplx_ptr = NULL;
		cplx_ptr = jvx_allocator->alloc(sizeof(complex_float),
					(JVX_ALLOCATOR_ALLOCATE_FIELD | JVX_MEMORY_ALLOCATE_FAST_SLOW), ptr->common.fftParameters.fftSize/2+1);
		ptr->output = (jvxDataCplx*)cplx_ptr;
	}

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

	assert(jvx_allocator);
	ptr = jvx_allocator->alloc(sizeof(jvx_ifft_core_complex_2_real),  (JVX_ALLOCATOR_ALLOCATE_OBJECT | JVX_MEMORY_ALLOCATE_SLOW), 1);

	if(!ptr)
		return JVX_DSP_ERROR_INTERNAL;


	// Specify purpose of fft transform
	res = jvx_core_fft_init_common(&ptr->common, JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL, fftType, operate, g_hdl, fftSizeArbitrary);

	ptr->common.ref_global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;
	ptr->common.ref_global_hdl->refCount++;
	ptr->common.twStride = global_hdl->twiddle_fft_size / ptr->common.fftParameters.fftSize;

	if(res != JVX_DSP_NO_ERROR)
		return JVX_DSP_ERROR_INTERNAL;

	if(input)
	{
		ptr->input = input;
	}
	else
	{
		// Here, the important issue: the ifft requires full complex input - which is not very clever..
		complex_float* cplx_ptr = NULL;
		cplx_ptr = jvx_allocator->alloc(sizeof(complex_float),
					(JVX_ALLOCATOR_ALLOCATE_FIELD | JVX_MEMORY_ALLOCATE_FAST_SLOW), ptr->common.fftParameters.fftSize);
		ptr->input = (jvxDataCplx*)cplx_ptr;
	}

	if(output)
	{
		ptr->output = output;
	}
	else
	{
		ptr->output = jvx_allocator->alloc(sizeof(jvxData),
								(JVX_ALLOCATOR_ALLOCATE_FIELD | JVX_MEMORY_ALLOCATE_FAST_SLOW), ptr->common.fftParameters.fftSize);
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

	if(res == JVX_DSP_NO_ERROR)
	{
		*hdl = (jvx_fft_ifft_core_common*) ptr;
	}

	return res;
}

// =======================================================================
// Currently unused functions!!
// =======================================================================

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
	jvx_fft_ifft_core_global_common* gHdl = (jvx_fft_ifft_core_global_common* )hdl->glob_ptr;

	jvxHandle* inputWork = NULL;
	jvxHandle* input = NULL;
	jvxDataCplx* output = NULL;
	int twStride = 1;

	if(hdl->operate==JVX_FFT_IFFT_PRESERVE_INPUT)
	{
		// Make sure the fft is properly setup
		assert(gHdl->cfg.mode & (1 << JVX_FFT_GLOBAL_FFT_IFFT_PRESERVE_INPUT_SHIFT));
		// assert(hdl->shadow_buffer!=0);

		if (hdl->fftParameters.coreFftType==JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)
		{
			input =  hdl_fft_r2c->input;
			inputWork = (jvxHandle*)gHdl->work_buffer_common;
			output = hdl_fft_r2c->output;
			twStride = hdl->twStride;
		}
		else
		{
			assert(0);
		}
	}
	else
	{
		if (hdl->fftParameters.coreFftType==JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)
		{
			input =  hdl_fft_r2c->input;
			inputWork =  hdl_fft_r2c->input;
			output = hdl_fft_r2c->output;
			twStride = hdl->twStride;
		}
		else
		{
			assert(0);
		}
	}

	if(hdl->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX)
	{
		assert(0);
	}
	else if(hdl->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)
	{
		rfft(input, inputWork, (complex_float*)output, gHdl->twiddle_facs,
		 		twStride, hdl->fftParameters.fftSize);
	}

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_execute_ifft(jvxIFFT* hdlRef)
{
	jvx_fft_ifft_core_common* hdl = (jvx_fft_ifft_core_common*)hdlRef;
	jvx_ifft_core_complex_2_complex* hdl_ifft_c2c = (jvx_ifft_core_complex_2_complex*)hdl;
	jvx_ifft_core_complex_2_real* hdl_ifft_c2r = (jvx_ifft_core_complex_2_real*)hdl;
	jvx_fft_ifft_core_global_common* gHdl = (jvx_fft_ifft_core_global_common* )hdl->glob_ptr;

	complex_float* input = NULL;
	complex_float* inputWork = NULL;
	complex_float* output = NULL;
	float* outreal = NULL;

	int twStride = 1;

	if(hdl->operate==JVX_FFT_IFFT_PRESERVE_INPUT)
	{
		// Due to missing complex2real ifft no different cases
		input = (complex_float*)hdl_ifft_c2r->input;
		inputWork = gHdl->work_buffer_common;
		twStride = hdl->twStride;
		if(hdl->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL)
		{
			output = gHdl->work_buffer_ifft;
			outreal = hdl_ifft_c2r->output;
		}
		else
		{
			assert(0);
		}

		// We need to mirror the complex input spectrum
		for (int k = 1; k < hdl->fftParameters.fftSize/2; k++)
		{
			input[hdl->fftParameters.fftSize-k] = conj(input[k]);
		}
	}


	if(hdl->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX)
	{
		assert(0);
	}
	else if(hdl->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL)
	{
#ifdef AYF_USE_OPTIMIZED_IFFT_COMPLEX_2_REAL

		// Run the efficient ifft complex2real.
		// This implementation runs the first stage of the ifft taking into account a real valued output
		// and then an ifft of length hdl->fftParameters.fftSize/2.
		// Check the reference code in <sources/jvxLibraries/ayf_awe_embedded/ifft_eff.m>
		int i = 0;
		int iTw = twStride;
		int N2 = hdl->fftParameters.fftSize/2;

		complex_float* inPtrFwd = input;
		complex_float* outPtrFwd = inputWork;
		complex_float* inPtrBwd = input + N2;
		complex_float tmp, XO, TW, XOX;

		// XE = Xk(cindex2matindex(idxWrite)) + Xk(cindex2matindex(idxEnd));
	    // XO = Xk(cindex2matindex(idxWrite)) - Xk(cindex2matindex(idxEnd));
	    // Xk_fft_all(cindex2matindex(idxWrite)) = 0.5 * (XE + j * XO);
		tmp.re = 0.5 * (inPtrFwd->re + inPtrBwd->re);
		tmp.im = 0.5 * (inPtrFwd->re - inPtrBwd->re);

		*outPtrFwd = tmp;

		inPtrFwd++;
		outPtrFwd++;
		inPtrBwd--;

		for(i = 1; i < N2; i++)
		{
			// XER = real(Xk(cindex2matindex(idxWrite))) + real(Xk(cindex2matindex(idxEnd)));
			// XEI = imag(Xk(cindex2matindex(idxWrite))) - imag(Xk(cindex2matindex(idxEnd)));
			tmp.re = inPtrFwd->re + inPtrBwd->re;
			tmp.im = inPtrFwd->im - inPtrBwd->im;

			// XOR = real(Xk(cindex2matindex(idxWrite))) - real(Xk(cindex2matindex(idxEnd)));
			// XOI = imag(Xk(cindex2matindex(idxWrite))) + imag(Xk(cindex2matindex(idxEnd)));
			XO.re = inPtrFwd->re - inPtrBwd->re;
			XO.im = inPtrFwd->im + inPtrBwd->im;

			TW = gHdl->twiddle_facs[iTw];
			iTw += twStride;

			// ## XOXR = (XOR*TWR -XOI*TWI);
			// ## XOXI = (XOR * TWI  + XOI * TWR);
			// Note that the tw facs are conj as TW is for fft not ifft
			// XOXR = (XOR*TWR + XOI*TWI_);
	        // XOXI = (XOI * TWR - XOR * TWI_ );
			XOX.re = XO.re * TW.re + XO.im * TW.im;
			XOX.im = XO.im * TW.re - XO.re * TW.im;

			// XAR = XER - XOXI;
			// XAI = XEI + XOXR;
			tmp.re -= XOX.im;
			tmp.im += XOX.re;

			// Add factor 0.5
			tmp.re *= 0.5;
			tmp.im *= 0.5;

			// We need to store in a dedicated buffer to not override inPtrBwd
			*outPtrFwd= tmp;
			inPtrFwd++;
			outPtrFwd++;
			inPtrBwd--;
		}

		ifft(inputWork, inputWork, (complex_float*)outreal, gHdl->twiddle_facs,
				 		2*twStride, N2);
#else

		// ========================================================================
		ifft(input, inputWork, output, gHdl->twiddle_facs,
		 		twStride, hdl->fftParameters.fftSize);

		// ifft2048(input, output);

		for(int k = 0; k < hdl->fftParameters.fftSize; k++)
		{
			// Extract only the real part, imag part should be all zero!
			outreal[k] = output[k].re;
		}
#endif
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
		assert(0);
		/*
		if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_r2c->input);
		}
		if(hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_r2c->output);
		}
		*/
		hdl_fft_r2c->input = NULL;
		hdl_fft_r2c->output = NULL;


		break;
	case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX:
		/*
		if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_c2c->input);
		}
		if(hdl->outputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_c2c->output);
		}
		*/
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
		/*
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
		*/
		hdl_ifft_c2r->input = NULL;
		hdl_ifft_c2r->output = NULL;


		break;
	case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX:
		/*
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
		*/
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
