#include "jvx_measure_ir_reg.h"
#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_math/jvx_complex.h"

struct jvx_measure_ir_reg_priv
{
	struct jvx_measure_ir_reg_init init_cpy;
	struct jvx_measure_ir_reg_async async_cpy;
	struct jvx_measure_ir_reg_derived derived_cpy;

	struct
	{
		jvxFFTGlobal* glob_fft;
		jvxFFT* fftTestSignal;
		jvxFFT* fftMeasureSignal;

		jvxIFFT* ifftMeasureTestSignal;

		jvxSize fftSize;
		jvxSize fftSize_log2;
		jvxSize fftSize2_p1;

		jvxData* inSigTest_avrg;
		jvxDataCplx* inSigTest_avrg_fft;

		jvxData* inSigMeasure_avrg;
		jvxDataCplx* inSigMeasure_avrg_fft;

		jvxDataCplx* inSigMeasureTest_avrg_ifft;
		jvxData* outSigMeasureTest_avrg;

		jvxData* sigMultipleTest_loops_avrg_fft_abs_s;

		jvxData normFac;

	} runtime;
};



void jvx_measure_ir_reg_cfg_init(struct jvx_measure_ir_reg* hdl)
{
	hdl->init.loopCnt = 5;
	hdl->init.fs = 48000;
	hdl->init.skipFirst = false;
	hdl->init.flowStart = 0;
	hdl->init.flowStop = hdl->init.flowStart;
	hdl->init.fhighStart = hdl->init.fs / 2;
	hdl->init.fhighStop = hdl->init.fhighStart;
	hdl->async.regulationCoeffMin = 1e-12;
	hdl->async.regulationCoeffMax = 1e-4;
	hdl->async.interpol_log = c_true;
	hdl->async.exponent_fac = 4;

	hdl->init.llMax = 0;

	hdl->derived.ll_oneLoop = 0;

	hdl->prv = NULL;
}


jvxErrorType jvx_measure_ir_reg_init(struct jvx_measure_ir_reg* hdl)
{
	struct jvx_measure_ir_reg_priv* retStr = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize dummy = 0;

	if (!hdl)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}
	
	if (hdl)
	{
		if ((hdl->init.llMax % hdl->init.loopCnt) != 0)
		{
			return JVX_ERROR_INVALID_FORMAT;
		}
	}

	// ==============================================================================

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(retStr, struct jvx_measure_ir_reg_priv);
	if (retStr)
	{
		retStr->init_cpy = hdl->init;		
		retStr->async_cpy = hdl->async;
		retStr->derived_cpy.ll_oneLoop = retStr->init_cpy.llMax / retStr->init_cpy.loopCnt;

		// Prepare buffers fo rthe right size 
		retStr->runtime.fftSize = retStr->derived_cpy.ll_oneLoop;
		if (retStr->runtime.fftSize % 2)
		{
			retStr->runtime.fftSize++;
		}

		retStr->runtime.fftSize_log2 = (jvxSize)(ceil(log((jvxData)retStr->runtime.fftSize * 2) / log(2.0)));
		retStr->runtime.fftSize2_p1 = retStr->runtime.fftSize / 2 + 1;
		// newHdl->runtime.fftSize22_p1 = newHdl->runtime.fftSize + 1;

		res = jvx_create_fft_ifft_global(&retStr->runtime.glob_fft, (jvxFFTSize)(retStr->runtime.fftSize_log2 - 4), NULL JVX_FFT_GLOBAL_CONFIG_ADD_ARGUMENT_CALL);
		assert(res == JVX_DSP_NO_ERROR);

		res = jvx_create_fft_real_2_complex(&retStr->runtime.fftTestSignal,
			retStr->runtime.glob_fft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
			&retStr->runtime.inSigTest_avrg,
			&retStr->runtime.inSigTest_avrg_fft,
			&dummy,
			JVX_FFT_IFFT_EFFICIENT,
			NULL, NULL,
			retStr->runtime.fftSize);
		assert(res == JVX_DSP_NO_ERROR);

		res = jvx_create_fft_real_2_complex(&retStr->runtime.fftMeasureSignal,
			retStr->runtime.glob_fft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
			&retStr->runtime.inSigMeasure_avrg,
			&retStr->runtime.inSigMeasure_avrg_fft,
			&dummy,
			JVX_FFT_IFFT_EFFICIENT,
			NULL, NULL,
			retStr->runtime.fftSize);
		assert(res == JVX_DSP_NO_ERROR);

		res = jvx_create_ifft_complex_2_real(&retStr->runtime.ifftMeasureTestSignal,
			retStr->runtime.glob_fft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
			&retStr->runtime.inSigMeasureTest_avrg_ifft,
			&retStr->runtime.outSigMeasureTest_avrg,
			&dummy,
			JVX_FFT_IFFT_EFFICIENT,
			retStr->runtime.inSigTest_avrg_fft, NULL,
			retStr->runtime.fftSize);
		assert(res == JVX_DSP_NO_ERROR);

		retStr->runtime.normFac = 1;
		jvxCBool normOut = jvx_fft_requires_normalization(retStr->runtime.glob_fft);
		if (normOut)
		{
			retStr->runtime.normFac = 1.0 / (jvxData)retStr->runtime.fftSize;
		}

		JVX_SAFE_ALLOCATE_FIELD_Z(retStr->runtime.sigMultipleTest_loops_avrg_fft_abs_s, jvxData, retStr->runtime.fftSize2_p1);

		hdl->prv = (jvxHandle*)retStr;
		hdl->derived = retStr->derived_cpy;

		// ===============================================================================
	}
	return JVX_NO_ERROR;
}

