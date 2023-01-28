#include "jvx_dsp_base.h"
#include "jvx_resampler/jvx_adaptive_resampler.h"
#include "jvx_resampler/jvx_adaptive_resampler_config.h"
#include "jvx_math/jvx_math_extensions.h"
#include "jvx_windows/jvx_windows.h"


// Configuration of the IIR resampler filter: Stopband attenuation
typedef enum
  {
    RESAMPLER_IIR_CONFIG_STOPBAND_ATTENUATION_40DB = 0,
    RESAMPLER_IIR_CONFIG_STOPBAND_ATTENUATION_60DB = 1
  } jvxAdaptiveResampler_config_stopband;

#include "jvx_tables_resampler.h"

// Struct for variant I
typedef struct
{
  struct
  {
    int id;
  } ident;

  struct
  {
    jvxInt32 oversamplingFactor;
    jvxInt32 buffersizeInMax;
    jvxInt32 buffersizeUpsampled;
    jvxDataFormat format;
    jvxInt16 lagrange_toLeft;
    jvxInt16 lagrange_toRight;
    jvxInt16 lagrange_width;
    jvxData stopbandattenuation_db;
    jvxData socket;
    jvxInt32 lFilter;
    jvxInt32 lFilterUpsampled;
    jvxInt32 midPosition;
    jvxData alpha;
    jvxData delta_f;
    jvxData group_delay;
    jvxInt32 lStates;
    jvxInt32 lStatesLagrangeUpsampled;
  } constants;

  struct
  {
    struct
    {
      jvxData* inv_denomL;
      jvxData**rootsL;
    } lagrange;

    jvxData* impResp;
    jvxData* statesLagrangeUpsampled;
    jvxData* states;
  } ram;

} ars_vI;

// Struct for variant II
typedef struct
{
  struct
  {
    int id;
  } ident;

  struct
  {
    jvxInt32 oversamplingFactor;
    jvxInt32 buffersizeInMax;
    jvxInt32 buffersizeUpsampled;
    jvxDataFormat format;
    jvxInt16 lagrange_toLeft;
    jvxInt16 lagrange_toRight;
    jvxInt16 lagrange_width;
    jvxData stopbandattenuation_db;
    jvxData socket;
    jvxInt32 lFilter;
    jvxInt32 lFilterUpsampled;
    jvxInt32 stateSpaceLagrange;
    jvxInt32 midPosition;
    jvxData alpha;
    jvxData delta_f;
    jvxData group_delay;
    jvxInt32 lStates;
  } constants;

  struct
  {
    struct
    {
      jvxData* inv_denomL;
      jvxData**rootsL;
    } lagrange;

    jvxData* impResp;
    jvxData* signalFragmentLagrange;
    jvxData* states;
  } ram;

} ars_vII;

// Struct for variant III
typedef struct
{
  struct
  {
    int id;
  } ident;

  struct
  {
    jvxInt32 oversamplingFactor;
    jvxInt32 buffersizeInMax;
    jvxInt32 buffersizeUpsampledMax;
    jvxDataFormat format;
    jvxInt16 lagrange_toLeft;
    jvxInt16 lagrange_toRight;
    jvxInt16 lagrange_width;
    jvxData stopbandattenuation_db;
    jvxInt32 lFilter;
    jvxInt32 filterOrder;
    jvxData group_delay;
    jvxInt32 lStatesLagrangeUpsampled;
  } constants;

  struct
  {
    struct
    {
      jvxData* inv_denomL;
      jvxData**rootsL;
    } lagrange;

    jvxData* statesLagrangeUpsampled;
    jvxData* statesIir;
  } ram;
  struct
  {
    struct
    {
      jvxData* coeffs;
    } num;

    struct
    {
      jvxData* coeffs;
    } den;
  } rom;
} ars_vIII;

