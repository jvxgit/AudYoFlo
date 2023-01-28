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

// TI C67x DSPLIB documentation: http://www.ti.com/lit/ug/spru657c/spru657c.pdf
// r-2-c/c-2-r optimization: http://processors.wiki.ti.com/index.php/Efficient_FFT_Computation_of_Real_Input

#pragma DATA_ALIGN(brev, 8);
static unsigned char brev[64] = {
	0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
	0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
	0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
	0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
	0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
	0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
	0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
	0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f
};

// generate vector of twiddle factors for optimized algorithm
static void tw_gen(float *w, int n)
{
	int i, j, k;
	double x_t, y_t, theta1, theta2, theta3;

	_nassert((int)w % 8 == 0);

	for (j = 1, k = 0; j <= n >> 2; j = j << 2)
	{
		for (i = 0; i < n >> 2; i += j)
		{
			theta1 = 2 * M_PI * i / n;
			x_t = cos (theta1);
			y_t = sin (theta1);
			w[k] = (float) x_t;
			w[k + 1] = (float) y_t;

			theta2 = 4 * M_PI * i / n;
			x_t = cos (theta2);
			y_t = sin (theta2);
			w[k + 2] = (float) x_t;
			w[k + 3] = (float) y_t;

			theta3 = 6 * M_PI * i / n;
			x_t = cos (theta3);
			y_t = sin (theta3);
			w[k + 4] = (float) x_t;
			w[k + 5] = (float) y_t;
			k += 6;
		}
	}
}

static void calculate_A_B(jvxDataCplx **A, jvxDataCplx **B, jvxInt32 N)
{
	int i;

	assert((!(*A) && !(*B)) || ((*A) && (*B))); // A and B belong together
	assert(N % 2 == 0);

	if(*A) // check if already created
		return;

	jvxDataCplx *newA, *newB;
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newA, jvxDataCplx, N/2);
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newB, jvxDataCplx, N/2);

	_nassert ((int)newA % 8 == 0);
	_nassert ((int)newB % 8 == 0);

	for (i = 0; i < N/2; i++)
	{
	  newA[i].re = 0.5 * ( 1.0 - sin(2 * M_PI / (double)N * (double)i));
	  newA[i].im = 0.5 * (-1.0 * cos(2 * M_PI / (double)N * (double)i));
	  newB[i].re = 0.5 * ( 1.0 + sin(2 * M_PI / (double)N * (double)i));
	  newB[i].im = 0.5 * ( 1.0 * cos(2 * M_PI / (double)N * (double)i));
	}

	*A = newA;
	*B = newB;
}

// =============================================================================
// =============================================================================
#define JVX_FFT_BUFFERS_ALLOCATE(varName_buf, varName_type, type, numElements) \
	if(varName_buf == NULL)						\
	{							\
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->varName_buf, type, numElements); \
		ptr->common.varName_type = JVX_FFT_IFFT_BUFFER_ALLOCATED; \
	}							\
	else								\
	{							\
		ptr->varName_buf = varName_buf;			\
		ptr->common.varName_type = JVX_FFT_IFFT_BUFFER_EXTERNAL; \
		memset(ptr->varName_buf, 0, sizeof(type) * (numElements)); \
	}


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
		hdl->fftParameters.fftType = fftType;
		hdl->fftParameters.fftSizeLog = (int)fftType + JVX_OFFSET_FFT_TYPE_MIN;
		hdl->fftParameters.fftSize = (jvxInt32) 1 << (hdl->fftParameters.fftSizeLog );
		hdl->fftParameters.acceleratedFft = true;

		assert(hdl->fftParameters.fftSizeLog <= ((jvx_fft_ifft_core_global_common*)glob_ptr)->fftSizeLog);

		hdl->glob_ptr = glob_ptr;
		hdl->operate = operate;
		hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
		hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;

		// calculate twiddle factors (only once for each used fft length!)
		jvxInt32 realFftSize = hdl->fftParameters.fftSize; // JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX
		jvxFFTSize realFftType = fftType;
		if(corefftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX || corefftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL)
		{
			assert(fftType >= JVX_FFT_TOOLS_FFT_SIZE_32); // TODO implement for FFT length 16 (which uses complex-2-complex FFT length 8)
			realFftSize >>= 1; // for real-2-complex we use N/2 complex-2-complex FFT
			realFftType -= 1;
		}
		jvxData **twiddleN = &(global_hdl->twiddleFactors[(int)realFftType]);
		if(!*twiddleN)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(*twiddleN, jvxData, 2*realFftSize);
			assert(*twiddleN);
			tw_gen(*twiddleN, realFftSize);
		}
	}
	else
	{
		res = JVX_DSP_ERROR_INVALID_ARGUMENT;
	}
	return res;
}


