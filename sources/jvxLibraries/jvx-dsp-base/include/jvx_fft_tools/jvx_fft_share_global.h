#ifndef __AYF_COMMON_COMPONENTS_H__
#define __AYF_COMMON_COMPONENTS_H__

#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_fft_tools.h"

JVX_SYSTEM_LIB_BEGIN

jvxErrorType jvx_create_or_request_fft_global(jvxFFTGlobal** hdlOnReturn, jvxFFTSize szFft, jvxCBool* wasCreated);
jvxErrorType jvx_return_or_destroy_fft_global(jvxFFTGlobal* hdlToReturn, jvxCBool* wasDestroyed);

JVX_SYSTEM_LIB_END

#endif

