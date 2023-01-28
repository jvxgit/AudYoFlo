#ifndef __JVX_POWERALIGNMENT__H__
#define __JVX_POWERALIGNMENT__H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

#define JVX_POWERALIGN_DESCRIPTION "Javox Power Alignment"
#define JVX_POWERALIGN_VERSION "1.0"

// additional types for parameters
enum {
  jvx_powerAlignment_recursive,
  jvx_powerAlignment_block,
  jvx_powerAlignment_off
};
typedef jvxInt16 jvx_powerAlignmentMode;


// Configuration parameters which can be changed anytime
typedef struct  {
  jvx_powerAlignmentMode mode;
  jvxData ar; // recursive power smoothing constant
  jvxData as; // gain factor smoothing constant
} jvx_powerAlign_prm;


// Configuration parameters which can be changed between buffer processing
typedef struct  {
  int dummy;
} jvx_powerAlign_prmSync;


// debug struct
typedef struct {

  // set on init
  // ...

  // updated in each frame
  unsigned int frameCounter;

  // updated on async prm update
  // ...

  // updated on sync prm update
  // ...


} jvx_powerAlign_readOnly;


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
  jvx_powerAlign_prmSync prmSync;
  jvx_powerAlign_prm prm;
  jvx_powerAlign_readOnly readOnly;

  // public submodules
  // ...

} jvx_powerAlign;



jvxDspBaseErrorType jvx_powerAlign_configInit(jvx_powerAlign* hdl,
                                              unsigned int pid);

jvxDspBaseErrorType jvx_powerAlign_init(jvx_powerAlign* hdl,
                                        int frameSize,
                                        int nChannels,
                                        int samplingRate);

jvxDspBaseErrorType jvx_powerAlign_process(jvx_powerAlign* hdl,
                                           jvxData** in,
                                           jvxData** out);

jvxDspBaseErrorType jvx_powerAlign_terminate(jvx_powerAlign* hdl);


jvxDspBaseErrorType jvx_powerAlign_update(jvx_powerAlign* hdl,
                                          jvxCBool syncUpdate);

JVX_DSP_LIB_END

#endif
