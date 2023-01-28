#include "jvx_measure_ir.h"
#include "jvx_dsp_base.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_math/jvx_complex.h"
#include <math.h>
#include <assert.h>

typedef struct
{
	jvxMeasureIr_init prm_init;

	jvxMeasureIr_sync prm_sync;

	struct
	{
		jvxSize fftSize_log2;
		jvxSize fftSize;
		jvxSize fftSize2_p1;
		jvxSize fftSize22_p1;

		jvxFFTGlobal* glob_fft;

		jvxFFT* fftTestSignal_ir;
		jvxDataCplx* cplx_TestSignal_ir;
		jvxData* superposedTestSignal_ir;

		jvxFFT* fftMeasured_ir;
		jvxDataCplx* cplx_Measured_ir;
		jvxData* superposedRecording_ir;


		jvxFFT* fftTestSignal_xc;
		jvxDataCplx* cplx_TestSignal_xc;
		jvxData* superposedTestSignal_xc;

		jvxFFT* fftMeasured_xc;
		jvxDataCplx* cplx_Measured_xc;
		jvxData* superposedRecording_xc;


		jvxFFT* ifftIr;
		jvxDataCplx* cplx_Ir;

		jvxFFT* ifftCCorr;
		jvxDataCplx* cplx_CCorr;

	} runtime;

} jvx_measure_ir_private;

