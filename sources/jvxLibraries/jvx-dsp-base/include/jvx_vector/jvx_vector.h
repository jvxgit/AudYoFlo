#ifndef __JVX_VECTOR_H__
#define __JVX_VECTOR_H__

#include "jvx_dsp_base.h"

#include <math.h>

JVX_DSP_LIB_BEGIN

void jvx_vector_cross_product(jvxData* in1, jvxData* in2, jvxData* out, jvxSize n);
void jvx_vector_mat_comp(jvxData* in, jvxData* out, jvxData** mat, jvxSize n, jvxCBool transpMode);

JVX_DSP_LIB_END

#endif

