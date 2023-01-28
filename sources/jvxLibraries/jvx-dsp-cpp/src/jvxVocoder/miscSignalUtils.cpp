#include <math.h>
#include <string.h>
#include "jvxVocoder/miscSignalUtils.h"

jvxInt16
convSignalImpulse(jvxData* inputSignal, jvxData* impulseInput, jvxInt16 lVec, jvxData* resultVec)
{
	jvxInt16 i,j;
//	memset(resultVec, 0, sizeof(jvxData)*lVec);
	jvxData* ptrjvxData1 = resultVec;
	jvxData* ptrjvxData2 = inputSignal;
	jvxData* ptrjvxData3 = impulseInput;
	jvxData* ptrjvxData4;

	for(j = 0; j < lVec; j++)
	{
		*ptrjvxData1++ = *ptrjvxData2 * *ptrjvxData3++;
		//resultVec[j] =*inputSignal*impulseInput[j];
	}
	ptrjvxData1 = resultVec;
	for(i = 1; i < lVec; i++)
	{
		ptrjvxData3 = impulseInput;
		ptrjvxData4 = ++ptrjvxData1;
		ptrjvxData2++;
		for(j = i; j < lVec; j++)
		{
			*ptrjvxData4++ += *(ptrjvxData2) * *ptrjvxData3++;
			//resultVec[j]+=inputSignal[i]*impulseInput[j-i];
		}
	}
	return(0);
}

/**
 * IIR filter with fw and bw coefficients. Tried to optimize performance by 
 * avoidance of array subscripting. 2.can. is the form that MATLAB does not use 
 * in filter function!
 *///========================================================================
jvxInt16 
iirFilter_2can_fwbw(jvxData* stateBuf, jvxData* fwBuffer, jvxData* bwBuffer, 
						   jvxInt16 numFilterCoeffs, jvxData* inputBuf, jvxData* outputBuf, jvxInt16 lengthSignal)
{	
	// A whole lot of variables for better performance
	jvxInt16 i,j;
	jvxData tmpInput;
	jvxData* stateBufCopyTo;
	jvxData*	stateBufCopyFrom;
	jvxData* coeffBufferBW;
	jvxData* coeffBufferFW;

	// Process sample by sample
	for(i = 0; i< lengthSignal; i++)
	{
		tmpInput = 0;
		*outputBuf = 0;

		stateBufCopyTo = &stateBuf[numFilterCoeffs-2];
		stateBufCopyFrom = &stateBuf[numFilterCoeffs-3];
		coeffBufferBW = &bwBuffer[numFilterCoeffs-1];
		coeffBufferFW = &fwBuffer[numFilterCoeffs-1];

		for(j = numFilterCoeffs-2; j>0; j--)
		{
			*outputBuf += *stateBufCopyTo * *(coeffBufferFW--);//stateBuf[j]*fwBuffer[j+1];
			tmpInput -= *stateBufCopyTo * *(coeffBufferBW--);//stateBuf[j]*bwBuffer[j+1];
			*(stateBufCopyTo--) = *(stateBufCopyFrom--);//stateBuf[j]=stateBuf[j-1];
		}
		*outputBuf += *stateBuf * *(coeffBufferFW);//fwBuffer[1];
		tmpInput -= *stateBuf * *(coeffBufferBW);//bwBuffer[1];
			
		tmpInput += *(inputBuf++);
		*stateBuf = 1/(*bwBuffer)* tmpInput;
		*(outputBuf++) += *stateBuf * *fwBuffer;			
	}
	return(0);
}
 
/**
 * IIR filter with fw and bw coefficients. Tried to optimize performance by 
 * avoidance of array subscripting. 2.can. is the form that MATLAB does not use 
 * in filter function!
 *///========================================================================
jvxInt16 
iirFilter_2can_fwbw(jvxData* stateBuf, jvxData* fwBuffer, jvxData* bwBuffer, 
					jvxInt16 numFilterCoeffs, jvxData inputSample, jvxData* outputSample)
{	
	// A whole lot of variables for better performance
	jvxInt16 j;
	jvxData tmpInput;
	jvxData* stateBufCopyTo;
	jvxData*	stateBufCopyFrom;
	jvxData* coeffBufferBW;
	jvxData* coeffBufferFW;

	// Process sample by sample
	tmpInput = 0;
	*outputSample = 0;

	stateBufCopyTo = &stateBuf[numFilterCoeffs-2];
	stateBufCopyFrom = &stateBuf[numFilterCoeffs-3];
	coeffBufferBW = &bwBuffer[numFilterCoeffs-1];
	coeffBufferFW = &fwBuffer[numFilterCoeffs-1];

	for(j = numFilterCoeffs-2; j>0; j--)
	{
		*outputSample += *stateBufCopyTo * *(coeffBufferFW--);//stateBuf[j]*fwBuffer[j+1];
		tmpInput -= *stateBufCopyTo * *(coeffBufferBW--);//stateBuf[j]*bwBuffer[j+1];
		*(stateBufCopyTo--) = *(stateBufCopyFrom--);//stateBuf[j]=stateBuf[j-1];
	}
	*outputSample += *stateBuf * *(coeffBufferFW);//fwBuffer[1];
	tmpInput -= *stateBuf * *(coeffBufferBW);//bwBuffer[1];

	tmpInput += (inputSample);
	*stateBuf = 1/(*bwBuffer)* tmpInput;
	*(outputSample) += *stateBuf * *fwBuffer;			
	return(0);
}
 
