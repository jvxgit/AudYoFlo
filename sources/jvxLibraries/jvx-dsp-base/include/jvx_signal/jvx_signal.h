#ifndef __JVX_SIGNAL_H__
#define __JVX_SIGNAL_H__

#include "jvx_dsp_base.h"

#include <math.h>

JVX_DSP_LIB_BEGIN

void jvx__signal_copy_multply_gain__max_level(jvxHandle** copyTo, const jvxHandle** copyFrom, jvxSize numChans, jvxSize buffersize, jvxDataFormat processingFormat, jvxData gain, jvxData* maxAbs);
void jvx__signal_copy_multply_gain__saturate__max_level(jvxHandle** copyTo, const jvxHandle** copyFrom, jvxSize numChans, jvxSize buffersize, jvxDataFormat processingFormat, jvxData gain, jvxData satValAbsMax, jvxData*maxAbs);
void jvx__signal_copy_multply_gain__saturate__smoothed_level(jvxHandle** copyTo, const jvxHandle** copyFrom, jvxSize numChans, jvxSize buffersize, jvxDataFormat processingFormat, jvxData gain, jvxData satValAbsMax, jvxData smooth, jvxData* repPower);
void jvx__signal_copy_multply_gain__saturate_detect__smoothed_level(jvxHandle** copyTo, const jvxHandle** copyFrom, jvxSize numChans, jvxSize buffersize, jvxDataFormat processingFormat, jvxData gain, jvxData satValAbsMax, jvxData smooth, jvxData* repPower, jvxCBool* repOverload);

JVX_DSP_LIB_END

#endif
