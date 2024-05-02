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

#include "jvx_math/jvx_math_extensions.h"

#include <assert.h>
#include <stdlib.h>

////
// private functions
////

static jvxDspBaseErrorType gser(jvxData *gamser,
				    jvxData a,
				    jvxData x,
				    jvxData *gln)
{
#define ITMAX 100
#define EPS 3.0e-7
	int n;
	jvxData sum,del,ap;
	*gln = jvx_gamma(a, GAMMA_LOG);
	if (x <= 0.0)
	{
		if (x < 0.0)
		{
			return JVX_DSP_ERROR_INVALID_SETTING;
		}
		*gamser=0.0;
		return JVX_DSP_NO_ERROR;
	}
	else
	{
		ap=a;
		del=sum=1.0/a;
		for (n=1;n<=ITMAX;n++)
		{
			++ap;
			del *= x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPS)
			{
				*gamser=sum*exp(-x+a*log(x)-(*gln));
				return JVX_DSP_NO_ERROR;
			}
		}
		return JVX_DSP_ERROR_INVALID_SETTING;
	}
#undef ITMAX
#undef EPS
}

// Returns the incomplete gamma function Q(a, x) evaluated by its continued fraction representation
// as gammcf. Also returns ln gamma(a) as gln.
static jvxDspBaseErrorType gcf(jvxData *gammcf,
				   jvxData a,
				   jvxData x,
				   jvxData *gln)
{
#define ITMAX 100
#define EPS 3.0e-7
#define FPMIN 1.0e-30
	int i;
	jvxData an,b,c,d,del,h;
	*gln = jvx_gamma(a, GAMMA_LOG);

	//Set up for evaluating continued fraction by modified Lentzs method (5.2) with b0 = 0.
	b=x+1.0-a;
	c=1.0/FPMIN;
	d=1.0/b;
	h=d;
	for (i=1;i<=ITMAX;i++)
	{
		// Iterate to convergence.
		an = -i*(i-a);
		b += 2.0;
		d=an*d+b;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=b+an/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		del=d*c;
		h *= del;
		if (fabs(del-1.0) < EPS) break;
	}
	if (i > ITMAX)
	{
		return JVX_DSP_ERROR_INVALID_SETTING;
	}
	*gammcf=exp(-x+a*log(x)-(*gln))*h;
	return JVX_DSP_NO_ERROR;
#undef ITMAX
#undef EPS
#undef FPMIN
}


////
// public functions
////


jvxData
jvx_gamma(jvxData x, gammaComputationType mod)
{
	jvxData xx,y,tmp,ser;
	static const jvxData cof[6] =
	{
		JVX_DBL_2_DATA(76.18009172947146),
		JVX_DBL_2_DATA(-86.50532032941677),
		JVX_DBL_2_DATA(24.01409824083091),
		JVX_DBL_2_DATA(-1.231739572450155),
		JVX_DBL_2_DATA(0.1208650973866179e-2),
		JVX_DBL_2_DATA(-0.5395239384953e-5)
	};

	int j;

	y=xx=x;
	tmp=xx+JVX_DBL_2_DATA(5.5);
	tmp -= JVX_DBL_2_DATA((xx+0.5)*log(tmp));
	ser=JVX_DBL_2_DATA(1.000000000190015);
	for (j=0;j<=5;j++)
	{
		ser += cof[j]/++y;
	}
	if(mod == GAMMA_LOG)
	{
		return JVX_DBL_2_DATA(-tmp+log(2.5066282746310005*ser/xx));
	}
	else
	{
		return JVX_DBL_2_DATA(exp(-tmp+log(2.5066282746310005*ser/xx)));
	}
}




jvxData
jvx_gammaincP(jvxData x, jvxData a)
{
	jvxData gamser,gammcf,gln;

	if (x < 0.0 || a <= 0.0)
	{
		// std::cerr<< "Invalid arguments! Parameters x and a shall be non-zero values!";
		assert(0);
	}

	if (x < (a+1.0))			//Use the series representation
	{
		gser(&gamser,a,x,&gln);
		return gamser;
	}
	else						// Use the continued fraction representation
	{
		gcf(&gammcf,a,x,&gln);  // and take its complement.
		return JVX_DBL_2_DATA(1.0-gammcf);
	}
}

