#ifndef __JVX_RESAMPLER_PLOTTER_H__
#define __JVX_RESAMPLER_PLOTTER_H__

#include "jvx_dsp_base.h"
#include <assert.h>

JVX_DSP_LIB_BEGIN

// Struct for fir, private variables
typedef struct
{
	struct
	{
		int id;
	} ident;

	struct
	{
		jvxData downsamplingFactor;
	} constants;

	struct
	{
		jvxData currentMinVal;
		jvxData currentMaxVal;
	} ram;

	struct
	{
		jvxInt32 inPhaseCounter_input;
		jvxData inPhaseCounter_output;
	} runtime;

} resampler_plotter_private;

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// Fixed resampler, VARIANT FIR
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

/**
 * Initialize fixed resampler, variant FIR.
 *///=========================================================================================================
jvxDspBaseErrorType jvx_resampler_plotter_init(resampler_plotter_private** handle, jvxData downSample);

/**
 * Terminate resampler FIR.
 *///=========================================================================================================

/**
 * Initialize fixed resampler, variant FIR.
 *///=========================================================================================================
jvxDspBaseErrorType jvx_resampler_plotter_reconfig(resampler_plotter_private* handle, jvxData downSample);

jvxDspBaseErrorType
jvx_resampler_plotter_terminate(resampler_plotter_private* handle);

/**
  Be careful with the sample-by-sample resampler: The purpose is to have one end of variable
  framesize, either the input or the output. The other end is fixed. We have to run this
  resampler until the fixed side is complete. If you use this resampler with a fixed buffersize
  for input and output, this reampler most likely will not work propperly.
  We might also make a phase modification in the input or output to speed up or slow down the playback.
 *///====================================================================================================
inline jvxDspBaseErrorType
jvx_resampler_plotter_process(jvxData in,
			 jvxData* out_min, jvxData* out_max, jvxCBool* outputProduced,
			 resampler_plotter_private* handle)
{
	//jvxData accuOut = 0;

	*outputProduced = false;

	handle->runtime.inPhaseCounter_output--;

	if(in < handle->ram.currentMinVal)
	{
		handle->ram.currentMinVal = in;
	}
	if(in > handle->ram.currentMaxVal)
	{
		handle->ram.currentMaxVal = in;
	}

	if(handle->runtime.inPhaseCounter_output <= 0)
	{
		*out_min = handle->ram.currentMinVal;
		*out_max =  handle->ram.currentMaxVal;
		handle->ram.currentMaxVal = JVX_DATA_MAX_NEG;
		handle->ram.currentMinVal = JVX_DATA_MAX_POS;
		*outputProduced = true;
		handle->runtime.inPhaseCounter_output += handle->constants.downsamplingFactor;
	}

	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType
jvx_resampler_plotter_reconfigure(resampler_plotter_private* handle, jvxData downSample);

/**
  Be careful with the sample-by-sample resampler: The purpose is to have one end of variable
  framesize, either the input or the output. The other end is fixed. We have to run this
  resampler until the fixed side is complete. If you use this resampler with a fixed buffersize
  for input and output, this reampler most likely will not work propperly.
  We might also make a phase modification in the input or output to speed up or slow down the playback.
 *///====================================================================================================
inline jvxDspBaseErrorType
jvx_resampler_plotter_process_reconfig(jvxData in,
			 jvxData* out_min, jvxData* out_max, jvxCBool* outputProduced,
			 resampler_plotter_private* handle, jvxData downSampleFac, jvxCBool* wasReconfigured)
{
	//jvxData accuOut = 0;

	*outputProduced = false;

	handle->runtime.inPhaseCounter_output--;

	if(in < handle->ram.currentMinVal)
	{
		handle->ram.currentMinVal = in;
	}
	if(in > handle->ram.currentMaxVal)
	{
		handle->ram.currentMaxVal = in;
	}

	if(handle->runtime.inPhaseCounter_output <= 0)
	{
		*out_min = handle->ram.currentMinVal;
		*out_max =  handle->ram.currentMaxVal;
		handle->ram.currentMaxVal = JVX_DATA_MAX_NEG;
		handle->ram.currentMinVal = JVX_DATA_MAX_POS;
		*outputProduced = true;
		handle->runtime.inPhaseCounter_output += handle->constants.downsamplingFactor;
	}

	if(	downSampleFac != handle->constants.downsamplingFactor)
	{
		handle->constants.downsamplingFactor = downSampleFac;
		handle->runtime.inPhaseCounter_output = 0;
		handle->runtime.inPhaseCounter_input = 0;
		if(wasReconfigured)
		{
			*wasReconfigured = true;
		}
	}
	else
	{
		if(wasReconfigured)
		{
			*wasReconfigured = false;
		}
	}
	return(JVX_DSP_NO_ERROR);
}


inline jvxDspBaseErrorType
jvx_resampler_plotter_predict(jvxCBool* nextTimeInput, jvxCBool* nextTimeOutput, resampler_plotter_private* handle)
{
	*nextTimeOutput = (handle->runtime.inPhaseCounter_output <= 1);
	*nextTimeInput = true;
	return(JVX_DSP_NO_ERROR);
}


JVX_DSP_LIB_END

#endif
