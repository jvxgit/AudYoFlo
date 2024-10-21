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

#include "CjvxCToMatlabConverter.h"

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
	jvxSize i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	mxArray* arr = NULL;
	jvxFFTSize fftSize = JVX_FFT_TOOLS_FFT_SIZE_16;
	jvxFFTGlobal* glob_fft = nullptr;
	jvx_circbuffer_fft* sig = nullptr;
	jvxDataCplx* datCplx = nullptr;
	jvxSize lengthCplx = 0;

	if((nrhs == 2)&&(nlhs == 1))
	{
		if(mxIsData(prhs[0]) && mxIsData(prhs[1]))
		{
			jvxData* fldDataFrom0 = (jvxData*)mxGetData(prhs[0]);
			jvxData* fldDataFrom1 = (jvxData*)mxGetData(prhs[1]);
			jvxSize num0 = mxGetNumberOfElements(prhs[0]);
			jvxSize num1 = mxGetNumberOfElements(prhs[1]);

			num0 = JVX_MIN(num0, num1);
			jvxSize fftNum = (num0 - 1) * 2;
			
			switch (fftNum)
			{
			case 16:
				fftSize = JVX_FFT_TOOLS_FFT_SIZE_16;
				break;
			case 32:
				fftSize = JVX_FFT_TOOLS_FFT_SIZE_32;
				break;
			case 64:
				fftSize = JVX_FFT_TOOLS_FFT_SIZE_64;
				break;
			case 128:
				fftSize = JVX_FFT_TOOLS_FFT_SIZE_128;
				break;
			case 256:
				fftSize = JVX_FFT_TOOLS_FFT_SIZE_256;
				break;
			case 512:
				fftSize = JVX_FFT_TOOLS_FFT_SIZE_512;
				break;
			case 1024:
				fftSize = JVX_FFT_TOOLS_FFT_SIZE_1024;
				break;
			case 2048:
				fftSize = JVX_FFT_TOOLS_FFT_SIZE_2048;
				break;
			case 4096:
				fftSize = JVX_FFT_TOOLS_FFT_SIZE_4096;
				break;
			case 8192:
				fftSize = JVX_FFT_TOOLS_FFT_SIZE_8192;
				break;
			default:
				mexWarnMsgTxt("Function only supports a specific vector size (16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192)");
				return;
			}

			jvxData fNorm = 1.0 / (jvxData)fftNum;
			jvx_circbuffer_allocate_global_fft_ifft(&glob_fft, fftSize);
			jvx_circbuffer_allocate_fft_ifft(
				&sig, glob_fft, JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL, fftSize, false, 1);
			jvx_circbuffer_access_cplx_fft_ifft(sig, &datCplx, &lengthCplx, 0);
			for (i = 0; i < lengthCplx; i++)
			{
				datCplx[i].re = fldDataFrom0[i] * fNorm;
				datCplx[i].im = fldDataFrom1[i] * fNorm;
			}
			jvx_circbuffer_process_fft_ifft(sig);

			dims[1] = (SZ_MAT_TYPE)sig->circBuffer.length;
			arr = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
			jvxData* ptr = (jvxData*)mxGetData(arr);
			jvx_circbuffer_read_update(&sig->circBuffer, &ptr, sig->circBuffer.length);
			plhs[0] = arr;

			jvx_circbuffer_deallocate_fft_ifft(sig);
			sig = nullptr;
			jvx_circbuffer_destroy_global_fft_ifft(glob_fft);
			glob_fft = nullptr;
		}
		else
		{ 
			mexWarnMsgTxt("Input arguments must be jvxData!");
		}
		return;
	}
	mexWarnMsgTxt("This function requires 1 input and 2 output arguments!");
}

