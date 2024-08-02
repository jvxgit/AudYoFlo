#ifndef __JVX_QUAT_H__
#define __JVX_QUAT_H__

#include "jvx_dsp_base.h"
#include "jvx_matrix/jvx_matrix.h"

JVX_DSP_LIB_BEGIN

struct jvx_quat
{
	jvxData x;
	jvxData y;
	jvxData z;
	jvxData w;
};

void jvx_quat_mult(const struct jvx_quat* qxyzw1, const struct jvx_quat* qxyzw2, struct jvx_quat* qxyzw3);
void jvx_quat_inv(const struct jvx_quat* qin, struct jvx_quat* qout);
jvxData jvx_quat_abs2(const struct jvx_quat* q);
void jvx_quat_reset(struct jvx_quat* q);

JVX_DSP_LIB_END

#endif