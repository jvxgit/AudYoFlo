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
#include "math.h"

JVX_DSP_LIB_BEGIN

//! complex argument
jvxData jvx_complex_arg(jvxData real_in, jvxData imag_in);

//! complex cartesian to polar conversion
void jvx_complex_cart_2_polar(jvxData real_in, jvxData imag_in, jvxData* abs_out, jvxData* angle_out);

//! complex polar to cartesian conversion
void jvx_complex_polar_2_cart(jvxData abs_in, jvxData angle_in, jvxData* real_out, jvxData* imag_out);

//! complex multiplication
void JVX_STATIC_INLINE
jvx_complex_multiply(jvxDataCplx in1, jvxDataCplx in2, jvxDataCplx* out)
{
	out->re = in1.re * in2.re - in1.im * in2.im;
	out->im = in1.im * in2.re + in1.re * in2.im;
}

void JVX_STATIC_INLINE
jvx_complex_multiply_n(jvxDataCplx* in1, jvxDataCplx* in2, jvxDataCplx* out, jvxSize n)
{
	for (jvxSize i = 0; i < n; i++)
	{
		jvx_complex_multiply(*in1, *in2, out);
		in1++;
		in2++;
		out++;
	}
}

void JVX_STATIC_INLINE
jvx_complex_multiply_i_n(jvxDataCplx* inout, jvxDataCplx* mult, jvxSize n)
{
	for (jvxSize i = 0; i < n; i++)
	{
		jvxDataCplx out;
		jvx_complex_multiply(*inout, *mult, &out);
		*inout = out;
		inout++;
		mult++;		
	}
}

void JVX_STATIC_INLINE
jvx_complex_multiply_real_n(jvxDataCplx* inout, jvxData* in2, jvxSize n)
{
	for (jvxSize i = 0; i < n; i++)
	{
		inout->re *= *in2;
		inout->im *= *in2;
		in2++;
		inout++;
	}
}

//! complex multiplication with conjugate first argument
void JVX_STATIC_INLINE
jvx_complex_multiply_conj1(jvxDataCplx in1, jvxDataCplx in2, jvxDataCplx* out)
{
	out->re = in1.re * in2.re + in1.im * in2.im;
	out->im = in1.re * in2.im - in1.im * in2.re;
}

void JVX_STATIC_INLINE
jvx_complex_multiply_conj1_n(jvxDataCplx* in1, jvxDataCplx* in2, jvxDataCplx* out, jvxSize n)
{
	for (jvxSize i = 0; i < n; i++)
	{
		jvx_complex_multiply_conj1(*in1, *in2, out);
		in1++;
		in2++;
		out++;
	}
}

//! square of magnitude
void JVX_STATIC_INLINE jvx_complex_square_of_magnitude(jvxDataCplx in, jvxData* out)
{
	*out = in.re * in.re + in.im * in.im;
}

//! square of magnitude
jvxData JVX_STATIC_INLINE jvx_complex_square_of_magnitude_i(jvxDataCplx in)
{
	jvxData out = in.re * in.re + in.im * in.im;
	return out;
}

void JVX_STATIC_INLINE jvx_complex_square_of_magnitude_n(jvxDataCplx* in, jvxData* out, jvxSize n)
{
	for (jvxSize i = 0; i < n; i++)
	{
		jvx_complex_square_of_magnitude(*in, out);
		in++;
		out++;
	}
}

jvxData JVX_STATIC_INLINE jvx_complex_square_of_magnitude_sum_n(jvxDataCplx* in, jvxSize n)
{
	jvxData out = 0;
	for (jvxSize i = 0; i < n; i++)
	{
		out += jvx_complex_square_of_magnitude_i(*in);
		in++;
	}
	return out;
}

jvxData JVX_STATIC_INLINE jvx_complex_square_of_magnitude_sum_fft_n(jvxDataCplx* in, jvxSize n, jvxData scaleFac)
{
	jvxData out = 0;
	out += jvx_complex_square_of_magnitude_i(*in);
	in++;
	for (jvxSize i = 1; i < n-1; i++)
	{
		out += 2* jvx_complex_square_of_magnitude_i(*in);
		in++;
	}
	out += jvx_complex_square_of_magnitude_i(*in);
	in++;
	out *= scaleFac;
	return out;
}

void JVX_STATIC_INLINE jvx_complex_square_of_magnitude_add_n(jvxDataCplx* in, jvxData* out, jvxSize n)
{
	for (jvxSize i = 0; i < n; i++)
	{
		jvxData tmp;
		jvx_complex_square_of_magnitude(*in, &tmp);
		*out += tmp;
		in++;
		out++;
	}
}

//! complex multiplication
jvxDataCplx JVX_STATIC_INLINE jvx_complex_multiply_i(jvxDataCplx in1, jvxDataCplx in2)
{
	jvxDataCplx out;
	out.re = in1.re * in2.re - in1.im * in2.im;
	out.im = in1.im * in2.re + in1.re * in2.im;
	return out;
}

//! complex multiplication with conjugate first argument
jvxDataCplx JVX_STATIC_INLINE jvx_complex_multiply_conj1_i(jvxDataCplx in1, jvxDataCplx in2)
{
	jvxDataCplx out;
	out.re = in1.re * in2.re + in1.im * in2.im;
	out.im = in1.re * in2.im - in1.im * in2.re;
	return out;
}

jvxDataCplx JVX_STATIC_INLINE jvx_complex_add_i(jvxDataCplx in1, jvxDataCplx in2)
{
	jvxDataCplx out;
	out.re = in1.re + in2.re;
	out.im = in1.im + in2.im;
	return out;
}

void JVX_STATIC_INLINE
jvx_complex_add_n(jvxDataCplx* in1, jvxDataCplx* in2, jvxDataCplx* out, jvxSize n)
{
	for (jvxSize i = 0; i < n; i++)
	{
		*out = jvx_complex_add_i(*in1, *in2);
		in1++;
		in2++;
		out++;
	}
}


jvxDataCplx JVX_STATIC_INLINE jvx_complex_sub_i(jvxDataCplx in1, jvxDataCplx in2)
{
	jvxDataCplx out;
	out.re = in1.re - in2.re;
	out.im = in1.im - in2.im;
	return out;
}

// https://mathworld.wolfram.com/ComplexDivision.html
jvxDataCplx JVX_STATIC_INLINE jvx_complex_div_i(jvxDataCplx in1, jvxDataCplx in2, jvxData epsAdd)
{
	jvxDataCplx out;
	jvxData den = in2.re * in2.re + in2.im * in2.im + epsAdd;
	out.re = (in1.re * in2.re + in1.im * in2.im) / den;
	out.im = (in1.im * in2.re - in1.re * in2.im) / den;
	return out;
}

jvxData JVX_STATIC_INLINE jvx_complex_abs_i(jvxDataCplx in)
{
	jvxData out;
	out = in.re * in.re;
	out += in.im * in.im;
	out = sqrt(out);
	return out;
}

jvxData JVX_STATIC_INLINE jvx_complex_abs2_i(jvxDataCplx in)
{
	jvxData out;
	out = in.re * in.re;
	out += in.im * in.im;	
	return out;
}
JVX_DSP_LIB_END

#endif
