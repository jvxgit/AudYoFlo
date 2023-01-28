#ifndef __JVX_STARTHERE_PRV__H__
#define __JVX_STARTHERE_PRV__H__

#include "jvx_startHere/jvx_startHere.h"

// RAM
typedef struct {
     jvxData *buf;
} jvx_startHere_ram;

// State data (i.e. delay line memory).
typedef struct {
     jvxData *mem;
} jvx_startHere_state;

// Parameters, which are derived from configuration but not set directly.
typedef struct  {
     jvxData gainFactor;
} jvx_startHere_cfg_derived;


// Prvate data struct for noise estimator.
typedef struct
{
     jvx_startHere_ram ram;
     jvx_startHere_state state;
     jvx_startHere_cfg_derived derived;
     jvx_startHere_prmSync prmSync;
     jvx_startHere_prm prm;

     // private submodules
     // ... none ...

} jvx_startHere_prv;


#endif
