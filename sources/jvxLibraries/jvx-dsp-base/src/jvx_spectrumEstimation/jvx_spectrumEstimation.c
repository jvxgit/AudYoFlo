#include <assert.h>
/* #include <string.h> */
/* #include <stdio.h> */
#include <math.h>
#include "jvx_dsp_base.h"
#include "jvx_spectrumEstimation/jvx_spectrumEstimation.h"
#include "jvx_spectrumEstimation_prv.h"
#include "jvx_misc/jvx_rangecheck.h"
#include <float.h>


static void
allocate_derived(jvx_spectrumEstimation_cfg_derived *derived)
{
	derived->window = NULL; // will be allocated in update function
}

static void
deallocate_derived(jvx_spectrumEstimation_cfg_derived *derived)
{
	JVX_DSP_SAFE_DELETE_FIELD(derived->window);
}

static void
allocate_ram(jvx_spectrumEstimation *hdl,
	jvx_spectrumEstimation_ram *ram)
{
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->tmpBuf, jvxData, hdl->frameSize);
}

static void
deallocate_ram(jvx_spectrumEstimation_ram *ram)
{
	JVX_DSP_SAFE_DELETE_FIELD(ram->tmpBuf);
}

static void
allocate_state(jvx_spectrumEstimation_state *state)
{
	state->psd = NULL; // will be allocated in update function
	state->logpsd = NULL; // will be allocated in update function
}


static void
deallocate_state(jvx_spectrumEstimation_state *state)
{
	JVX_DSP_SAFE_DELETE_FIELD(state->psd);
	JVX_DSP_SAFE_DELETE_FIELD(state->logpsd);
}

static jvxSize getSpectrumSize(jvx_spectrumEstimation *hdl)
{
	jvx_spectrumEstimation_prv* prv;

	if (!hdl)
		return 0;

	prv = (jvx_spectrumEstimation_prv*)hdl->prv;

	if (!prv)
		return (jvxSize)pow(2, hdl->prmSync.fftLengthLog2) / 2 + 1;

	return (jvxSize)pow(2, prv->prmSync.fftLengthLog2) / 2 + 1;
}

