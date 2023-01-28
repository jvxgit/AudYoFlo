#ifndef AUTOCORRELATION_H__
#define AUTOCORRELATION_H__

#include "jvxVocoder/vocoder.h"

/*! Function to determine the autocorrelation coefficients for a given speech
 *  buffer:
 *  \f[ cor(k) = (\sum_{i=0}^{L_{speech}-i-1} x_{in}(i) \cdot h_{speech}(i) 
 *      \cdot x_{in}(i+k) \cdot h_{speech}(i+k)) \cdot h_{corr}(k) \f]
 *  with:
 *  \f[ cor = correlation \f]
 *  \f[ L_{speech} = amountSpeechSamples \f]
 *  \f[ h_{speech} = speechWindow \f]
 *  \f[ h_{corr} = correlationWindow \f] */
void autoCorrelation(jvxData* correlation, const jvxData* correlationWindow, 
					 int amountCorr, const jvxData* xin, 
					 const jvxData* speechWindow, int amountSpeechSamples);

#endif
