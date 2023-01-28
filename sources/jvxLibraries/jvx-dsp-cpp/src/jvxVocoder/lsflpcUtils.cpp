
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#include "jvxVocoder/lsflpcUtils.h"

static void getLsfpol(const jvxData  *lsf, int   amountLsfs, int index, jvxData *f);
static jvxData chebichev(jvxData  x, int   amountLsfs, jvxData  *f);

int
convertLsf2Lpc(const jvxData *lsf, jvxData *lpcs, int amountLsfs )
{
   jvxData  *f1, *f2;
   int i, j;

   /* allocate dynamic RAM */
   f1 	= (jvxData*)malloc(sizeof(jvxData) * (amountLsfs/2+1));
   f2 	= (jvxData*)malloc(sizeof(jvxData) * (amountLsfs/2+1));

   getLsfpol( lsf, amountLsfs, 0, f1);    /* find f1[] ( the sum polynomial)  */
   getLsfpol( lsf, amountLsfs, 1, f2);    /* find f2[] ( the difference pol.) */

    for( i=(amountLsfs/2); i>0; i--)
	{
        f1[i] += f1[i-1];   /* multiply by (1+z^{-1}    */
        f2[i] -= f2[i-1];   /* multiply by (1-z^{-1}    */
    }

    lpcs[0] = 1;

    for( i=1, j=amountLsfs; i<= (amountLsfs/2); i++, j--)
	{
        lpcs[i] = (jvxData)0.5*(f1[i] + f2[i]);
        lpcs[j] = (jvxData)0.5*(f1[i] - f2[i]);
    }

	/* de-allocate dynamic RAM */
	free(f1);
	free(f2);

    return(amountLsfs);
} /* end of Lsf2a() */

static void
getLsfpol(const jvxData  *lsf, int   amountLsfs, int index, jvxData *f)
{
    jvxData  b;
    int   i, j;

    f[0] = 1.0;
    b    = -2.0*cos(lsf[index]);
    f[1] = b;

    for( i=2; i<=(amountLsfs/2); i++)
	{
        b    = -2.0*cos(lsf[2*i+index-2]);
        f[i] = b*f[i-1] + 2.0*f[i-2];
        for( j=i-1; j>1; j--) f[j] += b*f[j-1] + f[j-2];
        f[1] += b;
    }
    return;
} /* end of Get_lsfpol() */

/*==================================================*/
/*==================================================*/
/*==================================================*/
/*==================================================*/

int
convertLpc2Lsf(const jvxData *lpcs, jvxData *lsf, int amountLsfs)
{
	jvxData	*f1, *f2, *coef;
	jvxData 	*pf1;
	jvxData	delta, xint, xlow, ylow, xhigh, yhigh, xmid, ymid;
	int		i, j, nc, nf, ip;

	delta = 0.01;		/* step size */
	nc    = amountLsfs/2;

	f1   = (jvxData*) malloc(sizeof(jvxData) * (nc+1));
	f2   = (jvxData*) malloc(sizeof(jvxData) * (nc+1));
	coef = (jvxData*) malloc(sizeof(jvxData) * (nc+1));

	/* find the sum and difference polynomials F1(z) and F2(z)	*/
	/* after division by (1+z^{-1}) and (1-z^{-1}) respectively	*/

	f1[0] = 1.0;
	f2[0] = 1.0;

	for( i=1, j=amountLsfs; i<=nc; i++, j--)
	{
		f1[i] = lpcs[i] + lpcs[j] - f1[i-1];
		f2[i] = lpcs[i] - lpcs[j] + f2[i-1];
	}

	/* find the LSFs using the Chebichev polynomial evaluation */

	nf = 0;		/* number of found frequencies	*/
	ip = 0;		/* 0 --> F1(z),   1--> F2(z)	*/

	for( i=0; i<= nc; i++)
		coef[i] = f1[i];

	xlow = 1.0;
	ylow = chebichev( xlow, amountLsfs, coef);

	while( xlow > -1.0 )
	{
		xhigh = xlow;
		yhigh = ylow;
		xlow  = xlow - delta;
		ylow  = chebichev( xlow, amountLsfs, coef);

		if( ylow*yhigh <= 0.0 )		/* if sign change */
		{
			/* division of the interval of sign change by 4 */
			for( i=0; i<=3; i++)
			{
				xmid = 0.5*(xlow + xhigh);
				ymid = chebichev( xmid, amountLsfs, coef);

				if( ylow*ymid <= 0.0 )
				{
					yhigh = ymid;
					xhigh = xmid;
				}
				else
				{
					ylow = ymid;
					xlow = xmid;
				}
			}

			/* linear interpolation to evaluate the root */
			xint    = xlow - ylow*(xhigh-xlow) / (yhigh-ylow);
			lsf[nf] = acos(xint);
			nf      = nf + 1;
			ip      = 1-ip; 	/* to find next root of other polynomial */
			xlow    = xint;

			if( ip==0 )
				pf1 = f1;
			else
				pf1 = f2;

			for( i=0; i<=nc; i++)
				coef[i] = *pf1++;

			ylow = chebichev( xlow, amountLsfs, coef);
		}
	}


	free(f1);
	free(f2);
	free(coef);

	return( nf );

	/* if the returned number nf is less than m, some of the roots are missed 	*/
	/* This may happen in very high gain filters(eg. when the input is a single	*/
	/* tone) . In this case the filter is bandwidth expanded and the routine   	*/
	/* A2Lsf must be recalled.													*/

} /* end of A2Lsf() */

