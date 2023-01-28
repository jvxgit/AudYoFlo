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


	if((nrhs == 1)&&(nlhs == 1))
	{
		if(mxIsData(prhs[0]))
		{
			jvxData* fldDataFrom = (jvxData*)mxGetData(prhs[0]);
			jvxSize numN = mxGetN(prhs[0]);
			jvxSize numM = mxGetM(prhs[0]);
			jvxSize num = numN * numM;
			dims[0] = (SZ_MAT_TYPE)numM; 
			dims[1] = (SZ_MAT_TYPE)numN;
			arr = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
			jvxData* fldDataTo = (jvxData*)mxGetData(arr);
			memcpy(fldDataTo, fldDataFrom, sizeof(jvxData) * num);
			jvxSize channelMap = 0;
			jvx_fieldLevelGainClip(
				reinterpret_cast<jvxHandle**>(&fldDataTo),
				1,
				num,
				JVX_DATAFORMAT_DATA,
				&input_avrg,
				&input_max,
				&input_gain,
				&input_clip,
				&channelMap,
				CLIP_VALUE, SMOOTH_LEVELS);
			plhs[0] = arr;
		}		
		else
		{ 
			mexWarnMsgTxt("Input arguments must be jvxData!");
		}
		return;
	}
	mexWarnMsgTxt("This function requires 1 input and 1 output arguments!");
}

