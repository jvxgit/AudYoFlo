
#ifndef __JVX_SPECTRUM_ESTIMATION_PRV__H__
#define __JVX_SPECTRUM_ESTIMATION_PRV__H__

#include "jvx_spectrumEstimation/jvx_spectrumEstimation.h"
#include "jvx_circbuffer/jvx_circbuffer_fftifft.h"
#include "jvx_fft_core_typedefs.h"
#include "jvx_fft_tools/jvx_fft_tools.h"


// RAM
typedef struct {
  jvxData *tmpBuf;
} jvx_spectrumEstimation_ram;

// State data (i.e. delay line memory).
typedef struct {
  jvxData *psd;
  jvxData *logpsd;
} jvx_spectrumEstimation_state;

// Parameters, which are derived from configuration but not set directly.
typedef struct  {
  // depending on synced prms
  jvxData *window;
  jvxData winNormalize;
  jvxSize fftLength;
  jvxSize spectrumSize;

  // not depending on synced prms
  // ... nothing at the moment ...

} jvx_spectrumEstimation_cfg_derived;


// Private data struct
typedef struct
{
  jvx_spectrumEstimation_ram ram;
  jvx_spectrumEstimation_state state;
  jvx_spectrumEstimation_cfg_derived derived;
  jvx_spectrumEstimation_prmSync prmSync;
  jvx_spectrumEstimation_prm prm;

  // submodules
  jvx_circbuffer_fft **cb;
  jvxHandle* globalFFT;

} jvx_spectrumEstimation_prv;


#endif