jvxDspBaseErrorType
jvx_measure_ir_initConfig(jvxMeasureIr* handle)
{
	handle->prv = NULL;

	memset(&handle->prm_init, 0, sizeof(handle->prm_init));
	handle->prm_init.numReps = 1;
	handle->prm_init.szTestsignal_onerep = 32768;
	handle->prm_init.skipNumber = 0;

	memset(&handle->prm_sync, 0, sizeof(handle->prm_sync));

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_measure_ir_prepare(jvxMeasureIr* handle)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvx_measure_ir_private* newHdl = NULL;
	jvxSize dummy;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newHdl, jvx_measure_ir_private);
	memset(newHdl, 0, sizeof(jvx_measure_ir_private));

	memcpy(&newHdl->prm_init, &handle->prm_init, sizeof(jvxMeasureIr_init));
	newHdl->runtime.fftSize = handle->prm_init.szTestsignal_onerep;
	if(newHdl->runtime.fftSize % 2)
	{
		newHdl->runtime.fftSize ++;
	}

	newHdl->runtime.fftSize_log2 = (jvxSize) (ceil(log((jvxData)newHdl->runtime.fftSize*2)/log(2.0)));
	newHdl->runtime.fftSize2_p1 = newHdl->runtime.fftSize/2 + 1;
	newHdl->runtime.fftSize22_p1 = newHdl->runtime.fftSize + 1;
	newHdl->prm_sync.offsetXCorr = 0;

	res = jvx_create_fft_ifft_global(&newHdl->runtime.glob_fft, (jvxFFTSize)(newHdl->runtime.fftSize_log2-4));
	assert(res == JVX_DSP_NO_ERROR);

	newHdl->prm_sync.lRecording = newHdl->prm_init.szTestsignal_onerep *newHdl->prm_init.numReps;

	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newHdl->prm_sync.fldBufferMeasureSignal, jvxData, newHdl->prm_sync.lRecording);
	memset(newHdl->prm_sync.fldBufferMeasureSignal, 0, sizeof(jvxData) * newHdl->prm_sync.lRecording);

	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newHdl->prm_sync.fldBufferTestSignal, jvxData, newHdl->prm_sync.lRecording);
	memset(newHdl->prm_sync.fldBufferTestSignal, 0, sizeof(jvxData) * newHdl->prm_sync.lRecording);

	newHdl->prm_sync.fldIResponse = NULL;
	newHdl->prm_sync.fldSPosedMeasure = NULL;
	newHdl->prm_sync.fldSPosedTest = NULL;
	newHdl->prm_sync.lIResponse = 0;
	newHdl->runtime.superposedTestSignal_ir = NULL;
	newHdl->runtime.cplx_TestSignal_ir = NULL;
	newHdl->runtime.superposedRecording_ir = NULL;
	newHdl->runtime.cplx_Measured_ir = NULL;

	if(newHdl->prm_init.computeIr)
	{
		res = jvx_create_fft_real_2_complex(&newHdl->runtime.fftTestSignal_ir,
			newHdl->runtime.glob_fft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
			&newHdl->runtime.superposedTestSignal_ir,
			&newHdl->runtime.cplx_TestSignal_ir,
			&dummy,
			JVX_FFT_IFFT_EFFICIENT,
			NULL, NULL,
			newHdl->runtime.fftSize);
		assert(res == JVX_DSP_NO_ERROR);

		res = jvx_create_fft_real_2_complex(&newHdl->runtime.fftMeasured_ir,
			newHdl->runtime.glob_fft,
			JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
			&newHdl->runtime.superposedRecording_ir,
			&newHdl->runtime.cplx_Measured_ir,
			&dummy,
			JVX_FFT_IFFT_EFFICIENT,
			NULL, NULL,
			newHdl->runtime.fftSize);
		assert(res == JVX_DSP_NO_ERROR);

		res = jvx_create_ifft_complex_2_real(&newHdl->runtime.ifftIr,
			newHdl->runtime.glob_fft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
			&newHdl->runtime.cplx_Ir,
			&newHdl->prm_sync.fldIResponse,
			&newHdl->prm_sync.lIResponse,
			JVX_FFT_IFFT_EFFICIENT,
			NULL, NULL,
			newHdl->runtime.fftSize);
		assert(res == JVX_DSP_NO_ERROR);
	}

	if (newHdl->prm_init.storeSuperPosed)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newHdl->prm_sync.fldSPosedMeasure, jvxData, newHdl->prm_sync.lIResponse);
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newHdl->prm_sync.fldSPosedTest, jvxData, newHdl->prm_sync.lIResponse);
	}

	newHdl->prm_sync.fldXCorr = NULL;
	newHdl->prm_sync.lXCorr = 0;
	newHdl->runtime.superposedTestSignal_xc = NULL;
	newHdl->runtime.cplx_TestSignal_xc = NULL;
	newHdl->runtime.superposedRecording_xc = NULL;
	newHdl->runtime.cplx_Measured_xc = NULL;

	if(newHdl->prm_init.computeXc)
	{
		res = jvx_create_fft_real_2_complex(&newHdl->runtime.fftTestSignal_xc,
			newHdl->runtime.glob_fft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
			&newHdl->runtime.superposedTestSignal_xc,
			&newHdl->runtime.cplx_TestSignal_xc,
			&dummy,
			JVX_FFT_IFFT_EFFICIENT,
			NULL, NULL,
			newHdl->runtime.fftSize*2);
		assert(res == JVX_DSP_NO_ERROR);

		res = jvx_create_fft_real_2_complex(&newHdl->runtime.fftMeasured_xc,
			newHdl->runtime.glob_fft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
			&newHdl->runtime.superposedRecording_xc,
			&newHdl->runtime.cplx_Measured_xc,
			&dummy,
			JVX_FFT_IFFT_EFFICIENT,
			NULL, NULL,
			newHdl->runtime.fftSize*2);
		assert(res == JVX_DSP_NO_ERROR);

		res = jvx_create_ifft_complex_2_real(&newHdl->runtime.ifftCCorr,
			newHdl->runtime.glob_fft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
			&newHdl->runtime.cplx_CCorr,
			&newHdl->prm_sync.fldXCorr,
			&newHdl->prm_sync.lXCorr,
			JVX_FFT_IFFT_EFFICIENT,
			NULL, NULL,
			newHdl->runtime.fftSize*2);
		assert(res == JVX_DSP_NO_ERROR);
	}
	handle->prv = newHdl;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_measure_ir_update(jvxMeasureIr* handle, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvx_measure_ir_private* locHdl = NULL;
	locHdl = (jvx_measure_ir_private*)handle->prv;
	switch(whatToUpdate)
	{
		case JVX_DSP_UPDATE_SYNC:
			if(do_set)
			{
				locHdl->prm_sync.ext = handle->prm_sync.ext;
			}
			else
			{
				handle->prm_sync = locHdl->prm_sync;
			}
			break;
		default:
			res = JVX_DSP_ERROR_UNSUPPORTED;
	}
	return(res);
}

