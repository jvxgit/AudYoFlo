#include "jvx.h"

#include "jvx_matrix/jvx_matrix_rot.h"

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

#define CLIP_VALUE 0.99
#define SMOOTH_LEVELS 0.995



/**
 * C-style entry from MATLAB to MEX component.
 * All command arguments are passed in the optional arguments for
 * this mex function.
 *///=============================================================
void mexFunction( int nlhs, mxArray *plhs[],
 				  int nrhs, const mxArray *prhs[] )
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

	jvxSize i,j;

	if (nlhs == 1)
	{
		if (
			(nrhs > 0) && (nrhs <= 2))
		{
			if (mxIsData(prhs[0]))
			{
				jvxData* rotxyz = (jvxData*)mxGetData(prhs[0]);
				jvxSize numN = mxGetN(prhs[0]);
				jvxSize numM = mxGetM(prhs[0]);
				jvxData tmp1, tmp2, tmp3;
				if (
					(numN == 1) && (numM == 3) ||
					(numN == 3) && (numM == 1))
				{

					jvx_matrix rotMat;

					jvx_matrix_initConfig(&rotMat);
					rotMat.prmInit.format = JVX_DATAFORMAT_DATA;
					rotMat.prmInit.N = 3;
					rotMat.prmInit.M = 3;
					rotMat.prmInit.lin_field_matrix = c_false;
					rotMat.prmInit.intern_alloc_cont = c_true;

					jvx_matrix_prepare(&rotMat);
					jvx_matrix_euler_deg_2_rotmatrix_extrinsic(rotxyz , &rotMat, JVX_EULER_CONVERT_XYZ);

					if (nrhs == 1)
					{
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
					}
					else
					{
						jvxData* inxyz = (jvxData*)mxGetData(prhs[1]);
						jvxSize numN = mxGetN(prhs[0]);
						jvxSize numM = mxGetM(prhs[0]);
						jvxData tmp1, tmp2, tmp3;
						if (
							(numN == 1) && (numM == 3) ||
							(numN == 3) && (numM == 1))
						{
							dims[0] = (SZ_MAT_TYPE)1;
							dims[1] = (SZ_MAT_TYPE)3;
							arr = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
							jvxData* out = (jvxData*)mxGetData(arr);
							memset(out, 0, sizeof(jvxData) * 1 * 3);

							jvx_matrix_process_rotmatrix_vec(&rotMat, inxyz, out);
							plhs[0] = arr;
						} // if ((numN == 1) && (numM == 3))
						else
						{
							mexWarnMsgTxt("Second input argument must be either of dimension [1x3] or [3x1].");
						}
					} // if (nrhs == 2)

					// Clear out the matrix
					jvx_matrix_postprocess(&rotMat);

				} // if (
				  //    (numN == 1) && (numM == 3) ||
				  //    (numN == 3) && (numM == 1))				
				else
				{
					mexWarnMsgTxt("First input argument must be either of dimension [1x3] or [3x1].");
				}
			} // if (mxIsData(prhs[0]))
			else
			{
				mexWarnMsgTxt("First argument must be of type <jvxData>.");
			}
		} // if ((nrhs > 0) && (nrhs <= 2))
		else
		{ 
			mexWarnMsgTxt("Either 1 or 2 input arguments are required!");
		}

		return;
	}	
}

