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
	mxArray* arrRe = NULL;
	mxArray* arrIm = NULL;
	jvxFFTSize fftSize = JVX_FFT_TOOLS_FFT_SIZE_16;
	jvxFFTGlobal* glob_fft = nullptr;
	jvx_circbuffer_fft* sig = nullptr;
	jvxDataCplx* datCplx = nullptr;
	jvxSize lenCplx = 0;

	if((nrhs == 1)&&(nlhs == 2))
	{
		if(mxIsData(prhs[0]))
		{
			jvxData* fldDataFrom = (jvxData*)mxGetData(prhs[0]);
			jvxSize num = mxGetNumberOfElements(prhs[0]);

			switch (num)
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

			jvx_circbuffer_allocate_global_fft_ifft(&glob_fft, fftSize);
			jvx_circbuffer_allocate_fft_ifft(
				&sig, glob_fft, JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX, fftSize, false, 1);
			jvx_circbuffer_access_cplx_fft_ifft(sig, &datCplx, &lenCplx, 0);
			jvx_circbuffer_write_update(&sig->circBuffer, &fldDataFrom, num);
			jvx_circbuffer_process_fft_ifft(sig);

			dims[1] = (SZ_MAT_TYPE)lenCplx;
			arrRe = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
			arrIm = mxCreateNumericArray(ndim, dims, mxDATA_CLASS, mxREAL);
			jvxData* ptrRe = (jvxData*)mxGetData(arrRe);
			jvxData* ptrIm = (jvxData*)mxGetData(arrIm);
			for (i = 0; i < lenCplx; i++)
			{
				ptrRe[i] = datCplx[i].re;
				ptrIm[i] = datCplx[i].im;
			}
			plhs[0] = arrRe;
			plhs[1] = arrIm;

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