// =======================================================================================
// =======================================================================================
// =======================================================================================

jvxErrorType jvx_measure_ir_reg_terminate(struct jvx_measure_ir_reg* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (hdl)
	{
		struct jvx_measure_ir_reg_priv* hdlPrv = (struct jvx_measure_ir_reg_priv*)hdl->prv;
		if (hdlPrv)
		{
			JVX_SAFE_DELETE_FIELD(hdlPrv->runtime.sigMultipleTest_loops_avrg_fft_abs_s);

			res = jvx_destroy_ifft(hdlPrv->runtime.ifftMeasureTestSignal);
			assert(res == JVX_DSP_NO_ERROR);

			res = jvx_destroy_fft(hdlPrv->runtime.fftMeasureSignal);
			assert(res == JVX_DSP_NO_ERROR);

			res = jvx_destroy_fft(hdlPrv->runtime.fftTestSignal);
			assert(res == JVX_DSP_NO_ERROR);

			res = jvx_destroy_fft_ifft_global(hdlPrv->runtime.glob_fft, NULL);
			assert(res == JVX_DSP_NO_ERROR);

			hdlPrv->runtime.ifftMeasureTestSignal = NULL;
			hdlPrv->runtime.fftMeasureSignal = NULL;
			hdlPrv->runtime.fftTestSignal = NULL;
			hdlPrv->runtime.glob_fft = NULL;
		}

		JVX_SAFE_DELETE_OBJECT(hdlPrv);
		hdl->prv = NULL;
	}
	return JVX_NO_ERROR;
}

// =======================================================================================
// =======================================================================================
// =======================================================================================

