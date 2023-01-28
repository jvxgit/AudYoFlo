#include "jvx_detect_peaks_helpers.h"
#include "jvx_detect_peaks.h"
#include <math.h>

void postprocessMainPositions(jvx_detect_peaks_io_per_evenodd* pPosis, jvx_detect_peaks_data_per_evenodd* data, jvxPostProcessSearchStartegy* strat, jvxInt32* posiMainPeaks, 
							  jvxInt16* indexSearchStruct, jvxInt16 numMainPeaks)
{
	jvxSize i,j;
	jvxSize idxStart, idxStop, increment;
	jvxData oldVal, delta;

	jvxInt32 idxTreatThis;
	for(i = 0; i < numMainPeaks; i++)
	{
		idxTreatThis = -1;
		jvxInt32 minVal = INT_MAX;

		for(j = 0; j < numMainPeaks; j++)
		{
			if(indexSearchStruct[j] >= 0)
			{
				if(posiMainPeaks[j] < minVal)
				{
					minVal = posiMainPeaks[j];
					idxTreatThis = (jvxInt32)j;
				}
			}
		}
		if(idxTreatThis >= 0)
		{
			switch(strat[i])
			{
			case JVX_POSTPROCESS_STRAT_LEFT_TO_RIGHT:
				idxStart = pPosis->idxStartArea[indexSearchStruct[idxTreatThis]];
				idxStop = pPosis->idxStopArea[indexSearchStruct[idxTreatThis]];
				increment = 1;
				break;
			case JVX_POSTPROCESS_STRAT_RIGHT_TO_LEFT:
				idxStart = pPosis->idxStopArea[indexSearchStruct[idxTreatThis]];
				idxStop = pPosis->idxStartArea[indexSearchStruct[idxTreatThis]];
				increment = -1;
				break;
			default:
				idxStart = 0;
				idxStop = 0;
				increment = 0;
				break;
			}

			oldVal = data->theData[idxStart];
			for(j = idxStart + increment; j != idxStop; j+=increment)
			{
				delta = data->theData[j] - oldVal;
				if(delta < 0)
				{
					posiMainPeaks[idxTreatThis] = (jvxInt32)j - (jvxInt32)increment;
					break;
				}
			}
			indexSearchStruct[idxTreatThis] = -1;
		}
	}
}

void 
findBestPosition(jvx_detect_peaks_io_per_evenodd* pPosis, jvxInt32* thisPosition_p, 
					  jvxInt32* previousPosition_p, jvxCBool* noResultFound_p, jvxData deviateGainFactor, 
					  jvxData minDeviateFactor, jvxInt32* forbidden, jvxSize numForbidden,
					  jvxInt16* idxSelectInSearchResult_p, jvxData* this_qual)
{
	jvxSize i,j;
	jvxData maxVal = -FLT_MAX;
	*noResultFound_p = c_false;
	*idxSelectInSearchResult_p = -1;

	for(i = 0; i <pPosis->numberPositionsPeaksCurrent; i++)
	{
		jvxCBool isAForbiddenPosition = c_false;
		for(j = 0; j < numForbidden; j++)
		{
			if(pPosis->idxMaxAbs[i] == forbidden[j])
			{
				isAForbiddenPosition = c_true;
				break;
			}
		}
		if(!isAForbiddenPosition)
		{
			jvxData factor = 1.0;
			jvxData compareVal = 0;
			jvxData tmp = 0;

			// Compute weighting factor for deviation of ref pulse from previous position
			if(*previousPosition_p < 0)
			{
				factor = 1.0;
			}
			else
			{
				tmp = fabs((jvxData)(pPosis->idxMaxAbs[i] - *previousPosition_p));
				tmp *= deviateGainFactor;
				factor = factor - tmp;
				factor = JVX_MAX(factor, minDeviateFactor);
			}


			compareVal = factor * pPosis->maxValueAll[i];
			if(compareVal > maxVal)
			{
				*idxSelectInSearchResult_p = (jvxInt32)i;
				maxVal = compareVal;
				if (this_qual)
				{
					*this_qual = pPosis->maxValueAboveMean[i];
				}
			}
		}
	}
	if(*idxSelectInSearchResult_p >= 0)
	{
		*thisPosition_p = pPosis->idxMaxAbs[*idxSelectInSearchResult_p];
		*previousPosition_p = *thisPosition_p;
	}
	else
	{
		//thisPosition = previousPosition;
		*thisPosition_p = -1;
		*noResultFound_p = c_true;
	}
}

void 
findCenterOfGravity(jvxSize idxInt, jvxData* cog_p, jvxSize leftrightView, jvxSize minIdx, jvxSize maxIdx, jvxData* data)
{
	// Compute center of gravity of peak			
	jvxSize i;
	jvxData cog_peak_num = 0.0;
	jvxData cog_peak_den = 0.0;

	jvxSize startIdx = idxInt;
	startIdx -= leftrightView;
	startIdx = JVX_MAX(startIdx, minIdx);

	jvxSize stopIdx = idxInt;
	stopIdx += leftrightView;
	stopIdx = JVX_MIN(maxIdx, stopIdx);

	jvxData tmp = 0;
	for(i = startIdx; i <= stopIdx; i++)
	{
		tmp = data[i] * 0.1;
		tmp = pow((jvxData)10.0, tmp); // lin = 10^(log/10)
		tmp *= tmp;
		cog_peak_num += tmp * (jvxData)i;
		cog_peak_den += tmp;
	}
	*cog_p = cog_peak_num/(cog_peak_den + 1e-8);
}

void 
findCenterOfGravity_exp(jvxSize idxInt, jvxData* cog_p, jvxSize leftrightView, jvxSize minIdx, jvxSize maxIdx, jvxData* data, jvxData exp)
{
	// Compute center of gravity of peak			
	jvxSize i;
	jvxData cog_peak_num = 0.0;
	jvxData cog_peak_den = 0.0;

	jvxSize startIdx = idxInt;
	startIdx -= leftrightView;
	startIdx = JVX_MAX(startIdx, minIdx);

	jvxSize stopIdx = idxInt;
	stopIdx += leftrightView;
	stopIdx = JVX_MIN(maxIdx, stopIdx);

	jvxData tmp = 0;
	for(i = startIdx; i <= stopIdx; i++)
	{
		tmp = exp * data[i] * 0.1; // Consider exponent already in log domain
		tmp = pow((jvxData)10.0, tmp); // lin = 10^(exp*log/10)
		cog_peak_num += tmp * (jvxData)i;
		cog_peak_den += tmp;
	}
	*cog_p = cog_peak_num/(cog_peak_den + 1e-8);
}
