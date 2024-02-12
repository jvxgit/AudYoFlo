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
 
 #ifndef __JVX_WINDOWS_H__
#define __JVX_WINDOWS_H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

//! Typedefs for window function
enum
{
	JVX_WINDOW_RECT = 0,
	JVX_WINDOW_HAMMING = 1,
	JVX_WINDOW_BLACKMAN = 2,
	JVX_WINDOW_FLATTOP_HANN = 3,
	JVX_WINDOW_SQRT_FLATTOP_HANN = 4,
	JVX_WINDOW_HANN = 5,
	JVX_WINDOW_SQRT_HANN = 6,
	JVX_WINDOW_HALF_HANN = 7,
	JVX_WINDOW_HALF_LINEAR = 8,
	JVX_WINDOW_HALF_HAMMING = 9,
	JVX_WINDOW_HAMMING_ASYM = 10,
    JVX_WINDOW_KAISER = 11,
	JVX_WINDOW_LIMIT
};
typedef jvxInt16 jvx_windowType;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// WINDOW FUNCTIONS WINDOW FUNCTIONS WINDOW FUNCTIONS WINDOW FUNCTIONS WINDOW FUNCTIONS WINDOW FUNCTIONS
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//! Compute a window function in the buffer passed. Second arg may be of importance or may not be
jvxDspBaseErrorType
jvx_compute_window(jvxData* ptrField, jvxInt32 fieldSize,
                   jvxData secondArgDbl, jvxData thirdArg,
                   jvx_windowType winType, jvxData* normalization);


jvxDspBaseErrorType jvx_hamming_window_it(jvxData* ptrField, jvxUInt32 fieldSize);
jvxDspBaseErrorType jvx_hann_window_it(jvxData* ptrField, jvxUInt32 fieldSize);
jvxDspBaseErrorType jvx_sqrt_hann_window_it(jvxData* ptrField, jvxUInt32 fieldSize);
jvxDspBaseErrorType jvx_blackman_window_it(jvxData* ptrField, jvxUInt32 fieldSize);


JVX_DSP_LIB_END

#endif