jvxDspBaseErrorType
jvx_measure_ir_process(jvxMeasureIr* handle)
{
	jvxSize i,ii, startCnt = 0;
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvx_measure_ir_private* locHdl = NULL;
	jvxData absVal_num = 0;
	jvxData angle_num = 0;
	jvxData absVal_den = 0;
	jvxData angle_den = 0;
	jvxData absValResult = 0;
	jvxData angleResult  = 0;
	jvxData tmp = 0;

	locHdl = (jvx_measure_ir_private*)handle->prv;
	startCnt = JVX_MIN(locHdl->prm_init.skipNumber, locHdl->prm_init.numReps-1); ;

	// Prepare the folded signal
	if(locHdl->prm_init.computeIr)
	{
		for(i = startCnt; i < locHdl->prm_init.szTestsignal_onerep; i++)
		{
			locHdl->runtime.superposedRecording_ir[i] = 0.0;
			locHdl->runtime.superposedTestSignal_ir[i] = 0.0;

			for(ii = 0; ii < locHdl->prm_init.numReps; ii++)
			{
				locHdl->runtime.superposedRecording_ir[i] += locHdl->prm_sync.fldBufferMeasureSignal[i + ii * locHdl->prm_init.szTestsignal_onerep];
				locHdl->runtime.superposedTestSignal_ir[i] += locHdl->prm_sync.fldBufferTestSignal[i + ii * locHdl->prm_init.szTestsignal_onerep];
			}
			locHdl->runtime.superposedRecording_ir[i] /= (jvxData) (locHdl->prm_init.numReps-startCnt);
			locHdl->runtime.superposedTestSignal_ir[i] /= (jvxData) (locHdl->prm_init.numReps-startCnt);
		}

		if (locHdl->prm_init.storeSuperPosed)
		{
			assert(locHdl->prm_init.szTestsignal_onerep == locHdl->prm_sync.lIResponse);
			memcpy(locHdl->prm_sync.fldSPosedMeasure, locHdl->runtime.superposedRecording_ir, sizeof(jvxData) * locHdl->prm_sync.lIResponse);
			memcpy(locHdl->prm_sync.fldSPosedTest, locHdl->runtime.superposedTestSignal_ir, sizeof(jvxData) * locHdl->prm_sync.lIResponse);
		}

		res = jvx_execute_fft(locHdl->runtime.fftTestSignal_ir);
		assert(res == JVX_DSP_NO_ERROR);
		res = jvx_execute_fft(locHdl->runtime.fftMeasured_ir);
		assert(res == JVX_DSP_NO_ERROR);

		for(ii = 0; ii < locHdl->runtime.fftSize2_p1; ii++)
		{
			absVal_num = 0;
			angle_num = 0;
			absVal_den = 0;
			angle_den = 0;

			jvx_complex_cart_2_polar(locHdl->runtime.cplx_Measured_ir[ii].re, locHdl->runtime.cplx_Measured_ir[ii].im, &absVal_num, &angle_num);
			jvx_complex_cart_2_polar(locHdl->runtime.cplx_TestSignal_ir[ii].re, locHdl->runtime.cplx_TestSignal_ir[ii].im, &absVal_den, &angle_den);

			absValResult = absVal_num/absVal_den;
			angleResult = angle_num-angle_den;

			jvx_complex_polar_2_cart(absValResult, angleResult, &locHdl->runtime.cplx_Ir[ii].re, &locHdl->runtime.cplx_Ir[ii].im);
		}

		if (locHdl->prm_sync.ext.freq_buf && locHdl->prm_sync.ext.gain_buf)
		{
			jvxSize binIdxStart = 0;
			jvxData gain = 0;
			jvxData freq = 0;
			for (i = 0; i < locHdl->prm_sync.ext.freq_gain_len; i++)
			{
				if (locHdl->prm_sync.ext.gain_buf[i] != gain)
				{		
					jvxSize binIdxEnd = 0;
					if (locHdl->prm_sync.ext.gain_buf[i] == 0)
					{
						freq = locHdl->prm_sync.ext.freq_buf[i];
						binIdxEnd = 1;
					}
					binIdxEnd += JVX_DATA2SIZE((freq * locHdl->runtime.fftSize)/ (jvxData)locHdl->prm_sync.ext.sRate);
					assert(binIdxEnd <= locHdl->runtime.fftSize2_p1);
					//binIdxEnd = JVX_MIN(binIdxEnd, locHdl->runtime.fftSize2_p1);
					for (ii = binIdxStart; ii < binIdxEnd; ii++)
					{
						locHdl->runtime.cplx_Ir[ii].re *= gain;
						locHdl->runtime.cplx_Ir[ii].im *= gain;
					}
					binIdxStart = binIdxEnd;
					gain = locHdl->prm_sync.ext.gain_buf[i];					
				}
				freq = locHdl->prm_sync.ext.freq_buf[i];
			}
			for (ii = binIdxStart; ii < locHdl->runtime.fftSize2_p1; ii++)
			{
				locHdl->runtime.cplx_Ir[ii].re *= gain;
				locHdl->runtime.cplx_Ir[ii].im *= gain;
			}
		}

		res = jvx_execute_ifft(locHdl->runtime.ifftIr);
		assert(res == JVX_DSP_NO_ERROR);

		tmp = 1.0/(jvxData)locHdl->prm_sync.lIResponse;

		for(i = 0; i < locHdl->prm_sync.lIResponse; i++)
		{
			locHdl->prm_sync.fldIResponse[i] *= tmp;
		}
	}

	if(locHdl->prm_init.computeXc)
	{
		for(i = startCnt; i < locHdl->prm_init.szTestsignal_onerep; i++)
		{
			locHdl->runtime.superposedRecording_xc[i] = 0.0;
			locHdl->runtime.superposedTestSignal_xc[i] = 0.0;

			for(ii = 0; ii < locHdl->prm_init.numReps; ii++)
			{
				locHdl->runtime.superposedRecording_xc[i] += locHdl->prm_sync.fldBufferMeasureSignal[i + ii * locHdl->prm_init.szTestsignal_onerep];
				locHdl->runtime.superposedTestSignal_xc[i] += locHdl->prm_sync.fldBufferTestSignal[i + ii * locHdl->prm_init.szTestsignal_onerep];
			}
			locHdl->runtime.superposedRecording_xc[i] /= (jvxData) (locHdl->prm_init.numReps-startCnt);
			locHdl->runtime.superposedTestSignal_xc[i] /= (jvxData) (locHdl->prm_init.numReps-startCnt);
		}

		res = jvx_execute_fft(locHdl->runtime.fftTestSignal_xc);
		assert(res == JVX_DSP_NO_ERROR);
		res = jvx_execute_fft(locHdl->runtime.fftMeasured_xc);
		assert(res == JVX_DSP_NO_ERROR);

		for(ii = 0; ii < locHdl->runtime.fftSize22_p1; ii++)
		{

			locHdl->runtime.cplx_CCorr[ii].re = locHdl->runtime.cplx_Measured_xc[ii].re* locHdl->runtime.cplx_TestSignal_xc[ii].re  + locHdl->runtime.cplx_Measured_xc[ii].im * locHdl->runtime.cplx_TestSignal_xc[ii].im;
			locHdl->runtime.cplx_CCorr[ii].im = locHdl->runtime.cplx_TestSignal_xc[ii].re * locHdl->runtime.cplx_Measured_xc[ii].im - locHdl->runtime.cplx_TestSignal_xc[ii].im * locHdl->runtime.cplx_Measured_xc[ii].re;
		}

		res = jvx_execute_ifft(locHdl->runtime.ifftCCorr);
		assert(res == JVX_DSP_NO_ERROR);
	}

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_measure_ir_postprocess(jvxMeasureIr* handle)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvx_measure_ir_private* newHdl = NULL;

	newHdl = (jvx_measure_ir_private*) handle->prv;

	if(newHdl->prm_init.computeIr)
	{
		res = jvx_destroy_fft(newHdl->runtime.fftTestSignal_ir);
		assert(res == JVX_DSP_NO_ERROR);

		res = jvx_destroy_fft(newHdl->runtime.fftMeasured_ir);
		assert(res == JVX_DSP_NO_ERROR);

		res = jvx_destroy_ifft(newHdl->runtime.ifftIr);
		assert(res == JVX_DSP_NO_ERROR);
	}

	if (newHdl->prm_init.storeSuperPosed)
	{
		JVX_DSP_SAFE_DELETE_FIELD(newHdl->prm_sync.fldSPosedMeasure);
		JVX_DSP_SAFE_DELETE_FIELD(newHdl->prm_sync.fldSPosedTest);
	}

	if(newHdl->prm_init.computeXc)
	{

		res = jvx_destroy_fft(newHdl->runtime.fftTestSignal_xc);
		assert(res == JVX_DSP_NO_ERROR);

		res = jvx_destroy_fft(newHdl->runtime.fftMeasured_xc);
		assert(res == JVX_DSP_NO_ERROR);

		res = jvx_destroy_ifft(newHdl->runtime.ifftCCorr);
		assert(res == JVX_DSP_NO_ERROR);
	}


	JVX_DSP_SAFE_DELETE_FIELD(newHdl->prm_sync.fldBufferMeasureSignal);
	JVX_DSP_SAFE_DELETE_FIELD(newHdl->prm_sync.fldBufferTestSignal);

	newHdl->prm_sync.lRecording = 0;

	res = jvx_destroy_fft_ifft_global(newHdl->runtime.glob_fft);
	assert(res == JVX_DSP_NO_ERROR);

	JVX_DSP_SAFE_DELETE_OBJECT(newHdl);

	handle->prv = NULL;
	return JVX_DSP_NO_ERROR;
}


