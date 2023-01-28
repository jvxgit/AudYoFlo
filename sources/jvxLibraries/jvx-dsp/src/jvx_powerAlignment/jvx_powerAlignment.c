#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "jvx_dsp_base.h"
#include "jvx_powerAlignment/jvx_powerAlignment.h"
#include "jvx_powerAlignment/jvx_powerAlignment_prv.h"
#include "jvx_debugout.h"
#include "jvx_misc/jvx_rangecheck.h"

static void
allocate_derived(jvx_powerAlign_cfg_derived *derived)
{
}

static void
deallocate_derived(jvx_powerAlign_cfg_derived *derived)
{
}

static void
allocate_ram(jvx_powerAlign_ram *ram)
{
}

static void
deallocate_ram(jvx_powerAlign_ram *ram)
{
}

static void
allocate_state(jvx_powerAlign_state *state)
{
  state->memE0 = 0.;
  state->memE1 = 0.;
  state->memG0 = 1.;
  state->memG1 = 1.;
}


static void
deallocate_state(jvx_powerAlign_state *state)
{
}


jvxDspBaseErrorType
jvx_powerAlign_configInit(jvx_powerAlign* hdl,
                          unsigned int pid)
{
  hdl->prv = NULL;

  hdl->id = jvx_id(__func__, pid);
  hdl->pid = pid;
  hdl->samplingRate = 16000;
  hdl->frameSize = 320;
  hdl->nChannelsIn = 2;
  hdl->nChannelsOut = 2;
  hdl->description = JVX_POWERALIGN_DESCRIPTION;
  hdl->version = JVX_POWERALIGN_VERSION;

  // debug struct (static part)
  hdl->readOnly.frameCounter = 0;

  // async parameters
  hdl->prm.mode = jvx_powerAlignment_recursive;
  hdl->prm.ar = 0.95;
  hdl->prm.as = 0.95;

  // sync data
  // ... nothing at the moment ...

  // configInit of submodules
  // ...

  return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType
jvx_powerAlign_init(jvx_powerAlign* hdl,
                    int frameSize,
                    int nChannels,
                    int samplingRate)
{
  jvx_powerAlign_prv* prv = NULL;

  // allocate private part and hook into struct
  JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(prv, jvx_powerAlign_prv);
  hdl->prv = (jvxHandle*) prv;

  // check arguments
  // ...

  // set const configuration from params
  // the cnst struct must not be written anywhere else!
  hdl->frameSize = frameSize;
  hdl->samplingRate = samplingRate;
  hdl->nChannelsIn = hdl->nChannelsOut = nChannels;
  assert(nChannels == 2); // FIXME: more than 2 channels tbd.

  // private data
  allocate_ram(&prv->ram);
  allocate_state(&prv->state);
  allocate_derived(&prv->derived);

  // init of submodules
  // ...

  // trigger parameter update
  jvx_powerAlign_update(hdl, true);

  // print init results
  // ... nothing at the moment ...

  return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_powerAlign_process(jvx_powerAlign* hdl,
                                           jvxData** in,
                                           jvxData** out)
{
  int i;
  jvxData tmp, G0, G1, E0, E1, xfadeIncrement;
  jvx_powerAlign_prv* prv;

  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  prv = (jvx_powerAlign_prv*) hdl->prv;

  // power estimation
  switch(prv->prm.mode)
    {
    case jvx_powerAlignment_recursive:
      // powers are recursively smoothed, but only the E at the end of the frame is effectively used!
      // this allows different adaptation speeds (through E a) while keeping the frame size constant
      E0 = prv->state.memE0;
      E1 = prv->state.memE1;
      for(i = 0; i < hdl->frameSize; i++)
        {
          E0 = prv->prm.ar * E0 + (1. - prv->prm.ar) * in[0][i] * in[0][i];
          E1 = prv->prm.ar * E1 + (1. - prv->prm.ar) * in[1][i] * in[1][i];
        }
      prv->state.memE0 = E0;
      prv->state.memE1 = E1;
      break;

    case jvx_powerAlignment_block:
      // Block based (this is fixed to the given frame size)
      E0 = 0.;
      E1 = 0.;
      for(i = 0; i < hdl->frameSize; i++)
        {
          E0 += in[0][i] * in[0][i];
          E1 += in[1][i] * in[1][i];
        }
      E0 /= hdl->frameSize;
      E1 /= hdl->frameSize;
      break;

    case jvx_powerAlignment_off:
      break;

    default:
      assert(0);
    }

  if(prv->prm.mode != jvx_powerAlignment_off)
    {
      // gain factor smoothing, but only allow gain reduction
      // -> smoothed gain G (<1.) for louder channel, unity for softer channel
      if(E0 > E1)
        {
          G0 = (1. - prv->prm.as) * sqrt(E1 / (E0 + mat_epsMin))
            + prv->prm.as * prv->state.memG0;
          G1 = 1.0;
        }
      else
        {
          G0 = 1.0;
          G1 = (1. - prv->prm.as) * sqrt(E0 / (E1 + mat_epsMin))
            + prv->prm.as * prv->state.memG1;
        }
    }
  else
    {
      G0 = G1 = 1.0;
    }

  // crossfade
  xfadeIncrement = 1.0 / (jvxData)hdl->frameSize;
  tmp = 0;
  for(i = 0; i < hdl->frameSize; i++)
    {
      out[0][i] = in[0][i] * (G0 * tmp + prv->state.memG0 * (1 - tmp));
      out[1][i] = in[1][i] * (G1 * tmp + prv->state.memG1 * (1 - tmp));
      tmp += xfadeIncrement;
    }

  // memory update for gain smoothing
  prv->state.memG0 = G0;
  prv->state.memG1 = G1;

  // readOnly data update
  hdl->readOnly.frameCounter++;

  return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_powerAlign_terminate(jvx_powerAlign* hdl)
{
  jvx_powerAlign_prv* prv;

  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  prv = (jvx_powerAlign_prv*) hdl->prv;

  deallocate_state(&prv->state);
  deallocate_ram(&prv->ram);
  deallocate_derived(&prv->derived);

  // submodules
  // ...

  JVX_DSP_SAFE_DELETE_OBJECT(prv);
  hdl->prv = NULL;

  return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType jvx_powerAlign_update(jvx_powerAlign* hdl, jvxCBool syncUpdate)
{
  jvx_powerAlign_prv* prv;
  jvx_powerAlign_cfg_derived *derived;

  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  prv = (jvx_powerAlign_prv*) hdl->prv;
  derived = &prv->derived;

  // parameter checks (sync & async)
  jvx_rangeCheck_jvxData(&hdl->prm.ar, 0, 1.-1e-16, __func__, hdl->id,  "powerAlignment_ar");
  jvx_rangeCheck_jvxData(&hdl->prm.as, 0, 1.-1e-16, __func__, hdl->id,"powerAlignment_as");

  if(syncUpdate)
    {
      // update synced prms
      prv->prmSync = hdl->prmSync;

      // derived prms depending on synced prms
      // ... nothing at the moment ...

      // update submodules depending on synced prms
      // ... nothing at the moment ...
    }


  // update async prms
  prv->prm = hdl->prm;

  // derived prms not depending on synced prms
  // ... nothing at the moment ...

  // update submodules depending not depending on synced prms
  // ...nothing...

  // update of submodules with dedicated sync/async handling
  // ...

  return JVX_DSP_NO_ERROR;
}
