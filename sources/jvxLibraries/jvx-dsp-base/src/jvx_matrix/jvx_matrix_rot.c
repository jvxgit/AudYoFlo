#include "jvx_dsp_base.h"
#include "jvx_matrix/jvx_matrix.h"
#include "jvx_matrix/jvx_matrix_prv.h"

jvxErrorType jvx_matrix_process_rotmatrix_xyz_real(jvx_matrix* mat, jvxData* rotxyz_deg)
{
	jvxData tmp1, tmp2, tmp3;
	jvxSize i;
	jvx_matrix_prv* hdl = (jvx_matrix_prv*)mat->prv;

	if (hdl)
	{
		if ((mat->prmInit.M == 3) && (mat->prmInit.N == 3))
		{

			jvxData rotx_deg = rotxyz_deg[0];
			jvxData roty_deg = rotxyz_deg[1];
			jvxData rotz_deg = rotxyz_deg[2];

			jvxData rotx_rad = rotx_deg / 360 * 2 * M_PI;
			jvxData roty_rad = roty_deg / 360 * 2 * M_PI;
			jvxData rotz_rad = rotz_deg / 360 * 2 * M_PI;


			jvxData cos_rotx = cos(rotx_rad);
			jvxData sin_rotx = sin(rotx_rad);

			jvxData cos_roty = cos(roty_rad);
			jvxData sin_roty = sin(roty_rad);

			jvxData cos_rotz = cos(rotz_rad);
			jvxData sin_rotz = sin(rotz_rad);

			jvxData** R = (jvxData**)mat->prmSync.theMat;

			// Matrix rotx filled
			R[0][0] = 1;
			R[0][1] = 0;
			R[0][2] = 0;
			R[1][0] = 0;
			R[1][1] = cos_rotx;
			R[1][2] = -sin_rotx;
			R[2][0] = 0;
			R[2][1] = sin_rotx;
			R[2][2] = cos_rotx;


			// Involve matrix roty
			for (i = 0; i < 3; i++)
			{
				tmp1 = R[i][0];
				tmp2 = R[i][1];
				tmp3 = R[i][2];

				R[i][0] = tmp1 * cos_roty + tmp2 * 0 - tmp3 * sin_roty;
				R[i][1] = tmp1 * 0 + tmp2 * 1 + tmp3 * 0;
				R[i][2] = tmp1 * sin_roty + tmp2 * 0 + tmp3 * cos_roty;
			}


			// Involve matrix roty
			for (i = 0; i < 3; i++)
			{
				tmp1 = R[i][0];
				tmp2 = R[i][1];
				tmp3 = R[i][2];

				R[i][0] = tmp1 * cos_rotz + tmp2 * sin_rotz + tmp3 * 0;
				R[i][1] = -tmp1 * sin_rotz + tmp2 * cos_rotz + tmp3 * 0;
				R[i][2] = tmp1 * 0 + tmp2 * 0 + tmp3;
			}

			/*
			tmp1 += R[1] * cos_roty + R[4] * 0 - R[6] * sin_roty;
			tmp2 += R[0] * 0 + R[3] * 1 + R[6] * 0;
			tmp3 += R[0] * sin_roty + R[3] * 0 + R[6] * cos_roty;
			R[0] = tmp1;
			R[3] = tmp2;
			R[6] = tmp3;
			*/
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_SETTING;
	}
	return JVX_ERROR_NOT_READY;
}

jvxErrorType jvx_matrix_process_rotmatrix_xyz_vec_real(jvx_matrix* mat, jvxData* xyz_in, jvxData* out)
{

	jvxData tmp1, tmp2, tmp3;
	jvx_matrix_prv* hdl = (jvx_matrix_prv*)mat->prv;
	
	jvxSize i;

	if (hdl)
	{
		if ((mat->prmInit.M == 3) && (mat->prmInit.N == 3))
		{
			jvxData** R = (jvxData**)hdl->sync.theMat;
			for (i = 0; i < 3; i++)
			{
				out[i] = 0;
				tmp1 = R[i][0];
				tmp2 = R[i][1];
				tmp3 = R[i][2];

				out[i] = tmp1 * xyz_in[0] + tmp2 * xyz_in[1] + tmp3 * xyz_in[2];
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_SETTING;
	}
	return JVX_ERROR_NOT_READY;

}
