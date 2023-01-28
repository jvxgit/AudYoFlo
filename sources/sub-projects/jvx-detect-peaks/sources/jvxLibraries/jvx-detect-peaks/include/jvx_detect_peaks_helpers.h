#ifndef __JVX_DETECT_PEAK_HELPERS_H__
#define __JVX_DETECT_PEAK_HELPERS_H__

//#include "jvx_detect_peaks_bdx1.h"
#include "jvx_detect_peaks.h"

JVX_DSP_LIB_BEGIN

void findBestPosition(jvx_detect_peaks_io_per_evenodd* pPosis, jvxInt32* thisPosition_p,
					  jvxInt32* previousPosition_p, jvxCBool* noResultFound_p, jvxData deviateGainFactor,
					  jvxData minDeviateFactor, jvxInt32* forbidden, jvxSize numForbidden,
					  jvxInt16* idxSelectInSearchResult_p, jvxData* this_qual);

void findCenterOfGravity(jvxSize idxInt, jvxData* cog_p, jvxSize leftrightView, jvxSize minIdx, jvxSize maxIdx, jvxData* data);

void findCenterOfGravity_exp(jvxSize idxInt, jvxData* cog, jvxSize leftrightView, jvxSize minIdx, jvxSize maxIdx, jvxData* data, jvxData exp);

void postprocessMainPositions(jvx_detect_peaks_io_per_evenodd* pPosis, jvx_detect_peaks_data_per_evenodd* data, jvxPostProcessSearchStartegy* strat, jvxInt32* posiMainPeaks, 
							  jvxInt16* indexSearchStruct, jvxInt16 numMainPeaks);

JVX_DSP_LIB_END

#endif