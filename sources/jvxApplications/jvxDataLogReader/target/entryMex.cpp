#if _MATLAB_MEXVERSION < 500
#if (_MSC_VER >= 1600)
#include <yvals.h>
#define __STDC_UTF_16__
#endif
#endif
#include <mex.h>

/*
#pragma comment ( lib, "libmx")
#pragma comment ( lib, "libmex")
#pragma comment ( lib, "libeng")
#pragma comment ( lib, "libmat")*/
//#pragma comment ( lib, "libmatlb") 


//

#include <stdio.h>
#include <string>
#include <vector>

#include "mexCommandsFileReader.h"
#include "mexFileReader.h"


/**
 * Main entry point for realtime host: Mex-Entry: 
 * If calling for the first time, allocate object.
 * Unloading is implemented by closing matlab at the moment so
 * do not bither about this yet.
 *///=================================================
//rtProcHostControlMex* instControl = NULL;

//! Instance for the MATLAB fileReader. Will be allocated as soon as required for the very first time
mexFileReader instControl;

/**
 * C-style entry from MATLAB to MEX component.
 * All command arguments are passed in the optional arguments for
 * this mex function.
 *///=============================================================
void mexFunction( int nlhs, mxArray *plhs[],
 				  int nrhs, const mxArray *prhs[] )
{	
	try
	{
		// Pass MEX command through to the underlying component
		jvxBool resB = instControl.parseInput(nlhs, plhs, nrhs, prhs);
	}

	// Be save for returned exceptions
	catch(...)
	{
		if(nlhs > 0)
		{
			instControl.mexReturnBool(plhs[0], false);	
		}
		if(nlhs > 1)
		{
			instControl.mexReturnAnswerNegativeResult(plhs[1], "Standard Exception (Segmentation Violation).", JVX_ERROR_STANDARD_EXCEPTION);
		}
	}
}

