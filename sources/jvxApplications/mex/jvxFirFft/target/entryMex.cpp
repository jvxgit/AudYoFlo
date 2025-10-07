#include "jvx.h"
extern "C"
{
#include "jvx_fft_tools/jvx_firfft.h"
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

#include "CjvxCToMatlabConverter.h"

#define CLIP_VALUE 0.99
#define SMOOTH_LEVELS 0.995

/**
 * C-style entry from MATLAB to MEX component.
 * All command arguments are passed in the optional arguments for
 * this mex function.
 * Input arguments
 * 1) input signal,
 * 2) numerator coeffs
 * 3) denominator coeffs, can be even left out (=1)
 * 4) Filter states, can even be left out to start with 0-states
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
	const mxArray* arrDen = NULL;
	const mxArray* arrStates = NULL;

	jvxBool paramsOk = true;
	if (!
		((nrhs >= 1) && (nrhs <= 4)))
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
		mexPrintf("Function <jvxIIR1Can> to filter an input with an IIR filter of order M.\n");
		mexPrintf("Input Argument #1: 1 x N input vector.\n");
		mexPrintf("Input Argument #2: 1 x (M+1) Numerator vector - double, opt.\n");
		mexPrintf("Input Argument #3: 1 x (M+1) Denominator vector - double, opt.\n");
		mexPrintf("Input Argument #4: 1 x M Filter States - double, opt.\n");
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
			arrDen = prhs[2];
			if (arrDen)
			{
				if (mxIsData(arrDen))
				{
					lenBufStates = JVX_MAX(lenBufStates, mxGetNumberOfElements(arrDen) - 1);
				}
			}
		}
		if (nrhs >= 4)
		{
			arrStates = prhs[3];
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
			// jvx_circbuffer* theCircBuffer = NULL;
			// jvx_circbuffer_allocate_1chan(&theCircBuffer, lenBufStates);
			jvxData* fCoeffsNum = NULL;
			jvxData* fCoeffsDen = NULL;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fCoeffsNum, jvxData, (lenBufStates + 1));
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fCoeffsDen, jvxData, (lenBufStates + 1));
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
			if (arrDen && mxIsData(arrDen))
			{
				jvxData* cpFrom = (jvxData*)mxGetData(arrDen);
				jvxSize num = mxGetNumberOfElements(arrDen);
				memcpy(fCoeffsDen, cpFrom, sizeof(jvxData) * num);
			}
			else
			{
				fCoeffsDen[0] = 1.0;
			}
			if (arrStates && mxIsData(arrStates))
			{
				jvxData* cpFrom = (jvxData*)mxGetData(arrStates);
				jvxSize num = mxGetNumberOfElements(arrStates);
				//memcpy(theCircBuffer->ram.field[0], cpFrom, sizeof(jvxData) * num);
			}

			jvxData* in = (jvxData*)mxGetData(arrIn);

			// Important: we must not run in-place: this breaks the variable in Matlab!!
			jvxData* out = nullptr;
			JVX_SAFE_ALLOCATE_FIELD_CPP_Z(out, jvxData, lenSig);

			//jvx_circbuffer_iir_1can_2io(theCircBuffer, fCoeffsNum, fCoeffsDen, &in, &out, lenSig);

			if (nlhs >= 1)
			{
				CjvxCToMatlabConverter::mexReturnDataList(plhs[0], out, lenSig);
			}
			if (nlhs >= 2)
			{
				//CjvxCToMatlabConverter::mexReturnDataList(plhs[1], theCircBuffer->ram.field[0], lenBufStates);
			}

			// Clear data 
			JVX_DSP_SAFE_DELETE_FIELD(out);
			// jvx_circbuffer_deallocate(theCircBuffer);
			JVX_DSP_SAFE_DELETE_FIELD(fCoeffsNum);
			JVX_DSP_SAFE_DELETE_FIELD(fCoeffsDen);

		} // if ((lenBufStates > 0) &&(lenSig > 0))
		else
		{
			mexWarnMsgTxt("Input arguments must be fields of jvxData to hold the input signal and at least numerator or denominator!");
		}
		return;
	}
	mexWarnMsgTxt("This function requires 1-4 input and 1-2 output arguments!");
}

