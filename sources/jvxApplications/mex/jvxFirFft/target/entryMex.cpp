#include "jvx.h"
extern "C"
{
#include "jvx_fft_tools/jvx_firfft.h"
#include "jvx_fft_tools/jvx_firfft_cf.h"
#include "jvx_fft_tools/jvx_firfft_cf_nout.h"
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

#define VERSION_NOUT

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
	jvxSize filterOrder = 1;
	jvxSize filteredOut = 1;
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
		mexPrintf("Function <jvxFirFftn> to filter an input with an FIR filter of order M by involving FFTs.\n");
		mexPrintf("Input Argument #1: 1 x N input vector.\n");
		mexPrintf("Input Argument #2: P x M vector of filter coefficients.\n");
		return;
	}
	else
	{
		jvxSize lenSig = 0;
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
					filteredOut = mxGetM(arrNum);
					filterOrder = mxGetN(arrNum);
				}
			}
		}
		if (
			(filterOrder > 0) &&
			(lenSig > 0))
		{
			// Now here we go
			std::string errDescr;
			jvx_firfft init;
			jvx_firfft_initCfg(&init);

			init.init.bsize = 128;
			init.init.lFir = filterOrder;
			init.init.type = JVX_FIRFFT_FLEXIBLE_FIR;
	
			jvxData** firsIn = nullptr;
			JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(firsIn, jvxData, filteredOut, filterOrder);
			CjvxCToMatlabConverter::convertMexToC(reinterpret_cast<jvxHandle**>(firsIn), filteredOut, filterOrder, JVX_DATAFORMAT_DATA, arrNum, "input", false, errDescr);

			if (filteredOut == 1)
			{
				init.init.fir = firsIn[0];
				jvx_firfft_cf_init(&init, nullptr);
			}
			else
			{
				jvx_firfft_cf_nout_init(&init, nullptr, filteredOut, firsIn);
			}
			jvxData* inSig = (jvxData*)mxGetData(arrIn);
			
			jvxData** outSig = nullptr;
			JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(outSig, jvxData, filteredOut, lenSig);

			jvxDataCplx** inFirCplx = nullptr;
			JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(inFirCplx, jvxDataCplx, filteredOut, init.derived.szFftValue / 2 + 1);
	
			jvxData* inSigBuf = nullptr;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(inSigBuf, jvxData, init.init.bsize);

			jvxData** outSigBuf = nullptr;
			JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(outSigBuf, jvxData, filteredOut, init.init.bsize);
			
			jvxSize cntStart = 0;
			jvxSize cntStop = JVX_MIN(cntStart + init.init.bsize, lenSig);
			while (1)
			{
				jvxSize nSamples = cntStop - cntStart;
				jvxBool weightsUpdated = false;
				if (nSamples > 0)
				{
					memset(inSigBuf, 0, sizeof(jvxData) * init.init.bsize);
					memcpy(inSigBuf, inSig + cntStart, sizeof(jvxData) * nSamples);
					if (!weightsUpdated)
					{
						if (filteredOut == 1)
						{
							jvx_firfft_cf_compute_weights(&init, firsIn[0], filterOrder);
							jvx_firfft_cf_copy_weights(&init, inFirCplx[0], init.derived.szFftValue / 2 + 1);
							jvx_firfft_cf_process_update_weights(&init, inSigBuf, outSigBuf[0], inFirCplx[0], false);
						}
						else
						{
							for (i = 0; i < filteredOut; i++)
							{
								jvx_firfft_cf_nout_compute_weights(&init, firsIn[i], filterOrder);
								jvx_firfft_cf_nout_copy_weights(&init, inFirCplx[i], init.derived.szFftValue / 2 + 1);
							}
							jvx_firfft_cf_nout_process_update_weights(&init, inSigBuf, outSigBuf, inFirCplx, false);
						}
						weightsUpdated = true;
					}
					else
					{
						jvx_firfft_cf_nout_process(&init, inSigBuf, outSigBuf, false);
					}
					for (i = 0; i < filteredOut; i++)
					{
						memcpy(outSig[i] + cntStart, outSigBuf[i], sizeof(jvxData) * nSamples);
					}

					cntStart = cntStop;
					cntStop = JVX_MIN(cntStart + init.init.bsize, lenSig);
				}
				else
				{
					// Loop is done!
					break;
				}
			}

			if (nlhs >= 1)
			{
				CjvxCToMatlabConverter::mexReturnGenericNumeric(plhs[0], (const jvxHandle**) outSig, filteredOut, lenSig, JVX_DATAFORMAT_DATA);
			}

			// Clear data 
			JVX_DSP_SAFE_DELETE_FIELD(outSig);
			JVX_DSP_SAFE_DELETE_FIELD(inSigBuf);
			JVX_DSP_SAFE_DELETE_FIELD(outSigBuf);


			if (filteredOut == 1)
			{
				jvx_firfft_cf_terminate(&init);
			}
			else
			{
				jvx_firfft_cf_nout_terminate(&init);
			}
		} // if ((lenBufStates > 0) &&(lenSig > 0))
		else
		{
			mexWarnMsgTxt("Input arguments must be fields of jvxData to hold the input signal and at least numerator or denominator!");
		}
		return;
	}
	mexWarnMsgTxt("This function requires 1-4 input and 1-2 output arguments!");
}

