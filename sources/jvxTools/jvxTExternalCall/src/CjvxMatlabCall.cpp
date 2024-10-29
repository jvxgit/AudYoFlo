#include <iostream>

#include "CjvxMatlabCall.h"

#define ERROR_THREAD_ID(a) (std::string)"Function IrtpExternalCall::" + a + (std::string)" called from thread other than the Matlab thread (would lead to exception).";

#ifndef RTPROC_ENABLE_MATLAB_ACCESS
#define RTPROC_ENABLE_MATLAB_ACCESS
#endif

#define MAXSTRING 256

#if _MATLAB_MEXVERSION >= 100
#define SZ_MAT_TYPE mwSize
#else
#define SZ_MAT_TYPE int
#endif

// #define VERBOSE

#define FLD_NAME_DICT_TAG_STRING "JVX_TAG"
#define FLD_NAME_DICT_VALUE_STRING "JVX_VALUE"

//======================================================
// Matlab call handler - global object
//======================================================
//CjvxExternalCall theHandler;
/*
extern "C"
{
// Global access function to matlab call handler
IrtpExternalCall* JVX_CALLINGCONVENTION IrtpExternalCall_getReference()
{
	return(&theHandler);
}
}
*/

/**
 * Static function to convert an mxArray string into a std string
 *///=====================================================
static std::string
createCStringMex(const mxArray* phs)
{
	int bufLen = (int)(mxGetM(phs)*mxGetN(phs)*sizeof(char)+1);
	char* buf = new char[bufLen];
	mxGetString(phs, buf, bufLen);
	std::string str = buf;
	delete[](buf);
	return(str);
}


/**
 * API (C function): Function to return a reference to the global
 * handler for Matlab access to C functions.
 *///=====================================================
/*
CjvxExternalCall* getReferenceCjvxExternalCall()
{
	return(&theHandler);
}
*/