#if 0
RTP_API_RETURN_TYPE
rtp_calibrate_delay_init(	rtpHandle** handleOnReturn, size_t szTestsignal_onerep, size_t numReps, rtpDataFormat format,
DATATYPE_ARITHMETIC_USED** fldBufferTestSignal, DATATYPE_ARITHMETIC_USED** recording, bool fftPowerTwo)
{
	if(handleOnReturn && fldBufferTestSignal && recording)
	{
		if(format == RTP_DATAFORMAT_DOUBLE)
		{
			rtp_calibrateDelay_handle* H = new rtp_calibrateDelay_handle;
			H->sigSize_onerep = szTestsignal_onerep;
			H->sigSize_record = H->sigSize_onerep * numReps;
			H->numRepetitions = numReps;

			if(fftPowerTwo)
			{
				DATATYPE_ARITHMETIC_USED tmp = (DATATYPE_ARITHMETIC_USED)(log((DATATYPE_ARITHMETIC_USED)(H->sigSize_onerep*2))/log(2.0));
				H->fftSize = (size_t)1 << DOUBLE2INT32(ceil(tmp));
				H->fftSize2_p1 = (H->fftSize >> 1) + 1;
			}
			else
			{
				H->fftSize = H->sigSize_onerep;
				if(H->fftSize % 2)
				{
					H->fftSize++;
				}
				H->fftSize2_p1 = (H->fftSize >> 1) + 1;
			}

			RTP_DSP_SAFE_ALLOCATE_FIELD(H->inputBuffer_Record_reps, DATATYPE_ARITHMETIC_USED, H->sigSize_record);
			memset(H->inputBuffer_Record_reps, 0, sizeof(DATATYPE_ARITHMETIC_USED) * H->sigSize_record);
			RTP_DSP_SAFE_ALLOCATE_FIELD(H->inputBuffer_Test_reps, DATATYPE_ARITHMETIC_USED, H->sigSize_record);
			memset(H->inputBuffer_Test_reps, 0, sizeof(DATATYPE_ARITHMETIC_USED) * H->sigSize_record);

#ifdef GLOBAL_DATA_FORMAT_DOUBLE

			H->inputBuffer_Test_onerep = (DATATYPE_ARITHMETIC_USED*) fftw_malloc(sizeof(DATATYPE_ARITHMETIC_USED) * H->fftSize);
			memset(H->inputBuffer_Test_onerep, 0, sizeof(DATATYPE_ARITHMETIC_USED) * H->fftSize);

			H->inputBuffer_Record_onerep = (DATATYPE_ARITHMETIC_USED*) fftw_malloc(sizeof(DATATYPE_ARITHMETIC_USED) * H->fftSize);
			memset(H->inputBuffer_Record_onerep, 0, sizeof(DATATYPE_ARITHMETIC_USED) * H->fftSize);

			H->outputBuffer_Corr_onerep = (DATATYPE_ARITHMETIC_USED*) fftw_malloc(sizeof(DATATYPE_ARITHMETIC_USED) * H->fftSize);
			memset(H->outputBuffer_Corr_onerep, 0, sizeof(DATATYPE_ARITHMETIC_USED) * H->fftSize);

			H->fft_Test = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * H->fftSize2_p1 );
			memset(H->fft_Test, 0, sizeof(fftw_complex) * H->fftSize2_p1);
			H->fft_Record = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * H->fftSize2_p1 );
			memset(H->fft_Record, 0, sizeof(fftw_complex) * H->fftSize2_p1);
			H->ifft_Corr = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * H->fftSize2_p1);
			memset(H->ifft_Corr, 0, sizeof(fftw_complex) * H->fftSize2_p1);
