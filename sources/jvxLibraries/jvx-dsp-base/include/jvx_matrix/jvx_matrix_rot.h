#ifndef __JVX_MATRIX_ROT_H__
#define __JVX_MATRIX_ROT_H__

#include "jvx_dsp_base.h"
#include "jvx_matrix/jvx_matrix.h"

#include <math.h>

JVX_DSP_LIB_BEGIN

// This computes a rotation matrix Rx * Ry * Rz. That is, it runs rotation 
// along z-axis first, then along y-axis and finally along x-axis

/* Test totation
* 
jvxData tmp1[3] = { 0 };
jvxData tmp2[3] = { 0 };
jvxData rrp_rho_delta[3] = { 0,0,0 };

tmp1[0] = 1;
tmp1[1] = 0;
tmp1[2] = 0;
rrp_rho_delta[0] = 90;
rrp_rho_delta[1] = 90;
rrp_rho_delta[2] = 90;

// z: [1 0 0] -> [0 1 0] -- y: [0 1 0] -> [0 1 0] -- x: [0 1 0] -> [0 0 1]
jvx_matrix_process_rotmatrix_xyz_real(&rotMat, rrp_rho_delta);

jvx_matrix_process_rotmatrix_xyz_vec_real(&rotMat, tmp1, tmp2);
*/
enum jvxRotMatEulerConversionType
{
	JVX_EULER_CONVERT_XYZ,
	JVX_EULER_CONVERT_ZYX
};

jvxErrorType jvx_matrix_process_rotmatrix_xyz_real(jvx_matrix* mat, jvxData* rotxyz_deg);

jvxErrorType jvx_matrix_process_rotmatrix_xyz_vec_real(jvx_matrix* mat, jvxData* xyz_in, jvxData* out);

jvxErrorType jvx_matrix_process_rotmatrix_2_quat(jvx_matrix* rotmat, jvxData* qOut);

jvxErrorType jvx_matrix_process_quat_2rotmatrix(jvxData* q, jvx_matrix* rMOut);

jvxErrorType jvx_matrix_process_quat_2_euler_deg( const jvxData* qIn, jvxData* euler3Out,
	jvxCBool extrinsic, jvxSize ii, jvxSize jj, jvxSize kk, jvxCBool outputDegree, jvxCBool* gymLock);

jvxErrorType jvx_matrix_process_rotmat_2_euler_deg(jvx_matrix* rMIn, jvxData* out_euler_0, jvxData* out_euler_1, enum jvxRotMatEulerConversionType tp);


JVX_DSP_LIB_END

#endif