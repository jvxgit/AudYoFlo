#ifndef __JVX_BEAMPATTERN__H__
#define __JVX_BEAMPATTERN__H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN


#define JVX_BEAMPATTERN_DESCRIPTION "Javox beampattern simulator"
#define JVX_BEAMPATTERN_VERSION "1.0"

// Configuration parameters which can be changed anytime
typedef struct  {
  void *dummy;
} jvx_beampattern_prm;


// Configuration parameters which can be changed between buffer processing
typedef struct  {
  jvxInt16 numAzimuthAngles;
  jvxInt16 numElevationAngles;
  jvxInt16 fftLengthLog2;
  jvxData radiusCircleAround;
  jvxInt16 fracDelayFilterDelay;
  jvxData azimuthMin;
  jvxData azimuthMax;
  jvxData elevationMin;
  jvxData elevationMax;
  jvxCBool removePropagationToArray;
  jvxData speedOfSound;
} jvx_beampattern_prmSync;


// readOnly struct
typedef struct {

  // set on init
  // ...

  // updated in each frame
  unsigned int frameCounter;

  // updated on async prm update
  // ...

  // updated on sync prm update
  jvxInt16 spectrumSize;
  jvxData *spectrum;

} jvx_beampattern_readOnly;


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
  jvx_beampattern_prmSync prmSync;
  jvx_beampattern_prm prm;
  jvx_beampattern_readOnly readOnly;

  // public submodules
  // ..

} jvx_beampattern;



// additional typdefs
typedef jvxDspBaseErrorType (*jvx_beampattern_process_cb)(jvxHandle* hdl,
                                                          int frameSize,
                                                          int nChannelsIn,
                                                          int samplingRate,
                                                          jvxData** in,
                                                          jvxData* out);

jvxDspBaseErrorType jvx_beampattern_configInit(jvx_beampattern* hdl,
                                               unsigned int pid);

jvxDspBaseErrorType jvx_beampattern_init(jvx_beampattern* hdl,
                                         int samplingRate,
                                         jvxHandle *bf_hdl,
                                         jvx_beampattern_process_cb bf_cb);

/*
 * Coordinate system:
 *
 *
 *                         | +z (90°/undefined)
 *                         |
 *                         |     (0°/0°)
 *                         |    / +y
 *                         |   /
 *                         |  /
 *                         | /
 *                         | ---------------- +x (0°/270°)
 */
jvxDspBaseErrorType jvx_beampattern_addMicrophone(jvx_beampattern* hdl,
                                                  jvxData x,
                                                  jvxData y,
                                                  jvxData z,
                                                  jvxSize *id);

jvxDspBaseErrorType jvx_beampattern_removeAllMicrophones(jvx_beampattern* hdl);

jvxDspBaseErrorType jvx_beampattern_removeMicrophone(jvx_beampattern* hdl,
                                                     jvxSize id);

jvxDspBaseErrorType jvx_beampattern_printMicrophones(jvx_beampattern* hdl);

jvxDspBaseErrorType jvx_beampattern_process(jvx_beampattern* hdl,
                                            jvxData **out);

jvxDspBaseErrorType jvx_beampattern_terminate(jvx_beampattern* hdl);


jvxDspBaseErrorType jvx_beampattern_update(jvx_beampattern* hdl,
                                           jvxCBool syncUpdate);

JVX_DSP_LIB_END

#endif
