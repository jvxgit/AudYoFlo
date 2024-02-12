#ifndef __CJVXNECORE_H__
#define __CJVXNECORE_H__

#include "jvx.h"

#include "localMexIncludes.h"

extern "C"
{
#include "jvx_fft_processing/jvx_fft_processing.h"
}

class CjvxFFTFwk
{
protected:
	
	jvx_fft_signal_processing* core_inst = nullptr;
	jvxFFTGlobal* global_hdl = nullptr;

public:

	CjvxFFTFwk();
	jvxErrorType init_fft_fwk(const mxArray* arrCfg);
	jvxErrorType term_fft_fwk();
	jvxErrorType process_fft_fwk_input(jvxData* input, jvxDataCplx* outputBufProc, jvxDataCplx* outputBufAna);
	jvxErrorType process_fft_fwk_output(jvxDataCplx* input, jvxData* weights, jvxData* output);

	jvxSize M2P1 = 0;
	jvxSize hopsize = 0;

};

#endif
