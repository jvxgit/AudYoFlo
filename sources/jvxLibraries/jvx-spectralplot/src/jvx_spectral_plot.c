#include <float.h>
#include <math.h>
#include "jvx_dsp_base.h"
#include "jvx_circbuffer/jvx_circbuffer.h"
#include "colormap_jet.h"
#include "jvx_spectral_plot.h"

JVX_FFT_TOOLS_DEFINE_FFT_SIZES;

// main struct
typedef struct {

	//fft
	jvxFFTGlobal* fft_gbl;
	jvxFFT** fft_hdl; //per channel
	//sizes
	jvxInt32 fft_size; //must be unsigned
	jvxUInt32 frame_size;
	jvxUInt32 hop_size;
	jvxUInt16 channels;
	//window
	jvx_windowType window_type;
	jvxData* arr_window;
	jvxData win_weight_dB;
	//range in dB
	jvxData ctff_low;
	jvxData ctff_high;
	//buffer
	jvx_circbuffer* buf_circle_in;
	jvxData** buf_work_real;
	jvxDataCplx** buf_work_cmplx;

} jvx_spectralplot_prv;



typedef struct
{
	jvxSpectralPlot_init initPrm;
	jvxSpectralPlot_async asyncParam;

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
			hdl->initPrm.channels = 1;
			hdl->initPrm.fftSize = JVX_FFT_TOOLS_FFT_SIZE_1024;
			hdl->initPrm.highdB = 120;
			hdl->initPrm.lowdB = 0;
			hdl->initPrm.hopSize = 128;
			hdl->initPrm.bufferSize = 1024;
			hdl->initPrm.winType = JVX_WINDOW_HANN;
			hdl->initPrm.secondArg = 0.0;
			hdl->initPrm.thirdArg = 0.0;

			hdl->asyncParam.sensFactor = 1.0;
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

			newObj->initPrm = hdl->initPrm;
			newObj->asyncParam = hdl->asyncParam;

			newObj->derived_fft_size = jvxFFTSize_sizes[newObj->initPrm.fftSize];

			res = jvx_circbuffer_allocate(&newObj->buf_circle_in, newObj->initPrm.bufferSize, 1, 1);

			res = jvx_create_fft_ifft_global(&newObj->fft_gbl, newObj->initPrm.fftSize);
			res = jvx_create_fft_real_2_complex(&newObj->fft_hdl,
				newObj->fft_gbl,
				newObj->initPrm.fftSize,
				&newObj->buf_work_real,
				&newObj->buf_work_cmplx,
				&N,
				JVX_FFT_IFFT_PRESERVE_INPUT,
				NULL,
				NULL,
				0);

			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newObj->arr_window, jvxData, newObj->initPrm.bufferSize);
			res = jvx_compute_window(newObj->arr_window, newObj->initPrm.bufferSize,
				newObj->initPrm.secondArg, newObj->initPrm.thirdArg, newObj->initPrm.winType, NULL);

			/*
			if (res == JVX_DSP_NO_ERROR)
			{
				ptrHdl->window_type = winType;
				weight = 0;
				for (i = 0; i < ptrHdl->frame_size; ++i)
				{
					weight += ptrHdl->arr_window[i] * ptrHdl->arr_window[i];
				}
				ptrHdl->win_weight_dB = 10 * log10(weight);
			}*/
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
			jvx_destroy_fft_ifft_global(theObj->fft_gbl);
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

