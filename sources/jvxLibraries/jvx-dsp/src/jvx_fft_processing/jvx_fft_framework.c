#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "jvx_dsp_base.h"
#include "jvx_fft_processing/jvx_fft_framework.h"
#include "jvx_fft_framework_prv.h"

#include "jvx_misc/jvx_rangecheck.h"
#include "jvx_debugout.h"

JVX_FFT_TOOLS_DEFINE_FFT_SIZES

#define SET_FFT_BUFFER_ZERO_BEFORE_TRANSFORM

// Translate macro for older TI compiler
#if __TI_COMPILER_VERSION__ <= 8000000
#define __func__ __FUNCTION__
#endif

static void
allocate_derived(jvx_fftFramework_cfg_derived *derived,
                 jvx_fftFramework *hdl)
{
  int i;
  jvx_fftFramework_prv* prv = (jvx_fftFramework_prv*) hdl->prv;

  derived->fftLength = (jvxSize)pow(2, prv->prmSync.fftLengthLog2);
  derived->spectrumSize = (jvxSize)(derived->fftLength / 2) + 1;
  assert(derived->fftLength > hdl->frameSize);
  derived->irLength = derived->fftLength - hdl->frameSize;
  derived->irDelay = (jvxSize)floor((jvxData)(derived->irLength / 2));
  derived->bufferSize = (int) (hdl->frameSize * JVX_MAX(1.0, prv->prmSync.bufferSizeFactor));

  // init pointers to zero at first
  derived->window = NULL;
  derived->window_CF = NULL;
  derived->ifftIn_oldWeights = NULL;
  derived->ifftIn_currentWeights = NULL;
  derived->winNormalize = 1.0;

  switch(prv->prmSync.frameworkType)
    {
    case JVX_FFT_FRAMEWORK_SW_OLA:
      if(prv->prmSync.nFFT > 0)
        {
          JVX_DSP_SAFE_ALLOCATE_FIELD_Z(derived->window, jvxData, derived->bufferSize);
          jvx_compute_window(derived->window,
                             derived->bufferSize,
                             hdl->frameSize, 0,
                             JVX_WINDOW_FLATTOP_HANN,
                             &derived->winNormalize);
        }
      break;

    case JVX_FFT_FRAMEWORK_DW_OLA:
      JVX_DSP_SAFE_ALLOCATE_FIELD_Z(derived->window, jvxData, derived->bufferSize);
      jvx_compute_window(derived->window,
                         derived->bufferSize,
                         hdl->frameSize, 0,
                         JVX_WINDOW_SQRT_FLATTOP_HANN,
                         &derived->winNormalize);
      break;

    case JVX_FFT_FRAMEWORK_OLA_CF:
    case JVX_FFT_FRAMEWORK_OLS_CF:
      if(prv->prmSync.nIFFT > 0)
        {
          JVX_DSP_SAFE_ALLOCATE_FIELD_Z(derived->window_CF, jvxData, derived->fftLength);
          jvx_compute_window(derived->window_CF,
                             hdl->frameSize, 0, 0,
                             JVX_WINDOW_HALF_HANN_FALLING, &derived->winNormalize);
          JVX_DSP_SAFE_ALLOCATE_FIELD_Z(derived->ifftIn_oldWeights, jvxDataCplx, derived->spectrumSize);
        }
      break;

    case JVX_FFT_FRAMEWORK_OLS:
      break;

  default:
      assert(0);
    }

  derived->winNormalize /= (jvxData)derived->fftLength; // used in both windowed and crossfade frameworks
  if(prv->prmSync.nIFFT > 0)
    {
      JVX_DSP_SAFE_ALLOCATE_FIELD_Z(derived->ifftIn_currentWeights, jvxDataCplx, derived->spectrumSize);
    }


  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(derived->window_AA, jvxData, derived->irLength);
  jvx_compute_window(derived->window_AA, JVX_SIZE_INT32(derived->irLength), 0, 0, JVX_WINDOW_HAMMING, NULL);
  for(i = 0; i < derived->irLength; i++)
    derived->window_AA[i] /= (jvxData)derived->fftLength;
}

static void
deallocate_derived(jvx_fftFramework_cfg_derived *derived,
		   jvx_fftFramework *hdl)
{
  JVX_DSP_SAFE_DELETE_FIELD(derived->window);
  JVX_DSP_SAFE_DELETE_FIELD(derived->window_CF);
  JVX_DSP_SAFE_DELETE_FIELD(derived->window_AA);
  JVX_DSP_SAFE_DELETE_FIELD(derived->ifftIn_currentWeights);
  JVX_DSP_SAFE_DELETE_FIELD(derived->ifftIn_oldWeights);
}