#else
			H->inputBuffer_Test_onerep = (DATATYPE_ARITHMETIC_USED*) fftwf_malloc(sizeof(DATATYPE_ARITHMETIC_USED) * H->fftSize);
			memset(H->inputBuffer_Test_onerep, 0, sizeof(DATATYPE_ARITHMETIC_USED) * H->fftSize);

			H->inputBuffer_Record_onerep = (DATATYPE_ARITHMETIC_USED*) fftwf_malloc(sizeof(DATATYPE_ARITHMETIC_USED) * H->fftSize);
			memset(H->inputBuffer_Record_onerep, 0, sizeof(DATATYPE_ARITHMETIC_USED) * H->fftSize);

			H->outputBuffer_Corr_onerep = (DATATYPE_ARITHMETIC_USED*) fftwf_malloc(sizeof(DATATYPE_ARITHMETIC_USED) * H->fftSize);
			memset(H->outputBuffer_Corr_onerep, 0, sizeof(DATATYPE_ARITHMETIC_USED) * H->fftSize);

			H->fft_Test = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * H->fftSize2_p1 );
			memset(H->fft_Test, 0, sizeof(fftwf_complex) * H->fftSize2_p1);
			H->fft_Record = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * H->fftSize2_p1 );
			memset(H->fft_Record, 0, sizeof(fftwf_complex) * H->fftSize2_p1);
			H->ifft_Corr = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * H->fftSize2_p1);
			memset(H->ifft_Corr, 0, sizeof(fftwf_complex) * H->fftSize2_p1);
