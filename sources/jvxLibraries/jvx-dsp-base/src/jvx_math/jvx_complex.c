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

#include "jvx_math/jvx_complex.h"
#include <math.h>


jvxData
jvx_complex_arg(jvxData real_in, jvxData imag_in)
{
        jvxData angle;
	angle = JVX_SIGN(imag_in) * M_PI/2;
	if(real_in > 0)
	{
		angle = atan(imag_in/real_in);
	}
	if(real_in < 0)
	{
		angle = M_PI + atan(imag_in/real_in);
	}
	if(angle > M_PI)
	{
		angle -= 2*M_PI;
	}
        return angle;
}

void
jvx_complex_cart_2_polar(jvxData real_in, jvxData imag_in, jvxData* abs_out, jvxData* angle_out)
{
	*abs_out = sqrt(real_in * real_in + imag_in * imag_in);
	*angle_out = jvx_complex_arg(real_in, imag_in);
}

void
jvx_complex_polar_2_cart(jvxData abs_in, jvxData angle_in, jvxData* real_out, jvxData* imag_out)
{
	*real_out = abs_in * cos(angle_in);
	*imag_out = abs_in * sin(angle_in);
}

void
jvx_complex_multiply(jvxDataCplx in1, jvxDataCplx in2, jvxDataCplx* out)
{
	out->re = in1.re * in2.re - in1.im * in2.im;
	out->im = in1.im * in2.re + in1.re * in2.im;
}

void
jvx_complex_multiply_conj1(jvxDataCplx in1, jvxDataCplx in2, jvxDataCplx* out)
{
	out->re = in1.re * in2.re + in1.im * in2.im;
	out->im = in1.re * in2.im - in1.im * in2.re;
}

void
jvx_complex_square_of_magnitude(jvxDataCplx in, jvxData * out)
{
	*out = in.re * in.re + in.im * in.im;
}
