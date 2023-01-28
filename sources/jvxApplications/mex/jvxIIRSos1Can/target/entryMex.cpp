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
 * 3) denominator coeffs, can be even left out (=1)
 * 4) Filter states, can even be left out to start with 0-states
 * Output arguments
 * 1) Output signal
 * 2) Filterstates, can be left out if not interesting
 *///=============================================================
void mexFunction( int nlhs, mxArray *plhs[],
 				  int nrhs, const mxArray *prhs[] )
{
	jvxSize i,j;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	const mxArray* arrIn = NULL;
	const mxArray* arrNum = NULL;
	const mxArray* arrDen = NULL;
	const mxArray* arrGains = NULL;
	const mxArray* arrShift = NULL;
	const mxArray* arrStates = NULL;

	jvxBool paramsOk = true;
	if (!
		((nrhs >= 1) && (nrhs <= 6)))
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
		mexPrintf("Function <jvxIIRSos1Can> to filter an input with an IIR filter specified in second order sections. Min order is 1.\n");
		mexPrintf("Input Argument #1: 1 x N input vector.\n");
		mexPrintf("Input Argument #2: M x 3 Numerator arrays - double, opt.\n");
		mexPrintf("Input Argument #3: M x 3 Denominator arrays - double, opt.\n");
		mexPrintf("Input Argument #4: 1 x M Gains array - double, opt.\n");
		mexPrintf("Input Argument #5: 1 x 1 Shift factor - int16, opt.\n");
		mexPrintf("Input Argument #6: 1 x M Filter States - double, opt.\n");
		return;
	}
	else
	{
		jvxSize lenSig = 0;
		jvxSize nSection = 1;
		jvxSize orderFilt = 2;
		assert(nrhs >= 1);
		arrIn = prhs[0];
		if (arrIn)
		{
			if (mxIsData(arrIn))
			{
				lenSig = JVX_MAX(lenSig, mxGetNumberOfElements(arrIn));
			}
		}

		if (nrhs >= 2)
		{
			arrNum = prhs[1];
			if (arrNum)
			{
				if (mxIsData(arrNum))
				{
					nSection = JVX_MAX(nSection, mxGetM(arrNum));
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
					nSection = JVX_MAX(nSection, mxGetM(arrDen));
				}
			}
		}

		if (nrhs >= 4)
		{
			arrGains = prhs[3];
			if (mxIsData(arrGains))
			{
				nSection = JVX_MAX(nSection, mxGetNumberOfElements(arrGains));
			}
		}

		if (nrhs >= 5)
		{
			arrShift = prhs[4];
		}

		if (nrhs >= 6)
		{
			arrStates = prhs[5];
			/* The states should follow the other configuration parameters, they should not have impact on sections or filtersize
			if (arrStates)
			{
				if (mxIsData(arrStates))
				{
					nSection = JVX_MAX(nSection, mxGetNumberOfElements(arrStates));
				}
			}
			*/
		}		
		
		if (
			(nSection > 0) &&
			(lenSig > 0))
		{
			// Now here we go
			jvx_circbuffer* theCircBuffer = NULL;

			// Filter order is always orderFilt=2 - second order section :-). Number of sections, however, may vary
			jvx_circbuffer_allocate(&theCircBuffer, orderFilt, nSection, 1);
			jvxData* fCoeffsNum = NULL;
			jvxData* fCoeffsDen = NULL;
			jvxData* fCoeffsGain = NULL;
			jvxInt16 shiftFac = 0;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fCoeffsNum, jvxData, (nSection * 3));
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fCoeffsDen, jvxData, (nSection * 3));
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fCoeffsGain, jvxData, nSection);

			for (i = 0; i < nSection; i++)
			{
				fCoeffsNum[i * 3] = 1;
				fCoeffsDen[i * 3] = 1;
				fCoeffsGain[i] = 1;
			}
			
			if ((arrNum) && mxIsData(arrNum))
			{
				jvxData* cpFrom = (jvxData*)mxGetData(arrNum);
				jvxSize M = mxGetM(arrNum);
				jvxSize N = mxGetN(arrNum);
				for (i = 0; i < M; i++)
				{
					for (j = 0; j < N; j++)
					{
						fCoeffsNum[i * 3 + j] = cpFrom[j * M + i];
					}
				}
			}

			if (arrDen && mxIsData(arrDen))
			{
				jvxData* cpFrom = (jvxData*)mxGetData(arrDen);
				jvxSize M = mxGetM(arrDen);
				jvxSize N = mxGetN(arrDen);
				for (i = 0; i < M; i++)
				{
					for (j = 0; j < N; j++)
					{
						fCoeffsDen[i * 3 + j] = cpFrom[j * M + i];
					}
				}
			}

			if (arrStates && mxIsData(arrStates))
			{
				jvxData* cpFrom = (jvxData*)mxGetData(arrStates);
				jvxSize num = mxGetNumberOfElements(arrStates);
				for (i = 0; i < nSection; i++)
				{
					jvxSize numCp = JVX_MIN(orderFilt, num);
					if (numCp)
					{
						memcpy(theCircBuffer[i].ram.field[0], cpFrom, sizeof(jvxData) * numCp);
					}
					num -= numCp;
					cpFrom += numCp;
				}
			}

			if(arrGains && mxIsData(arrGains))
			{
				jvxData* cpFrom = (jvxData*)mxGetData(arrGains);
				jvxSize num = mxGetNumberOfElements(arrGains);
				memcpy(fCoeffsGain, cpFrom, sizeof(jvxData) * num);
			}

			if (arrShift && mxIsInt16(arrShift))
			{
				jvxInt16* cpFrom = (jvxInt16*)mxGetData(arrShift);				
				shiftFac = *cpFrom;
			}

			jvxData* in = (jvxData*)mxGetData(arrIn);
			jvx_circbuffer_iir_sos1can_1io(theCircBuffer, fCoeffsNum, fCoeffsDen, fCoeffsGain, shiftFac, &in, lenSig);

			if (nlhs >= 1)
			{
				CjvxCToMatlabConverter::mexReturnDataList(plhs[0], in, lenSig);
			}
			if (nlhs >= 2)
			{
				// Copy the states of sections one after the other
				SZ_MAT_TYPE ndim = 2;
				SZ_MAT_TYPE dims[2] = { 1, 1 };
				jvxSize num = nSection * orderFilt;
				dims[1] = num;
				plhs[1] = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);

				jvxData* ptrWrite = (jvxData*)mxGetData(plhs[1]);				
				for (i = 0; i < nSection; i++)
				{
					memcpy(ptrWrite, theCircBuffer[i].ram.field[0], sizeof(jvxData) * orderFilt);
					ptrWrite += orderFilt;
				}
			}

			// Clear data 
			jvx_circbuffer_deallocate(theCircBuffer);
			JVX_DSP_SAFE_DELETE_FIELD(fCoeffsNum);
			JVX_DSP_SAFE_DELETE_FIELD(fCoeffsDen);
			JVX_DSP_SAFE_DELETE_FIELD(fCoeffsGain);

		} // if ((lenBufStates > 0) &&(lenSig > 0))
		else
		{
			mexWarnMsgTxt("Input arguments must be fields of jvxData to hold the input signal and at least numerator, denominator, gains and shiftFac!");
		}
		return;
	}
	mexWarnMsgTxt("This function requires 1-6 input and 1-2 output arguments!");
}