// midPosition can also be negative
jvxDspBaseErrorType
jvxAdaptiveResampler_fir_fragment(jvxInt32 midPosition, ars_vII* handle)
{
  int i,j;
  for(i = 0; i < handle->constants.lagrange_width; i++)
    {
      // Map the linear counter 0, 1, 2,3,4 to -2, -1, 0, 1, 2
      jvxInt32 indU = i - handle->constants.lagrange_toLeft;

      // Treat also the mid position
      jvxInt32 indUM = indU + midPosition;

      // Shift factor for addressing within the upsampling time window
      int inFrameOffset = indUM % handle->constants.oversamplingFactor;

      int sampleOffset;
      jvxData accu;
      int cntC;
      int cntS;

      if(inFrameOffset < 0)
        {
          inFrameOffset += handle->constants.oversamplingFactor;
        }
      sampleOffset = (handle->constants.lFilter-1) + handle->constants.stateSpaceLagrange + (int)(floor((jvxData)indUM/(jvxData)handle->constants.oversamplingFactor));

      accu = 0.0;
      cntC = inFrameOffset;
      cntS = sampleOffset;

      for(j = 0; j < handle->constants.lFilter; j++)
        {
          jvxData op1 = handle->ram.states[cntS];
          jvxData op2 = handle->ram.impResp[cntC];
          accu += op1 * op2;
          cntC += handle->constants.oversamplingFactor;
          cntS--;
        }

      handle->ram.signalFragmentLagrange[i] = accu * handle->constants.oversamplingFactor;
    }
  return(JVX_DSP_NO_ERROR);
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// Adaptive resampler, VARIANT I
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

/**
 * Initialize resampler, variant I. Variant I means that we
 * 1) Upsample the input
 * 2) Interpolate all samples using polyphase FIR filter
 * 3) Make Lagrange interpolation.
 * Difference to variant II: In variant II only those samples which aere required for Lagrange interpolation aree computed.
 *///=========================================================================================================
jvxDspBaseErrorType
jvxAdaptiveResampler_variantI_initialize(jvxHandle** H, jvxData* delay_samples, int lagrange_toLeft, int lagrange_toRight, int lFilter, int buffersizeInMax, int oversamplingFactor, jvxDataFormat format)
{
  int i,j;
  ars_vI* handle;
  jvxData Omega_g;
  jvxData m_pi_omega_g;
  int cntO;

  // Do not accept NULL pointer for handle
  if(!H)
    {
      return(JVX_DSP_ERROR_INVALID_ARGUMENT);
    }

  // ====================================================================
  // Start config
  // ====================================================================
  // Configuration for lowpass design, FIR filter with Kaiser window

  JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, ars_vI);

  handle->ident.id = 1;

  // Some common parameters
  handle->constants.oversamplingFactor = oversamplingFactor;
  handle->constants.buffersizeInMax = buffersizeInMax;
  handle->constants.buffersizeUpsampled = handle->constants.buffersizeInMax * handle->constants.oversamplingFactor;
  handle->constants.format = format;

  // Lagrange interpolation setup
  handle->constants.lagrange_toLeft = lagrange_toLeft;
  handle->constants.lagrange_toRight = lagrange_toRight;
  handle->constants.lagrange_width = handle->constants.lagrange_toLeft + handle->constants.lagrange_toRight + 1;

  // If we want to have a clear algorithmic delay only allow for odd
  // filter length
  if(lFilter%2 == 0)
    {
      lFilter = lFilter +1;
    }

  // ====================================================================
  // Low pass design Low pass design Low pass design Low pass design
  // ====================================================================
  // Setup some parameters
  handle->constants.stopbandattenuation_db = STOPBANDATTENUATION_DB_FIR;
  handle->constants.socket = SOCKET_FIR;
  handle->constants.lFilter = lFilter;
  handle->constants.lFilterUpsampled = (lFilter-1) * handle->constants.oversamplingFactor + 1;
  handle->constants.midPosition = (lFilter-1)/2 *handle->constants.oversamplingFactor;
  handle->constants.alpha = 0;

  // From the stopband attenuation, derive the factor alpha for Kaiser
  // window
  if(handle->constants.stopbandattenuation_db > 21)
    {
      if(handle->constants.stopbandattenuation_db > 50)
        {
          handle->constants.alpha = 0.1102 * (handle->constants.stopbandattenuation_db - 8.7);
        }
      else
        {
          handle->constants.alpha = 0.5842 * pow((handle->constants.stopbandattenuation_db -21.0), 0.4) + 0.07886*(handle->constants.stopbandattenuation_db -21);
        }
    }

  // Allocate impulse response followed by some sapece for zeros
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.impResp, jvxData, (handle->constants.lFilterUpsampled + handle->constants.oversamplingFactor));
  memset(handle->ram.impResp, 0, sizeof(jvxData) * (handle->constants.lFilterUpsampled + handle->constants.oversamplingFactor));

  jvx_compute_window(handle->ram.impResp, handle->constants.lFilterUpsampled, handle->constants.alpha, 0,
                     JVX_WINDOW_KAISER, NULL);

  // Next, derive the desired frequency response parameters: Consider
  // a specific distance to the maximum allowed frequency
  handle->constants.delta_f = 0;
  if ( handle->constants.stopbandattenuation_db > 21.0 )
    {
      handle->constants.delta_f = (( handle->constants.stopbandattenuation_db - 7.95 ) )	/ ( 14.36 * ( handle->constants.lFilterUpsampled - 1 ) );
    }
  else
    {
      handle->constants.delta_f = ( 0.9222 ) / ( handle->constants.lFilterUpsampled - 1 );
    }

  Omega_g = ( 1/(jvxData)handle->constants.oversamplingFactor - handle->constants.delta_f );
  m_pi_omega_g = M_PI * Omega_g;

  // Compute the time domain impulse response based on the sin(x)/x
  for(i = 0; i < handle->constants.lFilterUpsampled; i++)
    {
      handle->ram.impResp[i] *= jvx_si( m_pi_omega_g * ((jvxData)i - handle->constants.midPosition))* Omega_g;
    }

  // ====================================================================
  // Allocate the relevant buffers for interpolation filter
  // ====================================================================

  // State buffer for convolution
  handle->constants.lStates = (handle->constants.lFilter-1) + handle->constants.buffersizeInMax;
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.states, jvxData, handle->constants.lStates);
  memset(handle->ram.states, 0, sizeof(jvxData) * handle->constants.lStates);

  // ====================================================================
  // Compute parameters for Lagrange interpolation
  // ====================================================================
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.lagrange.inv_denomL, jvxData, handle->constants.lagrange_width);
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.lagrange.rootsL, jvxData*, handle->constants.lagrange_width);
  for(i = 0; i < handle->constants.lagrange_width; i++)
    {
      JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.lagrange.rootsL[i], jvxData, handle->constants.lagrange_width-1);
    }

  cntO = 0;
  for(i = -handle->constants.lagrange_toLeft; i <= handle->constants.lagrange_toRight; i++)
    {
      int cntI = 0;
      int denT = 1;
      for(j = -handle->constants.lagrange_toLeft; j <= handle->constants.lagrange_toRight; j++)
        {
          if(i != j)
            {
              denT *= (i - j);
              handle->ram.lagrange.rootsL[cntO][cntI] = j;
              cntI++;
            }
        }
      handle->ram.lagrange.inv_denomL[cntO] = 1/((jvxData)denT);
      cntO++;
    }

  // ====================================================================
  // Allocate space to hold upsampled audio samples plus the Lagrange
  // "states"
  // ====================================================================
  handle->constants.lStatesLagrangeUpsampled = (handle->constants.lagrange_width - 1) + (handle->constants.buffersizeInMax * handle->constants.oversamplingFactor);
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.statesLagrangeUpsampled, jvxData, handle->constants.lStatesLagrangeUpsampled);
  memset(handle->ram.statesLagrangeUpsampled, 0, sizeof(jvxData) * handle->constants.lStatesLagrangeUpsampled);

  // ====================================================================
  // Compute the delay of the resampler
  // ====================================================================

  //% Compute the group delay
  //%[handle.fir.groupDelay, x] = grpdelay(handle.fir.impResp,1); <-
  //version computed from impulse response
  handle->constants.group_delay = (handle->constants.midPosition + handle->constants.lagrange_toRight) / (jvxData)handle->constants.oversamplingFactor;

  if(delay_samples)
    {
      *delay_samples = handle->constants.group_delay;
    }

  // Return reference to handle
  *H = handle;

  return(JVX_DSP_NO_ERROR);
}

/**
 * Termionate resampler, variant I.
 *///=========================================================================================================

jvxDspBaseErrorType
jvxAdaptiveResampler_variantI_terminate(jvxHandle* H)
{
  int i;
  // ====================================================================
  // Start config
  // ====================================================================
  // Configuration for lowpass design, FIR filter with Kaiser window

  ars_vI* handle = (ars_vI*)H;

  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.impResp);
  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.states);
  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.lagrange.inv_denomL);
  for(i = 0; i < handle->constants.lagrange_width; i++)
    {
      JVX_DSP_SAFE_DELETE_FIELD(handle->ram.lagrange.rootsL[i]);
    }
  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.lagrange.rootsL);
  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.statesLagrangeUpsampled);

  JVX_DSP_SAFE_DELETE_OBJECT(handle);
  return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType
jvxAdaptiveResampler_variantI_process(void* in, int bSizeIn, void* out, int bSizeOut, jvxHandle* H)
{
  int i,j,k;

  ars_vI* handle = (ars_vI*) H;

  jvxData* inDbl = (jvxData*)in;
  jvxInt64* inInt64 = (jvxInt64*)in;
  jvxInt32* inInt32 = (jvxInt32*)in;
  jvxInt16* inInt16 = (jvxInt16*)in;
  jvxInt8* inInt8 = (jvxInt8*)in;

  jvxData* outDbl = (jvxData*)out;
  jvxInt64* outInt64 = (jvxInt64*)out;
  jvxInt32* outInt32 = (jvxInt32*)out;
  jvxInt16* outInt16 = (jvxInt16*)out;
  jvxInt8* outInt8 = (jvxInt8*)out;

  int buffersizeUpsampled_local = bSizeIn * handle->constants.oversamplingFactor;
  int buffersize_local = bSizeIn;

  // This is the memove to copy new buffers into state buffer. A circular
  // buffer would be more complex on an architecture without circular
  // addressing support
  //memmove(handle->ram.states, handle->ram.states + handle->constants.buffersizeIn,
  //	sizeof(jvxData) * (handle->constants.lStates-handle->constants.buffersizeIn));

  jvxData* ptrStatesIn = handle->ram.states + (handle->constants.lFilter-1); //(handle->constants.lStates-handle->constants.buffersizeInMax);

  jvxData* ptrTmpOut;
  jvxSize inFrameOffset;
  jvxSize idxIn_offset;

  // Input filter for various datatypes
  switch(handle->constants.format)
    {
    case JVX_DATAFORMAT_DATA:
      for(i = 0; i < buffersize_local; i++)
        {
          *ptrStatesIn++ = *inDbl;
          inDbl++;
        }
      break;
    case JVX_DATAFORMAT_32BIT_LE:
      for(i = 0; i < buffersize_local; i++)
        {
          *ptrStatesIn++ = JVX_INT32_2_DATA(*inInt32);
          inInt32++;
        }
      break;
    case JVX_DATAFORMAT_16BIT_LE:
      for(i = 0; i < buffersize_local; i++)
        {
          *ptrStatesIn++ = JVX_INT16_2_DATA(*inInt16);
          inInt16++;
        }
      break;
    case JVX_DATAFORMAT_8BIT:
      for(i = 0; i < buffersize_local; i++)
        {
          *ptrStatesIn++ = JVX_INT8_2_DATA(*inInt8);
          inInt8++;
        }
      break;
    case JVX_DATAFORMAT_64BIT_LE:
      for(i = 0; i < buffersize_local; i++)
        {
          *ptrStatesIn++ = JVX_INT64_2_DATA(*inInt64);
          inInt64++;
        }
      break;
    default:
      assert(0);
    }

  // What happens for handle->constants.lagrange_width == 1?
  //memmove(handle->ram.statesLagrangeUpsampled,
  //	handle->ram.statesLagrangeUpsampled + (handle->constants.lStatesLagrangeUpsampled - (handle->constants.lagrange_width -1)),
  //	sizeof(jvxData) * (handle->constants.lagrange_width -1));

  // Already produce the output with a specific offset
  //jvxSize idxOut = (handle->constants.lagrange_width -1);
  ptrTmpOut = handle->ram.statesLagrangeUpsampled + (handle->constants.lagrange_width -1);

  // Step I: Produce oversampled output:
  inFrameOffset = 0;
  idxIn_offset = handle->constants.lFilter - 1;

  for(i = 0; i < /*handle->constants.buffersizeUpsampled*/buffersizeUpsampled_local; i++)
    {

      jvxData accu = 0.0;
      jvxSize cntS = idxIn_offset;
      jvxSize cntC = inFrameOffset;
      jvxSize tmp;

      for(j = 0; j < handle->constants.lFilter; j++)
        {
          jvxData op1 = handle->ram.states[cntS];
          jvxData op2 = handle->ram.impResp[cntC];
          accu = accu + op1 * op2;
          cntC += handle->constants.oversamplingFactor;
          cntS--;
        }

      // Consider the power manipulation due to oversampling, produce temp output
      *ptrTmpOut++ = accu * (jvxData)handle->constants.oversamplingFactor;

      // Index update
      tmp = inFrameOffset + 1;
      inFrameOffset = tmp % handle->constants.oversamplingFactor;
      tmp = tmp/handle->constants.oversamplingFactor;
      idxIn_offset += tmp;
    }

  // If we are here, we have computed the upsampled version of the input
  // Next, procede with the Lagrange interpolation

  // Loop over all output samples, employ output filter
  switch(handle->constants.format)
    {
    case JVX_DATAFORMAT_DATA:
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          // Reset accu
          jvxData accuOut = 0;
          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.statesLagrangeUpsampled[idxOffset + j];
            }

          // Here, the output filter
          *outDbl++ = accuOut;
        }
      break;
    case JVX_DATAFORMAT_32BIT_LE:
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          // Reset accu
          jvxData accuOut = 0;
          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.statesLagrangeUpsampled[idxOffset + j];
            }

          // Here, the output filter
          *outInt32++ = JVX_DATA2INT32(accuOut);
        }
      break;
    case JVX_DATAFORMAT_16BIT_LE:
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          // Reset accu
          jvxData accuOut = 0;
          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.statesLagrangeUpsampled[idxOffset + j];
            }

          // Here, the output filter
          *outInt16++ = JVX_DATA2INT16(accuOut);
        }
      break;
    case JVX_DATAFORMAT_8BIT:
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          // Reset accu
          jvxData accuOut = 0;
          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.statesLagrangeUpsampled[idxOffset + j];
            }

          // Here, the output filter
          *outInt8++ = JVX_DATA2INT8(accuOut);
        }
      break;
    case JVX_DATAFORMAT_64BIT_LE:
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          // Reset accu
          jvxData accuOut = 0;
          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.statesLagrangeUpsampled[idxOffset + j];
            }

          // Here, the output filter
          *outInt64++ = JVX_DATA2INT64(accuOut);
        }
      break;
    default:
      assert(0);
    }

  // Shift the states in buffers for next frame
  memmove(handle->ram.states, handle->ram.states + buffersize_local,
          sizeof(jvxData) * (handle->constants.lFilter-1));

  memmove(handle->ram.statesLagrangeUpsampled,
          handle->ram.statesLagrangeUpsampled + (buffersizeUpsampled_local),
          sizeof(jvxData) * (handle->constants.lagrange_width -1));


  return(JVX_DSP_NO_ERROR);
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// Adaptive resampler, VARIANT II
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

