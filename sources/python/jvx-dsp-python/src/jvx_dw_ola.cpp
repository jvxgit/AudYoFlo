#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "jvx.h"
#include "jvx_fft_processing/jvx_fft_processing.h"

JVX_FFT_TOOLS_DEFINE_FFT_SIZES

// ===========================================================================
class oneInstanceDwOla
{
public:
    jvxSize hop_size = 256;
    jvxSize fft_buffersize = 1024;

    jvxFFTSize szfft = JVX_FFT_TOOLS_FFT_SIZE_1024;
    jvxSize numSpectrumCoeffs = 0;

    jvxFFTGlobal* globalFft = nullptr;
    
    jvxFFT* hdlFft = nullptr;
    jvxFFTfwk* hdlFwkFft = nullptr;

    jvxIFFT* hdlIFft = nullptr;
    jvxIFFTfwk* hdlFwkIFft = nullptr;
    
    jvxData* inputFFT = nullptr;
    jvxDataCplx* outputFFT = nullptr;

    jvxData* outputIFFT = nullptr;
    jvxDataCplx* inputIFFT = nullptr;

};

void* init_processing_dw_ola(int hopsize, int framesize) 
{
    oneInstanceDwOla* hdl = nullptr;
    
    jvxData fwk_delay = 0;
    jvxInt32 dummy1 = 0;
    jvxInt32 dummy2 = 0;
    jvxSize loc_fftsize = 0;

    jvxErrorType res = JVX_NO_ERROR;
    JVX_DSP_SAFE_ALLOCATE_OBJECT(hdl, oneInstanceDwOla);

    hdl->fft_buffersize = framesize;
    hdl->hop_size = hopsize;

    // =================================================================================
    // INITIALIZATION
    // =================================================================================
    jvx_get_nearest_size_fft(&hdl->szfft, hdl->fft_buffersize, JVX_FFT_ROUND_NEAREST, &loc_fftsize);

    hdl->szfft = (jvxFFTSize)(JVX_DATA2INT32(log10((jvxData)hdl->fft_buffersize) / log10(2.0) - log10(jvxFFTSize_sizes[0])/log10(2.0)));// <- First allowed FFT size is 16
     
    // Derive number of spec coefficients
    hdl->numSpectrumCoeffs = jvxFFTSize_sizes[hdl->szfft] / 2 + 1;

    // Allocate global data for FFT usage (reuse twiddle factors in Mac cases)
    res = jvx_create_fft_ifft_global(&hdl->globalFft, (jvxFFTSize)JVX_FFT_TOOLS_FFT_SIZE_1024, nullptr);
    assert(res == JVX_DSP_NO_ERROR);

    // Input side
    res = jvx_create_fft_real_2_complex(&hdl->hdlFft,
        hdl->globalFft, (jvxFFTSize)hdl->szfft,
        NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
    assert(res == JVX_DSP_NO_ERROR);

    // Allocate framework
    res = jvx_fft_tools_init_framework_dw_ola(
        &hdl->hdlFwkFft, NULL,
        (jvxInt32)hdl->hop_size, 
        hdl->fft_buffersize,
        hdl->hdlFft, NULL,
        &hdl->inputFFT, NULL,
        NULL, &hdl->outputFFT,
        &dummy1, &dummy2, &fwk_delay);
    assert(res == JVX_DSP_NO_ERROR);

    assert(dummy1 == hdl->hop_size);
    assert(dummy2 == hdl->numSpectrumCoeffs);
    
    // Output side
    res = jvx_create_ifft_complex_2_real(&hdl->hdlIFft,
        hdl->globalFft, (jvxFFTSize)JVX_FFT_TOOLS_FFT_SIZE_1024,
        NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
    assert(res == JVX_DSP_NO_ERROR);

    // Allocate framework
    res = jvx_fft_tools_init_framework_dw_ola(
        NULL, &hdl->hdlFwkIFft,
        (jvxInt32)hdl->hop_size, 
        hdl->fft_buffersize,
        NULL, hdl->hdlIFft,
        NULL, &hdl->outputIFFT,
        &hdl->inputIFFT, NULL,
        &dummy1, &dummy2, &fwk_delay);
    assert(res == JVX_DSP_NO_ERROR);
    assert(dummy1 == hdl->hop_size);
    assert(dummy2 == hdl->numSpectrumCoeffs);
    
    return hdl;
}

int term_processing_dw_ola(void* hdlArg)
{
    jvxErrorType res = JVX_NO_ERROR;
    
    oneInstanceDwOla* hdl = (oneInstanceDwOla*)hdlArg;

    // Deallocate framework output side
    res = jvx_fft_tools_terminate_framework(
        NULL, hdl->hdlFwkIFft);
    assert(res == JVX_DSP_NO_ERROR);

    res = jvx_destroy_ifft(hdl->hdlIFft);
    assert(res == JVX_DSP_NO_ERROR);

    hdl->hdlIFft = NULL;
    hdl->hdlFwkIFft = NULL;
    hdl->outputIFFT = NULL;
    hdl->inputIFFT = NULL;

    // Deallocate framework for input side
    res = jvx_fft_tools_terminate_framework(
        hdl->hdlFwkFft, NULL);
    assert(res == JVX_DSP_NO_ERROR);

    res = jvx_destroy_fft(hdl->hdlFft);
    assert(res == JVX_DSP_NO_ERROR);

    hdl->hdlFft = NULL;
    hdl->hdlFwkFft = NULL;
    hdl->inputFFT = NULL;
    hdl->outputFFT = NULL;

    res = jvx_destroy_fft_ifft_global(hdl->globalFft);
    assert(res == JVX_DSP_NO_ERROR);
    hdl->globalFft = NULL;

    JVX_DSP_SAFE_DELETE_OBJECT(hdl);
    
    return JVX_NO_ERROR;
}


int run_processing_dw_ola_raw(void* hdlArg, jvxData* bufio, const std::complex<jvxData>* weights)
{
	oneInstanceDwOla* hdl = (oneInstanceDwOla*)hdlArg;

	// Run input side
	jvx_fft_tools_process_framework_fft(hdl->hdlFwkFft, (jvxData*)bufio);


	for (jvxSize i = 0; i < hdl->numSpectrumCoeffs; i++)
	{
		// Complex multiplication
		hdl->inputIFFT[i].re = hdl->outputFFT[i].re * weights[i].real() - hdl->outputFFT[i].im * weights[i].imag();
		hdl->inputIFFT[i].im = hdl->outputFFT[i].im * weights[i].real() + hdl->outputFFT[i].re * weights[i].imag();
	}

	// Run output side
	jvx_fft_tools_process_framework_ifft(hdl->hdlFwkIFft, (jvxData*)bufio, 0);

	return JVX_NO_ERROR;
}


int run_processing_dw_ola(void* hdlArg, pybind11::array_t<jvxData> bufio, pybind11::array_t<std::complex<jvxData>> weights)
{
    
    jvxData* buf_in_out = bufio.mutable_data();
    const std::complex<jvxData>* buf_weight = weights.data();
    oneInstanceDwOla* hdl = (oneInstanceDwOla*)hdlArg;

	run_processing_dw_ola_raw(hdlArg, buf_in_out, buf_weight);
    
    return JVX_NO_ERROR;    
}
