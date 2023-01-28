#ifndef __JVX_SAMPLE_BY_SAMPLE_RESAMPLER_H__
#define __JVX_SAMPLE_BY_SAMPLE_RESAMPLER_H__

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "jvx_dsp_base.h"

typedef enum
{
	SAMPLE_BY_SAMPLE_RESAMPLER_STRATEGY_LOWPASS = 0,
	SAMPLE_BY_SAMPLE_RESAMPLER_STRATEGY_ENVELOPE = 1,
	SAMPLE_BY_SAMPLE_RESAMPLER_STRATEGY_ABS_ENVELOPE = 2
} sample_by_sample_strategy;

// Struct for fir, private variables
typedef struct
{
	struct
	{
		int id;
	} ident;

	struct
	{
		jvxInt32 oversamplingFactor;
		jvxInt32 downsamplingFactor;
		jvxData stopbandattenuation_db;
		jvxData socket;
		jvxInt32 lFilter;
		jvxInt32 lFilterUpsampled;
		jvxInt32 midPosition;
		jvxData alpha;
		jvxData delta_f;
		jvxData group_delay;
		jvxData correctionFactor;
		jvxInt32 lStates;
		sample_by_sample_strategy theStrategy;
	} constants;

	struct
	{
		jvxData* impResp;
		struct
		{
			jvxData* states;
			jvxSize idxWrite;
		} circBuffer;
	} ram;

	struct
	{
		jvxInt32 inPhaseCounter_input;
		jvxInt32 inPhaseCounter_output;
	} runtime;

} sample_by_sample_resampler_private;

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// Fixed resampler, VARIANT FIR
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

/**
 * Initialize fixed resampler, variant FIR.
 *///=========================================================================================================
jvxDspBaseErrorType
jvx_sample_by_sample_resampler_init(sample_by_sample_resampler_private** handle, jvxData* delay_samples, int lFilter, int upSample, int downSample,
                                    int stopband_attenuation_dB, jvxData fg, jvxData fg_min, jvxData correctionFactor,
                                    sample_by_sample_strategy theStrat);
                                    /* int stopband_attenuation_dB = 60, jvxData fg = -1, jvxData fg_min = -1, jvxData correctionFactor = -1, */
                                    /* sample_by_sample_strategy theStrat = SAMPLE_BY_SAMPLE_RESAMPLER_STRATEGY_LOWPASS); */

/**
 * Terminate resampler FIR.
 *///=========================================================================================================

jvxDspBaseErrorType
jvx_sample_by_sample_resampler_terminate(sample_by_sample_resampler_private* handle);

/**
  Be careful with the sample-by-sample resampler: The purpose is to have one end of variable
  framesize, either the input or the output. The other end is fixed. We have to run this
  resampler until the fixed side is complete. If you use this resampler with a fixed buffersize
  for input and output, this reampler most likely will not work propperly.
  We might also make a phase modification in the input or output to speed up or slow down the playback.
 *///====================================================================================================
