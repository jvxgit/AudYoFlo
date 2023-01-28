#ifndef __JVX_SPECTRUM_ESTIMATION_H__
#define __JVX_SPECTRUM_ESTIMATION_H__

#include "jvx_dsp_base.h"
#include "jvx_windows/jvx_windows.h"

JVX_DSP_LIB_BEGIN

#define JVX_SPECTRUMESTIMATION_DESCRIPTION "Javox spectrum estimation"
#define JVX_SPECTRUMESTIMATION_VERSION "1.0"

// additional types for parameters
enum {
  jvx_spectrumEstimation_welchRecursive,
  jvx_spectrumEstimation_instantaneous,
  //other ideas:
  // jvx_spectrumEstimation_welchMovingAverage
  // jvx_spectrumEstimation_lpc
};
typedef jvxInt16 jvx_spectrumEstimationMethod;

// Configuration parameters which can be changed anytime
typedef struct  {
	// Required for compilation in Windows
	int dummy;
} jvx_spectrumEstimation_prm;


// Configuration parameters which can be changed between buffer processing
typedef struct  {

  jvx_spectrumEstimationMethod method;
  jvxInt16 logarithmic;
  jvxInt16 fftLengthLog2;
  jvx_windowType windowType;
  jvxData winArgA;
  jvxData winArgB;

  // parameters for welchRecursive method
  jvxData alpha;

  // other spectrum estimation methods tbd.

} jvx_spectrumEstimation_prmSync;


// readOnly struct
typedef struct {

  // set on init
  // ...

  // updated in each frame
  unsigned int frameCounter;

  // updated on async prm update
  // ...

  // updated on sync prm update
  jvxSize spectrumSize;
  jvxData *spectrum;

} jvx_spectrumEstimation_readOnly;


// main struct
typedef struct {

  // constant parameters
  unsigned int id;
  unsigned int pid;
  char *description;
  char *version;
  int frameSize;
  int samplingRate;
  int nChannelsIn;
  int nChannelsOut;

  // sub-structs
  jvxHandle* prv;
  jvx_spectrumEstimation_prmSync prmSync;
  jvx_spectrumEstimation_prm prm;
  jvx_spectrumEstimation_readOnly readOnly;

  // public submodules
  // ...

} jvx_spectrumEstimation;


////
// API
////

jvxDspBaseErrorType jvx_spectrumEstimation_configInit(jvx_spectrumEstimation *hdl,
                                                      unsigned int pid);

jvxDspBaseErrorType jvx_spectrumEstimation_init(jvx_spectrumEstimation* hdl,
                                                int frameSize,
                                                int nChannelsIn,
                                                int samplingRate);

// input: frameSize
// output: pointer to (fftLength/2+1)
jvxDspBaseErrorType jvx_spectrumEstimation_process(jvx_spectrumEstimation* hdl,
                                                   jvxData* in,
                                                   jvxData** out,
                                                   jvxSize channel);

jvxDspBaseErrorType jvx_spectrumEstimation_terminate(jvx_spectrumEstimation* hdl);


jvxDspBaseErrorType jvx_spectrumEstimation_update(jvx_spectrumEstimation* hdl,
                                                  jvxCBool syncUpdate);

JVX_DSP_LIB_END

#endif