/**
 * IIR filter with fw and bw coefficients. Tried to optimize performance by 
 * avoidance of array subscripting. 1.can. is the form that MATLAB uses
 * in filter function!
 *///========================================================================
jvxInt16 iirFilter_1can_fwbw(jvxData* stateBuf, jvxData* fwBuffer, jvxData* bwBuffer, 
						   jvxInt16 numFilterCoeffs, jvxData* inputBuf, jvxData* outputBuf, jvxInt16 lengthSignal)
{
	jvxInt16 i,j;
	jvxData* multBufferFW;
	jvxData* multBufferBW;
	jvxData* stateBufferRead;
	jvxData* stateBufferWrite;

	for(i = 0; i< lengthSignal; i++)
	{

		multBufferFW = fwBuffer;
		multBufferBW = bwBuffer;
		stateBufferRead = stateBuf;
		stateBufferWrite = stateBuf;

		*(outputBuf) = (*(inputBuf) * *(multBufferFW++) + *(stateBufferRead++)) * 1/(*(multBufferBW++));

		for(j = 1; j < numFilterCoeffs-1; j++)
		{
			*(stateBufferWrite++) = *(stateBufferRead++) + *inputBuf * *(multBufferFW++)
				- *(outputBuf) * *(multBufferBW++);
		}		
		*(stateBufferWrite) = *inputBuf * *(multBufferFW) -  *(outputBuf++) * *(multBufferBW++);

		inputBuf++;
	}

	return(0);
}
 
/**
 * IIR filter with fw and bw coefficients. Tried to optimize performance by 
 * avoidance of array subscripting. 1.can. is the form that MATLAB uses
 * in filter function!
 *///========================================================================
jvxInt16 iirFilter_1can_fwbw(jvxData* stateBuf, jvxData* fwBuffer, jvxData* bwBuffer, 
						   jvxInt16 numFilterCoeffs, jvxData inputSample, jvxData* outputSample)
{
	jvxInt16 j;
	jvxData* multBufferFW;
	jvxData* multBufferBW;
	jvxData* stateBufferRead;
	jvxData* stateBufferWrite;


	multBufferFW = fwBuffer;
	multBufferBW = bwBuffer;
	stateBufferRead = stateBuf;
	stateBufferWrite = stateBuf;

	*outputSample = (inputSample * *(multBufferFW++) + *(stateBufferRead++)) * 1/(*(multBufferBW++));

	for(j = 1; j < numFilterCoeffs-1; j++)
	{
		*(stateBufferWrite++) = *(stateBufferRead++) + inputSample * *(multBufferFW++)
			- *outputSample * *(multBufferBW++);
	}		
	*(stateBufferWrite) = inputSample * *(multBufferFW) -  *outputSample * *(multBufferBW++);
	return(0);
}

/**
 * IIR filter with only bw coefficients. Tried to optimize performance by 
 * avoidance of array subscripting.
 * Implemented NOT equal to MATLAB filter function.
 *///========================================================================
jvxInt16
iirFilter_2can_bw(jvxData* stateBuf, jvxData* bwBuffer, 
						   jvxInt16 numFilterCoeffs, jvxData* inputBuf, jvxData* outputBuf, jvxInt16 lengthSignal)
{	
	jvxInt16 i,j;
	jvxData tmpInput;
	jvxData* stateBufCopyTo;
	jvxData*	stateBufCopyFrom;
	jvxData* coeffBufferBW;

	for(i = 0; i< lengthSignal; i++)
	{
		tmpInput = 0;
		*outputBuf = 0;
		stateBufCopyTo = &stateBuf[numFilterCoeffs-2];
		stateBufCopyFrom = &stateBuf[numFilterCoeffs-3];
		coeffBufferBW = &bwBuffer[numFilterCoeffs-1];

		for(j = numFilterCoeffs-2; j>0; j--)
		{
			tmpInput -= *(stateBufCopyTo) * *(coeffBufferBW--);
			*(stateBufCopyTo--) = *(stateBufCopyFrom--);
		}
		tmpInput += *(inputBuf++)- *stateBuf * *(coeffBufferBW);
		*stateBuf = 1/(*bwBuffer)* tmpInput;
		*(outputBuf++) += *stateBuf;			
	}
	return(0);
}

/**
 * IIR filter with only bw coefficients. Tried to optimize performance by 
 * avoidance of array subscripting.
 * Implemented equal to MATLAB filter function.
 *///========================================================================