jvxDspBaseErrorType jvx_spectral_plot_process(jvxSpectralPlot* hdl, jvxData* in, jvxByte* out_rgb_ptr)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxSize i;
	if (hdl)
	{
		if (hdl->prv)
		{
			jvx_spectral_plot_prv* theObj = (jvx_spectral_plot_prv*)hdl->prv;

			res = jvx_circbuffer_write_update_wrap(theObj->buf_circle_in, (const jvxData**) &in, theObj->initPrm.hopSize);

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
				tmp *= theObj->asyncParam.sensFactor;

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
				tmp *= theObj->asyncParam.sensFactor;

				*ptrBuf = tmp;

				ptrBuf++;
				ptrWin++;
				ptrIn++;
			}

			res = jvx_execute_fft(theObj->fft_hdl);

			//calc color plot
			
			for (i = 0; i < theObj->derived_fft_size / 2; ++i)
			{
				jvxData tmp = theObj->buf_work_cmplx[i].re * theObj->buf_work_cmplx[i].re
					+ theObj->buf_work_cmplx[i].im * theObj->buf_work_cmplx[i].im;
				tmp = tmp / (jvxData)theObj->derived_fft_size;
				tmp = JVX_MAX(tmp, DBL_MIN);
				tmp= 10 * log10(tmp);
				tmp = JVX_MIN(tmp, theObj->initPrm.highdB);
				tmp = JVX_MAX(tmp, theObj->initPrm.lowdB);

				tmp = (tmp - theObj->initPrm.lowdB) / (theObj->initPrm.highdB - theObj->initPrm.lowdB);
				jvx_compute_color_map(tmp, (jvxUInt8*)out_rgb_ptr);
				out_rgb_ptr += 4; // RGBA
			}
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}


