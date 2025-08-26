#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "jvx_beampattern/jvx_beampattern.h"
#include "jvx_beampattern/jvx_beampattern_prv.h"
#include "jvx_dsp_base.h"
#include "jvx_windows/jvx_windows.h"
#include "jvx_math/jvx_math_extensions.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_tools/jvx_fft_tools.h"
#include "jvx_debugout.h"
#include "jvx_misc/jvx_rangecheck.h"
#include "jvx_fft_tools/jvx_fft_core.h"

static void
allocate_derived(jvx_beampattern_cfg_derived *derived)
{
}

static void
deallocate_derived(jvx_beampattern_cfg_derived *derived)
{
}

static void
allocate_ram(jvx_beampattern_ram *ram)
{
	ram->testSignals = NULL; // will be allocated in update function
	ram->response = NULL; // will be allocated in update function
	ram->responseFFT = NULL; // will be allocated in update function
	ram->result = NULL; // will be allocated in update function
}

static void
deallocate_ram(jvx_beampattern_ram *ram)
{
	JVX_DSP_SAFE_DELETE_FIELD(ram->response);
	JVX_DSP_SAFE_DELETE_FIELD(ram->responseFFT);
}

static void
allocate_state(jvx_beampattern_state *state)
{
}


static void
deallocate_state(jvx_beampattern_state *state)
{
}


static jvxSize getSpectrumSize(jvx_beampattern *hdl)
{
	jvx_beampattern_prv* prv;

	if (!hdl)
		return 0;

	prv = (jvx_beampattern_prv*)hdl->prv;

	if (!prv)
		return (jvxSize)(pow(2, hdl->prmSync.fftLengthLog2) / 2. + 1);

	return (jvxSize)(pow(2, prv->prmSync.fftLengthLog2) / 2. + 1);
}

static jvxDspBaseErrorType
allocateResultBuffer(jvx_beampattern *hdl)
{
	jvx_beampattern_prv* prv;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_beampattern_prv*)hdl->prv;
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->ram.result, jvxData,
		prv->prmSync.numElevationAngles *
		prv->prmSync.numAzimuthAngles *
		prv->derived.spectrumSize);
	return JVX_DSP_NO_ERROR;
}

static jvxDspBaseErrorType
deallocateResultBuffer(jvx_beampattern *hdl)
{
	jvx_beampattern_prv* prv;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_beampattern_prv*)hdl->prv;

	JVX_DSP_SAFE_DELETE_FIELD(prv->ram.result);

	return JVX_DSP_NO_ERROR;
}

static jvxDspBaseErrorType
deallocateTestSignals(jvx_beampattern* hdl)
{
	jvx_beampattern_prv* prv;
	int ia, ie, im;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_beampattern_prv*)hdl->prv;

	JVX_DSP_SAFE_DELETE_FIELD(prv->ram.anglesElevation);
	JVX_DSP_SAFE_DELETE_FIELD(prv->ram.anglesAzimuth);

	if (prv->ram.testSignals)
	{
		for (ie = 0; ie < prv->prmSync.numElevationAngles; ie++)
		{
			for (ia = 0; ia < prv->prmSync.numAzimuthAngles; ia++)
			{
				for (im = 0; im < prv->mics.num; im++)
				{
					JVX_DSP_SAFE_DELETE_FIELD(prv->ram.testSignals[ie][ia][im]);
				}
				JVX_DSP_SAFE_DELETE_FIELD(prv->ram.testSignals[ie][ia]);
			}
			JVX_DSP_SAFE_DELETE_FIELD(prv->ram.testSignals[ie]);
		}
		JVX_DSP_SAFE_DELETE_FIELD(prv->ram.testSignals);
	}

	return JVX_DSP_NO_ERROR;
}

