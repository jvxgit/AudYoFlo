#include "jvx_dsp_base.h"
#include "jvx_resampler/jvx_resampler_plotter.h"

jvxDspBaseErrorType
jvx_resampler_plotter_init(resampler_plotter_private** handle, jvxData downSample)
{
  // Do not accept NULL pointer for handle
  if(!handle)
    {
      return JVX_DSP_ERROR_INVALID_ARGUMENT;
    }

  // ====================================================================
  // Start config
  // ====================================================================
  // Configuration for lowpass design, FIR filter with Kaiser window

  JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(*handle, resampler_plotter_private);

  (*handle)->ident.id = 1;

  // Some common parameters
  //handle->constants.buffersizeIn = 1;
  (*handle)->constants.downsamplingFactor = downSample;
  (*handle)->ram.currentMaxVal = JVX_DATA_MAX_NEG;
  (*handle)->ram.currentMinVal = JVX_DATA_MAX_POS;

  // Start the inphase counters
  (*handle)->runtime.inPhaseCounter_input = 0;
  (*handle)->runtime.inPhaseCounter_output = 0;

  return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_resampler_plotter_reconfigure(resampler_plotter_private* handle, jvxData downSample)
{
	// Some common parameters
	//handle->constants.buffersizeIn = 1;
	handle->constants.downsamplingFactor = downSample;
	handle->ram.currentMaxVal = JVX_DATA_MAX_NEG;
	handle->ram.currentMinVal = JVX_DATA_MAX_POS;

	// Start the inphase counters
	handle->runtime.inPhaseCounter_input = 0;
	handle->runtime.inPhaseCounter_output = 0;

	return JVX_DSP_NO_ERROR;
}



jvxDspBaseErrorType
jvx_resampler_plotter_reconfig(resampler_plotter_private* handle, jvxData downSample)
{
  handle->constants.downsamplingFactor = downSample;
  handle->runtime.inPhaseCounter_output = 0;
  handle->runtime.inPhaseCounter_input = 0;
  return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_resampler_plotter_terminate(resampler_plotter_private* handle)
{
  JVX_DSP_SAFE_DELETE_OBJECT(handle);
  return JVX_DSP_NO_ERROR;
}
