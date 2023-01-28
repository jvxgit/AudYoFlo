#ifndef __JVX_ESTIMATE_BUFFER_FILLHEIGHT_H__
#define __JVX_ESTIMATE_BUFFER_FILLHEIGHT_H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

jvxDspBaseErrorType jvx_estimate_buffer_fillheight_init(jvxHandle** hdlOnReturn, 
															   jvxInt32 numberEventsConsidered_perMinMaxSection, 
															   jvxInt32 num_MinMaxSections, 
															   jvxData recSmoothFactor,
															   jvxInt32 numOperations);
jvxDspBaseErrorType jvx_estimate_buffer_fillheight_restart(jvxHandle* hdlOnEnter);
jvxDspBaseErrorType jvx_estimate_buffer_fillheight_terminate(jvxHandle* hdlOnEnter);
jvxDspBaseErrorType jvx_estimate_buffer_fillheight_process(jvxHandle* hdlOnEnter, jvxData in_value, 
																  jvxData in_weight,
																  jvxSize in_operate_id,
																  jvxData* out_average, 
																  jvxData* out_min, 
																  jvxData* out_max,
																  jvxData* out_operate_average,
																  jvxData* out_operate_min,
																  jvxData* out_operate_max);

// ================================================================================
jvxDspBaseErrorType jvx_estimate_buffer_fillheight_process_time_int(jvxHandle* hdlOnEnter, jvxSize fHeight, jvxSize in_operate_id);
jvxDspBaseErrorType jvx_estimate_buffer_fillheight_process_time_ext(jvxHandle* hdlOnEnter, jvxSize fHeight, jvxSize in_operate_id, jvxUInt64 tStampStart);

jvxDspBaseErrorType jvx_estimate_buffer_fillheight_read(jvxHandle* hdlOnEnter, 
	jvxSize in_operate_id,
	jvxData* fHeight,
	jvxData* out_min, jvxData* out_max, 
	jvxData* out_operate_average, 
	jvxData* out_operate_min,
	jvxData* out_operate_max);

JVX_DSP_LIB_END

#endif