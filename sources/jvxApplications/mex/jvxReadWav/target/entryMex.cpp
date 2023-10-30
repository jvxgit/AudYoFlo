#include "jvx.h"
#include "jvxFileReader.h"

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
	jvxSize i,ix,iy;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	const mxArray* arrIn = NULL;
	jvxSize rate = 0;

	jvxBool paramsOk = true;
	if (!
		((nrhs >= 1) && (nrhs <= 1)))
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

	if(paramsOk)
	{
		std::string fName;
		if (nrhs >= 1)
		{
			jvxErrorType res = mexArgument2String(fName, prhs, 0, 1);
			if(res == JVX_NO_ERROR)
			{
				jvxFileReader fReader;
				jvxEndpointClass epCls = JVX_FILETYPE_WAV;
				jvxFileDescriptionEndpoint_open fileDescrO;				
				res = fReader.activate(fName, epCls, &fileDescrO);
				if (res == JVX_NO_ERROR)
				{
					jvxFileDescriptionEndpoint_prepare fileDescrP;
					fileDescrP.dFormatAppl = JVX_DATAFORMAT_DATA;
					fileDescrP.numSamplesFrameMax_moveBgrd = 8192;
					res = fReader.prepare(8192, JVX_FILECONTINUE_STOPEND, false, &fileDescrP, jvxFileOperationType::JVX_FILEOPERATION_BATCH);
					if (res == JVX_NO_ERROR)
					{
						fReader.start();

						dims[0] = fileDescrO.numberChannels;
						dims[1] = fileDescrO.lengthFile;

						plhs[0] = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxREAL);
						jvxData* ptrWrite = (jvxData*)mxGetData(plhs[0]);
						jvxData** bufsChannels = nullptr;
						jvxSize copybufSize = 128;
						jvxSize offsX = 0;
						jvxSize toCopy = 0;
						JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bufsChannels, jvxData*, dims[0]);
						for (i = 0; i < dims[0]; i++)
						{
							JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bufsChannels[i], jvxData, copybufSize);
						}
						jvxSize numFrames = floor((jvxData)fileDescrO.lengthFile / (jvxData)copybufSize);
						for (i = 0; i < numFrames; i++)
						{
							toCopy = copybufSize;
							fReader.audio_buffer(reinterpret_cast<jvxHandle**>(bufsChannels), fileDescrO.numberChannels, toCopy, true, nullptr);
							/* Copy in correct way*/
							/* Copy in correct way*/
							for (ix = 0; ix < toCopy; ix++)
							{
								for (iy = 0; iy < fileDescrO.numberChannels; iy++)
								{
									jvxSize ixx = ix + offsX;
									ptrWrite[ixx * fileDescrO.numberChannels + iy] = ((jvxData*)bufsChannels[iy])[ix];
								}
							}
							offsX += toCopy;
						}

						toCopy = fileDescrO.lengthFile - numFrames * copybufSize;
						if (toCopy)
						{
							fReader.audio_buffer(reinterpret_cast<jvxHandle**>(bufsChannels), fileDescrO.numberChannels, toCopy, true, nullptr);
							
							/* Copy in correct way*/
							for (ix = 0; ix < toCopy; ix++)
							{
								for(iy = 0; iy < fileDescrO.numberChannels; iy++)
								{
									jvxSize ixx = ix + offsX;
									ptrWrite[ixx * fileDescrO.numberChannels + iy] = ((jvxData*)bufsChannels[iy])[ix];
								}
							}							
						}
						
						if (nlhs >= 2)
						{
							CjvxCToMatlabConverter::mexReturnInt32(plhs[1], fileDescrO.samplerate);
						}
						for (i = 0; i < dims[0]; i++)
						{
							JVX_DSP_SAFE_DELETE_FIELD(bufsChannels[i]);
						}
						JVX_DSP_SAFE_DELETE_FIELD(bufsChannels);

						fReader.stop();
						fReader.postprocess();
					}
					fReader.deactivate();
				}
			}
		}
		/*if (
			(lenBufStates > 0) &&
			(lenSig > 0))
		{
			// Now here we go
			jvx_circbuffer* theCircBuffer = NULL;
			jvx_circbuffer_allocate_1chan(&theCircBuffer, lenBufStates);
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
				memcpy(theCircBuffer->ram.field[0], cpFrom, sizeof(jvxData) * num);
			}

			jvxData* in = (jvxData*)mxGetData(arrIn);
			jvx_circbuffer_iir_1can_1io(theCircBuffer, fCoeffsNum, fCoeffsDen, &in, lenSig);

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
			JVX_DSP_SAFE_DELETE_FIELD(fCoeffsDen);

		} // if ((lenBufStates > 0) &&(lenSig > 0))
		else
		{
			mexWarnMsgTxt("Input arguments must be fields of jvxData to hold the input signal and at least numerator or denominator!");
		}*/
		return;
	}
	mexWarnMsgTxt("This function requires 1-4 input and 1-2 output arguments!");
}

