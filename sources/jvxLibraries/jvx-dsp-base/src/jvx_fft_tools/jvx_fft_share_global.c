#include "jvx_fft_tools/jvx_fft_share_global.h"

struct jvxGlobalFFTInstance
{
	jvxFFTGlobal* common_global;
	jvxSize refCnt;
};

struct jvxGlobalFFTInstance theGlobalFftRef = { 0 };

jvxErrorType jvx_create_or_request_fft_global(jvxFFTGlobal** hdlOnReturn, jvxFFTSize szFft, jvxCBool* wasCreated)
{
	// What to do with JVX_FFT_TOOLS_FFT_SIZE_8192?
	if (theGlobalFftRef.common_global)
	{
		jvxFFTSize fftSizeGlobal = JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE;
		fftSizeGlobal = jvx_fft_ifft_global_max_fft_size(theGlobalFftRef.common_global);
		if (fftSizeGlobal >= szFft)
		{
			if (hdlOnReturn) *hdlOnReturn = theGlobalFftRef.common_global;
			theGlobalFftRef.refCnt++;

			if (wasCreated)
			{
				*wasCreated = c_false;
			}
			return JVX_DSP_NO_ERROR;
		}

		// Means: we can not reuse the fft global handle!
		if (hdlOnReturn) *hdlOnReturn = NULL;
		return JVX_DSP_ERROR_INVALID_SETTING;
	}
	jvx_create_fft_ifft_global(&theGlobalFftRef.common_global, szFft, NULL JVX_FFT_GLOBAL_CONFIG_ADD_ARGUMENT_CALL);
	theGlobalFftRef.refCnt = 1;
	if (hdlOnReturn) *hdlOnReturn = theGlobalFftRef.common_global;
	if (wasCreated) *wasCreated = c_true;
	return JVX_DSP_NO_ERROR;
}

jvxErrorType jvx_return_or_destroy_fft_global(jvxFFTGlobal* hdlToReturn, jvxCBool* wasDestroyed)
{
	if (theGlobalFftRef.common_global)
	{
		if (theGlobalFftRef.common_global == hdlToReturn)
		{
			if (theGlobalFftRef.refCnt > 1)
			{
				theGlobalFftRef.refCnt--;
			}
			else
			{
				jvx_destroy_fft_ifft_global(theGlobalFftRef.common_global, NULL);
				theGlobalFftRef.refCnt = 0;
				theGlobalFftRef.common_global = NULL;
				if (wasDestroyed)
				{
					*wasDestroyed = c_false;
				}
			}
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}
