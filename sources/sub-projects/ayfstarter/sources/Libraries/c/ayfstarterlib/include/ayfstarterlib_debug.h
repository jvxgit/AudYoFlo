#ifndef __JVX_STARTER_DBG_H__
#define __JVX_STARTER_DBG_H__

#include "jvx_profiler/jvx_profiler_data_entry.h"

JVX_DSP_LIB_BEGIN

struct ayf_starter;

typedef enum
{
	JVX_STARTER_DATA_DEBUG_TP0_SHIFT,
}jvxStarterDataDebugSpec;

struct ayf_starter_data_debug
{
	jvxCBitField specData;

	struct jvx_profiler_data_entry TP0; 
};

jvxDspBaseErrorType ayf_starter_data_debug_prepare(
	struct ayf_starter_data_debug* hdlDbg,
	struct ayf_starter* hdlStarter, jvxCBitField spec,
	jvx_register_entry_profiling_data_c cb, jvxHandle* inst);

jvxDspBaseErrorType ayf_starter_data_debug_postprocess(
	struct ayf_starter_data_debug* hdlDbg,
	jvx_unregister_entry_profiling_data_c cb, jvxHandle* inst);

JVX_DSP_LIB_END

#endif