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
#include "mexConfigWriter.h"
#include "jvxTconfigProcessor.h"
//#include "HrtpRemoteCall.h"

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


//=========================================================
/**
 * Constructor: Set all values to initialized/uninitialized values,
 * Constructor opens the used fileWriter/Reader library and sets all variables to initial.
 *///========================================================
mexConfigWriter::mexConfigWriter(void)
{
	theWriter.theHdl = NULL;
	theWriter.theObj = NULL;
}

jvxErrorType
mexConfigWriter::init()
{
	jvxErrorType res = jvxTconfigProcessor_init(&theWriter.theObj);
	jvxComponentIdentification tp = JVX_COMPONENT_UNKNOWN;
	jvxBool multi = false;
	if((res == JVX_NO_ERROR) && theWriter.theObj)
	{
		res = theWriter.theObj->request_specialization(reinterpret_cast<jvxHandle**>(&theWriter.theHdl), &tp, &multi);
	}
	return(res);
}
/**
 * Destructor: Set all values to initial, unload the library for fileReading and leave.
 *///======================================================================================
mexConfigWriter::~mexConfigWriter(void)
{
	this->terminate();
}

jvxErrorType 
mexConfigWriter::terminate()
{
	if(theWriter.theObj)
	{
		jvxTconfigProcessor_terminate(theWriter.theObj);
	}
	theWriter.theObj = NULL;
	theWriter.theHdl = NULL;
	return(JVX_NO_ERROR);
}

// ==================================================================================

jvxErrorType 
mexConfigWriter::produceFile(std::string fName, const mxArray* inputStruct, mxArray*& returnField)
{
	bool success = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* theSectionAddTo = NULL;
	const mxArray* arr = NULL;

	if(theWriter.theHdl)
	{
		res = decideType(inputStruct, theWriter.theHdl, theSectionAddTo, "MAIN_SECTION");
		if(res == JVX_NO_ERROR)
		{
			jvxApiString fldStr;
			theWriter.theHdl->printConfiguration(theSectionAddTo, &fldStr, false);
			
			std::string content = fldStr.std_str();

			res = jvx_writeContentToFile(fName, content);
			if (res != JVX_NO_ERROR)
			{
				mexErrMsgTxt(("Error: Failed to write content to file " + fName).c_str());
			}
			else
			{
				success = true;
			}
		}
		else
		{
			mexErrMsgTxt(("Error: Failed to write content to file " + fName).c_str());
		}
	}
	else
	{
		mexErrMsgTxt("Error: No config processor handle available.");
	}

	this->mexReturnBool(returnField, success);
	return(JVX_NO_ERROR);
}

jvxErrorType
mexConfigWriter::decideType(const mxArray* inputStruct, IjvxConfigProcessor* processor, jvxConfigData*& theSectionProduce, std::string name)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (mxIsStruct(inputStruct))
	{
		processor->createEmptySection(&theSectionProduce, name.c_str());
		res = writeStruct(inputStruct, processor, theSectionProduce);
	}
	else if(mxIsNumeric(inputStruct))
	{
		res = writeNumeric(inputStruct, processor, theSectionProduce, name);
		// return jvxData element
	}
	else if(mxIsChar(inputStruct))
	{
		res = writeCharacter(inputStruct, processor, theSectionProduce, name);
	}
	else if(mxIsCell(inputStruct))
	{
		res = writeCell(inputStruct, processor, theSectionProduce, name);
	}
	else
	{
		mexErrMsgTxt("Invalid use of rtpConfigWriterMatlab: Second argument must be a struct, a numeric, a string or a cell array");
		res = JVX_ERROR_WRONG_SECTION_TYPE;
	}
	return(res);
}

jvxErrorType 
mexConfigWriter::writeStruct(const mxArray* inputStruct, IjvxConfigProcessor* processor, jvxConfigData* theSectionAddTo)
{
	jvxErrorType res;
	int i;
	jvxConfigData* theSectionToBeAdded = NULL;
	int numFields = mxGetNumberOfFields(inputStruct);
	mxArray* subField = NULL;
	for(i = 0; i < numFields; i++)
	{
		std::string nameField = mxGetFieldNameByNumber(inputStruct, i);
		subField = NULL;
		subField = mxGetFieldByNumber(inputStruct, 0, i);
		
		theSectionToBeAdded = NULL;
		res = decideType(subField, processor, theSectionToBeAdded, nameField);
		
		if(res != JVX_NO_ERROR)
		{
			std::string err = jvxErrorType_txt(res);
			err = "Error in file generation substruct " + nameField + ", " + err;
			mexPrintf(err.c_str());
			processor->removeHandle(theSectionToBeAdded);
			processor->createComment(&theSectionToBeAdded, err.c_str());
		}

		if(theSectionToBeAdded)
		{
			processor->addSubsectionToSection(theSectionAddTo, theSectionToBeAdded);
		}
		theSectionToBeAdded = NULL;
	}
	return(JVX_NO_ERROR);
}
	

