#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "jvx_startHere/jvx_startHere.h"
#include "jvx_startHere_prv.h"
#include "jvx_dsp_base.h"
#include "jvx_debugout.h"
#include "jvx_misc/jvx_rangecheck.h"

// octave-generated parameter tables
#ifdef JVX_DSP_USE_DEFAULT_OCTCONF
#include "jvx_startHere_defaultOctConfig.h" // pre-generated tables
#else
#include "jvx_startHere_octConfig.h" // build-time generation (octave required)
#endif

static void
allocate_derived(jvx_startHere_cfg_derived *derived,
                 jvx_startHere *hdl)
{
}

static void
deallocate_derived(jvx_startHere_cfg_derived *derived,
		   jvx_startHere *hdl)
{
}

static void
allocate_ram(jvx_startHere_ram *ram,
             jvx_startHere *hdl)
{
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->buf, jvxData, hdl->frameSize);
}

static void
deallocate_ram(jvx_startHere_ram *ram,
	       jvx_startHere *hdl)
{
  JVX_DSP_SAFE_DELETE_FIELD(ram->buf);
}

static void
allocate_state(jvx_startHere_state *state,
	       jvx_startHere *hdl)
{
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(state->mem, jvxData, hdl->frameSize);
}


static void
deallocate_state(jvx_startHere_state *state,
		 jvx_startHere *hdl)
{
  JVX_DSP_SAFE_DELETE_FIELD(state->mem);
}


jvxDspBaseErrorType
jvx_startHere_configInit(jvx_startHere* hdl,
                         unsigned int pid)
{
  hdl->id = jvx_id(__func__, pid); 
  hdl->pid = pid;
  hdl->description = JVX_STARTHERE_DESCRIPTION;
  hdl->version = JVX_STARTHERE_VERSION;
  hdl->samplingRate = 48000;
  hdl->frameSize = 256;
  hdl->nChannelsIn = 2;
  hdl->nChannelsOut = 2;

  hdl->prv = NULL;

  // readOnly struct (static part)
  hdl->readOnly.frameCounter = 0;

  // async parameters
  hdl->prm.gain_dB = 0;
  hdl->prm.flip = 0;

  // sync parameters
  // ...

  // config init of submodules
  // ...

  return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType
jvx_startHere_init(jvx_startHere* hdl,
                  int frameSize,
                  int nChannelsIn,
                  int nChannelsOut,
                  int samplingRate)
{
  jvx_startHere_prv* prv = NULL;


  // allocate private part and hook into struct
  JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(prv, jvx_startHere_prv);
  hdl->prv = (jvxHandle*) prv;

  // check arguments
  if(nChannelsIn != nChannelsOut) // only n-to-n mapping for this demo algorithm
    {
      printf("[%s] ERROR: invalid input arg\n", __func__);
      return JVX_DSP_ERROR_INVALID_ARGUMENT;
    }

  // set const configuration from params
  hdl->frameSize = frameSize;
  hdl->nChannelsIn = nChannelsIn;
  hdl->nChannelsIn = nChannelsOut;
  hdl->samplingRate = samplingRate;

  // submodule init
  // ...

  // trigger parameter update & synchronized reallocations
  jvx_startHere_update(hdl, true);

  // print init results
  // ...

  return JVX_DSP_NO_ERROR;
}

// local helper functions must be static
static jvxData
applyGain(jvxData x, jvxData g)
{
  return x * g;
}

jvxDspBaseErrorType
jvx_startHere_process(jvx_startHere* hdl,
                      jvxData** in,
                      jvxData** out)
{
  int chFrom, chTo, i;
  jvx_startHere_prv* prv;

  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  prv = (jvx_startHere_prv*) hdl->prv;

  // core processing
  for(chFrom = 0; chFrom < hdl->nChannelsIn; chFrom++)
    {
      if(prv->prm.flip)
        {
          chTo = hdl->nChannelsOut - chFrom - 1;
        }
      else
        {
          chTo = chFrom;
        }
      for(i = 0; i < hdl->frameSize; i++)
        {
          out[chTo][i] = applyGain(in[chFrom][i], prv->derived.gainFactor);
        }
    }

  // readOnly data update
  hdl->readOnly.frameCounter++;

  return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_startHere_terminate(jvx_startHere* hdl)
{
  jvx_startHere_prv* prv;

  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  prv = (jvx_startHere_prv*) hdl->prv;

  if(!prv)
    return JVX_DSP_NO_ERROR;

  deallocate_state(&prv->state, hdl);
  deallocate_ram(&prv->ram, hdl);
  deallocate_derived(&prv->derived, hdl);

  // terminate submodules
  // ...

  JVX_DSP_SAFE_DELETE_OBJECT(prv);
  hdl->prv = NULL;

  return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType
jvx_startHere_update(jvx_startHere* hdl, jvxCBool syncUpdate)
{
  jvx_startHere_prv* prv;
  jvx_startHere_cfg_derived *derived;

  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  prv = (jvx_startHere_prv*) hdl->prv;
  derived = &prv->derived;

  // parameter checks (sync & async)
  jvx_rangeCheck_jvxData(&hdl->prm.gain_dB, -20, 20, __func__,  hdl->id, "gain in dB");

  // update async prms
  prv->prm = hdl->prm;

  // update derived prms not depending on synced prms
  derived->gainFactor = pow(10., prv->prm.gain_dB / 20.);

  // reinit submodules without dedicated async handling
  // ...

  if(syncUpdate)
    {
      // update synced prms
      prv->prmSync = hdl->prmSync;

      // reallocate private data
      deallocate_derived(&prv->derived, hdl);
      allocate_derived(&prv->derived, hdl);
      deallocate_ram(&prv->ram, hdl);
      allocate_ram(&prv->ram, hdl);
      deallocate_state(&prv->state, hdl);
      allocate_state(&prv->state, hdl);

      // reinit submodules without dedicated sync handling
      // ...
    }

  // update of submodules with dedicated sync/async update function
  // ...

  return JVX_DSP_NO_ERROR;
}
