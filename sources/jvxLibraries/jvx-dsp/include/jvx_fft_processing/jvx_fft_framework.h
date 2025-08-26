/*															  		*
 *  ***********************************************************		*
 *  ***********************************************************		*
 *  																*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Copyright (C) Javox Solutions GmbH - All Rights Reserved		*
 *  Unauthorized copying of this file, via any medium is			*
 *  strictly prohibited. 											*
 *  																*
 *  Proprietary and confidential									*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Written by Hauke Krüger <hk@javox-solutions.com>, 2012-2020		*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Javox Solutions GmbH, Gallierstr. 33, 52074 Aachen				*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Contact: info@javox-solutions.com, www.javox-solutions.com		*
 *  																*
 *  ********************************************************   		*/

#ifndef __JVX_FFT_FRAMEWORK_H__
#define __JVX_FFT_FRAMEWORK_H__

#include "jvx_dsp_base.h"
#include "jvx_windows/jvx_windows.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_tools/jvx_fft_tools.h"

JVX_DSP_LIB_BEGIN

#define JVX_FFTFRAMEWORK_DESCRIPTION "Javox FFT framework"
#define JVX_FFTFRAMEWORK_VERSION "0.1"

//! Typedefs for FFT framework types
enum
{
	JVX_FFT_FRAMEWORK_SW_OLA = 0,
	JVX_FFT_FRAMEWORK_DW_OLA,
	JVX_FFT_FRAMEWORK_OLA_CF,
	JVX_FFT_FRAMEWORK_OLS,
	JVX_FFT_FRAMEWORK_OLS_CF,
        JVX_FFT_FRAMEWORK_NUMTYPES
};
typedef jvxInt16 jvx_fftFrameworkType;

// Configuration parameters which are constant during runtime.
typedef struct {
	int dummy; // <- HK: At least one field required inside to compile in Windows

} jvx_fftFramework_cfg_const;


// Configuration parameters which can be changed anytime
typedef struct  {
	// Required for compilation in Windows
	int dummy;
} jvx_fftFramework_prm;


// Configuration parameters which can be changed between buffer processing
typedef struct  {
  jvxData bufferSizeFactor; // bufferSize = max(1,bufferSizeFactor) * frameSize
  jvx_fftFrameworkType frameworkType;
  jvxInt16 nFFT;
  jvxInt16 nIFFT;
  jvxInt16 fftLengthLog2;
  jvxInt16 applyAliasCompensation;
} jvx_fftFramework_prmSync;


// debug struct
typedef struct {

  // set on init
  // ...

  // updated in each frame
  unsigned int frameCounter;

  // updated on async prm update
  // ...

  // updated on sync prm update
  jvxInt16 delay;
  jvxSize spectrumSize;
  jvxSize fftLength;

} jvx_fftFramework_readOnly;


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
  jvx_fftFramework_prmSync prmSync;
  jvx_fftFramework_prm prm;
  jvx_fftFramework_readOnly readOnly;

  // public submodules
  // ... none ...

} jvx_fftFramework;

////
// API
////

jvxDspBaseErrorType jvx_fftFramework_configInit(jvx_fftFramework *hdl,
                                                unsigned int pid);

jvxDspBaseErrorType jvx_fftFramework_init(jvx_fftFramework* hdl,
                                          int frameSize, jvxHandle* fftCfgHdl);

jvxDspBaseErrorType jvx_fftFramework_processFFT(jvx_fftFramework* hdl,
                                                jvxInt16 fftIdx,
                                                jvxData* in,
                                                jvxDataCplx* out);

jvxDspBaseErrorType jvx_fftFramework_processFFT_ZP(jvx_fftFramework* hdl,
                                                   jvxData* in,
                                                   jvxDataCplx* out);

jvxDspBaseErrorType jvx_fftFramework_processIFFT(jvx_fftFramework* hdl,
                                                 jvxInt16 ifftIdx,
                                                 const jvxDataCplx* in,
                                                 jvxDataCplx* weights,
                                                 jvxData* out,
                                                 jvxData* aliasError_dB);

jvxDspBaseErrorType jvx_fftFramework_terminate(jvx_fftFramework* hdl);


jvxDspBaseErrorType jvx_fftFramework_update(jvx_fftFramework* hdl,
                                            jvxCBool syncUpdate);

JVX_DSP_LIB_END


#endif