jvxErrorType
CjvxExternalCall::registerThreadId()
{
	this->threadIdMatlab = JVX_GET_CURRENT_THREAD_ID();
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxExternalCall::unregisterThreadId()
{

	this->threadIdMatlab = 0;
	return(JVX_NO_ERROR);
}
/**
 * API (C function) for MEX: This entry function for Matab
 * is te gateway to user function calls, decided on the basis of the
 * first passed argument hich is the function name.
 *///=====================================================
jvxErrorType
CjvxExternalCall::external_call( jvxInt32 nlhs, jvxExternalDataType **plhs, jvxInt32 nrhs, const jvxExternalDataType **prhs, int offset_nlhs, int offset_nrhs )
{
	const mxArray* arr1 = nullptr;
	const mxArray* arr2 = nullptr;
	std::string nObject;

	std::string nFunction;
	bool foundFunc = false;
	unsigned short i;
	jvxErrorType res = JVX_NO_ERROR;

	// Find optional name of object
	if (nrhs >= 1)
	{
		arr1 = (const mxArray*)prhs[0];
		if (mxIsChar(arr1))
		{
			nObject = createCStringMex(arr1);
		}
	}

	if(nrhs >= 2)
	{
		arr2 = (const mxArray*)prhs[1];
		if((mxIsChar(arr2)))
		{
			// First argument is a string, so take this string and
			// select an object in the list of registered function objects
			nFunction = createCStringMex(arr2);
			for(i=0; i < this->_lst_functions.size(); i++)
			{
				if((nFunction == this->_lst_functions[i].fName)&&(nObject == this->_lst_functions[i].objectName))
				{
#ifdef VERBOSE
					std::cout << "Calling function \"" << nFunction << "\"." << std::endl;
#endif
					// Object found! Call the C-function and pass object pointer
					foundFunc = true;
					int cpnrhs = nrhs-2;
					const jvxExternalDataType** cpprhs = NULL;
					if(cpnrhs > 0)
					{
						cpprhs = (const jvxExternalDataType**)&prhs[2];
					}
					res = this->_lst_functions[i].fPtr(static_cast<IjvxExternalCall*>(this), this->_lst_functions[i].fObject, nlhs, (jvxExternalDataType**)plhs, cpnrhs, cpprhs, offset_nlhs, offset_nrhs+2);
					if(res != JVX_NO_ERROR)
					{
						// PTR->postMessageExternal(((std::string)"Error: Call of function\"" + nFunction + "\" failed, error code: " + jvx_int2String(fRet) +"\n").c_str());
						this->mexFillEmpty(nlhs, (mxArray**)plhs);
					}
					return(res);
				}
			}

			// If object could not be found, print out error message
			if(!foundFunc)
			{
				this->postMessageExternal(((std::string)"Error: Function \"" + nFunction + "\" not registered in C/C++ object\n").c_str());
				return(JVX_ERROR_ELEMENT_NOT_FOUND);
			}
			// Fill all lhs parameters
			this->mexFillEmpty(nlhs, (mxArray**)plhs);
			return(JVX_ERROR_UNEXPECTED);
		}

		// Error + fillup of parameters
		this->postMessageExternal("Error: Expect names of object and function as first argument.\n");
		this->mexFillEmpty(nlhs, (mxArray**) plhs);
		return(JVX_ERROR_INVALID_ARGUMENT);
	}

	// Info call to this object. As result a struct with all registered
	// function is returned
	if(nlhs >= 1)
	{
		mxArray* ptr = NULL;
		std::vector<elementFunctions> lstFcts;
		if (nObject.empty())
		{
			lstFcts = this->_lst_functions;
		}
		else
		{
			for (auto elmL : this->_lst_functions)
			{
				if (elmL.objectName == nObject)
				{
					lstFcts.push_back(elmL);
				}
			}
		}
		this->mexReturnStructsFunctions(ptr, lstFcts);
		plhs[0] = ptr;
	}
	this->mexFillEmpty(nlhs, (mxArray**)plhs);
	return(JVX_NO_ERROR);
}


/**
 * API: Function to register a new function including the
 * object pointer reference, the name, a function pointer and
 * a description.
 * Return values:
 * - 0: operation completed succesfully
 * - -1: a function with the same name has been registered before
 *///=====================================================
jvxErrorType
CjvxExternalCall::registerFunction(const char* objectName, const char* fName, jvxExternalFunction fPtr, const char* fDescription, IjvxExternalCallTarget* fObject,
									const char** inputParams, int numInParams, const char** outputParams, int numOutParams)
{
	unsigned short i;
	CjvxExternalCall::elementFunctions singleElm;


	for(i = 0; i < this->_lst_functions.size(); i++)
	{
		if((_lst_functions[i].fName == fName)&&((_lst_functions[i].objectName == objectName)))
		{
			return(JVX_ERROR_DUPLICATE_ENTRY);
		}
	}

#ifdef VERBOSE
	std::cout << "Register function \"" << fName << "\"." << std::endl;
#endif
	// New function ready to be added
	singleElm.fObject = fObject;
	singleElm.fName = fName;
	singleElm.objectName = objectName;
	singleElm.fDescription = fDescription;
	singleElm.fPtr = fPtr;

	for(int i = 0; i < numInParams; i++)
	{
		if(inputParams)
		{
			singleElm.fdescrsInputParams.push_back(inputParams[i]);
		}
		else
		{
			singleElm.fdescrsInputParams.push_back("Input Parameter #" + jvx_int2String(i));
		}
	}

	for(int i = 0; i < numOutParams; i++)
	{
		if(outputParams)
		{
			singleElm.fdescrsOutputParams.push_back(outputParams[i]);
		}
		else
		{
			singleElm.fdescrsOutputParams.push_back("Output Parameter #" + jvx_int2String(i));
		}
	}

	_lst_functions.push_back(singleElm);

	return(JVX_NO_ERROR);
}

/**
 * API : Function to unregister a registered function.
 * The function to be unregistered is passed as a string containing the
 * function name.
 * Return values:
 * - 0: operation cmpleted succesfully
 * - -1: Function with this name has not been registered
 *///=====================================================
jvxErrorType
CjvxExternalCall::unregisterFunction(jvxExternalFunction fPtr)
{
	std::vector<elementFunctions>::iterator elm;
	for(elm = _lst_functions.begin(); elm != _lst_functions.end(); elm++)
	{
		if(elm->fPtr == fPtr)
			break;
	}
	if(elm != _lst_functions.end())
	{
#ifdef VERBOSE
		std::cout << "Unregister function \"" << elm->fName << "\"." << std::endl;
#endif
		_lst_functions.erase(elm);
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ELEMENT_NOT_FOUND);
}

jvxErrorType 
CjvxExternalCall::obtain_reference(jvxSize* idRef)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxExternalCall::release_reference(jvxSize idRef) 
{
	if (_lst_functions.size())
	{
		std::string outTxt = __FUNCTION__;
		outTxt += ": Not all functions were unregistered, unregistering by host to protect system state!!";
		mexWarnMsgTxt(outTxt.c_str());
		for (auto elm : _lst_functions)
		{
			outTxt = "--> ";
			outTxt += elm.objectName;
			outTxt += "::";
			outTxt += elm.fName;

			mexWarnMsgTxt(outTxt.c_str());
		}
		_lst_functions.clear();
	}

	return JVX_NO_ERROR;
}


jvxErrorType
CjvxExternalCall::postMessageExternal(const char* message, bool isError)
{
	if(this->checkThread())
	{
		if(isError)
		{
			//mexErrMsgTxt(message);
			//mexPrintf
			mexWarnMsgTxt(((std::string)" --> Error: " + message).c_str());
		}
		else
		{
			
			mexPrintf(message);
		}
		return(JVX_NO_ERROR);
	}
	_common_set.theErrordescr = ERROR_THREAD_ID("postMessageExternal");
	return(JVX_ERROR_THREADING_MISMATCH);
}

/**
 * Function to return a cell arra of function names and descriptions
 * to view all registered functions in Matlab.
 *///=====================================================

bool
CjvxExternalCall::mexReturnStructsFunctions(mxArray*& plhs, const std::vector<elementFunctions>& lstFcts)
{
	unsigned i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	mxArray* arr = NULL;
	plhs = NULL;

	if(lstFcts.size() == 0)
	{
		dims[0] = 1;
		dims[1] = 0;
		plhs = mxCreateCellArray(ndim, dims);
	}
	else
	{
		dims[0] = 1;
		dims[1] = (int)lstFcts.size();
		plhs = mxCreateCellArray(ndim, dims);
		for(i = 0; i < lstFcts.size(); i++)
		{
			this->mexReturnStructFunction(arr, lstFcts[i].objectName, lstFcts[i].fName, lstFcts[i].fDescription,
				lstFcts[i].fdescrsInputParams, lstFcts[i].fdescrsOutputParams);
			mxSetCell(plhs, i, arr);
			arr = NULL;
		}
	}
	return(true);
}

/**
 * Function to return a simgle struct corresponding to one
 * registered function.
 *///=================================================================
bool
CjvxExternalCall::mexReturnStructFunction(mxArray*& plhs,
										   const std::string& objectName,
											const std::string& fName,
											const std::string& fDescription,
											const std::vector<std::string>& strListIn,
											const std::vector<std::string>& strListOut)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	mxArray* arr = NULL;

	const char** fld = new const char*[5];
	std::string fldName1 = "OBJECT_NAME_STRING";
	std::string fldName2 = "FUNCTION_NAME_STRING";
	std::string fldName3 = "DESCRIPTION_STRING";
	std::string fldName4 = "INPUT_ARGS";
	std::string fldName5 = "OUTPUT_ARGS";

	fld[0] = fldName1.c_str();
	fld[1] = fldName2.c_str();
	fld[2] = fldName3.c_str();
	fld[3] = fldName4.c_str();
	fld[4] = fldName5.c_str();

	plhs = mxCreateStructArray(ndim, dims, 5, fld);
	delete[](fld);

	// Add name of function
	mexReturnString(arr, objectName);
	mxSetFieldByNumber(plhs,0, 0, arr);
	arr = NULL;

	// Add name of function
	mexReturnString(arr, fName);
	mxSetFieldByNumber(plhs,0, 1, arr);
	arr = NULL;

	// Add description for function
	mexReturnString(arr, fDescription);
	mxSetFieldByNumber(plhs,0, 2, arr);
	arr = NULL;

	mexReturnStringCell(arr, strListIn);
	mxSetFieldByNumber(plhs,0,3, arr);
	arr = NULL;

	mexReturnStringCell(arr, strListOut);
	mxSetFieldByNumber(plhs,0, 4, arr);
	arr = NULL;

	return(true);
}

