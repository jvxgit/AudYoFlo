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




// The rotmat should be of size 3x3, the output qOut should be a field with 4 elements.
// The output produced q = [qx qy qz qw]
jvxErrorType
jvx_matrix_process_rotmatrix_2_quat(jvx_matrix* rotmat, jvxData* qOut)
{
	//  This code outputs [qx qy qz qw]

	// Source for realization:
	// https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

	// Source for verification : https://www.andre-gaschler.com/rotationconverter/    

	assert(rotmat->prmInit.N == 3);
	assert(rotmat->prmInit.M == 3);
	assert(rotmat->prmSync.theMat);

	jvxData S, qw, qx, qy, qz;

	jvxData** rM = (jvxData**)rotmat->prmSync.theMat;
	jvxData m00 = rM[0][0]; // (1, 1);
	jvxData m01 = rM[0][1]; // (1, 2);
	jvxData m02 = rM[0][2]; //(1, 3);

	jvxData m10 = rM[1][0]; //(2, 1);
	jvxData m11 = rM[1][1]; //(2, 2);
	jvxData m12 = rM[1][2]; //(2, 3);

	jvxData m20 = rM[2][0]; //(3, 1);
	jvxData m21 = rM[2][1]; //(3, 2);
	jvxData m22 = rM[2][2]; //(3, 3);

	jvxData tr = m00 + m11 + m22;

	if (tr > 0)
	{
		S = sqrt(tr + 1.0) * 2;
		qw = 0.25 * S;
		qx = (m21 - m12) / S;
		qy = (m02 - m20) / S;
		qz = (m10 - m01) / S;
	}
	else if ((m00 > m11) && (m00 > m22))
	{
		S = sqrt(1.0 + m00 - m11 - m22) * 2;// S = 4 * qx
		qw = (m21 - m12) / S;
		qx = 0.25 * S;
		qy = (m01 + m10) / S;
		qz = (m02 + m20) / S;
	}
	else if (m11 > m22)
	{
		S = sqrt(1.0 + m11 - m00 - m22) * 2;// S = 4 * qy
		qw = (m02 - m20) / S;
		qx = (m01 + m10) / S;
		qy = 0.25 * S;
		qz = (m12 + m21) / S;
	}
	else
	{
		S = sqrt(1.0 + m22 - m00 - m11) * 2;// S = 4 * qz
		qw = (m10 - m01) / S;
		qx = (m02 + m20) / S;
		qy = (m12 + m21) / S;
		qz = 0.25 * S;
	}

	qOut[0] = qx;
	qOut[1] = qy;
	qOut[2] = qz;
	qOut[3] = qw;
	return JVX_NO_ERROR;
}

jvxErrorType
jvx_matrix_process_quat_2rotmatrix(jvxData* qIn, jvx_matrix* rMOut)
{
	// This function reads q = [qx, qy, qz, qw];

	//  Taken from here :
	// https ://automaticaddison.com/how-to-convert-a-quaternion-to-a-rotation-matrix/

	// Source for verification : https ://www.andre-gaschler.com/rotationconverter/    
	assert(rMOut->prmInit.M == 3);
	assert(rMOut->prmInit.N == 3);
	assert(rMOut->prmSync.theMat);
	jvxData** rM = (jvxData **)rMOut->prmSync.theMat;

	jvxData qx = qIn[0];//(1);
	jvxData qy = qIn[1];//(2);
	jvxData qz = qIn[2];//(3);
	jvxData qw = qIn[3];//(4);

	rM[0][0] = 2 * (qw * qw + qx * qx) - 1;
	rM[1][0] = 2 * (qx * qy + qw * qz);
	rM[2][0] = 2 * (qx * qz - qw * qy);
	//rM(1, 1) = 2 * (qw ^ 2 + qx ^ 2) - 1;
	//rM(2, 1) = 2 * (qx * qy + qw * qz);
	//rM(3, 1) = 2 * (qx * qz - qw * qy);

	rM[0][1] = 2 * (qx * qy - qw * qz);
	rM[1][1] = 2 * (qw * qw + qy * qy) - 1;
	rM[2][1] = 2 * (qy * qz + qw * qx);
	//rM(1, 2) = 2 * (qx * qy - qw * qz);
	//rM(2, 2) = 2 * (qw ^ 2 + qy ^ 2) - 1;
	//rM(3, 2) = 2 * (qy * qz + qw * qx);

	rM[0][2] = 2 * (qx * qz + qw * qy);
	rM[1][2] = 2 * (qy * qz - qw * qx);
	rM[2][2] = 2 * (qw * qw + qz * qz) - 1;
	//rM(1, 3) = 2 * (qx * qz + qw * qy);
	//rM(2, 3) = 2 * (qy * qz - qw * qx);
	//rM(3, 3) = 2 * (qw ^ 2 + qz ^ 2) - 1;
	return JVX_NO_ERROR;
}

