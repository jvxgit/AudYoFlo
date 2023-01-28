#ifndef __JVX_BEAMPATTERN_PRV__H__
#define __JVX_BEAMPATTERN_PRV__H__

#include "jvx_beampattern/jvx_beampattern.h"

// RAM
typedef struct {
  jvxData *anglesElevation;
  jvxData *anglesAzimuth;
  jvxData ****testSignals;
  jvxData *response;
  jvxDataCplx *responseFFT;
  jvxData *result;
} jvx_beampattern_ram;

// microphones are handeled as submodules
struct microphone {
  jvxData x;
  jvxData y;
  jvxData z;
  int id;
  struct microphone *next;
};
typedef struct {
  int maxId;
  int num;
  struct microphone *first;
} microphones;


// State data (i.e. delay line memory).
typedef struct {
  void *dummy;
} jvx_beampattern_state;

// Parameters, which are derived from configuration but not set directly.
typedef struct  {
  // depending on synced prms
  int frameSize;
  int spectrumSize;

  // not depending on synced prms
  // ... nothing ...
} jvx_beampattern_cfg_derived;


// Prvate data struct for noise estimator.
typedef struct
{
  jvx_beampattern_ram ram;
  jvx_beampattern_state state;
  jvx_beampattern_cfg_derived derived;
  jvx_beampattern_prmSync prmSync;
  jvx_beampattern_prm prm;

  // submodules
  microphones mics;
  jvxHandle *bf_hdl;
  jvx_beampattern_process_cb bf_cb;
  jvxHandle *FFT_hdl;
  jvxHandle *FFT_global_hdl;

} jvx_beampattern_prv;


#endif