#endif

#ifdef GLOBAL_DATA_FORMAT_DOUBLE
			H->pfft_Test = fftw_plan_dft_r2c_1d(H->fftSize, H->inputBuffer_Test_onerep, H->fft_Test, FFTW_ESTIMATE);
			H->pfft_Record  = fftw_plan_dft_r2c_1d(H->fftSize, H->inputBuffer_Record_onerep, H->fft_Record, FFTW_ESTIMATE);
			H->pifft_Corr= fftw_plan_dft_c2r_1d(H->fftSize, H->ifft_Corr, H->outputBuffer_Corr_onerep, FFTW_ESTIMATE);
#else
			H->pfft_Test = fftwf_plan_dft_r2c_1d(H->fftSize, H->inputBuffer_Test_onerep, H->fft_Test, FFTW_ESTIMATE);
			H->pfft_Record  = fftwf_plan_dft_r2c_1d(H->fftSize, H->inputBuffer_Record_onerep, H->fft_Record, FFTW_ESTIMATE);
			H->pifft_Corr= fftwf_plan_dft_c2r_1d(H->fftSize, H->ifft_Corr, H->outputBuffer_Corr_onerep, FFTW_ESTIMATE);
#endif

			*handleOnReturn = (rtpHandle*)H;
			*fldBufferTestSignal = H->inputBuffer_Test_reps;
			*recording = H->inputBuffer_Record_reps;

			RTP_API_RETURN_POSITIVE;
		}
		RTP_API_RETURN_NEGATIVE(RTP_ERROR_OPERATION_NOT_SUPPORTED);
	}
	RTP_API_RETURN_NEGATIVE(RTP_ERROR_INVALID_POINTER);
}

