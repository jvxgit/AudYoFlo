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

/**
 * C-style entry from MATLAB to MEX component.
 * All command arguments are passed in the optional arguments for
 * this mex function.
 *///=============================================================
void mexFunction( int nlhs, mxArray *plhs[],
 				  int nrhs, const mxArray *prhs[] )
{
	bool doit = false;
	jvxInt64 value = 0;
	jvxInt16 shiftVal = 0;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	plhs[0] = mxCreateNumericArray(ndim, dims, mxINT64_CLASS, mxREAL);
	jvxInt64* ptrWrite = (jvxInt64*)mxGetData(plhs[0]);
	*ptrWrite = 0;

	if((nrhs == 1)&&(nlhs == 1))
	{
		if(mxIsInt64(prhs[0]))
		{

			value = (*(jvxInt64*)mxGetData(prhs[0]));
			if(value >= 0)
			{
				doit = true;
			}
		}
	}

	if(doit)
	{
		int cntBits = 0;
		while(value > 0)
		{
			value = value >> 1;
			cntBits ++;
		}
		*ptrWrite = cntBits;
	}
	else
	{
		mexWarnMsgTxt("Input arguments not correct for rtpBitShift64!");
	}
}

