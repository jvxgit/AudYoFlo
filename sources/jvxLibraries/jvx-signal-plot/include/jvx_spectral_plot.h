#ifndef __JVX_SPECTRALPLOTMATRIX_H__
#define __JVX_SPECTRALPLOTMATRIX_H__

#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_windows/jvx_windows.h"

JVX_DSP_LIB_BEGIN

typedef struct
{
	jvx_windowType winType;
	jvxData secondArg;
	jvxData thirdArg;

	jvxFFTSize fftSize;
	jvxSize hopSize;
	jvxSize bufferSize;
	jvxSize channels;
} jvxSpectralPlot_init;

typedef struct
{
	jvxData sensFactor;
	jvxData lowdB;
	jvxData highdB;
} jvxSpectralPlot_async;

typedef struct
{
	jvxSpectralPlot_init initParam;
	jvxSpectralPlot_async asyncParam;
	jvxHandle* prv;
} jvxSpectralPlot;

//typedef jvxHandle jvxSpectralPlot;

jvxDspBaseErrorType jvx_spectral_plot_initCfg(jvxSpectralPlot* hdl);
jvxDspBaseErrorType jvx_spectral_plot_init(jvxSpectralPlot* hdl);
jvxDspBaseErrorType jvx_spectral_plot_process(jvxSpectralPlot* hdl, jvxData* in, jvxByte* out_rgb_ptr, jvxSize rgba_vec_length);
jvxDspBaseErrorType jvx_spectral_plot_terminate(jvxSpectralPlot* hdl);
jvxDspBaseErrorType jvx_spectral_plot_update(jvxSpectralPlot* hdl, jvxUInt16 operation, jvxCBool wantToSet);

JVX_DSP_LIB_END
#endif