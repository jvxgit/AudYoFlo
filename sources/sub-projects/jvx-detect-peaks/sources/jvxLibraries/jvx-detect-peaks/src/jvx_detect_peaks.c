#include "jvx_dsp_base.h"
#include "jvx_detect_peaks.h"
#include "jvx_smooth_sliding_window.h"

typedef struct
{
	jvxHandle* theSmoother;
} jvx_detect_peaks_private;

jvxDspBaseErrorType
jvx_detect_peaks_init(jvxHandle** theHdl, jvxSize halfLengthSlidingWindow, jvxSize dimVector)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	if(theHdl)
	{
		jvx_detect_peaks_private* theHandle = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(theHandle, jvx_detect_peaks_private);
		memset(theHandle, 0, sizeof(jvx_detect_peaks_private));

		res = jvx_smooth_sliding_window_init(&theHandle->theSmoother, dimVector, halfLengthSlidingWindow);

		*theHdl = (jvxHandle*)theHandle;
	}
	else
	{
		res = JVX_DSP_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxDspBaseErrorType
jvx_detect_peaks_process_smooth(jvxHandle* theHdl, jvx_detect_peaks_data_per_evenodd* theIo)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	if(theHdl)
	{
		jvx_detect_peaks_private* theHandle = (jvx_detect_peaks_private*)theHdl;

		// Find the sliding window averaging function
		jvx_smooth_sliding_window_process(theHandle->theSmoother, theIo->theData, theIo->theDataSmoothed);
	}
	else
	{
		res = JVX_DSP_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxDspBaseErrorType
jvx_detect_peaks_process_incremental(jvx_detect_peaks_data_per_evenodd* theInput, jvxInt32* posi_p, 
		jvx_detect_peaks_parameters* params, jvxSize minIdx, jvxSize maxIdx)
{
	jvxSize idxStart;
	jvxSize idxStop;
	jvxSize i;
	if(*posi_p >= 0)
	{
		idxStart = *posi_p - params->lookleftright;
		idxStart = JVX_MAX(minIdx, idxStart);
		idxStop = *posi_p + params->lookleftright;
		idxStop = JVX_MIN(maxIdx, idxStop);

		jvxData maxVal = -FLT_MAX;
		for(i = idxStart; i <= idxStop; i++)
		{
			if(theInput->theData[i] > maxVal)
			{
				maxVal = theInput->theData[i];
				*posi_p = (jvxInt32)i;
			}
		}
	}
	return(JVX_DSP_NO_ERROR);
}

JVX_STATIC_INLINE void jvx_post_process_area(jvx_detect_peaks_io_per_evenodd* theOutput, 
								  jvx_detect_peaks_parameters* params,
								jvxData* maxValSectionAboveMean_p, jvxData* maxValSectionAbs_p, 
								int* idxStartSection_p, int* idxStopSection_p, 
								int* idxMaxSectionAboveMean_p, int* idxMaxSectionAbs_p,
								jvxInt16* numPeaksCmpAll_p, jvxInt16* numPeaksCmpArea_p)
{
	jvxSize j;
	int idxReplace = -1;
	int idxStopMax = -1;
	jvxData minValCurrent = +FLT_MAX;

	// Find if there is another peaks very close to current peak
	for(j = 0; j< *numPeaksCmpAll_p; j++)
	{
		if((theOutput->idxStopArea[j] + params->minProximity) > *idxStartSection_p)
		{
			idxStopMax = (int)j;
			break;
		}
	}

	if(idxStopMax >= 0)
	{
		// Yes, there is one, replace this one if the new peak is higher
		if(theOutput->maxValueAboveMean[idxStopMax] < *maxValSectionAboveMean_p)
		{
			theOutput->maxValueAboveMean[idxStopMax] = *maxValSectionAboveMean_p;
			theOutput->maxValueAll[idxStopMax] = *maxValSectionAbs_p;
			theOutput->idxStartArea[idxStopMax] = *idxStartSection_p;
			theOutput->idxStopArea[idxStopMax] = *idxStopSection_p;
			theOutput->idxMaxAboveMean[idxStopMax] = *idxMaxSectionAboveMean_p;
			theOutput->idxMaxAbs[idxStopMax] = *idxMaxSectionAbs_p;
		}
		// else: stick with the old on, discard the current one
	}
	else
	{
		if(*numPeaksCmpAll_p < theOutput->numberPositionsPeaksMax)
		{
			theOutput->maxValueAboveMean[*numPeaksCmpAll_p] = *maxValSectionAboveMean_p;
			theOutput->maxValueAll[*numPeaksCmpAll_p] = *maxValSectionAbs_p;
			theOutput->idxStartArea[*numPeaksCmpAll_p] = *idxStartSection_p;
			theOutput->idxStopArea[*numPeaksCmpAll_p] = *idxStopSection_p;
			theOutput->idxMaxAboveMean[*numPeaksCmpAll_p] = *idxMaxSectionAboveMean_p;
			theOutput->idxMaxAbs[*numPeaksCmpAll_p] = *idxMaxSectionAbs_p;
			(*numPeaksCmpAll_p)++;
		}
		else
		{
			// Find current min val
			for(j = 0; j< theOutput->numberPositionsPeaksMax; j++)
			{
				if(theOutput->maxValueAboveMean[j] < minValCurrent)
				{
					minValCurrent = theOutput->maxValueAboveMean[j];
					idxReplace = (int)j;
				}
			}

			if(*maxValSectionAboveMean_p > minValCurrent)
			{
				if(idxReplace >= 0) // <- need this check if theOutput->numberPositionsPeaksMax is 0
				{
					theOutput->maxValueAboveMean[idxReplace] = *maxValSectionAboveMean_p;
					theOutput->maxValueAll[idxReplace] = *maxValSectionAbs_p;
					theOutput->idxStartArea[idxReplace] = *idxStartSection_p;
					theOutput->idxStopArea[idxReplace] = *idxStopSection_p;
					theOutput->idxMaxAboveMean[idxReplace] = *idxMaxSectionAboveMean_p;
					theOutput->idxMaxAbs[idxReplace] = *idxMaxSectionAbs_p;
				}
			}
		}
		(*numPeaksCmpArea_p)++;
	}
}

jvxDspBaseErrorType
jvx_detect_peaks_process_full(jvxHandle* theHdl, jvx_detect_peaks_io_per_evenodd* theOutput, 
							  jvx_detect_peaks_data_per_evenodd* theInput, 
							  jvxSize* offsetStart, jvxSize* offsetStop, 
							  jvxSize* numPulsesPerArea,
							  jvxSize numAreas, jvx_detect_peaks_parameters* params)
{
	jvxSize i,k;
	//jvxData avrgOut = 0;
	jvxData accu1 = 0;
	jvxData accu1p = 0;
	//jvxSize cntReadValue = 0;
	//jvxSize cntStore = 0;
	//jvxSize cntReadTable = 0;
	//jvxSize cntRemove = 0;
	//jvxData* ptrStore = this->runtime.avrgOut;
	//jvxSize cntPosisInore = 0;
	//jvxData maxSearchAllFirst = -FLT_MAX;
	//jvxData maxSearchAllSecond = -FLT_MAX;
	jvxInt16 numPeaksCmpAll = 0;
	jvxInt16 numPeaksCmpArea = 0;

	if(theHdl)
	{
		//jvx_detect_peaks_private* theHandle = (jvx_detect_peaks_private*)theHdl;

		/* No longer needed as the number of found peaks is returned 
		// Reset the search vector
		for(i = 0; i< theOutput->numberPositionsPeaksMax; i++)
		{
			theOutput->idxMaxAboveMean[i] = -1;
			theOutput->maxValueAboveMean[i] = -1;
			theOutput->maxValueAll[i] = -1;
			theOutput->idxStartArea[i] = -1;
			theOutput->idxStopArea[i] = -1;
			theOutput->idxMaxAboveMean[i] = -1;
			theOutput->idxMaxAbs[i] = -1;
		}*/

		// Step II: The actual evaluation
		jvxData accuPrev = +1; // <- by using a positive value here, we will EXCLUDE the first peak (which is there always)
	
		jvxData maxValSectionAboveMean = -1;
		jvxData maxValSectionAbs = -1;
		int idxStartSection = -1;
		int idxMaxSectionAboveMean = -1;
		int idxMaxSectionAbs = -1;
		int idxStopSection = 0;
		//int idxReplace = -1;
		//jvxData minValCurrent = +FLT_MAX;
		//jvxData maxValCurrent = -FLT_MAX;
		//jvxData maxValCurrentSecond = -FLT_MAX;

		// Here, we got the actual evaluation
		if(params->external_weights)
		{
			for(k = 0; k < numAreas; k++)
			{
				numPeaksCmpArea = 0;
				jvxSize start = offsetStart[k];
				jvxSize stop = offsetStop[k];
				for(i = start; i <= stop; i++)
				{
					accu1p = theInput->theData[i];

					// Calculate how much the current peak i higher than the average
					accu1 = (accu1p - theInput->theDataSmoothed[i] - params->corridor_sensitivity);

					// Weight with weighting function
					accu1 *= params->external_weights[i];

					// If positive means higher than average
					if( accu1 >= 0)
					{
						// If we have a sign change, this indicates the start of a peak "section"
						if(accu1 * accuPrev <= 0)
						{
							// Start this section here
							maxValSectionAboveMean = accu1;
							maxValSectionAbs = theInput->theData[i];
							idxMaxSectionAboveMean = (int)i;
							idxMaxSectionAbs = (int)i;

							idxStartSection = (int)i - (int)params->dist_left;
							idxStopSection = -1;
						}
						else
						{
							if(idxStopSection == -1)
							{
								// If local maximum (with mean subtracted
								if(accu1 > maxValSectionAboveMean )
								{
									maxValSectionAboveMean = accu1;
									idxMaxSectionAboveMean = (int)i;
								}
								// If local maximum (with mean subtracted
								if(accu1p > maxValSectionAbs )
								{
									maxValSectionAbs = accu1p;
									idxMaxSectionAbs = (int)i;
								}
							}
						}
					}
					else
					{
						if(idxStopSection == -1)
						{

							idxStopSection = (int)i - 1 + (int)params->dist_right;

							jvx_post_process_area(theOutput, params,
								&maxValSectionAboveMean, &maxValSectionAbs, &idxStartSection,
								&idxStopSection, &idxMaxSectionAboveMean, &idxMaxSectionAbs,
								&numPeaksCmpAll, &numPeaksCmpArea);
						}
					}
					accuPrev = accu1;
				}

				if(idxStopSection == -1)
				{

					idxStopSection = (int)i - 1 + (int)params->dist_right;;

					jvx_post_process_area(theOutput, params,
						&maxValSectionAboveMean, &maxValSectionAbs, &idxStartSection,
						&idxStopSection, &idxMaxSectionAboveMean, &idxMaxSectionAbs,
						&numPeaksCmpAll, &numPeaksCmpArea);
				}
				//maxValCurrent = -FLT_MAX;
				//maxValCurrentSecond = -FLT_MAX;

				if(numPulsesPerArea)
				{
					numPulsesPerArea[k] = numPeaksCmpArea;
				}
				//numPeaksCmpAll += numPeaksCmpArea; <- no, value has been incremented before
			} // for(k = 0; k < numAreas; k++)
		}
		else
		{
			for(k = 0; k < numAreas; k++)
			{
				numPeaksCmpArea = 0;
				jvxSize start = offsetStart[k];
				jvxSize stop = offsetStop[k];
				for(i = start; i <= stop; i++)
				{
					accu1p = theInput->theData[i];

					// Calculate how much the current peak i higher than the average
					accu1 = (accu1p - theInput->theDataSmoothed[i] - params->corridor_sensitivity);

					// If positive means higher than average
					if( accu1 >= 0)
					{
						// If we have a sign change, this indicates the start of a peak "section"
						if(accu1 * accuPrev <= 0)
						{
							// Start this section here
							maxValSectionAboveMean = accu1;
							maxValSectionAbs = theInput->theData[i];
							idxMaxSectionAboveMean = (int)i;
							idxMaxSectionAbs = (int)i;

							idxStartSection = (int)i - (int)params->dist_left;
							idxStopSection = -1;
						}
						else
						{
							if(idxStopSection == -1)
							{
								// If local maximum (with mean subtracted
								if(accu1 > maxValSectionAboveMean )
								{
									maxValSectionAboveMean = accu1;
									idxMaxSectionAboveMean = (int)i;
								}
								// If local maximum (with mean subtracted
								if(accu1p > maxValSectionAbs )
								{
									maxValSectionAbs = accu1p;
									idxMaxSectionAbs = (int)i;
								}
							}
						}
					}
					else
					{
						if(idxStopSection == -1)
						{
							idxStopSection = (int)i - 1 + (int)params->dist_right;

							jvx_post_process_area(theOutput, params,
								&maxValSectionAboveMean, &maxValSectionAbs, &idxStartSection,
								&idxStopSection, &idxMaxSectionAboveMean, &idxMaxSectionAbs,
								&numPeaksCmpAll, &numPeaksCmpArea);	
						}
					}
					accuPrev = accu1;
				}

				if(idxStopSection == -1)
				{

					idxStopSection = (int)i - 1 + (int)params->dist_right;

					jvx_post_process_area(theOutput, params,
						&maxValSectionAboveMean, &maxValSectionAbs, &idxStartSection,
						&idxStopSection, &idxMaxSectionAboveMean, &idxMaxSectionAbs,
						&numPeaksCmpAll, &numPeaksCmpArea);			
				}
				//maxValCurrent = -FLT_MAX;
				//maxValCurrentSecond = -FLT_MAX;

				if(numPulsesPerArea)
				{
					numPulsesPerArea[k] = numPeaksCmpArea;
				}
			}	// for(k = 0; k < numAreas; k++)		
		}
		theOutput->numberPositionsPeaksCurrent = numPeaksCmpAll;

		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType
jvx_detect_peaks_process_simple(jvxHandle* theHdl, jvx_detect_peaks_io_per_evenodd* theOutput, 
							  jvx_detect_peaks_data_per_evenodd* theInput, jvxSize* offsetStart, jvxSize* offsetStop, 
							  jvxSize numAreas, jvx_detect_peaks_parameters* params)
{
	if(theHdl)
	{
		//jvx_detect_peaks_private* theHandle = (jvx_detect_peaks_private*)theHdl;
	
		jvxSize i,j,k;
		//jvxData avrgOut = 0;
		//jvxData accu1 = 0;
		//jvxData accu1p = 0;
		//jvxSize cntReadValue = 0;
		//jvxSize cntStore = 0;
		//jvxSize cntReadTable = 0;
		//jvxSize cntRemove = 0;
		//jvxSize cntPosisInore = 0;
		//jvxData maxSearchAllFirst = -FLT_MAX;
		//jvxData maxSearchAllSecond = -FLT_MAX;
		jvxData maxSearchAll;	
		int idxMax;

		for(k = 0; k < numAreas; k++)
		{
			jvxSize start = offsetStart[k];
			jvxSize stop = offsetStop[k];
			for(i = 0; i < theOutput->numberPositionsPeaksMax; i++)
			{
				if(theOutput->idxMaxAbs[i] >= 0)
				{
					int offsetStartLoc =theOutput-> idxMaxAbs[i] - (int)params->lookleftright;
					offsetStartLoc = (offsetStartLoc >= start) ? offsetStartLoc:(int)start;
					int offsetStopLoc = theOutput->idxMaxAbs[i] + (int)params->lookleftright;
					offsetStopLoc = (offsetStopLoc <= stop) ? offsetStopLoc : (int)stop;
					maxSearchAll = -FLT_MAX;
					idxMax = -1;
					for(j = offsetStartLoc; j < offsetStopLoc; j++)
					{
						if(theInput->theData[j] >= maxSearchAll)
						{
							idxMax = (int)j;
							maxSearchAll = theInput->theData[j];
						}
					}
					int delta = idxMax - theOutput->idxMaxAbs[i];
					theOutput->idxStartArea[i] += delta;
					theOutput->idxStopArea[i] += delta;
					theOutput->idxMaxAbs[i] += delta;
					theOutput->idxMaxAboveMean[i] += delta;
					theOutput->maxValueAll[i] = maxSearchAll;
				}
			}
		}
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType
jvx_detect_peaks_terminate(jvxHandle* theHdl)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	if(theHdl)
	{
		jvx_detect_peaks_private* theHandle = (jvx_detect_peaks_private*)theHdl;

		res = jvx_smooth_sliding_window_terminate(theHandle->theSmoother);

		JVX_DSP_SAFE_DELETE_OBJECT(theHandle);
	}
	else
	{
		res = JVX_DSP_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}
