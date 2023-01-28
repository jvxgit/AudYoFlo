#ifndef __JVX_DETECT_PEAKS_H__
#define __JVX_DETECT_PEAKS_H__

#include "jvx_dsp_base.h"

typedef enum
{
	JVX_POSTPROCESS_STRAT_NONE,
	JVX_POSTPROCESS_STRAT_LEFT_TO_RIGHT,
	JVX_POSTPROCESS_STRAT_RIGHT_TO_LEFT
} jvxPostProcessSearchStartegy;

typedef struct
{
	jvxInt64 deltaT_smooth;
	jvxInt64 deltaT_findpeaks;	
	jvxInt64 deltaT_findincremental;
	jvxInt64 deltaT_findcog;
#ifdef JVX_DETECT_PEAKS_TIME_PROFILE
	jvxTimeStampData* tStamp;
#endif
} jvx_detect_peaks_profiler;

typedef struct
{
	jvxData corridor_sensitivity;
	jvxData* external_weights;
	jvxInt32 minProximity;
	jvxSize lookleftright;
	jvxInt16 searchGranularity_smooth;
	jvxInt16 searchGranularity_peaks;
	jvxData deviateGainFactor;
	jvxData deviateMinGain;
	jvxSize leftRightViewCog;
	jvxData exponentCog;
	jvxSize dist_left;
	jvxSize dist_right;
} jvx_detect_peaks_parameters;

typedef struct
{
	jvxInt16* idxStartArea;
	jvxInt16* idxStopArea;
	jvxInt16* idxMaxAboveMean;
	jvxInt16* idxMaxAbs;
	jvxData* maxValueAboveMean;
	jvxData*maxValueAll;
	jvxSize numberPositionsPeaksCurrent;
	jvxSize numberPositionsPeaksMax;
} jvx_detect_peaks_io_per_evenodd;

typedef struct
{
	jvxData* theData;
	jvxData* theDataSmoothed;
} jvx_detect_peaks_data_per_evenodd;

// ===================================================================================================0

JVX_DSP_LIB_BEGIN

jvxDspBaseErrorType jvx_detect_peaks_init(jvxHandle** theHdl, jvxSize halfLengthSlidingWindow, jvxSize dimVector);
jvxDspBaseErrorType jvx_detect_peaks_process_smooth(jvxHandle* theHdl, jvx_detect_peaks_data_per_evenodd* theInput);
jvxDspBaseErrorType jvx_detect_peaks_process_incremental(jvx_detect_peaks_data_per_evenodd* theInput, jvxInt32* posi,
												  jvx_detect_peaks_parameters* params, jvxSize minIdx, jvxSize maxIdx);

jvxDspBaseErrorType jvx_detect_peaks_process_full(jvxHandle* theHdl, jvx_detect_peaks_io_per_evenodd* theOutput,
							  jvx_detect_peaks_data_per_evenodd* theInput, 
							  jvxSize* offsetStart, jvxSize* offsetStop, 
							  jvxSize* numPulsesPerArea,
							  jvxSize numAreas, jvx_detect_peaks_parameters* params);
jvxDspBaseErrorType jvx_detect_peaks_process_simple(jvxHandle* theHdl, jvx_detect_peaks_io_per_evenodd* theOutput,
							  jvx_detect_peaks_data_per_evenodd* theInput, jvxSize* offsetStart, jvxSize* offsetStop, 
							  jvxSize numAreas, jvx_detect_peaks_parameters* params);
jvxDspBaseErrorType jvx_detect_peaks_terminate(jvxHandle* theHdl);

JVX_DSP_LIB_END

#endif