jvxData
jvx_gammaincQ(jvxData x, jvxData a)
{
	jvxData gamser,gammcf,gln;

	if (x < 0.0 || a <= 0.0)
	{
		//std::cerr<< "Invalid arguments! Parameters x and a shall be non-zero values!";
		assert(0);
	}

	if (x < (a+1.0))			//Use the series representation
	{
		gser(&gamser,a,x,&gln);
		return JVX_DBL_2_DATA(1.0-gamser);
	}
	else						// Use the continued fraction representation
	{
		gcf(&gammcf,a,x,&gln);  // and take its complement.
		return gammcf;
	}
}



void
jvx_linSquared2LogdB(jvxData* bufInOut, int bufL)
{
	int i;
	jvxData rega;

	for(i = 0; i < bufL; i++)
	{
		rega = *bufInOut;
		rega = log10(rega);
		rega *= 10.0;
		*bufInOut = rega;

		bufInOut++;
	}
}

void
jvx_LogdB2linSquared(jvxData* bufInOut, int bufL)
{
	int i;
	jvxData rega;

	for(i = 0; i < bufL; i++)
	{
		rega = *bufInOut;
		rega = (jvxData)(rega * 0.1); // 1/10
		rega = pow(10, rega);
		*bufInOut = rega;

		bufInOut++;
	}
}

jvxData
jvx_bessj0(jvxData x)
{
	jvxData ax,z;
	jvxData xx,y,ans,ans1,ans2; // Accumulate polynomials in jvxData precision.
	if ((ax=fabs(x)) < 8.0)
	{
		// Direct rational function fit.
		y=x*x;
		ans1 = 57568490574.0 + y * (-13362590354.0 + y*(651619640.7 + y * (-11214424.18 + y * (77392.33017 + y * (-184.9052456)))));
		ans2 = 57568490411.0 + y * (1029532985.0 + y * (9494680.718 + y * (59272.64853+y*(267.8532712+y*1.0))));
		ans=ans1/ans2;
	}
	else
	{
		// Fitting function (6.5.9).
		z = 8.0 / ax;
		y = z * z;
		xx = ax - 0.785398164;
		ans1 = 1.0 + y * (-0.1098628627e-2+y*(0.2734510407e-4 + y * (-0.2073370639e-5 + y * 0.2093887211e-6)));
		ans2 = -0.1562499995e-1 + y * (0.1430488765e-3 + y * (-0.6911147651e-5 + y * (0.7621095161e-6 - y * 0.934945152e-7)));
		ans = sqrt(0.636619772/ax)*(cos(xx)*ans1-z*sin(xx)*ans2);
	}
	return ans;
}

jvxData
jvx_bessj1(float x)
{
	jvxData ax,z;
	jvxData xx,y,ans,ans1,ans2; //Accumulate polynomials in jvxData precision.
	if ((ax=fabs(x)) < 8.0)
	{
		//Direct rational approximation.
		y=x*x;
		ans1 = x * (72362614232.0 + y*(-7895059235.0+y*(242396853.1	+ y * (-2972611.439 + y * (15704.48260+y*(-30.16036606))))));
		ans2 = 144725228442.0 + y * (2300535178.0+y*(18583304.74 + y * (99447.43394 + y * (376.9991397+y*1.0))));
		ans=ans1/ans2;
	}
	else
	{
		// Fitting function (6.5.9).
		z = 8.0 / ax;
		y = z * z;
		xx = ax - 2.356194491;
		ans1 = 1.0 + y * (0.183105e-2+y*(-0.3516396496e-4 + y * (0.2457520174e-5+y*(-0.240337019e-6))));
		ans2 = 0.04687499995 + y * (-0.2002690873e-3 + y * (0.8449199096e-5+y*(-0.88228987e-6 + y * 0.105787412e-6)));
		ans = sqrt(0.636619772/ax)*(cos(xx)*ans1-z*sin(xx)*ans2);
		if (x < 0.0)
			ans = -ans;
	}
	return ans;
}

