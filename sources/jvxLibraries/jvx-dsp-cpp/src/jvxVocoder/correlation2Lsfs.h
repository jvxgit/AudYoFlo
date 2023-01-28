#ifndef CORRELATION2LSFS_H__
#define CORRELATION2LSFS_H__

#include "jvxVocoder/vocoder.h"

/*! 
 * Function to determine the (amountCorrelation-1) reflection (PARCOR) 
 * coefficients based on the amountCorrelation correlation coefficients */
int correlation2Lsfs( const jvxData *correlation, jvxData *lsps, 
					   jvxData *reflectionCoeffs, 
					   int amountCorrelation);

#endif
