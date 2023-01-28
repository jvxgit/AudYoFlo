#include "jvx.h"

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

#include "codeFragments/matlab_c/CjvxCToMatlabConverter.h"
#include "jvx-apvq.h"

typedef struct
{
	jvxApvq ap;
	jvxUInt64 numIdxMaxSphere;
	jvxUInt64 numIdxMaxRadius;
} oneApvqHdl;

jvxUInt32 uId = 1;
std::map<jvxUInt32, oneApvqHdl> allHandles;

/**
 * C-style entry from MATLAB to MEX component.
 * All command arguments are passed in the optional arguments for
 * this mex function.
 *///=============================================================
void mexFunction( int nlhs, mxArray *plhs[],
 				  int nrhs, const mxArray *prhs[] )
{
	if (nrhs >= 1)
	{
		if (mxIsChar(prhs[0]))
		{
			std::string token = jvx_mex_2_cstring(prhs[0]);
			if (token == "init")
			{
				jvxApvq hdl;
				jvx_apvq_initCfg(&hdl);
				if (nrhs >= 2)
				{
					if (mxIsDouble(prhs[1]))
					{
						hdl.init_param.dimensionVQ = (*(double*)mxGetData(prhs[1]));
					}
				}
				if (nrhs >= 2)
				{
					if (mxIsDouble(prhs[2]))
					{
						hdl.init_param.overallBitrate = (*(double*)mxGetData(prhs[2]));
					}
				}
				if (nrhs >= 3)
				{
					if (mxIsDouble(prhs[3]))
					{
						hdl.init_param.num_delta_steps = (*(double*)mxGetData(prhs[3]));
					}
				}

				hdl.init_param.radius_and_sphere_q = false;

				jvx_apvq_init(&hdl);

				oneApvqHdl newElm;
				newElm.ap = hdl;
				jvx_apvq_number_indices(&hdl, &newElm.numIdxMaxSphere, &newElm.numIdxMaxRadius);
				allHandles[uId] = newElm;
				if (nlhs >= 1)
				{
					CjvxCToMatlabConverter::mexReturnUInt32(plhs[0], uId);
				}
				uId++;

				CjvxCToMatlabConverter::mexFillEmpty(plhs, nlhs, 1);
			}
			else if (token == "numq")
			{
				jvxUInt32 id = 0;
				jvxUInt64 numNodes = 0;
				jvxUInt64  numRadius = 0;
				jvxSize off = 0;
				if (nrhs >= 2)
				{
					if (mxIsUint32(prhs[1]))
					{
						id = (*(jvxUInt32*)mxGetData(prhs[1]));
					}
				}
				auto elm = allHandles.find(id);
				if (elm != allHandles.end())
				{
					if (nlhs >= 1)
					{
						CjvxCToMatlabConverter::mexReturnUInt64(plhs[0], elm->second.numIdxMaxSphere);
					}
					if (nlhs >= 2)
					{
						CjvxCToMatlabConverter::mexReturnUInt64(plhs[1], elm->second.numIdxMaxRadius);
					}
					off = 2;
				}
				else
				{
					mexWarnMsgTxt(("Could not find vq with id " + jvx_size2String(id) + "!").c_str());
				}
				CjvxCToMatlabConverter::mexFillEmpty(plhs, nlhs, off);
			}
			else if (token == "terminate")
			{
				jvxUInt32 id = 0;
				jvxUInt64 numNodes = 0;
				jvxUInt64  numRadius = 0;

				if (nrhs >= 2)
				{
					if (mxIsUint32(prhs[1]))
					{
						id = (*(jvxUInt32*)mxGetData(prhs[1]));
					}
				}
				auto elm = allHandles.find(id);
				if (elm != allHandles.end())
				{
					jvxApvq hdl = elm->second.ap;
					jvx_apvq_terminate(&hdl);
					allHandles.erase(elm);
				}
				else
				{
					mexWarnMsgTxt(("Could not find vq with id " + jvx_size2String(id) + "!").c_str());
				}

				CjvxCToMatlabConverter::mexFillEmpty(plhs, nlhs, 0);
			}
			else if (token == "decode")
			{
				jvxUInt32 id = 0;
				jvxUInt64 idxVec = 0;
				jvxUInt64  idxRadius = 0;
				jvxSize off = 0;
				jvxBool cartOut = false;
				if (nrhs >= 2)
				{
					if (mxIsUint32(prhs[1]))
					{
						id = (*(jvxUInt32*)mxGetData(prhs[1]));
					}
				}
				if (nrhs >= 3)
				{
					if (mxIsDouble(prhs[2]))
					{
						idxVec = JVX_DATA2SIZE(*(double*)mxGetData(prhs[2]));
					}
					if (mxIsUint64(prhs[2]))
					{
						idxVec = (*(jvxUInt64*)mxGetData(prhs[2]));
					}
					if (mxIsUint32(prhs[2]))
					{
						idxVec = (*(jvxUInt32*)mxGetData(prhs[2]));
					}
					if (mxIsInt64(prhs[2]))
					{
						idxVec = (*(jvxUInt64*)mxGetData(prhs[2]));
					}
					if (mxIsInt32(prhs[2]))
					{
						idxVec = (*(jvxUInt32*)mxGetData(prhs[2]));
					}
				}
				if (nrhs >= 4)
				{
					if (mxIsDouble(prhs[3]))
					{
						idxRadius = JVX_DATA2SIZE(*(double*)mxGetData(prhs[3]));
					}
					if (mxIsUint64(prhs[3]))
					{
						idxRadius = (*(jvxUInt64*)mxGetData(prhs[3]));
					}
					if (mxIsUint32(prhs[3]))
					{
						idxRadius = (*(jvxUInt32*)mxGetData(prhs[3]));
					}
					if (mxIsInt64(prhs[3]))
					{
						idxRadius = (*(jvxUInt64*)mxGetData(prhs[3]));
					}
					if (mxIsInt32(prhs[3]))
					{
						idxRadius = (*(jvxUInt32*)mxGetData(prhs[3]));
					}
				}
				if (nrhs >= 5)
				{
					if (mxIsDouble(prhs[4]))
					{
						if (JVX_DATA2SIZE(*(double*)mxGetData(prhs[4])) > 0)
						{
							cartOut = true;
						}
					}
				}

				auto elm = allHandles.find(id);
				if (elm != allHandles.end())
				{
					if (idxVec < elm->second.numIdxMaxSphere)
					{
						if (idxRadius < elm->second.numIdxMaxRadius)
						{
							jvxApvq hdl = elm->second.ap;
							if (nlhs >= 1)
							{
								SZ_MAT_TYPE ndim = 2;
								SZ_MAT_TYPE dims[2] = { 1, 1 };

								dims[0] = 1;
								dims[1] = (int)hdl.init_param.dimensionVQ;

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
								plhs[0] = mxCreateNumericArray(ndim, dims, mxDOUBLE_CLASS, mxREAL);
#else
								plhs[0] = mxCreateNumericArray(ndim, dims, mxSINGLE_CLASS, mxREAL);
#endif
								jvxData* ptrWrite = (jvxData*)mxGetData(plhs[0]);
								jvx_apvq_decode(&hdl, ptrWrite, hdl.init_param.dimensionVQ, idxVec, idxRadius, cartOut);
								off = 1;
							}
						}
						else
						{
							mexWarnMsgTxt(("Index for radius decoding " + jvx_size2String(idxRadius) + " is too large!").c_str());
						}
					}
					else
					{
						mexWarnMsgTxt(("Index for sphere decoding " + jvx_size2String(idxVec) + " is too large!").c_str());
					}
				}
				else
				{
					mexWarnMsgTxt(("Could not find vq with id " + jvx_size2String(id) + "!").c_str());
				}

				CjvxCToMatlabConverter::mexFillEmpty(plhs, nlhs, off);
			}
			else
			{
				// No valid token
				mexWarnMsgTxt("No valid text token!");
				CjvxCToMatlabConverter::mexFillEmpty(plhs, nlhs, 0);
			}
		}
		else
		{
			// No valid token
			mexWarnMsgTxt("First argument must be text token!");
			CjvxCToMatlabConverter::mexFillEmpty(plhs, nlhs, 0);
		}
	}
	else
	{
		mexWarnMsgTxt("This function requires >= 1 inputargument!");
		CjvxCToMatlabConverter::mexFillEmpty(plhs, nlhs, 0);
	}
}

