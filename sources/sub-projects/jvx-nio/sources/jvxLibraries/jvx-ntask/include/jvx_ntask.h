#ifndef __JVX_NIO_H__
#define __JVX_NIO_H__

#include "jvx_dsp_base.h"
						
typedef struct
{
	int dummy;
} jvxNtask_sync;

typedef struct
{
	int dummy;
} jvxNtask_async;

typedef struct
{
	int dummy;
} jvxNtask_init;

typedef struct
{
	jvxNtask_init prm_init;

	jvxNtask_sync prm_sync;
	
	jvxNtask_async prm_async;

	jvxHandle* prv;
} jvxNtask;

// ============================================================================

jvxDspBaseErrorType jvx_ntask_initConfig(jvxNtask* handle);

jvxDspBaseErrorType jvx_ntask_prepare(jvxNtask* handleOnReturn);

jvxDspBaseErrorType jvx_ntask_update(jvxNtask* handle, jvxUInt16 whatToUpdate, jvxCBool do_set);

jvxDspBaseErrorType jvx_ntask_process(jvxNtask* handle, jvxHandle** inputs, jvxHandle** outputs);

jvxDspBaseErrorType jvx_ntask_postprocess(jvxNtask* handleOnReturn);


#endif