jvxInt16 
iirFilter_1can_bw(jvxData* stateBuf, jvxData* bwBuffer, 
						   jvxInt16 numFilterCoeffs, jvxData* inputBuf, jvxData* outputBuf, jvxInt16 lengthSignal)
{
	jvxInt16 i, j;
	jvxData* multBufferBW;
	jvxData* stateBufferRead;
	jvxData* stateBufferWrite;

	for(i = 0; i< lengthSignal; i++)
	{
		multBufferBW = bwBuffer;
		stateBufferRead = stateBuf;
		stateBufferWrite = stateBuf;

		*(outputBuf) = (*(inputBuf) + *(stateBufferRead++)) * 1/(*(multBufferBW++));

		for(j = 1; j < numFilterCoeffs-1; j++)
		{
			*(stateBufferWrite++) = *(stateBufferRead++) - *(outputBuf) * *(multBufferBW++);
		}		
		*(stateBufferWrite) = -*(outputBuf++) * *(multBufferBW++);

		inputBuf++;
	}

	return(0);
}

/**
 * FIR filter (with fw coefficients only). Tried to optimize performance by 
 * avoidance of array subscripting.
 * Implemented NOT equal to MATLAB filter function.
 *///========================================================================
jvxInt16 
firFilter_2can(jvxData* stateBuf, jvxData* fwBuffer, 
						   jvxInt16 numFilterCoeffs, jvxData* inputBuf, jvxData* outputBuf, jvxInt16 lengthSignal)
{	
	jvxInt16 i,j;
	jvxData tmpInput;
	jvxData* stateBufCopyTo;
	jvxData*	stateBufCopyFrom;
	jvxData* coeffBufferFW;

	for(i = 0; i< lengthSignal; i++)
	{
		tmpInput = 0;
		*outputBuf = 0;

		stateBufCopyTo = &stateBuf[numFilterCoeffs-2];
		stateBufCopyFrom = &stateBuf[numFilterCoeffs-3];
		coeffBufferFW = &fwBuffer[numFilterCoeffs-1];

		for(j = numFilterCoeffs-2; j>0; j--)
		{
			*outputBuf += *stateBufCopyTo * *(coeffBufferFW--);//stateBuf[j]*fwBuffer[j+1];
			*(stateBufCopyTo--) = *(stateBufCopyFrom--);//stateBuf[j]=stateBuf[j-1];
		}
		*outputBuf += *stateBuf * *(coeffBufferFW);//fwBuffer[1];
		*stateBuf =  *(inputBuf++);
		*(outputBuf++) += *stateBuf * *fwBuffer;			
	}
	return(0);
}

/**
 * FIR filter (with fw coefficients only). Tried to optimize performance by 
 * avoidance of array subscripting.
 * Implemented equal to MATLAB filter function.
 *///========================================================================
jvxInt16
firFilter_1can(jvxData* stateBuf, jvxData* fwBuffer, 
						   jvxInt16 numFilterCoeffs, jvxData* inputBuf, jvxData* outputBuf, jvxInt16 lengthSignal)
{
	jvxInt16 i,j;
	//inputBuffer = (const jvxData*) inputStruct;
	//outputBuffer = (jvxData*) outputStruct;
	
	jvxData* multBuffer;
	jvxData* stateBufferRead;
	jvxData* stateBufferWrite;

	for(i = 0; i< lengthSignal; i++)
	{
		multBuffer = fwBuffer;
		stateBufferRead = stateBuf;
		stateBufferWrite = stateBuf;

		*(outputBuf++) = *(inputBuf) * *(multBuffer++) + *(stateBufferRead++);
		for(j = 1; j < numFilterCoeffs-1; j++)
		{
			*(stateBufferWrite++) = *(stateBufferRead++) + *inputBuf * *(multBuffer++);
		}		
		*(stateBufferWrite) = *inputBuf * *(multBuffer);

		inputBuf++;
	}
	return(0);
}

/**
 * Implementation of the realization of the autocorrelation function.
 *///=================================================================
jvxInt16
autocorrFunc(jvxData* in, jvxData* autocorrBuff, jvxInt16 lengthIn, jvxInt16 orderAutocorr)
{
	int i, ii;
	jvxData* ptr_x = NULL;
	jvxData* ptr_x_m = NULL;
	jvxData* ptrAc = NULL;

	ptrAc = autocorrBuff;
	for(i = 0; i < orderAutocorr; i++)
	{
		ptr_x = in;
		ptr_x_m = in+i;

		*ptrAc = 0.0;
		for(ii = 0; ii < (lengthIn-i); ii++)
		{
			//std::cout << "(FL)Index:" << ii << ", In1:" << *ptr_x << "; In2:" << *ptr_x_m << std::endl;

			*ptrAc += *ptr_x++ * *ptr_x_m++;
		}
		//std::cout << *ptrAc << std::cout << std::endl;
		ptrAc++;
	}
	return(0);
}

#define ABS(a) (a >= 0 ? a : -a)

jvxInt16
logarithmicQuantizer(jvxData* inputSample, jvxData* outputSample, jvxData scaleFac)
{
	jvxData temp;

	if(inputSample&&outputSample)
	{
		*outputSample = JVX_SIGN(*inputSample);
		temp = ABS(*inputSample);
		temp = log(temp+0.00000000001);
		temp *= scaleFac;
		temp = floor(temp+0.5);
		temp /= scaleFac;

		*outputSample *= exp(temp);
		return(0);
	}
	return(-1);
}
//