jvxDspBaseErrorType
jvxAdaptiveResampler_variantII_initialize(jvxHandle** H, jvxData* delay_samples, int lagrange_toLeft, int lagrange_toRight, int lFilter, int buffersizeInMax, int oversamplingFactor, jvxDataFormat format)
{
  int i,j;
  ars_vII* handle;
  jvxData Omega_g;
  jvxData m_pi_omega_g;
  int cntO;

  // Do not accept NULL pointer for handle
  if(!H)
    {
      return(JVX_DSP_ERROR_INVALID_ARGUMENT);
    }

  // ====================================================================
  // Start config
  // ====================================================================
  // Configuration for lowpass design, FIR filter with Kaiser window

  JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, ars_vII);

  handle->ident.id = 2;

  // Some common parameters
  handle->constants.oversamplingFactor = oversamplingFactor;
  handle->constants.buffersizeInMax = buffersizeInMax;
  handle->constants.buffersizeUpsampled = handle->constants.buffersizeInMax * handle->constants.oversamplingFactor;
  handle->constants.format = format;

  // Lagrange interpolation setup
  handle->constants.lagrange_toLeft = lagrange_toLeft;
  handle->constants.lagrange_toRight = lagrange_toRight;
  handle->constants.lagrange_width = handle->constants.lagrange_toLeft + handle->constants.lagrange_toRight + 1;

  // If we want to have a clear algorithmic delay only allow for odd
  // filter length
  if(lFilter%2 == 0)
    {
      lFilter = lFilter +1;
    }

  // ====================================================================
  // Low pass design Low pass design Low pass design Low pass design
  // ====================================================================
  // Setup some parameters
  handle->constants.stopbandattenuation_db = STOPBANDATTENUATION_DB_FIR;
  handle->constants.socket = SOCKET_FIR;
  handle->constants.lFilter = lFilter;
  handle->constants.lFilterUpsampled = (lFilter-1) * handle->constants.oversamplingFactor + 1;
  handle->constants.midPosition = (lFilter-1)/2 *handle->constants.oversamplingFactor;
  handle->constants.alpha = 0;

  // From the stopband attenuation, derive the factor alpha for Kaiser
  // window
  if(handle->constants.stopbandattenuation_db > 21)
    {
      if(handle->constants.stopbandattenuation_db > 50)
        {
          handle->constants.alpha = 0.1102 * (handle->constants.stopbandattenuation_db - 8.7);
        }
      else
        {
          handle->constants.alpha = 0.5842 * pow((handle->constants.stopbandattenuation_db -21.0), 0.4) + 0.07886*(handle->constants.stopbandattenuation_db -21);
        }
    }

  // Allocate impulse response followed by some sapece for zeros
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.impResp, jvxData, (handle->constants.lFilterUpsampled + handle->constants.oversamplingFactor));
  memset(handle->ram.impResp, 0, sizeof(jvxData) * (handle->constants.lFilterUpsampled + handle->constants.oversamplingFactor));

  jvx_compute_window(handle->ram.impResp, handle->constants.lFilterUpsampled, handle->constants.alpha, 0,
                     JVX_WINDOW_KAISER, NULL);

  // Next, derive the desired frequency response parameters: Consider
  // a specific distance to the maximum allowed frequency
  handle->constants.delta_f = 0;
  if ( handle->constants.stopbandattenuation_db > 21.0 )
    {
      handle->constants.delta_f = (( handle->constants.stopbandattenuation_db - 7.95 ) )	/ ( 14.36 * ( handle->constants.lFilterUpsampled - 1 ) );
    }
  else
    {
      handle->constants.delta_f = ( 0.9222 ) / ( handle->constants.lFilterUpsampled - 1 );
    }

  Omega_g = ( 1/(jvxData)handle->constants.oversamplingFactor - handle->constants.delta_f );
  m_pi_omega_g = M_PI * Omega_g;

  // Compute the time domain impulse response based on the sin(x)/x
  for(i = 0; i < handle->constants.lFilterUpsampled; i++)
    {
      handle->ram.impResp[i] *= jvx_si( m_pi_omega_g * ((jvxData)i - handle->constants.midPosition))* Omega_g;
    }

  // ====================================================================
  // Allocate the relevant buffers for interpolation filter
  // ====================================================================

  // Consider a specific space for states due to Lagrange
  handle->constants.stateSpaceLagrange = (jvxInt32)ceil((jvxData)(handle->constants.lagrange_width -1)/ (jvxData)handle->constants.oversamplingFactor);

  // State buffer for convolution
  handle->constants.lStates = (handle->constants.lFilter-1) + handle->constants.buffersizeInMax + handle->constants.stateSpaceLagrange;
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.states, jvxData, handle->constants.lStates);
  memset(handle->ram.states, 0, sizeof(jvxData) * handle->constants.lStates);

  // ====================================================================
  // Compute parameters for Lagrange interpolation
  // ====================================================================
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.lagrange.inv_denomL, jvxData, handle->constants.lagrange_width);
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.lagrange.rootsL, jvxData*, handle->constants.lagrange_width);
  for(i = 0; i < handle->constants.lagrange_width; i++)
    {
      JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.lagrange.rootsL[i], jvxData, handle->constants.lagrange_width-1);
    }

  cntO = 0;
  for(i = -handle->constants.lagrange_toLeft; i <= handle->constants.lagrange_toRight; i++)
    {
      int cntI = 0;
      int denT = 1;
      for(j = -handle->constants.lagrange_toLeft; j <= handle->constants.lagrange_toRight; j++)
        {
          if(i != j)
            {
              denT *= (i - j);
              handle->ram.lagrange.rootsL[cntO][cntI] = j;
              cntI++;
            }
        }
      handle->ram.lagrange.inv_denomL[cntO] = 1/((jvxData)denT);
      cntO++;
    }

  // ====================================================================
  // Allocate space to hold upsampled audio samples plus the Lagrange
  // "states"
  // ====================================================================
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.signalFragmentLagrange, jvxData, handle->constants.lagrange_width);
  memset(handle->ram.signalFragmentLagrange, 0, sizeof(jvxData) * handle->constants.lagrange_width);

  // ====================================================================
  // Compute the delay of the resampler
  // ====================================================================

  //% Compute the group delay
  //%[handle.fir.groupDelay, x] = grpdelay(handle.fir.impResp,1); <-
  //version computed from impulse response
  handle->constants.group_delay = (handle->constants.midPosition + handle->constants.lagrange_toRight) / (jvxData)handle->constants.oversamplingFactor;

  if(delay_samples)
    {
      *delay_samples = handle->constants.group_delay;
    }

  // Return reference to handle
  *H = handle;

  return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType
jvxAdaptiveResampler_variantII_terminate(jvxHandle* H)
{
  int i;
  // ====================================================================
  // Start config
  // ====================================================================
  // Configuration for lowpass design, FIR filter with Kaiser window

  ars_vII* handle = (ars_vII*)H;

  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.impResp);
  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.states);
  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.lagrange.inv_denomL);
  for(i = 0; i < handle->constants.lagrange_width; i++)
    {
      JVX_DSP_SAFE_DELETE_FIELD(handle->ram.lagrange.rootsL[i]);
    }
  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.lagrange.rootsL);
  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.signalFragmentLagrange);

  JVX_DSP_SAFE_DELETE_OBJECT(handle);
  return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType
jvxAdaptiveResampler_variantII_process(void* in, int bSizeIn, void* out, int bSizeOut, jvxHandle* H)
{
  int i,j,k;

  ars_vII* handle = (ars_vII*) H;

  jvxData* inDbl = (jvxData*)in;
  jvxInt64* inInt64 = (jvxInt64*)in;
  jvxInt32* inInt32 = (jvxInt32*)in;
  jvxInt16* inInt16 = (jvxInt16*)in;
  jvxInt8* inInt8 = (jvxInt8*)in;

  jvxData* outDbl = (jvxData*)out;
  jvxInt64* outInt64 = (jvxInt64*)out;
  jvxInt32* outInt32 = (jvxInt32*)out;
  jvxInt16* outInt16 = (jvxInt16*)out;
  jvxInt8* outInt8 = (jvxInt8*)out;

  int buffersizeUpsampled_local = bSizeIn * handle->constants.oversamplingFactor;
  int buffersize_local = bSizeIn;

  // This is the memove to copy new buffers into state buffer. A circular
  // buffer would be more complex on an architecture without circular
  // addressing support
  //memmove(handle->ram.states, handle->ram.states + handle->constants.buffersizeIn,
  //	sizeof(jvxData) * (handle->constants.lStates-handle->constants.buffersizeIn));

  jvxData* ptrStatesIn = handle->ram.states + (handle->constants.lFilter-1 + handle->constants.stateSpaceLagrange);//(handle->constants.lStates-handle->constants.buffersizeIn);

  // Input filter for various datatypes
  switch(handle->constants.format)
    {
    case JVX_DATAFORMAT_DATA:
      for(i = 0; i < buffersize_local; i++)
        {
          *ptrStatesIn++ = *inDbl;
          inDbl++;
        }
      break;
    case JVX_DATAFORMAT_32BIT_LE:
      for(i = 0; i < buffersize_local; i++)
        {
          *ptrStatesIn++ = JVX_INT32_2_DATA(*inInt32);
          inInt32++;
        }
      break;
    case JVX_DATAFORMAT_16BIT_LE:
      for(i = 0; i < buffersize_local; i++)
        {
          *ptrStatesIn++ = JVX_INT16_2_DATA(*inInt16);
          inInt16++;
        }
      break;
    case JVX_DATAFORMAT_8BIT:
      for(i = 0; i < buffersize_local; i++)
        {
          *ptrStatesIn++ = JVX_INT8_2_DATA(*inInt8);
          inInt8++;
        }
      break;
    case JVX_DATAFORMAT_64BIT_LE:
      for(i = 0; i < buffersize_local; i++)
        {
          *ptrStatesIn++ = JVX_INT64_2_DATA(*inInt64);
          inInt64++;
        }
      break;
    default:
      assert(0);
    }

  // Input filter for various datatypes
  switch(handle->constants.format)
    {
    case JVX_DATAFORMAT_DATA:
      // Loop over all output samples
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          jvxInt32 midPositionLagrange = (jvxInt32)idxOffset - handle->constants.lagrange_toRight;

          jvxData accuOut = 0;

          // Get the important fragment in upsampled data
          jvxAdaptiveResampler_fir_fragment(midPositionLagrange, handle);

          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.signalFragmentLagrange[j];
            }

          // Here, the output filter
          *outDbl++ = accuOut;
        }
      break;
    case JVX_DATAFORMAT_32BIT_LE:
      // Loop over all output samples
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          jvxInt32 midPositionLagrange = (jvxInt32)idxOffset - handle->constants.lagrange_toRight;

          jvxData accuOut = 0;

          // Get the important fragment in upsampled data
          jvxAdaptiveResampler_fir_fragment(midPositionLagrange, handle);

          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.signalFragmentLagrange[j];
            }

          // Here, the output filter
          *outInt32++ = JVX_DATA2INT32(accuOut);
        }
      break;
    case JVX_DATAFORMAT_16BIT_LE:
      // Loop over all output samples
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          jvxInt32 midPositionLagrange = (jvxInt32)idxOffset - handle->constants.lagrange_toRight;

          jvxData accuOut = 0;

          // Get the important fragment in upsampled data
          jvxAdaptiveResampler_fir_fragment(midPositionLagrange, handle);

          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.signalFragmentLagrange[j];
            }

          // Here, the output filter
          *outInt16++ = JVX_DATA2INT16(accuOut);
        }
      break;
    case JVX_DATAFORMAT_8BIT:
      // Loop over all output samples
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          jvxInt32 midPositionLagrange = (jvxInt32)idxOffset - handle->constants.lagrange_toRight;

          jvxData accuOut = 0;

          // Get the important fragment in upsampled data
          jvxAdaptiveResampler_fir_fragment(midPositionLagrange, handle);

          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.signalFragmentLagrange[j];
            }

          // Here, the output filter
          *outInt8++ = JVX_DATA2INT8(accuOut);
        }
      break;
    case JVX_DATAFORMAT_64BIT_LE:
      // Loop over all output samples
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          jvxInt32 midPositionLagrange = (jvxInt32)idxOffset - handle->constants.lagrange_toRight;

          jvxData accuOut = 0;

          // Get the important fragment in upsampled data
          jvxAdaptiveResampler_fir_fragment(midPositionLagrange, handle);

          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.signalFragmentLagrange[j];
            }

          // Here, the output filter
          *outInt64++ = JVX_DATA2INT64(accuOut);
        }
      break;
    default:
      assert(0);
    }

  // Shift the states in buffers for next frame
  memmove(handle->ram.states, handle->ram.states + buffersize_local,
          sizeof(jvxData) * (handle->constants.lFilter-1 + handle->constants.stateSpaceLagrange));

  return(JVX_DSP_NO_ERROR);
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// Adaptive resampler, VARIANT III
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

