#ifndef __JVX_POWERALIGNMENT_PRV__H__
#define __JVX_POWERALIGNMENT_PRV__H__

#include "jvx_powerAlignment/jvx_powerAlignment.h"

// RAM
typedef struct {
     int dummy;
} jvx_powerAlign_ram;

// State data (i.e. delay line memory).
typedef struct {
     jvxData memE0;
     jvxData memE1;
     jvxData memG0;
     jvxData memG1;
} jvx_powerAlign_state;

// Parameters, which are derived from configuration but not set directly.
typedef struct  {
     // depending on synced prms
     // ... nothing at the moment ...

     // not depending on synced prms
     // ... nothing at the moment ...

     int dummy;
} jvx_powerAlign_cfg_derived;


// Prvate data struct for noise estimator.
typedef struct
{
     jvx_powerAlign_ram ram;
     jvx_powerAlign_state state;
     jvx_powerAlign_cfg_derived derived;
     jvx_powerAlign_prmSync prmSync;
     jvx_powerAlign_prm prm;

     // private submodules
     // ...

} jvx_powerAlign_prv;


#endif
