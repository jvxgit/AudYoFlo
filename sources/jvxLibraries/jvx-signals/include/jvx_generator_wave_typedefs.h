/* 
 *****************************************************
 * Filename: jvx_generator_wave_typedefs.h
 *****************************************************
 * Project: RTProc-ESP (Echtzeit-Software-Plattform) *
 *****************************************************
 * Description:                                      *
 *****************************************************
 * Developed by JAVOX SOLUTIONS GMBH, 2012           *
 *****************************************************
 * COPYRIGHT BY JAVOX SOLUTION GMBH                  *
 *****************************************************
 * Contact: rtproc@javox-solutions.com               *
 *****************************************************
*/

#ifndef __JVX_GENERATOR_WAVE_TYPEDEFS_H__
#define __JVX_GENERATOR_WAVE_TYPEDEFS_H__

// =========================================================
// ENUM TYPEDEFS ENUM TYPEDEFS ENUM TYPEDEFS ENUM TYPEDEFS ENUM TYPEDEFS
// =========================================================

typedef enum
{
	JVX_GENERATOR_WAVE_STOPPED = 0,
	JVX_GENERATOR_WAVE_STARTED = 1,
	JVX_GENERATOR_WAVE_COMPLETE = 2
} jvxGeneratorWaveStatus;

typedef enum
{
	JVX_GENERATOR_WAVE_NONE = 0,
	JVX_GENERATOR_WAVE_SINE_UNBUFFERED = 1,
	JVX_GENERATOR_WAVE_RECT_UNBUFFERED = 2,
	JVX_GENERATOR_WAVE_RAMP_UNBUFFERED = 3,
	JVX_GENERATOR_WAVE_NOISE_UNBUFFERED = 4,
	JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED = 5,
	JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED = 6,
	JVX_GENERATOR_WAVE_LOGSWEEP = 7,
	JVX_GENERATOR_WAVE_LINEARSWEEP = 8
} jvxGeneratorWaveType;

typedef enum
{
	JVX_GENERATOR_UNBUFFERED = 0,
	JVX_GENERATOR_BUFFERED = 1
} jvxGeneratorWaveBuffertype;

typedef enum
{
	JVX_GENERATOR_NOISE_UNIFORM = 0,
	JVX_GENERATOR_NOISE_NORMAL = 1
} jvxGeneratorWaveNoiseType;

// =========================================================
// COMMON COMMON COMMON COMMON COMMON COMMON COMMON COMMON
// =========================================================

typedef struct
{
	jvxInt32 samplerate;
}jvx_generator_wave_common_parameter_init;

typedef struct
{
	jvxGeneratorWaveStatus theStatus;
	jvxData amplitude;
}jvx_generator_wave_common_parameter_runtime_async;

// =========================================================
// PERIODIC PERIODIC PERIODIC PERIODIC PERIODIC PERIODIC PERIODIC
// =========================================================

typedef struct
{
	jvx_generator_wave_common_parameter_init common;
} jvx_generator_wave_periodic_parameter_init;

typedef struct
{
	jvx_generator_wave_common_parameter_runtime_async common;
	jvxData frequency;
} jvx_generator_wave_periodic_parameter_runtime_async;

typedef struct
{
	jvxData phase;
} jvx_generator_wave_periodic_parameter_runtime_sync;

// =========================================================
// UNBUFFERED UNBUFFERED UNBUFFERED UNBUFFERED UNBUFFERED 
// =========================================================

typedef struct
{
	jvxData progress;
	size_t length;
	jvxData* freq_ptr;
	jvxData* gain_ptr;
}jvx_generator_wave_unbuffered_parameter_runtime_sync;

// =========================================================
// BUFFERED BUFFERED BUFFERED BUFFERED BUFFERED BUFFERED
// =========================================================

typedef struct
{
	jvxData progress;
	size_t length;
	jvxData* bufField;
}jvx_generator_wave_buffered_parameter_runtime_sync;


// =========================================================
// SINE WAVE SINE WAVE SINE WAVE SINE WAVE SINE WAVE SINE WAVE
// =========================================================

typedef struct
{
	jvx_generator_wave_periodic_parameter_init periodic;
} jvx_generator_wave_sine_parameter_init;

typedef struct
{
	jvx_generator_wave_periodic_parameter_runtime_async periodic;
} jvx_generator_wave_sine_parameter_runtime_async;

