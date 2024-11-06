#ifndef __JVX_CONV_H__
#define __JVX_CONV_H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

jvxData
jvx_compute_energy(jvxData* inF, jvxSize ninF);

jvxData 
jvx_compute_conv_energy(jvxData* in1, jvxSize nin1, jvxData* in2, jvxSize nin2);

jvxData
jvx_compute_conv_energy_add(jvxData* inF1, jvxData* inF2, jvxSize ninF, jvxData* inS1, jvxData* inS2, jvxSize ninS);

void
jvx_compute_conv(jvxData* in1, jvxSize nin1, jvxData* in2, jvxSize nin2, jvxData* out, jvxSize nout);

void
jvx_compute_conv_constrained_size(jvxData* inF, jvxSize ninF, jvxData* inS, jvxSize ninS, jvxData* out, jvxSize nout);

void
jvx_compute_conv_add(jvxData* inF1, jvxData* inF2, jvxSize ninF, jvxData* inS1, jvxData* inS2, jvxSize ninS, jvxData* out, jvxSize nout);

JVX_DSP_LIB_END

#endif