jvxErrorType 
mexConfigWriter::writeNumeric(const mxArray* inputStruct, IjvxConfigProcessor* processor, jvxConfigData*& theSectionWriteTo, std::string name)
{
	int i,j;
	jvxInt8* fldI8 = NULL;
	jvxInt16* fldI16 = NULL;
	jvxInt32* fldI32 = NULL;
	jvxInt64* fldI64 = NULL;
	jvxUInt8* fldUI8 = NULL;
	jvxUInt16* fldUI16 = NULL;
	jvxUInt32* fldUI32 = NULL;
	jvxUInt64* fldUI64 = NULL;
	float* fldFLT = NULL;
	double* fldDBL = NULL;
	jvxConfigData* theSectionToBeAdded = NULL;

	mwSize M = 0;
	mwSize N = 0;

	M = mxGetM(inputStruct);
	N = mxGetN(inputStruct);

	if(N*M == 1)
	{
		if(mxIsInt8(inputStruct))
		{
			fldI8 = (jvxInt8*)mxGetData(inputStruct);
			processor->createAssignmentValue(&theSectionWriteTo, name.c_str(), (jvxData)*fldI8);
		}
		if(mxIsUint8(inputStruct))
		{
			fldUI8 = (jvxUInt8*)mxGetData(inputStruct);
			processor->createAssignmentValue(&theSectionWriteTo, name.c_str(), (jvxData)*fldUI8);
		}
				
		
		if(mxIsInt16(inputStruct))
		{
			fldI16 = (jvxInt16*)mxGetData(inputStruct);
			processor->createAssignmentValue(&theSectionWriteTo, name.c_str(), (jvxData)*fldI16);
		}
		if(mxIsUint16(inputStruct))
		{
			fldUI16 = (jvxUInt16*)mxGetData(inputStruct);
			processor->createAssignmentValue(&theSectionWriteTo, name.c_str(), (jvxData)*fldUI16);
		}

				
		
		if(mxIsInt32(inputStruct))
		{
			fldI32 = (jvxInt32*)mxGetData(inputStruct);
			processor->createAssignmentValue(&theSectionWriteTo, name.c_str(), (jvxData)*fldI32);
		}
		if(mxIsUint32(inputStruct))
		{
			fldUI32 = (jvxUInt32*)mxGetData(inputStruct);
			processor->createAssignmentValue(&theSectionWriteTo, name.c_str(), (jvxData)*fldUI32);
		}

				
		if(mxIsInt64(inputStruct))
		{
			fldI64 = (jvxInt64*)mxGetData(inputStruct);
			processor->createAssignmentValue(&theSectionWriteTo, name.c_str(), (jvxData)((jvxInt64)*fldI64));
		}
		if(mxIsUint64(inputStruct))
		{
			fldUI64 = (jvxUInt64*)mxGetData(inputStruct);
			processor->createAssignmentValue(&theSectionWriteTo, name.c_str(), (jvxData)((jvxUInt64)*fldUI64));
		}

		if(mxIsDouble(inputStruct))
		{
			fldDBL = (double*)mxGetData(inputStruct);
			processor->createAssignmentValue(&theSectionWriteTo, name.c_str(), (jvxData)*fldDBL);
		}

		if(mxIsSingle(inputStruct))
		{
			fldFLT = (float*)mxGetData(inputStruct);
			processor->createAssignmentValue(&theSectionWriteTo, name.c_str(), (jvxData)((float)*fldFLT));
		}
	}
	else
	{
		processor->createAssignmentValueList(&theSectionWriteTo, name.c_str());
		if(mxIsInt8(inputStruct))
		{
			fldI8 = (jvxInt8*)mxGetData(inputStruct);
			for(i = 0; i < M; i++)
			{
				for(j = 0; j < N; j++)
				{
					processor->addAssignmentValueToList(theSectionWriteTo, i, (jvxData)fldI8[j*M+i]);
				}
			}
		}
		if(mxIsUint8(inputStruct))
		{
			fldUI8 = (jvxUInt8*)mxGetData(inputStruct);
			for(i = 0; i < M; i++)
			{
				for(j = 0; j < N; j++)
				{
					processor->addAssignmentValueToList(theSectionWriteTo, i, (jvxData)fldUI8[j*M+i]);
				}
			}
		}
				
		
		if(mxIsInt16(inputStruct))
		{
			fldI16 = (jvxInt16*)mxGetData(inputStruct);
			for(i = 0; i < M; i++)
			{
				for(j = 0; j < N; j++)
				{
					processor->addAssignmentValueToList(theSectionWriteTo, i, (jvxData)fldI16[j*M+i]);
				}
			}
		}
		if(mxIsUint16(inputStruct))
		{
			fldUI16 = (jvxUInt16*)mxGetData(inputStruct);
			for(i = 0; i < M; i++)
			{
				for(j = 0; j < N; j++)
				{
					processor->addAssignmentValueToList(theSectionWriteTo, i, (jvxData)fldUI16[j*M+i]);
				}
			}
		}

				
		
		if(mxIsInt32(inputStruct))
		{
			fldI32 = (jvxInt32*)mxGetData(inputStruct);
			for(i = 0; i < M; i++)
			{
				for(j = 0; j < N; j++)
				{
					processor->addAssignmentValueToList(theSectionWriteTo, i, (jvxData)fldI32[j*M+i]);
				}
			}
		}
		if(mxIsUint32(inputStruct))
		{
			fldUI32 = (jvxUInt32*)mxGetData(inputStruct);
			for(i = 0; i < M; i++)
			{
				for(j = 0; j < N; j++)
				{
					processor->addAssignmentValueToList(theSectionWriteTo, i, (jvxData)fldUI32[j*M+i]);
				}
			}
		}

				
		if(mxIsInt64(inputStruct))
		{
			fldI64 = (jvxInt64*)mxGetData(inputStruct);
			for(i = 0; i < M; i++)
			{
				for(j = 0; j < N; j++)
				{
					processor->addAssignmentValueToList(theSectionWriteTo, i, (jvxData)((jvxInt64)fldI64[j*M+i]));
				}
			}
		}
		if(mxIsUint64(inputStruct))
		{
			fldUI64 = (jvxUInt64*)mxGetData(inputStruct);
			for(i = 0; i < M; i++)
			{
				for(j = 0; j < N; j++)
				{
					processor->addAssignmentValueToList(theSectionWriteTo, i, (jvxData)((jvxUInt64)fldUI64[j*M+i]));
				}
			}
		}

		if(mxIsDouble(inputStruct))
		{
			fldDBL = (double*)mxGetData(inputStruct);
			for(i = 0; i < M; i++)
			{
				for(j = 0; j < N; j++)
				{
					processor->addAssignmentValueToList(theSectionWriteTo, i, (jvxData)fldDBL[j*M+i]);
				}
			}
		}
		if(mxIsSingle(inputStruct))
		{
			fldFLT = (float*)mxGetData(inputStruct);
			for(i = 0; i < M; i++)
			{
				for(j = 0; j < N; j++)
				{
					processor->addAssignmentValueToList(theSectionWriteTo, i, (jvxData)fldFLT[j*M+i]);
				}
			}
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
mexConfigWriter::writeCharacter(const mxArray* inputStruct, IjvxConfigProcessor* processor, jvxConfigData*& theSectionWriteTo, std::string name)
{
	std::string entry = createCStringMex(inputStruct);
	processor->createAssignmentString(&theSectionWriteTo, name.c_str(), entry.c_str());
	return(JVX_NO_ERROR);
}


jvxErrorType 
mexConfigWriter::writeCell(const mxArray* inputStruct, IjvxConfigProcessor* processor, jvxConfigData*& theSectionWriteTo, std::string name)
{
	int i;
	mwSize numCells = mxGetNumberOfElements(inputStruct);
	processor->createAssignmentStringList(&theSectionWriteTo, name.c_str());

	mxArray* arr = NULL;

	for(i = 0; i < numCells; i++)
	{
		arr = NULL;
		arr = mxGetCell(inputStruct, i);
		if(arr)
		{
			if(mxIsChar(arr))
			{
				std::string entry = createCStringMex(arr);
				processor->addAssignmentStringToList(theSectionWriteTo, entry.c_str());
			}
			else
			{
				mexPrintf("Warning: When parsing cell array, a non-string entry was observed. This entry will be ignored.");
			}
		}
	}
	return(JVX_NO_ERROR);
}
