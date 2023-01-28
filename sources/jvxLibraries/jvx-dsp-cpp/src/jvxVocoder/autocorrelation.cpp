
#include "jvxVocoder/autocorrelation.h"

/* Determine the windowed correlation function */
void
autoCorrelation(jvxData* correlation, const jvxData* correlationWindow, int amountCorr,
				const jvxData* xin, const jvxData* speechWindow, int amountSpeechSamples)
{
	int i,j;

	for(i=0; i< amountCorr; i++)
	{
		correlation[i] =0;
		for(j=0; j<(amountSpeechSamples-i);j++)
		{
			correlation[i] += xin[j]*speechWindow[j] * xin[i+j]*speechWindow[i+j];

		}
		correlation[i] = correlation[i]*correlationWindow[i];
	}

	for(i=1; i< amountCorr; i++)
	{
		correlation[i] /= (correlation[0] + JVX_DATA_EPS);
	}
	correlation[0] = 1.0;
}
