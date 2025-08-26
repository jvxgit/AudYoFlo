#ifndef __JVX_CIRCBUFFERFFT_H__
#define __JVX_CIRCBUFFERFFT_H__

#include "jvx_dsp_base.h"
#include "jvx_circbuffer/jvx_circbuffer.h"
#include "jvx_fft_tools/jvx_fft_core.h"

struct jvxFftIfftHandle;

typedef struct
{
	jvx_circbuffer circBuffer;

	jvxFftTools_coreFftType fftType;
	jvxFFTSize fftSize;
	struct jvxFftIfftHandle* fftifft;

	jvxSize length_cplx;

	struct
	{
		jvxDataCplx** field_cplx;
	} ram;

} jvx_circbuffer_fft;

jvxDspBaseErrorType
jvx_circbuffer_allocate_global_fft_ifft(jvxFFTGlobal** global_fft,
					jvxFFTSize fftType_max, jvxHandle* fftGlobalCfg);

jvxDspBaseErrorType
jvx_circbuffer_destroy_global_fft_ifft(jvxFFTGlobal* global_fft);

jvxDspBaseErrorType
jvx_circbuffer_allocate_fft_ifft(jvx_circbuffer_fft** hdlOnReturn,
	jvxFFTGlobal* hdl_global_fft,
				 jvxFftTools_coreFftType fftType,
				 jvxFFTSize fftSize,
				 jvxCBool preserveInput, 
	jvxSize nChannels);

jvxDspBaseErrorType
jvx_circbuffer_allocate_fft_ifft_extbuf(jvx_circbuffer_fft** hdlOnReturn,
	jvxFFTGlobal* hdl_global_fft,
				 jvxFftTools_coreFftType fftType,
				 jvxFFTSize fftSize,
				 jvxCBool preserveInput,
				 void* extBuffer1, void* extBuffer2);

jvxDspBaseErrorType
jvx_circbuffer_deallocate_fft_ifft(jvx_circbuffer_fft* hdlReturn);

jvxDspBaseErrorType
jvx_circbuffer_deallocate_fft_ifft_extbuf(jvx_circbuffer_fft* hdlReturn);

jvxDspBaseErrorType
jvx_circbuffer_access_cplx_fft_ifft(jvx_circbuffer_fft* hdl, jvxDataCplx** outPtr, jvxSize* outLen, jvxSize idx);

jvxDspBaseErrorType
jvx_circbuffer_get_read_phase_fft_ifft(jvx_circbuffer_fft* hdl, jvxSize* phase);

jvxDspBaseErrorType
jvx_circbuffer_set_read_phase_fft_ifft(jvx_circbuffer_fft* hdl, jvxSize phase);

jvxDspBaseErrorType
jvx_circbuffer_get_write_phase_fft_ifft(jvx_circbuffer_fft* hdl, jvxSize* phase);

jvxDspBaseErrorType
jvx_circbuffer_process_fft_ifft(jvx_circbuffer_fft* hdl);

#endif