/**
 * Function to fill all lhs parameters with empty arrays to avoid
 * the "output not assigned" error in Matlab.
 *///=====================================================
bool
CjvxExternalCall::mexFillEmpty(jvxSize nlhs, mxArray* plhs[])
{
	jvxSize i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	dims[0] = 0;
	dims[1] = 0;

	if (JVX_CHECK_SIZE_SELECTED(nlhs))
	{
		for (i = 0; i < nlhs; i++)
		{
			if (plhs[i] == NULL)
			{
				plhs[i] = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxREAL);
			}
		}
	}
	return(true);
}

jvxErrorType
CjvxExternalCall::fill_empty(jvxExternalDataType** plhs, jvxSize nlhs, jvxSize offset)
{
	mxArray** arr = (mxArray**)plhs;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	dims[0] = 0;
	dims[1] = 0;
	for(jvxSize l = offset; l < nlhs; l++)
	{
		arr[l] = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxREAL);
	}
	return(JVX_NO_ERROR);
};

/**
 * Constructor: Only print status on console output.
 *///=====================================================
CjvxExternalCall::CjvxExternalCall(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	this->_common_set.theComponentType.unselected(JVX_COMPONENT_EXTERNAL_CALL);
	this->_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxExternalCall*>(this));
	this->_common_set.thisisme = static_cast<IjvxObject*>(this);

#ifdef VERBOSE
	std::cout << "Starting Matlab Call Handler Object" << std::endl;
#endif
	this->threadIdMatlab = 0;
}