// =======================================================================================

jvxDspBaseErrorType jvx_create_fft_ifft_global(jvxFFTGlobal** glob_hdl,
						   jvxFFTSize fftType_max)
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

	// calculate A and B
	calculate_A_B(&(global_hdl->A[(int)(fftType)]),
				  &(global_hdl->B[(int)(fftType)]),
				  ptr->common.fftParameters.fftSize);

	// Allocate input
	JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxData, ptr->common.fftParameters.fftSize);
	JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, (ptr->common.fftParameters.fftSize/2)+1);
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->temp, jvxDataCplx, (ptr->common.fftParameters.fftSize/2)+1);

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
	JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, ptr->common.fftParameters.fftSize);
	JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, ptr->common.fftParameters.fftSize);

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

	// calculate A and B
	calculate_A_B(&(global_hdl->A[(int)(fftType)]),
				  &(global_hdl->B[(int)(fftType)]),
				  ptr->common.fftParameters.fftSize);

	// Allocate input
	JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, (ptr->common.fftParameters.fftSize/2)+1);
	JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxData, ptr->common.fftParameters.fftSize);
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->temp, jvxDataCplx, (ptr->common.fftParameters.fftSize/2));

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
	JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, ptr->common.fftParameters.fftSize);
	JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, ptr->common.fftParameters.fftSize);

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


jvxDspBaseErrorType jvx_fft_ifft_add_input_shadow_buffer(jvxHandle* hdl_extern, jvxHandle* buffer,
                                                         jvxFftTools_coreFftType tp)
{
    jvx_fft_ifft_core_common* hdl = (jvx_fft_ifft_core_common*)hdl_extern;
    assert(hdl->fftParameters.coreFftType==tp);
    assert(hdl->shadow_buffer==0);
    hdl->shadow_buffer = (void*)buffer;
    return JVX_DSP_NO_ERROR;
}



typedef void (*DSPF_sp_fftifftSPxSP)(int N, float *x, float *w, float *y,
									 unsigned char *brev, int n_min, int offset, int n_max);

