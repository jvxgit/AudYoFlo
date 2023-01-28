#ifndef __MISCSIGNALUTILS_H__
#define __MISCSIGNALUTILS_H__

#include "jvxVocoder/vocoder.h"


jvxInt16
convSignalImpulse(jvxData* inputSignal, jvxData* impulseInput, jvxInt16 lVec, jvxData* resultVec);

//! IIR filter with fw and bw coefficients, DF1
jvxInt16 iirFilter_2can_fwbw(jvxData* stateBuf, jvxData* fwBuffer, jvxData* bwBuffer, 
		jvxInt16 numFilterCoeffs, jvxData* inputBuf, jvxData* outputBuf, jvxInt16 lengthSignal);

jvxInt16 iirFilter_2can_fwbw(jvxData* stateBuf, jvxData* fwBuffer, jvxData* bwBuffer, 
					jvxInt16 numFilterCoeffs, jvxData inputSample, jvxData* outputSample);

jvxInt16 iirFilter_1can_fwbw(jvxData* stateBuf, jvxData* fwBuffer, jvxData* bwBuffer, 
		jvxInt16 numFilterCoeffs, jvxData* inputBuf, jvxData* outputBuf, jvxInt16 lengthSignal);

jvxInt16 iirFilter_1can_fwbw(jvxData* stateBuf, jvxData* fwBuffer, jvxData* bwBuffer, 
						   jvxInt16 numFilterCoeffs, jvxData inputSample, jvxData* outputSample);

						   //! IIR filter with only bw coefficients, DF1
jvxInt16 iirFilter_2can_bw(jvxData* stateBuf, jvxData* bwBuffer, 
	jvxInt16 numFilterCoeffs, jvxData* inputBuf, jvxData* outputBuf, jvxInt16 lengthSignal);

jvxInt16 iirFilter_1can_bw(jvxData* stateBuf, jvxData* bwBuffer, 
		jvxInt16 numFilterCoeffs, jvxData* inputBuf, jvxData* outputBuf, jvxInt16 lengthSignal);

//! FIR filter, DF1
jvxInt16 firFilter_2can(jvxData* stateBuf, jvxData* fwBuffer, 
		jvxInt16 numFilterCoeffs, jvxData* inputBuf, jvxData* outputBuf, jvxInt16 lengthSignal);

jvxInt16 firFilter_1can(jvxData* stateBuf, jvxData* fwBuffer, 
		jvxInt16 numFilterCoeffs, jvxData* inputBuf, jvxData* outputBuf, jvxInt16 lengthSignal);

jvxInt16 autocorrFunc(jvxData* in, jvxData* autocorrBuff, jvxInt16 lengthIn, jvxInt16 orderAutocorr);

jvxInt16
logarithmicQuantizer(jvxData* inputSample, jvxData* outputSample, jvxData scaleFac);

#endif