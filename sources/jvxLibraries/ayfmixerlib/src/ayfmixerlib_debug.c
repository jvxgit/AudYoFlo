#include "ayfmixerlib_debug.h"
#include "ayfmixerlib.h"

// =======================================================================================================


jvxDspBaseErrorType ayf_mixer_data_debug_prepare(
	struct ayf_mixer_data_debug* hdlDbg, struct ayf_mixer* hdlMixer, jvxCBitField spec, 
	jvx_register_entry_profiling_data_c cb, jvxHandle* inst)
{
	jvxDspBaseErrorType res = JVX_NO_ERROR;

#if JVX_ALLOW_PROFILER_DATA_MACROS
	hdlDbg->specData = spec;

	// All pointers are initialized to 0 from outside (memset(0))	
	if(hdlDbg->specData & ((jvxCBitField)1 << JVX_MIXER_DATA_DEBUG_TP0_SHIFT))
	{
		jvx_profiler_allocate_single_entry(&hdlDbg->TP0, hdlMixer->prmInit.bsize, false);
		if(cb) cb(&hdlDbg->TP0, "mixer_tp0_c", inst);
	}
#endif
	return res;
}

jvxDspBaseErrorType ayf_mixer_data_debug_postprocess(struct ayf_mixer_data_debug* hdl,
	jvx_unregister_entry_profiling_data_c cb, jvxHandle* inst)
{
#if JVX_ALLOW_PROFILER_DATA_MACROS
	if (cb) cb("mixer_tp0_c", inst);
	jvx_profiler_deallocate_single_entry(&hdl->TP0);
#endif

	return JVX_NO_ERROR;
}
