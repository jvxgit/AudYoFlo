#ifndef __AYFSTARTERLIB_H__
#define __AYFSTARTERLIB_H__

#include "jvx_dsp_base.h"
#include "ayfstarterlib_debug.h"

JVX_DSP_LIB_BEGIN

struct ayf_starter_init
{
	jvxSize bsize;
};

struct ayf_starter_async
{
	jvxData volume;

	// Orc arguments only useful if USE_ORC. Hoiwever we do not ifdef these variables as we 
	// should not ifdef in header files to prevent undesired seg faults!!!
	jvxCBool runorc;
	const char* orcTokenBackend_ip;
	const char* orcTokenBackend_op;
	const char* orcTokenDebug;
};

struct ayf_starter
{
	struct ayf_starter_async prmAsync;
	struct ayf_starter_init prmInit;

	struct
	{
		struct ayf_starter_data_debug* dbgHandler;
	} develop;
	jvxHandle* prv;
};

jvxDspBaseErrorType ayf_starter_init(struct ayf_starter* hdl, jvxSize bsize);

jvxDspBaseErrorType ayf_starter_prepare(struct ayf_starter* hdl);
jvxDspBaseErrorType ayf_starter_process(struct ayf_starter* hdl, jvxData** inputs, jvxData** outputs, jvxSize numChannelsIn, jvxSize numChannelsOut, jvxSize bufferSize);
jvxDspBaseErrorType ayf_starter_postprocess(struct ayf_starter* hdl);
jvxDspBaseErrorType ayf_starter_update(struct ayf_starter* hdl, jvxUInt16 operation, jvxCBool wantToSet);

JVX_DSP_LIB_END

#endif