jvxData jvx_bessi0(jvxData x)
{
#if 0

#define I_EPS   1.0e-40
        long    k       = 1;
        jvxData  k_fak   = 1.0;
        jvxData  x_k     = 1.0;
        jvxData  x_2     = sqr( x/2 );
        jvxData  d;
        jvxData  sum     = 1.0;

        do
        {
                k_fak *= k++;
                x_k   *= x_2;
                d = x_k / sqr( k_fak );
                sum += d;
        }
        while( d > I_EPS );
        return sum;
#undef I_EPS

#else // alternative implementation

	jvxData ax,ans;
	jvxData y; //Accumulate polynomials in jvxData precision.
	if ((ax=fabs(x)) < 3.75)
	{
		// Polynomial fit.
		y=x/3.75;
		y*=y;
		ans=1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492
			+y*(0.2659732+y*(0.360768e-1+y*0.45813e-2)))));
	}
	else
	{
		y=3.75/ax;
		ans=(exp(ax)/sqrt(ax))*(0.39894228+y*(0.1328592e-1
			+y*(0.225319e-2+y*(-0.157565e-2+y*(0.916281e-2
			+y*(-0.2057706e-1+y*(0.2635537e-1+y*(-0.1647633e-1
			+y*0.392377e-2))))))));
	}
	return ans;
#endif
}

jvxData
jvx_bessk0(jvxData x)
{
	jvxData y,ans; // Accumulate polynomials in jvxData precision.
	if (x <= 2.0)
	{
		// Polynomial fit.
		y=x*x/4.0;
		ans=(-log(x/2.0) * jvx_bessi0(x)) + (-0.57721566+y*(0.42278420
			+y*(0.23069756+y*(0.3488590e-1+y*(0.262698e-2
			+y*(0.10750e-3+y*0.74e-5))))));
	}
	else
	{
		y=2.0/x;
		ans=(exp(-x)/sqrt(x))*(1.25331414+y*(-0.7832358e-1
			+y*(0.2189568e-1+y*(-0.1062446e-1+y*(0.587872e-2
			+y*(-0.251540e-2+y*0.53208e-3))))));
	}
	return ans;
}

jvxData
jvx_bessi1(jvxData x)
{
	jvxData ax,ans;
	jvxData y; // Accumulate polynomials in jvxData precision.
	if ((ax=fabs(x)) < 3.75)
	{
		// Polynomial fit.
		y=x/3.75;
		y*=y;
		ans=ax*(0.5+y*(0.87890594+y*(0.51498869+y*(0.15084934
			+y*(0.2658733e-1+y*(0.301532e-2+y*0.32411e-3))))));
	}
	else
	{
		y=3.75/ax;
		ans=0.2282967e-1+y*(-0.2895312e-1+y*(0.1787654e-1
			-y*0.420059e-2));
		ans=0.39894228+y*(-0.3988024e-1+y*(-0.362018e-2
			+y*(0.163801e-2+y*(-0.1031555e-1+y*ans))));
		ans *= (exp(ax)/sqrt(ax));
	}
	return x < 0.0 ? -ans : ans;
}

jvxData jvx_bessk1(jvxData x)
{
	jvxData y,ans; // Accumulate polynomials in jvxData precision.
	if (x <= 2.0)
	{
		// Polynomial fit.
		y=x*x/4.0;
		ans=(log(x/2.0) * jvx_bessi1(x))+(1.0/x)*(1.0+y*(0.15443144
			+y*(-0.67278579+y*(-0.18156897+y*(-0.1919402e-1
			+y*(-0.110404e-2+y*(-0.4686e-4)))))));
	}
	else
	{
		y=2.0/x;
		ans=(exp(-x)/sqrt(x))*(1.25331414+y*(0.23498619
			+y*(-0.3655620e-1+y*(0.1504268e-1+y*(-0.780353e-2
			+y*(0.325614e-2+y*(-0.68245e-3)))))));
	}
	return ans;
}

