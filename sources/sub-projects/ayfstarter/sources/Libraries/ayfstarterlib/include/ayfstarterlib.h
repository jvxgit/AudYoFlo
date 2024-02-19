#ifndef __AYFSTARTERLIB_H__
#define __AYFSTARTERLIB_H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

struct ayf_starter_async
{
	jvxData volume;
};

struct ayf_starter
{
	struct ayf_starter_async prmAsync;
	jvxHandle* prv;
};

jvxDspBaseErrorType ayf_starter_init(struct ayf_starter* hdl);

jvxDspBaseErrorType ayf_starter_prepare(struct ayf_starter* hdl);
jvxDspBaseErrorType ayf_starter_process(struct ayf_starter* hdl, jvxData** inputs, jvxData** outputs, jvxSize numChannelsIn, jvxSize numChannelsOut, jvxSize bufferSize);
jvxDspBaseErrorType ayf_starter_postprocess(struct ayf_starter* hdl);
jvxDspBaseErrorType ayf_starter_update(struct ayf_starter* hdl, jvxUInt16 operation, jvxCBool wantToSet);

JVX_DSP_LIB_END

#endif