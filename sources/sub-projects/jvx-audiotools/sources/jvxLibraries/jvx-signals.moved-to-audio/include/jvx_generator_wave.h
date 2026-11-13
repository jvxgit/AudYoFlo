/* 
 *****************************************************
 * Filename: jvx_generator_wave.h
 *****************************************************
 * Project: RTProc-ESP (Echtzeit-Software-Plattform) *
 *****************************************************
 * Description:                                      *
 *****************************************************
 * Developed by JAVOX SOLUTIONS GMBH, 2012           *
 *****************************************************
 * COPYRIGHT BY JAVOX SOLUTION GMBH                  *
 *****************************************************
 * Contact: jvxroc@javox-solutions.com               *
 *****************************************************
*/

#ifndef __JVX_GENERATOR_WAVE_H__
#define __JVX_GENERATOR_WAVE_H__

#include "jvx_dsp_base.h"
#include "jvx_generator_wave_typedefs.h"

// main struct
typedef struct 
{
	jvxGeneratorWaveType tpWave;
	
	jvxHandle* prv;

	union
	{
		jvx_generator_wave_sine_parameter_init sineWave;
		jvx_generator_wave_rect_parameter_init rectWave;
		jvx_generator_wave_ramp_parameter_init rampWave;
		jvx_generator_wave_noise_parameter_init noiseWave;
		jvx_generator_wave_perfectsweep_parameter_init perfSweepWave;
		jvx_generator_wave_wavplayer_parameter_init wavWave;
		jvx_generator_wave_linlogsweep_parameter_init linlogSweepWave;
	} prm_init;

	union
	{
		jvx_generator_wave_sine_parameter_runtime_async sineWave;
		jvx_generator_wave_rect_parameter_runtime_async rectWave;
		jvx_generator_wave_ramp_parameter_runtime_async rampWave;
		jvx_generator_wave_noise_parameter_runtime_async noiseWave;
		jvx_generator_wave_perfectsweep_parameter_runtime_async perfSweepWave;
		jvx_generator_wave_wavplayer_parameter_runtime_async wavWave;
		jvx_generator_wave_linlogsweep_parameter_runtime_async linlogSweepWave;
	} prm_async;

	union
	{
		jvx_generator_wave_sine_parameter_runtime_sync sineWave;
		jvx_generator_wave_rect_parameter_runtime_sync rectWave;
		jvx_generator_wave_ramp_parameter_runtime_sync rampWave;
		//jvx_generator_wave_noise_parameter_runtime_sync noiseWave;
		jvx_generator_wave_perfectsweep_parameter_runtime_sync perfSweepWave;
		jvx_generator_wave_wavplayer_parameter_runtime_sync wavWave;
		jvx_generator_wave_linlogsweep_parameter_runtime_sync linlogSweepWave;
	} prm_sync;

	// public submodules

} jvx_generatorWave;


jvxDspBaseErrorType jvx_generatorwave_initConfig(jvx_generatorWave* hdl);

jvxDspBaseErrorType jvx_generatorwave_activate(jvx_generatorWave* hdl);

jvxDspBaseErrorType jvx_generatorwave_prepare(jvx_generatorWave* hdl);

jvxDspBaseErrorType jvx_generatorwave_update(jvx_generatorWave* hdl, jvxUInt16 whatToSet, jvxCBool do_set);

jvxDspBaseErrorType jvx_generatorwave_process(jvx_generatorWave* hdl, jvxData* bufferFill, size_t lField, size_t* written);

jvxDspBaseErrorType jvx_generatorwave_postprocess(jvx_generatorWave* hdl);

jvxDspBaseErrorType jvx_generatorwave_deactivate(jvx_generatorWave* hdl);


#endif

