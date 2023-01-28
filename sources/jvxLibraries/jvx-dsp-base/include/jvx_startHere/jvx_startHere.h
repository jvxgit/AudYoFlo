#ifndef __JVX_STARTHERE__H__
#define __JVX_STARTHERE__H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

#define JVX_STARTHERE_DESCRIPTION "Javox Demo Algorithm"
#define JVX_STARTHERE_VERSION "0.1"

// Configuration parameters which can be changed anytime
typedef struct  {
  jvxData gain_dB;
  jvxInt16 flip;
} jvx_startHere_prm;

// Configuration parameters which can be changed between buffer processing
typedef struct  {
  int dummy;
} jvx_startHere_prmSync;

// readOnly struct
typedef struct {

  // set on init
  // ...

  // updated in each frame
  unsigned int frameCounter;

  // updated on async prm update
  // ...

  // updated on sync prm update
  // ...

} jvx_startHere_readOnly;


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
  jvx_startHere_prmSync prmSync;
  jvx_startHere_prm prm;
  jvx_startHere_readOnly readOnly;

  // public submodules
  // ... none ...

} jvx_startHere;



jvxDspBaseErrorType
jvx_startHere_configInit(jvx_startHere* hdl,
                         unsigned int pid);

jvxDspBaseErrorType
jvx_startHere_init(jvx_startHere* hdl,
                   int frameSize,
                   int nChannelsIn,
                   int nChannelsOut,
                   int samplingRate);

jvxDspBaseErrorType
jvx_startHere_process(jvx_startHere* hdl,
                      jvxData** in,
                      jvxData** out);

jvxDspBaseErrorType
jvx_startHere_terminate(jvx_startHere* hdl);


jvxDspBaseErrorType
jvx_startHere_update(jvx_startHere* hdl, jvxCBool syncUpdate);

JVX_DSP_LIB_END

#endif