static void
allocate_ram(jvx_fftFramework_ram *ram,
             jvx_fftFramework *hdl)
{
  int i;
  jvx_fftFramework_prv* prv = (jvx_fftFramework_prv*) hdl->prv;

  // init pointers to zero at first
  ram->olaBuf = NULL;
  ram->buf = NULL;

  switch(prv->prmSync.frameworkType)
    {
    case JVX_FFT_FRAMEWORK_DW_OLA:
      if(prv->prmSync.nIFFT > 0)
        {
          JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->olaBuf, jvxData*, prv->prmSync.nIFFT);
          for(i = 0; i < prv->prmSync.nIFFT; i++)
            {
              JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->olaBuf[i], jvxData, prv->derived.bufferSize);
            }
        }
      if(prv->prmSync.nFFT > 0)
        {
          JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->buf, jvxData*, prv->prmSync.nFFT);
          for(i = 0; i < prv->prmSync.nFFT; i++)
            {
              JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->buf[i], jvxData, prv->derived.bufferSize);
            }
        }
        break;
    case JVX_FFT_FRAMEWORK_SW_OLA:
      if(prv->prmSync.nIFFT > 0)
        {
          JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->olaBuf, jvxData*, prv->prmSync.nIFFT);
          for(i = 0; i < prv->prmSync.nFFT; i++)
            {
              JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->olaBuf[i], jvxData, prv->derived.fftLength);
            }
        }
      if(prv->prmSync.nFFT > 0)
        {
          JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->buf, jvxData*, prv->prmSync.nFFT);
          for(i = 0; i < prv->prmSync.nFFT; i++)
            {
              JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->buf[i], jvxData, prv->derived.bufferSize);
            }
        }
      break;
    case JVX_FFT_FRAMEWORK_OLA_CF:
      if(prv->prmSync.nIFFT > 0)
        {
          JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->olaBuf, jvxData*, prv->prmSync.nIFFT);
          for(i = 0; i < prv->prmSync.nFFT; i++)
            {
              JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->olaBuf[i], jvxData, prv->derived.fftLength);
            }
        }
    case JVX_FFT_FRAMEWORK_OLS_CF:
    case JVX_FFT_FRAMEWORK_OLS:
      if(prv->prmSync.nFFT > 0)
        {
          JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->buf, jvxData*, prv->prmSync.nFFT);
          for(i = 0; i < prv->prmSync.nFFT; i++)
            {
              JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->buf[i], jvxData, prv->derived.fftLength);
            }
        }
      break;
    default:
      assert(0);
    }

  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ram->weightsAA, jvxDataCplx, prv->derived.spectrumSize);

  // fftbuffers are set by fft/ifft init functions
}

static void
deallocate_ram(jvx_fftFramework_ram *ram,
               jvx_fftFramework *hdl)
{
  int i;
  jvx_fftFramework_prv* prv = (jvx_fftFramework_prv*) hdl->prv;

  if(ram->buf)
    {
      for(i = 0; i < prv->prmSync.nFFT; i++)
        {
          JVX_DSP_SAFE_DELETE_FIELD(ram->buf[i]);
        }
      JVX_DSP_SAFE_DELETE_FIELD(ram->buf);
    }
  if(ram->olaBuf)
    {
      for(i = 0; i < prv->prmSync.nIFFT; i++)
        {
          JVX_DSP_SAFE_DELETE_FIELD(ram->olaBuf[i]);
        }
      JVX_DSP_SAFE_DELETE_FIELD(ram->olaBuf);
    }


  JVX_DSP_SAFE_DELETE_FIELD(ram->weightsAA);

  // fftbuf is deleted by fft terminate function
}

static void
allocate_state(jvx_fftFramework_state *state,
               jvx_fftFramework *hdl)
{
  int i, j;
  jvx_fftFramework_prv* prv = (jvx_fftFramework_prv*) hdl->prv;

  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(state->idxBufFFT, jvxSize, prv->prmSync.nFFT);
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(state->idxBufIFFT, jvxSize, prv->prmSync.nIFFT);
  state->lastIdxBufIFFT = 0;
  if((prv->prmSync.frameworkType == JVX_FFT_FRAMEWORK_OLS_CF ||
     prv->prmSync.frameworkType == JVX_FFT_FRAMEWORK_OLA_CF) &&
     prv->prmSync.nIFFT > 0)
    {
      JVX_DSP_SAFE_ALLOCATE_FIELD_Z(state->oldWeights, jvxDataCplx*, prv->prmSync.nIFFT);
      for(i = 0; i < prv->prmSync.nIFFT; i++)
        {
          JVX_DSP_SAFE_ALLOCATE_FIELD_Z(state->oldWeights[i], jvxDataCplx, prv->derived.spectrumSize);

          // initialize to unity
          for(j = 0; j < prv->derived.spectrumSize; j++)
            state->oldWeights[i][j].re = 1;
        }
    }
  else
    state->oldWeights = NULL;
}


static void
deallocate_state(jvx_fftFramework_state *state,
                 jvx_fftFramework *hdl)
{
  int i;
  jvx_fftFramework_prv* prv = (jvx_fftFramework_prv*) hdl->prv;

  if(state->oldWeights)
    {
      for(i = 0; i < prv->prmSync.nIFFT; i++)
        {
          JVX_DSP_SAFE_DELETE_FIELD(state->oldWeights[i]);
        }
      JVX_DSP_SAFE_DELETE_FIELD(state->oldWeights);
    }
  JVX_DSP_SAFE_DELETE_FIELD(state->idxBufFFT);
  JVX_DSP_SAFE_DELETE_FIELD(state->idxBufIFFT);
}

