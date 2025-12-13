/******************************************************************************
* program : LBG_MEX.C  lbg codebook training algorithm (MEX version)          * 
* author  : G.Harles (IND/RWTH-Aachen, StA Schnitzler: St27/95)               *
* date    : 18.07.95   							      * 
* update  : 01.08.95 (Sn)  LBG - algorithm: use absolute energy difference    *
*			   for breakoff condition  			      * 
* update  : 15.10.96 (Sn)  added comments, modified distance functions        *
*******************************************************************************
* function:                                                                   *
*                                                                             *
*    LBG creates a new codebook or retrains an existing codebook              *
*    using training data as reference                                         *
*    LBG is based on the Linde-Buzo-Gray training algorithm                   *  
*    the new codebook can then be used for a vector-quantization              *
*******************************************************************************
* usage   :                                                                   *
* [newcodebook,abserr,relerr,it]                                              *
*             =lbg(traindat,oldcodebook,ny,energy-limit,energy-diff-limit)    *
*                                                                             *
* OUTPUT **********************************************************************
* 
*     newcodebook   : centroids resulting from lbg algorithm                  *
*                    the dimension of the newcodebook vectors is              *
*                    the same as the dimension of traindat/oldcodebook        *
*    abserr        : (optional) error energy at algorithm breakoff            *
*    relerr        : (optional) difference between last two error energies    *
*                    before breakoff                                          *
*    it            : (optional) number of iteration steps           
*
* 
* INPUT ***********************************************************************
* 
*    traindat      : data which is used to train the codebook                 *
*    oldcodebook   : existing codebook which is going to be adapted           *
*                    if no old codebook exists, enter []                      *
*    ny            : (optional) number of vectors in newcodebook              *
*                    must be equal to number of vectors in oldcodebook        *
*                    is no more optional when oldcodebook does not exist      *
*    energy-limit  : (optional) error energy which should lead to breakoff    *
*                    default value = 0                                        *
*    energy-diff-limit : (optional) difference between                        *
*                        two error energies which should lead to breakoff     *
*                        default value = 1e-06                                *
*    NOTE:                                                                    *
*    All vectors are understood to be COLUMN VECTORS                          * 
*    In a matrix the columns are considered as vectors                        *
*       the number of columns rerturns the number of vectors                  *
*       the number of rows returns the dimension of the vectors               *
*                                                                             *
*    An optional input is still necessary if the                              *
*    optional variable to its right is given!                                 *
*									      *
*******************************************************************************
*******************************************************************************/

#include        "mex.h"
#include        <stdio.h>
#include        <stdlib.h>
#include	<math.h>
#include	<time.h>

// #define VERBOSE_OUT

void initialise_codebook();
void adapt_centroids();
double lbg();
void timeseed();
void use();
double assign_vectors_to_centroids();
double	deuclidean_distance();
double deuclidean_distance_m();

double drandLoc()
{
	return((double)rand()/(double)RAND_MAX);
}

/******************************************************************************
*  mexFunction : MATLAB interface                                             *
*******************************************************************************/

