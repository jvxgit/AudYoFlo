#include "jvx.h"

#include "localMexIncludes.h"
#include "CjvxCToMatlabConverter.h"
#include "CjvxMatlabToCConverter.h"

#include "jvx_math/jvx_math_extensions.h"

/**
 * C-style entry from MATLAB to MEX component.
 * All command arguments are passed in the optional arguments for
 * this mex function.
 * Call it as follows:
 * out = jvxURand(<NUM>, [<seed>]);
 * to output a 1xNUM vector of uniform random data.
 * 
 *///=============================================================
void mexFunction( int nlhs, mxArray *plhs[],
 				  int nrhs, const mxArray *prhs[] )
{
	if((nrhs >= 1)&&(nlhs >= 1))
	{
		jvxSize i;
		jvxInt32 seed = 0;
		jvxSize numVals = 1;
		CjvxMatlabToCConverter::mexArgument2Index<jvxSize>(numVals, prhs, 0, nrhs);
		if (nrhs > 1)
		{
			CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(seed, prhs, 1, nrhs);
		}
		
		SZ_MAT_TYPE ndim = 2;
		SZ_MAT_TYPE dims[2] = { 1, 1 };
		dims[1] = (int)numVals;
		mxArray* pplhs = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxREAL);
		jvxData* randOut = (jvxData*)mxGetData(pplhs);
		
		*randOut++ = jvx_rand_uniform(&seed);
		for (i = 0; i < (numVals - 1); i++)
		{
			*randOut++ = jvx_rand_uniform(&seed);			
		}
		plhs[0] = pplhs;
		if (nlhs > 1)
		{
			CjvxCToMatlabConverter::mexReturnInt32(plhs[1], seed);
		}
		return;
	}
	mexWarnMsgTxt(
		"This function requires <1-2> input and <1-2> output arguments. Use it as follows: \n"
		"[<out>, [<state>]] = jvxURand(<num>, [<state>]);  \n"
		"with: \n"
		" - <num>: I: Number of output values. \n"
		" - <state>: IO: Internal state taken from synthesis to synthesis. \n"
		" - <out>: O: Current random value. \n"
		" \n"
		"1) If <state> is not provided on call, it is <state> = 0.\n"
		"2) If <state> is <= 0, it acts as the seed.\n");

	CjvxCToMatlabConverter::mexFillEmpty(plhs, nlhs, 0);
}

