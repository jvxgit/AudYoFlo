#ifndef __JVX_MATRIX_ROT_H__
#define __JVX_MATRIX_ROT_H__

#include "jvx_dsp_base.h"
#include "jvx_matrix/jvx_matrix.h"

#include <math.h>

JVX_DSP_LIB_BEGIN



jvxErrorType jvx_matrix_process_rotmatrix_xyz_real(jvx_matrix* mat, jvxData* rotxyz_deg);
jvxErrorType jvx_matrix_process_rotmatrix_xyz_vec_real(jvx_matrix* mat, jvxData* xyz_in, jvxData* out);

JVX_DSP_LIB_END

#endif