typedef struct
{
	jvx_generator_wave_periodic_parameter_runtime_sync periodic;
} jvx_generator_wave_sine_parameter_runtime_sync;

// =========================================================
// RECT RECT RECT RECT RECT RECT RECT RECT RECT RECT RECT
// =========================================================

typedef struct
{
	jvx_generator_wave_periodic_parameter_init periodic;
} jvx_generator_wave_rect_parameter_init;

typedef struct
{
	jvx_generator_wave_periodic_parameter_runtime_async periodic;
	jvxData pulsewidth;
} jvx_generator_wave_rect_parameter_runtime_async;

typedef struct
{
	jvx_generator_wave_periodic_parameter_runtime_sync periodic;
} jvx_generator_wave_rect_parameter_runtime_sync;
// =========================================================
// RAMP RAMP RAMP RAMP RAMP RAMP RAMP RAMP RAMP RAMP
// =========================================================

typedef struct
{
	jvx_generator_wave_periodic_parameter_init periodic;
} jvx_generator_wave_ramp_parameter_init;

typedef struct
{
	jvx_generator_wave_periodic_parameter_runtime_async periodic;
	jvxData pulsewidth1;
	jvxData pulsewidth2;
} jvx_generator_wave_ramp_parameter_runtime_async;

typedef struct
{
	jvx_generator_wave_periodic_parameter_runtime_sync periodic;
} jvx_generator_wave_ramp_parameter_runtime_sync;

// =========================================================
// UNBUFFERED NOISE UNBUFFERED NOISE UNBUFFERED NOISE
// =========================================================

typedef struct
{
	jvx_generator_wave_common_parameter_init common;
	unsigned long seed;
} jvx_generator_wave_noise_parameter_init;

typedef struct
{
	jvx_generator_wave_common_parameter_runtime_async common;
	jvxGeneratorWaveNoiseType noiseType;
} jvx_generator_wave_noise_parameter_runtime_async;

// =========================================================
// PERFECT_SWEEP PERFECT_SWEEP PERFECT_SWEEP PERFECT_SWEEP
// =========================================================

typedef struct
{
	jvx_generator_wave_common_parameter_init common;
} jvx_generator_wave_perfectsweep_parameter_init;

typedef struct
{
	jvx_generator_wave_common_parameter_runtime_async common;
	jvxData silenceStartSeconds;
	jvxData lengthSeconds;
	jvxData silenceStopSeconds;
	jvxInt32 loopCount;
} jvx_generator_wave_perfectsweep_parameter_runtime_async;


typedef struct
{
	jvx_generator_wave_buffered_parameter_runtime_sync buffered;
	jvxSize itCount;
} jvx_generator_wave_perfectsweep_parameter_runtime_sync;

// =========================================================
// WAV PLAYER WAV PLAYER WAV PLAYER WAV PLAYER WAV PLAYER
// =========================================================

typedef struct
{
	jvx_generator_wave_common_parameter_init common;
} jvx_generator_wave_wavplayer_parameter_init;

typedef struct
{
	jvx_generator_wave_common_parameter_runtime_async common;
	jvxInt32 loopCount;
} jvx_generator_wave_wavplayer_parameter_runtime_async;


typedef struct
{
	jvx_generator_wave_buffered_parameter_runtime_sync buffered;
	jvxSize itCount;
//	jvxData* fldExtern;
//	size_t szFldExtern;
} jvx_generator_wave_wavplayer_parameter_runtime_sync;

// =========================================================
// LOGLIN_SWEEP LOGLIN_SWEEP LOGLIN_SWEEP LOGLIN_SWEEP LOGLIN_SWEEP
// =========================================================

typedef struct
{
	jvx_generator_wave_common_parameter_init common;
} jvx_generator_wave_linlogsweep_parameter_init;

typedef struct
{
	jvx_generator_wave_common_parameter_runtime_async common;
	jvxData silenceStartSeconds;
	jvxData lengthSeconds;
	jvxData silenceStopSeconds;
	jvxInt32 loopCount;
	jvxData fLow;
	jvxData fUp;
} jvx_generator_wave_linlogsweep_parameter_runtime_async;


typedef struct
{
	jvx_generator_wave_unbuffered_parameter_runtime_sync unbuffered;
	jvxSize itCount;
} jvx_generator_wave_linlogsweep_parameter_runtime_sync;

// ========================================================
// ========================================================
// ========================================================

#endif
