
#include "jvxVocoder/correlation2Lsfs.h"

#define SIGN(a,b) ((b)<0 ? -fabs((a)) : fabs((a)))

static void splitLevinson_anti( const jvxData* correlation, 
								jvxData  *reflect, jvxData *alphap, 
								jvxData* alphaq, int amountLsfs );

static void calculateSymmetricLsfs(jvxData *alphap, jvxData *teta, int amountLsfs);

static void calculateAntiSymmetricLsfs(jvxData *alphaq, jvxData *teta, int amountLsfs);

static void Tqli ( jvxData *d, jvxData  *e, int order);

/*! Function required for qsort! */
static int
compare(const void* one, const void* two)
{
	if((*((jvxData*)one)) > (*((jvxData*)two)))
	{
		return(1);
	}
	else if(*((jvxData*)one) < *((jvxData*)two))
	{
		return(-1);
	}
	return(0);
}

/* Function to convert correlation coefficients into lsfs */
int 
correlation2Lsfs( const jvxData *correlation, jvxData *lsfs, jvxData* reflect, int amountLsfs)
{

    jvxData *alphap, *alphaq;
    int i;

	alphap = (jvxData*)malloc(sizeof(jvxData)*(amountLsfs+1));
	alphaq = (jvxData*)malloc(sizeof(jvxData)*(amountLsfs+1));

	for(i=0; i<(amountLsfs);i++)
	{
		reflect[i] = 0;
	}

    splitLevinson_anti(correlation, reflect, alphap, alphaq, amountLsfs);
    for (i=0; i<(amountLsfs); i++) 
	{
		if(fabs(reflect[i]) > 1.0)
		{  
			return(0);
		}
	}
	
	/* Calculate the symmetric and the antisymmetric part of the lsfs */
	calculateSymmetricLsfs(alphap, lsfs, amountLsfs);
	calculateAntiSymmetricLsfs(alphaq, lsfs, amountLsfs);

	/* Finallay sort the lsfs */
	qsort(lsfs, amountLsfs, sizeof(jvxData), compare);
	free(alphaq);
	free(alphap);
	return(1);
}

/* Determine the two symmetric and asymmetric parts of the lsps */
void 
splitLevinson_anti( const jvxData* correlation, jvxData  *reflect, 
					jvxData *alphap, jvxData* alphaq, int amountLsfs )
{

    int   i, j, t, l;

    jvxData *taup, *lambda, *p_lin;

	/* allocate dynamic RAM */
	taup 	= (jvxData*) malloc(sizeof(jvxData)*(amountLsfs+1));
	lambda 	= (jvxData*) malloc(sizeof(jvxData)*(amountLsfs+1));
	p_lin 	= (jvxData*) malloc(sizeof(jvxData)*(amountLsfs+2)*(amountLsfs+1));

    /* initial values */
    p_lin[0] =0.0;
    p_lin[amountLsfs+2] =-1.0;

    for(i=1; i<=(amountLsfs); i++) 
	{
        p_lin[i] =1.0;
    }
    taup[0] = correlation[0];

    lambda[0]=1.0;

    /* three term recurrence formula */
    for(i=1; i<=(amountLsfs); i++) 
	{ /* in [1] and [2] i=k */
		t = i / 2;
		l = 2 * t;
		taup[i]=0.0;
		
		if (i==l) 
		{     /*i,k is even; case k=2*t in [3]-(25)*/
			for (j=0; j<t; j++) 
			{
				taup[i] += (correlation[j] - correlation[i-j]) * p_lin[i+(amountLsfs+1)*j];
			}
		}
		else 
		{
			/*i,k is odd; case k=2*t+1 in [3]-(25)*/
			for (j=0; j<=t; j++) 
			{
				taup[i] += (correlation[j] - correlation[i-j]) * p_lin[i+(amountLsfs+1)*j];
			}
		}
		
		alphaq[i] = taup[i] / taup[i-1];
		lambda[i] = 2.0 - alphaq[i] / lambda[i-1];
		alphap[i] = lambda[i] * (2.0-lambda[i-1]);
		
		for (j=1;j<=t;j++) 
		{
            p_lin[(i+1)+((amountLsfs+1)*j)] = p_lin[i+(amountLsfs+1)*j]
				+ p_lin[i+((amountLsfs+1)*(j-1))]
				- alphaq[i] * p_lin[i-1+((amountLsfs+1)*(j-1))];
		}
		
		reflect[i-1] = 1.0 - lambda[i];
		p_lin[ (i+1) + ((amountLsfs+1)*(t+1)) ] = 0.0;
		
	}/* for */
	
	/* de-allocate dynamic RAM */
	free( taup );
	free( lambda);
	free( p_lin);

	return;

} /* end of Split_Levinson_anti() */