jvxErrorType jvx_matrix_process_quat_2_euler(
	const jvxData* qIn, jvxData* euler3Out,
	jvxCBool extrinsic, jvxSize ii, jvxSize jj, jvxSize kk,
	jvxCBool outputDegree, jvxCBool* gymLock)
{
	if (gymLock)
	{
		*gymLock = false;
	}

	jvxCBool symmetric = c_false;
	if (
		(ii == kk) || (ii == jj) || (jj == kk))
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	jvxSize angle_first = 0;
	jvxSize angle_third = 2;

	if (!extrinsic)
	{
		angle_first = 2;
		angle_third = 0;
		int help = ii;
		ii = kk;
		kk = help;
	}

	int sign = ((int)ii - (int)jj) * ((int)jj - (int)kk) * ((int)kk - (int)ii) / 2;
	memset(euler3Out, 0, sizeof(jvxData) * 3);

	// _empty2(num_rotations, 3)

	jvxData a = 0, b = 0, c = 0, d = 0;
	jvxData half_sum = 0, half_diff = 0;
	jvxData eps = 1e-7;
	int theCase = 0;
	jvxSize idx = 0;

	// Step 1
	// Permutate quaternion elements
	if (symmetric)
	{
		a = qIn[3];
		b = qIn[ii];
		c = qIn[jj];
		d = qIn[kk] * sign;
	}
	else
	{
		a = qIn[3] - qIn[jj];
		b = qIn[ii] + qIn[kk] * sign;
		c = qIn[jj] + qIn[3];
		d = qIn[kk] * sign - qIn[ii];
	}

	// Step 2
	// Compute second angle...
	euler3Out[1] = 2 * atan2(hypot(c, d), hypot(a, b));

	// ... and check if equal to is 0 or pi, causing a singularity
	if (abs(euler3Out[1]) <= eps)
	{
		theCase = 1;
	}
	else if (abs(euler3Out[1] - M_PI) <= eps)
	{
		theCase = 2;
	}
	else
	{
		theCase = 0;
	}

	// Step 3
	// compute first and third angles, according to case
	half_sum = atan2(b, a);
	half_diff = atan2(d, c);

	if (theCase == 0)
	{
		euler3Out[angle_first] = half_sum - half_diff;
		euler3Out[angle_third] = half_sum + half_diff;
	}
	else
	{
		// any degenerate case
		euler3Out[2] = 0;
		if (theCase == 1)
		{
			euler3Out[0] = 2 * half_sum;
		}
		else
		{
			if (extrinsic)
			{
				euler3Out[0] = -2 * half_diff;
			}
			else
			{
				euler3Out[0] = 2 * half_diff;
			}
		}
	}

	// for Tait - Bryan angles
	if (symmetric == c_false)
	{
		euler3Out[angle_third] *= sign;
		euler3Out[1] -= M_PI / 2;
	}

	for (idx = 0; idx < 3; idx++)
	{
		if (euler3Out[idx] < -M_PI)
		{
			euler3Out[idx] += 2 * M_PI;
		}
		else if (euler3Out[idx] > M_PI)
		{
			euler3Out[idx] -= 2 * M_PI;
		}
	}

	if (theCase != 0)
	{
		/*
		printf("Gimbal lock detected. Setting third angle to zero "
			   "since it is not possible to uniquely determine "
			   "all angles.\n");
		*/
		if (gymLock)
		{
			*gymLock = true;
		}
	}
	if (outputDegree)
	{
		for (idx = 0; idx < 3; idx++)
		{
			euler3Out[idx] *= 180 / M_PI;
		}
	}

	return JVX_NO_ERROR;
}