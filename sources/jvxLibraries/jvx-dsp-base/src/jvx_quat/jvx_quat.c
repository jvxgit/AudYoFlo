#include "jvx_dsp_base.h"
#include "jvx_quat/jvx_quat.h"

void jvx_quat_mult_core(const struct jvx_quat* qxyzw1, const struct jvx_quat* qxyzw2, struct jvx_quat* qxyzw3)
{
	jvxData x1 = qxyzw1->x;
	jvxData x2 = qxyzw1->y;
	jvxData x3 = qxyzw1->z;
	jvxData x0 = qxyzw1->w;

	jvxData y1 = qxyzw2->x;
	jvxData y2 = qxyzw2->y;
	jvxData y3 = qxyzw2->z;
	jvxData y0 = qxyzw2->w;

	// https://de.wikipedia.org/wiki/Quaternion
	jvxData z0 = x0 * y0 - x1 * y1 - x2 * y2 - x3 * y3;
	jvxData z1 = x0 * y1 + x1 * y0 + x2 * y3 - x3 * y2;
	jvxData z2 = x0 * y2 - x1 * y3 + x2 * y0 + x3 * y1;
	jvxData z3 = x0 * y3 + x1 * y2 - x2 * y1 + x3 * y0;

	qxyzw3->x = z1;
	qxyzw3->y = z2;
	qxyzw3->z = z3;
	qxyzw3->w = z0;
}

void jvx_quat_combine_rot(const struct jvx_quat* rot_first, const struct jvx_quat* rot_second, struct jvx_quat* rot_combined)
{
	jvx_quat_mult_core(rot_second, rot_first, rot_combined);
}

void jvx_quat_inv(const struct jvx_quat* qin, struct jvx_quat* qout)
{
	jvxData q1 = qin->x;
	jvxData q2 = qin->y;
	jvxData q3 = qin->z;
	jvxData q0 = qin->w;

	jvxData divisor = q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3;
	jvxData fac = 1 / divisor;

	qout->x = -q1 * fac;
	qout->y = -q2 * fac;
	qout->z = -q3 * fac;
	qout->w = q0 * fac;
}

jvxData jvx_quat_abs2(const struct jvx_quat* q)
{
	jvxData absVal = 0;
	absVal += q->x * q->x;
	absVal += q->y * q->y;
	absVal += q->z * q->z;
	absVal += q->w * q->w;
	return absVal;
}

void jvx_quat_reset(struct jvx_quat* q)
{
	q->w = 1.0;
	q->x = 0.0;
	q->y = 0.0;
	q->z = 0.0;
}

