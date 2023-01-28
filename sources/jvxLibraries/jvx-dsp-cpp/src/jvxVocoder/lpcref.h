#ifndef __LPCREF_H__
#define __LPCREF_H__

#include "jvxVocoder/vocoder.h"


/* This file includes all functions required for a transformation 
 * between LPC and reflection coefficients */

/*! Convert reflection coeffs into LPCs */
void 
ref2Lpc(const jvxData* refCoeffs, int amountRefCoeffs,
			 jvxData* lpcCoeffs);



/*! Convert LPCs into reflection coeffs */
void 
lpc2Ref(const jvxData* lpcCoeffs, int amountLPCCoeffs, jvxData* refCoeffs);

#endif