/*
jvxDspBaseErrorType jvx_spectralplot_init(jvxSpectralPlot** hdl,
	jvxFFTSize fftSize,
	jvxUInt32 inputSize,
	jvxUInt32 hopSize,
	jvxUInt16 channels)
{
	//check parameters
	if (!hdl) return JVX_DSP_ERROR_INTERNAL;
	if (inputSize == 0 || hopSize == 0 || channels == 0 || inputSize < hopSize || inputSize % hopSize != 0)
	{
		*hdl = NULL;
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	//vars
	jvx_spectralplot_prv* ptrHdl = NULL;
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxSize N = 0;
	jvxSize fftSizeArbitrary = 0;
	jvxUInt16 idxChannel = 0;
	jvxUInt32 i = 0;
	
	//handle struct
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(ptrHdl, jvx_spectralplot_prv);
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptrHdl->arr_window, jvxData, inputSize);
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptrHdl->fft_hdl, jvxFFT*, channels);
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptrHdl->buf_work_real, jvxData*, channels);
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptrHdl->buf_work_cmplx, jvxDataCplx*, channels);
	ptrHdl->fft_size = jvxFFTSize_sizes[fftSize];
	ptrHdl->frame_size = inputSize;
	ptrHdl->hop_size = hopSize;
	ptrHdl->channels = channels;
	ptrHdl->ctff_low = 0;
	ptrHdl->ctff_high = 120;
	ptrHdl->buf_circle_in = NULL;
	ptrHdl->fft_gbl = NULL;
	jvx_spectralplot_config_window(ptrHdl, JVX_WINDOW_HANN, 0.0, 0.0);

	//circle buffer
	res = jvx_circbuffer_allocate(&ptrHdl->buf_circle_in, 2 * inputSize - hopSize, 1, channels);
	if (res != JVX_DSP_NO_ERROR)
	{
		//error, clean up
		jvx_spectralplot_terminate(ptrHdl);
		*hdl = NULL;
		return res;
	}
	res = jvx_circbuffer_fill(ptrHdl->buf_circle_in, 0.0, inputSize - hopSize); //set offset between read and write position
	if (res != JVX_DSP_NO_ERROR)
	{
		//error, clean up
		jvx_spectralplot_terminate(ptrHdl);
		*hdl = NULL;
		return res;
	}

	//fft
	//check for arbitrary size
	if (ptrHdl->fft_size == -1)
	{
		fftSizeArbitrary = inputSize;
	}
	res = jvx_create_fft_ifft_global(&ptrHdl->fft_gbl, fftSize);
	if (res != JVX_DSP_NO_ERROR)
	{
		//error, clean up
		jvx_spectralplot_terminate(ptrHdl);
		*hdl = NULL;
		return res;
	}
	for (idxChannel = 0; idxChannel < channels; ++idxChannel)
	{ 
		res = jvx_create_fft_real_2_complex(&ptrHdl->fft_hdl[idxChannel],
			ptrHdl->fft_gbl,
			fftSize,
			&ptrHdl->buf_work_real[idxChannel],
			&ptrHdl->buf_work_cmplx[idxChannel],
			&N,
			JVX_FFT_IFFT_EFFICIENT,
			NULL,
			NULL,
			fftSizeArbitrary);
		if (res != JVX_DSP_NO_ERROR) //maybe check if N == ptrHdl->fft_size?
		{
			//error, clean up
			jvx_spectralplot_terminate(ptrHdl);
			*hdl = NULL;
			return res;
		}
	}

	*hdl = ptrHdl;
	return res;
}

jvxDspBaseErrorType jvx_spectralplot_config_window(jvxSpectralPlot* hdl,
	jvx_windowType winType,
	jvxData secondArg,
	jvxData thirdArg)
{
	//check parameters
	if (!hdl) return JVX_DSP_ERROR_INTERNAL;

	//vars
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvx_spectralplot_prv* ptrHdl = (jvx_spectralplot_prv*)hdl;
	jvxUInt32 i = 0;
	jvxData weight;
	
	res = jvx_compute_window(ptrHdl->arr_window, ptrHdl->frame_size, secondArg,
		thirdArg, ptrHdl->window_type, NULL);
	if (res == JVX_DSP_NO_ERROR)
	{
		ptrHdl->window_type = winType;
		weight = 0;
		for (i = 0; i < ptrHdl->frame_size; ++i)
		{
			weight += ptrHdl->arr_window[i] * ptrHdl->arr_window[i];
		}
		ptrHdl->win_weight_dB = 10 * log10(weight);
	}

	return res;
}

jvxDspBaseErrorType jvx_spectralplot_config_range(jvxSpectralPlot* hdl,
	jvxData low,
	jvxData high)
{
	//check parameters
	if (!hdl) return JVX_DSP_ERROR_INTERNAL;
	if (high <= low) return JVX_DSP_ERROR_INVALID_ARGUMENT;

	//vars
	jvx_spectralplot_prv* ptrHdl = (jvx_spectralplot_prv*)hdl;
	ptrHdl->ctff_low = low;
	ptrHdl->ctff_high = high;

	return JVX_DSP_NO_ERROR;
}

*/
jvxDspBaseErrorType jvx_spectralplot_process(jvxSpectralPlot* hdl,
	jvxData** in,
	jvxUInt8*** out,
	jvxCBool* outofRange)
{
	//check parameters
	if (!hdl || !in || !out) return JVX_DSP_ERROR_INTERNAL;

	//vars
	jvx_spectralplot_prv* ptrHdl = (jvx_spectralplot_prv*)hdl;
	jvxSize fftSize = (ptrHdl->fft_size > 0) ? ptrHdl->fft_size : ptrHdl->frame_size; //check for arbitrary fft size
	jvxSize sizeDiff = fftSize - ptrHdl->frame_size;
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxData workPsd, workPsddB, workPercentage;
	jvxCBool anyValueOutOfRange = c_false;
	jvxUInt16 overlappingsPerFrame = ptrHdl->frame_size / ptrHdl->hop_size;
	jvxUInt16 idxOverlap = 0, idxChannel = 0;
	jvxSize i = 0;

	//copy signal into circle buffer
	res = jvx_circbuffer_write_update(ptrHdl->buf_circle_in, (const jvxData**)in, ptrHdl->frame_size);

	if (res == JVX_DSP_NO_ERROR)
	{
		for (idxOverlap = 0; idxOverlap < overlappingsPerFrame; ++idxOverlap)
		{
			//copy from circle buffer to work buffer and move read position of circle buffer on by hop size afterwards
			res = jvx_circbuffer_read_noupdate(ptrHdl->buf_circle_in, ptrHdl->buf_work_real, ptrHdl->frame_size);
			if (res != JVX_DSP_NO_ERROR) return res;
			res = jvx_circbuffer_remove(ptrHdl->buf_circle_in, ptrHdl->hop_size);
			if (res != JVX_DSP_NO_ERROR) return res;

			//process
			for (idxChannel = 0; idxChannel < ptrHdl->channels; ++idxChannel)
			{
				//zeropadding
				if (sizeDiff > 0)
				{
					memset(ptrHdl->buf_work_real[idxChannel] + ptrHdl->frame_size, 0, sizeof(jvxData) * sizeDiff);
				}
				//windowing, fft
				for (i = 0; i < ptrHdl->frame_size; ++i)
				{
					ptrHdl->buf_work_real[idxChannel][i] *= ptrHdl->arr_window[i];
				}
				res = jvx_execute_fft(ptrHdl->fft_hdl[idxChannel]);
				if (res != JVX_DSP_NO_ERROR) return res;
				//calc color plot
				for (i = 0; i < fftSize / 2; ++i)
				{
					workPsd = ptrHdl->buf_work_cmplx[idxChannel][i].re * ptrHdl->buf_work_cmplx[idxChannel][i].re
						+ ptrHdl->buf_work_cmplx[idxChannel][i].im * ptrHdl->buf_work_cmplx[idxChannel][i].im;
					workPsd = JVX_MAX(workPsd, DBL_MIN);
					workPsddB = 10 * log10(workPsd) - ptrHdl->win_weight_dB;
					if (workPsd > ptrHdl->ctff_high || workPsd < ptrHdl->ctff_low)
					{
						anyValueOutOfRange = c_true;
					}
					workPsddB = JVX_MAX(JVX_MIN(workPsddB, ptrHdl->ctff_high), ptrHdl->ctff_low);
					workPercentage = (workPsddB - ptrHdl->ctff_low) / (ptrHdl->ctff_high - ptrHdl->ctff_low);
					jvx_compute_color_map(workPercentage, &out[idxChannel][idxOverlap][3 * i]);
				}
			}
		}
	}

	//return
	if (outofRange)
	{
		*outofRange = anyValueOutOfRange;
	}
	return res;
}

