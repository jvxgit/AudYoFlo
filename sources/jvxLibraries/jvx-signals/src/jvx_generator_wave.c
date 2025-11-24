#include "jvx_generator_wave.h"
#include "jvx_dsp_base.h"
#include "math.h"
#include "jvx_math/jvx_math_extensions.h"
#include "jvx_windows/jvx_windows.h"
#include <assert.h>

#ifdef JVX_FFT_PRESENT
#include "jvx_fft_tools/jvx_fft_core.h"
#endif

#define M_PI_M_PI (2*M_PI)


// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 
// CORE STRUCTS IN USE CORE STRUCTS IN USE CORE STRUCTS IN USE CORE STRUCTS IN USE
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 

typedef struct
{
	jvxGeneratorWaveType tpWave;
	//jvxGeneratorWaveStatus status;
	struct
	{
		size_t samples_produced;
	} runtime;
} commonObjectStatus;

//! Struct for sine waves
typedef struct
{
	// This struct must be at first to do pointer typecasts to <jvx_generator_wave_common_parameter_init>
	commonObjectStatus common_data;

	jvx_generator_wave_sine_parameter_init init_parameters_intern;
	jvx_generator_wave_sine_parameter_runtime_async runtime_parameters_async_intern;
	jvx_generator_wave_sine_parameter_runtime_sync runtime_parameters_sync_intern;

} jvx_generator_wave_sine_parameter_runtime_private;

//! Struct for rect waves
typedef struct
{
	// This struct must be at first to do pointer typecasts to <jvx_generator_wave_common_parameter_init>
	commonObjectStatus common_data;

	jvx_generator_wave_rect_parameter_init init_parameters_intern;
	jvx_generator_wave_rect_parameter_runtime_async runtime_parameters_async_intern;
	jvx_generator_wave_rect_parameter_runtime_sync runtime_parameters_sync_intern;
	struct
	{
		jvxData phase_invert;
	} runtime;
} jvx_generator_wave_rect_parameter_runtime_private;

//! Struct for ramp waves
typedef struct
{
	// This struct must be at first to do pointer typecasts to <jvx_generator_wave_common_parameter_init>
	commonObjectStatus common_data;

	jvx_generator_wave_ramp_parameter_init init_parameters_intern;
	jvx_generator_wave_ramp_parameter_runtime_async runtime_parameters_async_intern;
	jvx_generator_wave_ramp_parameter_runtime_sync runtime_parameters_sync_intern;
	struct
	{
		jvxData phase_invert1;
		jvxData phase_invert2;
		jvxData delta_phase_1_inv;
		jvxData delta_phase_2_inv;
		jvxData delta_phase_3_inv;
	} runtime;
} jvx_generator_wave_ramp_parameter_runtime_private;

//! Struct for noise waves
typedef struct
{
	// This struct must be at first to do pointer typecasts to <jvx_generator_wave_common_parameter_init>
	commonObjectStatus common_data;

	jvx_generator_wave_noise_parameter_init init_parameters_intern;
	jvx_generator_wave_noise_parameter_runtime_async runtime_parameters_async_intern;
	struct
	{
		jvxInt32 seed;
	} runtime;
} jvx_generator_wave_noise_parameter_runtime_private;

#ifdef JVX_FFT_PRESENT

//! Struct for perfectsweep waves
typedef struct
{
	// This struct must be at first to do pointer typecasts to <jvx_generator_wave_common_parameter_init>
	commonObjectStatus common_data;

	jvx_generator_wave_perfectsweep_parameter_init init_parameters_intern;
	jvx_generator_wave_perfectsweep_parameter_runtime_async runtime_parameters_async_intern;
	jvx_generator_wave_perfectsweep_parameter_runtime_sync runtime_parameters_sync_intern;

	struct
	{
		jvxHandle* theIfft;
		jvxDataCplx* inCplx;
		jvxData* outReal;
	} generate;

	struct
	{
		size_t position;
		jvxData* fieldSamples;
	} runtime;
} jvx_generator_wave_perfectsweep_parameter_runtime_private;

#endif

//! Struct for perfectsweep waves
typedef struct
{
	// This struct must be at first to do pointer typecasts to <jvx_generator_wave_common_parameter_init>
	commonObjectStatus common_data;

	jvx_generator_wave_wavplayer_parameter_init init_parameters_intern;
	jvx_generator_wave_wavplayer_parameter_runtime_async runtime_parameters_async_intern;
	jvx_generator_wave_wavplayer_parameter_runtime_sync runtime_parameters_sync_intern;

	struct
	{
		size_t position;
		jvxData oldVal;
		jvxData gain;
		//jvxData* fieldSamples;
	} runtime;
} jvx_generator_wave_wavplayer_parameter_runtime_private;


//! Struct for linsweep waves
typedef struct
{
	// This struct must be at first to do pointer typecasts to <jvx_generator_wave_common_parameter_init>
	commonObjectStatus common_data;

	jvx_generator_wave_linlogsweep_parameter_init init_parameters_intern;
	jvx_generator_wave_linlogsweep_parameter_runtime_async runtime_parameters_async_intern;
	jvx_generator_wave_linlogsweep_parameter_runtime_sync runtime_parameters_sync_intern;

	struct
	{
		size_t position;
		size_t start_position_sweep;
		size_t stop_position_sweep;
		jvxData inst_frequency;
		jvxData linlogFreqMin;
		jvxData linlogFreqMax;
		jvxData linlogFreqGateMin;
		jvxData linlogFreqGateMax;
		jvxData freq_increment;
		jvxData phase;
		jvxData div_samplerate;
		jvxData oldVal;
		jvxData gain;
		struct
		{
			jvxData* freqBuffer;
			jvxData* gainBuffer;
			jvxSize lSeg;
			jvxSize idxOneSegment;
		} metaData;
	} runtime;
} jvx_generator_wave_linlogsweep_parameter_runtime_private;

// ================================================================================
// Initialize config
// ================================================================================

void
jvx_generator_wave_init_config_generator_unbuffered_sine(jvx_generatorWave* hdl)
{
	hdl->prm_init.sineWave.periodic.common.samplerate = 48000;

	hdl->prm_async.sineWave.periodic.common.amplitude = 1.0;
	hdl->prm_async.sineWave.periodic.frequency = 1000;

	hdl->prm_sync.sineWave.periodic.phase = 0.0;

	hdl->prv = NULL;
}

void
jvx_generator_wave_init_config_generator_unbuffered_rect(jvx_generatorWave* hdl)
{
	hdl->prm_init.rectWave.periodic.common.samplerate = 48000;

	hdl->prm_async.rectWave.periodic.common.amplitude = 1.0;
	hdl->prm_async.rectWave.periodic.frequency = 1000;
	hdl->prm_async.rectWave.pulsewidth = 0.5;

	hdl->prm_sync.rectWave.periodic.phase = 0.0;

	hdl->prv = NULL;
}

void
jvx_generator_wave_init_config_generator_unbuffered_ramp(jvx_generatorWave* hdl)
{
	hdl->prm_init.rampWave.periodic.common.samplerate = 48000;

	hdl->prm_async.rampWave.periodic.common.amplitude = 1.0;
	hdl->prm_async.rampWave.periodic.frequency = 1000;
	hdl->prm_async.rampWave.pulsewidth1 = 0.3;
	hdl->prm_async.rampWave.pulsewidth2 = 0.7;

	hdl->prm_sync.rampWave.periodic.phase = 0.0;

	hdl->prv = NULL;
}

void 
jvx_generator_wave_init_config_generator_unbuffered_noise(jvx_generatorWave* hdl)
{
	hdl->prm_init.noiseWave.common.samplerate = 48000;
	hdl->prm_init.noiseWave.seed = 0;

	hdl->prm_async.noiseWave.common.amplitude = 1.0;
	hdl->prm_async.noiseWave.noiseType = JVX_GENERATOR_NOISE_NORMAL;

	//hdl->prm_sync.noiseWave.;

	hdl->prv = NULL;
}

void 
jvx_generator_wave_init_config_generator_buffered_perfectsweep(jvx_generatorWave* hdl)
{
	hdl->prm_init.perfSweepWave.common.samplerate = 48000;
	hdl->prm_async.perfSweepWave.lengthSeconds = 10;
	hdl->prm_async.perfSweepWave.loopCount = -1;
	hdl->prm_async.perfSweepWave.silenceStartSeconds = 1;
	hdl->prm_async.perfSweepWave.silenceStopSeconds = 5;

	hdl->prm_async.perfSweepWave.common.amplitude = 1.0;

	/* Move those to shared */
	hdl->prm_sync.perfSweepWave.buffered.bufField = NULL;
	hdl->prm_sync.perfSweepWave.buffered.length = 0;
	hdl->prm_sync.perfSweepWave.buffered.progress = 0.0;
	hdl->prv = NULL;
}

void 
jvx_generator_wave_init_config_generator_buffered_wavplayer(jvx_generatorWave* hdl)
{
	hdl->prm_init.wavWave.common.samplerate = 48000;

	hdl->prm_async.wavWave.common.amplitude = 1.0;
	hdl->prm_async.wavWave.loopCount = -1;

	/* Move those to shared */
	hdl->prm_sync.wavWave.buffered.bufField = NULL;
	hdl->prm_sync.wavWave.buffered.length = 0;
	hdl->prm_sync.wavWave.buffered.progress = 0.0;
	hdl->prv = NULL;
}