void mexFunction(nlhs, plhs, nrhs, prhs)
     int nlhs;
     int nrhs;
     mxArray *plhs[];
     mxArray *prhs[];
{
  
  double	**x=(double**)NULL, **y=(double**)NULL;
  int           i,j;
  int		nx, ny=0;
  int		dim;
  int		*y_assigned, *count;
  double	energy_limit=0.0, energy_diff_limit=0.0;
  double	energy, old_energy;
  long          it;
  
   /**** number of input/output variables correct?    ****/
  
  if(((nrhs != 6) && (nrhs!=5)&&(nrhs!=4)&&(nrhs!=3)&&(nrhs!=2))||((nlhs!=1)&&(nlhs!=2)&&(nlhs!=3)&&(nlhs!=4)))
    {
	  mexErrMsgTxt("Wrong number of input/output arguments\n");
      use();
    }
 
  /**** reading of nx and dim                        ****/

  nx=(int)mxGetN(prhs[0]);
  dim=(int)mxGetM(prhs[0]);
 
  /**** creating outputs                             ****/

  plhs[1]=mxCreateDoubleMatrix(1,1,mxREAL);
  plhs[2]=mxCreateDoubleMatrix(1,1,mxREAL);
  plhs[3]=mxCreateDoubleMatrix(1,1,mxREAL);
  
  /**** if no error limits are given                 ****/
  energy_limit = 0;
  energy_diff_limit = 1e-06;

  if(nrhs > 3)
   {
	  energy_limit = *mxGetPr(prhs[3]);
    }
  /**** if one error limit is given                  ****/
   
  if (nrhs > 4)
    {
      energy_diff_limit=*mxGetPr(prhs[4]);
    }
   
  long srand_val = (long)time((time_t*)0);
  if (nrhs > 5)
  {
	  double val = *mxGetPr(prhs[5]);
	  srand_val = round(val);
  }

	srand(srand_val);
  // seed48

  /**** if old codebook does not exist                ****/
  
  if((mxGetM(prhs[1])==0)&&(mxGetN(prhs[1])==0))
	 {
	 if(nrhs==2)
		{
		mexErrMsgTxt("Old codebook and codebook size (ny) missing!\n");
		}
	 else
		{
#ifdef VERBOSE_OUT
			 mexPrintf("no old codebook given, new codebook is initialized\n");
#endif
		ny=(int)*mxGetPr(prhs[2]);
		plhs[0]=mxCreateDoubleMatrix(dim,ny,mxREAL);
	  
		/**** memory allocation for y             ****/
	  
		if((y=(double**)mxCalloc(ny,sizeof(double*)))==(double**)NULL)
	       mexErrMsgTxt("mxCalloc Error y\n");
	  
		/**** setting y to output codebook        ****/
	  
		for(i=0;i<ny;i++)
	       y[i]=(mxGetPr(plhs[0])+i*dim);
	  
		initialise_codebook(y,ny,dim);
		}
	 }
      
  /**** if old codebook exists                      ****/ 
  else
	 {   
	 if(dim!=mxGetM(prhs[1]))
	    mexErrMsgTxt("dimensions of new codebook (x) and existing codebook do not match!\n");
      
	 if(nrhs==2)    /**** ny not existing  ****/
		{
		ny=(int)mxGetN(prhs[1]);
		}
	 else           /**** ny existing      ****/
		{
		if(*mxGetPr(prhs[2])!=mxGetN(prhs[1]))
   	       mexErrMsgTxt("codebook size in old codebook and given ny  do not match!\n");
		else 
	       ny=(int)*mxGetPr(prhs[2]);
		}
	 
#ifdef VERBOSE_OUT
	 mexPrintf("old codebook is given, reading old codebook\n");
#endif

	 plhs[0]=mxCreateDoubleMatrix(dim,ny,mxREAL);
     
	 /**** allocate memory for pointer to column vectors y  ****/
      
	 if((y=(double**)mxCalloc(ny,sizeof(double*)))==(double**)NULL)
	 mexErrMsgTxt("mxCalloc Error y\n");
      
	 /**** setting y to output codebook             ****/
      
	 for(i=0;i<ny;i++)
	 y[i]=(mxGetPr(plhs[0])+i*dim);
      
	 /**** reading of oldcodebook and reshape to matrix ****/
      
	 for(i=0;i<ny;i++)
		{
		for(j=0;j<dim;j++)
		   {
		   y[i][j]=mxGetPr(prhs[1])[i*dim+j];
		   }
		}	
	 }

  /**** allocate memory for pointer to column vectors x  ****/
  
  if((x=(double**)mxCalloc(nx,sizeof(double*)))==(double**)NULL)
     mexErrMsgTxt("mxCalloc Error x\n");
  
  for(i=0;i<nx;i++)
     x[i]=(mxGetPr(prhs[0])+i*dim);

  /**** allocate memory for partion assignments ****/
  if( (y_assigned=(int*)mxCalloc(sizeof(int),nx)) == (int*)NULL )
	 {	
	 mexErrMsgTxt("Not enough memory\n" );	
	 exit(-1);	
	 }

  /**** allocate memory for assigment counters ****/
  if( (count=(int*)mxCalloc(sizeof(int),ny)) == (int*)NULL )
	 {	
	  mexErrMsgTxt("out of memory\n");
	 exit(-1);
	 }

  /**** loop for adapting centroids until error small enough  ****/

  energy = HUGE;
  it=0L;
  
  do
	 {
	 old_energy = energy;
	 energy = lbg( y, ny, x, nx, dim, y_assigned, count, deuclidean_distance_m);
	 it++;
#ifdef VERBOSE_OUT
	 mexPrintf("iteration %ld: Aver. distortion : %10.5f  \r",it, energy );
#endif

	 if (energy>old_energy)
	    mexWarnMsgTxt("Warning: distortion increasing !\n");
	 }
  while( (energy>energy_limit) && (old_energy-energy>energy_diff_limit) );

#ifdef VERBOSE_OUT
  mexPrintf("\n");
#endif

  *mxGetPr(plhs[1])=energy;
  *mxGetPr(plhs[2])=old_energy-energy;
  *mxGetPr(plhs[3])=(double)it;
  
  assign_vectors_to_centroids( y, ny, x, nx, dim, deuclidean_distance_m, y_assigned, count );

  mxFree( y_assigned );
  mxFree( count );
  
} 
 

