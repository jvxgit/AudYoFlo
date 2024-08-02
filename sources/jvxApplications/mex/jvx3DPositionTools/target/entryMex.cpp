#include "jvx.h"

#include "jvx_matrix/jvx_matrix_rot.h"
#include "jvx_quat/jvx_quat.h"

#define USE_EIGEN

#ifdef USE_EIGEN
#include <Geometry>
#endif

#if _MATLAB_MEXVERSION < 500
#if (_MSC_VER >= 1600)
#include <yvals.h>
#define __STDC_UTF_16__
#endif
#endif
#include <mex.h>

/*
#pragma comment ( lib, "libmx")
#pragma comment ( lib, "libmex")
#pragma comment ( lib, "libeng")
#pragma comment ( lib, "libmat")*/
//#pragma comment ( lib, "libmatlb") 

//#define DLL_EXPORT __declspec(dllexport)

#include <stdio.h>
#include <string>
#include <vector>

#if _MATLAB_MEXVERSION < 500
#if (_MSC_VER >= 1600)
#include <yvals.h>
#define __STDC_UTF_16__
#endif
#endif

#if _MATLAB_MEXVERSION >= 100
#define SZ_MAT_TYPE mwSize
#else
#define SZ_MAT_TYPE int
#endif

#include "CjvxCToMatlabConverter.h"
#include "CjvxMatlabToCConverter.h"

#define CLIP_VALUE 0.99
#define SMOOTH_LEVELS 0.995



/**
 * C-style entry from MATLAB to MEX component.
 * All command arguments are passed in the optional arguments for
 * this mex function.
 *///=============================================================
