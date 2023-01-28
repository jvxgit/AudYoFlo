

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "jvxVocoder/signalUtils.h"

#ifndef M_PI
#define M_PI 3.141592654
#endif

jvxData 
hammingWindow(int index, int middleFromLeft, int length)
{
	if(index < middleFromLeft)
	{
		return(0.54 -0.46*cos((M_PI *index)/(middleFromLeft-1)));
	}
	else
	{
/* For a correct window, uncomment the following line, the failure is added to */
/* compare with the original LPC-algorithm */
		return(0.54 -0.46*cos((M_PI *(length-index-1)/((length-middleFromLeft)-1))));
/*Error version: return(0.54 -0.46*cos((M_PI *(length-index)/((length-middleFromLeft))))); */
	}
}

jvxData
correlationWindow(int index, int length)
{
	int q = Q_BIN;
	int i;
	jvxData nom = 1;
	jvxData denom = 1;

	for(i= 0; i < index; i++)
	{
		nom *= q-i;
		denom *= q+index-i;
	} 

	return(nom/denom);
}