/*------------------------------------------------------------------------------*/
/* Evaluate the chebichev polynomial at point x									*/
/* (function module)															*/
/*------------------------------------------------------------------------------*/
static jvxData
chebichev(jvxData  x, int   amountLsfs, jvxData  *f)
{
	jvxData	b0, b1, b2;
	int	i,n;

	n  = amountLsfs/2;
	b2 = 1.0;
	b1 = 2.0*x + f[1];

	for( i=2; i<n; i++)
	{
		b0 = 2.0*x*b1 - b2 + f[i];
		b2 = b1;
		b1 = b0;
	}

	return( x*b1 - b2 + 0.5*f[n]);

} /* end of chebichev() */

#define EPS 1e-8

void
warp_Lsfs(jvxData* lsfs, int number_lsfs, jvxData warping_coeff)
{
	int i;
	jvxData re, im, tmp1, tmp2, tmp4, tmp6;
	for(i = 0; i < number_lsfs; i++)
	{
		re = cos(lsfs[i]);
		im = sin(lsfs[i]);

		tmp1 = 1 + re * warping_coeff;
		tmp2 = im * warping_coeff;
		tmp4 = atan(tmp2/(fabs(tmp1)+EPS));
		if(tmp1 < 0)
		{
			tmp4 = M_PI - tmp4;
		}
		tmp1 = re + warping_coeff;
		tmp2 = im;

		tmp6 = atan(tmp2/(fabs(tmp1)+EPS));
		if(tmp1 < 0)
		{
			tmp6 = M_PI - tmp6;
		}
		tmp2 = tmp6 - tmp4;
		lsfs[i] = tmp2;
	}
}

/*
% Weg 2:
for(ind=1:size(lsfs,2))
    real_lsf = cos(lsfs(ind));
    imag_lsf = sin(lsfs(ind));

    tmp1 = 1 + real_lsf * ap;
    tmp2 = imag_lsf * ap;
	tmp3 = tmp1 * tmp1 + tmp2*tmp2;
	tmp3 = sqrt(tmp3);
    tmp4 = atan(tmp2/(abs(tmp1) + EPS));
    if(tmp1 < 0)
        tmp4 = pi - tmp4;
    end

    tmp1 = real_lsf + ap;
	tmp2 = imag_lsf;

    tmp5 = tmp1 * tmp1 + tmp2*tmp2;
	tmp5 = sqrt(tmp5);
    tmp6 = atan(tmp2/(abs(tmp1)+EPS));
     if(tmp1 < 0)
        tmp6 = pi - tmp6;
    end

    tmp1 = tmp5 / tmp3;
	tmp2 = tmp6 - tmp4;
	lsfs_out_w2(ind) = exp(i*tmp2);
end*/
