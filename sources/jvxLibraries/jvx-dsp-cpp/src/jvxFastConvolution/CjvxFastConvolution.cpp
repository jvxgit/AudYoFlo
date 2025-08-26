#include "jvxFastConvolution/CjvxFastConvolution.h"

#include "jvx.h"

CjvxFastConvolution::CjvxFastConvolution()
{
	JVX_INITIALIZE_MUTEX(this->mutex_ir_buffers);
}

CjvxFastConvolution::~CjvxFastConvolution()
{
	JVX_TERMINATE_MUTEX(this->mutex_ir_buffers);

	if (!this->is_initialized)
		return;

	if (this->hdl_firfft) {
		JVX_DSP_SAFE_DELETE_OBJECT(this->hdl_firfft->init.fir);
		this->hdl_firfft->init.fir = NULL;
		jvx_firfft_terminate(this->hdl_firfft);
	}

	if (this->fft_ir)
		jvx_destroy_fft(this->fft_ir);

}

CjvxFastConvolution::CjvxFastConvolution(const CjvxFastConvolution& fast_convolution)
{
	JVX_INITIALIZE_MUTEX(this->mutex_ir_buffers);

	this->fft_ir = nullptr;

	this->fft_ir_signal_time = nullptr;
	this->fft_ir_signal_spec = nullptr;

	this->hdl_firfft = nullptr;
	this->fft_global = nullptr;

	this->fft_ir_size = fast_convolution.fft_ir_size;
	this->fft_ir_size_enum = fast_convolution.fft_ir_size_enum;

	this->is_initialized = fast_convolution.is_initialized;

	if (this->is_initialized){
		this->is_initialized = false;

		this->init(fast_convolution.hdl_firfft->init.lFir, fast_convolution.hdl_firfft->init.bsize);

		if (fast_convolution.fft_ir_signal_time){
			this->set_ir(fast_convolution.fft_ir_signal_time);
		}
	}
}

int CjvxFastConvolution::init(jvxSize fir_size, jvxSize b_size)
{
	if (this->is_initialized)
		return JVX_DSP_ERROR_WRONG_STATE;

	jvxErrorType res = JVX_DSP_NO_ERROR;

	jvxFFTSize fftSizeType;
	jvxSize fftSize;

	// init firfft module
	JVX_SAFE_ALLOCATE_OBJECT_CPP_Z(this->hdl_firfft, jvx_firfft);
	res = jvx_firfft_initCfg(this->hdl_firfft);

	this->hdl_firfft->init.bsize = b_size;
	this->hdl_firfft->init.lFir = fir_size;
	this->hdl_firfft->init.type = JVX_FIRFFT_FLEXIBLE_FIR;
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(this->hdl_firfft->init.fir, jvxData, fir_size);

	this->hdl_firfft->init.lFft = fir_size;
	this->hdl_firfft->init.delayFir = 0;

	jvx_firfft_init(this->hdl_firfft, nullptr);

	// init fft module for impulse response
	this->fft_ir_size_enum = this->hdl_firfft->derived.szFft;
	this->fft_ir_size = jvx_get_fft_size(this->fft_ir_size_enum);

	jvx_create_fft_ifft_global(&this->fft_global, this->fft_ir_size_enum, NULL);
	const jvxFftIfftOperate fft_operate = JVX_FFT_IFFT_PRESERVE_INPUT;
	jvxData* input = nullptr;
	jvxDataCplx* output = nullptr;
	const jvxSize fftSizeArbitrary = 0;

	res = jvx_create_fft_real_2_complex(
		&this->fft_ir, // jvxFFT** hdlRef,
		this->fft_global, // jvxFFTGlobal* g_hdl,
		this->fft_ir_size_enum, // jvxFFTSize fftType,
		&this->fft_ir_signal_time, // jvxData** in_ptr_fld_N,
		&this->fft_ir_signal_spec, // jvxDataCplx** out_ptr_fld_N2P1,
		&this->fft_ir_size, // jvxSize* N,
		fft_operate, // jvxFftIfftOperate operate,
		input, // jvxData* input,
		output, // jvxDataCplx* output,
		fftSizeArbitrary // jvxSize fftSizeArbitrary)
	);

	this->is_initialized = true;

	return res;
}

int CjvxFastConvolution::set_ir(jvxData* fir) {
	
	if (!this->is_initialized)
		return JVX_DSP_ERROR_WRONG_STATE;

	jvxErrorType res = JVX_DSP_NO_ERROR;

	// prevent change of IR during process
	JVX_LOCK_MUTEX(this->mutex_ir_buffers);

	// copy fir to beginning of fft_ir_signal_time
	std::copy(fir, fir + this->hdl_firfft->init.lFir, this->fft_ir_signal_time);
	
	// transform fft_ir_signal_time to fft_ir_signal_spec
	res = jvx_execute_fft(this->fft_ir);
	
	// copy spectral weights to filter module
	const jvxSize fft_size = this->hdl_firfft->derived.szFftValue;

	std::copy(this->fft_ir_signal_spec, this->fft_ir_signal_spec + (fft_size/2+1), this->hdl_firfft->sync.firW);

	JVX_UNLOCK_MUTEX(this->mutex_ir_buffers);

	return res;
}

int CjvxFastConvolution::process(jvxData* in, jvxData* out) {
	jvxErrorType res = JVX_DSP_NO_ERROR;
	
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE lock_result = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
	JVX_TRY_LOCK_MUTEX(lock_result, this->mutex_ir_buffers);

	jvx_firfft_process(this->hdl_firfft, in, out);
	
	if (lock_result == JVX_TRY_LOCK_MUTEX_SUCCESS)
		JVX_UNLOCK_MUTEX(this->mutex_ir_buffers);

	return res;
}
