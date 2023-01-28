/* 
 *****************************************************
 * Filename: mexGenericIp.h
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

#ifndef __MEXFILEREADER_H__
#define __MEXFILEREADER_H__

#include <vector>
#include "jvx.h"
#include "mexConstants.h"
#include "CjvxCToMatlabConverter.h"

/**
 * Object to wrap the fileReading functionality. This class dispatches 
 * MEX functioncalls to the underlying library component for fileReading
 *///=================================================================
class mexConfigReader: public CjvxCToMatlabConverter
{
	struct
	{
		IjvxConfigProcessor* theHdl;
		IjvxObject* theObj;
	} theReader;



public:

	//! Constructor
	mexConfigReader(void);

	//! Destructor
	~mexConfigReader(void);

	jvxErrorType init();

	jvxErrorType terminate();

	jvxErrorType parseFile(std::string fName, mxArray*& returnStruct);
	
	jvxErrorType readContent(std::string fName, std::string& storeContent);

};

#endif
