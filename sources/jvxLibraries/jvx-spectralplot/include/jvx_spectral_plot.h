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

	jvxData lowdB;
	jvxData highdB;
} jvxSpectralPlot_init;

typedef struct
{
	jvxData sensFactor;

} jvxSpectralPlot_async;

typedef struct
{
	jvxSpectralPlot_init initPrm;
	jvxSpectralPlot_async asyncParam;
	jvxHandle* prv;
} jvxSpectralPlot;

//typedef jvxHandle jvxSpectralPlot;

jvxDspBaseErrorType jvx_spectral_plot_initCfg(jvxSpectralPlot* hdl);
jvxDspBaseErrorType jvx_spectral_plot_init(jvxSpectralPlot* hdl);
jvxDspBaseErrorType jvx_spectral_plot_process(jvxSpectralPlot* hdl, jvxData* in, jvxByte* out_rgb_ptr);
jvxDspBaseErrorType jvx_spectral_plot_terminate(jvxSpectralPlot* hdl);

#if 0
//selects hann window and range [0dB, 120dB] per default
jvxDspBaseErrorType jvx_spectralplot_init(jvxSpectralPlot** hdl,
	jvxFFTSize fftSize,
	jvxUInt32 inputSize,
	jvxUInt32 hopSize,
	jvxUInt16 channels);

jvxDspBaseErrorType jvx_spectralplot_config_window(jvxSpectralPlot* hdl,
	jvx_windowType winType,
	jvxData secondArg,
	jvxData thirdArg);

//range in dB
jvxDspBaseErrorType jvx_spectralplot_config_range(jvxSpectralPlot* hdl,
	jvxData low,
	jvxData high);

jvxDspBaseErrorType jvx_spectralplot_process(jvxSpectralPlot* hdl,
	jvxData** in,
	jvxUInt8*** out, //channel x hop step x frequency (3-tuple rgb, one value after the other in array)
	jvxCBool* outofRange);

jvxDspBaseErrorType jvx_spectralplot_terminate(jvxSpectralPlot* hdl);
#endif

JVX_DSP_LIB_END
#endif