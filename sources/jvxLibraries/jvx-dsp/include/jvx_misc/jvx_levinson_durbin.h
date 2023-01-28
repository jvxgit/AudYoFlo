#ifndef __JVX_LEVINSON_DURBIN_H__
#define __JVX_LEVINSON_DURBIN_H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

jvxDspBaseErrorType jvx_levinson_durbin_initialize(jvxHandle** handleOnReturn, jvxSize lpcOrderMax);

jvxDspBaseErrorType jvx_levinson_durbin_terminate(jvxHandle* handle);

jvxDspBaseErrorType jvx_levinson_durbin_compute(jvxHandle* handle, jvxData* Rxx, jvxData* alpha, jvxData* eStart, jvxData* eStop);

JVX_DSP_LIB_END

#endif
