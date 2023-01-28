#ifndef __JVX_LIMITER_H__
#define __JVX_LIMITER_H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

typedef enum
{
	JVX_LIMITER_TYPE_NONE,
  JVX_LIMITER_TYPE_SEGMENTS,
  JVX_LIMITER_TYPE_SOFT_X2,
  JVX_LIMITER_TYPE_SOFT_X3_03,
  JVX_LIMITER_TYPE_SOFT_X3_04,
  JVX_LIMITER_TYPE_SOFT_X3_05,
  JVX_LIMITER_TYPE_SOFT_X3_06,
  JVX_LIMITER_TYPE_SOFT_X3_07,
  JVX_LIMITER_TYPE_SOFT_X3_08,
  JVX_LIMITER_TYPE_SOFT_X3_09
} jvx_limiter_types;

// helper structs
/*
typedef struct
{
	jvxInt16 dummy;
} jvx_limiter_config;
*/

typedef struct
{
  jvx_limiter_types type;
  jvxData x0;
  jvxData delta;
  jvxData limVal;
} jvx_limiter_async;

/*
typedef struct
{
} jvx_limiter_sync;

typedef struct
{
} jvx_limiter_debug;
*/

// main struct for interfacing with this module
typedef struct
{
  jvxHandle *intern;
  //rtp_limiter_config config;
  jvx_limiter_async async;
  //rtp_limiter_sync sync;
  //rtp_limiter_debug debug;
} jvx_limiter;

// fill an rtp_limiter struct with default values
static jvxDspBaseErrorType jvx_limiter_default_config(jvx_limiter *hdl)
{
  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  hdl->async.type = JVX_LIMITER_TYPE_NONE;
  hdl->async.x0 = 1.0;
  hdl->async.delta = 1.0;

  return JVX_DSP_NO_ERROR;
}

// public interface functions
jvxDspBaseErrorType jvx_limiter_init(jvx_limiter *hdl);
jvxDspBaseErrorType jvx_limiter_terminate(jvx_limiter *hdl);
jvxDspBaseErrorType jvx_limiter_process(jvx_limiter *hdl,
                                               jvxData* input,
                                               jvxData* output,
                                               jvxSize framesize);

jvxDspBaseErrorType jvx_limiter_process_inplace(jvx_limiter *hdl,
	jvxData* inout,
	jvxSize framesize);
/*
jvxDspBaseErrorType rtp_limiter_update_sync(jvx_limiter *hdl,
                                                   jvxCBool do_set);
												   */
jvxDspBaseErrorType jvx_limiter_update_async(jvx_limiter *hdl,
                                                    jvxCBool do_set);

JVX_DSP_LIB_END

#endif
