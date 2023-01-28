#ifndef SIGNAL_UTILS_H__
#define SIGNAL_UTILS_H__

#include "jvxVocoder/vocoder.h"
/*!
 * Function to evaluate the hamming windowing function:
 * \f[ h_{window}(k) = 
 *  0.54 - 0.46 \cdot cos(\frac{\pi \cdot m}{k_{middle}-1}) 
 *	\f] for the lower part of the window and 
 * \f[ h_{window}(k) = 
 *  0.54 - 0.46 \cdot cos(\frac{\pi \cdot (L_{window}-m-1)}
 * {L_{window}-k_{middle}-1})\f] for te upper part with 
 * \f[k_{middle}= middleFromLeft \f] and
 * \f[k = index\f] and \f[L_{window} = length\f] */
jvxData hammingWindow(int index, int middleFromLeft, int length);

/*!
 * Function to evaluate a binomial correlation window:
 * \f[ h_{corr}(k) = \frac{binom{2\cdot q}{(q-k)}}{binom{2\cdot q}{q}} = 
 * \frac{\prod_{i=0}^{k-1}(q-i)}{\prod_{i=0}^{k-1}(q+k-i)}\f] with
 * \f[ k=index \f] */
jvxData correlationWindow(int index, int length);

/*! Specify Q for binmial window */
#define Q_BIN 6321

#endif
