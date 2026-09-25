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

#include "CjvxDigitalFilter.h"
#include "CjvxCToMatlabConverter.h"
#include "CjvxMatlabToCConverter.h"

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
	jvxSize i,j;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	const mxArray* arrTech = NULL;
	const mxArray* arrDescr = NULL;
	std::string tokenOper = "init";

	jvxBool paramsOk = true;
	jvxSize cntIn = 0;
	jvxSize cntOut = 0;

	if(nrhs > cntIn)
	{
		tokenOper = CjvxMatlabToCConverter::jvx_mex_2_cstring(prhs[cntIn++]);
	}

	if (tokenOper == "init")
	{
		if (!
			((nrhs >= cntIn) && (nrhs <= 9)))
		{
			paramsOk = false;
		}

		if (paramsOk)
		{
			if (!
				(nlhs > 1) && (nlhs <= 3))
			{
				paramsOk = false;
			}
		}

		if (!paramsOk)
		{
			mexPrintf("Function <jvxIIRDesign> to design filter and return SOS.\n");
			mexPrintf("Input Argument #0: String with operation: -> Chosen <init>.\n");

			mexPrintf("Input Argument #1: String with filter technology: [Elliptic, ChebyshevI, ChebyshevII, Bessel, Butterworth,v Legendre].\n");
			/*
			Elliptic,
			ChebyshevI,
			ChebyshevII,
			Bessel,
			Butterworth,
			Legendre
			*/

			mexPrintf("Input Argument #2: String with filter description: [LowPass, HighPass, BandPass, BandStop, LowShelf, HighShelf, BandShelf, Other].\n");
			/*
			kindLowPass,
			kindHighPass,
			kindBandPass,
			kindBandStop,
			kindLowShelf,
			kindHighShelf,
			kindBandShelf,
			kindOther
			*/


			mexPrintf("Input Argument #3: order.\n");
			mexPrintf("Input Argument #4: Samplerate fs.\n");
			mexPrintf("Input Argument #5: Samplerate fc.\n");
			mexPrintf("Input Argument #6: ripplePassDb.\n");
			mexPrintf("Input Argument #7: stopDb.\n");
			mexPrintf("Input Argument #8: freqWidth.\n");
			goto exit_call_error;
		}
		else
		{
			CjvxDigitalFilter* theFilterInst = nullptr;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(theFilterInst, CjvxDigitalFilter);

			constexpr int maxOrder = 16;
			int order = 6;
			std::string tokenTech = "Elliptic";
			std::string tokenDescr = "LowPass";
			int fs = 48000;
			int fc = 24000;
			int freqWidth = 200;
			jvxData rippleDb = 1;
			jvxData stopDb = 60;

			//jvxSize lenSig = 0;
			//jvxSize lenBufStates = 0;
			
			if (nrhs > cntIn)
			{
				arrTech = prhs[cntIn++];
				tokenTech = CjvxMatlabToCConverter::jvx_mex_2_cstring(arrTech);
			}

			if (nrhs > cntIn)
			{
				arrDescr = prhs[cntIn++];
				tokenDescr = CjvxMatlabToCConverter::jvx_mex_2_cstring(arrDescr);
			}

			if (nrhs > cntIn)
			{
				auto retVal = CjvxMatlabToCConverter::jvx_mex_2_numeric(prhs[cntIn++]);
				if (retVal.size() == 1)
				{
					retVal[0].toContent(&order);
					if (order > maxOrder)
					{
						order = maxOrder;
					}
				}
			}
			if (nrhs > cntIn)
			{
				auto retVal = CjvxMatlabToCConverter::jvx_mex_2_numeric(prhs[cntIn++]);
				if (retVal.size() == 1)
				{
					retVal[0].toContent(&fs);
				}
			}
			if (nrhs > cntIn)
			{
				auto retVal = CjvxMatlabToCConverter::jvx_mex_2_numeric(prhs[cntIn++]);
				if (retVal.size() == 1)
				{
					retVal[0].toContent(&fc);
				}
			}
			if (nrhs > cntIn)
			{
				auto retVal = CjvxMatlabToCConverter::jvx_mex_2_numeric(prhs[cntIn++]);
				if (retVal.size() == 1)
				{
					retVal[0].toContent(&rippleDb);					
				}
			}
			if (nrhs > cntIn)
			{
				auto retVal = CjvxMatlabToCConverter::jvx_mex_2_numeric(prhs[cntIn++]);
				if (retVal.size() == 1)
				{
					retVal[0].toContent(&stopDb);
				}
			}
			if (nrhs > cntIn)
			{
				auto retVal = CjvxMatlabToCConverter::jvx_mex_2_numeric(prhs[cntIn++]);
				if (retVal.size() == 1)
				{
					retVal[0].toContent(&freqWidth);
				}
			}

			// kindHighPass,
			// kindBandPass,
			// kindBandStop,
			// kindLowShelf,
			// kindHighShelf,
			// kindBandShelf
			jvxBool withG = false;
			if (nlhs > 1)
			{
				withG = true;
			}
			if (theFilterInst->initialize(tokenTech, tokenDescr, order, fs, fc, rippleDb, stopDb, freqWidth, withG) == JVX_NO_ERROR)
			{

				auto coeffs = theFilterInst->coefficients_sos();
				auto g = theFilterInst->gain();

				jvxData** out = nullptr;
				jvxSize N = coeffs.size();

				if (N > 0)
				{
					jvxSize M = coeffs[0].size();
					JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(out, jvxData, N, M);
					for (i = 0; i < N; i++)
					{
						std::vector<jvxData>& oneSet = coeffs[i];
						for (j = 0; j < M; j++)
						{
							out[i][j] = oneSet[j];
						}
					}

					CjvxCToMatlabConverter::mexReturnGenericNumeric(plhs[cntOut++], (const jvxHandle**)out, N, M, JVX_DATAFORMAT_DATA);
					JVX_SAFE_DELETE_2DFIELD(out, N);

					if (nlhs > cntOut)
					{
						CjvxCToMatlabConverter::mexReturnData(plhs[cntOut++], g);
					}

					if (nlhs > cntOut)
					{
						CjvxCToMatlabConverter::mexReturnIntPtr(plhs[cntOut++], theFilterInst);
						theFilterInst = nullptr;
					}
				}
				else
				{
					mexWarnMsgTxt("Designed filter is empty.");
					goto exit_call_error;
				}
			}
			else
			{
				mexWarnMsgTxt("Filter design failed.");
				goto exit_call_error;
			}						
		}
	} // if (tokenOper == "init")
	else if (tokenOper == "terminate")
	{
		if (!(nrhs > cntIn))
		{
			paramsOk = false;
		}

		if (!paramsOk)
		{
			mexPrintf("Function <jvxIIRDesign> to design filter and return SOS.\n");
			mexPrintf("Input Argument #0: String with operation: -> Chosen <terminate>.\n");
			mexPrintf("Input Argument #1: Digital filter class handle.\n");
			goto exit_call_error;
		}
		else
		{
			CjvxDigitalFilter* theFilterInst = (CjvxDigitalFilter*)CjvxMatlabToCConverter::jvx_mex_2_intptr(prhs[cntIn]);
			theFilterInst->terminate();

			JVX_DSP_SAFE_ALLOCATE_OBJECT(theFilterInst, CjvxDigitalFilter);
		}
	}
	else if (tokenOper == "process")
	{
		if (!(nrhs = cntIn + 3))
		{
			paramsOk = false;
		}

		if (!paramsOk)
		{
			mexPrintf("Function <jvxIIRDesign> to design filter and return SOS.\n");
			mexPrintf("Input Argument #0: String with operation: -> Chosen <process>.\n");
			mexPrintf("Input Argument #1: Digital filter class handle.\n");
			mexPrintf("Input Argument #2: Input Signal.\n");
			mexPrintf("Input Argument #3: Buffsersize.\n");
			goto exit_call_error;
		}
		else
		{
			CjvxDigitalFilter* theFilterInst = (CjvxDigitalFilter*)CjvxMatlabToCConverter::jvx_mex_2_intptr(prhs[cntIn++]);
			const mxArray* dat = prhs[cntIn++];
			jvxSize cChans = mxGetM(dat);
			jvxSize lSig = mxGetN(dat);
			jvxData* in = (jvxData*)mxGetData(dat);

			jvxSize bSize = 128;
			auto retVal = CjvxMatlabToCConverter::jvx_mex_2_numeric(prhs[cntIn++]);
			if (retVal.size() == 1)
			{
				retVal[0].toContent(&bSize);
			}
			jvxSize nFrames = floor((jvxData)lSig / (jvxData)bSize);
			jvxSize lSig_trunc = nFrames * bSize;

			jvxData** ptrIn = nullptr;
			JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(ptrIn, jvxData, cChans, lSig_trunc);
			jvxData** ptrIn_ptr = nullptr;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrIn_ptr, jvxData*, cChans);
			for (j = 0; j < cChans; j++)
			{
				ptrIn_ptr[j] = ptrIn[j];
			}

			jvxData** ptrOut = nullptr;
			JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(ptrOut, jvxData, cChans, lSig_trunc);
			jvxData** ptrOut_ptr = nullptr;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrOut_ptr, jvxData*, cChans);
			for (j = 0; j < cChans; j++)
			{
				ptrOut_ptr[j] = ptrOut[j];
			}

			std::string errDescr;
			CjvxCToMatlabConverter::convertMexToC(reinterpret_cast<jvxHandle**>(ptrIn), cChans, lSig_trunc, JVX_DATAFORMAT_DATA, dat, "input", false, errDescr);

			theFilterInst->prepare(cChans, bSize);

			for (i = 0; i < nFrames; i++)
			{
				theFilterInst->process((const jvxData**)ptrIn_ptr, ptrOut_ptr, cChans, bSize);
				for (j = 0; j < cChans; j++)
				{
					ptrIn_ptr[j] += bSize;
					ptrOut_ptr[j] += bSize;
				}
			}

			theFilterInst->postprocess();

			if (nlhs > cntOut)
			{
				CjvxCToMatlabConverter::mexReturnGenericNumeric(plhs[cntOut], (const jvxHandle**)ptrOut, cChans, lSig_trunc, JVX_DATAFORMAT_DATA);
				cntOut++;
			}

			JVX_SAFE_DELETE_2DFIELD(ptrIn, cChans);
			JVX_SAFE_DELETE_2DFIELD(ptrOut, cChans);
		}
	}

exit_call_error:

	CjvxCToMatlabConverter::mexFillEmpty(plhs, nlhs, cntOut);
}