jvxDspBaseErrorType jvx_spectralplot_terminate(jvxSpectralPlot* hdl)
{
	if (!hdl) return JVX_DSP_ERROR_INTERNAL;

	//vars
	jvx_spectralplot_prv* ptrHdl = (jvx_spectralplot_prv*)hdl;
	jvxUInt16 idxChannel = 0;
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;

	if (ptrHdl->fft_hdl)
	{
		for (idxChannel = 0; idxChannel < ptrHdl->channels; ++idxChannel)
		{
			if (ptrHdl->fft_hdl[idxChannel])
			{
				res = jvx_destroy_fft(ptrHdl->fft_hdl[idxChannel]);
				if (res != JVX_DSP_NO_ERROR) return res;
				ptrHdl->fft_hdl[idxChannel] = NULL;
			}
		}
		JVX_DSP_SAFE_DELETE_FIELD(ptrHdl->fft_hdl);
	}
	if (ptrHdl->fft_gbl)
	{
		res = jvx_destroy_fft_ifft_global(ptrHdl->fft_gbl);
		if (res != JVX_DSP_NO_ERROR) return res;
		ptrHdl->fft_gbl = NULL;
	}
	if (ptrHdl->buf_circle_in)
	{
		res = jvx_circbuffer_deallocate(ptrHdl->buf_circle_in);
		if (res != JVX_DSP_NO_ERROR) return res;
		ptrHdl->buf_circle_in = NULL;
	}
	if (ptrHdl->buf_work_cmplx) JVX_DSP_SAFE_DELETE_FIELD(ptrHdl->buf_work_cmplx);
	if (ptrHdl->buf_work_real) JVX_DSP_SAFE_DELETE_FIELD(ptrHdl->buf_work_real);
	if (ptrHdl->arr_window) JVX_DSP_SAFE_DELETE_FIELD(ptrHdl->arr_window);
	JVX_DSP_SAFE_DELETE_OBJECT(ptrHdl);

	return res;
}
