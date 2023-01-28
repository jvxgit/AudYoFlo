#ifndef __JVXIOSHOST_FINETUNING_H__
#define __JVXIOSHOST_FINETUNING_H__

// ===============================================
// Some fine tuning arguments
// ===============================================
// UI timer for main window refresh
#define JVX_TIMER_SEC 0.1

// For all log values, do not compute log of smaller values
#define JVX_EPS 1e-5

// Slider scaling factor
#define JVX_MAX_VAL_DB_SLIDER 40

// Level before saturation (red color) is shown
#define JVX_MAX_LEVEL 0.95

// Specify quality of the resamplers, 0: 32 taps, 1: 64 taps, 2: 128 taps
#define JVX_QUALITY_RESAMPLER_ID_IN 1
#define JVX_QUALITY_RESAMPLER_ID_OUT 1

#endif