jvxDspBaseErrorType
jvxAdaptiveResampler_variantIII_initialize(jvxHandle** H, jvxData* delay_samples, int lagrange_toLeft, int lagrange_toRight, int lFilter, int buffersizeInMax, int oversamplingFactor, jvxDataFormat format)
{
  int i,j;
  ars_vIII* handle;
  int cntO;

  // Do not accept NULL pointer for handle
  if(!H)
    {
      return(JVX_DSP_ERROR_INVALID_ARGUMENT);
    }

  // ====================================================================
  // Start config
  // ====================================================================
  // Configuration for lowpass design, FIR filter with Kaiser window

  JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, ars_vIII);

  handle->ident.id = 3;

  // Some common parameters
  handle->constants.oversamplingFactor = oversamplingFactor;
  handle->constants.buffersizeInMax = buffersizeInMax;
  handle->constants.buffersizeUpsampledMax = handle->constants.buffersizeInMax * handle->constants.oversamplingFactor;
  handle->constants.format = format;

  // Lagrange interpolation setup
  handle->constants.lagrange_toLeft = lagrange_toLeft;
  handle->constants.lagrange_toRight = lagrange_toRight;
  handle->constants.lagrange_width = handle->constants.lagrange_toLeft + handle->constants.lagrange_toRight + 1;

  // ====================================================================
  // Low pass design is taken from Matlab (generated in Matlab version!)
  // ====================================================================
  // Setup some parameters
  if(handle->constants.oversamplingFactor > 1)
    {
      switch(IIR_CHARACTERISTIC_STOPBAND_ADAPTIVE_RESAMPLER)
        {
        case RESAMPLER_IIR_CONFIG_STOPBAND_ATTENUATION_40DB:
          handle->constants.stopbandattenuation_db = 40;

          switch(handle->constants.oversamplingFactor)
            {
            case 2:
              assert(ars_osmpl_2_stp_40dB_coeffs_num_dimx == ars_osmpl_2_stp_40dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_2_stp_40dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_2_stp_40dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_2_stp_40dB_coeffs_num_dimx;
              break;
            case 3:
              assert(ars_osmpl_3_stp_40dB_coeffs_num_dimx == ars_osmpl_3_stp_40dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_3_stp_40dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_3_stp_40dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_3_stp_40dB_coeffs_num_dimx;
              break;
            case 4:
              assert(ars_osmpl_4_stp_40dB_coeffs_num_dimx == ars_osmpl_4_stp_40dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_4_stp_40dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_4_stp_40dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_4_stp_40dB_coeffs_num_dimx;
              break;
            case 5:
              assert(ars_osmpl_5_stp_40dB_coeffs_num_dimx == ars_osmpl_5_stp_40dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_5_stp_40dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_5_stp_40dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_5_stp_40dB_coeffs_num_dimx;
              break;
            case 6:
              assert(ars_osmpl_6_stp_40dB_coeffs_num_dimx == ars_osmpl_6_stp_40dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_6_stp_40dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_6_stp_40dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_6_stp_40dB_coeffs_num_dimx;
              break;
            case 7:
              assert(ars_osmpl_7_stp_40dB_coeffs_num_dimx == ars_osmpl_7_stp_40dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_7_stp_40dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_7_stp_40dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_7_stp_40dB_coeffs_num_dimx;
              break;
            case 8:
              assert(ars_osmpl_8_stp_40dB_coeffs_num_dimx == ars_osmpl_8_stp_40dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_8_stp_40dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_8_stp_40dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_8_stp_40dB_coeffs_num_dimx;
              break;
            }
          break;

        case RESAMPLER_IIR_CONFIG_STOPBAND_ATTENUATION_60DB:
          handle->constants.stopbandattenuation_db = 60;
          switch(handle->constants.oversamplingFactor)
            {
            case 2:
              assert(ars_osmpl_2_stp_60dB_coeffs_num_dimx == ars_osmpl_2_stp_60dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_2_stp_60dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_2_stp_60dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_2_stp_60dB_coeffs_num_dimx;
              break;
            case 3:
              assert(ars_osmpl_3_stp_60dB_coeffs_num_dimx == ars_osmpl_3_stp_60dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_3_stp_60dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_3_stp_60dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_3_stp_60dB_coeffs_num_dimx;
              break;
            case 4:
              assert(ars_osmpl_4_stp_60dB_coeffs_num_dimx == ars_osmpl_4_stp_60dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_4_stp_60dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_4_stp_60dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_4_stp_60dB_coeffs_num_dimx;
              break;
            case 5:
              assert(ars_osmpl_5_stp_60dB_coeffs_num_dimx == ars_osmpl_5_stp_60dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_5_stp_60dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_5_stp_60dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_5_stp_60dB_coeffs_num_dimx;
              break;
            case 6:
              assert(ars_osmpl_6_stp_60dB_coeffs_num_dimx == ars_osmpl_6_stp_60dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_6_stp_60dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_6_stp_60dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_6_stp_60dB_coeffs_num_dimx;
              break;
            case 7:
              assert(ars_osmpl_7_stp_60dB_coeffs_num_dimx == ars_osmpl_7_stp_60dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_7_stp_60dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_7_stp_60dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_7_stp_60dB_coeffs_num_dimx;
              break;
            case 8:
              assert(ars_osmpl_8_stp_60dB_coeffs_num_dimx == ars_osmpl_8_stp_60dB_coeffs_den_dimx);
              handle->rom.num.coeffs = ars_osmpl_8_stp_60dB_coeffs_num;
              handle->rom.den.coeffs = ars_osmpl_8_stp_60dB_coeffs_den;
              handle->constants.lFilter = ars_osmpl_8_stp_60dB_coeffs_num_dimx;
              break;
            }
          break;
        }

      // Take values from generated files

      handle->constants.filterOrder = handle->constants.lFilter - 1;

      // Allocate states for filter
      JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.statesIir, jvxData, handle->constants.filterOrder);
      memset(handle->ram.statesIir, 0, sizeof(jvxData) * (handle->constants.filterOrder));
    }
  else
    {
      handle->rom.num.coeffs = NULL;
      handle->rom.den.coeffs = NULL;
      handle->constants.filterOrder = 0;
      handle->ram.statesIir = NULL;
    }

  // ====================================================================
  // Compute parameters for Lagrange interpolation
  // ====================================================================
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.lagrange.inv_denomL, jvxData, handle->constants.lagrange_width);
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.lagrange.rootsL, jvxData*, handle->constants.lagrange_width);
  for(i = 0; i < handle->constants.lagrange_width; i++)
    {
      JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.lagrange.rootsL[i], jvxData, handle->constants.lagrange_width-1);
    }

  cntO = 0;
  for(i = -handle->constants.lagrange_toLeft; i <= handle->constants.lagrange_toRight; i++)
    {
      int cntI = 0;
      int denT = 1;
      for(j = -handle->constants.lagrange_toLeft; j <= handle->constants.lagrange_toRight; j++)
        {
          if(i != j)
            {
              denT *= (i - j);
              handle->ram.lagrange.rootsL[cntO][cntI] = j;
              cntI++;
            }
        }
      handle->ram.lagrange.inv_denomL[cntO] = 1/((jvxData)denT);
      cntO++;
    }

  // ====================================================================
  // Allocate space to hold upsampled audio samples plus the Lagrange
  // "states"
  // ====================================================================
  handle->constants.lStatesLagrangeUpsampled = (handle->constants.lagrange_width - 1) + (handle->constants.buffersizeInMax * handle->constants.oversamplingFactor);
  JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.statesLagrangeUpsampled, jvxData, handle->constants.lStatesLagrangeUpsampled);
  memset(handle->ram.statesLagrangeUpsampled, 0, sizeof(jvxData) * handle->constants.lStatesLagrangeUpsampled);

  // ====================================================================
  // Delay? It is low but not specified (frequency dependent group delay)
  // ====================================================================

  handle->constants.group_delay = -1;

  if(delay_samples)
    {
      *delay_samples = handle->constants.group_delay;
    }

  // Return reference to handle
  *H = handle;

  return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType
jvxAdaptiveResampler_variantIII_terminate(jvxHandle* H)
{
  int i;
  // ====================================================================
  // Start config
  // ====================================================================
  // Configuration for lowpass design, FIR filter with Kaiser window

  ars_vIII* handle = (ars_vIII*)H;

  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.statesIir);
  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.lagrange.inv_denomL);
  for(i = 0; i < handle->constants.lagrange_width; i++)
    {
      JVX_DSP_SAFE_DELETE_FIELD(handle->ram.lagrange.rootsL[i]);
    }
  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.lagrange.rootsL);
  JVX_DSP_SAFE_DELETE_FIELD(handle->ram.statesLagrangeUpsampled);

  JVX_DSP_SAFE_DELETE_OBJECT(handle);
  return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType jvxAdaptiveResampler_variantIII_process(void* in, int bSizeIn, void* out, int bSizeOut, jvxHandle* H)
{
  int i,j,k;

  ars_vIII* handle = (ars_vIII*) H;

  jvxData* inDbl = (jvxData*)in;
  jvxInt64* inInt64 = (jvxInt64*)in;
  jvxInt32* inInt32 = (jvxInt32*)in;
  jvxInt16* inInt16 = (jvxInt16*)in;
  jvxInt8* inInt8 = (jvxInt8*)in;

  jvxData* outDbl = (jvxData*)out;
  jvxInt64* outInt64 = (jvxInt64*)out;
  jvxInt32* outInt32 = (jvxInt32*)out;
  jvxInt16* outInt16 = (jvxInt16*)out;
  jvxInt8* outInt8 = (jvxInt8*)out;

  int buffersizeUpsampled_local = bSizeIn * handle->constants.oversamplingFactor;
  int buffersize_local = bSizeIn;

  // What happens for handle->constants.lagrange_width == 1?
  //memmove(handle->ram.statesLagrangeUpsampled,
  //	handle->ram.statesLagrangeUpsampled + (handle->constants.lStatesLagrangeUpsampled - (handle->constants.lagrange_width -1)),
  //	sizeof(jvxData) * (handle->constants.lagrange_width -1));

  if(handle->constants.oversamplingFactor == 1)
    {
      // We need a special rule for oversampling=1 since we do not have filter coefficients

      // Already produce the output with a specific offset
      jvxData* ptrTmpOut = handle->ram.statesLagrangeUpsampled + (handle->constants.lagrange_width -1);

      // Step I: Produce oversampled output:
      //jvxData divC = 1/handle->rom.den.coeffs[0];

      // TODO: this for other datatypes
      switch(handle->constants.format)
        {
        case JVX_DATAFORMAT_DATA:
          // Loop over input samples
          for(i = 0; i < buffersize_local; i++)
            {
              // Set input and output accu
              *ptrTmpOut++  = inDbl[i];
            }
          break;
        case JVX_DATAFORMAT_32BIT_LE:
          // Loop over input samples
          for(i = 0; i < buffersize_local; i++)
            {
              // Set input and output accu
              *ptrTmpOut++ = JVX_INT32_2_DATA(inInt32[i]);

            }
          break;
        case JVX_DATAFORMAT_16BIT_LE:
          // Loop over input samples
          for(i = 0; i < buffersize_local; i++)
            {
              // Set input and output accu
              *ptrTmpOut++ = JVX_INT16_2_DATA(inInt16[i]);
            }
          break;
        case JVX_DATAFORMAT_8BIT:
          // Loop over input samples
          for(i = 0; i < buffersize_local; i++)
            {
              // Set input and output accu
              *ptrTmpOut++ = JVX_INT8_2_DATA(inInt8[i]);
            }
          break;
        case JVX_DATAFORMAT_64BIT_LE:
          // Loop over input samples
          for(i = 0; i < buffersize_local; i++)
            {
              // Set input and output accu
              *ptrTmpOut++ = JVX_INT64_2_DATA(inInt64[i]);

            }
          break;
        default:
          assert(0);
        }//switch(handle->constants.format)
    }// if(handle->constants.oversamplingFactor == 1)
  else
    {
      // Already produce the output with a specific offset
      jvxData* ptrTmpOut = handle->ram.statesLagrangeUpsampled + (handle->constants.lagrange_width -1);

      // Step I: Produce oversampled output:
      jvxData divC = 1/handle->rom.den.coeffs[0];

      // TODO: this for other datatypes
      switch(handle->constants.format)
        {
        case JVX_DATAFORMAT_DATA:
          // Loop over input samples
          for(i = 0; i < buffersize_local; i++)
            {
              // Set input and output accu
              jvxData accuIn = JVX_DATA_2_DATA(inDbl[i]);
              jvxData accuOut = 0;

              // Loop over one non-zero and handle.oversampling-1 zero samples
              for(j = 0; j < handle->constants.oversamplingFactor; j++)
                {
                  // First write output of filter
                  accuOut = (handle->ram.statesIir[0] + handle->rom.num.coeffs[0] * accuIn)*divC;

                  // Consider the states
                  for(k = 0; k < handle->constants.filterOrder-1; k++)
                    {
                      handle->ram.statesIir[k] = handle->ram.statesIir[k+1] + accuIn * handle->rom.num.coeffs[k+1] - accuOut * handle->rom.den.coeffs[k+1];
                    }

                  // The last state is fed only from input and output sample
                  handle->ram.statesIir[handle->constants.filterOrder-1] = accuIn * handle->rom.num.coeffs[handle->constants.filterOrder] -
                    accuOut * handle->rom.den.coeffs[handle->constants.filterOrder];

                  // For all except the first sample, set input to zero
                  accuIn = 0;

                  *ptrTmpOut++ = accuOut * handle->constants.oversamplingFactor;
                }
            }
          break;
        case JVX_DATAFORMAT_32BIT_LE:
          // Loop over input samples
          for(i = 0; i < buffersize_local; i++)
            {
              // Set input and output accu
              jvxData accuIn = JVX_INT32_2_DATA(inInt32[i]);
              jvxData accuOut = 0;

              // Loop over one non-zero and handle.oversampling-1 zero samples
              for(j = 0; j < handle->constants.oversamplingFactor; j++)
                {
                  // First write output of filter
                  accuOut = (handle->ram.statesIir[0] + handle->rom.num.coeffs[0] * accuIn)*divC;

                  // Consider the states
                  for(k = 0; k < handle->constants.filterOrder-1; k++)
                    {
                      handle->ram.statesIir[k] = handle->ram.statesIir[k+1] + accuIn * handle->rom.num.coeffs[k+1] - accuOut * handle->rom.den.coeffs[k+1];
                    }

                  // The last state is fed only from input and output sample
                  handle->ram.statesIir[handle->constants.filterOrder-1] = accuIn * handle->rom.num.coeffs[handle->constants.filterOrder] -
                    accuOut * handle->rom.den.coeffs[handle->constants.filterOrder];

                  // For all except the first sample, set input to zero
                  accuIn = 0;

                  *ptrTmpOut++ = accuOut * handle->constants.oversamplingFactor;
                }
            }
          break;
        case JVX_DATAFORMAT_16BIT_LE:
          // Loop over input samples
          for(i = 0; i < buffersize_local; i++)
            {
              // Set input and output accu
              jvxData accuIn = JVX_INT16_2_DATA(inInt16[i]);
              jvxData accuOut = 0;

              // Loop over one non-zero and handle.oversampling-1 zero samples
              for(j = 0; j < handle->constants.oversamplingFactor; j++)
                {
                  // First write output of filter
                  accuOut = (handle->ram.statesIir[0] + handle->rom.num.coeffs[0] * accuIn)*divC;

                  // Consider the states
                  for(k = 0; k < handle->constants.filterOrder-1; k++)
                    {
                      handle->ram.statesIir[k] = handle->ram.statesIir[k+1] + accuIn * handle->rom.num.coeffs[k+1] - accuOut * handle->rom.den.coeffs[k+1];
                    }

                  // The last state is fed only from input and output sample
                  handle->ram.statesIir[handle->constants.filterOrder-1] = accuIn * handle->rom.num.coeffs[handle->constants.filterOrder] -
                    accuOut * handle->rom.den.coeffs[handle->constants.filterOrder];

                  // For all except the first sample, set input to zero
                  accuIn = 0;

                  *ptrTmpOut++ = accuOut * handle->constants.oversamplingFactor;
                }
            }
          break;
        case JVX_DATAFORMAT_8BIT:
          // Loop over input samples
          for(i = 0; i < buffersize_local; i++)
            {
              // Set input and output accu
              jvxData accuIn = JVX_INT8_2_DATA(inInt8[i]);
              jvxData accuOut = 0;

              // Loop over one non-zero and handle.oversampling-1 zero samples
              for(j = 0; j < handle->constants.oversamplingFactor; j++)
                {
                  // First write output of filter
                  accuOut = (handle->ram.statesIir[0] + handle->rom.num.coeffs[0] * accuIn)*divC;

                  // Consider the states
                  for(k = 0; k < handle->constants.filterOrder-1; k++)
                    {
                      handle->ram.statesIir[k] = handle->ram.statesIir[k+1] + accuIn * handle->rom.num.coeffs[k+1] - accuOut * handle->rom.den.coeffs[k+1];
                    }

                  // The last state is fed only from input and output sample
                  handle->ram.statesIir[handle->constants.filterOrder-1] = accuIn * handle->rom.num.coeffs[handle->constants.filterOrder] -
                    accuOut * handle->rom.den.coeffs[handle->constants.filterOrder];

                  // For all except the first sample, set input to zero
                  accuIn = 0;

                  *ptrTmpOut++ = accuOut * handle->constants.oversamplingFactor;
                }
            }
          break;
        case JVX_DATAFORMAT_64BIT_LE:
          // Loop over input samples
          for(i = 0; i < buffersize_local; i++)
            {
              // Set input and output accu
              jvxData accuIn = JVX_INT64_2_DATA(inInt64[i]);
              jvxData accuOut = 0;

              // Loop over one non-zero and handle.oversampling-1 zero samples
              for(j = 0; j < handle->constants.oversamplingFactor; j++)
                {
                  // First write output of filter
                  accuOut = (handle->ram.statesIir[0] + handle->rom.num.coeffs[0] * accuIn)*divC;

                  // Consider the states
                  for(k = 0; k < handle->constants.filterOrder-1; k++)
                    {
                      handle->ram.statesIir[k] = handle->ram.statesIir[k+1] + accuIn * handle->rom.num.coeffs[k+1] - accuOut * handle->rom.den.coeffs[k+1];
                    }

                  // The last state is fed only from input and output sample
                  handle->ram.statesIir[handle->constants.filterOrder-1] = accuIn * handle->rom.num.coeffs[handle->constants.filterOrder] -
                    accuOut * handle->rom.den.coeffs[handle->constants.filterOrder];

                  // For all except the first sample, set input to zero
                  accuIn = 0;

                  *ptrTmpOut++ = accuOut * handle->constants.oversamplingFactor;
                }
            }
          break;
        default:
          assert(0);

        }//switch(handle->constants.format)
    }// else: if(handle->constants.oversamplingFactor == 1)

  // If we are here, we have computed the upsampled version of the input
  // Next, procede with the Lagrange interpolation

  // Loop over all output samples, employ output filter
  switch(handle->constants.format)
    {
    case JVX_DATAFORMAT_DATA:
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          // Reset accu
          jvxData accuOut = 0;
          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.statesLagrangeUpsampled[idxOffset + j];
            }

          // Here, the output filter
          *outDbl++ = JVX_DATA_2_DATA(accuOut);
        }
      break;
    case JVX_DATAFORMAT_32BIT_LE:
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          // Reset accu
          jvxData accuOut = 0;
          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.statesLagrangeUpsampled[idxOffset + j];
            }

          // Here, the output filter
          *outInt32++ = JVX_DATA2INT32(accuOut);
        }
      break;
    case JVX_DATAFORMAT_16BIT_LE:
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          // Reset accu
          jvxData accuOut = 0;
          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.statesLagrangeUpsampled[idxOffset + j];
            }

          // Here, the output filter
          *outInt16++ = JVX_DATA2INT16(accuOut);
        }
      break;
    case JVX_DATAFORMAT_8BIT:
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          // Reset accu
          jvxData accuOut = 0;
          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.statesLagrangeUpsampled[idxOffset + j];
            }

          // Here, the output filter
          *outInt8++ = JVX_DATA2INT8(accuOut);
        }
      break;
    case JVX_DATAFORMAT_64BIT_LE:
      for(i = 0; i < bSizeOut; i++)
        {
          jvxData fracIdxInFrame = ((jvxData)(i * buffersizeUpsampled_local)) / ((jvxData)bSizeOut);
          jvxSize idxOffset = (jvxSize)floor(fracIdxInFrame);
          jvxData idxDelta = fracIdxInFrame - idxOffset;

          // Reset accu
          jvxData accuOut = 0;
          for(j = 0; j < handle->constants.lagrange_width; j++)
            {
              // Compute and apply Lagrange coefficients
              jvxData coeff = 1;
              for(k = 0; k < handle->constants.lagrange_width-1; k++)
                {
                  coeff = coeff * (idxDelta - handle->ram.lagrange.rootsL[j][k]);
                }

              coeff *= handle->ram.lagrange.inv_denomL[j];
              accuOut += coeff * handle->ram.statesLagrangeUpsampled[idxOffset + j];
            }

          // Here, the output filter
          *outInt64++ = JVX_DATA2INT64(accuOut);
        }
      break;
    default:
      assert(0);
    }

  // Copy the state buffer at the end for next frame
  memmove(handle->ram.statesLagrangeUpsampled,
          handle->ram.statesLagrangeUpsampled + buffersizeUpsampled_local,
          sizeof(jvxData) * (handle->constants.lagrange_width -1));

  return(JVX_DSP_NO_ERROR);
}