jvxDspBaseErrorType
jvx_fftFramework_configInit(jvx_fftFramework* hdl,
                              unsigned int pid)
{
  hdl->prv = NULL;

  // const
  hdl->frameSize = 0;
  hdl->id = jvx_id(__func__, pid);
  hdl->pid = pid;
  hdl->description = JVX_FFTFRAMEWORK_DESCRIPTION;
  hdl->version = JVX_FFTFRAMEWORK_VERSION;
  hdl->samplingRate = hdl->nChannelsIn = hdl->nChannelsOut = -1;

  // readOnly struct (static part)
  hdl->readOnly.frameCounter = 0;

  // async parameters
  // ... nothing at the moment ...

  // sync parameters
  hdl->prmSync.bufferSizeFactor = 2;
  hdl->prmSync.frameworkType = JVX_FFT_FRAMEWORK_SW_OLA;
  hdl->prmSync.nFFT = 1;
  hdl->prmSync.nIFFT = 1;
  hdl->prmSync.fftLengthLog2 = 10;
  hdl->prmSync.applyAliasCompensation = 1;

  // configInit of submodules
  // ... nothing at the moment ...

  return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fftFramework_init(jvx_fftFramework* hdl,
		      int frameSize)
{
  jvx_fftFramework_prv* prv = NULL;


  // allocate private part and hook into struct
  JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(prv, jvx_fftFramework_prv);
  hdl->prv = (jvxHandle*) prv;

  // check arguments
  // ...

  // set const configuration from params
  // the cnst struct must not be written anywhere else!
  hdl->frameSize = frameSize;

  // global allocations
  jvx_create_fft_ifft_global(&prv->globalFFT, JVX_FFT_TOOLS_FFT_SIZE_8192); // global init with max size

  // trigger parameter update & synchronized reallocations
  jvx_fftFramework_update(hdl, true);

  // print init results
  // ... nothing at the moment ...

  return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fftFramework_terminate(jvx_fftFramework* hdl)
{
  jvx_fftFramework_prv* prv;

  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  prv = (jvx_fftFramework_prv*) hdl->prv;

  if(!prv)
    return JVX_DSP_NO_ERROR;

  deallocate_state(&prv->state, hdl);
  deallocate_ram(&prv->ram, hdl);
  deallocate_derived(&prv->derived, hdl);

  // submodules
  jvx_destroy_fft(prv->fftR2C);
  jvx_destroy_ifft(prv->ifftC2R);
  jvx_destroy_ifft(prv->ifftC2C);
  jvx_destroy_fft_ifft_global(prv->globalFFT);

  JVX_DSP_SAFE_DELETE_OBJECT(prv);
  hdl->prv = NULL;

  return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fftFramework_processFFT(jvx_fftFramework* hdl,
                            jvxInt16 fftIdx,
                            jvxData* in,
                            jvxDataCplx* out)
{
  jvx_fftFramework_prv* prv;
  jvxSize ll1, ll2, ll3;
  jvxData *ptrIn;
  jvxData *ptrOut;
  jvxData *ptrWin;
  jvxSize i;

  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  prv = (jvx_fftFramework_prv*) hdl->prv;

  if(fftIdx < 0 || fftIdx >= prv->prmSync.nFFT)
    {
      printf("[%s] invalid FFT index %i\n",__func__,fftIdx);
      return JVX_DSP_ERROR_INVALID_ARGUMENT;
    }


  // prepare FFT input buffer
  switch(prv->prmSync.frameworkType)
    {
    case JVX_FFT_FRAMEWORK_SW_OLA:
    case JVX_FFT_FRAMEWORK_DW_OLA:
      ll1 = JVX_MIN(prv->derived.bufferSize - (jvxInt32)prv->state.idxBufFFT[fftIdx], hdl->frameSize);
      ll2 = hdl->frameSize - ll1;
      ptrIn = in;
      ptrOut = prv->ram.buf[fftIdx] + prv->state.idxBufFFT[fftIdx];
      for(i = 0; i < ll1; i++)
        *ptrOut++ = *ptrIn++;
      ptrOut = prv->ram.buf[fftIdx];
      for(i = 0; i < ll2; i++)
        *ptrOut++ = *ptrIn++;
      prv->state.idxBufFFT[fftIdx] = (prv->state.idxBufFFT[fftIdx] + hdl->frameSize) % prv->derived.bufferSize;
      ll1 = prv->derived.bufferSize - prv->state.idxBufFFT[fftIdx];
      ll2 = prv->derived.bufferSize - ll1;
      ll3 = prv->derived.fftLength - prv->derived.bufferSize;
      ptrIn = prv->ram.buf[fftIdx] + prv->state.idxBufFFT[fftIdx];
      ptrWin = prv->derived.window;
      ptrOut = prv->ram.fftIn;
      for(i = 0; i < ll1; i++)
        *ptrOut++ = *ptrIn++ * *ptrWin++;
      ptrIn = prv->ram.buf[fftIdx];
      for(i = 0; i < ll2; i++)
        *ptrOut++ = *ptrIn++ * *ptrWin++;
#ifdef SET_FFT_BUFFER_ZERO_BEFORE_TRANSFORM
      for(i = 0; i < ll3; i++)
        *ptrOut++ = 0.0;
#endif
      break;

    case JVX_FFT_FRAMEWORK_OLA_CF:
#ifdef SET_FFT_BUFFER_ZERO_BEFORE_TRANSFORM
      memset(prv->ram.fftIn, 0, sizeof(jvxData)*prv->derived.fftLength);
#endif
      memcpy(prv->ram.fftIn, in, sizeof(jvxData)*hdl->frameSize);
      break;

    case JVX_FFT_FRAMEWORK_OLS_CF:
    case JVX_FFT_FRAMEWORK_OLS:
      ll1 = JVX_MIN(prv->derived.fftLength - (jvxInt32)prv->state.idxBufFFT[fftIdx], hdl->frameSize);
      ll2 = hdl->frameSize - ll1;
      ptrIn = in;
      ptrOut = prv->ram.buf[fftIdx] + prv->state.idxBufFFT[fftIdx];
      for(i = 0; i < ll1; i++)
        *ptrOut++ = *ptrIn++;
      ptrOut = prv->ram.buf[fftIdx];
      for(i = 0; i < ll2; i++)
        *ptrOut++ = *ptrIn++;
      prv->state.idxBufFFT[fftIdx] = (prv->state.idxBufFFT[fftIdx] + hdl->frameSize) % prv->derived.fftLength;
      memcpy(prv->ram.fftIn, prv->ram.buf[fftIdx], sizeof(jvxData)*prv->derived.fftLength);
      break;

    default:
      assert(0);
    }

  // FFT
  jvx_execute_fft(prv->fftR2C);

  // copy spectrum
  if(out)
    for(i = 0; i < prv->derived.spectrumSize; i++)
      out[i] = prv->ram.fftOut[i];

  // readOnly data update
  hdl->readOnly.frameCounter++;

  return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fftFramework_processFFT_ZP(jvx_fftFramework* hdl,
                               jvxData* in,
                               jvxDataCplx* out)
{
  jvx_fftFramework_prv* prv;
  jvxSize i;
  jvxData* ptrOut;
  jvxData* ptrIn;
  jvxSize ll1;
  jvxSize ll2;
  jvxSize ofs;

  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  prv = (jvx_fftFramework_prv*) hdl->prv;

  memset(prv->ram.fftIn, 0, sizeof(jvxData)*prv->derived.fftLength);

  ofs = (prv->derived.fftLength + prv->state.lastIdxBufIFFT - prv->derived.irLength/2 + 1) % prv->derived.fftLength;
  ptrOut = prv->ram.fftIn + ofs;
  ptrIn = in;
  ll1 = JVX_MIN(prv->derived.fftLength - ofs, hdl->frameSize);
  ll2 = hdl->frameSize - ll1;
  for(i = 0; i < ll1; i++)
    *ptrOut++ = *ptrIn++;
  ptrOut = prv->ram.fftIn;
  for(i = 0; i < ll2; i++)
    *ptrOut++ = *ptrIn++;

  jvx_execute_fft(prv->fftR2C);
  for(i = 0; i < prv->derived.spectrumSize; i++)
    out[i] = prv->ram.fftOut[i];

  return JVX_DSP_NO_ERROR;
}

static void
applyWeights(const jvxDataCplx *x,
             jvxDataCplx *y,
             jvxDataCplx *w,
             int n)
{
  int i;
  jvxDataCplx tmp;

  for(i = 0; i < n; i++)
    {
      tmp = x[i]; // if x and y are identical
      y[i].re = tmp.re * w[i].re - tmp.im * w[i].im;
      y[i].im = tmp.im * w[i].re + tmp.re * w[i].im;
    }
}

static void
processIFFT_sw_ola(jvx_fftFramework* hdl,
                   jvxInt16 ifftIdx,
                   jvxDataCplx* in,
                   jvxData* out)
{
  jvx_fftFramework_prv* prv;
  jvxData* ptrOutOut;
  jvxData* ptrOut;
  jvxData* ptrIn;
  jvxSize limll;
  jvxSize ll1;
  jvxSize ll1_0;
  jvxSize ll1_1;
  jvxSize ll2;
  jvxSize ll2_0;
  jvxSize ll2_1;
  jvxSize i;
  jvxData accu;

  prv = (jvx_fftFramework_prv*) hdl->prv;

  for(i = 0; i < prv->derived.spectrumSize; i++)
    prv->ram.ifftIn[i] = in[i];

  jvx_execute_ifft(prv->ifftC2R);

  ptrOutOut = out;
  ptrOut = prv->ram.olaBuf[ifftIdx] + prv->state.idxBufIFFT[ifftIdx];
  ptrIn = prv->ram.ifftOut;

  limll = hdl->frameSize;
  ll1 = prv->derived.fftLength - prv->state.idxBufIFFT[ifftIdx];
  ll1_0 = JVX_MIN(ll1, limll);
  ll1_1 = ll1 - ll1_0;
  limll -= ll1_0;
  ll2 = prv->derived.fftLength - ll1;
  ll2_0 = JVX_MIN(ll2, limll);
  ll2_1 = ll2 - ll2_0;

  // Output the returned values, set the ola buffer to zero at this place
  for(i = 0; i < ll1_0; i++)
    {
      accu = *ptrIn;
      accu *= prv->derived.winNormalize;
      *ptrOutOut = accu + (*ptrOut);
      *ptrOut = 0.0;
      ptrOut++;
      ptrIn++;
      ptrOutOut++;
    }
  for(i = 0; i < ll1_1; i++)
    {
      accu = *ptrIn;
      accu *= prv->derived.winNormalize;
      *ptrOut += accu;
      ptrOut++;
      ptrIn++;
    }

  ptrOut = prv->ram.olaBuf[ifftIdx];

  // Output the returned values, set the ola buffer to zero at this place
  for(i = 0; i < ll2_0; i++)
    {
      accu = *ptrIn;
      accu *= prv->derived.winNormalize;
      *ptrOutOut = accu + *ptrOut;
      *ptrOut = 0.0;
      ptrOut++;
      ptrIn++;
      ptrOutOut++;
    }
  for(i = 0; i < ll2_1; i++)
    {
      accu = *ptrIn;
      accu *= prv->derived.winNormalize;
      *ptrOut += accu;
      ptrOut++;
      ptrIn++;
    }
  prv->state.idxBufIFFT[ifftIdx] = (prv->state.idxBufIFFT[ifftIdx] + hdl->frameSize) % prv->derived.fftLength;
}

static void
processIFFT_dw_ola(jvx_fftFramework* hdl,
                   jvxInt16 ifftIdx,
                   jvxDataCplx* in,
                   jvxData* out)
{
  jvx_fftFramework_prv* prv;

  jvxData* ptrOutOut;
  jvxData* ptrOut;
  jvxData* ptrIn;
  jvxData* ptrWin;

  jvxSize limll;
  jvxSize ll1;
  jvxSize ll1_0;
  jvxSize ll1_1;
  jvxSize ll2;
  jvxSize ll2_0;
  jvxSize ll2_1;
  jvxSize i;
  jvxData accu;

  prv = (jvx_fftFramework_prv*) hdl->prv;

  for(i = 0; i < prv->derived.spectrumSize; i++)
    prv->ram.ifftIn[i] = in[i];

  jvx_execute_ifft(prv->ifftC2R);

  ptrOutOut = out;
  ptrOut = prv->ram.olaBuf[ifftIdx] + prv->state.idxBufIFFT[ifftIdx];
  ptrIn = prv->ram.ifftOut;
  ptrWin = prv->derived.window;

  limll = hdl->frameSize;
  ll1 = prv->derived.bufferSize - prv->state.idxBufIFFT[ifftIdx];
  ll1_0 = JVX_MIN(ll1, limll);
  ll1_1 = ll1 - ll1_0;
  limll -= ll1_0;
  ll2 = prv->derived.bufferSize - ll1;
  ll2_0 = JVX_MIN(ll2, limll);
  ll2_1 = ll2 - ll2_0;

  // Output the returned values, set the ola buffer to zero at this place
  for(i = 0; i < ll1_0; i++)
    {
      accu = *ptrIn;
      accu *= prv->derived.winNormalize;
      *ptrOutOut = (accu * *ptrWin)+ (*ptrOut);
      *ptrOut = 0.0;
      ptrOut++;
      ptrIn++;
      ptrOutOut++;
      ptrWin++;
    }
  for(i = 0; i < ll1_1; i++)
    {
      accu = *ptrIn;
      accu *= prv->derived.winNormalize;
      *ptrOut += (accu * *ptrWin);
      ptrOut++;
      ptrIn++;
      ptrWin++;
    }

  ptrOut = prv->ram.olaBuf[ifftIdx];

  // Output the returned values, set the ola buffer to zero at this place
  for(i = 0; i < ll2_0; i++)
    {
      accu = *ptrIn;
      accu *= prv->derived.winNormalize;
      *ptrOutOut = (accu * *ptrWin) + *ptrOut;
      *ptrOut = 0.0;
      ptrOut++;
      ptrIn++;
      ptrOutOut++;
      ptrWin++;
    }
  for(i = 0; i < ll2_1; i++)
    {
      accu = *ptrIn;
      accu *= prv->derived.winNormalize;
      *ptrOut += (accu * *ptrWin);
      ptrOut++;
      ptrIn++;
      ptrWin++;
    }
  prv->state.idxBufIFFT[ifftIdx] = (prv->state.idxBufIFFT[ifftIdx] + hdl->frameSize) % prv->derived.bufferSize;
}

static void
processIFFT_ola_cf(jvx_fftFramework* hdl,
                   jvxInt16 ifftIdx,
                   jvxDataCplx* inNew,
                   jvxDataCplx* inOld,
                   jvxData* out)
{
  jvx_fftFramework_prv* prv;

  jvxDataCplx* ptrTarget_firstHalf;
  jvxDataCplx* ptrTarget_secondHalf;
  jvxDataCplx* ptrSource_old;
  jvxDataCplx* ptrSource_new;

  jvxData* ptrOutOut;
  jvxDataCplx* ptrIn;
  jvxData* ptrWin_start;
  jvxData* ptrOla;

  jvxSize limll;
  jvxSize ll1;
  jvxSize ll1_0;
  jvxSize ll1_1;
  jvxSize ll2;
  jvxSize ll2_0;
  jvxSize ll2_1;
  jvxData accuWin;
  jvxSize i;
  jvxData accu;

  prv = (jvx_fftFramework_prv*) hdl->prv;

  ptrTarget_firstHalf = prv->ram.ifftInC2C;
  ptrTarget_secondHalf = prv->ram.ifftInC2C + prv->derived.fftLength;
  ptrSource_old = inOld;
  ptrSource_new = inNew;

  // prepare joint ifft of both versions
  ptrTarget_firstHalf->re = ptrSource_old->re - ptrSource_new->im;
  ptrTarget_firstHalf->im = ptrSource_old->im + ptrSource_new->re;
  ptrTarget_firstHalf ++;
  ptrSource_old++;
  ptrSource_new++;
  for(i = 0; i < prv->derived.spectrumSize-2; i++)
    {
      ptrTarget_firstHalf->re = ptrSource_old->re - ptrSource_new->im;
      ptrTarget_firstHalf->im = ptrSource_old->im + ptrSource_new->re;
      ptrTarget_firstHalf ++;
      --ptrTarget_secondHalf;
      ptrTarget_secondHalf->re = ptrSource_old->re + ptrSource_new->im;
      ptrTarget_secondHalf->im = - ptrSource_old->im + ptrSource_new->re;
      ptrSource_new++;
      ptrSource_old++;
    }
  ptrTarget_firstHalf->re = ptrSource_old->re - ptrSource_new->im;
  ptrTarget_firstHalf->im = ptrSource_old->im + ptrSource_new->re;

  // Transform signal
  jvx_execute_ifft(prv->ifftC2C);

  ptrOutOut = out;
  ptrIn = prv->ram.ifftOutC2C;
  ptrWin_start = prv->derived.window_CF;
  ptrOla = prv->ram.olaBuf[ifftIdx] + prv->state.idxBufIFFT[ifftIdx];

  limll = hdl->frameSize;
  ll1 = prv->derived.fftLength - prv->state.idxBufIFFT[ifftIdx];
  ll1_0 = JVX_MIN(ll1, limll);
  ll1_1 = ll1 - ll1_0;
  limll -= ll1_0;
  ll2 = prv->derived.fftLength - ll1;
  ll2_0 = JVX_MIN(ll2, limll);
  ll2_1 = ll2 - ll2_0;
  accuWin = 0.0;

  for(i = 0; i < ll1_0; i++)
    {
      accuWin = *ptrWin_start;
      accu = ptrIn->re * accuWin + ptrIn->im * (1-accuWin);// <- real part is from "old" weights
      accu *= prv->derived.winNormalize;
      *ptrOutOut++ = *ptrOla + accu;
      *ptrOla = 0.0;
      ptrIn++;
      ptrWin_start++;
      ptrOla++;
    }
  for(i = 0; i < ll1_1; i++)
    {
      accuWin = *ptrWin_start;
      accu = ptrIn->re * accuWin + ptrIn->im * (1-accuWin);// <- real part is from "old" weights
      accu *= prv->derived.winNormalize;
      *ptrOla = *ptrOla + accu;
      ptrIn++;
      ptrWin_start++;
      ptrOla++;
    }

  ptrOla = prv->ram.olaBuf[ifftIdx];

  for(i = 0; i < ll2_0; i++)
    {
      accuWin = *ptrWin_start;
      accu = ptrIn->re * accuWin + ptrIn->im * (1-accuWin);// <- real part is from "old" weights
      accu *= prv->derived.winNormalize;
      *ptrOutOut++ = *ptrOla + accu;
      *ptrOla = 0.0;
      ptrIn++;
      ptrWin_start++;
      ptrOla++;
    }
  for(i = 0; i < ll2_1; i++)
    {
      accuWin = *ptrWin_start;
      accu = ptrIn->re * accuWin + ptrIn->im * (1-accuWin);// <- real part is from "old" weights
      accu *= prv->derived.winNormalize;
      *ptrOla = *ptrOla + accu;
      ptrIn++;
      ptrWin_start++;
      ptrOla++;
    }
  prv->state.idxBufIFFT[ifftIdx] = (prv->state.idxBufIFFT[ifftIdx] + hdl->frameSize) % prv->derived.fftLength;
}

static void
processIFFT_ols_cf(jvx_fftFramework* hdl,
                   jvxInt16 ifftIdx,
                   jvxDataCplx* inNew,
                   jvxDataCplx* inOld,
                   jvxData* out)
{
  jvx_fftFramework_prv* prv;

  jvxDataCplx* ptrTarget_firstHalf;
  jvxDataCplx* ptrTarget_secondHalf;
  jvxDataCplx* ptrSource_old;
  jvxDataCplx* ptrSource_new;

  jvxData* ptrOutOut;
  jvxDataCplx* ptrIn;
  jvxData* ptrWin_start;
  jvxData* ptrWin_end;

  jvxSize ll1;
  jvxSize ll2;
  jvxSize i;
  jvxData accu;

  prv = (jvx_fftFramework_prv*) hdl->prv;

  ptrTarget_firstHalf = prv->ram.ifftInC2C;
  ptrTarget_secondHalf = prv->ram.ifftInC2C + prv->derived.fftLength;
  ptrSource_old = inOld;
  ptrSource_new = inNew;

  // prepare joint ifft of both versions
  ptrTarget_firstHalf->re = ptrSource_old->re - ptrSource_new->im;
  ptrTarget_firstHalf->im = ptrSource_old->im + ptrSource_new->re;
  ptrTarget_firstHalf ++;
  ptrSource_old++;
  ptrSource_new++;
  for(i = 0; i < prv->derived.spectrumSize-2; i++)
    {
      ptrTarget_firstHalf->re = ptrSource_old->re - ptrSource_new->im;
      ptrTarget_firstHalf->im = ptrSource_old->im + ptrSource_new->re;
      ptrTarget_firstHalf ++;
      --ptrTarget_secondHalf;
      ptrTarget_secondHalf->re = ptrSource_old->re + ptrSource_new->im;
      ptrTarget_secondHalf->im = - ptrSource_old->im + ptrSource_new->re;
      ptrSource_new++;
      ptrSource_old++;
    }
  ptrTarget_firstHalf->re = ptrSource_old->re - ptrSource_new->im;
  ptrTarget_firstHalf->im = ptrSource_old->im + ptrSource_new->re;

  // Transform signal
  jvx_execute_ifft(prv->ifftC2C);

  ptrOutOut = out;
  ptrIn = prv->ram.ifftOutC2C + prv->state.idxBufIFFT[ifftIdx];
  ptrWin_start = prv->derived.window_CF;
  ptrWin_end = prv->derived.window_CF + hdl->frameSize;

  ll1 = JVX_MIN(prv->derived.fftLength - prv->state.idxBufIFFT[ifftIdx], hdl->frameSize);
  ll2 = hdl->frameSize - ll1;

  for(i = 0; i < ll1; i++)
    {
      --ptrWin_end;
      accu = ptrIn->re * *ptrWin_start + ptrIn->im * *ptrWin_end;// <- real part is from "old" weights
      accu *= prv->derived.winNormalize;
      *ptrOutOut++ = accu;
      ptrIn++;
      ptrWin_start++;
    }
  ptrIn = prv->ram.ifftOutC2C;
  for(i = 0; i < ll2; i++)
    {
      --ptrWin_end;
      accu = ptrIn->re * *ptrWin_start + ptrIn->im * *ptrWin_end;
      accu *= prv->derived.winNormalize;
      *ptrOutOut++ = accu;
      ptrIn++;
      ptrWin_start++;
    }
  prv->state.idxBufIFFT[ifftIdx] = (prv->state.idxBufIFFT[ifftIdx] + hdl->frameSize) % prv->derived.fftLength;
}


static void
processIFFT_ols(jvx_fftFramework* hdl,
                jvxInt16 ifftIdx,
                jvxDataCplx* in,
                jvxData* out)
{
  jvx_fftFramework_prv* prv;

  jvxData* ptrOutOut;
  jvxData* ptrIn;

  jvxSize ll1;
  jvxSize ll2;
  jvxSize i;

  prv = (jvx_fftFramework_prv*) hdl->prv;

  for(i = 0; i < prv->derived.spectrumSize; i++)
    prv->ram.ifftIn[i] = in[i];

  jvx_execute_ifft(prv->ifftC2R);

  ptrOutOut = out;
  ptrIn = prv->ram.ifftOut + prv->state.idxBufIFFT[ifftIdx];

  ll1 = JVX_MIN(prv->derived.fftLength - prv->state.idxBufIFFT[ifftIdx], hdl->frameSize);
  ll2 = hdl->frameSize - ll1;

  for(i = 0; i < ll1; i++)
    *ptrOutOut++ = *ptrIn++ * prv->derived.winNormalize;
  ptrIn = prv->ram.ifftOut;
  for(i = 0; i < ll2; i++)
    *ptrOutOut++ = *ptrIn++  * prv->derived.winNormalize;

  prv->state.lastIdxBufIFFT = prv->state.idxBufIFFT[ifftIdx];
  prv->state.idxBufIFFT[ifftIdx] = (prv->state.idxBufIFFT[ifftIdx] + hdl->frameSize) % prv->derived.fftLength;

}


static void
jvx_fftFramework_processAntiAlias(jvx_fftFramework* hdl,
                                  jvxDataCplx* in,
                                  jvxDataCplx* out,
                                  jvxInt16 ifftIdx,
                                  jvxData* aliasError_dB)
{
  jvx_fftFramework_prv* prv;
  jvxSize i;
  jvxData *pIn;
  jvxData *pWin;
  jvxData *pOut;
  jvxData E1 = 0.;
  jvxData E2 = 0.;
  static jvxData maxLoss = 0;

  prv = (jvx_fftFramework_prv*) hdl->prv;

  memcpy(prv->ram.ifftIn, in, sizeof(jvxDataCplx)*prv->derived.spectrumSize);
  jvx_execute_ifft(prv->ifftC2R);
  /* if(hdl->pid==3 && ifftIdx==1) */
  /*   { */
  /*     DATAOUT(prv->ram.ifftOut,prv->derived.fftLength,"ir"); */
  /*   } */

  pIn = prv->ram.ifftOut + prv->derived.fftLength - (jvxSize)floor((jvxData)prv->derived.irLength/2.0);
  pWin = prv->derived.window_AA;
  pOut = prv->ram.fftIn;
  for(i = 0; i < (jvxSize)floor(prv->derived.irLength/2.0); i++)
    *pOut++ = *pIn++ * *pWin++;
  pIn = prv->ram.ifftOut;
  for(i = 0; i < (jvxSize)ceil(prv->derived.irLength/2.0); i++)
    *pOut++ = *pIn++ * *pWin++;
  for(i = 0; i < prv->derived.fftLength - prv->derived.irLength; i++)
    *pOut++ =0;
  /* if(hdl->pid==3 && ifftIdx==1) */
  /*   { */
  /*     DATAOUT(prv->ram.fftIn,prv->derived.fftLength,"iraa"); */
  /*   } */

  if(aliasError_dB)
    {
      for(i = 0; i < prv->derived.fftLength; i++)
        {
          E1 += prv->ram.ifftOut[i] * prv->ram.ifftOut[i];
          E2 += prv->ram.fftIn[i] * prv->ram.fftIn[i] * (jvxData)prv->derived.fftLength * (jvxData)prv->derived.fftLength;
        }
      *aliasError_dB = 10*log10(E1/E2);
    }

  jvx_execute_fft(prv->fftR2C);
  memcpy(out, prv->ram.fftOut, sizeof(jvxDataCplx)*prv->derived.spectrumSize);
}

jvxDspBaseErrorType
jvx_fftFramework_processIFFT(jvx_fftFramework* hdl,
                             jvxInt16 ifftIdx,
                             const jvxDataCplx* in,
                             jvxDataCplx* weights, // can be NULL (but cf is not effective then)
                             jvxData* out,
                             jvxData* aliasError_dB)
{
  jvx_fftFramework_prv* prv;
  jvxSize i;
  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  prv = (jvx_fftFramework_prv*) hdl->prv;

  if(ifftIdx < 0 || ifftIdx >= prv->prmSync.nIFFT)
    {
      printf("[%s] invalid IFFT index %i\n",__func__,ifftIdx);
      return JVX_DSP_ERROR_INVALID_ARGUMENT;
    }

  if(weights)
    {
      if(prv->prmSync.applyAliasCompensation)
        jvx_fftFramework_processAntiAlias(hdl, weights, prv->ram.weightsAA, ifftIdx, aliasError_dB);

      // apply current weights
      applyWeights(in, prv->derived.ifftIn_currentWeights,
                   prv->ram.weightsAA, JVX_SIZE_INT(prv->derived.spectrumSize));

      // apply old weights
      if(prv->derived.ifftIn_oldWeights)
        applyWeights(in, prv->derived.ifftIn_oldWeights,
                     prv->state.oldWeights[ifftIdx], JVX_SIZE_INT(prv->derived.spectrumSize));
    }
  else
    {
      memcpy(prv->derived.ifftIn_currentWeights, in, sizeof(jvxDataCplx) * prv->derived.spectrumSize);
      memcpy(prv->derived.ifftIn_oldWeights, in, sizeof(jvxDataCplx) * prv->derived.spectrumSize);
      if(aliasError_dB)
        *aliasError_dB = 0;
    }

  // processing
  switch(prv->prmSync.frameworkType)
    {
    case JVX_FFT_FRAMEWORK_SW_OLA:
      processIFFT_sw_ola(hdl, ifftIdx, prv->derived.ifftIn_currentWeights, out);
      break;
    case JVX_FFT_FRAMEWORK_DW_OLA:
      processIFFT_dw_ola(hdl, ifftIdx, prv->derived.ifftIn_currentWeights, out);
      break;
    case JVX_FFT_FRAMEWORK_OLA_CF:
      processIFFT_ola_cf(hdl, ifftIdx, prv->derived.ifftIn_currentWeights,
                         prv->derived.ifftIn_oldWeights, out);
      break;
    case JVX_FFT_FRAMEWORK_OLS_CF:
      processIFFT_ols_cf(hdl, ifftIdx, prv->derived.ifftIn_currentWeights,
                         prv->derived.ifftIn_oldWeights, out);
      break;
    case JVX_FFT_FRAMEWORK_OLS:
      processIFFT_ols(hdl, ifftIdx, prv->derived.ifftIn_currentWeights, out);
      break;

    default:
      assert(0);
    }

  // memory update
  if(weights && prv->state.oldWeights)
    {
      for(i = 0; i < prv->derived.spectrumSize; i++)
        prv->state.oldWeights[ifftIdx][i] = weights[i];
    }

  return JVX_DSP_NO_ERROR;
}



jvxDspBaseErrorType
jvx_fftFramework_update(jvx_fftFramework* hdl,
                        jvxCBool syncUpdate)
{
  jvx_fftFramework_prv* prv;
  jvx_fftFramework_cfg_derived *derived;
  jvxFFTSize szFft = JVX_FFT_TOOLS_FFT_SIZE_1024;
  jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  prv = (jvx_fftFramework_prv*) hdl->prv;
  derived = &prv->derived;

  // parameter checks (sync & async)
  jvx_rangeCheck_int16(&hdl->prmSync.fftLengthLog2, (jvxInt16)ceil(log(hdl->frameSize+1)/log(2.0)),
                       13, __func__, hdl->id,  "log 2 of FFT length");
  jvx_rangeCheck_int16(&hdl->prmSync.nFFT, 0, 10, __func__, hdl->id,  "nFFT");
  jvx_rangeCheck_int16(&hdl->prmSync.nIFFT, 0, 10, __func__,  hdl->id, "nIFFT");
  jvx_rangeCheck_int16(&hdl->prmSync.frameworkType, 0, JVX_FFT_FRAMEWORK_NUMTYPES-1, __func__,  hdl->id, "FFT framework type");

  // update async prms
  prv->prm = hdl->prm;

  if(syncUpdate)
    {
      // update synced prms
      prv->prmSync = hdl->prmSync;

      // private data
      deallocate_derived(&prv->derived, hdl);
      allocate_derived(&prv->derived, hdl);
      deallocate_ram(&prv->ram, hdl);
      allocate_ram(&prv->ram, hdl);
      deallocate_state(&prv->state, hdl);
      allocate_state(&prv->state, hdl);

	  szFft = JVX_FFT_TOOLS_FFT_SIZE_1024;
	  res = jvx_get_nearest_size_fft(&szFft, prv->derived.fftLength, JVX_FFT_ROUND_UP, NULL);
	  assert(res == JVX_DSP_NO_ERROR);

      // reinit of submodules (no update function available for these)
      jvx_destroy_fft(prv->fftR2C);
	  jvx_create_fft_real_2_complex(&prv->fftR2C,
		  prv->globalFFT,
		  szFft,
		  &prv->ram.fftIn,
		  &prv->ram.fftOut,
		  NULL,
		  JVX_FFT_IFFT_PRESERVE_INPUT,
		  NULL,
		  NULL,
		  0);
	  jvx_destroy_ifft(prv->ifftC2C);

	  jvxFFTSize szFft;
	  jvxDspBaseErrorType res = jvx_get_nearest_size_fft(&szFft, prv->derived.fftLength, JVX_FFT_ROUND_UP, NULL);
	  assert(res == JVX_DSP_NO_ERROR);

      jvx_create_ifft_complex_2_complex(&prv->ifftC2C, prv->globalFFT,
                                        szFft,
                                        &prv->ram.ifftInC2C,
                                        &prv->ram.ifftOutC2C,
                                        NULL,
                                        JVX_FFT_IFFT_PRESERVE_INPUT,
                                        NULL,
                                        NULL,
                                        0);
      jvx_destroy_ifft(prv->ifftC2R);
      jvx_create_ifft_complex_2_real(&prv->ifftC2R, prv->globalFFT,
                                     szFft,
                                     &prv->ram.ifftIn,
                                     &prv->ram.ifftOut,
                                     NULL,
                                     JVX_FFT_IFFT_PRESERVE_INPUT,
                                     NULL,
                                     NULL,
                                     0);

      // update readOnly data depending on synced prms
      switch(prv->prmSync.frameworkType)
        {
        case JVX_FFT_FRAMEWORK_SW_OLA:
        case JVX_FFT_FRAMEWORK_DW_OLA:
          hdl->readOnly.delay = JVX_SIZE_INT16(prv->derived.bufferSize - hdl->frameSize + prv->derived.irDelay);
          break;
        case JVX_FFT_FRAMEWORK_OLA_CF:
        case JVX_FFT_FRAMEWORK_OLS_CF:
        case JVX_FFT_FRAMEWORK_OLS:
          hdl->readOnly.delay = JVX_SIZE_INT16(prv->derived.irDelay);
          break;
        default:
          assert(0);
        }
      hdl->readOnly.spectrumSize = prv->derived.spectrumSize;
      hdl->readOnly.fftLength = prv->derived.fftLength;

    }


  // derived prms not depending on synced prms
  // ... nothing at the moment ...

  // update submodules depending not depending on synced prms
  // ...nothing...

  // update of submodules with dedicated sync/async handling
  //

  return JVX_DSP_NO_ERROR;
}