RTP_API_RETURN_TYPE
rtp_calibrate_delay_process(rtpHandle* handleProcess, rtpInt32* delay, DATATYPE_ARITHMETIC_USED* probability, rtpInt16* signalInverted)
{
	size_t ii;
	size_t i;
	bool signChange = false;

	if(handleProcess && delay && probability && signalInverted)
	{
		rtp_calibrateDelay_handle* H = (rtp_calibrateDelay_handle*)handleProcess;

		// Prepare the folded signal
		for(i = 0; i < H->sigSize_onerep; i++)
		{
			H->inputBuffer_Record_onerep[i] = 0.0;
			H->inputBuffer_Test_onerep[i] = 0.0;
			for(ii = 0; ii < H->numRepetitions; ii++)
			{
				H->inputBuffer_Record_onerep[i] += H->inputBuffer_Record_reps[i + ii * H->sigSize_onerep];
				H->inputBuffer_Test_onerep[i] += H->inputBuffer_Test_reps[i + ii * H->sigSize_onerep];
			}
			H->inputBuffer_Record_onerep[i] /= (DATATYPE_ARITHMETIC_USED) H->numRepetitions;
			H->inputBuffer_Test_onerep[i] /= (DATATYPE_ARITHMETIC_USED) H->numRepetitions;
		}

		// FFTs
#ifdef GLOBAL_DATA_FORMAT_DOUBLE
		fftw_execute(H->pfft_Test);
		fftw_execute(H->pfft_Record);
#else
		fftwf_execute(H->pfft_Test);
		fftwf_execute(H->pfft_Record);
#endif

		for(ii = 0; ii < H->fftSize2_p1; ii++)
		{
				H->ifft_Corr[ii][0] = H->fft_Record[ii][0]*H->fft_Test[ii][0] + H->fft_Record[ii][1]*H->fft_Test[ii][1];
				H->ifft_Corr[ii][1] = H->fft_Test[ii][0]*H->fft_Record[ii][1] - H->fft_Test[ii][1]*H->fft_Record[ii][0];
		}

#ifdef GLOBAL_DATA_FORMAT_DOUBLE
		fftw_execute(H->pifft_Corr);
#else
		fftwf_execute(H->pifft_Corr);
#endif

		int idxMax = 0;
		DATATYPE_ARITHMETIC_USED maxVal = 0.0;

		// Find first maximum
		for(ii = 0; ii < H->sigSize_onerep; ii++)
		{
			DATATYPE_ARITHMETIC_USED tmp = abs(H->outputBuffer_Corr_onerep[ii]);
			if( tmp > maxVal)
			{
				idxMax = ii;
				maxVal = tmp;
			}
		}

		if(H->outputBuffer_Corr_onerep[idxMax] < 0)
		{
			signChange = true;
		}

		// Exclude the first values around the maximum from search buffer
		// (We exclude the area defined as a local maximum)
		size_t idxLeft = idxMax-1;
		size_t idxRight = idxMax+1;
		double oldVal = (H->outputBuffer_Corr_onerep[idxMax]);
		if(signChange)
		{
		  while(/*(idxLeft >= 0)&&*/(	(H->outputBuffer_Corr_onerep[idxLeft]) > oldVal) && (H->outputBuffer_Corr_onerep[idxLeft] <= 0)) // idxLeft is unsigned
			{
				oldVal = (H->outputBuffer_Corr_onerep[idxLeft]);
				H->outputBuffer_Corr_onerep[idxLeft] = 0;
				idxLeft ++;
			}

			oldVal = (H->outputBuffer_Corr_onerep[idxMax]);
			while((idxRight < H->sigSize_onerep)&&((H->outputBuffer_Corr_onerep[idxRight]) > oldVal) && (H->outputBuffer_Corr_onerep[idxRight] <= 0))
			{
				oldVal = (H->outputBuffer_Corr_onerep[idxRight]);
				H->outputBuffer_Corr_onerep[idxRight] = 0;
				idxRight++;
			}
			H->outputBuffer_Corr_onerep[idxMax] = 0;
		}
		else
		{
		  while(/*(idxLeft >= 0)&&*/(	(H->outputBuffer_Corr_onerep[idxLeft]) < oldVal) && (H->outputBuffer_Corr_onerep[idxLeft] >= 0)) // idxLeft is unsigned
			{
				oldVal = (H->outputBuffer_Corr_onerep[idxLeft]);
				H->outputBuffer_Corr_onerep[idxLeft] = 0;
				idxLeft ++;
			}

			oldVal = (H->outputBuffer_Corr_onerep[idxMax]);
			while((idxRight < H->sigSize_onerep)&&((H->outputBuffer_Corr_onerep[idxRight]) < oldVal) && (H->outputBuffer_Corr_onerep[idxRight] >= 0))
			{
				oldVal = (H->outputBuffer_Corr_onerep[idxRight]);
				H->outputBuffer_Corr_onerep[idxRight] = 0;
				idxRight++;
			}
			H->outputBuffer_Corr_onerep[idxMax] = 0;
		}

		// Search for next
		DATATYPE_ARITHMETIC_USED maxVal2 = 0.0;
		int idxMax2 = 0;

		// Search second maximum
		for(ii = 0; ii < H->sigSize_onerep; ii++)
		{
			DATATYPE_ARITHMETIC_USED tmp = abs(H->outputBuffer_Corr_onerep[ii]);
			if( tmp > maxVal2)
			{
				idxMax2 = ii;
				maxVal2 = tmp;
			}
		}

		*delay = idxMax;
		*probability = (maxVal - maxVal2)/maxVal;
		*signalInverted = signChange;

		RTP_API_RETURN_POSITIVE;
	}
	RTP_API_RETURN_NEGATIVE(RTP_ERROR_INVALID_POINTER);
}

