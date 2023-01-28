#include "jvx_resampler/jvx_sample_by_sample_resampler.h"
#include "jvx_resampler/jvx_sample_by_sample_resampler_config.h"
#include "jvx_math/jvx_math_extensions.h"
#include "jvx_dsp_base.h"
#include "jvx_windows/jvx_windows.h"

jvxDspBaseErrorType
jvx_sample_by_sample_resampler_init(sample_by_sample_resampler_private** handle, jvxData* delay_samples, int lFilter, int upSample,
                                    int downSample, int stopband_attenuation_dB, jvxData fg, jvxData fg_min,
                                    jvxData correctionFactor, sample_by_sample_strategy theStrat)
{
  int i;
  jvxData fgOmega;
  jvxData Omega_g;
  jvxData m_pi_omega_g;

  // Do not accept NULL pointer for handle
  if(!handle)
    {
      return(JVX_DSP_ERROR_INVALID_ARGUMENT);
    }

  // ====================================================================
  // Start config
  // ====================================================================
  // Configuration for lowpass design, FIR filter with Kaiser window

  JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(*handle, sample_by_sample_resampler_private);

  (*handle)->ident.id = 1;

  // Some common parameters
  //handle->constants.buffersizeIn = 1;
  (*handle)->constants.downsamplingFactor = downSample;
  (*handle)->constants.oversamplingFactor = upSample;
  (*handle)->constants.correctionFactor = (jvxData)(*handle)->constants.oversamplingFactor;
  (*handle)->constants.theStrategy = theStrat;
  if(correctionFactor >= 0)
    {
      (*handle)->constants.correctionFactor = correctionFactor;
    }

  //handle->constants.buffersizeUpsampled = handle->constants.buffersizeIn * handle->constants.oversamplingFactor;

  // If we want to have a clear algorithmic delay only allow for odd
  // filter length
  if(lFilter%2 == 0)
    {
      lFilter = lFilter +1;
    }

  // ====================================================================
  // Low pass design Low pass design Low pass design Low pass design
  // ====================================================================
  // Setup some parameters
  (*handle)->constants.stopbandattenuation_db = (jvxData)stopband_attenuation_dB;
  (*handle)->constants.socket = SOCKET_FIR;
  (*handle)->constants.lFilter = lFilter;
  (*handle)->constants.lFilterUpsampled = (lFilter-1) * (*handle)->constants.oversamplingFactor + 1;
  (*handle)->constants.midPosition = (lFilter-1)/2 *(*handle)->constants.oversamplingFactor;
  (*handle)->constants.alpha = 0;

  // From the stopband attenuation, derive the factor alpha for Kaiser
  // window
  if((*handle)->constants.stopbandattenuation_db > 21)
    {
      if((*handle)->constants.stopbandattenuation_db > 50)
        {
          (*handle)->constants.alpha = 0.1102 * ((*handle)->constants.stopbandattenuation_db - 8.7);
        }
      else
        {
          (*handle)->constants.alpha = 0.5842 * pow(((*handle)->constants.stopbandattenuation_db -21.0), 0.4) + 0.07886*((*handle)->constants.stopbandattenuation_db -21);
        }
    }

  // ====================================================================
  // Allocate the relevant buffers for interpolation filter
  // ====================================================================

  // State buffer for convolution
  (*handle)->constants.lStates = (*handle)->constants.lFilter;
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z((*handle)->ram.circBuffer.states, jvxData, (*handle)->constants.lStates);
  memset((*handle)->ram.circBuffer.states, 0, sizeof(jvxData) * (*handle)->constants.lStates);
  (*handle)->ram.circBuffer.idxWrite = 0;

  // Allocate impulse response followed by some sapece for zeros: maximum offset in terms of shift is (handle->constants.oversamplingFactor - 1)
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z((*handle)->ram.impResp, jvxData, ((*handle)->constants.lFilterUpsampled + (*handle)->constants.oversamplingFactor - 1));
  memset((*handle)->ram.impResp, 0, sizeof(jvxData) * ((*handle)->constants.lFilterUpsampled + (*handle)->constants.oversamplingFactor -1));

  if((*handle)->constants.lFilterUpsampled == 1)
    {
      (*handle)->ram.impResp[0] = 1.0;
    }
  else
    {
      jvx_compute_window((*handle)->ram.impResp, (*handle)->constants.lFilterUpsampled, (*handle)->constants.alpha, 0,
                     JVX_WINDOW_KAISER, NULL);

      // Next, derive the desired frequency response parameters: Consider
      // a specific distance to the maximum allowed frequency
      (*handle)->constants.delta_f = 0;
      if ( (*handle)->constants.stopbandattenuation_db > 21.0 )
        {
          (*handle)->constants.delta_f = (( (*handle)->constants.stopbandattenuation_db - 7.95 ) )	/ ( 14.36 * ( (*handle)->constants.lFilterUpsampled - 1 ) );
        }
      else
        {
          (*handle)->constants.delta_f = ( 0.9222 ) / ( (*handle)->constants.lFilterUpsampled - 1 );
        }

      fgOmega = JVX_MIN(1.0/(jvxData)(*handle)->constants.oversamplingFactor, 1.0/(jvxData)(*handle)->constants.downsamplingFactor);
      Omega_g = ( fgOmega - (*handle)->constants.delta_f );
      if(fg > 0)
        {
          Omega_g = fg;
        }
      if(fg_min > 0)
        {
          Omega_g = JVX_MAX(fg_min, Omega_g);
        }
      m_pi_omega_g = M_PI * Omega_g;

      // Compute the time domain impulse response based on the sin(x)/x
      for(i = 0; i < (*handle)->constants.lFilterUpsampled; i++)
        {
          (*handle)->ram.impResp[i] *= jvx_si ( m_pi_omega_g * ((jvxData)i - (*handle)->constants.midPosition))* Omega_g;
        }
    }

  // Start the inphase counters
  (*handle)->runtime.inPhaseCounter_input = 0;
  (*handle)->runtime.inPhaseCounter_output = 0;

  // ====================================================================
  // Compute the delay of the resampler
  // ====================================================================

  //% Compute the group delay
  //%[handle.fir.groupDelay, x] = grpdelay(handle.fir.impResp,1); <-
  //version computed from impulse response
  (*handle)->constants.group_delay = ((*handle)->constants.midPosition) / (jvxData)(*handle)->constants.oversamplingFactor;

  if(delay_samples)
    {
      *delay_samples = (*handle)->constants.group_delay;
    }

  return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType
jvx_sample_by_sample_resampler_terminate(sample_by_sample_resampler_private* handle)
{
  // ====================================================================
  // Start config
  // ====================================================================
  // Configuration for lowpass design, FIR filter with Kaiser window

  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.impResp);
  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.circBuffer.states);

  JVX_DSP_SAFE_DELETE_OBJECT(handle);
  return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType
jvx_sample_by_sample_resampler_modifyPhase(jvxInt32 modPhase, jvxInt32* modPhase_done, jvxCBool actOnInput, sample_by_sample_resampler_private* handle)
{
  if(actOnInput)
    {
      jvxInt32 mod = JVX_MIN(modPhase, handle->runtime.inPhaseCounter_input);

      handle->runtime.inPhaseCounter_input -= mod;

      if(modPhase_done)
        {
          *modPhase_done = mod;
        }
    }
  else
    {
      jvxInt32 mod = JVX_MIN(modPhase, handle->runtime.inPhaseCounter_output);

      handle->runtime.inPhaseCounter_output -= mod;

      if(modPhase_done)
        {
          *modPhase_done = mod;
        }
    }
  return(JVX_DSP_NO_ERROR);
}
