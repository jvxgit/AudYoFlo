#include "jvx_dsp_base.h"
#include "jvx_fft_processing/jvx_fft_processing.h"
#include <assert.h>

typedef struct
{
	jvxFFTframework fftFramework;
	jvxFFTframeworkAnalysis fftFrameworkAnalysis;
	jvxFFTframeworkWeightCorrection fftWeightCorrection;
	jvxDataCplx* gains_buffer_last;

	jvx_fft_signal_processing_prmInit cfg_cpy;
	jvx_fft_signal_processing_prmDerived derived_cpy;
	jvx_fft_signal_processing_prmAsync async_cpy;
	jvx_fft_signal_processing_prmSync sync_cpy;
} jvx_fft_signal_processing_prv;

// =====================================================================================
static void
jvx_reset_derived(jvx_fft_signal_processing_prmDerived* derived)
{
	derived->outFftBuffer = NULL;
	derived->outFftBufferAnalysis = NULL;
	derived->weight_correction_in = NULL;
	derived->weight_correction_out = NULL;
	derived->desiredNumFilterCoeffs = 0;
	derived->fftsize = 0;
	derived->numSpectrumCoeffs = 0;
}

// ======================================================================================

void
jvx_fft_processing_updatewindow(jvx_fft_signal_processing_prv* H)
{
	if (H->fftFrameworkAnalysis.window)
	{
		// Reset memory for memory, fill with selected window type
		memset(H->fftFrameworkAnalysis.window, 0, sizeof(jvxData)*H->derived_cpy.fftsize);

		// Insert the window function here..
		//hdl->fftFrameworkAnalysis.numSamplesValidWindow = hdl->intern_use.hdlParams_in->window_size_fft_analysis;
		H->fftFrameworkAnalysis.numSamplesValidWindow =
			JVX_DATA2INT32(H->sync_cpy.percentAnalysisWinFftSize * H->derived_cpy.fftsize * 0.01);
		H->fftFrameworkAnalysis.numSamplesValidWindow = JVX_MIN(H->derived_cpy.fftsize, H->fftFrameworkAnalysis.numSamplesValidWindow);
		H->fftFrameworkAnalysis.numSamplesValidWindow = JVX_MAX(1, H->fftFrameworkAnalysis.numSamplesValidWindow);

		if (H->fftFrameworkAnalysis.numSamplesValidWindow % 2)
		{
			H->fftFrameworkAnalysis.numSamplesValidWindow++;
		}

		H->fftFrameworkAnalysis.window_ptr =
			H->fftFrameworkAnalysis.window + H->derived_cpy.fftsize - H->fftFrameworkAnalysis.numSamplesValidWindow;


		jvx_compute_window(H->fftFrameworkAnalysis.window_ptr,
			H->fftFrameworkAnalysis.numSamplesValidWindow,
			H->fftFrameworkAnalysis.numSamplesValidWindow / 2,
			0,
			H->sync_cpy.wintype,
			NULL);
	}
}

// ==============================================================================================

