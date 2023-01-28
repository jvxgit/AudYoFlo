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

/**
   @file
   @brief Some basic math functions
 */

#ifndef __JVX_MATH_EXTENSIONS_H__
#define __JVX_MATH_EXTENSIONS_H__


#include "jvx_dsp_base.h"
#include <math.h>

JVX_DSP_LIB_BEGIN

typedef enum
{
	GAMMA_LIN = 0,
	GAMMA_LOG = 1
} gammaComputationType;


/** \brief Gamma function, linear  or logarithmic

    gamma() calculates the gamma function of input parameter x
    Implements the Lanczos-Approximation with 6 coefficients.
    Has an error magnitude of \f$10^{-14}\f$ compared to the Matlab
    gamma function implementation

    \param x input value
    \param mod set to GAMMA_LIN or GAMMA_LOG
    \return computed value
*/
jvxData jvx_gamma(jvxData x, gammaComputationType mod);


//! calculate the incomplete gamma function of input parameter x wrt. parameter a. The elements of a as well as x must be nonnegative.
jvxData jvx_gammaincP(jvxData x, jvxData a);


//! calculate the inverse incomplete gamma function of input parameter x wrt. parameter a
jvxData jvx_gammaincQ(jvxData x, jvxData a);


/** \brief Computes \f$10\cdot log10(x)\f$ for vector inputs
 */
void jvx_linSquared2LogdB(jvxData* bufInOut, int bufL);


/** \brief Computes \f$10^\frac{x}{10}\f$ for vector inputs
 */
void jvx_LogdB2linSquared(jvxData* bufInOut, int bufL);


//! Returns the Bessel function J0(x) for any real x.
jvxData jvx_bessj0(jvxData x);


//! Returns the Bessel function J1(x) for any real x.
jvxData jvx_bessj1(float x);


//! Returns the modified Bessel function I0(x) for any real x.
jvxData jvx_bessi0(jvxData x);


//! Returns the modified Bessel function K0(x) for positive real x.
jvxData jvx_bessk0(jvxData x);


//! Returns the modified Bessel function I1(x) for any real x.
jvxData jvx_bessi1(jvxData x);


//! Returns the modified Bessel function K1(x) for positive real x.
jvxData jvx_bessk1(jvxData x);


//! Evaluates the exponential integral En(x).
jvxDspBaseErrorType jvx_expintn(int n, jvxData x_in, jvxData* result_out );


//! Evaluates the exponential integral E1(x).
jvxDspBaseErrorType jvx_expint(jvxData x_in, jvxData* result_out );


//! Computes the exponential integral Ei(x) for x > 0.
jvxDspBaseErrorType jvx_ei(jvxData x_in, jvxData* res_out);


//! Evaluates the exponential integral for \f$x\geq -4\f$
jvxDspBaseErrorType jvx_ExponentialIntegral(jvxData x_in, jvxData* res_out);


//! greatest common divisor of a and b
jvxInt32 jvx_gcd(jvxInt32 a, jvxInt32 b);


//! sinc function: \f$\sin(x)/x\f$
jvxData jvx_si (jvxData x);


//! Returns the error function erf(x).
jvxData jvx_erf(jvxData x);


//! Returns the complementary error function erfc(x).
jvxData jvx_erfc(jvxData x);


//! Uniform random noise generator, idum should be initialized with negative long integer
jvxData jvx_rand_uniform(long *idum);


//! Gaussian random noise generator, idum should be initialized with negative long integer
jvxData jvx_rand_normal(long *idum);

//! Computes the square of the argument
jvxData jvx_sqr(jvxData x);

//! Computes the decibel value of the argument (power)
jvxData jvx_pow2db(jvxData value);

//! Computes the normal power value of a given decibel argument
jvxData jvx_db2pow(jvxData value);

//! Computes the decibel value of the argument (magnitude)
jvxData jvx_mag2db(jvxData value);

//! Computes the magnitude of the given decibel argument
jvxData jvx_db2mag(jvxData value);

JVX_DSP_LIB_END

#endif