RTP_API_RETURN_TYPE
rtp_calibrate_copy_impulse_response(rtpHandle* handleProcess, DATATYPE_ARITHMETIC_USED* impulse_response, size_t numElements, size_t offset_copy_start, rtpBool skipFirst)
{
	size_t i,ii;
	bool signChange = false;
	int startCnt = 0;
	int divCount = 1;
	if(handleProcess && impulse_response)
	{
		rtp_calibrateDelay_handle* H = (rtp_calibrateDelay_handle*)handleProcess;

		divCount = H->numRepetitions;

		if(skipFirst)
		{
			startCnt = 1;
			divCount--;
		}

		if(divCount == 0)
		{
			RTP_API_RETURN_NEGATIVE(RTP_ERROR_INVALID_ARGUMENT);
		}

		if(offset_copy_start + numElements <= H->fftSize)
		{
			// Prepare the folded signal
			for(i = 0; i < H->sigSize_onerep; i++)
			{
				H->inputBuffer_Record_onerep[i] = 0.0;
				H->inputBuffer_Test_onerep[i] = 0.0;
				for(ii = startCnt; ii < H->numRepetitions; ii++)
				{
					H->inputBuffer_Record_onerep[i] += H->inputBuffer_Record_reps[i + ii * H->sigSize_onerep];
					H->inputBuffer_Test_onerep[i] += H->inputBuffer_Test_reps[i + ii * H->sigSize_onerep];
				}
				H->inputBuffer_Record_onerep[i] /= (DATATYPE_ARITHMETIC_USED) divCount;
				H->inputBuffer_Test_onerep[i] /= (DATATYPE_ARITHMETIC_USED) divCount;
			}

			// FFTs
#ifdef GLOBAL_DATA_FORMAT_DOUBLE
			fftw_execute(H->pfft_Test);
			fftw_execute(H->pfft_Record);
#else
			fftwf_execute(H->pfft_Test);
			fftwf_execute(H->pfft_Record);
#endif

			// Here, derive the impulse response
			for(ii = 0; ii < H->fftSize2_p1; ii++)
			{
				DATATYPE_ARITHMETIC_USED absVal_num = 0;
				DATATYPE_ARITHMETIC_USED angle_num = 0;
				DATATYPE_ARITHMETIC_USED absVal_den = 0;
				DATATYPE_ARITHMETIC_USED angle_den = 0;

				complex_cart_2_polar(H->fft_Record[ii][0], H->fft_Record[ii][1], &absVal_num, &angle_num);
				complex_cart_2_polar(H->fft_Test[ii][0], H->fft_Test[ii][1], &absVal_den, &angle_den);

				DATATYPE_ARITHMETIC_USED absValResult = absVal_num/absVal_den;
				DATATYPE_ARITHMETIC_USED angleResult = angle_num-angle_den;

				complex_polar_2_cart(absValResult, angleResult, &H->ifft_Corr[ii][0], &H->ifft_Corr[ii][1]);
			}

#ifdef GLOBAL_DATA_FORMAT_DOUBLE
			fftw_execute(H->pifft_Corr);
#else
			fftwf_execute(H->pifft_Corr);
#endif
			memcpy(impulse_response, H->outputBuffer_Corr_onerep + offset_copy_start, sizeof(DATATYPE_ARITHMETIC_USED) * numElements);

			// Normalize to FFT length
			for(i = 0; i < numElements; i++)
			{
				impulse_response[i] /= (DATATYPE_ARITHMETIC_USED)H->fftSize;
			}

			RTP_API_RETURN_POSITIVE;
		}
		RTP_API_RETURN_NEGATIVE(RTP_ERROR_INVALID_ARGUMENT);
	}
	RTP_API_RETURN_NEGATIVE(RTP_ERROR_INVALID_POINTER);
}

RTP_API_RETURN_TYPE
rtp_calibrate_delay_terminate(rtpHandle* handle)
{
	if(handle)
	{
		rtp_calibrateDelay_handle* H = (rtp_calibrateDelay_handle*)handle;

#ifdef GLOBAL_DATA_FORMAT_DOUBLE
		fftw_destroy_plan(H->pfft_Test);
		fftw_destroy_plan(H->pfft_Record);
		fftw_destroy_plan(H->pifft_Corr);

		fftw_free(H->fft_Test);
		fftw_free(H->fft_Record);
		fftw_free(H->ifft_Corr);

		fftw_free(H->inputBuffer_Test_onerep);
		fftw_free(H->inputBuffer_Record_onerep);
		fftw_free(H->outputBuffer_Corr_onerep);
#else
		fftwf_destroy_plan(H->pfft_Test);
		fftwf_destroy_plan(H->pfft_Record);
		fftwf_destroy_plan(H->pifft_Corr);

		fftwf_free(H->fft_Test);
		fftwf_free(H->fft_Record);
		fftwf_free(H->ifft_Corr);

		fftwf_free(H->inputBuffer_Test_onerep);
		fftwf_free(H->inputBuffer_Record_onerep);
		fftwf_free(H->outputBuffer_Corr_onerep);
#endif

		RTP_DSP_SAFE_DELETE_FIELD(H->inputBuffer_Record_reps);
		RTP_DSP_SAFE_DELETE_FIELD(H->inputBuffer_Test_reps);

		delete(H);
		RTP_API_RETURN_POSITIVE;
	}
	RTP_API_RETURN_NEGATIVE(RTP_ERROR_INVALID_POINTER);
}

#endif