jvxDspBaseErrorType jvx_fft_signal_processing_initCfg(jvx_fft_signal_processing* signal_proc)
{
	signal_proc->cfg.fftType = JVX_FFT_TOOLS_FFT_SIZE_1024;
	signal_proc->cfg.fftFrameworkType = JVX_FFT_TOOLS_FWK_OLS_CF;
	signal_proc->cfg.hopsize = 256;
	signal_proc->cfg.buffersize = 512;
	signal_proc->cfg.ifcf_length_max = 0;
	signal_proc->cfg.ir_length_max = 1024;
	signal_proc->cfg.desiredWindowTypeWeightCorrection = JVX_WINDOW_HAMMING;
	signal_proc->cfg.allocateAnalysisFft = false;
	signal_proc->cfg.allocateProcessingFft = true;
	signal_proc->cfg.allocateWeightCorrection = false;

	signal_proc->async.ifcf_length = signal_proc->cfg.ifcf_length_max;
	signal_proc->async.auto_aliasing = true;
	signal_proc->async.employ_antialias_weight_processing = true;

	signal_proc->sync.percentAnalysisWinFftSize = 75;
	signal_proc->sync.wintype = JVX_WINDOW_FLATTOP_HANN;

	jvx_reset_derived(&signal_proc->derived);
	
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_fft_signal_processing_updateCfg(jvx_fft_signal_processing_prmDerived* H, jvx_fft_signal_processing_prmInit* cfg)
{
	H->fftsize = jvx_get_fft_size(cfg->fftType);
	H->numSpectrumCoeffs = H->fftsize / 2 + 1;
	H->desiredNumFilterCoeffs =
		jvx_fft_tools_compute_num_filter_coeffs_frameworks(H->fftsize,
			cfg->hopsize,
			cfg->buffersize,
			cfg->fftFrameworkType,
			cfg->ifcf_length_max);
	H->desiredNumFilterCoeffs = JVX_MIN(H->desiredNumFilterCoeffs, cfg->ir_length_max);
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fft_signal_processing_update(jvx_fft_signal_processing* signal_proc, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	jvx_fft_signal_processing_prv* H = NULL;

	if (signal_proc->prv != NULL)
	{
		jvx_fft_signal_processing_prv* H = (jvx_fft_signal_processing_prv*)signal_proc->prv;
		switch (whatToUpdate)
		{
		case JVX_DSP_UPDATE_ASYNC:
			if (do_set)
			{
				H->async_cpy = signal_proc->async;
			}
			else
			{
				signal_proc->async = H->async_cpy;
			}
			break;

		case JVX_DSP_UPDATE_SYNC:
			if (do_set)
			{
				H->sync_cpy = signal_proc->sync;
				jvx_fft_processing_updatewindow(H);
				H->sync_cpy.percentAnalysisWinFftSize =
					JVX_DATA2INT32((jvxData)H->fftFrameworkAnalysis.numSamplesValidWindow /(jvxData)H->derived_cpy.fftsize * 100.0);
			}
			else
			{
				signal_proc->sync = H->sync_cpy;
			}
			break;
		default:
			assert(0);
			break;
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType 
jvx_fft_signal_processing_prepare(jvx_fft_signal_processing* signal_proc, jvxHandle* global_hdl)
{
	jvxDspBaseErrorType err = JVX_DSP_NO_ERROR;
	jvxInt32 dummy1, dummy2;
	
	jvx_fft_signal_processing_prv* H = NULL;

	if (signal_proc->prv != NULL)
	{
		return JVX_DSP_ERROR_WRONG_STATE;
	}

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(H, jvx_fft_signal_processing_prv);
	signal_proc->prv = H;

	H->cfg_cpy = signal_proc->cfg;

	// Replace the following t2 by an update call
	jvx_fft_signal_processing_update(signal_proc, JVX_DSP_UPDATE_ASYNC, true);
	//H->async_cpy = signal_proc->async;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Derive some parameters
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	jvx_fft_signal_processing_updateCfg(&H->derived_cpy, &H->cfg_cpy);

	if (H->cfg_cpy.allocateAnalysisFft)
	{
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Allocate everything for ANALYSIS FFT
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		err = jvx_create_fft_real_2_complex(&H->fftFrameworkAnalysis.hdlFft,
			global_hdl, (jvxFFTSize)H->cfg_cpy.fftType, NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT,
			NULL, NULL, 0);

		jvx_fft_tools_init_framework_fft_analysis(
			&H->fftFrameworkAnalysis.hdlFftFwk,
			(jvxInt32) H->cfg_cpy.hopsize,
			H->fftFrameworkAnalysis.hdlFft,
			&H->fftFrameworkAnalysis.inputFFT,
			&H->fftFrameworkAnalysis.outputFFT,
			&dummy1, &dummy2);

		// Make sure, result is ok
		assert(dummy1 == H->cfg_cpy.hopsize);
		assert(dummy2 == H->derived_cpy.numSpectrumCoeffs);

		// Allocate memory for memory, fill with selected window type
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(H->fftFrameworkAnalysis.window, jvxData, H->derived_cpy.fftsize);
		memset(H->fftFrameworkAnalysis.window, 0, sizeof(jvxData)*H->derived_cpy.fftsize);

		H->derived_cpy.outFftBufferAnalysis = H->fftFrameworkAnalysis.outputFFT;

	}

	if (H->cfg_cpy.allocateProcessingFft)
	{

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Allocate everything for signal processing FFT
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		switch (H->cfg_cpy.fftFrameworkType)
		{
		case JVX_FFT_TOOLS_FWK_DW_OLA:

			// Allocate FFT kernel object
			err = jvx_create_fft_real_2_complex(&H->fftFramework.hdlFft,
				global_hdl, (jvxFFTSize)H->cfg_cpy.fftType, NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);

			assert(err == JVX_DSP_NO_ERROR);

			// Allocate IFFT kernel object
			err = jvx_create_ifft_complex_2_real(&H->fftFramework.hdlIfft, global_hdl,
				(jvxFFTSize)H->cfg_cpy.fftType, NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
			assert(err == JVX_DSP_NO_ERROR);

			// Allocate framework
			err = jvx_fft_tools_init_framework_dw_ola(
				&H->fftFramework.hdlFftFwk, &H->fftFramework.hdlIfftFwk,
				(jvxInt32)H->cfg_cpy.hopsize, H->cfg_cpy.buffersize,
				H->fftFramework.hdlFft, H->fftFramework.hdlIfft,
				&H->fftFramework.inputFFT, &H->fftFramework.outputIFFT,
				&H->fftFramework.inputIFFT_newWeight, &H->fftFramework.outputFFT,
				&dummy1, &dummy2, &H->fftFramework.delay);
			assert(err == JVX_DSP_NO_ERROR);

			H->fftFramework.inputIFFT_oldWeight = NULL;

			// Make sure, result is ok
			assert(dummy1 == H->cfg_cpy.hopsize);
			assert(dummy2 == H->derived_cpy.numSpectrumCoeffs);

			break;
		case JVX_FFT_TOOLS_FWK_SW_OLA:
			// Allocate FFT kernel object
			err = jvx_create_fft_real_2_complex(
				&H->fftFramework.hdlFft,
				global_hdl, (jvxFFTSize)H->cfg_cpy.fftType, NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT,
				NULL, NULL, 0);
			assert(err == JVX_DSP_NO_ERROR);

			// Allocate IFFT kernel object
			err = jvx_create_ifft_complex_2_real(
				&H->fftFramework.hdlIfft, global_hdl, (jvxFFTSize)H->cfg_cpy.fftType,
				NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT,
				NULL, NULL, 0);
			assert(err == JVX_DSP_NO_ERROR);


			// Allocate framework
			jvx_fft_tools_init_framework_sw_ola(
				&H->fftFramework.hdlFftFwk, &H->fftFramework.hdlIfftFwk,
				(jvxInt32)H->cfg_cpy.hopsize, H->cfg_cpy.buffersize,
				H->fftFramework.hdlFft, H->fftFramework.hdlIfft,
				&H->fftFramework.inputFFT, &H->fftFramework.outputIFFT,
				&H->fftFramework.inputIFFT_newWeight, &H->fftFramework.outputFFT,
				&dummy1, &dummy2, &H->fftFramework.delay);
			assert(err == JVX_DSP_NO_ERROR);

			H->fftFramework.inputIFFT_oldWeight = NULL;

			// Make sure, result is ok
			assert(dummy1 == H->cfg_cpy.hopsize);
			assert(dummy2 == H->derived_cpy.numSpectrumCoeffs);

			break;

		case JVX_FFT_TOOLS_FWK_OLA_CF:

			// Allocate FFT kernel object
			err = jvx_create_fft_real_2_complex(&H->fftFramework.hdlFft, global_hdl, (jvxFFTSize)H->cfg_cpy.fftType,
				NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
			assert(err == JVX_DSP_NO_ERROR);

			// Allocate IFFT kernel object
			jvx_create_ifft_complex_2_complex(
				&H->fftFramework.hdlIfft, global_hdl, (jvxFFTSize)H->cfg_cpy.fftType,
				NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
			assert(err == JVX_DSP_NO_ERROR);

			// Allocate framework
			err = jvx_fft_tools_init_framework_ola_cf(
				&H->fftFramework.hdlFftFwk, &H->fftFramework.hdlIfftFwk,
				(jvxInt32)H->cfg_cpy.hopsize,
				H->fftFramework.hdlFft, H->fftFramework.hdlIfft,
				&H->fftFramework.inputFFT, &H->fftFramework.outputIFFT,
				&H->fftFramework.inputIFFT_newWeight, &H->fftFramework.inputIFFT_oldWeight,
				&H->fftFramework.outputFFT, &dummy1, &dummy2, &H->fftFramework.delay);
			assert(err == JVX_DSP_NO_ERROR);

			// Make sure, result is ok
			assert(dummy1 == H->cfg_cpy.hopsize);
			assert(dummy2 == H->derived_cpy.numSpectrumCoeffs);

			break;
		case JVX_FFT_TOOLS_FWK_OLS:
			// Allocate FFT kernel object
			err = jvx_create_fft_real_2_complex(&H->fftFramework.hdlFft, global_hdl,
				(jvxFFTSize)H->cfg_cpy.fftType, NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
			assert(err == JVX_DSP_NO_ERROR);

			// Allocate IFFT kernel object
			err = jvx_create_ifft_complex_2_real(&H->fftFramework.hdlIfft, global_hdl,
				(jvxFFTSize)H->cfg_cpy.fftType, NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
			assert(err == JVX_DSP_NO_ERROR);

			// Allocate framework
			err = jvx_fft_tools_init_framework_ols(
				&H->fftFramework.hdlFftFwk, &H->fftFramework.hdlIfftFwk,
				(jvxInt32)H->cfg_cpy.hopsize,
				H->fftFramework.hdlFft, H->fftFramework.hdlIfft,
				&H->fftFramework.inputFFT, &H->fftFramework.outputIFFT,
				&H->fftFramework.inputIFFT_newWeight, &H->fftFramework.outputFFT,
				&dummy1, &dummy2, &H->fftFramework.delay);
			assert(err == JVX_DSP_NO_ERROR);


			H->fftFramework.inputIFFT_oldWeight = NULL;

			// Make sure, result is ok
			assert(dummy1 == H->cfg_cpy.hopsize);
			assert(dummy2 == H->derived_cpy.numSpectrumCoeffs);

			break;
		case JVX_FFT_TOOLS_FWK_OLS_CF:

			// Allocate FFT kernel object
			err = jvx_create_fft_real_2_complex(&H->fftFramework.hdlFft, global_hdl,
				(jvxFFTSize)H->cfg_cpy.fftType, NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
			assert(err == JVX_DSP_NO_ERROR);

			// Allocate IFFT kernel object
			jvx_create_ifft_complex_2_complex(&H->fftFramework.hdlIfft, global_hdl,
				(jvxFFTSize)H->cfg_cpy.fftType, NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
			assert(err == JVX_DSP_NO_ERROR);

			// Allocate framework
			err = jvx_fft_tools_init_framework_ols_cf(
				&H->fftFramework.hdlFftFwk, &H->fftFramework.hdlIfftFwk,
				(jvxInt32)H->cfg_cpy.hopsize,
				H->fftFramework.hdlFft, H->fftFramework.hdlIfft,
				&H->fftFramework.inputFFT, &H->fftFramework.outputIFFT,
				&H->fftFramework.inputIFFT_newWeight, &H->fftFramework.inputIFFT_oldWeight,
				&H->fftFramework.outputFFT, &dummy1, &dummy2, &H->fftFramework.delay);
			assert(err == JVX_DSP_NO_ERROR);

			// Make sure, result is ok
			assert(dummy1 == H->cfg_cpy.hopsize);
			assert(dummy2 == H->derived_cpy.numSpectrumCoeffs);

			break;

		case JVX_FFT_TOOLS_FWK_OLS_IFCF:
			// Allocate FFT kernel object
			err = jvx_create_fft_real_2_complex(&H->fftFramework.hdlFft, global_hdl,
				(jvxFFTSize)H->cfg_cpy.fftType, NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
			assert(err == JVX_DSP_NO_ERROR);

			// Allocate IFFT kernel object
			err = jvx_create_ifft_complex_2_real(&H->fftFramework.hdlIfft, global_hdl,
				(jvxFFTSize)H->cfg_cpy.fftType, NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
			assert(err == JVX_DSP_NO_ERROR);

			// Allocate framework
			err = jvx_fft_tools_init_framework_ols_ifcf(
				&H->fftFramework.hdlFftFwk, &H->fftFramework.hdlIfftFwk,
				(jvxInt32)H->cfg_cpy.hopsize,
				H->fftFramework.hdlFft, H->fftFramework.hdlIfft,
				&H->fftFramework.inputFFT, &H->fftFramework.outputIFFT,
				&H->fftFramework.inputIFFT_newWeight, &H->fftFramework.outputFFT,
				&dummy1, &dummy2, &H->fftFramework.delay, H->cfg_cpy.ifcf_length_max);
			assert(err == JVX_DSP_NO_ERROR);


			H->fftFramework.inputIFFT_oldWeight = NULL;

			// Make sure, result is ok
			assert(dummy1 == H->cfg_cpy.hopsize);
			assert(dummy2 == H->derived_cpy.numSpectrumCoeffs);

			break;
		default:
			assert(0);
		}
		if (H->fftFramework.inputIFFT_oldWeight)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(H->gains_buffer_last, jvxDataCplx, H->derived_cpy.numSpectrumCoeffs);
		}

		H->derived_cpy.outFftBuffer = H->fftFramework.outputFFT;
	}

	if (H->cfg_cpy.allocateWeightCorrection)
	{
		err = jvx_create_ifft_complex_2_real(&H->fftWeightCorrection.hdlIfft, global_hdl,
			(jvxFFTSize)H->cfg_cpy.fftType, NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
		assert(err == JVX_DSP_NO_ERROR);

		err = jvx_create_fft_real_2_complex(&H->fftWeightCorrection.hdlFft, global_hdl,
			(jvxFFTSize)H->cfg_cpy.fftType, NULL, NULL, NULL, JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
		assert(err == JVX_DSP_NO_ERROR);

		err = jvx_fft_tools_init_modify_weights_no_aliasing(&H->fftWeightCorrection.hdlProcessWeights,
			H->fftWeightCorrection.hdlFft,
			H->fftWeightCorrection.hdlIfft,
			&H->fftWeightCorrection.input_complex,
			&H->fftWeightCorrection.output_complex,
			&H->fftWeightCorrection.number_inputoutputComplex,
			(jvxInt32)H->derived_cpy.desiredNumFilterCoeffs,
			H->cfg_cpy.desiredWindowTypeWeightCorrection);
		assert(err == JVX_DSP_NO_ERROR);

		H->derived_cpy.weight_correction_in = H->fftWeightCorrection.input_complex;
		H->derived_cpy.weight_correction_out = H->fftWeightCorrection.output_complex;
	}
	assert(err == JVX_DSP_NO_ERROR);

	jvx_fft_signal_processing_update(signal_proc, JVX_DSP_UPDATE_SYNC, true);
	//H->sync_cpy = signal_proc->sync;

	jvx_fft_signal_processing_update(signal_proc, JVX_DSP_UPDATE_ASYNC, false);
	jvx_fft_signal_processing_update(signal_proc, JVX_DSP_UPDATE_SYNC, false);

	signal_proc->derived = H->derived_cpy;

	return err;
}

jvxDspBaseErrorType
jvx_fft_signal_processing_input(jvx_fft_signal_processing* signal_proc, jvxData* in, jvxSize hopsize)
{
	if (signal_proc->prv == NULL)
		return JVX_DSP_ERROR_WRONG_STATE;

	jvx_fft_signal_processing_prv* H = (jvx_fft_signal_processing_prv*)signal_proc->prv;

	if (hopsize != H->cfg_cpy.hopsize)
		return JVX_DSP_ERROR_INVALID_FORMAT;

	if (H->cfg_cpy.allocateProcessingFft)
	{
		// ================================================================================
		// Copy the input samples for FFT analysis
		// ================================================================================
		memcpy(H->fftFramework.inputFFT, in, H->cfg_cpy.hopsize * sizeof(jvxData));
		jvx_fft_tools_process_framework_fft(H->fftFramework.hdlFftFwk, NULL);
	}

	// ================================================================================
	// Secondary FFT if desired
	// ================================================================================
	if (H->cfg_cpy.allocateAnalysisFft)
	{
		memcpy(H->fftFrameworkAnalysis.inputFFT, in, H->cfg_cpy.hopsize * sizeof(jvxData));
		jvx_fft_tools_process_framework_fft_analysis(H->fftFrameworkAnalysis.hdlFftFwk,
			H->fftFrameworkAnalysis.window_ptr,
			H->fftFrameworkAnalysis.numSamplesValidWindow);
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fft_signal_processing_output(jvx_fft_signal_processing* signal_proc, jvxData* out, jvxData* weights, jvxSize hopsize)
{
	jvxSize i;
	jvxCBool employWeights = false;

	if (signal_proc->prv == NULL)
		return JVX_DSP_ERROR_WRONG_STATE;

	jvx_fft_signal_processing_prv* H = (jvx_fft_signal_processing_prv*)signal_proc->prv;

	if (hopsize != H->cfg_cpy.hopsize)
		return JVX_DSP_ERROR_INVALID_FORMAT;

	if (H->cfg_cpy.allocateWeightCorrection)
	{
		// Setup for weight processing
		jvxCBool weight_correction_required = true;
		if (H->async_cpy.auto_aliasing)
		{
			weight_correction_required = true;
			if (H->cfg_cpy.fftFrameworkType == JVX_FFT_TOOLS_FWK_DW_OLA)
			{
				weight_correction_required = false;
			}
		}
		else
		{
			weight_correction_required = H->async_cpy.employ_antialias_weight_processing;
		}

		// Apply gains
		if (weight_correction_required)
		{
			for (i = 0; i < H->derived_cpy.numSpectrumCoeffs; i++)
			{
				H->fftWeightCorrection.input_complex[i].re = weights[i];
				H->fftWeightCorrection.input_complex[i].im = 0;
			}

			jvx_fft_tools_process_modify_weights_no_aliasing(H->fftWeightCorrection.hdlProcessWeights);

			for (i = 0; i < H->derived_cpy.numSpectrumCoeffs; i++)
			{
				H->fftFramework.inputIFFT_newWeight[i].re =
					H->fftFramework.outputFFT[i].re * H->fftWeightCorrection.output_complex[i].re -
					H->fftFramework.outputFFT[i].im * H->fftWeightCorrection.output_complex[i].im;

				H->fftFramework.inputIFFT_newWeight[i].im =
					H->fftFramework.outputFFT[i].im * H->fftWeightCorrection.output_complex[i].re +
					H->fftFramework.outputFFT[i].re * H->fftWeightCorrection.output_complex[i].im;

				if (H->fftFramework.inputIFFT_oldWeight)
				{
					H->fftFramework.inputIFFT_oldWeight[i].re =
						H->fftFramework.outputFFT[i].re * H->gains_buffer_last[i].re -
						H->fftFramework.outputFFT[i].im * H->gains_buffer_last[i].im;

					H->fftFramework.inputIFFT_oldWeight[i].im =
						H->fftFramework.outputFFT[i].im * H->gains_buffer_last[i].re +
						H->fftFramework.outputFFT[i].re * H->gains_buffer_last[i].im;

					H->gains_buffer_last[i].re = H->fftWeightCorrection.output_complex[i].re;
					H->gains_buffer_last[i].im = H->fftWeightCorrection.output_complex[i].im;
				}
			}
		}
		else
		{
			for (i = 0; i < H->derived_cpy.numSpectrumCoeffs; i++)
			{
				H->fftFramework.inputIFFT_newWeight[i].re =
					H->fftFramework.outputFFT[i].re * weights[i];

				H->fftFramework.inputIFFT_newWeight[i].im =
					H->fftFramework.outputFFT[i].im * weights[i];

				if (H->fftFramework.inputIFFT_oldWeight)
				{
					H->fftFramework.inputIFFT_oldWeight[i].re =
						H->fftFramework.outputFFT[i].re * H->gains_buffer_last[i].re;

					H->fftFramework.inputIFFT_oldWeight[i].im =
						H->fftFramework.outputFFT[i].im * H->gains_buffer_last[i].re;

					H->gains_buffer_last[i].re = weights[i];
					H->gains_buffer_last[i].im = 0;
				}
			}
		}
	}
	if (H->cfg_cpy.allocateProcessingFft)
	{
		// ================================================================================
		// Copy the input samples for FFT analysis
		// ================================================================================
		switch (H->cfg_cpy.fftFrameworkType)
		{
		case JVX_FFT_TOOLS_FWK_OLS:
		case JVX_FFT_TOOLS_FWK_OLS_IFCF:
			jvx_fft_tools_process_framework_ifft(H->fftFramework.hdlIfftFwk, out, H->async_cpy.ifcf_length);
			break;
		default:
			jvx_fft_tools_process_framework_ifft(H->fftFramework.hdlIfftFwk, NULL, H->async_cpy.ifcf_length);
			memcpy(out, H->fftFramework.outputIFFT, H->cfg_cpy.hopsize * sizeof(jvxData));
			break;
		}
	}

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fft_signal_processing_postprocess(jvx_fft_signal_processing* signal_proc)
{
	jvxDspBaseErrorType err = JVX_DSP_NO_ERROR;
	jvx_fft_signal_processing_prv* H = (jvx_fft_signal_processing_prv*)signal_proc->prv;

	if (!H)
	{
		return JVX_DSP_ERROR_WRONG_STATE;
	}

	if (H->cfg_cpy.allocateProcessingFft)
	{
		// ===================================================================================
		// Remove FFT frameworks
		// ===================================================================================
		if (H->fftFramework.hdlFftFwk && H->fftFramework.hdlIfftFwk)
		{
			jvx_fft_tools_terminate_framework(H->fftFramework.hdlFftFwk, H->fftFramework.hdlIfftFwk);
		}
		if (H->fftFramework.hdlFft)
		{
			jvx_destroy_fft(H->fftFramework.hdlFft);
		}
		if (H->fftFramework.hdlIfft)
		{
			jvx_destroy_ifft(H->fftFramework.hdlIfft);
		}
		H->fftFramework.delay = 0;
		H->fftFramework.hdlFft = NULL;
		H->fftFramework.hdlFftFwk = NULL;
		H->fftFramework.hdlIfft = NULL;
		H->fftFramework.hdlIfftFwk = NULL;
		H->fftFramework.inputFFT = NULL;
		H->fftFramework.inputIFFT_newWeight = NULL;
		H->fftFramework.inputIFFT_oldWeight = NULL;
		H->fftFramework.outputFFT = NULL;
		H->fftFramework.outputIFFT = NULL;
		if (H->gains_buffer_last)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(H->gains_buffer_last, jvxDataCplx, H->derived_cpy.numSpectrumCoeffs);
		}
	}

	if (H->cfg_cpy.allocateAnalysisFft)
	{
		// Delete analysis FFT window
		JVX_DSP_SAFE_DELETE_FIELD(H->fftFrameworkAnalysis.window);

		// Terminate FFT analysis framework
		jvx_fft_tools_terminate_framework_fft_analysis(H->fftFrameworkAnalysis.hdlFftFwk);

		// Deallocate FFT kernel object
		err = jvx_destroy_fft(H->fftFrameworkAnalysis.hdlFft);
	}

	// ===================================================================================
	// Remove weight processor
	// ===================================================================================
	if (H->cfg_cpy.allocateWeightCorrection)
	{
		if (H->fftWeightCorrection.hdlProcessWeights)
		{
			jvx_fft_tools_terminate_modify_weights_no_aliasing(H->fftWeightCorrection.hdlProcessWeights);
		}
		if (H->fftWeightCorrection.hdlIfft)
		{
			jvx_destroy_ifft(H->fftWeightCorrection.hdlIfft);
		}
		if (H->fftWeightCorrection.hdlFft)
		{
			jvx_destroy_fft(H->fftWeightCorrection.hdlFft);
		}

		H->fftWeightCorrection.hdlFft = NULL;
		H->fftWeightCorrection.hdlIfft = NULL;
		H->fftWeightCorrection.hdlProcessWeights = NULL;
		H->fftWeightCorrection.input_complex = NULL;
		H->fftWeightCorrection.output_complex = NULL;
		H->fftWeightCorrection.number_inputoutputComplex = 0;
	}

	jvx_reset_derived(&H->derived_cpy);
	JVX_DSP_SAFE_DELETE_OBJECT(H);
	signal_proc->prv = NULL;
	jvx_reset_derived(&signal_proc->derived);

	return err;
}