// TODO validate all decompositions
static void fft_wrap(DSPF_sp_fftifftSPxSP fft, int N, float *x, float *w, float *y,
					 unsigned char *brev)
{
	/* README
	 * see http://www.ti.com/lit/ug/spru657c/spru657c.pdf
	 * There are cases where manual optimization of this function will lead to better timings.
	 * Problems that might occurr are:
	 * 1) (cache) thrashing
	 * 2) non-optimal complexity if N is not a 4 potency
	 * Depending on how much L1/L2 cache is used as SRAM instead of actual cache memory, you
	 * might want to enable the 1024 and 4096 cases to resolve 1).
	 * In mist cases there will be a trade-off between function call overhead and FFT complexity.
	 * Benchmarking the target platform is probably the best way to find the fastest configuration.
	 * If N is a 4 potency and not large enough to produce thrashing, simply calling the DSPLIB fft/ifft
	 * should be the best solution.
	 * Split FFT calls are made in reverse order to maximize cache reuse.
	 */

	switch(N)
	{
	case 16: // 256B
	case 64: // 1kB
	case 256: // 4kB
		fft(N, x, w, y, brev, 4, 0, N); // radix-4
		break;

	case 512: // 8kB
#if 0 // radix-4
		fft(512, &x[2*0],   &w[0],     y, brev, 256, 0,   512); // radix-2
		fft(256, &x[2*256], &w[2*256], y, brev, 4,   256, 512);
		fft(256, &x[2*0],   &w[2*256], y, brev, 4,   0,   512);
#else // radix-2
		fft(512, x, w, y, brev, 2, 0, 512);
#endif
		break;

	case 1024: // 4^5 // 16 kB
#if 0 // radix-4
		fft(1024, &x[2*0],   &w[0],     y, brev, 256, 0,   1024); // radix-4
		fft(256,  &x[2*768], &w[2*768], y, brev, 4,   768, 1024);
		fft(256,  &x[2*512], &w[2*768], y, brev, 4,   512, 1024);
		fft(256,  &x[2*256], &w[2*768], y, brev, 4,   256, 1024);
		fft(256,  &x[2*0],   &w[2*768], y, brev, 4,   0,   1024);
#else // radix-4
		fft(1024, x, w, y, brev, 4, 0, 1024);
#endif
		break;

	case 2048: // 32kB
#if 0 // radix-4
		fft(2048, &x[2*0],    &w[0],      y, brev, 1024, 0,    2048); // radix-2
		fft(1024, &x[2*1024], &w[2*1024], y, brev, 4,    1024, 2048);
		fft(1024, &x[2*0],    &w[2*1024], y, brev, 4,    0,    2048);
#elif 0 // radix-2
		fft(2048, &x[2*0],    &w[0],      y, brev, 512, 0,    2048); // radix-4
		fft(512,  &x[2*1536], &w[2*1536], y, brev, 2,   1536, 2048);
		fft(512,  &x[2*1024], &w[2*1536], y, brev, 2,   1024, 2048);
		fft(512,  &x[2*512],  &w[2*1536], y, brev, 2,   512,  2048);
		fft(512,  &x[2*0],    &w[2*1536], y, brev, 2,   0,    2048);
#elif 0 // radix-4 but many calls
		fft(2048, &x[2*0],    &w[0],      y, brev, 256, 0,    2048); // radix-4
		fft(256,  &x[2*1792], &w[2*1792], y, brev, 4,   1792, 2048);
		fft(256,  &x[2*1536], &w[2*1792], y, brev, 4,   1536, 2048);
		fft(256,  &x[2*1280], &w[2*1792], y, brev, 4,   1280, 2048);
		fft(256,  &x[2*1024], &w[2*1792], y, brev, 4,   1024, 2048);
		fft(256,  &x[2*768],  &w[2*1792], y, brev, 4,   768,  2048);
		fft(256,  &x[2*512],  &w[2*1792], y, brev, 4,   512,  2048);
		fft(256,  &x[2*256],  &w[2*1792], y, brev, 4,   256,  2048);
		fft(256,  &x[2*0],    &w[2*1792], y, brev, 4,   0,    2048);
#else // radix-2
		fft(2048, x, w, y, brev, 2, 0, 2048);
#endif
		break;


	case 4096: // 4^6 // 64 kB
#if 0 // radix-4
		fft(4096, &x[2*0],    &w[0],      y, brev, 2048, 0,    4096); // radix-2
		fft(2048, &x[2*2048], &w[2*2048], y, brev, 4,    2048, 4096);
		fft(2048, &x[2*0],    &w[2*2048], y, brev, 4,    0,    4096);
#elif 0 // radix-4
		fft(4096, &x[2*0],    &w[0],      y, brev, 1024, 0,    4096); // radix-4
		fft(1024, &x[2*3072], &w[2*3072], y, brev, 4,    3072, 4096);
		fft(1024, &x[2*2048], &w[2*3072], y, brev, 4,    2048, 4096);
		fft(1024, &x[2*1024], &w[2*3072], y, brev, 4,    1024, 4096);
		fft(1024, &x[2*0],    &w[2*3072], y, brev, 4,    0,    4096);
#else // radix-4
		fft(4096, x, w, y, brev, 4, 0, 4096);
#endif
		break;

	case 8192: // 128 kB
#if 0 // radix-4
		fft(8192, &x[2*0],    &w[0],      y, brev, 4096, 0,    8192);
		fft(4096, &x[2*4096], &w[2*4096], y, brev, 4,    4096, 8192);
		fft(4096, &x[2*0],    &w[2*4096], y, brev, 4,    0,    8192);
#elif 0 // radix-2
		fft(8192, &x[2*0],    &w[0],      y, brev, 2048, 0,    8192);
		fft(2048, &x[2*6144], &w[2*6144], y, brev, 2,    6144, 8192);
		fft(2048, &x[2*4096], &w[2*6144], y, brev, 2,    4096, 8192);
		fft(2048, &x[2*2048], &w[2*6144], y, brev, 2,    2048, 8192);
		fft(2048, &x[2*0],    &w[2*6144], y, brev, 2,    0,    8192);
#elif0 // radix-4 but many calls
		fft(8192, &x[2*0],    &w[0],      y, brev, 1024, 0,    8192);
		fft(1024, &x[2*7168], &w[2*7168], y, brev, 4,    7168, 8192);
		fft(1024, &x[2*6144], &w[2*7168], y, brev, 4,    6144, 8192);
		fft(1024, &x[2*5120], &w[2*7168], y, brev, 4,    5120, 8192);
		fft(1024, &x[2*4096], &w[2*7168], y, brev, 4,    4096, 8192);
		fft(1024, &x[2*3072], &w[2*7168], y, brev, 4,    3072, 8192);
		fft(1024, &x[2*2048], &w[2*7168], y, brev, 4,    2048, 8192);
		fft(1024, &x[2*1024], &w[2*7168], y, brev, 4,    1024, 8192);
		fft(1024, &x[2*0],    &w[2*7168], y, brev, 4,    0,    8192);
#else // radix-2
		fft(8192, x, w, y, brev, 2, 0, 8192);
#endif
		break;

	default:
		// radix-2
		fft(N, x, w, y, brev, 2, 0, N);
	}
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
		fft_wrap(DSPF_sp_fftSPxSP,
				 hdl_fft_c2c->common.fftParameters.fftSize,
				 (float*)input,
				 hdl->glob_ptr->twiddleFactors[(int)hdl_fft_c2c->common.fftParameters.fftType],
				 (float*)hdl_fft_c2c->output,
				 brev);
	}
	else if(hdl->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX)
	{
		int i;
		const jvxFFTSize fftSize = hdl_fft_r2c->common.fftParameters.fftType;
		const jvxDataCplx * JVX_RESTRICT A = hdl->glob_ptr->A[(int)fftSize];
		const jvxDataCplx * JVX_RESTRICT B = hdl->glob_ptr->B[(int)fftSize];
		jvxDataCplx * JVX_RESTRICT X = hdl_fft_r2c->output;
		jvxDataCplx *JVX_RESTRICT T = hdl_fft_r2c->temp;
		_nassert ((int)A % 8 == 0);
		_nassert ((int)B % 8 == 0);
		_nassert ((int)X % 8 == 0);
		_nassert ((int)T % 8 == 0);
		const int N = hdl_fft_r2c->common.fftParameters.fftSize;

		// compute N/2 point c-2-c FFT
		fft_wrap(DSPF_sp_fftSPxSP,
				 N/2,
				 (float*)input,
				 hdl->glob_ptr->twiddleFactors[((int)fftSize)-1],
				 (float*)hdl_fft_r2c->temp,
				 brev);

		// compute output data
		// see http://processors.wiki.ti.com/index.php/Efficient_FFT_Computation_of_Real_Input
		T[N/2] = T[0];
#pragma MUST_ITERATE(16, 4096, 16)
		for (i = 0; i < N/2; i++)
		{
			X[i].re = T[i].re * A[i].re - T[i].im * A[i].im + T[N/2-i].re * B[i].re + T[N/2-i].im * B[i].im;
			X[i].im = T[i].im * A[i].re + T[i].re * A[i].im + T[N/2-i].re * B[i].im - T[N/2-i].im * B[i].re;
		}
		X[N/2].re = T[0].re - T[0].im;
		X[N/2].im = 0;
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
		fft_wrap(DSPF_sp_ifftSPxSP,
				 hdl_ifft_c2c->common.fftParameters.fftSize,
				 (float*)hdl_ifft_c2c->input,
				 hdl->glob_ptr->twiddleFactors[(int)hdl_ifft_c2c->common.fftParameters.fftType],
				 (float*)hdl_ifft_c2c->output,
				 brev);
	}
	else if(hdl->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL)
	{
		int i;
		const jvxFFTSize fftSize = hdl_ifft_c2r->common.fftParameters.fftType;
		const jvxDataCplx * JVX_RESTRICT A = hdl->glob_ptr->A[(int)fftSize];
		const jvxDataCplx * JVX_RESTRICT B = hdl->glob_ptr->B[(int)fftSize];
		jvxDataCplx * JVX_RESTRICT X = hdl_ifft_c2r->input;
		jvxDataCplx * JVX_RESTRICT T = hdl_ifft_c2r->temp;
		_nassert ((int)A % 8 == 0);
		_nassert ((int)B % 8 == 0);
		_nassert ((int)X % 8 == 0);
		_nassert ((int)T % 8 == 0);
		const int N = hdl_ifft_c2r->common.fftParameters.fftSize;

		// compute input data
		// see http://processors.wiki.ti.com/index.php/Efficient_FFT_Computation_of_Real_Input
		// note that IA(k) is the complex conjugate of A(k) and IB(k) is the complex conjugate of B(k)
#pragma MUST_ITERATE(16, 4096, 16)
		for (i = 0; i < N/2; i++)
		{
			T[i].re = X[i].re * A[i].re + X[i].im * A[i].im + X[N/2-i].re * B[i].re - X[N/2-i].im * B[i].im;
			T[i].im = X[i].im * A[i].re - X[i].re * A[i].im - X[N/2-i].re * B[i].im - X[N/2-i].im * B[i].re;
		}

		// compute N/2 point c-2-c IFFT
		fft_wrap(DSPF_sp_ifftSPxSP,
				 N/2,
				 (float*)hdl_ifft_c2r->temp,
				 hdl->glob_ptr->twiddleFactors[((int)fftSize)-1],
				 (float*)hdl_ifft_c2r->output,
				 brev);
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
