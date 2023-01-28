#include "jvx.h"
extern "C"
{
#include "jvx_circbuffer/jvx_circbuffer_fftifft.h"
}

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
 * Input arguments
 * 1) input signal,
 * 2) numerator coeffs
 * 3) Filter states, can even be left out to start with 0-states
 * Output arguments
 * 1) Output signal
 * 2) Filterstates, can be left out if not interesting
 *///=============================================================
void mexFunction( int nlhs, mxArray *plhs[],
 				  int nrhs, const mxArray *prhs[] )
{
	jvxSize i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	const mxArray* arrIn = NULL;
	const mxArray* arrNum = NULL;
	const mxArray* arrStates = NULL;

	jvxBool paramsOk = true;
	if (!
		((nrhs >= 1) && (nrhs <= 3)))
	{
		paramsOk = false;
	}
	if (paramsOk)
	{
		if (!
			((nlhs >= 1) && (nlhs <= 2)))
		{
			paramsOk = false;
		}
	}

	if (!paramsOk)
	{
		mexPrintf("Function <jvxFIR1Can> to filter an input with an FIR filter of order M.\n");
		mexPrintf("Input Argument #1: 1 x N input vector.\n");
		mexPrintf("Input Argument #2: 1 x (M+1) Numerator vector - double, opt.\n");
		mexPrintf("Input Argument #3: 1 x M Filter States - double, opt.\n");
		return;
	}
	else
	{
		jvxSize lenSig = 0;
		jvxSize lenBufStates = 0;
		if (nrhs >= 1)
		{
			arrIn = prhs[0];
			if (arrIn)
			{
				if (mxIsData(arrIn))
				{
					lenSig = JVX_MAX(lenSig, mxGetNumberOfElements(arrIn));
				}
			}
		}
		if (nrhs >= 2)
		{
			arrNum = prhs[1];
			if (arrNum)
			{
				if (mxIsData(arrNum))
				{
					lenBufStates = JVX_MAX(lenBufStates, mxGetNumberOfElements(arrNum) - 1);
				}
			}
		}
		
		if (nrhs >= 3)
		{
			arrStates = prhs[2];
			if (arrStates)
			{
				if (mxIsData(arrStates))
				{
					lenBufStates = JVX_MAX(lenBufStates, mxGetNumberOfElements(arrStates));
				}
			}
		}
		if (
			(lenBufStates > 0) &&
			(lenSig > 0))
		{
			// Now here we go
			jvx_circbuffer* theCircBuffer = NULL;
			jvx_circbuffer_allocate_1chan(&theCircBuffer, lenBufStates);
			jvxData* fCoeffsNum = NULL;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fCoeffsNum, jvxData, (lenBufStates + 1));
			if ((arrNum) && mxIsData(arrNum))
			{
				jvxData* cpFrom = (jvxData*)mxGetData(arrNum);
				jvxSize num = mxGetNumberOfElements(arrNum);
				memcpy(fCoeffsNum, cpFrom, sizeof(jvxData) * num);
			}
			else
			{
				fCoeffsNum[0] = 1.0;
			}
			if (arrStates && mxIsData(arrStates))
			{
				jvxData* cpFrom = (jvxData*)mxGetData(arrStates);
				jvxSize num = mxGetNumberOfElements(arrStates);
				memcpy(theCircBuffer->ram.field[0], cpFrom, sizeof(jvxData) * num);
			}

			jvxData* in = (jvxData*)mxGetData(arrIn);
			jvx_circbuffer_fir_1can_1io(theCircBuffer, fCoeffsNum, &in, lenSig);

			if (nlhs >= 1)
			{
				CjvxCToMatlabConverter::mexReturnDataList(plhs[0], in, lenSig);
			}
			if (nlhs >= 2)
			{
				CjvxCToMatlabConverter::mexReturnDataList(plhs[1], theCircBuffer->ram.field[0], lenBufStates);
			}

			// Clear data 
			jvx_circbuffer_deallocate(theCircBuffer);
			JVX_DSP_SAFE_DELETE_FIELD(fCoeffsNum);

		} // if ((lenBufStates > 0) &&(lenSig > 0))
		else
		{
			mexWarnMsgTxt("Input arguments must be fields of jvxData to hold the input signal and at least numerator or denominator!");
		}
		return;
	}
	mexWarnMsgTxt("This function requires 1-4 input and 1-2 output arguments!");
}

