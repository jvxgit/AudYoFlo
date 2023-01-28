/*															  		*
 *  ***********************************************************		*
 *  ***********************************************************		*
 *  																*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Copyright (C) Javox Solutions GmbH - All Rights Reserved		*
 *  Unauthorized copying of this file, via any medium is			*
 *  strictly prohibited. 											*
 *  																*
 *  Proprietary and confidential									*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Written by Hauke Krüger <hk@javox-solutions.com>, 2012-2020		*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Javox Solutions GmbH, Gallierstr. 33, 52074 Aachen				*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Contact: info@javox-solutions.com, www.javox-solutions.com		*
 *  																*
 *  ********************************************************   		*/

#include "jvx_windows/jvx_windows.h"
#include <math.h>
#include <assert.h>
#include "jvx_fft_tools/jvx_fft_tools.h"
#include "jvx_math/jvx_math_extensions.h"

// Kaiser Window computation, uses i0 (Bessel function order zero)
static void kaiser(jvxData* buffer, jvxInt32 N, jvxData alpha, jvxData buquet)
{
	int i;
	int halfWin = (int)ceil((jvxData)N/2.0);
	jvxData* ptrDbl = buffer + ((int)floor((jvxData)N/2.0));
	jvxData* ptrDbl_half = buffer + ((int)floor((jvxData)(N-1)/2.0));

	jvxData den = jvx_bessi0(alpha);

	for(i =0; i < halfWin; i++)
		{
			*ptrDbl = jvx_bessi0(alpha * sqrt(1- (4*(jvxData)i*(jvxData)i) / ((jvxData)(N-1)*(jvxData)(N-1))))/den;
			*ptrDbl = buquet + (1-buquet) * *ptrDbl;
			*ptrDbl_half-- = *ptrDbl++;
		}
}


