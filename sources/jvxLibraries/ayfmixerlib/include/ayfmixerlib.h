#ifndef __AYFMIXERLIB_H__
#define __AYFMIXERLIB_H__

#include "jvx_dsp_base.h"
#include "ayfmixerlib_debug.h"

JVX_DSP_LIB_BEGIN

struct ayf_mixer_init
{
	jvxSize bsize;
};

struct ayf_mixer_async
{
	jvxData volume;

	// Orc arguments only useful if USE_ORC. Hoiwever we do not ifdef these variables as we 
	// should not ifdef in header files to prevent undesired seg faults!!!
	jvxCBool runorc;
	const char* orcTokenBackend_ip;
	const char* orcTokenBackend_op;
	const char* orcTokenDebug;
};

struct ayf_mixer
{
	struct ayf_mixer_async prmAsync;
	struct ayf_mixer_init prmInit;

	struct
	{
		struct ayf_mixer_data_debug* dbgHandler;
	} develop;
	jvxHandle* prv;
};

jvxDspBaseErrorType ayf_mixer_init(struct ayf_mixer* hdl, jvxSize bsize);

jvxDspBaseErrorType ayf_mixer_prepare(struct ayf_mixer* hdl);
jvxDspBaseErrorType ayf_mixer_process(struct ayf_mixer* hdl, jvxData** inputs, jvxData** outputs, jvxSize numChannelsIn, jvxSize numChannelsOut, jvxSize bufferSize);
jvxDspBaseErrorType ayf_mixer_postprocess(struct ayf_mixer* hdl);
jvxDspBaseErrorType ayf_mixer_update(struct ayf_mixer* hdl, jvxUInt16 operation, jvxCBool wantToSet);

JVX_DSP_LIB_END

#endif