/*******************************************************************************
* lbg - cluster Algorithm
*
* Lloyd-iteration
*
********************************************************************************/
double	lbg( y, n_y, x, n_x, dim, y_assigned, count, distance_function )
     double		**y;						/* centroids of the codebook		*/
     int		n_y;						/* number of entrys in codebook		*/
     double		**x;						/* training	vextors x				*/
     int		n_x;						/* number of training vectors		*/
     int		dim;						/* vector dimension					*/
	 int		*y_assigned;				/* y index assigned to vector x		*/
	 int		*count;						/* counters for partion assignments	*/
     double		(*distance_function)();		/* distance function				*/
{
  double	error_energy;						/* mean error of codebook			*/
  double	assign_vectors_to_centroids();
  
  error_energy = assign_vectors_to_centroids( y, n_y, x, n_x, dim, distance_function, y_assigned);
  adapt_centroids( y, n_y, x, n_x, dim, y_assigned, count );
  
  
  return( error_energy );
}

/*******************************************************************************
* initialise_codebook                                                          *
* initialises a codebook y with n_y vectors of length dim                      *
* each vector-element is set to a random value -0.5 <= value < 0.5             *
********************************************************************************/
void initialise_codebook( y, n_y, dim )
     double	**y;
     int		n_y;
     int		dim;
{
  int		i, j;
  // double	drand48();
  
  timeseed();
  
  for( j=0 ; j<n_y ; j++ )
    for( i=0 ; i<dim ; i++ )
      y[j][i] = ( drandLoc() - 0.5 ) * 0.01;
}

/*******************************************************************************
* assign_vectors_to_centroids ( assign nearest neighbours )
*
* Nearest-Neighbour-condition of Lloyd-iteration
*
* given :	y = centroids of a codebook with n_y entrys
*			x = set of n_x training-vectors
*			dim = dimension ( length ) of the vectors
*			distance_function is a pointer to a distance function ( euklid )
* result :	y_assigned is a table containing the indices of the centroids y
*			assigned to each vector x of the training set
*			the value returned by assign_vectors_to_centroids is the mean
*			difference between each x and it's nearest neighbour y
*******************************************************************************/
double	assign_vectors_to_centroids( y, n_y, x, n_x, dim, distance_function, y_assigned )
     double		**y;						/* centroids of the codebook		*/
     int			n_y;						/* number of entrys in codebook		*/
     double		**x;						/* training	vextors x				*/
     int			n_x;						/* number of training vectors		*/
     int			dim;						/* vector dimension					*/
     double		(*distance_function)();			/* distance function				*/
     int			*y_assigned;	
{
  int		ix, iy;
  int		imin=0;
  double	dmin;
  double	distance;
  double	energy;
  
  /*
   *	find the nearest neighbour of each training vector x in the codebook
   */
  energy = 0.0;
  for( ix=0 ; ix<n_x ; ix++ )
	 {
	 dmin = HUGE;
	 for( iy=0 ; iy<n_y ; iy++ )
		{
		if( dmin > (distance=((*distance_function)( x[ix], y[iy], dim ))) )
		   {
		   dmin = distance;
		   imin = iy;
		   }
		}					
	 energy += (dmin);
	 y_assigned[ix] = imin;
	 }
  return( energy/(double)n_x );
}