//! Matlab C conversion functions
jvxErrorType
CjvxExternalCall::convertCToExternal(jvxExternalDataType** retObject, const jvxHandle** fieldInput, jvxInt32 dimY, jvxInt32 dimX, jvxDataFormat processingFormat, jvxBool isCplx)
{
	if(this->checkThread())
	{
		if(retObject)
		{
			mxArray* ptr = NULL;
			bool resL = mexReturnGenericNumeric(ptr, fieldInput, dimY, dimX, processingFormat, isCplx);
			if (resL)
			{
				assert(ptr);
				*retObject = ptr;
				return(JVX_NO_ERROR);
			}
			return JVX_ERROR_INTERNAL;
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	}
	_common_set.theErrordescr = ERROR_THREAD_ID("convertCToExternal");
	return(JVX_ERROR_THREADING_MISMATCH);
}

//! Matlab C conversion functions
jvxErrorType
CjvxExternalCall::convertCToExternal(jvxExternalDataType** retObject, const jvxHandle* fieldInput, jvxInt32 dimX, jvxDataFormat processingFormat, jvxBool isCplx)
{
	return(convertCToExternal(retObject, &fieldInput, 1, dimX, processingFormat, isCplx));
}

jvxErrorType
CjvxExternalCall::convertCToExternal(jvxExternalDataType** retObject, const char* textInput)
{
	if(this->checkThread())
	{
		if(retObject)
		{
			mxArray* retVal = mxCreateString(textInput);
			*retObject = retVal;
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	}
	_common_set.theErrordescr = ERROR_THREAD_ID("convertCToExternal");
	return(JVX_ERROR_THREADING_MISMATCH);
}

jvxErrorType
CjvxExternalCall::convertCToExternal(jvxExternalDataType** retObject, const char** dict_tag, const char** dict_value, jvxSize numEntries)
{
	unsigned i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	SZ_MAT_TYPE ndiml = 2;
	SZ_MAT_TYPE dimsl[2] = {1, 1};
	mxArray* arr = NULL;
	mxArray* arr2 = NULL;
	mxArray* retVal = NULL;
	std::string fldName1 = FLD_NAME_DICT_TAG_STRING;
	std::string fldName2 = FLD_NAME_DICT_VALUE_STRING;
	const char* fld[2];
	fld[0] = fldName1.c_str();	
	fld[1] = fldName2.c_str();	
	if(this->checkThread())
	{
		if(retObject)
		{
			if(numEntries == 0)
			{
				dims[0] = 0;
				dims[1] = (int)0;
				retVal = mxCreateCellArray(ndim, dims);
			}
			else
			{
				dims[0] = numEntries;
				dims[1] = 1;
				retVal = mxCreateCellArray(ndim, dims);
				for(i = 0; i < numEntries; i++)
				{
					arr = NULL;
					arr = mxCreateStructArray(ndiml, dimsl, 2, fld);

					arr2 = NULL;
					mexReturnString(arr2, dict_tag[i]);
					mxSetFieldByNumber(arr, 0, 0, arr2);

					arr2 = NULL;
					mexReturnString(arr2, dict_value[i]);
					mxSetFieldByNumber(arr, 0, 1, arr2);

					mxSetCell(retVal, i, arr);
					arr = NULL;
				}
			}
			*retObject = retVal;
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	}
	_common_set.theErrordescr = ERROR_THREAD_ID("convertCToExternal");
	return(JVX_ERROR_THREADING_MISMATCH);
}
	
jvxErrorType
CjvxExternalCall::getPropertiesVariable(const jvxExternalDataType* varF, jvxDataFormat* format, bool* isString, jvxInt32* dimX, jvxInt32* dimY, jvxBool* convertFloats)
{
	jvxErrorType res = JVX_NO_ERROR;

	if(convertFloats)
		*convertFloats = false;

	if(this->checkThread())
	{
		const mxArray* var = (const mxArray*) varF;
		if(isString)
		{
			*isString = false;
		}

		if(mxIsChar(var))
		{
			if(isString)
			{
				*isString = true;
			}
		}

		if(format)
		{
			if(mxIsData(var))
			{
				*format = JVX_DATAFORMAT_DATA;
			}
			else if(mxIsInt64(var))
			{
				*format = JVX_DATAFORMAT_64BIT_LE;
			}
			else if(mxIsInt32(var))
			{
				*format = JVX_DATAFORMAT_32BIT_LE;
			}
			else if(mxIsInt16(var))
			{
				*format = JVX_DATAFORMAT_16BIT_LE;
			}
			else if(mxIsInt8(var))
			{
				*format = JVX_DATAFORMAT_8BIT;
			}
			else if (mxIsUint64(var))
			{
				*format = JVX_DATAFORMAT_U64BIT_LE;
			}
			else if (mxIsUint32(var))
			{
				*format = JVX_DATAFORMAT_U32BIT_LE;
			}
			else if (mxIsUint16(var))
			{
				*format = JVX_DATAFORMAT_U16BIT_LE;
			}
			else if (mxIsUint8(var))
			{
				*format = JVX_DATAFORMAT_U8BIT;
			}
			else
			{
				if(convertFloats)
				{
#ifdef JVX_DSP_DATA_FORMAT_FLOAT
					if(mxIsDouble(var))
					{
						*format = JVX_DATAFORMAT_DATA;
						*convertFloats = true;
					}
#else
					if(mxIsSingle(var))
					{
						*format = JVX_DATAFORMAT_DATA;
						*convertFloats = true;
					}
#endif
					else
					{
						*format = JVX_DATAFORMAT_NONE;
						res = JVX_ERROR_UNSUPPORTED;
					}
				}
				else
				{
					*format = JVX_DATAFORMAT_NONE;
					res = JVX_ERROR_UNSUPPORTED;
				}
			}
		}

		if(dimY)
		{
			*dimY = (jvxInt32)mxGetM(var);
		}
		if(dimX)
		{
			*dimX = (jvxInt32)mxGetN(var);
		}
	}
	else
	{
		_common_set.theErrordescr = ERROR_THREAD_ID("getPropertiesVariable");
		res = JVX_ERROR_THREADING_MISMATCH;
	}
	return(res);
}


//! Matlab C conversion functions
jvxErrorType
CjvxExternalCall::convertExternalToC(jvxHandle** fieldOutput, jvxInt32 dimY, jvxInt32 dimX,
		jvxDataFormat processingFormat, const jvxExternalDataType* ptrF, const char* nameVar, jvxBool convertFloat, jvxBool outputComplex)
{
	if(this->checkThread())
	{
		const mxArray* ptr = (const mxArray*) ptrF;

		jvxData* ptrDouble = NULL;
		jvxDataCplx* ptrDoubleCplx = NULL;
		float* ptrFloat = NULL;
		jvxInt32* ptrInt32 = NULL;
		jvxInt64* ptrInt64 = NULL;
		jvxInt16* ptrInt16 = NULL;
		jvxInt8* ptrInt8 = NULL;
		jvxUInt32* ptrUInt32 = NULL;
		jvxUInt64* ptrUInt64 = NULL;
		jvxUInt16* ptrUInt16 = NULL;
		jvxUInt8* ptrUInt8 = NULL;

		_common_set.theErrordescr = "";

		if(mxGetM(ptr) != dimY)
		{
			_common_set.theErrordescr = ((std::string)"Format of matrix " + nameVar +
				" does not fit to expected field in Y-dimension, expected " + jvx_int2String(dimY) + ", found " + jvx_int2String((int)mxGetM(ptr)));
			_common_set.theErrorid = 0;
			return(JVX_ERROR_INVALID_ARGUMENT);
		}
		if(mxGetN(ptr) != dimX)
		{
			_common_set.theErrordescr = ((std::string)"Format of matrix " + nameVar +
				" does not fit to expected field in X-dimension, expected " + jvx_int2String(dimX) + ", found " + jvx_int2String((int)mxGetN(ptr)));
			_common_set.theErrorid = 1;
			return(JVX_ERROR_INVALID_ARGUMENT);
		}

		switch(processingFormat)
		{
		case JVX_DATAFORMAT_DATA:
			if(mxIsData(ptr))
			{				
				if(outputComplex)
				{
					if (mxIsComplex(ptr))
					{
						ptrDoubleCplx = (jvxDataCplx*)mxGetData(ptr);
						for (int i = 0; i < dimX; i++)
						{
							for (int ii = 0; ii < dimY; ii++)
							{
								((jvxDataCplx*)fieldOutput[ii])[i] = ptrDoubleCplx[i * dimY + ii];
							}
						}
						ptrDoubleCplx = nullptr;
					}
					else
					{
						ptrDouble = (jvxData*)mxGetData(ptr);
						for (int i = 0; i < dimX; i++)
						{
							for (int ii = 0; ii < dimY; ii++)
							{
								((jvxDataCplx*)fieldOutput[ii])[i].re = ptrDouble[i * dimY + ii];
								((jvxDataCplx*)fieldOutput[ii])[i].im = 0;
							}
						}
						ptrDouble = NULL;
					}
					
				}
				else
				{
					ptrDouble = (jvxData*)mxGetData(ptr);
					for (int i = 0; i < dimX; i++)
					{
						for (int ii = 0; ii < dimY; ii++)
						{
							((jvxData*)fieldOutput[ii])[i] = ptrDouble[i * dimY + ii];
						}
					}
					ptrDouble = NULL;
				}
			}
			else
			{
				if(convertFloat)
				{
#ifdef JVX_DSP_DATA_FORMAT_FLOAT
					if(mxIsDouble(ptr))
					{
						double* ptrD = (double*) mxGetData(ptr);
						for(int i = 0; i < dimX; i++)
						{
							for(int ii = 0; ii < dimY; ii++)
							{
								((jvxData*)fieldOutput[ii])[i] = (jvxData)ptrD[i*dimY + ii];
							}
						}
					}
#else
					if(mxIsSingle(ptr))
					{
						float* ptrF = (float*) mxGetData(ptr);
						for(int i = 0; i < dimX; i++)
						{
							for(int ii = 0; ii < dimY; ii++)
							{
								((jvxData*)fieldOutput[ii])[i] = (jvxData)ptrF[i*dimY + ii];
							}
						}
					}
#endif
					else
					{
						_common_set.theErrordescr  = ((std::string)"Type of matrix " +
						nameVar + " does not match the expected float type, expected float type is " + JVX_DATA_DESCRIPTOR_THE_OTHER);
						_common_set.theErrorid = 2;
						return(JVX_ERROR_INVALID_ARGUMENT);
					}
				}
				else
				{
					_common_set.theErrordescr  = ((std::string)"Type of matrix " +
						nameVar + " does not match the expected type, expected type is " + JVX_DATA_DESCRIPTOR);
					_common_set.theErrorid = 2;
					return(JVX_ERROR_INVALID_ARGUMENT);
				}
			}
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			if(!mxIsInt32(ptr))
			{
				_common_set.theErrordescr  = ((std::string)"Type of matrix " + nameVar +
					" does not match the expected type, expected type is int32");
				_common_set.theErrorid = 4;
				return(JVX_ERROR_INVALID_ARGUMENT);
			}
			ptrInt32 = (jvxInt32*) mxGetData(ptr);
			for(int i = 0; i < dimX; i++)
			{
				for(int ii = 0; ii < dimY; ii++)
				{
					((jvxInt32*)fieldOutput[ii])[i] = ptrInt32[i*dimY + ii];
				}
			}
			ptrInt32 = NULL;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			if(!mxIsInt64(ptr))
			{
				_common_set.theErrordescr  = ((std::string)"Type of matrix " + nameVar +
					" does not match the expected type, expected type is int64");
				_common_set.theErrorid = 4;
				return(JVX_ERROR_INVALID_ARGUMENT);
			}
			ptrInt64 = (jvxInt64*) mxGetData(ptr);
			for(int i = 0; i < dimX; i++)
			{
				for(int ii = 0; ii < dimY; ii++)
				{
					((jvxInt64*)fieldOutput[ii])[i] = ptrInt64[i*dimY + ii];
				}
			}
			ptrInt64 = NULL;
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			if(!mxIsInt16(ptr))
			{
				_common_set.theErrordescr  = ((std::string)"Type of matrix " + nameVar +
					" does not match the expected type, expected type is int16");
				_common_set.theErrorid = 5;
				return(JVX_ERROR_INVALID_ARGUMENT);
			}
			ptrInt16 = (jvxInt16*) mxGetData(ptr);
			for(int i = 0; i < dimX; i++)
			{
				for(int ii = 0; ii < dimY; ii++)
				{
					((jvxInt16*)fieldOutput[ii])[i] = ptrInt16[i*dimY + ii];
				}
			}
			ptrInt16 = NULL;
			break;
		case JVX_DATAFORMAT_8BIT:
			if(!mxIsInt8(ptr))
			{
				_common_set.theErrordescr  = ((std::string)"Type of matrix " + nameVar +
					" does not match the expected type, expected type is int8");
				_common_set.theErrorid = 5;
				return(JVX_ERROR_INVALID_ARGUMENT);
			}
			ptrInt8 = (jvxInt8*) mxGetData(ptr);
			for(int i = 0; i < dimX; i++)
			{
				for(int ii = 0; ii < dimY; ii++)
				{
					((jvxInt8*)fieldOutput[ii])[i] = ptrInt8[i*dimY + ii];
				}
			}
			ptrInt8 = NULL;
			break;



		case JVX_DATAFORMAT_U32BIT_LE:
			if (!mxIsUint32(ptr))
			{
				_common_set.theErrordescr = ((std::string)"Type of matrix " + nameVar +
					" does not match the expected type, expected type is uint32");
				_common_set.theErrorid = 4;
				return(JVX_ERROR_INVALID_ARGUMENT);
			}
			ptrUInt32 = (jvxUInt32*)mxGetData(ptr);
			for (int i = 0; i < dimX; i++)
			{
				for (int ii = 0; ii < dimY; ii++)
				{
					((jvxUInt32*)fieldOutput[ii])[i] = ptrUInt32[i*dimY + ii];
				}
			}
			ptrUInt32 = NULL;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			if (!mxIsUint64(ptr))
			{
				_common_set.theErrordescr = ((std::string)"Type of matrix " + nameVar +
					" does not match the expected type, expected type is int64");
				_common_set.theErrorid = 4;
				return(JVX_ERROR_INVALID_ARGUMENT);
			}
			ptrUInt64 = (jvxUInt64*)mxGetData(ptr);
			for (int i = 0; i < dimX; i++)
			{
				for (int ii = 0; ii < dimY; ii++)
				{
					((jvxUInt64*)fieldOutput[ii])[i] = ptrUInt64[i*dimY + ii];
				}
			}
			ptrUInt64 = NULL;
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			if (!mxIsUint16(ptr))
			{
				_common_set.theErrordescr = ((std::string)"Type of matrix " + nameVar +
					" does not match the expected type, expected type is int16");
				_common_set.theErrorid = 5;
				return(JVX_ERROR_INVALID_ARGUMENT);
			}
			ptrUInt16 = (jvxUInt16*)mxGetData(ptr);
			for (int i = 0; i < dimX; i++)
			{
				for (int ii = 0; ii < dimY; ii++)
				{
					((jvxUInt16*)fieldOutput[ii])[i] = ptrUInt16[i*dimY + ii];
				}
			}
			ptrUInt16 = NULL;
			break;
		case JVX_DATAFORMAT_U8BIT:
			if (!mxIsUint8(ptr))
			{
				_common_set.theErrordescr = ((std::string)"Type of matrix " + nameVar +
					" does not match the expected type, expected type is int8");
				_common_set.theErrorid = 5;
				return(JVX_ERROR_INVALID_ARGUMENT);
			}
			ptrUInt8 = (jvxUInt8*)mxGetData(ptr);
			for (int i = 0; i < dimX; i++)
			{
				for (int ii = 0; ii < dimY; ii++)
				{
					((jvxUInt8*)fieldOutput[ii])[i] = ptrUInt8[i*dimY + ii];
				}
			}
			ptrUInt8 = NULL;
			break;


		case JVX_DATAFORMAT_SIZE:
			if (mxIsDouble(ptr))
			{
				double* ptrD = (double*)mxGetData(ptr);
				for (int i = 0; i < dimX; i++)
				{
					for (int ii = 0; ii < dimY; ii++)
					{
						((jvxSize*)fieldOutput[ii])[i] = JVX_DATA2SIZE(ptrD[i*dimY + ii]);
					}
				}
				ptrD = NULL;
			}
			else if (mxIsSingle(ptr))
			{
				float* ptrF = (float*)mxGetData(ptr);
				for (int i = 0; i < dimX; i++)
				{
					for (int ii = 0; ii < dimY; ii++)
					{
						((jvxSize*)fieldOutput[ii])[i] = JVX_DATA2SIZE(ptrF[i*dimY + ii]);
					}
				}
				ptrF = NULL;
			}
			else if (mxIsInt8(ptr))
			{
				ptrInt8 = (jvxInt8*)mxGetData(ptr);
				for (int i = 0; i < dimX; i++)
				{
					for (int ii = 0; ii < dimY; ii++)
					{
						((jvxSize*)fieldOutput[ii])[i] = ptrInt8[i*dimY + ii];
					}
				}
				ptrInt8 = NULL;
			}
			else if (mxIsInt16(ptr))
			{
				ptrInt16 = (jvxInt16*)mxGetData(ptr);
				for (int i = 0; i < dimX; i++)
				{
					for (int ii = 0; ii < dimY; ii++)
					{
						((jvxSize*)fieldOutput[ii])[i] = ptrInt16[i*dimY + ii];
					}
				}
				ptrInt16 = NULL;
			}
			else if (mxIsInt32(ptr))
			{
				ptrInt32 = (jvxInt32*)mxGetData(ptr);
				for (int i = 0; i < dimX; i++)
				{
					for (int ii = 0; ii < dimY; ii++)
					{
						((jvxSize*)fieldOutput[ii])[i] = ptrInt32[i*dimY + ii];
					}
				}
				ptrInt32 = NULL;
			}
			else if (mxIsInt64(ptr))
			{
				ptrInt64 = (jvxInt64*)mxGetData(ptr);
				for (int i = 0; i < dimX; i++)
				{
					for (int ii = 0; ii < dimY; ii++)
					{
						((jvxSize*)fieldOutput[ii])[i] = ptrInt64[i*dimY + ii];
					}
				}
				ptrInt64 = NULL;
			}
			else
			{
				_common_set.theErrordescr = ((std::string)"Type of matrix " + nameVar +
					" does not match the expected type, it may be of type int8, int16, int32, int64, single or double.");
				_common_set.theErrorid = 6;
				return(JVX_ERROR_INVALID_ARGUMENT);
			}
			break;
		default:
		  assert(0);
		}
		return(JVX_NO_ERROR);
	}
	_common_set.theErrordescr  = ERROR_THREAD_ID("convertExternalToC");
	return(JVX_ERROR_THREADING_MISMATCH);
}

jvxErrorType
CjvxExternalCall::convertExternalToC(jvxApiString* textOutput, const jvxExternalDataType* ptrF, const char* nameVar)
{
	if(this->checkThread())
	{
		const mxArray* ptr = (const mxArray*) ptrF;

		if(mxIsChar(ptr))
		{
			std::string nm;
			int strLen = (int)mxGetN(ptr)+1;
			char* fld = new char[strLen+1];
			memset(fld, 0, sizeof(char) * strLen);
			mxGetString(ptr, fld, strLen);
			nm = fld;
			delete[](fld);
			if (textOutput)
			{
				textOutput->assign(nm);
			}
			return(JVX_NO_ERROR);
		}
		_common_set.theErrordescr  = ((std::string)"Type of variable " + nameVar +
			" is not of type string.");
		_common_set.theErrorid = 6;
		return(JVX_ERROR_INVALID_ARGUMENT);
	}
	_common_set.theErrordescr  = ERROR_THREAD_ID("convertExternalToC");
	return(JVX_ERROR_THREADING_MISMATCH);
}

//! Matlab C conversion functions
jvxErrorType
CjvxExternalCall::convertExternalToC(void* fieldOutput, unsigned dimX, jvxDataFormat processingFormat,
		const jvxExternalDataType* ptr, const char* nameVar, jvxBool convertFloat, jvxBool outputComplex)
{
	return(convertExternalToC(&fieldOutput, 1, dimX, processingFormat, ptr, nameVar, convertFloat, outputComplex));
}


jvxErrorType
CjvxExternalCall::getLastErrorReason(jvxApiString* err_argout)
{
	if (err_argout)
	{
		err_argout->assign(_common_set.theErrordescr);
	}
	_common_set.theErrordescr  = "No Error";

	return(JVX_NO_ERROR);
}

/*
jvxErrorType
CjvxExternalCall::releaseString(jvxString* err_argout)
{
	HjvxObject_deallocate(err_argout);
	_common_set.theErrordescr  = "";
	_common_set.theErrorid = -1;
	//_errorSubID = -1;
	//_errorType = JVX_ERROR_NO_ERROR;
	return(JVX_NO_ERROR);
}
*/

/**
 * Destructor: Only print status on console output.
 *///=====================================================
CjvxExternalCall::~CjvxExternalCall()
{
#ifdef VERBOSE
	std::cout << "Stopping Matlab Call Handler Object" << std::endl;
#endif
}

jvxErrorType
CjvxExternalCall::executeExternalCommand(const char* evalString, jvxBool catchAllExceptions)
{
	std::string errText;
	if(this->checkThread())
	{
		jvxErrorType res = JVX_NO_ERROR;
		try
		{
#ifdef CONFIG_COMPILE_FOR_OCTAVE
			int returnVal = mexEvalString(evalString);
			if (returnVal != 0)
			{
				errText = "Unknown";
				_common_set.theErrordescr = errText;
				return(JVX_ERROR_SYSTEMCALL_FAILED);
			}
#elif defined CONFIG_COMPILE_FOR_MATLAB
#if _MATLAB_MEXVERSION >= 250
			mxArray* ptrMex = mexEvalStringWithTrap(evalString);
			if (ptrMex != 0)
			{
				mxArray* ptrReport;
				mxArray* ptrMEX2 = mexCallMATLABWithTrap(1, &ptrReport, 1, &ptrMex, "getReport");
				errText = "Unknown";
				if (ptrReport)
				{
					if (mxIsChar(ptrReport))
					{
						int strLen = (int)mxGetN(ptrReport) + 1;
						char* fld = new char[strLen + 1];
						memset(fld, 0, sizeof(char) * strLen);
						mxGetString(ptrReport, fld, strLen);
						errText = fld;
						delete[](fld);
					}
				}

				_common_set.theErrordescr = errText;
				return(JVX_ERROR_SYSTEMCALL_FAILED);
			}
#else
			int returnVal = mexEvalString(evalString);
			if (returnVal != 0)
			{
				errText = "Unknown";
				_common_set.theErrordescr = errText;
				return JVX_ERROR_UNKNOWN;
			}
#endif
#else
#error "Must define CONFIG_COMPILE_FOR_MATLAB or CONFIG_COMPILE_FOR_OCTAVE"
#endif
		}
		catch (...)
		{
			_common_set.theErrordescr = "Call to Matlab command raised an exception. This typically happens if debugging was stopped.";
			res = JVX_ERROR_STANDARD_EXCEPTION;
		}
		return(res);
	}
	_common_set.theErrordescr = ERROR_THREAD_ID("executeExternalCommand");
	return(JVX_ERROR_THREADING_MISMATCH);
}

jvxErrorType
CjvxExternalCall::putVariableToExternal(const char* externalInformation, const char* variableName,
										 jvxExternalDataType* data)
{
	if(this->checkThread())
	{
		if(mexPutVariable(externalInformation, variableName, (mxArray*)data) == 0)
		{
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INTERNAL);
	}
	_common_set.theErrordescr = ERROR_THREAD_ID("putVariableToExternal");
	return(JVX_ERROR_THREADING_MISMATCH);
}

jvxErrorType
CjvxExternalCall::getVariableFromExternal(const char* externalInformation, const char* variableName,
										 jvxExternalDataType** data)
{
	if(this->checkThread())
	{
		mxArray* ptr = mexGetVariable(externalInformation, variableName);
		if(data)
		{
			*data = (jvxExternalDataType*)ptr;
		}

		if(ptr)
		{
			return(JVX_NO_ERROR);
		}
		_common_set.theErrordescr  = "Failed to get variable " + (std::string)variableName + " (context: " + (std::string)externalInformation + ").";
		return(JVX_ERROR_INTERNAL);
	}
	_common_set.theErrordescr = ERROR_THREAD_ID("getVariableFromExternal");
	return(JVX_ERROR_THREADING_MISMATCH);
}

jvxErrorType
CjvxExternalCall::destroyExternalVariable(jvxExternalDataType* data)
{
	if(this->checkThread())
	{
		if(data)
		{
			mxDestroyArray((mxArray*)data);
		}

		return(JVX_NO_ERROR);
	}
	_common_set.theErrordescr = ERROR_THREAD_ID("destroyExternalVariable");
	return(JVX_ERROR_THREADING_MISMATCH);
}

jvxErrorType
CjvxExternalCall::rtpPrintf(const char* text)
{
	if(this->checkThread())
	{
		mexPrintf(text);
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_THREADING_MISMATCH);
}

bool
CjvxExternalCall::checkThread()
{
	bool rightThread = false;
	if(this->threadIdMatlab != 0)
	{
		JVX_THREAD_ID threadIdLocal = JVX_GET_CURRENT_THREAD_ID();
		if(JVX_COMPARE_THREADS(threadIdLocal,  this->threadIdMatlab))
		{
			rightThread = true;
		}
	}
	else
	{
		rightThread = true;
	}
	return(rightThread);
}