jvxDspBaseErrorType
jvx_compute_window(jvxData* ptrField, jvxInt32 fieldSize,
                   jvxData secondArg, jvxData thirdArg,
                   jvx_windowType winType, jvxData* normalization)
{
	int i;
	int l_plus_ft;

	if(normalization)
		{
			*normalization = 1.0;
		}
	switch(winType)
		{
		case JVX_WINDOW_RECT:
			if(ptrField)
				{
					for(i = 0; i < fieldSize; i++)
						{
							ptrField[i] = 1.0;
						}
				}
			break;
		case JVX_WINDOW_HAMMING:
			if(ptrField)
				{
					for(i = 0; i < fieldSize; i++)
						{
							ptrField[i] = (jvxData)(0.54-0.46 * cos(2*M_PI*(jvxData)i/(jvxData)(fieldSize-1)));
						}
				}
			break;
		case JVX_WINDOW_BLACKMAN:
			if(ptrField)
				{
					for(i = 0; i < fieldSize; i++)
						{
							ptrField[i] = (jvxData)(0.42 - 0.5 * cos(2*M_PI*(jvxData)i/(fieldSize-1)) + 0.08 * cos(4*M_PI*(jvxData)i/(jvxData)(fieldSize-1)));
						}
				}
			break;
		case JVX_WINDOW_FLATTOP_HANN:
			// secondArg: frame shift(?)
			// this window (when used with the given shift) sums exactly up to one
			// but the "flat" part is actually very short
			// FIXME: design of a "real" flattop window is not yet possible
			l_plus_ft = (int)(ceil( ((jvxData)fieldSize/2.0) / secondArg) * secondArg);// l_plus_ft = ceil((M/2)/FS)*FS;
			if(ptrField)
				{
					for(i = 0; i < fieldSize - l_plus_ft; i++)
						{
							ptrField[i] =  (jvxData)(0.5*(1-cos(2*M_PI*(jvxData)i/(2.0*((jvxData)(fieldSize - l_plus_ft))-2.0))));
							ptrField[fieldSize - 1 - i] =  ptrField[i];
						}
					for(i = 0; i < ( 2*l_plus_ft - fieldSize); i++)
						{
							ptrField[i + fieldSize - l_plus_ft] = 1.0;
						}
				}
			if(normalization)
				{
					*normalization = (jvxData)
						(floor((ceil((jvxData)fieldSize/secondArg))/2.0) +
						 ((int)ceil((jvxData)fieldSize/secondArg))%2);

					*normalization = JVX_DATA_2_DATA(1.0/(*normalization));
				}
			break;
		case JVX_WINDOW_SQRT_FLATTOP_HANN:
			jvx_compute_window(ptrField, fieldSize, secondArg, thirdArg, JVX_WINDOW_FLATTOP_HANN, normalization);
			if(ptrField)
				{
					for(i = 0; i < fieldSize; i++)
						{
							ptrField[i] = sqrt(ptrField[i]);
						}
				}
			break;
		case JVX_WINDOW_HANN:
			if(ptrField)
				{
					for(i = 0; i < fieldSize; i++)
						{
							ptrField[i] = (jvxData)(0.5*(1-cos(2*M_PI*(jvxData)(i)/(jvxData)(fieldSize-1))));
						}
				}
			break;
		case JVX_WINDOW_SQRT_HANN:
			jvx_compute_window(ptrField, fieldSize, secondArg, thirdArg, JVX_WINDOW_HANN, normalization);
			if(ptrField)
				{
					for(i = 0; i < fieldSize; i++)
						{
							ptrField[i] = sqrt(ptrField[i]);
						}
				}
			break;
		case JVX_WINDOW_HALF_HANN:
			if(ptrField)
				{
					for(i = 0; i < fieldSize; i++)
						{
							ptrField[i] = (jvxData)(0.5*(1-cos(M_PI*(jvxData)(i+fieldSize-1)/(jvxData)(fieldSize-1))));
							//0.5*(1-cos(2*pi*(n+M-1)/(2*M-2)));
						}
				}
			break;
		case JVX_WINDOW_HALF_LINEAR:
			if(ptrField)
				{
					for(i = 0; i < fieldSize; i++)
						{
							ptrField[i] = (jvxData)(1.0 - (jvxData)i/(jvxData)fieldSize);
						}
				}
			break;
		case JVX_WINDOW_HALF_HAMMING:
			if(ptrField)
				{
					if(secondArg <= 0.0)
						{
							for(i = 0; i < fieldSize; i++)
								{
									ptrField[i] = (jvxData)(0.54-0.46 * cos(2*M_PI*(jvxData)i/(2.0*(jvxData)(fieldSize-1))));
								}
						}
					else
						{
							// Reverse order for hamming window: the second half of window
							for(i = 0; i < fieldSize; i++)
								{
									ptrField[fieldSize - 1 - i] = (jvxData)(0.54-0.46 * cos(2*M_PI*(jvxData)i/(2.0*(jvxData)(fieldSize-1))));
								}
						}
				}
			break;
		case JVX_WINDOW_HAMMING_ASYM:
			if(ptrField)
				{
					jvxData midPosi = JVX_DATA_2_DATA((fieldSize-1.0)/2.0 + secondArg);
					int stopIdxLeft = (int)floor(midPosi);
					int stopIdxRight = (fieldSize-1) - (int)ceil(midPosi);
					jvxData midPosiM;

					for(i = 0; i <= stopIdxLeft; i++)
						{
							ptrField[i] = JVX_DATA_2_DATA(0.54 - 0.46 * cos(M_PI * (jvxData)i/midPosi));
						}

					midPosiM = JVX_DATA_2_DATA(fieldSize - 1.0 - midPosi);

					for(i=0; i <= stopIdxRight ; i++)
						{
							ptrField[fieldSize- 1 - i] = JVX_DATA_2_DATA(0.54 - 0.46 * cos(M_PI * (jvxData)i/midPosiM));
						}
				}
		case JVX_WINDOW_KAISER:
			kaiser(ptrField, fieldSize, secondArg, thirdArg);
			break;
		default:
			assert(0);
		}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_hamming_window_it(jvxData* ptrField, jvxUInt32 fieldSize)
{
    jvxUInt32 i = 0;
    if (!ptrField) return JVX_DSP_ERROR_INVALID_ARGUMENT;

    for(i = 0; i < fieldSize; i++) {
        ptrField[i] *= (jvxData)(0.54-0.46 * cos(2*M_PI*(jvxData)i/(jvxData)(fieldSize-1)));
    }

    return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_hann_window_it(jvxData* ptrField, jvxUInt32 fieldSize)
{
    jvxUInt32 i = 0;
    if (!ptrField) return JVX_DSP_ERROR_INVALID_ARGUMENT;

    for(i = 0; i < fieldSize; i++) {
        ptrField[i] *= (jvxData)(0.5*(1-cos(2*M_PI*(jvxData)(i)/(jvxData)(fieldSize-1))));
    }

    return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_sqrt_hann_window_it(jvxData* ptrField, jvxUInt32 fieldSize)
{
    jvxUInt32 i = 0;
    if (!ptrField) return JVX_DSP_ERROR_INVALID_ARGUMENT;

    for(i = 0; i < fieldSize; i++) {
        ptrField[i] *= sqrt(0.5*(1-cos(2*M_PI*(jvxData)(i)/(jvxData)(fieldSize-1))));
    }

    return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_blackman_window_it(jvxData* ptrField, jvxUInt32 fieldSize)
{
    jvxUInt32 i = 0;
    if (!ptrField) return JVX_DSP_ERROR_INVALID_ARGUMENT;

    for(i = 0; i < fieldSize; i++) {
        ptrField[i] *= (jvxData)(0.42 - 0.5 * cos(2*M_PI*(jvxData)i/(fieldSize-1)) + 0.08 * cos(4*M_PI*(jvxData)i/(jvxData)(fieldSize-1)));
    }

    return JVX_DSP_NO_ERROR;
}