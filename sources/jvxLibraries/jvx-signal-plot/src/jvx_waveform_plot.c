#include "jvx_waveform_plot.h"

typedef struct
{
	jvxWaveformPlot_init initPrm_copy;
	jvxWaveformPlot_async asyncParam_copy;
	jvxData minVal;
	jvxData maxVal;
	jvxData mem;
} jvx_waveform_plot_prv;

jvxDspBaseErrorType jvx_waveform_plot_initCfg(jvxWaveformPlot* hdl)
{
	if (hdl)
	{
		if (hdl->prv == NULL)
		{
			hdl->initPrm.bsize = 1024;
			hdl->initPrm.amplMax = 1.0;
			hdl->initPrm.amplMin = -1.0;
			hdl->asyncParam.gainFactor = 1.0;
			hdl->asyncParam.smoothFactor = 0.0;
			hdl->asyncParam.plotMode = JVX_WAVEFORM_PLOT_AMPL_LINEAR;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_waveform_plot_init(jvxWaveformPlot* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxSize N;
	if (hdl)
	{
		if (hdl->prv == NULL)
		{
			jvx_waveform_plot_prv* newObj = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newObj, jvx_waveform_plot_prv);

			newObj->initPrm_copy = hdl->initPrm;
			newObj->asyncParam_copy = hdl->asyncParam;

			newObj->minVal = 1;
			newObj->maxVal = -1;
			newObj->mem = 0;
			hdl->prv = newObj;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_waveform_plot_terminate(jvxWaveformPlot* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	if (hdl)
	{
		if (hdl->prv)
		{
			jvx_waveform_plot_prv* theObj = (jvx_waveform_plot_prv*)hdl->prv;
			JVX_DSP_SAFE_DELETE_OBJECT(theObj);
			hdl->prv = NULL;

			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_waveform_plot_process(jvxWaveformPlot* hdl, jvxData* ptrShowMe, jvxByte* out_rgb_ptr, jvxSize rgba_vec_length)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxSize i;
	if (hdl)
	{
		if (hdl->prv)
		{
			jvx_waveform_plot_prv* theObj = (jvx_waveform_plot_prv*)hdl->prv;
			jvxSize offset_y = rgba_vec_length / 2;
			jvxSize shiftHeight = rgba_vec_length / 2;
			switch (theObj->asyncParam_copy.plotMode)
			{
			case JVX_WAVEFORM_PLOT_ENERGY_LINEAR:
				offset_y = 0;
				shiftHeight = rgba_vec_length;
				break;
			default:
				// Nothing here
				break;
			}

			for (i = 0; i < theObj->initPrm_copy.bsize; i++)
			{
				jvxData amplVal = ptrShowMe[i];
				amplVal *= theObj->asyncParam_copy.gainFactor;

				switch (theObj->asyncParam_copy.plotMode)
				{
				case JVX_WAVEFORM_PLOT_ENERGY_LINEAR:
				case JVX_WAVEFORM_PLOT_ENERGY_LOG:
					
					amplVal = amplVal * amplVal;
					jvxData fac1 = theObj->asyncParam_copy.smoothFactor;
					jvxData fac2 = (1.0 - theObj->asyncParam_copy.smoothFactor);
					theObj->mem = fac1 * theObj->mem + fac2 * amplVal;
					amplVal = theObj->mem;

					if(theObj->asyncParam_copy.plotMode == JVX_WAVEFORM_PLOT_ENERGY_LOG)
					{
						amplVal = log10(amplVal);
					}
					break;
				default: 
					break;
				}

				if (amplVal > theObj->maxVal)
				{
					theObj->maxVal = amplVal;
				}

				switch (theObj->asyncParam_copy.plotMode)
				{
				case JVX_WAVEFORM_PLOT_ENERGY_LINEAR:				
					
					// Lowest lim is "0"
					theObj->minVal = 0;
					break;
				default:
					if (amplVal < theObj->minVal)
					{
						theObj->minVal = amplVal;
					}
					break;
				}
			}

			if (out_rgb_ptr)
			{
				// Be careful here: values might be lower than 0!!
				int limLow = (int)offset_y +  theObj->minVal * (int)shiftHeight;
				int limHigh = (int)offset_y + theObj->maxVal * (int)shiftHeight;

				// Make suree there is at least 1 pixel
				limLow = JVX_MIN(limLow, (int)rgba_vec_length);
				limLow = JVX_MAX(limLow, 0);

				limHigh = JVX_MAX(limHigh, limLow + 1);

				limHigh = JVX_MIN(limHigh, (int)rgba_vec_length);
				limHigh = JVX_MAX(limHigh, 0);

				for (i = 0; i < limLow; i++)
				{
					out_rgb_ptr[4 * i] = 0x0;
					out_rgb_ptr[4 * i + 1] = 0x0;
					out_rgb_ptr[4 * i + 2] = 0x0;
					out_rgb_ptr[4 * i + 3] = 0x0;
				}
				for (i = limLow; i < limHigh; i++)
				{
					out_rgb_ptr[4 * i] = (jvxByte)0xFF;
					out_rgb_ptr[4 * i + 1] = (jvxByte)0xFF;
					out_rgb_ptr[4 * i + 2] = (jvxByte)0xFF;
					out_rgb_ptr[4 * i + 3] = (jvxByte)0xFF;
				}
				for (i = limHigh; i < rgba_vec_length; i++)
				{
					out_rgb_ptr[4 * i] = 0x00;
					out_rgb_ptr[4 * i + 1] = 0x00;
					out_rgb_ptr[4 * i + 2] = 0x00;
					out_rgb_ptr[4 * i + 3] = 0x00;
				}

				theObj->minVal = theObj->initPrm_copy.amplMax;
				theObj->maxVal = theObj->initPrm_copy.amplMin;
			}

			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_WRONG_STATE;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_waveform_plot_update(jvxWaveformPlot* hdl, jvxUInt16 operation, jvxCBool wantToSet)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	if (hdl)
	{
		if (hdl->prv)
		{
			jvx_waveform_plot_prv* theObj = (jvx_waveform_plot_prv*)hdl->prv;
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
