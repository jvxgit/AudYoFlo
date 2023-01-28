#ifndef __CJVX_FAST_CONVOLUTION_H__
#define __CJVX_FAST_CONVOLUTION_H__

#include "jvx_fft_tools/jvx_firfft.h"

class CjvxFastConvolution
{

public:
	CjvxFastConvolution();
	~CjvxFastConvolution();
	CjvxFastConvolution(const CjvxFastConvolution&);

	int init(jvxSize fir_size, jvxSize b_size);
	int set_ir(jvxData* ir);
	int process(jvxData* in, jvxData* out);

private:
	bool is_initialized = false;

	jvxFFTGlobal* fft_global = nullptr;

	jvxFFT* fft_ir = nullptr;
	jvxSize fft_ir_size = 0;
	jvxFFTSize fft_ir_size_enum = JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE; 
	jvxData* fft_ir_signal_time = nullptr;
	jvxDataCplx* fft_ir_signal_spec = nullptr;

	jvx_firfft* hdl_firfft = nullptr;
	JVX_MUTEX_HANDLE mutex_ir_buffers;
};

#endif