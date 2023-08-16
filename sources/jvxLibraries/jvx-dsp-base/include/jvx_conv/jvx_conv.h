#ifndef __JVX_CONV_H__
#define __JVX_CONV_H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

jvxData 
jvx_compute_conv_energy(jvxData* in1, jvxSize nin1, jvxData* in2, jvxSize nin2);

void
jvx_compute_conv(jvxData* in1, jvxSize nin1, jvxData* in2, jvxSize nin2, jvxData* out, jvxSize nout);

JVX_DSP_LIB_END

#endif