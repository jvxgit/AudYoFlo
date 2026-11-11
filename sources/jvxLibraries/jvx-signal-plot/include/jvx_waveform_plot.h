#ifndef __JVX_WAVEFORMPLOTMATRIX_H__
#define __JVX_WAVEFORMPLOTMATRIX_H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

typedef struct
{
	jvxSize bsize;
	jvxData amplMax;
	jvxData amplMin;
} jvxWaveformPlot_init;

typedef struct
{
	jvxData gainFactor;
	jvxCBool absAmplPlot;
	jvxData smoothFactor;
} jvxWaveformPlot_async;

typedef struct
{
	jvxWaveformPlot_init initPrm;
	jvxWaveformPlot_async asyncParam;
	jvxHandle* prv;
} jvxWaveformPlot;

jvxDspBaseErrorType jvx_waveform_plot_initCfg(jvxWaveformPlot* hdl);
jvxDspBaseErrorType jvx_waveform_plot_init(jvxWaveformPlot* hdl);
jvxDspBaseErrorType jvx_waveform_plot_terminate(jvxWaveformPlot* hdl);
jvxDspBaseErrorType jvx_waveform_plot_process(jvxWaveformPlot* hdl, jvxData* ptrShowMe, jvxByte* out_rgb_ptr, jvxSize rgba_vec_length);
jvxDspBaseErrorType jvx_waveform_plot_update(jvxWaveformPlot* hdl, jvxUInt16 operation, jvxCBool wantToSet);

JVX_DSP_LIB_END

#endif

