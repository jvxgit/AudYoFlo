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
 
 #ifndef __JVX_COMPLEX_H__
#define __JVX_COMPLEX_H__

#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

//! complex argument
jvxData jvx_complex_arg(jvxData real_in, jvxData imag_in);

//! complex cartesian to polar conversion
void jvx_complex_cart_2_polar(jvxData real_in, jvxData imag_in, jvxData* abs_out, jvxData* angle_out);

//! complex polar to cartesian conversion
void jvx_complex_polar_2_cart(jvxData abs_in, jvxData angle_in, jvxData* real_out, jvxData* imag_out);

//! complex multiplication
void jvx_complex_multiply(jvxDataCplx in1, jvxDataCplx in2, jvxDataCplx* out);

//! complex multiplication with conjugate first argument
void jvx_complex_multiply_conj1(jvxDataCplx in1, jvxDataCplx in2, jvxDataCplx* out);

//! square of magnitude
void jvx_complex_square_of_magnitude(jvxDataCplx in, jvxData* out);

JVX_DSP_LIB_END

#endif
