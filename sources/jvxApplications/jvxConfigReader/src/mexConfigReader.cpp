#include <cassert>

#define MAXSTRING 256

#if _MATLAB_MEXVERSION >= 100
#define SZ_MAT_TYPE mwSize
#else
#define SZ_MAT_TYPE int
#endif

#if _MATLAB_MEXVERSION < 500
#if (_MSC_VER >= 1600)
#include <yvals.h>
#define __STDC_UTF_16__
#endif
#endif
#include <mex.h>

#include <string>

//#include <matrix.h>
#include "mexConfigReader.h"
#include "jvx.h"
#include "jvxTconfigProcessor.h"
//#include "HrtpRemoteCall.h"


//=========================================================
/**
 * Constructor: Set all values to initialized/uninitialized values,
 * Constructor opens the used fileWriter/Reader library and sets all variables to initial.
 *///========================================================
mexConfigReader::mexConfigReader(void)
{
	theReader.theObj = NULL;
	theReader.theHdl = NULL;
}

jvxErrorType
mexConfigReader::init()
{
	jvxErrorType res = jvxTconfigProcessor_init(&theReader.theObj);
	jvxComponentIdentification tp = JVX_COMPONENT_UNKNOWN;
	jvxBool multi = false;
	if((res == JVX_NO_ERROR) && theReader.theObj)
	{
		res = theReader.theObj->request_specialization(reinterpret_cast<jvxHandle**>(&theReader.theHdl), &tp, &multi);
	}
	return(res);
}

/**
 * Destructor: Set all values to initial, unload the library for fileReading and leave.
 *///======================================================================================
mexConfigReader::~mexConfigReader(void)
{
	this->terminate();
}

jvxErrorType 
mexConfigReader::terminate()
{
	if(theReader.theObj)
	{
		jvxTconfigProcessor_terminate(theReader.theObj);
	}
	theReader.theObj = NULL;
	theReader.theHdl = NULL;
	return(JVX_NO_ERROR);
}

jvxErrorType 
mexConfigReader::parseFile(std::string fName, mxArray*& plhs)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* theSection = NULL;
	jvxApiError theError;
	std::string content;

	if(theReader.theHdl)
	{
		res = theReader.theHdl->parseFile(fName.c_str());
		if (res == JVX_NO_ERROR)
		{
			res = theReader.theHdl->getConfigurationHandle(&theSection);
			if (res == JVX_NO_ERROR)
			{
				if (this->mexReturnStructSection(plhs, theSection, theReader.theHdl))
				{
					res = JVX_NO_ERROR;
				}
				else
				{
					mexPrintf("Failed to transform content into Matlab struct.");
					this->mexFillEmpty(&plhs, 1, 0);
					res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
				}
				theReader.theHdl->removeHandle(theSection);
			}
			else
			{
				mexPrintf("Failed to obtain configuration handle.");
				this->mexFillEmpty(&plhs, 1, 0);
			}
		}
		else
		{
			switch (res)
			{
			case JVX_ERROR_OPEN_FILE_FAILED:
				mexPrintf(("File " + fName + " does not exist.").c_str());
				break;
			default:
				theReader.theHdl->getParseError(&theError);
				std::string errorText = "unknown";
				errorText = theError.errorDescription.std_str();
				mexPrintf(("Parse error: " + errorText).c_str());
				this->mexFillEmpty(&plhs, 1, 0);
			}
		}

	}
	else
	{
		mexPrintf("Failed to receive config reader module.");
		this->mexFillEmpty(&plhs,1,0);
		res = JVX_ERROR_NOT_READY;
	}
	return(res);
}

//======================================================================
//======================================================================
// MEX-RETURNS MEX-RETURNS MEX-RETURNS MEX-RETURNS MEX-RETURNS MEX-RETURNS
//======================================================================
//======================================================================


