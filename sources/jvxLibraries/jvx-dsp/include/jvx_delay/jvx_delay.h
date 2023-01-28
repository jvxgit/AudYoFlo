#ifndef _JVX_DELAY_H__
#define _JVX_DELAY_H__

#include "jvx_dsp_base.h"

typedef struct
{
	jvxSize numChannels;
	jvxSize delay_max;
	jvxSize bSize;
} jvx_delay_prmInit;

typedef struct
{
	jvxSize delay_current;
} jvx_delay_prmSync;

typedef struct
{
	jvx_delay_prmInit prmInit;
	jvx_delay_prmSync prmSync;

	jvxHandle* prv;
} jvx_delay;

// =============================================================================================

jvxDspBaseErrorType
jvx_delay_initCfg(jvx_delay* hdl);

jvxDspBaseErrorType
jvx_delay_prepare(jvx_delay* hdl);

jvxDspBaseErrorType
jvx_delay_process(jvx_delay* hdl, jvxData** in, jvxData** out);

jvxDspBaseErrorType
jvx_delay_postprocess(jvx_delay* hdl);

#endif