inline jvxDspBaseErrorType
jvx_sample_by_sample_resampler_process(jvxData in, jvxData* out,
			 jvxCBool* inputConsumed, jvxCBool* outputProduced,
			 sample_by_sample_resampler_private* handle)
{
	int i;

	jvxData accuOut = 0;
	int modPhase;
	jvxSize inFrameOffset;
	jvxData* ptrStates;

	*inputConsumed = false;
	*outputProduced = false;

	modPhase = JVX_MIN(handle->runtime.inPhaseCounter_output, handle->runtime.inPhaseCounter_input);
	handle->runtime.inPhaseCounter_output -= modPhase;
	handle->runtime.inPhaseCounter_input -= modPhase;
	inFrameOffset = (handle->constants.oversamplingFactor - handle->runtime.inPhaseCounter_input);// <- this must be the distance to the LAST added input
	 ptrStates = NULL;

	if(handle->runtime.inPhaseCounter_input == 0)
	{
		inFrameOffset = 0;//<- new sample, therefore inFrameOffset becomes 0
		handle->ram.circBuffer.idxWrite = (handle->ram.circBuffer.idxWrite + 1) % handle->constants.lStates;
		ptrStates = handle->ram.circBuffer.states + handle->ram.circBuffer.idxWrite;
		*ptrStates = in;
		*inputConsumed = true;
		handle->runtime.inPhaseCounter_input = handle->constants.oversamplingFactor;
	}

	if(handle->runtime.inPhaseCounter_output == 0)
	{
		jvxData accu = 0.0;

		jvxSize ll1 = handle->ram.circBuffer.idxWrite + 1;
		jvxSize ll2 = handle->constants.lStates - ll1;
		jvxData* ptrCoeffs  = NULL;

		jvxData accuMin = 0, accuMax = 0;

		ptrStates = handle->ram.circBuffer.states + handle->ram.circBuffer.idxWrite;

		switch(handle->constants.theStrategy)
		{
		case SAMPLE_BY_SAMPLE_RESAMPLER_STRATEGY_LOWPASS:

			ptrCoeffs = handle->ram.impResp + inFrameOffset;
			for(i = 0; i < ll1; i++)
			{
				accu += *ptrStates-- * *ptrCoeffs;
				ptrCoeffs += handle->constants.oversamplingFactor;
			}

			ptrStates = handle->ram.circBuffer.states + handle->constants.lStates -1;
			for(i = 0; i < ll2; i++)
			{
				accu += *ptrStates-- * *ptrCoeffs;
				ptrCoeffs += handle->constants.oversamplingFactor;
			}
			accuOut = accu * handle->constants.correctionFactor;
			break;
		case SAMPLE_BY_SAMPLE_RESAMPLER_STRATEGY_ENVELOPE:

			for(i = 0; i < ll1; i++)
			{
				accuMin = JVX_MIN(accuMin, *ptrStates);
				accuMax = JVX_MAX(accuMax, *ptrStates);
				ptrStates--;
			}

			ptrStates = handle->ram.circBuffer.states + handle->constants.lStates -1;
			for(i = 0; i < ll2; i++)
			{
				accuMin = JVX_MIN(accuMin, *ptrStates);
				accuMax = JVX_MAX(accuMax, *ptrStates);
				ptrStates--;
			}
			if(fabs(accuMin) > fabs(accuMax))
			{
				accuOut = accuMin;
			}
			else
			{
				accuOut = accuMax;
			}
			break;
		case SAMPLE_BY_SAMPLE_RESAMPLER_STRATEGY_ABS_ENVELOPE:
			for(i = 0; i < ll1; i++)
			{
				accuOut = JVX_MAX(accuOut, fabs(*ptrStates));
				ptrStates--;
			}

			ptrStates = handle->ram.circBuffer.states + handle->constants.lStates -1;
			for(i = 0; i < ll2; i++)
			{
				accuOut = JVX_MAX(accuOut, fabs(*ptrStates));
				ptrStates--;
			}
			break;
		}

		*out = accuOut;
		*outputProduced = true;
		handle->runtime.inPhaseCounter_output = handle->constants.downsamplingFactor;
	}


	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType
jvx_sample_by_sample_resampler_modifyPhase(jvxInt32 modPhase, jvxInt32* modPhase_done, jvxCBool actOnInput, sample_by_sample_resampler_private* handle);

inline jvxDspBaseErrorType
jvx_sample_by_sample_resampler_predict(jvxCBool* nextTimeInput, jvxCBool* nextTimeOutput, sample_by_sample_resampler_private* handle)
{
	*nextTimeOutput = (handle->runtime.inPhaseCounter_output <= handle->runtime.inPhaseCounter_input);
	*nextTimeInput = (handle->runtime.inPhaseCounter_input <= handle->runtime.inPhaseCounter_output);
	return(JVX_DSP_NO_ERROR);
}

#endif