static jvxDspBaseErrorType
allocateTestSignals(jvx_beampattern* hdl)
{
	jvxData* ham_window;
	int fracDelayOrder;
	jvxData maxDistance = 0.;
	int im, ie, ia, j;
	jvxData delta;
	jvxData removeDistance = 0.;
	jvx_beampattern_prv* prv;
	struct microphone *mic;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_beampattern_prv*)hdl->prv;

	// does not make sense without mics
	if (prv->mics.num == 0)
		return JVX_DSP_NO_ERROR;

	// create a window
	fracDelayOrder = 2 * prv->prmSync.fracDelayFilterDelay;
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ham_window, jvxData, fracDelayOrder + 1);
	jvx_compute_window(ham_window, fracDelayOrder + 1, 0, 0, JVX_WINDOW_HAMMING, NULL);

	// find maximum microphone distance
	mic = prv->mics.first;
	while (mic)
	{
		jvxData sqdistance = mic->x * mic->x + mic->y * mic->y + mic->y + mic->z;
		if (sqdistance > maxDistance)
			maxDistance = sqdistance;
		mic = mic->next;
	}
	maxDistance = sqrt(maxDistance);
	if (maxDistance > prv->prmSync.radiusCircleAround)
	{
		JVX_DSP_SAFE_DELETE_FIELD(ham_window);
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	// Compute elevation angles
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->ram.anglesElevation, jvxData, prv->prmSync.numElevationAngles);
	if (prv->prmSync.numElevationAngles > 1)
	{
		delta = (prv->prmSync.elevationMax - prv->prmSync.elevationMin) / (jvxData)(prv->prmSync.numElevationAngles - 1);
		for (ie = 0; ie < prv->prmSync.numElevationAngles; ie++)
			prv->ram.anglesElevation[ie] = (prv->prmSync.elevationMin + ie * delta) / 180.0 * M_PI;
	}
	else
	{
		prv->ram.anglesElevation[0] = (prv->prmSync.elevationMin + prv->prmSync.elevationMax) / 2.0 / 180. * M_PI;
	}

	// Compute azimuth angles
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->ram.anglesAzimuth, jvxData, prv->prmSync.numAzimuthAngles);
	delta = (prv->prmSync.azimuthMax - prv->prmSync.azimuthMin) / (jvxData)(prv->prmSync.numAzimuthAngles - 1);
	for (ia = 0; ia < prv->prmSync.numAzimuthAngles; ia++)
	{
		prv->ram.anglesAzimuth[ia] = (prv->prmSync.azimuthMin + ia * delta) / 180.0 * M_PI;
	}

	// compute "removeDistance" for removing leading zeros
	if (prv->prmSync.removePropagationToArray)
		removeDistance = prv->prmSync.radiusCircleAround - maxDistance;

	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->ram.testSignals, jvxData***, prv->prmSync.numElevationAngles);
	for (ie = 0; ie < prv->prmSync.numElevationAngles; ie++)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->ram.testSignals[ie], jvxData**, prv->prmSync.numAzimuthAngles);
		for (ia = 0; ia < prv->prmSync.numAzimuthAngles; ia++)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->ram.testSignals[ie][ia], jvxData*, prv->mics.num);
			mic = prv->mics.first;
			im = 0;
			while (mic)
			{
				jvxData angleElevation = prv->ram.anglesElevation[ie];
				jvxData angleAzimuth = prv->ram.anglesAzimuth[ia];
				jvxData zz = sin(angleElevation) * prv->prmSync.radiusCircleAround;
				jvxData rr = cos(angleElevation) * prv->prmSync.radiusCircleAround;
				jvxData xx = -sin(angleAzimuth) * rr;
				jvxData yy = cos(angleAzimuth) * rr;
				jvxData micDist;
				jvxData fracDelay;
				int intDelay;

				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->ram.testSignals[ie][ia][im], jvxData, prv->derived.frameSize);

				xx -= mic->x;
				yy -= mic->y;
				zz -= mic->z;

				micDist = sqrt(xx * xx + yy * yy + zz * zz) - removeDistance;

				fracDelay = micDist / prv->prmSync.speedOfSound * hdl->samplingRate;
				fracDelay += 1; // FIXME/BG: temporary fix to avoid idxTarget<0
				intDelay = (int)floor(fracDelay);
				fracDelay -= intDelay;

				for (j = 0; j < fracDelayOrder + 1; j++)
				{
					int idxTarget = intDelay + j;
					if ((idxTarget >= 0) && idxTarget < (prv->derived.frameSize))
					{
						jvxData x = -fracDelay - prv->prmSync.fracDelayFilterDelay + j;
						prv->ram.testSignals[ie][ia][im][idxTarget] = ham_window[j] * jvx_si(x*M_PI);
					}
					else
					{
						printf("[jvx_beampattern] problematic test signal buffersize/-offset (trying to write to %i in array of %i)\n", idxTarget, prv->derived.frameSize);
						break;
					}
				}
				mic = mic->next;
				im++;
			}
		}
	}

	JVX_DSP_SAFE_DELETE_FIELD(ham_window);
	return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType
jvx_beampattern_configInit(jvx_beampattern* hdl,
	unsigned int pid)
{
	hdl->prv = NULL;

	// const
	hdl->samplingRate = 32000;
	hdl->id = jvx_id(__func__, pid);
	hdl->pid = pid;
	hdl->description = JVX_BEAMPATTERN_DESCRIPTION;
	hdl->version = JVX_BEAMPATTERN_VERSION;
	hdl->frameSize = -1;
	hdl->nChannelsIn = -1;
	hdl->nChannelsOut = -1;

	// readOnly struct (static part)
	hdl->readOnly.frameCounter = 0;

	// async parameters
	// ... nothing at the moment ...

	// sync parameters
	hdl->prmSync.numAzimuthAngles = 45;
	hdl->prmSync.numElevationAngles = 1;
	hdl->prmSync.fftLengthLog2 = 8;
	hdl->prmSync.radiusCircleAround = 10;
	hdl->prmSync.fracDelayFilterDelay = 32;
	hdl->prmSync.azimuthMin = 90.;
	hdl->prmSync.azimuthMax = 180.;
	hdl->prmSync.elevationMin = -90.;
	hdl->prmSync.elevationMax = 90.;
	hdl->prmSync.removePropagationToArray = true;
	hdl->prmSync.speedOfSound = 343.;

	// configInit of submodules
	// ... nothing at the moment ...

	// copy to readOnly struct
	hdl->readOnly.spectrumSize = JVX_SIZE_INT16(getSpectrumSize(hdl));

	return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType
jvx_beampattern_init(jvx_beampattern* hdl,
	int samplingRate,
	jvxHandle *bf_hdl,
	jvx_beampattern_process_cb bf_cb)
{
	jvx_beampattern_prv* prv = NULL;

	// allocate private part and hook into struct
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(prv, jvx_beampattern_prv);
	hdl->prv = (jvxHandle*)prv;

	// check arguments
	// ... nothing at the moment ...

	// set const configuration from params
	hdl->samplingRate = samplingRate;

	// private data
	allocate_ram(&prv->ram);
	allocate_state(&prv->state);
	allocate_derived(&prv->derived);

	// init of submodules
	prv->mics.first = NULL;
	prv->mics.num = 0;
	prv->mics.maxId = -1;
	prv->bf_hdl = bf_hdl;
	prv->bf_cb = bf_cb;
	prv->FFT_global_hdl = prv->FFT_hdl = NULL; // will be allocated in update function

	// trigger parameter update
	jvx_beampattern_update(hdl, true);

	// print init results
	// ... nothing at the moment ...

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_beampattern_addMicrophone(jvx_beampattern* hdl,
	jvxData x,
	jvxData y,
	jvxData z,
	jvxSize *id)
{
	jvx_beampattern_prv* prv;
	struct microphone *p;
	jvxDspBaseErrorType res;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_beampattern_prv*)hdl->prv;

	res = deallocateTestSignals(hdl);
	if (res != JVX_DSP_NO_ERROR)
		return res;

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(p, struct microphone);
	p->x = x;
	p->y = y;
	p->z = z;
	p->id = ++prv->mics.maxId;
	if (id)
		*id = p->id;
	p->next = prv->mics.first;
	prv->mics.first = p;

	prv->mics.num++;

	res = allocateTestSignals(hdl);
	if (res != JVX_DSP_NO_ERROR)
		return res;

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_beampattern_printMicrophones(jvx_beampattern* hdl)
{
	jvx_beampattern_prv* prv;
	struct microphone *p;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_beampattern_prv*)hdl->prv;

	p = prv->mics.first;
	while (p)
	{
		printf("[jvx_beampattern] Mic %i @ position (%g, %g, %g)\n", p->id, p->x, p->y, p->z);
		p = p->next;
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_beampattern_removeAllMicrophones(jvx_beampattern* hdl)
{
	jvx_beampattern_prv* prv;
	struct microphone *p1, *p2;
	jvxDspBaseErrorType res;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_beampattern_prv*)hdl->prv;

	res = deallocateTestSignals(hdl);
	if (res != JVX_DSP_NO_ERROR)
		return res;

	p1 = prv->mics.first;

	if (!p1)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	do
	{
		p2 = p1->next;
		prv->mics.num--;
		JVX_DSP_SAFE_DELETE_OBJECT(p1);
		p1 = p2;
	} while (p1);
	prv->mics.first = NULL;
	assert(prv->mics.num == 0);

	/* res = allocateTestSignals(hdl); */
	/* if(res != JVX_DSP_NO_ERROR) */
	/*   return res; */

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_beampattern_removeMicrophone(jvx_beampattern* hdl,
	jvxSize id)
{
	jvx_beampattern_prv* prv;
	struct microphone *p1, *p2;
	jvxDspBaseErrorType res;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_beampattern_prv*)hdl->prv;

	res = deallocateTestSignals(hdl);
	if (res != JVX_DSP_NO_ERROR)
		return res;

	p1 = prv->mics.first;

	if (!p1)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	if (p1->id == id)
	{
		prv->mics.first = p1->next;
		JVX_DSP_SAFE_DELETE_OBJECT(p1);
		prv->mics.num--;
		return JVX_DSP_NO_ERROR;
	}

	p2 = p1->next;
	do
	{
		if (p2->id == id)
		{
			p1->next = p2->next;
			JVX_DSP_SAFE_DELETE_OBJECT(p2);
			prv->mics.num--;
			break;
		}
		p1 = p2;
		p2 = p2->next;
	} while (p2);

	res = allocateTestSignals(hdl);
	if (res != JVX_DSP_NO_ERROR)
		return res;

	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_beampattern_process(jvx_beampattern* hdl,
	jvxData **out)
{
	int ie, ia;
	jvx_beampattern_prv* prv;
	int n;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_beampattern_prv*)hdl->prv;

	if (prv)
	{
		// FIXME/BG: computation around arbitrary perimeter of cnst. radius instead of entire sphere
		for (ie = 0; ie < prv->prmSync.numElevationAngles; ie++)
		{
			for (ia = 0; ia < prv->prmSync.numAzimuthAngles; ia++)
			{
				(*prv->bf_cb)(prv->bf_hdl, prv->derived.frameSize,
					prv->mics.num, hdl->samplingRate,
					prv->ram.testSignals[ie][ia], prv->ram.response);
				jvx_execute_fft(prv->FFT_hdl);
				for (n = 0; n < prv->derived.spectrumSize; n++)
				{
					prv->ram.result[ie * prv->derived.spectrumSize * prv->prmSync.numAzimuthAngles + ia * prv->derived.spectrumSize + n] =
						10 * log10(prv->ram.responseFFT[n].re * prv->ram.responseFFT[n].re
							+ prv->ram.responseFFT[n].im * prv->ram.responseFFT[n].im + 1e-4);
				}
			}
		}
		if (out)
			*out = prv->ram.result;

		// readOnly data update
		hdl->readOnly.frameCounter++;
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_beampattern_terminate(jvx_beampattern* hdl)
{
	jvx_beampattern_prv* prv;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_beampattern_prv*)hdl->prv;

	if (!prv)
		return JVX_DSP_NO_ERROR;

	deallocateResultBuffer(hdl);
	jvx_beampattern_removeAllMicrophones(hdl);

	deallocate_state(&prv->state);
	deallocate_ram(&prv->ram);
	deallocate_derived(&prv->derived);

	// submodules
	if (prv->FFT_hdl)
		jvx_destroy_fft(prv->FFT_hdl);
	if (prv->FFT_global_hdl)
		jvx_destroy_fft_ifft_global(prv->FFT_global_hdl);

	JVX_DSP_SAFE_DELETE_OBJECT(prv);
	hdl->prv = NULL;

	return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType jvx_beampattern_update(jvx_beampattern* hdl, jvxCBool syncUpdate)
{
	jvx_beampattern_prv* prv;
	jvx_beampattern_cfg_derived *derived;
	jvxDspBaseErrorType res;
	jvxSize fftSizeCheck;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	prv = (jvx_beampattern_prv*)hdl->prv;
	derived = &prv->derived;

	// parameter checks (sync & async)
	jvx_rangeCheck_int16(&hdl->prmSync.numElevationAngles, 1, 360, __func__, hdl->id, "numElevationAngles");
	jvx_rangeCheck_int16(&hdl->prmSync.numAzimuthAngles, 10, 360, __func__, hdl->id, "numAzimuthAngles");
	jvx_rangeCheck_int16(&hdl->prmSync.fftLengthLog2, 7, 13, __func__, hdl->id, "fftLengthLog2");
	jvx_rangeCheck_jvxData(&hdl->prmSync.radiusCircleAround, 0.1, 100., __func__, hdl->id, "radiusCircleAround");
	jvx_rangeCheck_int16(&hdl->prmSync.fracDelayFilterDelay, 1, 256, __func__, hdl->id, "fracDelayFilterDelay");
	jvx_rangeCheck_jvxData(&hdl->prmSync.azimuthMin, 0., 360., __func__, hdl->id, "azimuthMin");
	jvx_rangeCheck_jvxData(&hdl->prmSync.azimuthMax, 0., 360., __func__, hdl->id, "azimuthMax");
	jvx_rangeCheck_jvxData(&hdl->prmSync.azimuthMin, 0, hdl->prmSync.azimuthMax, __func__, hdl->id, "azimuthMax (2)");
	jvx_rangeCheck_jvxData(&hdl->prmSync.elevationMin, -90., 90., __func__, hdl->id, "elevationMin");
	jvx_rangeCheck_jvxData(&hdl->prmSync.elevationMax, -90., 90., __func__, hdl->id, "elevationMax");
	jvx_rangeCheck_jvxData(&hdl->prmSync.elevationMin, -90, hdl->prmSync.elevationMax, __func__, hdl->id, "elevationMax (2)");
	jvx_rangeCheck_jvxData(&hdl->prmSync.speedOfSound, 0.1, 10000., __func__, hdl->id, "speedOfSound");

	if (syncUpdate)
	{
		// this has to be done first since the old sizes have to be still known
		res = deallocateTestSignals(hdl);
		if (res != JVX_DSP_NO_ERROR)
			return res;

		// update synced prms
		prv->prmSync = hdl->prmSync;

		// derived prms depending on synced prms
		hdl->readOnly.spectrumSize = derived->spectrumSize = JVX_SIZE_INT(getSpectrumSize(hdl));
		derived->frameSize = (derived->spectrumSize - 1) * 2;
		res = allocateTestSignals(hdl);
		if (res != JVX_DSP_NO_ERROR)
			return res;

		jvxFFTSize szFft = JVX_FFT_TOOLS_FFT_SIZE_1024;
		res = jvx_get_nearest_size_fft(&szFft, derived->frameSize, JVX_FFT_ROUND_UP, NULL);
		assert(res == JVX_DSP_NO_ERROR);

		// reallocate response buffer
		JVX_DSP_SAFE_DELETE_FIELD(prv->ram.response);
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->ram.response, jvxData, prv->derived.frameSize);
		JVX_DSP_SAFE_DELETE_FIELD(prv->ram.responseFFT);
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->ram.responseFFT, jvxDataCplx, prv->derived.spectrumSize);

		// reallocate result buffer
		res = deallocateResultBuffer(hdl);
		if (res != JVX_DSP_NO_ERROR)
			return res;
		res = allocateResultBuffer(hdl);
		if (res != JVX_DSP_NO_ERROR)
			return res;

		// update submodules depending on synced prms
		if (prv->FFT_hdl)
		{
			jvx_destroy_fft(prv->FFT_hdl);
			prv->FFT_hdl = NULL;
		}
		if (prv->FFT_global_hdl)
		{
			jvx_destroy_fft_ifft_global(prv->FFT_global_hdl);
			prv->FFT_global_hdl = NULL;
		}

		jvx_create_fft_ifft_global(&prv->FFT_global_hdl, szFft, NULL);
		jvx_create_fft_real_2_complex(&prv->FFT_hdl,
			prv->FFT_global_hdl,
			szFft,
			NULL, NULL,
			&fftSizeCheck,
			JVX_FFT_IFFT_EFFICIENT,
			prv->ram.response,
			prv->ram.responseFFT, 0);
		assert(fftSizeCheck == derived->frameSize);


		// update readOnly data depending on synced prms
		hdl->readOnly.spectrum = prv->ram.result;

	}


	// update async prms
	prv->prm = hdl->prm;

	// derived prms not depending on synced prms
	// ... nothing at the moment ...

	// update submodules depending not depending on synced prms
	// ...nothing...

	return JVX_DSP_NO_ERROR;
}