jvxErrorType jvx_measure_ir_reg_process(struct jvx_measure_ir_reg* hdl, 
	const jvxData* inMeasure_ptr, jvxSize inMeasure_sz,
	const jvxData* inTest_ptr, jvxSize inTest_sz,
	jvxData* outMeasureAvrg_ptr, jvxSize outMeasureAvrg_sz,
	jvxData* outTestAvrg_ptr, jvxSize outTestAvrg_sz,
	jvxData* outIr_ptr, jvxSize outIr_sz, 
	const jvxData* win_ptr, jvxSize win_sz,
	jvxData* dbgOut_ptr, jvxSize dbgOut_sz)
{
	jvxSize i,j;
	jvxErrorType res = JVX_DSP_NO_ERROR;	
	if (hdl)
	{
		struct jvx_measure_ir_reg_priv* hdlPrv = (struct jvx_measure_ir_reg_priv*)hdl->prv;
		if (hdlPrv)
		{
			jvxSize cnt = 0;
			jvxSize evalLoopCnt = (jvxData)hdlPrv->init_cpy.loopCnt;

			if (hdlPrv->init_cpy.skipFirst)
			{
				cnt += hdlPrv->derived_cpy.ll_oneLoop;
				evalLoopCnt = evalLoopCnt - 1;
			}

			jvxData scaleFacAvrg = 1.0 / (jvxData)evalLoopCnt;

			/*
			sigMultipleTest_loops_avrg = sum(sigMultipleTest_loops, 1) / (loopCnt);
			sigMultipleMeasure_loops_avrg = sum(sigMultipleMeasure_loops, 1) / (loopCnt);
			 */

			for (j = 0; j < evalLoopCnt; j++)
			{
				jvxData* fillMeasure = hdlPrv->runtime.inSigMeasure_avrg;
				jvxData* fillTest = hdlPrv->runtime.inSigTest_avrg;

				for (i = 0; i < hdlPrv->derived_cpy.ll_oneLoop; i++)
				{
					if (cnt < inMeasure_sz)
					{
						fillMeasure[i] += inMeasure_ptr[cnt] * scaleFacAvrg;
					}
					if (cnt < inTest_sz)
					{
						fillTest[i] += inTest_ptr[cnt] * scaleFacAvrg;
					}
					cnt++;
				}
			}

			if (outMeasureAvrg_ptr)
			{
				jvxSize nCpy = JVX_MIN(hdlPrv->derived_cpy.ll_oneLoop, outMeasureAvrg_sz);
				memcpy(outMeasureAvrg_ptr, hdlPrv->runtime.inSigMeasure_avrg, sizeof(jvxData) * nCpy);
				memcpy(outTestAvrg_ptr, hdlPrv->runtime.inSigTest_avrg, sizeof(jvxData) * nCpy);
			}

			/*
			sigMultipleTest_loops_avrg_fft = fft(sigMultipleTest_loops_avrg);
			sigMultipleMeasure_loops_avrg_fft = fft(sigMultipleMeasure_loops_avrg);
			*/
			res = jvx_execute_fft(hdlPrv->runtime.fftMeasureSignal);
			assert(res == JVX_DSP_NO_ERROR);
			res = jvx_execute_fft(hdlPrv->runtime.fftTestSignal);
			assert(res == JVX_DSP_NO_ERROR);

			/*
			-> Compute the abs spec buffer
			sigMultipleTest_loops_avrg_fft_abs_s = sigMultipleTest_loops_avrg_fft .* conj(sigMultipleTest_loops_avrg_fft);
			 */
			jvx_complex_square_of_magnitude_n(hdlPrv->runtime.inSigTest_avrg_fft, hdlPrv->runtime.sigMultipleTest_loops_avrg_fft_abs_s, hdlPrv->runtime.fftSize2_p1);

			jvxData deltaFreq = hdlPrv->init_cpy.fs / 2.0 / (hdlPrv->runtime.fftSize2_p1 - 1);
			jvxSize nLowStart = ceil(hdlPrv->init_cpy.flowStart / deltaFreq);
			jvxSize nLowStop = ceil(hdlPrv->init_cpy.flowStop / deltaFreq);
			jvxSize nHighStart = floor(hdlPrv->init_cpy.fhighStart/ deltaFreq);
			jvxSize nHighStop = floor(hdlPrv->init_cpy.fhighStop / deltaFreq);

			/*
			-> Find the maximum value
			sigMultipleTest_loops_avrg_fft_abs_s_max = max(sigMultipleTest_loops_avrg_fft_abs_s);
			*/
			jvxData sigMultipleTest_loops_avrg_fft_abs_s_max = -1;
			for (i = 0; i < hdlPrv->runtime.fftSize2_p1; i++)
			{
				if (hdlPrv->runtime.sigMultipleTest_loops_avrg_fft_abs_s[i] > sigMultipleTest_loops_avrg_fft_abs_s_max)
				{
					sigMultipleTest_loops_avrg_fft_abs_s_max = hdlPrv->runtime.sigMultipleTest_loops_avrg_fft_abs_s[i];
				}
			}

			for (i = 0; i < hdlPrv->runtime.fftSize2_p1; i++)
			{
			
				/*
				-> Derive the numerqtor part
				numer = sigMultipleMeasure_loops_avrg_fft .* conj(sigMultipleTest_loops_avrg_fft);
				*/
				jvxDataCplx numer = jvx_complex_multiply_conj1_i(hdlPrv->runtime.inSigTest_avrg_fft[i], hdlPrv->runtime.inSigMeasure_avrg_fft[i]);
				
				/*
				-> Compute areas of regularization
				nFFT2P1 = NFFT /2 + 1;
				deltaFreq = (fs/2) / (nFFT2P1-1);    
				nLowStart = ceil(flowStart / deltaFreq);
				nLowStop = ceil(flowStop / deltaFreq);
				nHighStart = floor(fhighStart / deltaFreq);
				nHighStop = floor(fhighStop / deltaFreq);

				deltaBuf = zeros(1, nFFT2P1);
				N1 = (nLowStop - nLowStart + 1);
				deltaBuf(nLowStart+1:nLowStop+1) = (1:N1)/N1;
    
				deltaBuf(nLowStop+1:nHighStart) = 1;

			    N2 = (nHighStop - nHighStart + 1);
				deltaBuf(nHighStart:nHighStop) = (N2:-1:1)/N2;
				deltaBuf = deltaBuf.^2;
    
				deltaBuf = regulationCoeffMax * (1-deltaBuf) + regulationCoeffMin *deltaBuf;
				regValue = deltaBuf * sigMultipleTest_loops_avrg_fft_abs_s_max;
				*/
				jvxCBool interpolLog = hdlPrv->async_cpy.interpol_log;
				jvxData exponentFac = hdlPrv->async_cpy.exponent_fac;

				jvxData regMax = hdlPrv->async_cpy.regulationCoeffMax;
				jvxData regMin = hdlPrv->async_cpy.regulationCoeffMin;
							
				if (interpolLog)
				{
					regMin = log(hdlPrv->async_cpy.regulationCoeffMin);
					regMax = log(hdlPrv->async_cpy.regulationCoeffMax);
				}

				jvxData regValue = regMax;
				if (
					(i >= nLowStart) && (i <= nLowStop)
					)
				{
					jvxData norm = (jvxData)(nLowStop - nLowStart + 1);
					norm = 1.0 / norm;
					jvxData linFac = norm * (jvxData)(i - nLowStart);
					linFac = log(linFac);
					linFac *= exponentFac;
					linFac = exp(linFac);
					regValue = regValue * (1 - linFac) + regMin * linFac;
				}
				else
				{
					if (
						(i > nLowStop) && (i < nHighStart))
					{
						regValue = regMin;
					}
					else
					{
						if (
							(i >= nHighStart) && (i <= nHighStop))
						{
							jvxData norm = (jvxData)(nHighStop - nHighStart + 1);
							norm = 1.0 / norm;
							jvxData linFac = norm * (jvxData)(nHighStop - i);
							linFac = log(linFac);
							linFac *= exponentFac;
							linFac = exp(linFac);
							regValue = regMax * (1 - linFac) + regMin * linFac;
						}
					}
				}

				if (interpolLog)
				{
					regValue = exp(regValue);
				}

				if (
					(dbgOut_ptr) && (i < dbgOut_sz))
				{
					dbgOut_ptr[i] = regValue;
				}

				regValue *= sigMultipleTest_loops_avrg_fft_abs_s_max;

				/*
				denumer = sigMultipleTest_loops_avrg_fft_abs_s + regValue;
				*/
				jvxData denumer = hdlPrv->runtime.sigMultipleTest_loops_avrg_fft_abs_s[i] + regValue;

				/*
				div_fft = numer. / denumer;
				*/
				hdlPrv->runtime.inSigMeasureTest_avrg_ifft[i].re = numer.re / denumer;
				hdlPrv->runtime.inSigMeasureTest_avrg_ifft[i].im = numer.im / denumer;
			}

			res = jvx_execute_ifft(hdlPrv->runtime.ifftMeasureTestSignal);			

			jvxSize nCpy = outIr_sz;
			nCpy = JVX_MIN(nCpy, hdlPrv->runtime.fftSize);
			for (i = 0; i < nCpy; i++)
			{
				if (win_ptr)
				{
					if (i < win_sz)
					{
						outIr_ptr[i] = hdlPrv->runtime.outSigMeasureTest_avrg[i] * win_ptr[i] * hdlPrv->runtime.normFac;
					}
				}
				else
				{
					outIr_ptr[i] = hdlPrv->runtime.outSigMeasureTest_avrg[i] * hdlPrv->runtime.normFac;
				}
			}

			/*
			numer = sigMultipleMeasure_loops_avrg_fft .* conj(sigMultipleTest_loops_avrg_fft);
			*/

			return JVX_NO_ERROR; 

			// ==================================================================================
		}
		return JVX_ERROR_NOT_READY; // hdlPrv is null
	}
	return JVX_ERROR_INVALID_ARGUMENT; // hdl is null
}

jvxErrorType jvx_measure_ir_reg_update(struct jvx_measure_ir_reg* hdl, jvxCBool do_set)
{
	if (hdl)
	{
		struct jvx_measure_ir_reg_priv* hdlPrv = (struct jvx_measure_ir_reg_priv*)hdl->prv;
		if (hdlPrv)
		{
			if (do_set)
			{
				hdlPrv->async_cpy = hdl->async;
			}
			else
			{
				hdl->async = hdlPrv->async_cpy;
			}
			return JVX_NO_ERROR;	
		}
		return JVX_ERROR_WRONG_STATE;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}
