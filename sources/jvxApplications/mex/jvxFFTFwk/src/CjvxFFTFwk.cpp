#include "CjvxFFTFwk.h"

#include "CjvxMatlabToCConverter.h"

CjvxFFTFwk::CjvxFFTFwk()
{
}

jvxErrorType
CjvxFFTFwk::init_fft_fwk(const mxArray* arrCfg)
{
	if (core_inst != nullptr)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	JVX_SAFE_ALLOCATE_OBJECT_CPP_Z(core_inst, jvx_fft_signal_processing);
	jvx_fft_signal_processing_initCfg(core_inst);

	if (mxIsStruct(arrCfg))
	{
		mexPrintf("fft signal processing is configured by sub-struct <fft_processor>.\n");
		const mxArray* arrFft = CjvxMatlabToCConverter::jvx_mex_lookup_strfield(arrCfg, "fft_processor");
		const mxArray* val = nullptr;

		jvxErrorType res = JVX_NO_ERROR;

		jvxSize fftSize = 0;

		if (arrFft)
		{
			std::string token;
			mexPrintf("--> Found struct field <fft_processor>, reading single values for fwk configuration.\n");

			/*
			* -- Struct cfg --
			* jvxFftTools_fwkType fftFrameworkType;						-> selectFftFramework
			* jvxSize ifcf_length_max;									-> ifcf_length_max
			* jvxSize ir_length_max;									-> ir_length_max

			* jvxFFTSize fftType;
			* jvxSize hopsize;											-> hopsize
			* jvxSize buffersize;										-> buffersize
			*
			* jvxCBool allocateProcessingFft;
			* jvxCBool allocateAnalysisFft;
			* jvxCBool allocateWeightCorrection;
			*
			* jvx_windowType desiredWindowTypeWeightCorrection;
			*
			* -- Struct async --
			* jvxCBool auto_aliasing;									-> auto_anti_aliasing
			* jvxCBool employ_antialias_weight_processing;				-> antialias
			* jvxSize ifcf_length;										-> ifcf_length
			*
			* -- Struct sync --
			* jvxInt32 percentAnalysisWinFftSize;						-> percentAnalysisWinFftSize
			* jvx_windowType wintype;									-> wintype
			*
			*
			* -> FFTSize
			* -> hopsize
			* -> buffersize
			*
			*/
			JVX_MEX_READ_SINGLE_FROM_STRUCT(res, "fftsize", arrFft, fftSize);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <FFTSize> in struct.\n");

			JVX_MEX_READ_SINGLE_FROM_STRUCT(res, "ifcf_length_max", arrFft, core_inst->cfg.ifcf_length_max);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <ifcf_length_max> in struct.\n");

			JVX_MEX_READ_SINGLE_FROM_STRUCT(res, "ir_length_max", arrFft, core_inst->cfg.ir_length_max);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <ir_length_max> in struct.\n");

			JVX_MEX_READ_SINGLE_FROM_STRUCT(res, "hopsize", arrFft, core_inst->cfg.hopsize);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <hopsize> in struct.\n");

			JVX_MEX_READ_SINGLE_FROM_STRUCT(res, "buffersize", arrFft, core_inst->cfg.buffersize);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <buffersize> in struct.\n");

			JVX_MEX_READ_SELECTION_FROM_STRUCT(res, "selectFftFramework", arrFft, core_inst->cfg.fftFrameworkType, JVX_FFT_TOOLS_FWK_LIMIT, jvxFftTools_fwkType);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <selectFftFramework> in struct.\n");

			// ASYNC 

			JVX_MEX_READ_SINGLE_FROM_STRUCT(res, "auto_anti_aliasing", arrFft, core_inst->async.auto_aliasing);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <auto_anti_aliasing> in struct.\n");

			JVX_MEX_READ_SINGLE_FROM_STRUCT(res, "antialias", arrFft, core_inst->async.employ_antialias_weight_processing);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <antialias> in struct.\n");

			JVX_MEX_READ_SINGLE_FROM_STRUCT(res, "ifcf_length", arrFft, core_inst->async.ifcf_length);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <ifcf_length> in struct.\n");

			// ASYNC 

			JVX_MEX_READ_SINGLE_FROM_STRUCT(res, "percentAnalysisWinFftSize", arrFft, core_inst->sync.percentAnalysisWinFftSize);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <percentAnalysisWinFftSize> in struct.\n");

			JVX_MEX_READ_SELECTION_FROM_STRUCT(res, "wintype", arrFft, core_inst->sync.wintype, JVX_WINDOW_LIMIT, jvx_windowType);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <wintype> in struct.\n");

			/*
			JVX_MEX_READ_SELECTION_FROM_STRUCT(res, "fftType/jvxSelection_value", arrFwk, fftType, jvxFFTSize::JVX_FFT_TOOLS_FFT_SIZE_8192, jvxFFTSize);

			/*
			JVX_MEX_READ_SINGLE_FROM_STRUCT(res, "FFTSize", arrFwk, R);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <R> in struct.\n");

			JVX_MEX_READ_SELECTION_FROM_STRUCT(res, "fftType/jvxSelection_value", arrFwk, fftType, jvxFFTSize::JVX_FFT_TOOLS_FFT_SIZE_8192, jvxFFTSize);
			if (res != JVX_NO_ERROR) mexPrintf("--> Failed to read entry for <fftType/jvxSelection_value> in struct.\n");
			*/
		}

		// ================================================================================================================
		jvxSize loc_fftsize;
		jvx_get_nearest_size_fft(&core_inst->cfg.fftType, fftSize, JVX_FFT_ROUND_NEAREST, &loc_fftsize);
		
		jvx_fft_signal_processing_updateCfg(&core_inst->derived, &core_inst->cfg);
		assert(core_inst->derived.fftsize == loc_fftsize);

		// FFT Size must be multiples of 2
		if (
			((core_inst->derived.fftsize % 2) == 1) ||
			(core_inst->derived.fftsize < core_inst->cfg.buffersize) ||
			(core_inst->derived.fftsize < core_inst->cfg.hopsize))

		{
			mexPrintf("--> Failed to initialize fft processor handle - invalid configuration.\n");
		}


		// =================================================================================
		// INITIALIZATION
		// =================================================================================
		

		// Allocate global data for FFT usage (reuse twiddle factors in Mac cases)
		jvxErrorType resL = jvx_create_fft_ifft_global(&global_hdl, (jvxFFTSize)core_inst->cfg.fftType, nullptr);
		assert(resL == JVX_DSP_NO_ERROR);

		/*
		 -> updateCfg will be called in prepare!!!
		jvx_fft_signal_processing_updateCfg(&H->runtime.noise_signal_proc.cfg, &H->runtime.noise_signal_proc.cfg);
		jvx_fft_signal_processing_updateCfg(&H->runtime.announce_signal_proc.cfg);
		*/

		// Configuration for noise signal
		core_inst->cfg.allocateAnalysisFft = true;
		core_inst->cfg.allocateWeightCorrection = true;
		core_inst->cfg.allocateProcessingFft = true;

		M2P1 = core_inst->derived.fftsize / 2 + 1;
		hopsize = core_inst->cfg.hopsize;

		jvx_fft_signal_processing_prepare(core_inst, global_hdl);

	}

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFFTFwk::term_fft_fwk()
{
	if (core_inst == nullptr)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	jvx_fft_signal_processing_postprocess(core_inst);
	JVX_SAFE_DELETE_OBJECT(core_inst);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFFTFwk::process_fft_fwk_input(jvxData* input, jvxDataCplx* outputBufProc, jvxDataCplx* outputBufAna)
{
	int i;	

	if (core_inst == nullptr)
	{
		return JVX_ERROR_WRONG_STATE;
	}
	
	jvx_fft_signal_processing_input(core_inst, input, core_inst->cfg.hopsize);

	if (outputBufProc)
	{
		memcpy(outputBufProc, core_inst->derived.outFftBuffer, sizeof(jvxDataCplx) * M2P1);
	}

	if (outputBufAna)
	{
		if (core_inst->derived.outFftBufferAnalysis)
		{
			memcpy(outputBufAna, core_inst->derived.outFftBufferAnalysis, sizeof(jvxDataCplx) * M2P1);
		}
		else
		{
			memcpy(outputBufAna, core_inst->derived.outFftBuffer, sizeof(jvxDataCplx) * M2P1);
		}
	}

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFFTFwk::process_fft_fwk_output(jvxDataCplx* input, jvxData* weights, jvxData* output)
{
	int i;
	if (core_inst == nullptr)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	if (input)
	{
		memcpy(core_inst->derived.outFftBuffer, input, sizeof(jvxDataCplx) * M2P1);
	}
	
	jvx_fft_signal_processing_output(core_inst, output, weights, core_inst->cfg.hopsize);
	return JVX_NO_ERROR;
}