void mexFunction(int nlhs, mxArray* plhs[],
	int nrhs, const mxArray* prhs[])
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	mxArray* arr = NULL;
	jvxData input_avrg = 0;
	jvxData input_max = 0;
	jvxData input_gain = 1;
	jvxCBool input_clip = c_false;
	jvxBool removeR = false;
	jvxData* R = nullptr;

	jvxData* E = nullptr;

	jvxSize i, j;

	if (nlhs >= 1)
	{
		std::string purpose;
		jvxErrorType res = CjvxMatlabToCConverter::mexArgument2String(purpose, prhs, 0, nrhs);
		if (res == JVX_NO_ERROR)
		{
			// =========================== CASE 1: QUAT2ROTMAT ======================================
			if (purpose == "quat2rotmat")
			{
				if (nrhs == 2)
				{
					if (mxIsData(prhs[1]))
					{
						jvxSize numN = mxGetN(prhs[1]);
						jvxSize numM = mxGetM(prhs[1]);

						if ((numN == 4) && (numM == 1))
						{
							struct jvx_quat* quat = (struct jvx_quat*)mxGetData(prhs[1]);
							jvx_matrix rotMat;

							jvx_matrix_initConfig(&rotMat);
							rotMat.prmInit.format = JVX_DATAFORMAT_DATA;
							rotMat.prmInit.N = 3;
							rotMat.prmInit.M = 3;
							rotMat.prmInit.lin_field_matrix = c_false;
							rotMat.prmInit.intern_alloc_cont = c_true;

							jvx_matrix_prepare(&rotMat);
							res = jvx_matrix_quat_2rotmatrix(quat, &rotMat);

							if (res != JVX_NO_ERROR)
							{
								std::string txt = "Call to C core function <jvx_matrix_process_quat_2rotmatrix> failed, error code: ";
								txt += jvxErrorType_descr(res);
								txt += ".";
								mexWarnMsgTxt(txt.c_str());
							}


							// Return the rot matrix
							dims[0] = (SZ_MAT_TYPE)3;
							dims[1] = (SZ_MAT_TYPE)3;
							arr = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
							R = (jvxData*)mxGetData(arr);
							for (i = 0; i < 3; i++)
							{
								for (j = 0; j < 3; j++)
								{
									R[j * 3 + i] = ((jvxData**)rotMat.prmSync.theMat)[i][j];
								}
							}
							plhs[0] = arr;

							jvx_matrix_postprocess(&rotMat);
						}
						else
						{
							mexWarnMsgTxt("Second input argument must be of dimension [1x4] to hold a quaternion array with [qx, qy, qz, qw].");
						}
					}
					else
					{
						mexWarnMsgTxt("Second input argument must be of type <jvxData>.");
					}
				} // if (nrhs == 2)
				else
				{
					std::string txt = "Option <" + purpose + "> requires a second input argument.";
					mexWarnMsgTxt(txt.c_str());
				}
			} // if(purpose == "quat2rotmat")
			
			// =========================== CASE 2: ROTMAT2QUAT ======================================
			else if (purpose == "rotmat2quat")
			{
				if (nrhs == 2)
				{
					if (mxIsData(prhs[1]))
					{
						jvxSize numN = mxGetN(prhs[1]);
						jvxSize numM = mxGetM(prhs[1]);

						if ((numN == 3) && (numM == 3))
						{
							struct jvx_quat* Q = nullptr;
							jvxData* rMat = (jvxData*)mxGetData(prhs[1]);
							
#ifdef USE_EIGEN

							Eigen::Matrix<jvxData, 3, 3> R;
							for (i = 0; i < 3; i++)
							{
								for (j = 0; j < 3; j++)
								{
									R(i, j) = rMat[j * 3 + i];
								}
							}
							
							// () = q.normalized().toRotationMatrix();
							Eigen::Quaternion<jvxData> q(R);
#else
							jvx_matrix rotMat;

							jvx_matrix_initConfig(&rotMat);
							rotMat.prmInit.format = JVX_DATAFORMAT_DATA;
							rotMat.prmInit.N = 3;
							rotMat.prmInit.M = 3;
							rotMat.prmInit.lin_field_matrix = c_false;
							rotMat.prmInit.intern_alloc_cont = c_true;

							jvx_matrix_prepare(&rotMat);

							// Copy input to matrix
							for (i = 0; i < 3; i++)
							{
								for (j = 0; j < 3; j++)
								{
									((jvxData**)rotMat.prmSync.theMat)[i][j] = rMat[j * 3 + i];
								}
							}
#endif
							// Return the rot matrix
							dims[0] = (SZ_MAT_TYPE)1;
							dims[1] = (SZ_MAT_TYPE)4;
							arr = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
							Q = (struct jvx_quat*)mxGetData(arr);

#ifdef USE_EIGEN
							Q->x = q.x();
							Q->y = q.y();
							Q->z = q.z();
							Q->w = q.w();
#else
							res = jvx_matrix_rotmatrix_2_quat(&rotMat, Q);
							jvx_matrix_postprocess(&rotMat);
#endif
							if (res != JVX_NO_ERROR)
							{
								std::string txt = "Call to C core function <jvx_matrix_process_rotmatrix_2_quat> failed, error code: ";
								txt += jvxErrorType_descr(res);
								txt += ".";
								mexWarnMsgTxt(txt.c_str());
							}

							plhs[0] = arr;							

						}
						else
						{
							mexWarnMsgTxt("Second input argument must be of dimension [3x3] to hold a rotation matrix [r00 r01 r02; r10 r11 r12;r31 r32 r33].");
						}
					}
					else
					{
						mexWarnMsgTxt("Second input argument must be of type <jvxData>.");
					}
				}
				
				
			} // else if(purpose == "rotmat2quat")

			// =========================== CASE 3: QUAT2EULER ======================================
			else if (purpose == "quat2euler")
			{
				if (nrhs >= 2)
				{
					std::string conversionTag = "xyz";
					if (nrhs == 3)
					{
						res = CjvxMatlabToCConverter::mexArgument2String(conversionTag, prhs, 2, nrhs);
					}
					if (mxIsData(prhs[1]))
					{
						jvxSize numN = mxGetN(prhs[1]);
						jvxSize numM = mxGetM(prhs[1]);

						if ((numN == 4) && (numM == 1))
						{
							struct jvx_quat* qVec = (struct jvx_quat*)mxGetData(prhs[1]);

							jvxCBool ext = c_true;
							jvxSize ii0 = 0;
							jvxSize jj0 = 1;
							jvxSize kk0 = 2;

							if (conversionTag == "zyx")
							{
								ext = c_true;
								ii0 = 0;
								jj0 = 1;
								kk0 = 2;
							}
							else if (conversionTag == "xyz")
							{
								ext = c_true;
								ii0 = 1;
								jj0 = 0;
								kk0 = 2;
							}
							else
							{
								std::string txt = "Second input argument must be a valid conversion tag - tag <" + conversionTag + "> is not a valid option.";
								mexWarnMsgTxt(txt.c_str());
							}
							jvxCBool gymLock = c_false;

							// Return the euler vector
							dims[0] = (SZ_MAT_TYPE)1;
							dims[1] = (SZ_MAT_TYPE)3;
							arr = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
							E = (jvxData*)mxGetData(arr);
							res = jvx_matrix_process_quat_2_euler_deg(qVec, E, ext, ii0, jj0, kk0, c_true, &gymLock);

							if (res != JVX_NO_ERROR)
							{
								std::string txt = "Call to C core function <jvx_matrix_process_quat_2_euler_deg> failed, error code: ";
								txt += jvxErrorType_descr(res);
								txt += ".";
								mexWarnMsgTxt(txt.c_str());
							}

							plhs[0] = arr;

							if (nlhs > 1)
							{
								dims[0] = (SZ_MAT_TYPE)1;
								dims[1] = (SZ_MAT_TYPE)1;
#ifdef JVX_CBOOL_IS_UINT16
								arr = mxCreateNumericArray(ndim, dims, mxINT16_CLASS, mxREAL);
								jvxCBool* GL = (jvxCBool*)mxGetData(arr);
								*GL = gymLock;
#else
								assert(0);
#endif
								plhs[0] = arr;

							}
						}
						else
						{
							mexWarnMsgTxt("Data input argument must be of dimension [1x4] to hold a quaternion ([qx, qy, qz, qw]).");
						}
					}
					else
					{
						mexWarnMsgTxt("Data input argument must be of type <jvxData>.");
					}
				} // if (nrhs >= 2)
				else
				{
					std::string txt = "Option <" + purpose + "> requires 2 or 3 input arguments.";
					mexWarnMsgTxt(txt.c_str());
				}
			}// else if (purpose == "quat2euler")

			// =========================== CASE 4: EULER2ROTMAT ======================================
			else if (purpose == "euler2rotmat")
			{
				if (nrhs >= 2)
				{						
					std::string conversionTag = "xyz";
					if (nrhs == 3)
					{
						res = CjvxMatlabToCConverter::mexArgument2String(conversionTag, prhs, 2, nrhs);
					}
					if (mxIsData(prhs[1]))
					{
						jvxSize numN = mxGetN(prhs[1]);
						jvxSize numM = mxGetM(prhs[1]);

						if ((numN == 3) && (numM == 1))
						{
							jvxData* rotxyz = (jvxData*)mxGetData(prhs[1]);
							jvxData tmp1, tmp2, tmp3;
							jvx_matrix rotMat;

							jvx_matrix_initConfig(&rotMat);
							rotMat.prmInit.format = JVX_DATAFORMAT_DATA;
							rotMat.prmInit.N = 3;
							rotMat.prmInit.M = 3;
							rotMat.prmInit.lin_field_matrix = c_false;
							rotMat.prmInit.intern_alloc_cont = c_true;

							jvx_matrix_prepare(&rotMat);
							if (conversionTag == "zyx")
							{
								res = jvx_matrix_euler_deg_2_rotmatrix_extrinsic(rotxyz, &rotMat, JVX_EULER_CONVERT_ZYX);
							}
							else
							{
								res = jvx_matrix_euler_deg_2_rotmatrix_extrinsic(rotxyz, &rotMat, JVX_EULER_CONVERT_XYZ);
							}

							if (res != JVX_NO_ERROR)
							{
								std::string txt = "Call to C core function <jvx_matrix_process_rotmatrix_xyz_real> failed, error code: ";
								txt += jvxErrorType_descr(res);
								txt += ".";
								mexWarnMsgTxt(txt.c_str());
							}

							// Return the rot matrix
							dims[0] = (SZ_MAT_TYPE)3;
							dims[1] = (SZ_MAT_TYPE)3;
							arr = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
							R = (jvxData*)mxGetData(arr);
							for (i = 0; i < 3; i++)
							{
								for (j = 0; j < 3; j++)
								{
									R[j * 3 + i] = ((jvxData**)rotMat.prmSync.theMat)[i][j];
								}
							}
							plhs[0] = arr;

							jvx_matrix_postprocess(&rotMat);
						}
						else
						{
							mexWarnMsgTxt("Data input argument must be of dimension [1x3] to hold euler angles ([rotx, roty, rotz]).");
						}
					}
					else
					{
						mexWarnMsgTxt("Data input argument must be of type <jvxData>.");
					}
				} // if (nrhs >= 2)
				else
				{
					std::string txt = "Option <" + purpose + "> requires 2 or 3 input arguments.";
					mexWarnMsgTxt(txt.c_str());
				}
			}// else if (purpose == "quat2euler")

			// =========================== CASE 5: ROTMAT2EULER ======================================
			else if (purpose == "rotmat2euler")
			{
				if (nrhs >= 2)
				{
					std::string conversionTag = "xyz";
					if (nrhs == 3)
					{
						res = CjvxMatlabToCConverter::mexArgument2String(conversionTag, prhs, 2, nrhs);
					}
					if (mxIsData(prhs[1]))
					{
						jvxSize numN = mxGetN(prhs[1]);
						jvxSize numM = mxGetM(prhs[1]);

						if ((numN == 3) && (numM == 3))
						{
							jvxData* mat = (jvxData*)mxGetData(prhs[1]);
							jvx_matrix rotMat;

							jvx_matrix_initConfig(&rotMat);
							rotMat.prmInit.format = JVX_DATAFORMAT_DATA;
							rotMat.prmInit.N = 3;
							rotMat.prmInit.M = 3;
							rotMat.prmInit.lin_field_matrix = c_false;
							rotMat.prmInit.intern_alloc_cont = c_true;

							jvx_matrix_prepare(&rotMat);

							for (i = 0; i < 3; i++)
							{
								for (j = 0; j < 3; j++)
								{
									((jvxData**)rotMat.prmSync.theMat)[i][j] = mat[j * 3 + i];
								}
							}

							jvxData* out_1 = nullptr;
							jvxData* out_2 = nullptr;

							dims[0] = (SZ_MAT_TYPE)1;
							dims[1] = (SZ_MAT_TYPE)3;
							arr = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
							out_1 = (jvxData*)mxGetData(arr);
							plhs[0] = arr;

							if (nlhs > 1)
							{
								arr = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
								out_2 = (jvxData*)mxGetData(arr);
								plhs[1] = arr;
							}

							res = jvx_matrix_rotmat_2_euler_deg_extrensic(&rotMat, out_1, out_2, JVX_EULER_CONVERT_XYZ);

							if (res != JVX_NO_ERROR)
							{
								std::string txt = "Call to C core function <jvx_matrix_process_rotmat_2_euler_deg> failed, error code: ";
								txt += jvxErrorType_descr(res);
								txt += ".";
								mexWarnMsgTxt(txt.c_str());
							}
							jvx_matrix_postprocess(&rotMat);
						}
						else
						{
							mexWarnMsgTxt("Data input argument must be of dimension [3x3] to hold hold a rotation matrix [r00 r01 r02; r10 r11 r12;r31 r32 r33].");
						}
					}
					else
					{
						mexWarnMsgTxt("Data input argument must be of type <jvxData>.");
					}
				} // if (nrhs >= 2)
				else
				{
					std::string txt = "Option <" + purpose + "> requires 2 or 3 input arguments.";
					mexWarnMsgTxt(txt.c_str());
				}
			}
			else if (purpose == "quatmult")
			{
				if (nrhs == 3)
				{
					if (mxIsData(prhs[1]) && mxIsData(prhs[2]))
					{
						jvxSize numN1 = mxGetN(prhs[1]);
						jvxSize numM1 = mxGetM(prhs[1]);
						jvxSize numN2 = mxGetN(prhs[2]);
						jvxSize numM2 = mxGetM(prhs[2]);

						if ((numN1 == 4) && (numM1 == 1) && (numN2 == 4) && (numM2 == 1))
						{
							jvx_quat* q1 = (jvx_quat*)mxGetData(prhs[1]);
							jvx_quat* q2 = (jvx_quat*)mxGetData(prhs[2]);

							dims[0] = (SZ_MAT_TYPE)1;
							dims[1] = (SZ_MAT_TYPE)4;
							arr = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
							jvx_quat* qout = (jvx_quat*)mxGetData(arr);
							plhs[0] = arr;

							jvx_quat_mult(q1, q2, qout);
						}
						else
						{
							mexWarnMsgTxt("Two data input arguments must be of dimension [1x4] to hold two quats [qx qy qz qw].");
						}
					}
					else
					{
						mexWarnMsgTxt("Data input arguments must be of type <jvxData>.");
					}
				} // if (nrhs == 3)
				else
				{
					std::string txt = "Option <" + purpose + "> requires 3 input arguments.";
					mexWarnMsgTxt(txt.c_str());
				}
			}
			else if (purpose == "quatinv")
			{
				if (nrhs == 2)
				{
					if (mxIsData(prhs[1]))
					{
						jvxSize numN1 = mxGetN(prhs[1]);
						jvxSize numM1 = mxGetM(prhs[1]);

						if ((numN1 == 4) && (numM1 == 1))
						{
							jvx_quat* q1 = (jvx_quat*)mxGetData(prhs[1]);							

							dims[0] = (SZ_MAT_TYPE)1;
							dims[1] = (SZ_MAT_TYPE)4;
							arr = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
							jvx_quat* qout = (jvx_quat*)mxGetData(arr);
							plhs[0] = arr;

							jvx_quat_inv(q1, qout);
						}
						else
						{
							mexWarnMsgTxt("Two data input arguments must be of dimension [1x4] to hold two quats [qx qy qz qw].");
						}
					}
					else
					{
						mexWarnMsgTxt("Data input arguments must be of type <jvxData>.");
					}
				} // if (nrhs == 2)
				else
				{
					std::string txt = "Option <" + purpose + "> requires 2 input arguments.";
					mexWarnMsgTxt(txt.c_str());
				}
			}
			
			else
			{
				std::string txt = "First input argument specifies string <" + purpose + "> which is not a valid option.";
				mexWarnMsgTxt(txt.c_str());
			}
		} // if (res == JVX_NO_ERROR)
		else
		{
			std::string txt = "First input argument must be a stringto declare a valid option.";
			mexWarnMsgTxt(txt.c_str());
		}

	}
	else
	{
		mexPrintf("This function requires to comply with the following conventions:\n");
		mexPrintf("Option 1: [rotMat] = jvx3DPositionTools('quat2rotmat', quat) \n");
		mexPrintf("Option 2: [quat] = jvx3DPositionTools('rotmat2quat', rotmat) \n");
		mexPrintf("Option 3: [angles] = jvx3DPositionTools('quat2euler', quat [, euleroption]) \n");
		mexPrintf("          (this is the cython implementation).\n");
		mexPrintf("Option 4: [rotMat] = jvx3DPositionTools('euler2rotmat', angles [, euleroption]) \n");
		mexPrintf("Option 5: [angles] = jvx3DPositionTools('rotmat2euler', angles [, euleroption]) \n");	
		mexPrintf("Option 6: [quat] = jvx3DPositionTools('quatmult', quat1, quat2) \n");
		mexPrintf("Option 7: [quat] = jvx3DPositionTools('quatinv', quatIn) \n");		
		mexPrintf("========  quat is always a 1x4 vector with [qx,qy,qz,qw].\n");
		mexPrintf("========  rotMat is always a 3x3 matrix to transform a vector [x;y;z] into a tranformed vector.\n");
		mexPrintf("========  euleroption is always a string with xyz or zyx - more options may be added in the future.\n");
		mexPrintf("========  All conversions regarding Euler are extrinsic.\n");

	}
	return;
}