/*******************************************************************************
* adapt_centroids( y, n_y, x, n_x, dim, distance_function, y_assigned, count )        *
*
* Centroid condition of Lloyd-Iteration:
*  calculate new centroids y for the given partition y_assigned of
*  training set x
*
* -> see Gersho/Gray, "Vector Quantization and Signal  Compression", p.365
*    
********************************************************************************/
void adapt_centroids( y, n_y, x, n_x, dim, y_assigned, count )
     double		**y;					/* centroids of the codebook		*/
     int		n_y;					/* number of entrys in codebook		*/
     double		**x;					/* training	vextors x	        */
     int		n_x;					/* number of training vectors		*/
     int		dim;					/* vector dimension			*/
     int		*y_assigned;	
	 int		*count;					/* counters for partion assignments	*/
{
  int		max_count = 0;
  int		count_max;
  int		ix;
  int		iy;
  int		k;
  int       count_empty = 0;
  
  /* initialize counters and centroids to zero */
  for( iy=0 ; iy<n_y ; iy++ )
	 {
	 count[iy] = 0;
	 for( k=0 ; k<dim ; k++ )
	    y[iy][k] = 0.0;
	 }
  
  /* accumulate numerator and denominator terms (see reference) */
  for( ix=0 ; ix<n_x ; ix++ )
	 {
	 iy = y_assigned[ix];
	 count[iy]++;
	 for( k=0 ; k<dim ; k++ )
	    y[iy][k] += x[ix][k];	
	 }
  
  /* search partition with maximum number of vectors assigned to */
  for (iy=0, count_max=-1; iy<n_y; ++iy)
	 {
	 if (count[iy] > count_max)
		{
		count_max = count[iy];
		max_count = iy;
		}
	 }

  /* if non-empty cell: finish centroid computation */
  for( iy=0 ; iy<n_y ; iy++ )
    if( count[iy] != 0 )
      for( k=0 ; k<dim ; k++ )
	y[iy][k] /= count[iy];
  
  /* if empty cell: split partion with maximum entries */
  for( iy=0 ; iy<n_y ; iy++ )
	 {
	 if( count[iy] == 0 )
		{
		count_empty++;
	    for( k=0 ; k<dim ; k++ )
		   y[iy][k] = y[max_count][k] + 0.01*drandLoc()-0.005;
		}
	 }
  if (count_empty > 0)
  {
#ifdef VERBOSE_OUT
	  mexPrintf("\n %d empty cell(s) found -> splitting ...\n", count_empty);
#endif
  }
}





/*******************************************************************************
* timeseed: set seed value by time                                             *
********************************************************************************/

#include	<time.h>

void timeseed()
{
  srand((long)time((time_t *)0));
}

/*******************************************************************************
*  use: information about how to use lbg                                       *
********************************************************************************/
void use()
{
  mexPrintf("\nuse : [newcodebook,abserr,relerr,it]=lbg(traindat,oldcodebook,ny,energy-limit,energy-diff-limit)\n\n");
  mexPrintf(" newcodebook   : centroids resulting from lbg algorithm\n");
  mexPrintf("                 the dimension of the newcodebook vectors is the same as the dimension of traindat/oldcodebook\n");
  mexPrintf(" abserr        : (optional) error energy at algorithm breakoff\n");
  mexPrintf(" relerr        : (optional) difference between last two error energies before breakoff\n");
  mexPrintf(" it            : (optional) number of iteration steps\n");
  mexPrintf(" traindat      : data which is used to train the codebook\n");
  mexPrintf(" oldcodebook   : existing codebook which is going to be adapted\n");
  mexPrintf("               : if no old codebook exists, enter []\n");
  mexPrintf(" ny            : (optional) number of vectors in newcodebook\n");
  mexPrintf("                 must be equal to number of vectors in oldcodebook\n");
  mexPrintf("                 is no more optional when oldcodebook does not exist\n");
  mexPrintf(" energy-limit  : (optional) error energy which should lead to breakoff\n");
  mexPrintf("                 default value = 0\n");
  mexPrintf(" energy-diff-limit : (optional) difference between two error energies which should lead to breakoff\n");
  mexPrintf("                 default value = 1e-06\n");
  mexPrintf("\nAll vectors must be COLUMN VECTORS\n");
  mexPrintf("In a matrix the columns are considered as vectors;\n");
  mexPrintf("     the number of columns rerturns the number of vectors;\n");
  mexPrintf("     the number of rows returns the dimension of the vectors;\n");
  mexPrintf("\nAn optional input is still necessary if the optional number to its right is given!\n");
  mexErrMsgTxt("program terminated");
}

/*******************************************************************************
* different routines out of functions.x                                        *
********************************************************************************/
/*
double deuclidean_distance( x, y, n )
	double	*x, *y;
	int		n;
{
	double	sum = 0.0;

	while( --n >= 0 )
		sum += sqr( *x++ - *y++ );

	return( sqrt(sum) );
}
*/
double deuclidean_distance_m( x, y, n )
	double	*x, *y;
	int		n;
{
	double  d;
	double	sum = 0.0;
	int 	i;

/*	while( --n >= 0 )
		sum += sqr( *x++ - *y++ ); */
	for (i=0; i<n; ++i)
	   {
	   d = x[i]-y[i];
	   sum += d*d;
	   }
	return( sum );
}