// Evaluates the exponential integral En(x).
jvxDspBaseErrorType
jvx_expintn(int n, jvxData x_in, jvxData* result_out )
{
#define MAXIT 100
#define EULER 0.5772156649
#define FPMIN 1.0e-30
#define EPS 1.0e-7
	int i,ii,nm1;
	jvxData a,b,c,d,del,fact,h,psi,ans;
	nm1=n-1;
	if (n < 0 || x_in < 0.0 || (x_in==0.0 && (n==0 || n==1)))
	{
		return JVX_DSP_ERROR_INVALID_SETTING;
	}
	else
	{
		// Special case.
		if (n == 0)
		{
			ans=exp(-x_in)/x_in;
		}
		else
		{
			// Another special case.
			if (x_in == 0.0)
			{
				ans=1.0/nm1;
			}
			else
			{
				// Lentzs algorithm (§5.2).
				if (x_in > 1.0)
				{
					b=x_in+n;
					c=1.0/FPMIN;
					d=1.0/b;
					h=d;
					for (i=1;i<=MAXIT;i++)
					{
						a = -i*(nm1+i);
						b += 2.0;

						// Denominators cannot be zero.
						d=1.0/(a*d+b);
						c=b+a/c;
						del=c*d;
						h *= del;
						if (fabs(del-1.0) < EPS)
						{
							ans=h*exp(-x_in);
							*result_out = ans;
							return JVX_DSP_NO_ERROR;
						}
					}
					return JVX_DSP_ERROR_INTERNAL;
				}
				else
				{
					// Evaluate series.

					// Set first term.
					ans = (nm1!=0 ? 1.0/nm1 : -log(x_in)-EULER);
					fact=1.0;
					for (i=1;i<=MAXIT;i++)
					{
						fact *= -x_in/i;
						if (i != nm1)
						{
							del = -fact/(i-nm1);
						}
						else
						{
							// Compute psi(n).
							psi = -EULER;
							for (ii=1;ii<=nm1;ii++)
							{
								psi += 1.0/ii;
							}
							del=fact*(-log(x_in)+psi);
						}
						ans += del;
						if (fabs(del) < fabs(ans)*EPS)
						{
							*result_out = ans;
							return JVX_DSP_NO_ERROR;
						}
					}
					return JVX_DSP_ERROR_INTERNAL;
				}
			}
		}
	}
	*result_out = ans;
	return JVX_DSP_NO_ERROR;
#undef MAXIT
#undef EULER
#undef FPMIN
#undef EPS
}

jvxDspBaseErrorType jvx_expint(jvxData x_in, jvxData* result_out )
{
	return jvx_expintn(1, x_in, result_out);
}



// Computes the exponential integral Ei(x) for x > 0.
jvxDspBaseErrorType ei(jvxData x_in, jvxData* res_out)
{
#define EULER 0.57721566
#define MAXIT 100
#define FPMIN 1.0e-30
#define EPS 6.0e-8
	int k;
	jvxData fact,prev,sum,term;
	if (x_in <= 0.0)
	{
		return JVX_DSP_ERROR_INVALID_SETTING;
	}
	if (x_in < FPMIN)
	{
		// Special case: avoid failure of convergence
		*res_out = log(x_in)+EULER;
		return JVX_DSP_NO_ERROR;
	}
	if (x_in <= -log(EPS))
	{
		//test because of underflow.

		// Use power series.
		sum=0.0;
		fact=1.0;
		for (k=1;k<=MAXIT;k++)
		{
			fact *= x_in/k;
			term=fact/k;
			sum += term;
			if (term < EPS*sum)
				break;
		}
		if (k > MAXIT)
		{
			return JVX_DSP_ERROR_INTERNAL;
		}
		*res_out = sum+log(x_in)+EULER;
	}
	else
	{
		//Use asymptotic series.
		//  Start with second term.
		sum=0.0;
		term=1.0;
		for (k=1;k<=MAXIT;k++)
		{
			prev=term;
			term *= k/x_in;
			if (term < EPS)
				break;

			// Since final sum is greater than one, term itself approximates the relative error.
			if (term < prev)
			{
				// Still converging: add new term.
				sum += term;
			}
			else
			{
				//  Diverging: subtract previous term and
				sum -= prev;
				break;
			}
		}
		*res_out = exp(x_in)*(1.0+sum)/x_in;
	}
	return JVX_DSP_NO_ERROR;
#undef MAXIT
#undef EULER
#undef FPMIN
#undef EPS
}