/* Determine the symmetric lsfs */
void 
calculateSymmetricLsfs(jvxData *alphap, jvxData *teta, int amountLsfs)
{
   int i, j;
   jvxData *d, *e;

   /* allocate dynamic RAM */
   d = (jvxData*) malloc(sizeof(jvxData)*((amountLsfs/2)+1));            /* d[] holds main diagonal */
   e = (jvxData*) malloc(sizeof(jvxData)*((amountLsfs/2)+1));            /* e[] holds sub diagonal */

   /*   length m_2+1 is due to Tqli shift */
   
   alphap[1]=2.0*alphap[1];   /* start value is jvxDatad, ref.[1]-(29)
								 first element of matrix is 2* alpha1...
								 instead of alpha_i...*/
   
   /* order is even; matrix J_p/2, ref.[1]-(29) */
    d[0]=alphap[1]+alphap[2]-2.0;      /* main diagonal element */

    for(i=1;i<(amountLsfs/2);i++)
	{
        j=2*i;
        d[i]=alphap[j+1]+alphap[j+2]-2.0;
        e[i]=alphap[j]*alphap[j+1];    /* sub diagonal element */
    }/* for */
	
    Tqli (&d[0], &e[0], amountLsfs);     /* function described in ref.[2]
											calculates eigenvalues of matrix
											eigenvalues are stored in d
											array e is destroyed */
	
    for(i=0;i<(amountLsfs/2); i++) 
        teta[i*2]=acos(d[i]*0.5);/*even part of teta*/

	/* de-allocate dynamic RAM */
	free(d);
	free(e);
	return;
	
} /* end of Symmetric() */

/* Determine the anti-symmetric lsfs */
void 
calculateAntiSymmetricLsfs(jvxData *alphaq, jvxData *teta, int amountLsfs)
{
	int i, j;
	jvxData *d, *e;

   /* allocate dynamic RAM */
   d = (jvxData*) malloc(sizeof(jvxData)*((amountLsfs/2)+1));            
   /* d[] holds main diagonal */

   e = (jvxData*) malloc(sizeof(jvxData)*((amountLsfs/2)+1));            
   /* e[] holds sub diagonal */

   /*   length m_2+1 is due to Tqli shift */

   d[0]=alphaq[2]-2.0;

   for(i=1;i<(amountLsfs/2);i++) 
   {
	   j    = 2 * i;
	   d[i] = alphaq[j+1] + alphaq[j+2] - 2.0;     /* d[] main diagonal */
	   e[i] = alphaq[j] * alphaq[j+1];             /* e[] sub diagonal */
   }/* for */
   
   Tqli (&d[0], &e[0], (amountLsfs)); /* calculate eigenvalues of matrix,
										   result stored in d[]*/
   
   for(i=0; i < (amountLsfs/2); i++) 
   {
	   teta[i*2+1] = acos(d[i]*0.5);
   }

   /* de-allocate dynamic RAM */
   free( d);            /* d[] holds main diagonal */
   free( e);            /* e[] holds sub diagonal */

   return;
   
} /* end of Anti_Symmetric() */

/* Execute the Eigenvalue-determination, using QL-decomposition*/
void 
Tqli ( jvxData *d, jvxData  *e, int order)
{
    int     m, l, i, order_2;
    jvxData    s, r, p, g, f, dd, c, b;

	order_2 = order/2;

    dd = 0.0;
    for(i=1; i<order_2; i++) 
	{
        e[i]=sqrt(e[i]); /* as root missing in Symmetric
							and Anti_Symmetric */
    }
	
    for(i=order_2;i>0; i--) 
	{
        d[i]=d[i-1]; /* as in ref.[1] index chosen otherwise */
    }
	
    e[order_2] = 0.0;
	
    for (l=1; l<=order_2; l++)
	{
        do 
		{
            for (m=l; m<=order_2-1; m++) 
			{
				if (fabs(e[m]) < (jvxData)JVX_DATA_EPS* 1000.0 ) 
					break;
            }
            if (m != l)
			{
                /* 1 line error handling not implemented */
                g = (d[l+1]-d[l]) / (2.0*e[l]);
                r = sqrt((g*g)+1.0);
                g = d[m] - d[l] + e[l]/(g+SIGN(r,g));
                s = c = 1.0;
                p = 0.0;
				
                for (i=m-1;i>=l; i--) 
				{
                    f = s*e[i];
                    b = c*e[i];
					
                    if(fabs(f)>= fabs(g))
					{
                        c      = g/f;
                        r      = sqrt((c*c)+1.0);
                        e[i+1] = f*r;
                        c     *= (s=1.0/r);
                    }
                    else 
					{
                        s      = f/g;
                        r      = sqrt((s*s)+1.0);
                        e[i+1] = g*r;
                        s     *= (c=1.0/r);
                    }
					
                    g      = d[i+1]-p;
                    r      = (d[i]-g) * s + 2.0 * b * c;
                    p      = s * r;
                    d[i+1] = g + p;
                    g      = c * r - b;
                }/*for*/
				
                d[l] = d[l] - p;
                e[l] = g;
                e[m] = 0.0;
            }/*if*/
        } while (m != l);
    } /*for*/
	
    for (i=0; i< order_2; i++) 
		d[i] = d[i+1];
	
    return;
} /* end of Tqli() */
