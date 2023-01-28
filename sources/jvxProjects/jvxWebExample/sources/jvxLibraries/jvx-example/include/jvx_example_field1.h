#ifndef __JVX_EXAMPLE_FIELD1_H__
#define __JVX_EXAMPLE_FIELD1_H__

#include "jvx_dsp_base.h"

typedef struct
{
	const char* dev_target;
} jvx_example_field1_init_str;

typedef struct
{
    jvxSize fld_num;
    jvxSize fld_sz;
    jvxUInt32* fld;
}jvx_example_field1_sync_str;

typedef struct
{
    jvx_example_field1_init_str prmInit;
    jvx_example_field1_sync_str prmSync;
    jvxHandle* prv;
} jvx_example_field1;

#ifdef __cplusplus
extern "C" {
#endif

    jvxDspBaseErrorType jvx_example_field1_initCfg(jvx_example_field1* theHandle);
    jvxDspBaseErrorType jvx_example_field1_init(jvx_example_field1* theHandle);
    jvxDspBaseErrorType jvx_example_field1_update(jvx_example_field1* theHandle, jvxCBool do_set, jvxUInt16 what);
    // jvxDspBaseErrorType jvx_example_field1_trigger_read(jvx_example_field1* theHandle);
    jvxDspBaseErrorType jvx_example_field1_read_memory(jvx_example_field1* theHandle);
    jvxDspBaseErrorType jvx_example_field1_terminate(jvx_example_field1* theHandle);

#ifdef __cplusplus
}
#endif

#endif
