/* ============================================================	*
 * LABORATORY COURSES FOR OBJECT ORIENTED PROGRAMMING,			*
 * INSTITUTE OF COMMUNICATION SYSTEMS AND DATA PROCESSING,		*
 * IND, RWTH-AACHEN												*/

/* DESCRIPTION:													*
 *	Class for an openloop pitch estimation. This class receives *
 *  frames of input speech and finds the best matching pitch 	*
 * 	in a specific range of possible pitch-related delays.		*
 *  The passed samples for the pitch analysis are stored 		*
 * 	internally. Pitch doubling can be avoided by the module		*
 *																*/

/* CLASS/FUNCTION:												*
 *	openLoopPitchEstimation										*
 *============================================================	*/
#include <vector>
#include <float.h>
#include <math.h>

#include "jvxVocoder/vocoder.h"
#include "jvxVocoder/openLoopPitchEstimation.h"

#ifndef HUGE
#define HUGE FLT_MAX
#endif

/*===========================================================*/
/* 	Constructor: Create a buffer that is sufficiently large to*
 * 	store as many values from the past as required to search  *
 *	for the maximum pitch period delay*/
openLoopPitchEstimation::openLoopPitchEstimation(int maxInter, 
												 int minInter, int lengthFrame,
												 jvxData weight, int delta)
{
	int i =0;
	maxTimeIntervall = maxInter;
	minTimeIntervall = minInter;

	frameLengthMax = lengthFrame;

	weightPitchDoubling = weight;
	deltaPitchDoubling = delta;

	// set length of buffer and set to zero
	bufferLength=maxTimeIntervall+frameLengthMax+1;

    buffer=(jvxData*)calloc(bufferLength,sizeof(jvxData));


}

openLoopPitchEstimation::~openLoopPitchEstimation()
{

	free(buffer);

}




/*============================================================*/
/* 	Add the input to the buffer, therefore copy values from the*
 * 	past as a leftshift and then the values from the current   *
 *	buffer.*/
bool 
openLoopPitchEstimation::setInput(const jvxData *subframe)
{
	// Copy as leftshift
	memcopy(buffer, buffer, frameLengthMax, 0, bufferLength-frameLengthMax);
    //memcpy(&buffer[0],&buffer[frameLengthMax],(bufferLength-frameLengthMax)*sizeof(jvxData));

	// Copy the new values
    //memcpy(&buffer[bufferLength-frameLengthMax],subframe,frameLengthMax*sizeof(jvxData));
	memcopy(subframe, buffer, 0, bufferLength-frameLengthMax, frameLengthMax);

	return(true);
}

/*============================================================*/
/* 	Determine the open loop pitch period based on the 			*
 *	correlation between the original signal and the shifted one.*
 *	Another output of this routine is the criterion for the 	*
 *	voiced/unvoiced characteristic of the current speech segment*/
int 
openLoopPitchEstimation::determineOpenLoopPitch(jvxData& excitationCrit)
{
	jvxSize i = 0;
	jvxSize j =0;

	jvxData correlation[FRAME_SIZE];
	int p = 0;
	int delta = deltaPitchDoubling;
	
	jvxData nom;
	jvxData denom1, denom2;
	jvxData jvxDataWeight = weightPitchDoubling;

	// Copy buffer and shift to determine the first 
	// possible pitch autocorrelation 


	jvxData *tempBuffer = (jvxData *)malloc(bufferLength*sizeof(jvxData));

    memcopy(buffer,tempBuffer,0,0,bufferLength);

	shift(tempBuffer,-minTimeIntervall,bufferLength);

	int max_index = 0;
	jvxData maxValue = -HUGE;
	
	// Find the maximum autocorrelation in the integer delay range
	for(i = minTimeIntervall ; i <=maxTimeIntervall; i ++)
	{
		// Determine autocorrelation
		correlation[i-minTimeIntervall] = 0;

		for(j =0; j < bufferLength; j++)
			correlation[i-minTimeIntervall] += buffer[j] * tempBuffer[j];
		
		// Find a way to avoid pitch doubling
		if(i<2*max_index-2)
		{
			if(correlation[i-minTimeIntervall] > maxValue)
			{
				max_index = i;
				maxValue = correlation[i-minTimeIntervall];
			}
		}
		else
		{
			// If pitch jvxData may be possible, consider a weighting coefficient
			if(correlation[i-minTimeIntervall]*jvxDataWeight > maxValue)
			{
				max_index = i;
				maxValue = correlation[i-minTimeIntervall];
			}
		}
		// Get the next autocorrelation coeff by simply shifting the buffer

		shift(tempBuffer,-1,bufferLength);
	}

	jvxDataWeight = jvxDataWeight*jvxDataWeight;
	
	// Avoid pitch triples
	p = max_index/3;
	
	if(p>= minTimeIntervall+delta)
	{
		for( i = p-delta; i < p+delta; i++)
		{
			if(maxValue * jvxDataWeight < correlation[i-minTimeIntervall])
				max_index = i;
		}
	}

	// Determine criterion for voiced/unvoiced
	
    memcopy(buffer,tempBuffer,0,0,bufferLength);


	shift(tempBuffer, -max_index,bufferLength);

	nom =0;
	denom1 = 0;
	denom2 = 0;
	for(i = 0; i < bufferLength; i++)
	{
		nom += buffer[i] * tempBuffer[i];
		denom1 += buffer[i] * buffer[i];
		denom2 += tempBuffer[i] * tempBuffer[i];
	}
	
	excitationCrit = nom/(sqrt(denom1*denom2)+0.01);

	free(tempBuffer);
	return(max_index);
}

//============================================================	
		
	
/*============================================================*/
/* 	Shift the values in buffer dest to left or right.				*
 * 	If shiftCoeff > 0, shift into the right direction, otherwhise 	*
 * 	into the left direction. The new values will be set to zero 	*
 *	when inserting.*/
bool
shift(jvxData *dest, int shiftCoeff,int size)
{
	unsigned int i;

	if(size < shiftCoeff)
		return(false);

	// Shift to right
	if(shiftCoeff > 0)
	{
		for(i =0; i <(size-shiftCoeff);i++)
		{
			dest[(size-1-i)] = dest[(size-1-i-shiftCoeff)];
		}
		for(i =0; i < (unsigned)shiftCoeff;i++)
		{
			dest[i] = 0;
		}
		
	}
	else
	{	
		// Shift to left
		for(i =0; i < (unsigned)(size+shiftCoeff);i++)
		{
			dest[(i)] = dest[(i-shiftCoeff)];
		}
		for(i =0; i < (unsigned) (-shiftCoeff);i++)
		{
			dest[(size+shiftCoeff+i)] = 0;
		}
	}
	return(true);
}
		
		
/*============================================================*/
/* Copy length elements from src, starting at position startSrc,*
 * to field dest, starting at position startDest */
bool
memcopy(const jvxData *src, jvxData *dest, 
	 unsigned int startSrc, unsigned int startDest, unsigned int length)
{
	//assert(startSrc+length <= src.size());
	//assert(startDest+length <= dest.size());
	
	for(unsigned int i = 0; i < length; i++)
		dest[startDest+i] = src[startSrc+i];
	return(true);
}		
		
		
	
