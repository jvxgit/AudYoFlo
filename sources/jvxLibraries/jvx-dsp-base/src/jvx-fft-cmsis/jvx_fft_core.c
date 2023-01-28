#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_core_typedefs.h"

#include "jvx_helpers.h"
#include "jvx_allocate.h"
#include "jvx_dataformats.h"

#include "arm_const_structs.h"

static jvxData* ptrTmpBuf = NULL;

// =============================================================================
// =============================================================================
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
#error "Only 32 bit float supported by CMSIS"
#endif

#define JVX_FFT_BUFFERS_ALLOCATE(varName_buf, varName_type, type, numElements) \
    if(varName_buf == NULL)                                                    \
    {                                                                          \
        JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptr->varName_buf, type, numElements);      \
        ptr->common.varName_type = JVX_FFT_IFFT_BUFFER_ALLOCATED;              \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        ptr->varName_buf = varName_buf;                                        \
        ptr->common.varName_type = JVX_FFT_IFFT_BUFFER_EXTERNAL;               \
    }                            \
    memset(ptr->varName_buf, 0, sizeof(type) * (numElements));

// ####################################################################

static jvxDspBaseErrorType jvx_core_fft_init_common(jvx_fft_ifft_core_common* hdl,
                                                    jvxFftTools_coreFftType corefftType,
                                                    jvxFFTSize fftType,
                                                    jvxFftIfftOperate operate,
                                                    jvxFFTGlobal* glob_ptr,
                                                    jvxSize fftSizeArbitrary)
{
    jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;

    assert(fftType >= JVX_FFT_TOOLS_FFT_SIZE_32 && fftType <= JVX_FFT_TOOLS_FFT_SIZE_4096);  // CMSIS constraints

    if(hdl && glob_ptr)
    {
        hdl->fftParameters.coreFftType = corefftType;
        hdl->fftParameters.fftType = fftType;
        hdl->fftParameters.fftSizeLog = (int)fftType + JVX_OFFSET_FFT_TYPE_MIN;
        hdl->fftParameters.fftSize = (jvxInt32) 1 << (hdl->fftParameters.fftSizeLog);
        hdl->fftParameters.acceleratedFft = true;

        assert(hdl->fftParameters.fftSizeLog <= ((jvx_fft_ifft_core_global_common*)glob_ptr)->fftSizeLog);

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

	/*
	 * Separate input and output buffers for both JVX_FFT_IFFT_PRESERVE_INPUT and JVX_FFT_IFFT_EFFICIENT,
	 * since CMSIS does not support in-place FFT (if it is called in-place, result will be buggy).
	 */
	if(output)
	{
		ptr->input = input;
		ptr->output = output;
		ptr->common.outputOrigin = JVX_FFT_IFFT_BUFFER_EXTERNAL;
		ptr->common.inputOrigin = ptr->common.outputOrigin;
	}
	else
	{
		// ==============================================================
		// Allocate input and output
		// ==============================================================
		JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxDataCplx, (ptr->common.fftParameters.fftSize/2+1));
		JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxData, (ptr->common.fftParameters.fftSize));
	}
	if (operate == JVX_FFT_IFFT_PRESERVE_INPUT) {
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptrTmpBuf, jvxData, ptr->common.fftParameters.fftSize);
		if (!ptrTmpBuf) assert(0);
	}

	// ==============================================================
	// Create fft handles
	// ==============================================================
	arm_status cmsis_res;
	cmsis_res = arm_rfft_fast_init_f32(&ptr->cmsis_inst, ptr->common.fftParameters.fftSize);
	if (cmsis_res != ARM_MATH_SUCCESS)
		res = JVX_DSP_ERROR_INTERNAL;

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

    if((void*)input != (void*)output)
        return JVX_DSP_ERROR_INVALID_ARGUMENT; // cmsis is inplace

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
	ptr->output = (jvxDataCplx*)ptr->input;
	ptr->common.outputOrigin = ptr->common.inputOrigin;

	// assign lookup tables
	switch (ptr->common.fftParameters.fftSize) {
	case 16:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len16;
		break;
	case 32:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len32;
		break;
	case 64:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len64;
		break;
	case 128:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len128;
		break;
	case 256:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len256;
		break;
	case 512:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len512;
		break;
	case 1024:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len1024;
		break;
	case 2048:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len2048;
		break;
	case 4096:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len4096;
		break;
	default:
		return JVX_DSP_ERROR_INTERNAL;
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

	// Allocate input and output
	JVX_FFT_BUFFERS_ALLOCATE(input, inputOrigin, jvxDataCplx, ptr->common.fftParameters.fftSize/2+1);
	JVX_FFT_BUFFERS_ALLOCATE(output, outputOrigin, jvxData, (ptr->common.fftParameters.fftSize));

	arm_status cmsis_res;
	cmsis_res = arm_rfft_fast_init_f32(&ptr->cmsis_inst, ptr->common.fftParameters.fftSize);
	if (cmsis_res != ARM_MATH_SUCCESS)
		res = JVX_DSP_ERROR_INTERNAL;

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

    if((void*)input != (void*)output)
        return JVX_DSP_ERROR_INVALID_ARGUMENT; // cmsis is inplace

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
	ptr->output = (jvxDataCplx*)ptr->input;
	ptr->common.outputOrigin = ptr->common.inputOrigin;

	// assign lookup tables
	switch (ptr->common.fftParameters.fftSize) {
	case 16:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len16;
		break;
	case 32:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len32;
		break;
	case 64:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len64;
		break;
	case 128:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len128;
		break;
	case 256:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len256;
		break;
	case 512:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len512;
		break;
	case 1024:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len1024;
		break;
	case 2048:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len2048;
		break;
	case 4096:
		ptr->cmsis_hdl = &arm_cfft_sR_f32_len4096;
		break;
	default:
		return JVX_DSP_ERROR_INTERNAL;
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

    if(res == JVX_DSP_NO_ERROR)
    {
        *hdl = (jvx_fft_ifft_core_common*) ptr;
    }

    return res;
}

jvxDspBaseErrorType jvx_execute_fft(jvxFFT* hdlRef)
{
	//jvxSize i;
    jvx_fft_ifft_core_common* hdl = (jvx_fft_ifft_core_common*)hdlRef;
    jvx_fft_core_real_2_complex* hdl_fft_r2c = (jvx_fft_core_real_2_complex*)hdl;
    //jvx_fft_core_complex_2_complex* hdl_fft_c2c = (jvx_fft_core_complex_2_complex*)hdl;

    switch(hdl->fftParameters.coreFftType)
    {
    case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX:
    case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX:
    case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL:

    	// Not yet implemented..
    	assert(0);
    	// complex FFT is in-place
    	//arm_cfft_f32(hdl_fft_c2c->cmsis_hdl, (jvxData*)hdl_fft_c2c->input, 0, 1);
    	break;

    case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX:
    	// Quick test of FFT #1:
    	//hdl_fft_r2c->input[0] = 128.0;
    	//for(i = 1; i < hdl->fftParameters.fftSize; i++)
    	//{
    	//	hdl_fft_r2c->input[i] = 0.0;
    	//}

    	// Quick test of FFT #2:
    	// Matlab: idxMax = 5; ffts = 512; vec = cos(2*pi * [0:ffts-1]/ffts * idxMax); res = abs(fft(vec)); figure; plot(res(1:ffts/2+1));
    	//for(i = 0; i < hdl->fftParameters.fftSize; i++)
    	//{
    	//	hdl_fft_r2c->input[i] = arm_cos_f32((jvxData)4.0 * 2*M_PI * (jvxData)i / (jvxData)hdl->fftParameters.fftSize );
    	//}

    	if (hdl->operate == JVX_FFT_IFFT_PRESERVE_INPUT) {
    		memcpy(ptrTmpBuf, hdl_fft_r2c->input, sizeof(jvxData) * hdl->fftParameters.fftSize);
    	}
        arm_rfft_fast_f32(&hdl_fft_r2c->cmsis_inst, (float*)hdl_fft_r2c->input, (float*)hdl_fft_r2c->output, 0);
        hdl_fft_r2c->output[hdl->fftParameters.fftSize/2].re = hdl_fft_r2c->output[0].im;
        hdl_fft_r2c->output[hdl->fftParameters.fftSize/2].im = 0.0;
        hdl_fft_r2c->output[0].im = 0.0;
        if (hdl->operate == JVX_FFT_IFFT_PRESERVE_INPUT) {
			memcpy(hdl_fft_r2c->input, ptrTmpBuf, sizeof(jvxData) * hdl->fftParameters.fftSize);
		}
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
        // complex FFT is in-place
        arm_cfft_f32(hdl_ifft_c2c->cmsis_hdl, (float*)hdl_ifft_c2c->input, 1, 1);
    }
    else if(hdl->fftParameters.coreFftType == JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL)
    {
        arm_rfft_fast_f32(&hdl_ifft_c2r->cmsis_inst, (float*)hdl_ifft_c2r->input, (float*)hdl_ifft_c2r->output, 1);
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

    if (ptrTmpBuf) {
		JVX_DSP_SAFE_DELETE_FIELD(ptrTmpBuf);
	}

    switch(hdl->fftParameters.coreFftType)
    {
    case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX:
        if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
        {
            JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_r2c->input);
        }
        hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
        hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
        hdl_fft_r2c->input = NULL;
        hdl_fft_r2c->output = NULL;

        break;
    case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX:
        if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
        {
            JVX_DSP_SAFE_DELETE_FIELD(hdl_fft_c2c->input);
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
        hdl->inputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
        hdl->outputOrigin = JVX_FFT_IFFT_BUFFER_NOTSET;
        hdl_ifft_c2r->input = NULL;
        hdl_ifft_c2r->output = NULL;

        break;
    case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX:
        if(hdl->inputOrigin == JVX_FFT_IFFT_BUFFER_ALLOCATED)
        {
            JVX_DSP_SAFE_DELETE_FIELD(hdl_ifft_c2c->input);
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
    jvx_fft_ifft_core_global_common* global_hdl = (jvx_fft_ifft_core_global_common*)g_hdl;

    if(global_hdl)
        {
            // All ffts released?
            assert(global_hdl->refCount == 0);

            JVX_DSP_SAFE_DELETE_OBJECT(global_hdl);

            return JVX_DSP_NO_ERROR;
        }
    return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

JVX_FFT_TOOLS_DEFINE_FFT_SIZES

// According to description, the CMSIS FFT minimum size is 32!!
jvxDspBaseErrorType 
jvx_get_nearest_size_fft(jvxFFTSize* szTypeOnReturn, jvxSize n, jvxFftRoundType tp, jvxSize* szOnReturn)
{
	// first valid size is 16 -> subtract 4 to address enum correctly
	int idxLow = (int)floor(log((jvxData)n) / log(2.)) - JVX_OFFSET_FFT_TYPE_MIN;
	int idxHigh = (int)ceil(log((jvxData)n) / log(2.)) - JVX_OFFSET_FFT_TYPE_MIN;
	idxLow = JVX_MAX(JVX_FFT_TOOLS_FFT_SIZE_32, idxLow);
	idxLow = JVX_MIN(JVX_FFT_TOOLS_FFT_SIZE_8192, idxLow);
	idxHigh = JVX_MAX(JVX_FFT_TOOLS_FFT_SIZE_32, idxHigh);
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
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_INVALID_SETTING;
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