void 
jvx_generator_wave_init_config_generator_unbuffered_linlogsweep(jvx_generatorWave* hdl)
{
	hdl->prm_init.linlogSweepWave.common.samplerate = 48000;
	hdl->prm_async.linlogSweepWave.fLow = 50;
	hdl->prm_async.linlogSweepWave.fUp = hdl->prm_init.linlogSweepWave.common.samplerate/2;
	hdl->prm_async.linlogSweepWave.fLowStart = hdl->prm_async.linlogSweepWave.fLow;
	hdl->prm_async.linlogSweepWave.fUpStop = hdl->prm_async.linlogSweepWave.fUp;

	hdl->prm_async.linlogSweepWave.lengthSeconds = 10;
	hdl->prm_async.linlogSweepWave.loopCount = -1;
	hdl->prm_async.linlogSweepWave.silenceStartSeconds = 1;
	hdl->prm_async.linlogSweepWave.silenceStopSeconds = 5;

	hdl->prm_async.linlogSweepWave.common.amplitude = 1.0;

	/* Move those to shared */
	hdl->prm_sync.linlogSweepWave.unbuffered.length = 0;
	hdl->prm_sync.linlogSweepWave.unbuffered.progress = 0;

	hdl->prv = NULL;
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

jvxDspBaseErrorType 
jvx_generatorwave_initConfig(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	
	if(hdl == NULL)
	{
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	//hdl->tpWave = JVX_GENERATOR_WAVE_SINE_UNBUFFERED;
	hdl->prv = NULL;
	
	memset(&hdl->prm_init, 0, sizeof(hdl->prm_init));
	memset(&hdl->prm_async, 0, sizeof(hdl->prm_async));
	memset(&hdl->prm_sync, 0, sizeof(hdl->prm_sync));

	switch(hdl->tpWave)
	{
	case JVX_GENERATOR_WAVE_SINE_UNBUFFERED:
		jvx_generator_wave_init_config_generator_unbuffered_sine(hdl);
		break;

	case JVX_GENERATOR_WAVE_RECT_UNBUFFERED:
		jvx_generator_wave_init_config_generator_unbuffered_rect(hdl);
		res = JVX_DSP_NO_ERROR;
		break;

	case JVX_GENERATOR_WAVE_RAMP_UNBUFFERED:
		jvx_generator_wave_init_config_generator_unbuffered_ramp(hdl);
		res = JVX_DSP_NO_ERROR;
		break;
	
	case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:
		jvx_generator_wave_init_config_generator_unbuffered_noise(hdl);
		res = JVX_DSP_NO_ERROR;
		break;

#ifdef JVX_FFT_PRESENT
	case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
		jvx_generator_wave_init_config_generator_buffered_perfectsweep(hdl);
		res = JVX_DSP_NO_ERROR;
		break;
#endif

	case JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED:
		jvx_generator_wave_init_config_generator_buffered_wavplayer(hdl);
		res = JVX_DSP_NO_ERROR;
		break;
	
	case JVX_GENERATOR_WAVE_LOGSWEEP:
	case JVX_GENERATOR_WAVE_LINEARSWEEP:
		jvx_generator_wave_init_config_generator_unbuffered_linlogsweep(hdl);
		res = JVX_DSP_NO_ERROR;
		break;
	}

	return(res);
}

// ================================================================================
// Activate module
// ================================================================================

jvxDspBaseErrorType 
jvx_generator_wave_init_generator_unbuffered_sine(jvx_generatorWave* hdl)
{
	jvx_generator_wave_sine_parameter_runtime_private* newHdl = NULL;

	//jvx_generator_wave_rect_parameter_runtime_private* newHdl = new jvx_generator_wave_rect_parameter_runtime_private;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newHdl, jvx_generator_wave_sine_parameter_runtime_private);

	// Specific data fields
	newHdl->common_data.tpWave = JVX_GENERATOR_WAVE_SINE_UNBUFFERED;	
	newHdl->common_data.runtime.samples_produced = 0;

	// Copy the parameters
	memcpy(&newHdl->init_parameters_intern, &hdl->prm_init.sineWave, sizeof(jvx_generator_wave_sine_parameter_init));
	memcpy(&newHdl->runtime_parameters_async_intern, &hdl->prm_async.sineWave, sizeof(jvx_generator_wave_sine_parameter_runtime_async));
	memcpy(&newHdl->runtime_parameters_sync_intern, &hdl->prm_sync.sineWave, sizeof(jvx_generator_wave_sine_parameter_runtime_sync));

	newHdl->runtime_parameters_async_intern.periodic.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;
	hdl->prv = newHdl;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType 
jvx_generator_wave_init_generator_unbuffered_rect(jvx_generatorWave* hdl)
{
	jvx_generator_wave_rect_parameter_runtime_private* newHdl = NULL;

	//jvx_generator_wave_rect_parameter_runtime_private* newHdl = new jvx_generator_wave_rect_parameter_runtime_private;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newHdl, jvx_generator_wave_rect_parameter_runtime_private);

	newHdl->common_data.tpWave = JVX_GENERATOR_WAVE_RECT_UNBUFFERED;
	newHdl->common_data.runtime.samples_produced = 0;

	// Copy the parameters
	memcpy(&newHdl->init_parameters_intern, &hdl->prm_init.rectWave, sizeof(jvx_generator_wave_rect_parameter_init));
	memcpy(&newHdl->runtime_parameters_async_intern, &hdl->prm_async.rectWave, sizeof(jvx_generator_wave_rect_parameter_runtime_async));
	memcpy(&newHdl->runtime_parameters_sync_intern, &hdl->prm_sync.rectWave, sizeof(jvx_generator_wave_rect_parameter_runtime_sync));

	newHdl->runtime_parameters_async_intern.periodic.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;
	hdl->prv = newHdl;

	return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType 
jvx_generator_wave_init_generator_unbuffered_ramp(jvx_generatorWave* hdl)
{
	jvx_generator_wave_ramp_parameter_runtime_private* newHdl = NULL;

	//jvx_generator_wave_rect_parameter_runtime_private* newHdl = new jvx_generator_wave_rect_parameter_runtime_private;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newHdl, jvx_generator_wave_ramp_parameter_runtime_private);

	newHdl->common_data.tpWave = JVX_GENERATOR_WAVE_RAMP_UNBUFFERED;
	newHdl->common_data.runtime.samples_produced = 0;

	// Copy the parameters
	memcpy(&newHdl->init_parameters_intern, &hdl->prm_init.rampWave, sizeof(jvx_generator_wave_ramp_parameter_init));
	memcpy(&newHdl->runtime_parameters_async_intern, &hdl->prm_async.rampWave, sizeof(jvx_generator_wave_ramp_parameter_runtime_async));
	memcpy(&newHdl->runtime_parameters_sync_intern, &hdl->prm_sync.rampWave, sizeof(jvx_generator_wave_ramp_parameter_runtime_sync));

	newHdl->runtime_parameters_async_intern.periodic.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;
	newHdl->runtime.phase_invert1 = JVX_DOUBLE_2_DATA(newHdl->runtime_parameters_async_intern.pulsewidth1 * M_PI_M_PI);
	newHdl->runtime.phase_invert2 = JVX_DOUBLE_2_DATA(newHdl->runtime_parameters_async_intern.pulsewidth1 * M_PI_M_PI);
	newHdl->runtime.delta_phase_1_inv = JVX_DOUBLE_2_DATA(1.0/newHdl->runtime.phase_invert1);
	newHdl->runtime.delta_phase_2_inv = JVX_DOUBLE_2_DATA(1.0/(newHdl->runtime.phase_invert2-newHdl->runtime.phase_invert1));
	newHdl->runtime.delta_phase_3_inv = JVX_DOUBLE_2_DATA(1.0/(M_PI_M_PI-newHdl->runtime.phase_invert1));

	hdl->prv = newHdl;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType 
jvx_generator_wave_init_generator_unbuffered_noise(jvx_generatorWave* hdl)
{
	jvx_generator_wave_noise_parameter_runtime_private* newHdl = NULL;

	//jvx_generator_wave_rect_parameter_runtime_private* newHdl = new jvx_generator_wave_rect_parameter_runtime_private;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newHdl, jvx_generator_wave_noise_parameter_runtime_private);

	newHdl->common_data.tpWave = JVX_GENERATOR_WAVE_NOISE_UNBUFFERED;
	newHdl->common_data.runtime.samples_produced = 0;

	// Copy the parameters
	memcpy(&newHdl->init_parameters_intern, &hdl->prm_init.noiseWave, sizeof(jvx_generator_wave_noise_parameter_init));
	memcpy(&newHdl->runtime_parameters_async_intern, &hdl->prm_async.noiseWave, sizeof(jvx_generator_wave_noise_parameter_runtime_async));
	//memcpy(&newHdl->runtime_parameters_sync_intern, &hdl->prm_sync.rampWave, sizeof(jvx_generator_wave_ramp_parameter_runtime_sync));

	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;
	hdl->prv = newHdl;
	return JVX_DSP_NO_ERROR;
}

#ifdef JVX_FFT_PRESENT

jvxDspBaseErrorType 
jvx_generator_wave_init_generator_buffered_perfectsweep(jvx_generatorWave* hdl)
{
	jvx_generator_wave_perfectsweep_parameter_runtime_private* newHdl = NULL;

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newHdl, jvx_generator_wave_perfectsweep_parameter_runtime_private);

	newHdl->common_data.tpWave = JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED;
	newHdl->common_data.runtime.samples_produced = 0;

	// Copy the parameters
	memcpy(&newHdl->init_parameters_intern, &hdl->prm_init.perfSweepWave, sizeof(jvx_generator_wave_perfectsweep_parameter_init));
	memcpy(&newHdl->runtime_parameters_async_intern, &hdl->prm_async.perfSweepWave, sizeof(jvx_generator_wave_perfectsweep_parameter_runtime_async));
	memcpy(&newHdl->runtime_parameters_sync_intern, &hdl->prm_sync.perfSweepWave, sizeof(jvx_generator_wave_perfectsweep_parameter_runtime_sync));

	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;

	newHdl->generate.inCplx = NULL;
	newHdl->generate.theIfft = NULL;

	newHdl->runtime.fieldSamples = NULL;
	newHdl->runtime.position = 0;

	hdl->prv = newHdl;
	return JVX_DSP_NO_ERROR;
}


#endif

jvxDspBaseErrorType 
jvx_generator_wave_init_generator_buffered_wavplayer(jvx_generatorWave* hdl)
{
	jvx_generator_wave_wavplayer_parameter_runtime_private* newHdl = NULL;

	//jvx_generator_wave_rect_parameter_runtime_private* newHdl = new jvx_generator_wave_rect_parameter_runtime_private;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newHdl, jvx_generator_wave_wavplayer_parameter_runtime_private);

	newHdl->common_data.tpWave = JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED;
	newHdl->common_data.runtime.samples_produced = 0;

	// Copy the parameters
	memcpy(&newHdl->init_parameters_intern, &hdl->prm_init.noiseWave, sizeof(jvx_generator_wave_noise_parameter_init));
	memcpy(&newHdl->runtime_parameters_async_intern, &hdl->prm_async.noiseWave, sizeof(jvx_generator_wave_noise_parameter_runtime_async));
	//memcpy(&newHdl->runtime_parameters_sync_intern, &hdl->prm_sync.rampWave, sizeof(jvx_generator_wave_ramp_parameter_runtime_sync));

	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;
	hdl->prv = newHdl;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType 
jvx_generator_wave_init_generator_unbuffered_linlogsweep(jvx_generatorWave* hdl)
{
	jvx_generator_wave_linlogsweep_parameter_runtime_private* newHdl = NULL;

	//jvx_generator_wave_rect_parameter_runtime_private* newHdl = new jvx_generator_wave_rect_parameter_runtime_private;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newHdl, jvx_generator_wave_linlogsweep_parameter_runtime_private);

	newHdl->common_data.tpWave = hdl->tpWave;
	newHdl->common_data.runtime.samples_produced = 0;

	// Copy the parameters
	memcpy(&newHdl->init_parameters_intern, &hdl->prm_init.linlogSweepWave, sizeof(jvx_generator_wave_linlogsweep_parameter_init));
	memcpy(&newHdl->runtime_parameters_async_intern, &hdl->prm_async.linlogSweepWave, sizeof(jvx_generator_wave_linlogsweep_parameter_runtime_async));
	memcpy(&newHdl->runtime_parameters_sync_intern, &hdl->prm_sync.linlogSweepWave, sizeof(jvx_generator_wave_linlogsweep_parameter_runtime_sync));

	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;
	newHdl->runtime.position = 0;
	newHdl->runtime.phase = 0.0;
	newHdl->runtime.position = 0;
	newHdl->runtime.start_position_sweep = 0;
	newHdl->runtime.stop_position_sweep = 0;
	newHdl->runtime.div_samplerate = 1.0;
	newHdl->runtime.freq_increment = 0;
	newHdl->runtime.inst_frequency = 0;
	newHdl->runtime.linlogFreqMax = 0;
	newHdl->runtime.linlogFreqMin = 0;

	hdl->prv = newHdl;
	return JVX_DSP_NO_ERROR;
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


jvxDspBaseErrorType 
jvx_generatorwave_activate(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	
	if(hdl == NULL)
	{
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	switch(hdl->tpWave)
	{
	case JVX_GENERATOR_WAVE_SINE_UNBUFFERED:
		res = jvx_generator_wave_init_generator_unbuffered_sine(hdl);
		break;

	case JVX_GENERATOR_WAVE_RECT_UNBUFFERED:
		res = jvx_generator_wave_init_generator_unbuffered_rect(hdl);
		break;

	case JVX_GENERATOR_WAVE_RAMP_UNBUFFERED:
		res = jvx_generator_wave_init_generator_unbuffered_ramp(hdl);
		break;

	case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:
		res = jvx_generator_wave_init_generator_unbuffered_noise(hdl);
		break;

#ifdef JVX_FFT_PRESENT
	case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
		res = jvx_generator_wave_init_generator_buffered_perfectsweep(hdl);
		break;
#endif

	case JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED:
		res = jvx_generator_wave_init_generator_buffered_wavplayer(hdl);
		break;

	case JVX_GENERATOR_WAVE_LOGSWEEP:
	case JVX_GENERATOR_WAVE_LINEARSWEEP:
		res = jvx_generator_wave_init_generator_unbuffered_linlogsweep(hdl);
		break;

	}
	
	return(res);
}

// ================================================================================
// Restart module
// ================================================================================

jvxDspBaseErrorType
jvx_generator_wave_restart_generator_unbuffered_sine(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	jvx_generator_wave_sine_parameter_runtime_private* newHdl = (jvx_generator_wave_sine_parameter_runtime_private*)hdl->prv;
	return(res);
}

jvxDspBaseErrorType
jvx_generator_wave_restart_generator_unbuffered_rect(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	jvx_generator_wave_rect_parameter_runtime_private* newHdl = (jvx_generator_wave_rect_parameter_runtime_private*)hdl->prv;
	return(res);
}

jvxDspBaseErrorType
jvx_generator_wave_restart_generator_unbuffered_ramp(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	jvx_generator_wave_ramp_parameter_runtime_private* newHdl = (jvx_generator_wave_ramp_parameter_runtime_private*)hdl->prv;
	return(res);
}

jvxDspBaseErrorType
jvx_generator_wave_restart_generator_unbuffered_noise(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	jvx_generator_wave_noise_parameter_runtime_private* newHdl = (jvx_generator_wave_noise_parameter_runtime_private*)hdl->prv;
	return(res);
}

#ifdef JVX_FFT_PRESENT
jvxDspBaseErrorType
jvx_generator_wave_restart_generator_buffered_perfectsweep(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	jvx_generator_wave_perfectsweep_parameter_runtime_private* newHdl = (jvx_generator_wave_perfectsweep_parameter_runtime_private*)hdl->prv;
	return(res);
}
#endif

jvxDspBaseErrorType
jvx_generator_wave_restart_generator_buffered_wavplayer(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	jvx_generator_wave_wavplayer_parameter_runtime_private* newHdl = (jvx_generator_wave_wavplayer_parameter_runtime_private*)hdl->prv;
	return(res);
}

jvxDspBaseErrorType
jvx_generator_wave_restart_generator_unbuffered_linlogsweep(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvx_generator_wave_linlogsweep_parameter_runtime_private* newHdl = (jvx_generator_wave_linlogsweep_parameter_runtime_private*)hdl->prv;

	newHdl->runtime.inst_frequency = newHdl->runtime.linlogFreqMin;
	newHdl->runtime_parameters_sync_intern.unbuffered.progress = 0.0;
	newHdl->common_data.runtime.samples_produced = 0;
	newHdl->runtime.oldVal = -1;
	newHdl->runtime.gain = 0;
	newHdl->runtime.phase = 0;
	newHdl->runtime.position = 0;
	newHdl->runtime_parameters_sync_intern.itCount = 0;
	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STARTED;
	newHdl->runtime.metaData.idxOneSegment = 0;

	return(res);
}

// ================================================================================
// Start module
// ================================================================================

jvxDspBaseErrorType
jvx_generator_wave_start_generator_unbuffered_sine(jvx_generatorWave* hdl)
{
	jvx_generator_wave_sine_parameter_runtime_private* newHdl = (jvx_generator_wave_sine_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.periodic.common.theStatus = JVX_GENERATOR_WAVE_STARTED;
	newHdl->common_data.runtime.samples_produced = 0;
	return JVX_DSP_NO_ERROR;
}
		
jvxDspBaseErrorType
jvx_generator_wave_start_generator_unbuffered_rect(jvx_generatorWave* hdl)
{
	jvx_generator_wave_rect_parameter_runtime_private* newHdl = (jvx_generator_wave_rect_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.periodic.common.theStatus = JVX_GENERATOR_WAVE_STARTED;
	newHdl->common_data.runtime.samples_produced = 0;

	newHdl->runtime.phase_invert = JVX_DOUBLE_2_DATA(newHdl->runtime_parameters_async_intern.pulsewidth * M_PI_M_PI);

	return JVX_DSP_NO_ERROR;

}

jvxDspBaseErrorType
jvx_generator_wave_start_generator_unbuffered_ramp(jvx_generatorWave* hdl)
{
	jvx_generator_wave_ramp_parameter_runtime_private* newHdl = (jvx_generator_wave_ramp_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.periodic.common.theStatus = JVX_GENERATOR_WAVE_STARTED;
	newHdl->common_data.runtime.samples_produced = 0;
	
	newHdl->runtime.phase_invert1 = JVX_DOUBLE_2_DATA(newHdl->runtime_parameters_async_intern.pulsewidth1 * M_PI_M_PI);
	newHdl->runtime.phase_invert2 = JVX_DOUBLE_2_DATA(newHdl->runtime_parameters_async_intern.pulsewidth2 * M_PI_M_PI);
	newHdl->runtime.delta_phase_1_inv = JVX_DOUBLE_2_DATA(1.0/newHdl->runtime.phase_invert1);
	newHdl->runtime.delta_phase_2_inv = JVX_DOUBLE_2_DATA(1.0/(newHdl->runtime.phase_invert2-newHdl->runtime.phase_invert1));
	newHdl->runtime.delta_phase_3_inv = JVX_DOUBLE_2_DATA(1.0/(M_PI_M_PI-newHdl->runtime.phase_invert1));
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_generator_wave_start_generator_unbuffered_noise(jvx_generatorWave* hdl)
{
	jvx_generator_wave_noise_parameter_runtime_private* newHdl = (jvx_generator_wave_noise_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STARTED;
	newHdl->common_data.runtime.samples_produced = 0;
	
	newHdl->runtime.seed = -(signed)newHdl->init_parameters_intern.seed;
	return JVX_DSP_NO_ERROR;
}

#ifdef JVX_FFT_PRESENT

jvxDspBaseErrorType
jvx_generator_wave_start_generator_buffered_perfectsweep(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	size_t i;
	size_t lengthSamplesNetto = 0;
	size_t silenceStart = 0;
	size_t silenceStop = 0;
	size_t lengthSamplesBrutto  = 0;
	jvxData durationSecs = 0;
	jvxSize tmp1 = 0;
	jvxData deltaOmega = 0;
	jvxData grpDelay = 0;
	jvxData phase = 0;
	jvxData maxVal = 0;
	jvxInt32 fftType_int = 0;
	jvxFFTGlobal* glob_fft = NULL;

	jvx_generator_wave_perfectsweep_parameter_runtime_private* newHdl = (jvx_generator_wave_perfectsweep_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STARTED;
	newHdl->common_data.runtime.samples_produced = 0;

		
	// =================================================================
	// Produce perfect sweep here...
	// =================================================================

	lengthSamplesNetto = JVX_DATA2SIZE( newHdl->init_parameters_intern.common.samplerate * newHdl->runtime_parameters_async_intern.lengthSeconds);

	if(lengthSamplesNetto % 2)
	{
		lengthSamplesNetto --;
	}

	silenceStart = JVX_DATA2SIZE(newHdl->init_parameters_intern.common.samplerate * 
		newHdl->runtime_parameters_async_intern.silenceStartSeconds);
	silenceStop = JVX_DATA2SIZE(newHdl->init_parameters_intern.common.samplerate * 
		newHdl->runtime_parameters_async_intern.silenceStopSeconds);

	lengthSamplesBrutto =  silenceStart + lengthSamplesNetto + silenceStop;


	durationSecs = (jvxData)newHdl->runtime_parameters_async_intern.lengthSeconds;

	fftType_int = (int)(ceil(log((jvxData)lengthSamplesNetto)/log(2.0))) - JVX_OFFSET_FFT_TYPE_MIN;

	res = jvx_create_fft_ifft_global(&glob_fft, (jvxFFTSize) fftType_int);
	assert(res == JVX_DSP_NO_ERROR);

	res = jvx_create_ifft_complex_2_real(&newHdl->generate.theIfft, glob_fft, JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
						   &newHdl->generate.inCplx, &newHdl->generate.outReal, &tmp1, JVX_FFT_IFFT_EFFICIENT,
							NULL, NULL, lengthSamplesNetto);
	assert(res == JVX_DSP_NO_ERROR);

	deltaOmega = JVX_DOUBLE_2_DATA(M_PI_M_PI / durationSecs);

	grpDelay = 0;
	phase = 0;

	for(i = 0; i < lengthSamplesNetto/2; i++)
	{
		grpDelay = (jvxData) i / ((jvxData)lengthSamplesNetto/2) * durationSecs;
		phase = JVX_DOUBLE_2_DATA(0.5 * grpDelay * deltaOmega * (jvxData) i);

		newHdl->generate.inCplx[i].re = cos(phase);
		newHdl->generate.inCplx[i].im = -sin(phase);
	}
	newHdl->generate.inCplx[i].re = 1.0;
	newHdl->generate.inCplx[i].im = 0;

	res = jvx_execute_ifft(newHdl->generate.theIfft);
	assert(res == JVX_DSP_NO_ERROR);

	maxVal = -FLT_MAX;
	for(i = 0; i < lengthSamplesNetto; i++)
	{
		if(fabs(newHdl->generate.outReal[i]) > maxVal)
		{
			maxVal = fabs(newHdl->generate.outReal[i]);
		}
	}

	maxVal = JVX_DOUBLE_2_DATA(1.0/maxVal);

	for(i = 0; i < lengthSamplesNetto; i++)
	{
		newHdl->generate.outReal[i] *= maxVal;
	}

	// Allocate and fill the buffer for psweep
	newHdl->runtime.fieldSamples = NULL;
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newHdl->runtime.fieldSamples, jvxData, lengthSamplesBrutto);
	memset(newHdl->runtime.fieldSamples, 0, sizeof(jvxData) * lengthSamplesBrutto);
	memcpy(newHdl->runtime.fieldSamples + silenceStart, newHdl->generate.outReal, sizeof(jvxData) * lengthSamplesNetto);
	newHdl->runtime.position = 0;

	res = jvx_destroy_ifft(newHdl->generate.theIfft);
	assert(res == JVX_DSP_NO_ERROR);

	newHdl->generate.inCplx = NULL;
	newHdl->generate.outReal = NULL;
	newHdl->generate.inCplx = NULL;

	newHdl->runtime_parameters_sync_intern.buffered.progress = 0.0;
	newHdl->runtime_parameters_sync_intern.buffered.length = lengthSamplesBrutto;
	newHdl->runtime_parameters_sync_intern.buffered.bufField = newHdl->runtime.fieldSamples;
	newHdl->runtime_parameters_sync_intern.itCount = 0;

	res = jvx_destroy_fft_ifft_global(glob_fft);
	assert(res == JVX_DSP_NO_ERROR);

	return JVX_DSP_NO_ERROR;
}
#endif

jvxDspBaseErrorType
jvx_generator_wave_start_generator_buffered_wavplayer(jvx_generatorWave* hdl)
{
	jvx_generator_wave_wavplayer_parameter_runtime_private* newHdl = (jvx_generator_wave_wavplayer_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STARTED;
	newHdl->common_data.runtime.samples_produced = 0;

	//newHdl->runtime.fieldSamples = NULL;
	newHdl->runtime.position = 0;
	newHdl->runtime_parameters_sync_intern.itCount = 0;

	newHdl->runtime_parameters_sync_intern.buffered.progress = 0.0;
	newHdl->runtime_parameters_sync_intern.buffered.length = 0;
	newHdl->runtime_parameters_sync_intern.buffered.bufField = 0;
		
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_generator_wave_start_generator_unbuffered_linlogsweep(jvx_generatorWave* hdl)
{
	size_t lengthSamplesNetto = 0;
	size_t silenceStart = 0;
	size_t silenceStop = 0;
	size_t lengthSamplesBrutto = 0;
	jvxData durationSecs = 0.0;

	jvx_generator_wave_linlogsweep_parameter_runtime_private* newHdl = (jvx_generator_wave_linlogsweep_parameter_runtime_private*) hdl->prv;	

	// =================================================================
	// Produce linlog sweep here...
	// =================================================================

	lengthSamplesNetto = JVX_DATA2SIZE( newHdl->init_parameters_intern.common.samplerate * newHdl->runtime_parameters_async_intern.lengthSeconds);

	silenceStart = JVX_DATA2SIZE(newHdl->init_parameters_intern.common.samplerate * 
		newHdl->runtime_parameters_async_intern.silenceStartSeconds);
	silenceStop = JVX_DATA2SIZE(newHdl->init_parameters_intern.common.samplerate * 
		newHdl->runtime_parameters_async_intern.silenceStopSeconds);

	lengthSamplesBrutto =  silenceStart + lengthSamplesNetto + silenceStop;


	durationSecs = (jvxData)newHdl->runtime_parameters_async_intern.lengthSeconds;

	switch(newHdl->common_data.tpWave)
	{
	case JVX_GENERATOR_WAVE_LINEARSWEEP:
		newHdl->runtime.linlogFreqMax = (double)newHdl->init_parameters_intern.common.samplerate/2.0;
		newHdl->runtime.linlogFreqMin = 1.0;
		break;
	case JVX_GENERATOR_WAVE_LOGSWEEP:
		newHdl->runtime.linlogFreqMax = log((double)newHdl->init_parameters_intern.common.samplerate/2.0);
		newHdl->runtime.linlogFreqMin = log(1.0);
		break;
	}

	newHdl->runtime.freq_increment = (newHdl->runtime.linlogFreqMax - newHdl->runtime.linlogFreqMin)/((double)lengthSamplesNetto-1);

	newHdl->runtime.start_position_sweep = silenceStart;
	newHdl->runtime.stop_position_sweep = silenceStart + lengthSamplesNetto;
	newHdl->runtime.div_samplerate = 1.0/(double)newHdl->init_parameters_intern.common.samplerate;

	newHdl->runtime_parameters_sync_intern.unbuffered.length = lengthSamplesBrutto;
	newHdl->runtime_parameters_sync_intern.unbuffered.freq_ptr = NULL;

	newHdl->runtime.metaData.lSeg = lengthSamplesNetto;
	JVX_SAFE_ALLOCATE_FIELD_Z(newHdl->runtime.metaData.gainBuffer, jvxData, newHdl->runtime.metaData.lSeg);
	JVX_SAFE_ALLOCATE_FIELD_Z(newHdl->runtime.metaData.freqBuffer, jvxData, newHdl->runtime.metaData.lSeg);

	jvxSize idxStartRampUp = JVX_SIZE_UNSELECTED;
	jvxSize idxStartRampUpDone = JVX_SIZE_UNSELECTED;
	jvxSize idxStartRampDown = JVX_SIZE_UNSELECTED;
	jvxSize idxStartRampDownDone = JVX_SIZE_UNSELECTED;


	jvxData instFreq = newHdl->runtime.linlogFreqMin;;
	
	for (jvxSize i = 0; i < newHdl->runtime.metaData.lSeg; i++)
	{
		jvxData instFreq_use = instFreq;
		if (newHdl->common_data.tpWave == JVX_GENERATOR_WAVE_LOGSWEEP)
		{
			instFreq_use = exp(instFreq);
			instFreq_use = JVX_MIN(instFreq_use, newHdl->init_parameters_intern.common.samplerate / 2);
		}
		newHdl->runtime.metaData.freqBuffer[i] = instFreq_use;
		if (JVX_CHECK_SIZE_UNSELECTED(idxStartRampUp))
		{
			if (instFreq_use >= newHdl->runtime_parameters_async_intern.fLowStart)
			{
				idxStartRampUp = i;
			}
		}
		if (JVX_CHECK_SIZE_UNSELECTED(idxStartRampUpDone))
		{
			if (instFreq_use >= newHdl->runtime_parameters_async_intern.fLow)
			{
				idxStartRampUpDone = i;
			}
		}
		if (JVX_CHECK_SIZE_UNSELECTED(idxStartRampDown))
		{
			if (instFreq_use >= newHdl->runtime_parameters_async_intern.fUp)
			{
				idxStartRampDown = i;
			}
		}
		if (JVX_CHECK_SIZE_UNSELECTED(idxStartRampDownDone))
		{
			if (instFreq_use >= newHdl->runtime_parameters_async_intern.fUpStop)
			{
				idxStartRampDownDone = i;
			}
		}

		instFreq += newHdl->runtime.freq_increment;
	}

	if (JVX_CHECK_SIZE_UNSELECTED(idxStartRampUpDone))
	{
		idxStartRampUpDone = 0;
	}
	if (idxStartRampUp > idxStartRampUpDone)
	{
		idxStartRampUp = idxStartRampUpDone;
	}

	if (JVX_CHECK_SIZE_UNSELECTED(idxStartRampDown))
	{
		idxStartRampDown = newHdl->runtime.metaData.lSeg - 1;
	}
	if (JVX_CHECK_SIZE_UNSELECTED(idxStartRampDownDone))
	{
		idxStartRampDownDone = newHdl->runtime.metaData.lSeg - 1;;
	}

	if (idxStartRampDown > idxStartRampDownDone)
	{
		idxStartRampDownDone = idxStartRampDown;
	}

	for (jvxSize i = 0; i < newHdl->runtime.metaData.lSeg; i++)
	{
		newHdl->runtime.metaData.gainBuffer[i] = 0;
		if ((i >= idxStartRampUpDone) && (i <= idxStartRampDown))
		{
			newHdl->runtime.metaData.gainBuffer[i] = 1;
		}
	}

	jvxSize lWin = idxStartRampUpDone - idxStartRampUp;
	if (lWin)
	{
		jvx_compute_window(newHdl->runtime.metaData.gainBuffer + idxStartRampUp, lWin, 0, 0, JVX_WINDOW_HALF_HANN, NULL);
		jvxSize idxStart = idxStartRampUp;
		jvxSize idxStop = idxStartRampUp + lWin - 1;
		while(idxStart < idxStop)
		{
			jvxData tmp = newHdl->runtime.metaData.gainBuffer[idxStart];
			newHdl->runtime.metaData.gainBuffer[idxStart] = newHdl->runtime.metaData.gainBuffer[idxStop];
			newHdl->runtime.metaData.gainBuffer[idxStop] = tmp;
			idxStart++;
			idxStop--;
		}
	}
	lWin = idxStartRampDownDone - idxStartRampDown;
	if (lWin)
	{
		jvx_compute_window(newHdl->runtime.metaData.gainBuffer + idxStartRampDown, lWin, 0, 0, JVX_WINDOW_HALF_HANN, NULL);
	}

	jvx_generator_wave_restart_generator_unbuffered_linlogsweep(hdl);
	/*
	newHdl->common_data.runtime.samples_produced = 0;
	newHdl->runtime_parameters_sync_intern.unbuffered.progress = 0.0;
	newHdl->runtime.inst_frequency = newHdl->runtime.linlogFreqMin;
	newHdl->runtime.oldVal = -1;
	newHdl->runtime.gain = 0;
	newHdl->runtime.phase = 0;
	newHdl->runtime.position = 0;
	newHdl->runtime_parameters_sync_intern.itCount = 0;
	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STARTED;
	*/

	return JVX_DSP_NO_ERROR;
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

jvxDspBaseErrorType 
jvx_generatorwave_prepare(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	
	if(hdl == NULL)
	{
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	switch(hdl->tpWave)
	{
	case JVX_GENERATOR_WAVE_SINE_UNBUFFERED:
		res = jvx_generator_wave_start_generator_unbuffered_sine(hdl);
		break;

	case JVX_GENERATOR_WAVE_RECT_UNBUFFERED:
		res = jvx_generator_wave_start_generator_unbuffered_rect(hdl);
		break;

	case JVX_GENERATOR_WAVE_RAMP_UNBUFFERED:
		res = jvx_generator_wave_start_generator_unbuffered_ramp(hdl);
		break;

	case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:
		res = jvx_generator_wave_start_generator_unbuffered_noise(hdl);
		break;

#ifdef JVX_FFT_PRESENT
	case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
		res = jvx_generator_wave_start_generator_buffered_perfectsweep(hdl);
		break;
#endif

	case JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED:
		res = jvx_generator_wave_start_generator_buffered_wavplayer(hdl);
		break;

	case JVX_GENERATOR_WAVE_LOGSWEEP:
	case JVX_GENERATOR_WAVE_LINEARSWEEP:
		res = jvx_generator_wave_start_generator_unbuffered_linlogsweep(hdl);
		break;

	}
	
	return(res);
}

// ================================================================================
// Process data
// ================================================================================

//! Fill one buffer with signal
jvxDspBaseErrorType jvx_generator_wave_process_unbuffered_sine(jvx_generatorWave* hdl, jvxData* bufferFill, size_t lField, size_t* written)
{
	size_t i;
	jvxData div = 0.0;
	jvx_generator_wave_sine_parameter_runtime_private* newHdl = NULL;
	jvxData phase = 0;
	jvxData phaseInc = 0;
	if(hdl)
	{
		newHdl = (jvx_generator_wave_sine_parameter_runtime_private*) hdl->prv;

		phase = newHdl->runtime_parameters_sync_intern.periodic.phase;
		phaseInc = JVX_DOUBLE_2_DATA(M_PI_M_PI * (jvxData)newHdl->runtime_parameters_async_intern.periodic.frequency/
			(jvxData)newHdl->init_parameters_intern.periodic.common.samplerate);
		for(i = 0; i < lField; i++)
		{
			*bufferFill++ = newHdl->runtime_parameters_async_intern.periodic.common.amplitude * sin(phase);
			phase += phaseInc;
		}

		// Finally, modulo operation
		div = JVX_DOUBLE_2_DATA(floor(phase / M_PI_M_PI));
		newHdl->runtime_parameters_sync_intern.periodic.phase = JVX_DOUBLE_2_DATA(phase - div * M_PI_M_PI);
		if(written)
		{
			*written = lField;
		}
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}
			
//! Fill one buffer with signal
jvxDspBaseErrorType jvx_generator_wave_process_unbuffered_rect(jvx_generatorWave* hdl, jvxData* bufferFill, size_t lField, size_t* written)
{
	jvxData div = 0.0;
	size_t i;
	jvx_generator_wave_rect_parameter_runtime_private* newHdl = NULL;
	jvxData phase = 0;
	jvxData phaseInc = 0;
	if(hdl)
	{
		newHdl = (jvx_generator_wave_rect_parameter_runtime_private*) hdl->prv;

		phase = newHdl->runtime_parameters_sync_intern.periodic.phase;
		phaseInc = JVX_DOUBLE_2_DATA(M_PI_M_PI * (jvxData)newHdl->runtime_parameters_async_intern.periodic.frequency/
			(jvxData)newHdl->init_parameters_intern.periodic.common.samplerate);
		for(i = 0; i < lField; i++)
		{
			if(phase >= newHdl->runtime.phase_invert)
			{
				*bufferFill++ = newHdl->runtime_parameters_async_intern.periodic.common.amplitude;
			}
			else
			{
				*bufferFill++ = -newHdl->runtime_parameters_async_intern.periodic.common.amplitude;
			}

			phase += phaseInc;
	
			// Finally, modulo operation
			div = JVX_DOUBLE_2_DATA(floor(phase / M_PI_M_PI));
			phase = JVX_DOUBLE_2_DATA(phase - div * M_PI_M_PI);
		}

		newHdl->runtime_parameters_sync_intern.periodic.phase = phase;
		if(written)
		{
			*written = lField;
		}
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

//! Fill one buffer with signal
jvxDspBaseErrorType jvx_generator_wave_process_unbuffered_ramp(jvx_generatorWave* hdl, jvxData* bufferFill, size_t lField, size_t* written)
{
	size_t i;
	jvx_generator_wave_ramp_parameter_runtime_private* newHdl = NULL;
	jvxData phase = 0;
	jvxData phaseInc = 0;
	jvxData div = 0;

	if(hdl)
	{
		newHdl = (jvx_generator_wave_ramp_parameter_runtime_private*) hdl->prv;

		phase = newHdl->runtime_parameters_sync_intern.periodic.phase;
		phaseInc = JVX_DOUBLE_2_DATA(M_PI_M_PI * (jvxData)newHdl->runtime_parameters_async_intern.periodic.frequency/
			(jvxData)newHdl->init_parameters_intern.periodic.common.samplerate);
		for(i = 0; i < lField; i++)
		{
			if(phase >= newHdl->runtime.phase_invert2)
			{
				*bufferFill++ = (-1 + (phase - newHdl->runtime.phase_invert2) * newHdl->runtime.delta_phase_3_inv) * newHdl->runtime_parameters_async_intern.periodic.common.amplitude;
			}
			else if(phase >= newHdl->runtime.phase_invert1)
			{
				*bufferFill++ = (1 - (phase - newHdl->runtime.phase_invert1) * newHdl->runtime.delta_phase_2_inv * 2) * newHdl->runtime_parameters_async_intern.periodic.common.amplitude;
			}
			else
			{
				*bufferFill++ = (phase * newHdl->runtime.delta_phase_1_inv) * newHdl->runtime_parameters_async_intern.periodic.common.amplitude;
			}

			phase += phaseInc;
			
			// Finally, modulo operation
			div = JVX_DOUBLE_2_DATA(floor(phase / M_PI_M_PI));
			phase = JVX_DOUBLE_2_DATA(phase - div * M_PI_M_PI);
		}

		newHdl->runtime_parameters_sync_intern.periodic.phase = phase;
		if(written)
		{
			*written = lField;
		}
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

//! Fill one buffer with signal
jvxDspBaseErrorType jvx_generator_wave_process_unbuffered_noise(jvx_generatorWave* hdl, jvxData* bufferFill, size_t lField, size_t* written)
{
	size_t i;
	jvx_generator_wave_noise_parameter_runtime_private* newHdl = NULL;
	if(hdl)
	{
		newHdl = (jvx_generator_wave_noise_parameter_runtime_private*) hdl->prv;

		switch(newHdl->runtime_parameters_async_intern.noiseType)
		{
		case JVX_GENERATOR_NOISE_UNIFORM:
			for(i = 0; i < lField; i++)
			{
				*bufferFill++ = (jvx_rand_uniform(&newHdl->runtime.seed)-0.5)* 2.0  * newHdl->runtime_parameters_async_intern.common.amplitude;
			} 
			break;
		case JVX_GENERATOR_NOISE_NORMAL:
			for(i = 0; i < lField; i++)
			{
				*bufferFill++ = jvx_rand_normal(&newHdl->runtime.seed) * newHdl->runtime_parameters_async_intern.common.amplitude;
			} 
			break;
		}

		if(written)
		{
			*written = lField;
		}
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

#ifdef JVX_FFT_PRESENT

//! Fill one buffer with signal
jvxDspBaseErrorType jvx_generator_wave_process_buffered_perfectsweep(jvx_generatorWave* hdl, jvxData* bufferFill, size_t lField, size_t* written)
{
	size_t i;
	if(hdl)
	{
		jvx_generator_wave_perfectsweep_parameter_runtime_private* newHdl = (jvx_generator_wave_perfectsweep_parameter_runtime_private*) hdl->prv;

		jvxData* cpyTo = bufferFill;
		jvxData* cpyFrom = NULL;

		size_t numSamples = lField;
		size_t numSamplesWritten = 0;
		while(1)
		{
			cpyFrom = newHdl->runtime.fieldSamples + newHdl->runtime.position;
			if(newHdl->runtime_parameters_sync_intern.buffered.length > newHdl->runtime.position)
			{
				size_t ll = JVX_MIN(numSamples, newHdl->runtime_parameters_sync_intern.buffered.length - newHdl->runtime.position);

				for(i = 0; i < ll; i++)
				{
					*cpyTo ++ = newHdl->runtime_parameters_async_intern.common.amplitude * *cpyFrom++;
				}

				numSamples -= ll;
				newHdl->runtime.position += ll;
				numSamplesWritten += ll;
			}
			else
			{
				newHdl->runtime_parameters_sync_intern.itCount++;
				if(newHdl->runtime_parameters_sync_intern.itCount < newHdl->runtime_parameters_async_intern.loopCount)
				{
					newHdl->runtime.position = 0;
				}
				else
				{
					for(i = 0; i < numSamples; i++)
					{
						*cpyTo ++ = 0.0;
					}
					newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_COMPLETE;
					break;
				}
			}
			if(numSamples == 0)
			{
				break;
			}
		}
		
		if(written)
		{
			*written = numSamplesWritten;
		}
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

#endif

//! Fill one buffer with signal
jvxDspBaseErrorType jvx_generator_wave_process_buffered_wavplayer(jvx_generatorWave* hdl, jvxData* bufferFill, size_t lField, size_t* written)
{
	size_t i;
	if(hdl)
	{
		jvx_generator_wave_wavplayer_parameter_runtime_private* newHdl = (jvx_generator_wave_wavplayer_parameter_runtime_private*) hdl->prv;

		jvxData* cpyTo = bufferFill;
		jvxData* cpyFrom = NULL;

		size_t numSamples = lField;
		size_t numSamplesWritten = 0;
		while(1)
		{
			cpyFrom = newHdl->runtime_parameters_sync_intern.buffered.bufField + newHdl->runtime.position;
			if(newHdl->runtime_parameters_sync_intern.buffered.length > newHdl->runtime.position)
			{
				size_t ll = JVX_MIN(numSamples, newHdl->runtime_parameters_sync_intern.buffered.length - newHdl->runtime.position);

				for(i = 0; i < ll; i++)
				{
					*cpyTo ++ = newHdl->runtime_parameters_async_intern.common.amplitude * *cpyFrom++;
				}

				numSamples -= ll;
				newHdl->runtime.position += ll;
				numSamplesWritten += ll;
			}
			else
			{
				newHdl->runtime_parameters_sync_intern.itCount++;
				if(newHdl->runtime_parameters_sync_intern.itCount < newHdl->runtime_parameters_async_intern.loopCount)
				{
					newHdl->runtime.position = 0;
				}
				else
				{
					for(i = 0; i < numSamples; i++)
					{
						*cpyTo ++ = 0.0;
					}
					newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_COMPLETE;
					break;
				}
			}
			if(numSamples == 0)
			{
				break;
			}
		}
		
		if(written)
		{
			*written = numSamplesWritten;
		}
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

//! Fill one buffer with signal
jvxDspBaseErrorType jvx_generator_wave_process_unbuffered_linlogsweep(jvx_generatorWave* hdl, jvxData* bufferFill, size_t lField, size_t* written)
{
	jvxSize i;
	jvxSize ll_start, ll_proc, ll_stop;
	jvxData phase, instFreq, instFreq_use;
	jvxSize written2 = 0;
	jvxSize loc_posi = 0;
	jvxSize numSamplesAll;
	jvx_generator_wave_linlogsweep_parameter_runtime_private* newHdl = NULL;

	if(hdl)
	{
		newHdl = (jvx_generator_wave_linlogsweep_parameter_runtime_private*) hdl->prv;

		jvxData* fPtr = newHdl->runtime_parameters_sync_intern.unbuffered.freq_ptr;
		jvxData* gPtr = newHdl->runtime_parameters_sync_intern.unbuffered.gain_ptr;

		instFreq = newHdl->runtime.inst_frequency;
		jvxSize loc_posi = newHdl->runtime.position;
		jvxCBool storeFreqGain = 
			(
				(fPtr || gPtr )
				&& (newHdl->runtime_parameters_sync_intern.itCount == 0));

		jvxSize startPosiSegment = 0;
		startPosiSegment = JVX_MAX(newHdl->runtime.position, startPosiSegment);
		// ll_start = JVX_MAX(0, (int)newHdl->runtime.start_position_sweep - (int)newHdl->runtime.position);
		ll_start = JVX_MAX(0, (int)newHdl->runtime.start_position_sweep - (int)startPosiSegment);
		ll_start = JVX_MIN(ll_start, (int)lField);
		lField -= ll_start;

		startPosiSegment = newHdl->runtime.start_position_sweep;
		startPosiSegment = JVX_MAX(newHdl->runtime.position, startPosiSegment);
		// ll_proc = JVX_MAX(0, (int)newHdl->runtime.stop_position_sweep - (int)newHdl->runtime.position);
		ll_proc = JVX_MAX(0, (int)newHdl->runtime.stop_position_sweep - (int)startPosiSegment);
		ll_proc = JVX_MIN(ll_proc, (int)lField);
		lField -= ll_proc;

		startPosiSegment = newHdl->runtime.stop_position_sweep;
		startPosiSegment = JVX_MAX(newHdl->runtime.position, startPosiSegment);
		// ll_stop = JVX_MAX(0, (int)newHdl->runtime_parameters_sync_intern.unbuffered.length - (int)newHdl->runtime.position);
		ll_stop = JVX_MAX(0, (int)newHdl->runtime_parameters_sync_intern.unbuffered.length - (int)startPosiSegment);
		ll_stop = JVX_MIN(ll_stop, (int)lField);
		lField -= ll_stop;

		// Zero padding at the beginning
		for(i = 0; i < ll_start; i++)
		{
			*bufferFill++ = 0.0;
		}

		if (storeFreqGain)
		{
			if (newHdl->common_data.tpWave == JVX_GENERATOR_WAVE_LOGSWEEP)
			{
				instFreq_use = exp(instFreq);
				instFreq_use = JVX_MIN(instFreq_use, newHdl->init_parameters_intern.common.samplerate / 2);
			}
			for (i = 0; i < ll_start; i++)
			{
				if (fPtr)
				{
					fPtr[loc_posi] = 0; // instFreq_use;
				}
				if (gPtr)
				{
					gPtr[loc_posi] = 0.0;
				}
				loc_posi++;
			}
		}

		// Produce the sweep
		phase = newHdl->runtime.phase;

		for(i = 0; i < ll_proc; i++)
		{
			jvxData val = JVX_DOUBLE_2_DATA(newHdl->runtime_parameters_async_intern.common.amplitude * sin(phase));
			if (newHdl->common_data.tpWave == JVX_GENERATOR_WAVE_LOGSWEEP)
			{
				instFreq_use = exp(instFreq);
				instFreq_use = JVX_MIN(instFreq_use, newHdl->init_parameters_intern.common.samplerate / 2);
			}

			assert(newHdl->runtime.metaData.idxOneSegment < newHdl->runtime.metaData.lSeg);
			jvxData instFreq_useBuf = newHdl->runtime.metaData.freqBuffer[newHdl->runtime.metaData.idxOneSegment];

			assert(instFreq_use == instFreq_useBuf);

			jvxData signSign = val * newHdl->runtime.oldVal;
			signSign = copysign(1.0, signSign);

			if (signSign < 0)
			{
				if (instFreq_use > newHdl->runtime_parameters_async_intern.fLow)
				{
					// Sign change, we can reset gain
					newHdl->runtime.gain = 1;
				}
				if (instFreq_use > newHdl->runtime_parameters_async_intern.fUp)
				{
					// Sign change, we can reset gain
					newHdl->runtime.gain = 0;
				}
			}
			newHdl->runtime.oldVal = val;

			//*bufferFill++ = val * newHdl->runtime.gain;
			*bufferFill++ = val * newHdl->runtime.metaData.gainBuffer[newHdl->runtime.metaData.idxOneSegment];
			
			newHdl->runtime.metaData.idxOneSegment++;

			phase += 2 * M_PI * instFreq_use * newHdl->runtime.div_samplerate;
			instFreq += newHdl->runtime.freq_increment;
			
			if (storeFreqGain)
			{
				if (fPtr)
				{
					fPtr[loc_posi] = instFreq_use;
				}
				if (gPtr)
				{
					gPtr[loc_posi] = newHdl->runtime.gain;
				}
				loc_posi++;
			}
		}
		newHdl->runtime.phase = (phase - floor(phase/(2*M_PI))*2*M_PI);
		newHdl->runtime.inst_frequency = instFreq;

		// Zero padding at the end
		for(i = 0; i < ll_stop; i++)
		{
			*bufferFill++ = 0.0;
		}

		if (storeFreqGain)
		{
			for (i = 0; i < ll_stop; i++)
			{
				if (fPtr)
				{
					fPtr[loc_posi] = instFreq_use;
				}
				if (gPtr)
				{
					gPtr[loc_posi] = 0.0;
				}
				loc_posi++;
			}
		}

		numSamplesAll = (ll_start + ll_proc + ll_stop);
		newHdl->runtime.position += (ll_start + ll_proc + ll_stop);
		if (written)
		{
			*written = (ll_start + ll_proc + ll_stop);
		}

		if(newHdl->runtime.position == newHdl->runtime_parameters_sync_intern.unbuffered.length)
		{
			// Try wrap-around of sweep if end is reached
			newHdl->runtime_parameters_sync_intern.itCount++;
			if(newHdl->runtime_parameters_sync_intern.itCount < newHdl->runtime_parameters_async_intern.loopCount)
			{
				// Here comes the wrap-around
				newHdl->runtime.phase = 0;
				newHdl->runtime.position = 0;
				newHdl->runtime.oldVal = -1;
				newHdl->runtime.gain = 0;
				newHdl->runtime.metaData.idxOneSegment = 0;
				newHdl->runtime.inst_frequency = newHdl->runtime.linlogFreqMin;

				jvx_generator_wave_process_unbuffered_linlogsweep(hdl, bufferFill, lField, &written2); // buffer and lField have been adapted when computing the segment counts
				if(written)
				{
					*written += written2;
				}
				return(JVX_DSP_NO_ERROR);
			}
			else
			{
				newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_COMPLETE;
			}
		}
		
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

jvxDspBaseErrorType 
jvx_generatorwave_process(jvx_generatorWave* hdl, jvxData* bufferFill, size_t lField, size_t* written)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	
	if(hdl == NULL)
	{
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	switch(hdl->tpWave)
	{
	case JVX_GENERATOR_WAVE_SINE_UNBUFFERED:
		res = jvx_generator_wave_process_unbuffered_sine( hdl, bufferFill,  lField, written);
		break;

	case JVX_GENERATOR_WAVE_RECT_UNBUFFERED:
		res = jvx_generator_wave_process_unbuffered_rect(hdl, bufferFill,  lField, written);
		break;

	case JVX_GENERATOR_WAVE_RAMP_UNBUFFERED:
		res = jvx_generator_wave_process_unbuffered_ramp(hdl, bufferFill,  lField, written);
		break;

	case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:
		res = jvx_generator_wave_process_unbuffered_noise(hdl, bufferFill,  lField, written);
		break;

#ifdef JVX_FFT_PRESENT
	case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
		res = jvx_generator_wave_process_buffered_perfectsweep(hdl, bufferFill,  lField, written);
		break;
#endif

	case JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED:
		res = jvx_generator_wave_process_buffered_wavplayer(hdl, bufferFill,  lField, written);
		break;

	case JVX_GENERATOR_WAVE_LOGSWEEP:
	case JVX_GENERATOR_WAVE_LINEARSWEEP:
		res = jvx_generator_wave_process_unbuffered_linlogsweep(hdl, bufferFill,  lField, written);
		break;

	}
	
	return(res);
}


// ================================================================================
// Postprocess module
// ================================================================================

jvxDspBaseErrorType
jvx_generator_wave_stop_generator_unbuffered_sine(jvx_generatorWave* hdl)
{
	jvx_generator_wave_sine_parameter_runtime_private* newHdl = (jvx_generator_wave_sine_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.periodic.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;
	return(JVX_DSP_NO_ERROR);
}
		
jvxDspBaseErrorType
jvx_generator_wave_stop_generator_unbuffered_rect(jvx_generatorWave* hdl)
{
	jvx_generator_wave_rect_parameter_runtime_private* newHdl = (jvx_generator_wave_rect_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.periodic.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;
	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType
jvx_generator_wave_stop_generator_unbuffered_ramp(jvx_generatorWave* hdl)
{
	jvx_generator_wave_ramp_parameter_runtime_private* newHdl = (jvx_generator_wave_ramp_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.periodic.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;
	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType
jvx_generator_wave_stop_generator_unbuffered_noise(jvx_generatorWave* hdl)
{
	jvx_generator_wave_noise_parameter_runtime_private* newHdl = (jvx_generator_wave_noise_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;
	return(JVX_DSP_NO_ERROR);
}

#ifdef JVX_FFT_PRESENT
jvxDspBaseErrorType
jvx_generator_wave_stop_generator_buffered_perfectsweep(jvx_generatorWave* hdl)
{
	jvx_generator_wave_perfectsweep_parameter_runtime_private* newHdl = (jvx_generator_wave_perfectsweep_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;

	JVX_DSP_SAFE_DELETE_FIELD(newHdl->runtime.fieldSamples);
	newHdl->runtime.fieldSamples = NULL;

	newHdl->runtime.position = 0;

	newHdl->runtime_parameters_sync_intern.buffered.progress = 0.0;
	newHdl->runtime_parameters_sync_intern.buffered.length = 0;

	return(JVX_DSP_NO_ERROR);
}
#endif

jvxDspBaseErrorType
jvx_generator_wave_stop_generator_buffered_wavplayer(jvx_generatorWave* hdl)
{
	jvx_generator_wave_wavplayer_parameter_runtime_private* newHdl = (jvx_generator_wave_wavplayer_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;

	newHdl->runtime.position = 0;

	newHdl->runtime_parameters_sync_intern.buffered.progress = 0.0;
	newHdl->runtime_parameters_sync_intern.buffered.length = 0;
	newHdl->runtime_parameters_sync_intern.buffered.bufField = 0;
	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType
jvx_generator_wave_stop_generator_unbuffered_linlogsweep(jvx_generatorWave* hdl)
{
	jvx_generator_wave_linlogsweep_parameter_runtime_private* newHdl = (jvx_generator_wave_linlogsweep_parameter_runtime_private*) hdl->prv;
	newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STOPPED;

	JVX_SAFE_DELETE_FIELD(newHdl->runtime.metaData.gainBuffer);
	JVX_SAFE_DELETE_FIELD(newHdl->runtime.metaData.freqBuffer);

	newHdl->runtime.freq_increment = 0.0;
	newHdl->runtime.linlogFreqMax = 0.0;
	newHdl->runtime.linlogFreqMin = 0.0;
	newHdl->runtime.phase = 0.0;
	newHdl->runtime.position = 0;
	newHdl->runtime.start_position_sweep = 0;
	newHdl->runtime.stop_position_sweep = 0;

	newHdl->runtime_parameters_sync_intern.unbuffered.progress = 0.0;
	newHdl->runtime_parameters_sync_intern.unbuffered.length = 0;

	return(JVX_DSP_NO_ERROR);
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

jvxDspBaseErrorType 
jvx_generatorwave_postprocess(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	
	if(hdl == NULL)
	{
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	switch(hdl->tpWave)
	{
	case JVX_GENERATOR_WAVE_SINE_UNBUFFERED:
		res = jvx_generator_wave_stop_generator_unbuffered_sine(hdl);
		break;

	case JVX_GENERATOR_WAVE_RECT_UNBUFFERED:
		res = jvx_generator_wave_stop_generator_unbuffered_rect(hdl);
		break;

	case JVX_GENERATOR_WAVE_RAMP_UNBUFFERED:
		res = jvx_generator_wave_stop_generator_unbuffered_ramp(hdl);
		break;

	case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:
		res = jvx_generator_wave_stop_generator_unbuffered_noise(hdl);
		break;

#ifdef JVX_FFT_PRESENT
	case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
		res = jvx_generator_wave_stop_generator_buffered_perfectsweep(hdl);
		break;
#endif

	case JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED:
		res = jvx_generator_wave_stop_generator_buffered_wavplayer(hdl);
		break;

	case JVX_GENERATOR_WAVE_LOGSWEEP:
	case JVX_GENERATOR_WAVE_LINEARSWEEP:
		res = jvx_generator_wave_stop_generator_unbuffered_linlogsweep(hdl);
		break;

	}
	
	return(res);
}

// ================================================================================
// ================================================================================


jvxDspBaseErrorType 
jvx_generatorwave_restart(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;

	if (hdl == NULL)
	{
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}
	switch (hdl->tpWave)
	{
	case JVX_GENERATOR_WAVE_SINE_UNBUFFERED:
		res = jvx_generator_wave_restart_generator_unbuffered_sine(hdl);
		break;

	case JVX_GENERATOR_WAVE_RECT_UNBUFFERED:
		res = jvx_generator_wave_restart_generator_unbuffered_rect(hdl);
		break;

	case JVX_GENERATOR_WAVE_RAMP_UNBUFFERED:
		res = jvx_generator_wave_restart_generator_unbuffered_ramp(hdl);
		break;

	case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:
		res = jvx_generator_wave_restart_generator_unbuffered_noise(hdl);
		break;

#ifdef JVX_FFT_PRESENT
	case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
		res = jvx_generator_wave_restart_generator_buffered_perfectsweep(hdl);
		break;
#endif

	case JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED:
		res = jvx_generator_wave_restart_generator_buffered_wavplayer(hdl);
		break;

	case JVX_GENERATOR_WAVE_LOGSWEEP:
	case JVX_GENERATOR_WAVE_LINEARSWEEP:
		res = jvx_generator_wave_restart_generator_unbuffered_linlogsweep(hdl);
		break;

	}

	return(res);
}

// ================================================================================
// Deactivate module
// ================================================================================

jvxDspBaseErrorType 
jvx_generator_wave_terminate_generator_unbuffered_sine(jvx_generatorWave* hdl)
{
	jvx_generator_wave_sine_parameter_runtime_private* remHdl = (jvx_generator_wave_sine_parameter_runtime_private*) hdl->prv;
	
	JVX_DSP_SAFE_DELETE_OBJECT(remHdl);
	hdl->prv = NULL;

	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType 
jvx_generator_wave_terminate_generator_unbuffered_rect(jvx_generatorWave* hdl)
{
	jvx_generator_wave_rect_parameter_runtime_private* remHdl = (jvx_generator_wave_rect_parameter_runtime_private*) hdl->prv;
	
	JVX_DSP_SAFE_DELETE_OBJECT(remHdl);
	hdl->prv = NULL;

	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType 
jvx_generator_wave_terminate_generator_unbuffered_ramp(jvx_generatorWave* hdl)
{
	jvx_generator_wave_ramp_parameter_runtime_private* remHdl = (jvx_generator_wave_ramp_parameter_runtime_private*) hdl->prv;
	
	JVX_DSP_SAFE_DELETE_OBJECT(remHdl);
	hdl->prv = NULL;

	return(JVX_DSP_NO_ERROR);
}


jvxDspBaseErrorType 
jvx_generator_wave_terminate_generator_unbuffered_noise(jvx_generatorWave* hdl)
{
	jvx_generator_wave_noise_parameter_runtime_private* remHdl = (jvx_generator_wave_noise_parameter_runtime_private*) hdl->prv;
	
	JVX_DSP_SAFE_DELETE_OBJECT(remHdl);
	hdl->prv = NULL;

	return(JVX_DSP_NO_ERROR);
}

#ifdef JVX_FFT_PRESENT

jvxDspBaseErrorType 
jvx_generator_wave_terminate_generator_buffered_perfectsweep(jvx_generatorWave* hdl)
{
	jvx_generator_wave_perfectsweep_parameter_runtime_private* remHdl = (jvx_generator_wave_perfectsweep_parameter_runtime_private*) hdl->prv;
	
	JVX_DSP_SAFE_DELETE_OBJECT(remHdl);
	hdl->prv = NULL;

	return(JVX_DSP_NO_ERROR);
}
#endif

jvxDspBaseErrorType 
jvx_generator_wave_terminate_generator_buffered_wavplayer(jvx_generatorWave* hdl)
{
	jvx_generator_wave_wavplayer_parameter_runtime_private* remHdl = (jvx_generator_wave_wavplayer_parameter_runtime_private*) hdl->prv;
	
	JVX_DSP_SAFE_DELETE_OBJECT(remHdl);
	hdl->prv = NULL;

	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType 
jvx_generator_wave_terminate_generator_unbuffered_linlogsweep(jvx_generatorWave* hdl)
{
	jvx_generator_wave_linlogsweep_parameter_runtime_private* remHdl = (jvx_generator_wave_linlogsweep_parameter_runtime_private*) hdl->prv;
	
	JVX_DSP_SAFE_DELETE_OBJECT(remHdl);
	hdl->prv = NULL;

	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType jvx_generatorwave_deactivate(jvx_generatorWave* hdl)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	
	if(hdl == NULL)
	{
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	switch(hdl->tpWave)
	{
	case JVX_GENERATOR_WAVE_SINE_UNBUFFERED:
		res = jvx_generator_wave_terminate_generator_unbuffered_sine(hdl);
		break;

	case JVX_GENERATOR_WAVE_RECT_UNBUFFERED:
		res = jvx_generator_wave_terminate_generator_unbuffered_rect(hdl);
		break;

	case JVX_GENERATOR_WAVE_RAMP_UNBUFFERED:
		res = jvx_generator_wave_terminate_generator_unbuffered_ramp(hdl);
		break;

	case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:
		res = jvx_generator_wave_terminate_generator_unbuffered_noise(hdl);
		break;

#ifdef JVX_FFT_PRESENT
	case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
		res = jvx_generator_wave_terminate_generator_buffered_perfectsweep(hdl);
		break;
#endif

	case JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED:
		res = jvx_generator_wave_terminate_generator_buffered_wavplayer(hdl);
		break;

	case JVX_GENERATOR_WAVE_LOGSWEEP:
	case JVX_GENERATOR_WAVE_LINEARSWEEP:
		res = jvx_generator_wave_terminate_generator_unbuffered_linlogsweep(hdl);
		break;

	}
	
	return(res);
}

// ================================================================================
// Update module parameters
// ================================================================================

jvxDspBaseErrorType jvx_generator_wave_update_unbuffered_sine(jvx_generatorWave* hdl, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	jvx_generator_wave_sine_parameter_runtime_private* newHdl = NULL;
	newHdl = (jvx_generator_wave_sine_parameter_runtime_private*) hdl->prv;

	if(newHdl == NULL)
	{
		return(JVX_DSP_ERROR_WRONG_STATE);
	}

	if (whatToUpdate & JVX_DSP_UPDATE_USER_OFFSET)
	{
		if (do_set)
		{
			return JVX_DSP_ERROR_UNSUPPORTED;
		}
		else
		{
			hdl->prm_async.sineWave.periodic.common.theStatus = newHdl->runtime_parameters_async_intern.periodic.common.theStatus;
		}
	}
	if (whatToUpdate & JVX_DSP_UPDATE_ASYNC)
	{
		if (do_set)
		{
			newHdl->runtime_parameters_async_intern = hdl->prm_async.sineWave;
		}
		else
		{
			hdl->prm_async.sineWave = newHdl->runtime_parameters_async_intern;
		}
	}
	if (whatToUpdate &  JVX_DSP_UPDATE_SYNC)
	{
		if(do_set)
		{
			newHdl->runtime_parameters_sync_intern = hdl->prm_sync.sineWave;
		}	
		else
		{
			hdl->prm_sync.sineWave = newHdl->runtime_parameters_sync_intern;
		}	
	}

	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType jvx_generator_wave_update_unbuffered_rect(jvx_generatorWave* hdl, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	jvx_generator_wave_rect_parameter_runtime_private* newHdl = NULL;
	newHdl = (jvx_generator_wave_rect_parameter_runtime_private*) hdl->prv;

	if(newHdl == NULL)
	{
		return(JVX_DSP_ERROR_WRONG_STATE);
	}

	if (whatToUpdate & JVX_DSP_UPDATE_USER_OFFSET)
	{
		if (do_set)
		{
			return JVX_DSP_ERROR_UNSUPPORTED;
		}
		else
		{
			hdl->prm_async.rectWave.periodic.common.theStatus = newHdl->runtime_parameters_async_intern.periodic.common.theStatus;
		}
	}
	if (whatToUpdate & JVX_DSP_UPDATE_ASYNC)
	{
		if (do_set)
		{
			newHdl->runtime_parameters_async_intern = hdl->prm_async.rectWave;
			if (newHdl->runtime_parameters_async_intern.pulsewidth <= 0)
			{
				newHdl->runtime_parameters_async_intern.pulsewidth = mat_epsMin;
			}
			if (newHdl->runtime_parameters_async_intern.pulsewidth >= 1.0)
			{
				newHdl->runtime_parameters_async_intern.pulsewidth = JVX_DOUBLE_2_DATA(1.0 - mat_epsMin);
			}
			newHdl->runtime.phase_invert = JVX_DOUBLE_2_DATA(M_PI_M_PI * newHdl->runtime_parameters_async_intern.pulsewidth);
		}
		else
		{
			hdl->prm_async.rectWave = newHdl->runtime_parameters_async_intern;
		}
	}
	if (whatToUpdate & JVX_DSP_UPDATE_SYNC)
	{
		if(do_set)
		{
			newHdl->runtime_parameters_sync_intern = hdl->prm_sync.rectWave;
		}	
		else
		{
			hdl->prm_sync.rectWave = newHdl->runtime_parameters_sync_intern;
		}	
	}

	return(JVX_DSP_NO_ERROR);
}




jvxDspBaseErrorType jvx_generator_wave_update_unbuffered_ramp(jvx_generatorWave* hdl, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	jvx_generator_wave_ramp_parameter_runtime_private* newHdl = NULL;
	newHdl = (jvx_generator_wave_ramp_parameter_runtime_private*) hdl->prv;

	if(newHdl == NULL)
	{
		return(JVX_DSP_ERROR_WRONG_STATE);
	}

	if (whatToUpdate & JVX_DSP_UPDATE_USER_OFFSET)
	{
		if (do_set)
		{
			return JVX_DSP_ERROR_UNSUPPORTED;
		}
		else
		{
			hdl->prm_async.rectWave.periodic.common.theStatus = newHdl->runtime_parameters_async_intern.periodic.common.theStatus;
		}
	}
	if (whatToUpdate &  JVX_DSP_UPDATE_ASYNC)
	{
		if (do_set)
		{
			newHdl->runtime_parameters_async_intern = hdl->prm_async.rampWave;
			newHdl->runtime.phase_invert1 = JVX_DOUBLE_2_DATA(newHdl->runtime_parameters_async_intern.pulsewidth1 * M_PI_M_PI);
			newHdl->runtime.phase_invert2 = JVX_DOUBLE_2_DATA(newHdl->runtime_parameters_async_intern.pulsewidth2 * M_PI_M_PI);
			newHdl->runtime.delta_phase_1_inv = JVX_DOUBLE_2_DATA(1.0 / newHdl->runtime.phase_invert1);
			newHdl->runtime.delta_phase_2_inv = JVX_DOUBLE_2_DATA(1.0 / (newHdl->runtime.phase_invert2 - newHdl->runtime.phase_invert1));
			newHdl->runtime.delta_phase_3_inv = JVX_DOUBLE_2_DATA(1.0 / (M_PI_M_PI - newHdl->runtime.phase_invert1));
		}
		else
		{
			hdl->prm_async.rampWave = newHdl->runtime_parameters_async_intern;
		}
	}
	if (whatToUpdate & JVX_DSP_UPDATE_SYNC)
	{
		if(do_set)
		{
			newHdl->runtime_parameters_sync_intern = hdl->prm_sync.rampWave;
		}	
		else
		{
			hdl->prm_sync.rampWave = newHdl->runtime_parameters_sync_intern;
		}	
	}
	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType jvx_generator_wave_update_unbuffered_noise(jvx_generatorWave* hdl, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	jvx_generator_wave_noise_parameter_runtime_private* newHdl = NULL;
	newHdl = (jvx_generator_wave_noise_parameter_runtime_private*) hdl->prv;

	if(newHdl == NULL)
	{
		return(JVX_DSP_ERROR_WRONG_STATE);
	}

	if (whatToUpdate & JVX_DSP_UPDATE_USER_OFFSET)
	{
		if (do_set)
		{
			return JVX_DSP_ERROR_UNSUPPORTED;
		}
		else
		{
			hdl->prm_async.noiseWave.common.theStatus = newHdl->runtime_parameters_async_intern.common.theStatus;
		}
	}
	if (whatToUpdate & JVX_DSP_UPDATE_ASYNC)
	{
		if (do_set)
		{
			newHdl->runtime_parameters_async_intern = hdl->prm_async.noiseWave;
		}
		else
		{
			hdl->prm_async.noiseWave = newHdl->runtime_parameters_async_intern;
		}
	}
	return(JVX_DSP_NO_ERROR);
}


#ifdef JVX_FFT_PRESENT

jvxDspBaseErrorType jvx_generator_wave_update_buffered_perfectsweep(jvx_generatorWave* hdl, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	jvx_generator_wave_perfectsweep_parameter_runtime_private* newHdl = NULL;
	newHdl = (jvx_generator_wave_perfectsweep_parameter_runtime_private*)hdl->prv;

	if (newHdl == NULL)
	{
		return(JVX_DSP_ERROR_WRONG_STATE);
	}

	if (whatToUpdate & JVX_DSP_UPDATE_USER_OFFSET)
	{
		if (do_set)
		{
			return JVX_DSP_ERROR_UNSUPPORTED;
		}
		else
		{
			newHdl->runtime_parameters_sync_intern.buffered.progress = (jvxData)newHdl->runtime.position / (jvxData)newHdl->runtime_parameters_sync_intern.buffered.length;
			hdl->prm_async.perfSweepWave.common.theStatus = newHdl->runtime_parameters_async_intern.common.theStatus;
			hdl->prm_sync.perfSweepWave.buffered.progress = newHdl->runtime_parameters_sync_intern.buffered.progress;
			hdl->prm_sync.perfSweepWave.itCount = newHdl->runtime_parameters_sync_intern.itCount;
		}
	}
	if (whatToUpdate & JVX_DSP_UPDATE_ASYNC)
	{
		if (do_set)
		{
			newHdl->runtime_parameters_async_intern = hdl->prm_async.perfSweepWave;
		}
		else
		{
			hdl->prm_async.perfSweepWave = newHdl->runtime_parameters_async_intern;
		}
	}
	if (whatToUpdate & JVX_DSP_UPDATE_SYNC)
	{
		if(do_set)
		{
			return(JVX_DSP_ERROR_UNSUPPORTED);
		}	
		else
		{
			hdl->prm_sync.perfSweepWave.buffered.progress = (jvxData)newHdl->runtime.position/(jvxData)newHdl->runtime_parameters_sync_intern.buffered.length;
			hdl->prm_sync.perfSweepWave.buffered.length = newHdl->runtime_parameters_sync_intern.buffered.length;
			hdl->prm_sync.perfSweepWave.buffered.bufField = newHdl->runtime_parameters_sync_intern.buffered.bufField;
		}			
	}
	return(JVX_DSP_NO_ERROR);
}

#endif

jvxDspBaseErrorType jvx_generator_wave_update_buffered_wavplayer(jvx_generatorWave* hdl, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	jvx_generator_wave_wavplayer_parameter_runtime_private* newHdl = NULL;
	newHdl = (jvx_generator_wave_wavplayer_parameter_runtime_private*) hdl->prv;

	if(newHdl == NULL)
	{
		return(JVX_DSP_ERROR_WRONG_STATE);
	}

	if (whatToUpdate & JVX_DSP_UPDATE_USER_OFFSET)
	{
		if (do_set)
		{
			return JVX_DSP_ERROR_UNSUPPORTED;
		}
		else
		{
			newHdl->runtime_parameters_sync_intern.buffered.progress = (jvxData)newHdl->runtime.position / (jvxData)newHdl->runtime_parameters_sync_intern.buffered.length;
			hdl->prm_async.wavWave.common.theStatus = newHdl->runtime_parameters_async_intern.common.theStatus;
			hdl->prm_sync.wavWave.buffered.progress = newHdl->runtime_parameters_sync_intern.buffered.progress;
			hdl->prm_sync.wavWave.itCount = newHdl->runtime_parameters_sync_intern.itCount;
		}
	}
	if (whatToUpdate & JVX_DSP_UPDATE_ASYNC)
	{
		if (do_set)
		{
			newHdl->runtime_parameters_async_intern = hdl->prm_async.wavWave;
		}
		else
		{
			hdl->prm_async.wavWave = newHdl->runtime_parameters_async_intern;
		}
	}
	if (whatToUpdate & JVX_DSP_UPDATE_SYNC)
	{
		if(do_set)
		{
			newHdl->runtime_parameters_sync_intern.buffered.length = hdl->prm_sync.wavWave.buffered.length;
			newHdl->runtime_parameters_sync_intern.buffered.bufField = hdl->prm_sync.wavWave.buffered.bufField;

			newHdl->runtime.position = 0;

			if(newHdl->runtime_parameters_sync_intern.buffered.length > 0)
			{
				newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_STARTED;
			}
			else
			{
				newHdl->runtime_parameters_async_intern.common.theStatus = JVX_GENERATOR_WAVE_COMPLETE;
			}
		}
		else
		{
			hdl->prm_sync.wavWave.buffered.progress = (jvxData)newHdl->runtime.position/
				(jvxData)newHdl->runtime_parameters_sync_intern.buffered.length;
			hdl->prm_sync.wavWave.buffered.bufField = newHdl->runtime_parameters_sync_intern.buffered.bufField;
			hdl->prm_sync.wavWave.buffered.length = newHdl->runtime_parameters_sync_intern.buffered.length;
		}	
	}
	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType jvx_generator_wave_update_unbuffered_linlogsweep(jvx_generatorWave* hdl, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	jvx_generator_wave_linlogsweep_parameter_runtime_private* newHdl = NULL;
	newHdl = (jvx_generator_wave_linlogsweep_parameter_runtime_private*) hdl->prv;

	if(newHdl == NULL)
	{
		return(JVX_DSP_ERROR_WRONG_STATE);
	}

	if (whatToUpdate & JVX_DSP_UPDATE_USER_OFFSET)
	{
		if (do_set)
		{
			return JVX_DSP_ERROR_UNSUPPORTED;
		}
		else
		{
			newHdl->runtime_parameters_sync_intern.unbuffered.progress = (jvxData)newHdl->runtime.position / (jvxData)newHdl->runtime_parameters_sync_intern.unbuffered.length;
			hdl->prm_async.linlogSweepWave.common.theStatus = newHdl->runtime_parameters_async_intern.common.theStatus;
			hdl->prm_sync.linlogSweepWave.unbuffered.progress = newHdl->runtime_parameters_sync_intern.unbuffered.progress;
			hdl->prm_sync.linlogSweepWave.itCount = newHdl->runtime_parameters_sync_intern.itCount;
		}
	}
	if (whatToUpdate & JVX_DSP_UPDATE_ASYNC)
	{
		if (do_set)
		{
			newHdl->runtime_parameters_async_intern = hdl->prm_async.linlogSweepWave;
		}
		else
		{
			hdl->prm_async.linlogSweepWave = newHdl->runtime_parameters_async_intern;
		}
	}
	if (whatToUpdate & JVX_DSP_UPDATE_SYNC)
	{
		if(do_set)
		{
			// Pull in eternal pointer
			newHdl->runtime_parameters_sync_intern.unbuffered.freq_ptr = hdl->prm_sync.linlogSweepWave.unbuffered.freq_ptr;				
			newHdl->runtime_parameters_sync_intern.unbuffered.gain_ptr = hdl->prm_sync.linlogSweepWave.unbuffered.gain_ptr;
			return(JVX_DSP_NO_ERROR);
		}	
		else
		{
			hdl->prm_sync.linlogSweepWave.unbuffered.progress = (jvxData)newHdl->runtime.position/
				(jvxData)newHdl->runtime_parameters_sync_intern.unbuffered.length;
			hdl->prm_sync.linlogSweepWave.unbuffered.length = newHdl->runtime_parameters_sync_intern.unbuffered.length;
		}	
	}
	return(JVX_DSP_NO_ERROR);
}


jvxDspBaseErrorType 
jvx_generatorwave_update(jvx_generatorWave* hdl, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_UNSUPPORTED;
	
	if(hdl == NULL)
	{
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}

	switch(hdl->tpWave)
	{
	case JVX_GENERATOR_WAVE_SINE_UNBUFFERED:
		res = jvx_generator_wave_update_unbuffered_sine(hdl, whatToUpdate, do_set);
		break;

	case JVX_GENERATOR_WAVE_RECT_UNBUFFERED:
		res = jvx_generator_wave_update_unbuffered_rect(hdl, whatToUpdate, do_set);
		break;

	case JVX_GENERATOR_WAVE_RAMP_UNBUFFERED:
		res = jvx_generator_wave_update_unbuffered_ramp(hdl, whatToUpdate, do_set);
		break;

	case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:
		res = jvx_generator_wave_update_unbuffered_noise(hdl, whatToUpdate, do_set);
		break;

#ifdef JVX_FFT_PRESENT
	case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
		res = jvx_generator_wave_update_buffered_perfectsweep(hdl, whatToUpdate, do_set);
		break;
#endif

	case JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED:
		res = jvx_generator_wave_update_buffered_wavplayer(hdl, whatToUpdate, do_set);
		break;

	case JVX_GENERATOR_WAVE_LOGSWEEP:
	case JVX_GENERATOR_WAVE_LINEARSWEEP:
		res = jvx_generator_wave_update_unbuffered_linlogsweep(hdl, whatToUpdate, do_set);
		break;

	}
	
	return(res);
}


