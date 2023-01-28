#ifndef __JVX_SMOOTH_SLIDING_WINDOW_H__
#define __JVX_SMOOTH_SLIDING_WINDOW_H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

jvxDspBaseErrorType jvx_smooth_sliding_window_init(jvxHandle** hdl, jvxSize dimVector, jvxSize halfWinLength);

jvxDspBaseErrorType jvx_smooth_sliding_window_terminate(jvxHandle* hdl);

jvxDspBaseErrorType jvx_smooth_sliding_window_process(jvxHandle* hdl, jvxData* bufIn, jvxData* bufOut);

JVX_DSP_LIB_END

#endif