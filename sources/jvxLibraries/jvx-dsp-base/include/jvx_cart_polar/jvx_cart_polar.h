#ifndef __JVX_CART_POLAR_H__
#define __JVX_CART_POLAR_H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

// Convert between cartesian and polar coordinates. Coord system:
// Angles in Azimuth and inclination [az, incl], frontal direction is [0, 90]
// Cartesian vector assumed to have unit absolute value
jvxErrorType jvx_polar_2_cartesion_deg(jvxData* in_azimIncl2, jvxData* out_xyz_vec3);
jvxErrorType jvx_cartesian_2_polar_deg(jvxData* in_xyz_vec3, jvxData* out_azimIncl2);

JVX_DSP_LIB_END

#endif