// ======================================================================

static jvxData expint1(jvxData x)
{
  static const int MAX = 23;  /* The number of coefficients in a[].   */

  static const jvxData a[ 23] = { 7.8737715392882774,
                          -8.0314874286705335,
                           3.8797325768522250,
                          -1.6042971072992259,
                           0.5630905453891458,
                          -0.1704423017433357,
                           0.0452099390015415,
                          -0.0106538986439085,
                           0.0022562638123478,
                          -0.0004335700473221,
                           0.0000762166811878,
                          -0.0000123417443064,
                           0.0000018519745698,
                          -0.0000002588698662,
                           0.0000000338604319,
                          -0.0000000041611418,
                           0.0000000004821606,
                          -0.0000000000528465,
                           0.0000000000054945,
                          -0.0000000000005433,
                           0.0000000000000512,
                          -0.0000000000000046,
                           0.0000000000000004 };

  int k;
  jvxData  arg, t, value, b0, b1, b2;

  arg = .25 * x;    /* Argument in Chebyshev expansion is x/4. */
  t = 2. * arg;

  b2 = 0.;
  b1 = 0.;
  b0 = a[MAX-1];

  for (k = MAX-2; k >= 0; k--) {
	  b2 = b1;
	  b1 = b0;
	  b0 = t * b1 - b2 + a[k];
  }

  value = .5 *  (b0 - b2);

  value += log(fabs(x));

  return -value;

} /* End of expint1(). */

static jvxData expint2(jvxData x)
{
  static const int MAX = 23;  /* The number of coefficients in a[].   */

  static const jvxData a[ 23] = { 0.2155283776715125,
         0.1028106215227030,
        -0.0045526707131788,
         0.0003571613122851,
        -0.0000379341616932,
         0.0000049143944914,
        -0.0000007355024922,
         0.0000001230603606,
        -0.0000000225236907,
         0.0000000044412375,
        -0.0000000009328509,
         0.0000000002069297,
        -0.0000000000481502,
         0.0000000000116891,
        -0.0000000000029474,
         0.0000000000007691,
        -0.0000000000002070,
         0.0000000000000573,
        -0.0000000000000163,
         0.0000000000000047,
        -0.0000000000000014,
         0.0000000000000004,
        -0.0000000000000001 };

  int k;
  jvxData  arg, t, value, b0, b1, b2;

  arg =4./x;    /* Argument in the Chebyshev expansion.       */
  t = 2. * (2. * arg - 1.);

  b2 = 0.;
  b1 = 0.;
  b0 = a[MAX-1];

  for (k = MAX-2; k >= 0; k--) {
      b2 = b1;
      b1 = b0;
      b0 = t * b1 - b2 + a[k];
      }

  value = .5 *  (b0 - b2);

  value *= exp(-x);

  return value;

} /* End of expint2(). */


jvxDspBaseErrorType
ExponentialIntegral(jvxData x_in, jvxData* res_out)
{
  if (x_in >= -4. && x_in <= 4.)
  {
	  *res_out = expint1(x_in);
	  return JVX_DSP_NO_ERROR;
  }
  else if (x_in > 4.)
  {
	  *res_out = expint2(x_in);
	  return JVX_DSP_NO_ERROR;
  }
  return JVX_DSP_ERROR_INVALID_SETTING;

}

jvxInt32
jvx_gcd(jvxInt32 a, jvxInt32 b)
{
  long u,v,q,t;

  u = labs(a);
  v = labs(b);

  while (v)
    {
      q = (long)floor((jvxData)u/(jvxData)v);
      t = u - q*v;
      u = v;
      v = t;
    }

  return (jvxInt32)u;
}


// sin(x)/x
jvxData jvx_si (jvxData x)
{
	return  (x==0.0) ? 1.0 : sin(x)/x ;
}


// Returns the error function erf(x).
jvxData jvx_erf(jvxData x)
{
	jvxData retVal = 0;
	retVal = jvx_gammaincP(x*x, 0.5);
	if(x < 0.0)
	{
		return -retVal;
	}
	return retVal;
}

