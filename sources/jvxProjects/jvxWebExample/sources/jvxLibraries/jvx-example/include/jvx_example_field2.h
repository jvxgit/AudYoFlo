#ifndef __JVX_EXAMPLE_FIELD2_H__
#define __JVX_EXAMPLE_FIELD2_H__

#include "jvx_dsp_base.h"

typedef struct
{
	const char* dev_target;
} jvx_example_field2_init_str;

typedef struct
{
    jvxSize fld_num;
    jvxSize fld_sz;
    jvxUInt32* fld;
}jvx_example_field2_sync_str;

typedef struct
{
    jvx_example_field2_init_str prmInit;
    jvx_example_field2_sync_str prmSync;
    jvxHandle* prv;
} jvx_example_field2;

#ifdef __cplusplus
extern "C" {
#endif

    jvxDspBaseErrorType jvx_example_field2_initCfg(jvx_example_field2* theHandle);
    jvxDspBaseErrorType jvx_example_field2_init(jvx_example_field2* theHandle);
    jvxDspBaseErrorType jvx_example_field2_update(jvx_example_field2* theHandle, jvxCBool do_set, jvxUInt16 what);
    // jvxDspBaseErrorType jvx_example_field2_trigger_read(jvx_example_field2* theHandle);
    jvxDspBaseErrorType jvx_example_field2_read_memory(jvx_example_field2* theHandle);
    jvxDspBaseErrorType jvx_example_field2_terminate(jvx_example_field2* theHandle);

#ifdef __cplusplus
}
#endif

#endif
