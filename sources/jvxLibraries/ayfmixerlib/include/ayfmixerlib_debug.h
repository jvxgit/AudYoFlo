#ifndef __JVX_MIXER_DBG_H__
#define __JVX_MIXER_DBG_H__

#include "jvx_profiler/jvx_profiler_data_entry.h"

JVX_DSP_LIB_BEGIN

struct ayf_mixer;

typedef enum
{
	JVX_MIXER_DATA_DEBUG_TP0_SHIFT,
}jvxMixerDataDebugSpec;

struct ayf_mixer_data_debug
{
	jvxCBitField specData;

	struct jvx_profiler_data_entry TP0; 
};

jvxDspBaseErrorType ayf_mixer_data_debug_prepare(
	struct ayf_mixer_data_debug* hdlDbg,
	struct ayf_mixer* hdlStarter, jvxCBitField spec,
	jvx_register_entry_profiling_data_c cb, jvxHandle* inst);

jvxDspBaseErrorType ayf_mixer_data_debug_postprocess(
	struct ayf_mixer_data_debug* hdlDbg,
	jvx_unregister_entry_profiling_data_c cb, jvxHandle* inst);

JVX_DSP_LIB_END

#endif