// Returns the complementary error function erfc(x).
jvxData jvx_erfc(jvxData x)
{
	jvxData retVal = 0;
	if(x < 0.0)
	{
                retVal = jvx_gammaincP(x*x, 0.5);
		retVal = 1.0 + retVal;
	}
	else
	{
		retVal = jvx_gammaincQ(x*x, 0.5);
	}
	return retVal;
}

// Random noise generators

// idum initialized with negative long integer
jvxData jvx_rand_uniform(jvxInt32 *idum)
{
#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)
	jvxInt32 j;
	jvxInt32 k;
	static jvxInt32 idum2=123456789;
	static jvxInt32 iy=0;
	static jvxInt32 iv[NTAB];
	jvxData temp;
	if (*idum <= 0) // Initialize
	{
		if (-(*idum) < 1)
		{
			*idum=1;
		}
		else
		{
			*idum = -(*idum);
		}
		idum2=(*idum);
		for (j=NTAB+7;j>=0;j--)
		{
			k=(*idum)/IQ1;
			*idum=IA1*(*idum-k*IQ1)-k*IR1;
			if (*idum < 0)
			{
				*idum += IM1;
			}
			if (j < NTAB)
			{
				iv[j] = *idum;
			}
		}
		iy=iv[0];
	}
	k=(*idum)/IQ1;
	*idum=IA1*(*idum-k*IQ1)-k*IR1;
	if (*idum < 0)
	{
		*idum += IM1;
	}
	k=idum2/IQ2;
	idum2=IA2*(idum2-k*IQ2)-k*IR2;
	if (idum2 < 0)
	{
		idum2 += IM2;
	}
	j=iy/NDIV;
	iy=iv[j]-idum2;
	iv[j] = *idum;
	if (iy < 1)
	{
		iy += IMM1;
	}
	if ((temp=AM*iy) > RNMX) // To limit to valid range
	{
		return RNMX;
	}
	else
	{
		return temp;
	}
#undef IM1
#undef IM2
#undef AM
#undef IMM1
#undef IA1
#undef IA2
#undef IQ1
#undef IQ2
#undef IR1
#undef IR2
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX
}


jvxData jvx_rand_normal(jvxInt32 *idum)
{
	static int iset=0;
	static jvxData gset;
	jvxData fac,rsq,v1,v2;
	if (*idum < 0)
	{
		iset = 0;
	}

	if (iset == 0)
	{
		do
		{
			v1 = 2.0 * jvx_rand_uniform(idum) - 1.0;
			v2 = 2.0 * jvx_rand_uniform(idum) - 1.0;
			rsq = v1 * v1 + v2 * v2;
		} while (rsq >= 1.0 || rsq == 0.0);

		fac = sqrt(-2.0 * log(rsq) / rsq);
		gset = v1 * fac;
		iset = 1;
		return v2 * fac;
	}
	else
	{
		iset = 0;
		return gset;
	}
}

// Computes the square of the argument
jvxData jvx_sqr(jvxData x)
{
   return  x*x;
}

jvxData jvx_pow2db(jvxData value)
{
#ifdef JVX_DSP_DATA_FORMAT_FLOAT
    return 10. * log10f(JVX_MAX(value, JVX_DATA_EPS));
#else
	return 10. * log10(JVX_MAX(value, JVX_DATA_EPS));
#endif
}

jvxData jvx_db2pow(jvxData value)
{
#ifdef JVX_DSP_DATA_FORMAT_FLOAT
    return powf(10., value/10.);
#else
	return pow(10., value/10.);
#endif
}

jvxData jvx_mag2db(jvxData value)
{
#ifdef JVX_DSP_DATA_FORMAT_FLOAT
    return 20. * log10f(JVX_MAX(value, JVX_DATA_EPS));
#else
	return 20. * log10(JVX_MAX(value, JVX_DATA_EPS));
#endif
}

jvxData jvx_db2mag(jvxData value)
{
#ifdef JVX_DSP_DATA_FORMAT_FLOAT
    return powf(10., value/20.);
#else
	return pow(10., value/20.);
#endif
}