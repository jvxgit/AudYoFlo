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

// If we combine two rotations, rot1 first and rot2 afterwards, we need to consider the order in 
// jvx_quat_mult_core: q1 -> q2 can be expressed on rotation matrices as 
// R2 * R1 * vec. Therefore to combine rot1 and rot2 in the right order, we need to compute q2 * q1!!
// The function <jvx_combine_rot> allows to more intuitively describe that in a function call than <jvx_quat_mult_core>!!

void jvx_quat_combine_rot(const struct jvx_quat* rot_first, const struct jvx_quat* rot_second, struct jvx_quat* rot_combined);
void jvx_quat_mult_core(const struct jvx_quat* qxyzw1, const struct jvx_quat* qxyzw2, struct jvx_quat* qxyzw3);

void jvx_quat_inv(const struct jvx_quat* qin, struct jvx_quat* qout);
jvxData jvx_quat_abs2(const struct jvx_quat* q);
void jvx_quat_reset(struct jvx_quat* q);

jvxData jvx_quat_delta_deg(struct jvx_quat* q1, struct jvx_quat* q2, jvx_matrix* workMat);

JVX_DSP_LIB_END

#endif

