

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "jvxVocoder/speech2Lsf.h"
#include "jvxVocoder/autocorrelation.h"
#include "jvxVocoder/correlation2Lsfs.h"

/* Main function: Determine the LSFs, pass windowing functions as functionpointers.
 * This function is for using multiple windows (quick attempts) only. 
 * To evaluate the complete windowing function in each step is very complex */
int
speech2lsp_functions( jvxData  *xin, jvxData  *lsfs, 
					  int amountSpeechSamples, int amountLsfs, 
					  int middleFromLeft,
					  jvxData(*f_windowSpeech)(int index, int middleFromLeft, 
											  int length), 
					  jvxData(*f_windowCorr)(int index, int length))
{
	jvxData *correlation;
	jvxData *correlationWindow;
	jvxData *speechWindow;
	jvxData *reflect;
	int result = 1;
	int i;
	
	correlation = (jvxData*) malloc(sizeof(jvxData)*(amountLsfs+1));
	correlationWindow = (jvxData*) malloc(sizeof(jvxData)*(amountLsfs+1));
	speechWindow = (jvxData*) malloc(sizeof(jvxData)*(amountSpeechSamples));
	reflect = (jvxData*) malloc(sizeof(jvxData)*(amountLsfs));

	for(i =0; i< amountSpeechSamples; i++)
	{
		speechWindow[i] = f_windowSpeech(i, middleFromLeft, amountSpeechSamples);
	}
	for(i =0; i< (amountLsfs+1); i++)
	{
		correlationWindow[i] = f_windowCorr(i, (amountLsfs+1));
	}

	/* Determine autocorrelation function */
	autoCorrelation(correlation, correlationWindow, (amountLsfs+1),
					xin, speechWindow, amountSpeechSamples);
					
	/* Determine the LSPS */
	result = correlation2Lsfs( correlation, lsfs, reflect, amountLsfs);

	free(correlation);
	free(correlationWindow);
	free(speechWindow);
	free(reflect);
	return(result);
}

/* Main function: Determine the LSFs, pass windowing functions as array.
 * This function should be used for the final version of the coder as
 * the windowing function must only be once evaluated. */
int 
speech2lsp_array( jvxData  *xin, jvxData  *lsfs, 
				  int amountSpeechSamples, int amountLsfs, 
				  jvxData *arrayWindowSpeech, 
				  jvxData *arrayWindowCorrelation)
{
	jvxData *correlation;
	jvxData *reflect;
	int result = 1;
	
	correlation = (jvxData*) malloc(sizeof(jvxData)*(amountLsfs+1));
	reflect = (jvxData*) malloc(sizeof(jvxData)*(amountLsfs));

	/* Determine the autocorrelation coefficients */
	autoCorrelation(correlation, arrayWindowCorrelation, (amountLsfs+1),
					xin, arrayWindowSpeech, amountSpeechSamples);

	/* Based on the CORRS determine the LSFs */
	result = correlation2Lsfs( correlation, lsfs, reflect, amountLsfs);

	free(correlation);
	free(reflect);
	return(result);
}
	