jvxDspBaseErrorType
jvx_spectrumEstimation_configInit(jvx_spectrumEstimation* hdl,
	unsigned int pid)
{
	hdl->prv = NULL;

	// const
	hdl->id = jvx_id(__func__, pid);
	hdl->pid = pid;
	hdl->frameSize = -1;
	hdl->samplingRate = 32000;
	hdl->description = JVX_SPECTRUMESTIMATION_DESCRIPTION;
	hdl->version = JVX_SPECTRUMESTIMATION_VERSION;
	hdl->nChannelsIn = 99;
	hdl->nChannelsOut = -1;

	// readOnly struct (static part)
	hdl->readOnly.frameCounter = 0;
	hdl->readOnly.spectrum = NULL;

	// async parameters
	// ... nothing at the moment ...

	// sync data
	hdl->prmSync.method = jvx_spectrumEstimation_welchRecursive;
	hdl->prmSync.logarithmic = true;
	hdl->prmSync.fftLengthLog2 = 8;
	hdl->prmSync.alpha = 0.75;
	hdl->prmSync.windowType = JVX_WINDOW_HAMMING;
	hdl->prmSync.winArgA = 0;
	hdl->prmSync.winArgB = 0;

	// configInit of submodules
	// ... nothing at the moment ...

	// copy to readOnly struct
	hdl->readOnly.spectrumSize = getSpectrumSize(hdl);


	return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType
jvx_spectrumEstimation_init(jvx_spectrumEstimation* hdl,
	int frameSize,
	int nChannelsIn,
	int samplingRate)
{
	jvx_spectrumEstimation_prv* prv = NULL;

	// allocate private part and hook into struct
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(prv, jvx_spectrumEstimation_prv);
	hdl->prv = (jvxHandle*)prv;

	// check arguments
	// ... nothing at the moment ...

	// set const configuration from params
	// the cnst struct must not be written anywhere else!
	hdl->frameSize = frameSize;
	hdl->samplingRate = samplingRate;
	hdl->nChannelsIn = nChannelsIn;

	// private data
	allocate_ram(hdl, &prv->ram);
	allocate_state(&prv->state);
	allocate_derived(&prv->derived);

	// init of submodules
	prv->cb = NULL; // will be allocated in update function
	jvx_circbuffer_allocate_global_fft_ifft(&prv->globalFFT, JVX_FFT_TOOLS_FFT_SIZE_8192); // global init with max size


	// trigger parameter update
	jvx_spectrumEstimation_update(hdl, true);

	// print init results
	// ... nothing at the moment ...

	return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType
jvx_spectrumEstimation_process(jvx_spectrumEstimation* hdl,
	jvxData* in,
	jvxData** out,
	jvxSize channel)
{
	int j;
	jvx_spectrumEstimation_prv* prv;
	jvxData x;
	jvxData *ptr;
	jvxData *logptr;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	if (channel >= hdl->nChannelsIn)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_spectrumEstimation_prv*)hdl->prv;

	ptr = prv->state.psd + channel*prv->derived.spectrumSize;
	logptr = prv->state.logpsd + channel*prv->derived.spectrumSize;

	// optionally pass through internal variable
	if (out)
	{
		if (prv->prmSync.logarithmic)
			*out = logptr;
		else
			*out = ptr;
	}

	// do the FFT
	for (j = 0; j < hdl->frameSize; j++)
		prv->ram.tmpBuf[j] = in[j] * prv->derived.window[j] * prv->derived.winNormalize;
	jvx_circbuffer_remove(&(*prv->cb[channel]).circBuffer, hdl->frameSize);
	jvx_circbuffer_write_update(&(*prv->cb[channel]).circBuffer, (const jvxData**)&prv->ram.tmpBuf, hdl->frameSize);
	jvx_circbuffer_fill(&(*prv->cb[channel]).circBuffer, 0, (jvxInt32)(prv->derived.fftLength - hdl->frameSize));
	jvx_circbuffer_process_fft_ifft(prv->cb[channel]);
	
	jvxDataCplx* cplxBuf = NULL;
	jvx_circbuffer_access_cplx_fft_ifft(prv->cb[channel], &cplxBuf, NULL, 0);

	switch (hdl->prmSync.method)
	{
	case jvx_spectrumEstimation_welchRecursive:
		for (j = 0; j < prv->derived.spectrumSize; j++)
		{
			x = cplxBuf[j].re * cplxBuf[j].re + cplxBuf[j].im * cplxBuf[j].im;
			ptr[j] = x + prv->prmSync.alpha * (ptr[j] - x);
			if (isnan(ptr[j]))
			{
				ptr[j] = 0;
				/* printf("[%s] WARNING: nan occured\n",__func__); */
			}
		}
		break;

	case jvx_spectrumEstimation_instantaneous:
		for (j = 0; j < prv->derived.spectrumSize; j++)
		{
			ptr[j] = cplxBuf[j].re * cplxBuf[j].re + cplxBuf[j].im * cplxBuf[j].im;
			if (isnan(ptr[j]))
			{
				ptr[j] = 0;
				/* printf("[%s] WARNING: nan occured\n",__func__); */
			}
		}
		break;

	default:
		assert(0);
	}


	if (prv->prmSync.logarithmic)
		for (j = 0; j < prv->derived.spectrumSize; j++)
			logptr[j] = 10 * log10(ptr[j] + 1e-4);


	// update readOnly struct
	hdl->readOnly.frameCounter++;

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_spectrumEstimation_terminate(jvx_spectrumEstimation* hdl)
{
	jvx_spectrumEstimation_prv* prv;
	int i;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_spectrumEstimation_prv*)hdl->prv;

	if (!prv)
		return JVX_DSP_NO_ERROR;

	deallocate_state(&prv->state);
	deallocate_ram(&prv->ram);
	deallocate_derived(&prv->derived);

	// submodules
	for (i = 0; i < hdl->nChannelsIn; i++)
		jvx_circbuffer_deallocate_fft_ifft(prv->cb[i]);
	JVX_DSP_SAFE_DELETE_FIELD(prv->cb);
	jvx_circbuffer_destroy_global_fft_ifft(prv->globalFFT);

	JVX_DSP_SAFE_DELETE_OBJECT(prv);
	hdl->prv = NULL;

	return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType
jvx_spectrumEstimation_update(jvx_spectrumEstimation* hdl,
	jvxCBool syncUpdate)
{
	jvx_spectrumEstimation_prv* prv;
	jvx_spectrumEstimation_cfg_derived *derived;
	int i;
	jvxFFTSize szFft;
	jvxDspBaseErrorType res;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_spectrumEstimation_prv*)hdl->prv;
	derived = &prv->derived;

	// parameter checks (sync & async)
	jvx_rangeCheck_jvxData(&hdl->prmSync.alpha, 0, 1. - 1e-4, __func__, hdl->id, "alpha");
	jvx_rangeCheck_int16(&hdl->prmSync.fftLengthLog2, (jvxInt16)(ceil(log(hdl->frameSize) / log(2))), 13, __func__, hdl->id, "fftLengthLog2");
	// FIXME/BG: for now we are limited to a fixed frame shift (i.e. fixed overlap with given fftLength)

	if (syncUpdate)
	{
		// update synced prms
		prv->prmSync = hdl->prmSync;

		// derived prms depending on synced prms
		derived->fftLength = (jvxSize)pow(2, prv->prmSync.fftLengthLog2);

		szFft = JVX_FFT_TOOLS_FFT_SIZE_1024;
		res = jvx_get_nearest_size_fft(&szFft, prv->derived.fftLength, JVX_FFT_ROUND_UP, NULL);
		assert(res == JVX_DSP_NO_ERROR);

		hdl->readOnly.spectrumSize = derived->spectrumSize = getSpectrumSize(hdl);
		JVX_DSP_SAFE_DELETE_FIELD(derived->window);
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(derived->window, jvxData, hdl->frameSize);
		jvx_compute_window(derived->window, hdl->frameSize,
			prv->prmSync.winArgA, prv->prmSync.winArgB,
			prv->prmSync.windowType, &derived->winNormalize);
		if (prv->cb)
			for (i = 0; i < hdl->nChannelsIn; i++)
				jvx_circbuffer_deallocate_fft_ifft(prv->cb[i]);
		JVX_DSP_SAFE_DELETE_FIELD(prv->cb);
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->cb, jvx_circbuffer_fft*, hdl->nChannelsIn);

		for (i = 0; i < hdl->nChannelsIn; i++)
		{
			jvx_circbuffer_allocate_fft_ifft(&prv->cb[i], prv->globalFFT,
				JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX,
				szFft,
				true, 1);
			jvx_circbuffer_fill(&(*prv->cb[i]).circBuffer, 0, (jvxInt32)derived->fftLength);
		}

		if (prv->state.psd)
			JVX_DSP_SAFE_DELETE_FIELD(prv->state.psd);
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->state.psd, jvxData, hdl->nChannelsIn * derived->spectrumSize);

		if (prv->state.logpsd)
			JVX_DSP_SAFE_DELETE_FIELD(prv->state.logpsd);
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->state.logpsd, jvxData, hdl->nChannelsIn * derived->spectrumSize);


		// update submodules depending on synced prms
		// ... nothing at the moment ...

		// update readOnly data depending on synced prms
		if (prv->prmSync.logarithmic)
			hdl->readOnly.spectrum = prv->state.logpsd;
		else
			hdl->readOnly.spectrum = prv->state.psd;

	}


	// update async prms
	prv->prm = hdl->prm;

	// derived prms not depending on synced prms
	// ... nothing at the moment ...

	// update submodules depending not depending on synced prms
	// ... nothing at the moment ...

	return JVX_DSP_NO_ERROR;
}
