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

#include "CjvxCToMatlabConverter.h"

/**
 * C-style entry from MATLAB to MEX component.
 * All command arguments are passed in the optional arguments for
 * this mex function.
 *///=============================================================
void mexFunction( int nlhs, mxArray *plhs[],
 				  int nrhs, const mxArray *prhs[] )
{
	if((nrhs == 3)&&(nlhs == 1))
	{
		if(
			(mxIsUint64(prhs[0])) &&
			(mxIsInt16(prhs[1])) &&
			(mxIsInt16(prhs[2])))
		{
			jvxUInt64 valI = (*(jvxUInt64*)mxGetData(prhs[0]));
			jvxInt16 valII = (*(jvxInt16*)mxGetData(prhs[1]));
			jvxInt16 valIII = (*(jvxInt16*)mxGetData(prhs[2]));
			jvxUInt64 mask = (1 << valIII) - 1;

			mask = mask << valII;
			valI = valI & mask;
			valI = valI >> valII;

			CjvxCToMatlabConverter::mexReturnUInt64(plhs[0], valI);
		}
		else if (
			(mxIsUint32(prhs[0])) &&
			(mxIsInt16(prhs[1])) &&
			(mxIsInt16(prhs[2])))
		{
			jvxUInt32 valI = (*(jvxUInt32*)mxGetData(prhs[0]));
			jvxInt16 valII = (*(jvxInt16*)mxGetData(prhs[1]));
			jvxInt16 valIII = (*(jvxInt16*)mxGetData(prhs[2]));
			jvxUInt32 mask = (1 << valIII) - 1;

			mask = mask << valII;
			valI = valI & mask;
			valI = valI >> valII;

			CjvxCToMatlabConverter::mexReturnUInt32(plhs[0], valI);
		}
		else if (
			(mxIsUint16(prhs[0])) &&
			(mxIsInt16(prhs[1])) &&
			(mxIsInt16(prhs[2])))
		{
			jvxUInt16 valI = (*(jvxUInt16*)mxGetData(prhs[0]));
			jvxInt16 valII = (*(jvxInt16*)mxGetData(prhs[1]));
			jvxInt16 valIII = (*(jvxInt16*)mxGetData(prhs[2]));
			jvxUInt16 mask = (1 << valIII) - 1;

			mask = mask << valII;
			valI = valI & mask;
			valI = valI >> valII;

			CjvxCToMatlabConverter::mexReturnUInt16(plhs[0], valI);
		}
		else
		{ 
			mexWarnMsgTxt("Input arguments must be 1 uint16, uint32 or uint64 and two int16!");
		}
		return;
	}
	mexWarnMsgTxt("This function requires 3 input and 1 output arguments!");
	CjvxCToMatlabConverter::mexReturnInt32List(plhs[0], NULL, 0);
}

