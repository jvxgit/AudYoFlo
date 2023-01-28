#ifndef __JVX_FFT_FRAMEWORK_PRV__H__
#define __JVX_FFT_FRAMEWORK_PRV__H__

#include "jvx_fft_processing/jvx_fft_framework.h"
//#include "jvx_circbuffer/jvx_circbuffer_fftifft.h"
#include "jvx_fft_tools/jvx_fft_core.h"


// RAM
typedef struct {
  jvxData **buf;
  jvxData *fftIn;
  jvxDataCplx *fftOut;
  jvxDataCplx *ifftIn;
  jvxData *ifftOut;
  jvxDataCplx *ifftInC2C;
  jvxDataCplx *ifftOutC2C;
  jvxData **olaBuf;
  jvxDataCplx *weightsAA;
} jvx_fftFramework_ram;

// State data (i.e. delay line memory).
typedef struct {
  jvxSize *idxBufFFT;
  jvxSize *idxBufIFFT;
  jvxSize lastIdxBufIFFT;
  jvxDataCplx **oldWeights;
} jvx_fftFramework_state;

// Parameters, which are derived from configuration but not set directly.
typedef struct  {
  // depending on synced prms
  jvxData winNormalize;
  jvxSize fftLength;
  jvxSize irLength;
  jvxSize irDelay;
  jvxSize spectrumSize;
  jvxData *window;
  jvxData *window_CF;
  jvxData *window_AA;
  jvxDataCplx *ifftIn_currentWeights;
  jvxDataCplx *ifftIn_oldWeights;
  int bufferSize;

  // not depending on synced prms
  // ... nothing at the moment ...

} jvx_fftFramework_cfg_derived;


// Private data struct
typedef struct
{
  jvx_fftFramework_ram ram;
  jvx_fftFramework_state state;
  jvx_fftFramework_cfg_derived derived;
  jvx_fftFramework_prmSync prmSync;
  jvx_fftFramework_prm prm;

  // submodules
  //jvx_circbuffer_fft **cb;
  jvxFFTGlobal* globalFFT;
  jvxFFT *fftR2C;
  jvxIFFT *ifftC2C;
  jvxIFFT *ifftC2R;

} jvx_fftFramework_prv;


#endif
