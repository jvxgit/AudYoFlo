/* 
 *****************************************************
 * Filename: entryMex.cpp
 *****************************************************
 * Project: RTProc-ESP (Echtzeit-Software-Plattform) *
 *****************************************************
 * Description:                                      *
 *****************************************************
 * Developed by JAVOX SOLUTIONS GMBH, 2012           *
 *****************************************************
 * COPYRIGHT BY JAVOX SOLUTION GMBH                  *
 *****************************************************
 * Contact: rtproc@javox-solutions.com               *
 *****************************************************
*/

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

#include "mexConfigWriter.h"
#include <stdio.h>
#include <string>
#include <vector>

/**
 * Function to create a C string in MEX format.
 *///=========================================================0
static std::string
createCStringMex(const mxArray* phs)
{
	size_t bufLen = mxGetM(phs)*mxGetN(phs)*sizeof(char)+1;
	char* buf = new char[bufLen];
	mxGetString(phs, buf, bufLen);
	std::string str = buf;
	delete[](buf);
	return(str);
}


/**
 * C-style entry from MATLAB to MEX component.
 * All command arguments are passed in the optional arguments for
 * this mex function.
 *///=============================================================
void mexFunction( int nlhs, mxArray *plhs[],
 				  int nrhs, const mxArray *prhs[] )
{
	mexConfigWriter theReader;
	theReader.init();
	if((nrhs == 2)&&(nlhs == 1))
	{
		std::string fName = "";

		// Step I: Extract command ID from input argument
		if(mxIsChar(prhs[0]))
		{
			fName = createCStringMex(prhs[0]);
		}

		theReader.produceFile(fName, prhs[1], plhs[0]);
	}
	else
	{
		mexPrintf("mexConfigWriterMatlab: Tool to write config files readable by RTProc.\n");
		mexPrintf("Syntax: [success] = mexConfigWriterMatlab(filename, datastruct);\n");
	}
	theReader.terminate();
}

