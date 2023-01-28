#ifndef SPEECH2LSP_H__
#define SPEECH2LSP_H__

#include "jvxVocoder/vocoder.h"

/*==================================================*/

/*!
 * Function to determine LSF coefficients for a given 
 * speech buffer (xin) of length amountSpeechSamples.
 * For the windowing purpose the speech buffer can be weighted 
 * window specified as function pointer f_windowSpeech which
 * must not be symmetric (lower bound: 0, upper bound: amountSpeechSamples-1,
 * modified middle: middleFromLeft).
 * The function must be able to return for one index, the upper boundary
 * and the modified middle position a windowing weight (return value jvxData).
 * The result of the autocorrelation function can also be weighted by a window
 * specified as function pointer f_windowCorr. The mechanism is the same as the
 * one for speech windowing.
 * This function is not very efficient due to the fact that the window must be evaluated 
 * for each function call. For testing purpose different weighting functions can 
 * be tested. 
 * Return value is 1 for stable filter, 0 for instable one.*/
int speech2lsp_functions( jvxData  *xin, jvxData  *lsfs, 
						   int amountSpeechSamples, int amountLsfs, 
						   int middleFromLeft,
						   jvxData (*f_windowSpeech)(int, int, int), 
						   jvxData (*f_windowCorr)(int, int));
	
/*!
 * Function to determine LSF coefficients for a given 
 * speech buffer (xin) of length amountSpeechSamples.
 * For the windowing purpose the speech buffer can be weighted 
 * with the values in the buffer arrayWindowSpeech.
 * The correlation function can be weighted with the window specified in
 * arrayWindowCorrelation.
 * This function is more efficient than the previous one due to the fact 
 * that the window is passed as parameter.
 * Return value is 1 for stable filter, 0 for instable one.*/
int speech2lsp_array( jvxData  *xin, jvxData  *lsfs, 
					   int amountSpeechSamples, int amountLsfs, 
					   jvxData *arrayWindowSpeech, 
					   jvxData *arrayWindowCorrelation);




#endif


