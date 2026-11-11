#include <float.h>
#include <math.h>
#include "jvx_dsp_base.h"
#include "jvx_circbuffer/jvx_circbuffer.h"
#include "colormap_jet.h"
#include "jvx_spectral_plot.h"

JVX_FFT_TOOLS_DEFINE_FFT_SIZES;


typedef struct
{
	jvxSpectralPlot_init initParam_copy;
	jvxSpectralPlot_async asyncParam_copy;

	jvxSize derived_fft_size;
	jvxFFTGlobal* fft_gbl;
	jvxFFT* fft_hdl; //per channel
	jvx_circbuffer* buf_circle_in;

	jvxData* buf_work_real;
	jvxDataCplx* buf_work_cmplx;

	jvxData* arr_window;

} jvx_spectral_plot_prv;

jvxDspBaseErrorType jvx_spectral_plot_initCfg(jvxSpectralPlot* hdl)
{
	if (hdl)
	{
		if (hdl->prv == NULL)
		{
			//hdl->initPrm.bufferSize = 1024;
			hdl->initParam.channels = 1;
			hdl->initParam.fftSize = JVX_FFT_TOOLS_FFT_SIZE_1024;
			hdl->initParam.hopSize = 128;
			hdl->initParam.bufferSize = 1024;
			hdl->initParam.winType = JVX_WINDOW_HANN;
			hdl->initParam.secondArg = 0.0;
			hdl->initParam.thirdArg = 0.0;

			hdl->asyncParam.sensFactor = 1.0;
			hdl->asyncParam.highdB = 120;
			hdl->asyncParam.lowdB = 0;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_spectral_plot_init(jvxSpectralPlot* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxSize N;
	if (hdl)
	{
		if (hdl->prv == NULL)
		{
			jvx_spectral_plot_prv* newObj = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newObj, jvx_spectral_plot_prv);

			newObj->initParam_copy = hdl->initParam;
			newObj->asyncParam_copy = hdl->asyncParam;

			newObj->derived_fft_size = jvxFFTSize_sizes[newObj->initParam_copy.fftSize];

			res = jvx_circbuffer_allocate(&newObj->buf_circle_in, newObj->initParam_copy.bufferSize, 1, 1);

			res = jvx_create_fft_ifft_global(&newObj->fft_gbl, newObj->initParam_copy.fftSize, NULL JVX_FFT_GLOBAL_CONFIG_ADD_ARGUMENT_CALL);
			res = jvx_create_fft_real_2_complex(&newObj->fft_hdl,
				newObj->fft_gbl,
				newObj->initParam_copy.fftSize,
				&newObj->buf_work_real,
				&newObj->buf_work_cmplx,
				&N,
				JVX_FFT_IFFT_PRESERVE_INPUT,
				NULL,
				NULL,
				0);

			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newObj->arr_window, jvxData, newObj->initParam_copy.bufferSize);
			res = jvx_compute_window(newObj->arr_window, newObj->initParam_copy.bufferSize,
				newObj->initParam_copy.secondArg, newObj->initParam_copy.thirdArg, newObj->initParam_copy.winType, NULL);
			
			hdl->prv = newObj;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_spectral_plot_terminate(jvxSpectralPlot* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	if (hdl)
	{
		if (hdl->prv)
		{
			jvx_spectral_plot_prv* theObj = (jvx_spectral_plot_prv*)hdl->prv;
			
			JVX_DSP_SAFE_DELETE_FIELD(theObj->arr_window);
			res = jvx_destroy_fft(theObj->fft_hdl);
			theObj->fft_hdl = NULL;
			jvx_destroy_fft_ifft_global(theObj->fft_gbl, NULL);
			theObj->fft_gbl = NULL;
			jvx_circbuffer_deallocate(theObj->buf_circle_in);
			theObj->buf_circle_in = NULL;

			JVX_DSP_SAFE_DELETE_OBJECT(theObj);
			hdl->prv = NULL;

			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_spectral_plot_process(jvxSpectralPlot* hdl, jvxData* in, jvxByte* out_rgb_ptr, jvxSize rgba_vec_length)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxSize i;
	if (hdl)
	{
		if (hdl->prv)
		{
			jvx_spectral_plot_prv* theObj = (jvx_spectral_plot_prv*)hdl->prv;

			res = jvx_circbuffer_write_update_wrap(theObj->buf_circle_in, (const jvxData**) &in, theObj->initParam_copy.hopSize);

			if (out_rgb_ptr)
			{
				jvxSize ll1, ll2;
				jvxSize idxWrite = (theObj->buf_circle_in->idxRead + theObj->buf_circle_in->fHeight) % theObj->buf_circle_in->length;
				ll1 = theObj->buf_circle_in->length - idxWrite;
				ll2 = theObj->buf_circle_in->length - ll1;
				jvxData* ptrIn = theObj->buf_circle_in->ram.field[0];
				ptrIn += idxWrite;

				jvxData* ptrWin = theObj->arr_window;
				jvxData* ptrBuf = theObj->buf_work_real;
				for (i = 0; i < ll1; i++)
				{
					jvxData tmp = *ptrIn;
					tmp *= *ptrWin;
					tmp *= theObj->asyncParam_copy.sensFactor;

					*ptrBuf = tmp;

					ptrBuf++;
					ptrWin++;
					ptrIn++;
				}
				ptrIn = theObj->buf_circle_in->ram.field[0];
				for (i = 0; i < ll2; i++)
				{
					jvxData tmp = *ptrIn;
					tmp *= *ptrWin;
					tmp *= theObj->asyncParam_copy.sensFactor;

					*ptrBuf = tmp;

					ptrBuf++;
					ptrWin++;
					ptrIn++;
				}

				res = jvx_execute_fft(theObj->fft_hdl);

				//calc color plot
				jvxSize cpMin = JVX_MIN(rgba_vec_length, theObj->derived_fft_size / 2);
				for (i = 0; i < cpMin /*theObj->derived_fft_size / 2*/; i++)
				{
					jvxData tmp = theObj->buf_work_cmplx[i].re * theObj->buf_work_cmplx[i].re
						+ theObj->buf_work_cmplx[i].im * theObj->buf_work_cmplx[i].im;
					tmp = tmp / (jvxData)theObj->derived_fft_size;
					tmp = JVX_MAX(tmp, DBL_MIN);
					tmp = 10 * log10(tmp);
					tmp = JVX_MIN(tmp, theObj->asyncParam_copy.highdB);
					tmp = JVX_MAX(tmp, theObj->asyncParam_copy.lowdB);

					tmp = (tmp - theObj->asyncParam_copy.lowdB) / (theObj->asyncParam_copy.highdB - theObj->asyncParam_copy.lowdB);
					jvx_compute_color_map(tmp, (jvxUInt8*)out_rgb_ptr);
					out_rgb_ptr += 4; // RGBA
				}
			}
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_spectral_plot_update(jvxSpectralPlot* hdl, jvxUInt16 operation, jvxCBool wantToSet)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	if (hdl)
	{
		if (hdl->prv)
		{
			jvx_spectral_plot_prv* theObj = (jvx_spectral_plot_prv*)hdl->prv;
			if (operation & JVX_DSP_UPDATE_ASYNC)
			{
				if (wantToSet)
				{
					theObj->asyncParam_copy = hdl->asyncParam;
				}
				else
				{
					hdl->asyncParam = theObj->asyncParam_copy;
				}
			}
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}
