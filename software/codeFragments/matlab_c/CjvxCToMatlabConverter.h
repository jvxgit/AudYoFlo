#ifndef __JVXCTOMATLABCONVERTER_H__
#define __JVXCTOMATLABCONVERTER_H__

#include "jvx.h"
#include "codeFragments/matlab_c/HjvxMex2CConverter.h"

#define FLD_NAME_ERROR_DESCRIPTION_STRING "DESCRIPTION_STRING"
#define FLD_NAME_ERROR_ID_INT32 "ERRORID_INT32"

#define FLD_NAME_PROPERTY_SELLIST_OPTIONS "OPTIONS"
#define FLD_NAME_PROPERTY_SELLIST_SELECTION_BITFIELD "SELECTION_BITFIELD"
#define FLD_NAME_PROPERTY_SELLIST_EXCULSIVE_BITFIELD "EXCLUSIVE_BITFIELD"

#define FLD_NAME_PROPERTY_VALUE_IN_RANGE_MIN_DBL "VALUE_IN_RANGE_MIN_DBL"
#define FLD_NAME_PROPERTY_VALUE_IN_RANGE_MAX_DBL "VALUE_IN_RANGE_MAX_DBL"
#define FLD_NAME_PROPERTY_VALUE_IN_RANGE_VALUE_DBL "VALUE_IN_RANGE_VALUE_DBL"

template<typename T> 
static jvxErrorType mexArgument2Index(T& value, const mxArray** thePointer, jvxSize idx,jvxSize numEntries)
{
	jvxInt64 valueT = -1;
	jvxErrorType res = JVX_NO_ERROR;

	if(idx < numEntries)
	{
		if(thePointer[idx])
		{
			if(mxIsDouble(thePointer[idx]))
			{
				valueT = JVX_DDOUBLE2INT64(*((double*)mxGetData(thePointer[idx])));
			}
			else if(mxIsSingle(thePointer[idx]))
			{
				valueT = JVX_DFLOAT2INT64(*((float*)mxGetData(thePointer[idx])));
			}
			else if(mxIsInt64(thePointer[idx]))
			{
				valueT = (*((jvxInt64*)mxGetData(thePointer[idx])));
			}
			else if(mxIsInt32(thePointer[idx]))
			{
				valueT = (*((jvxInt32*)mxGetData(thePointer[idx])));
			}
			else if(mxIsInt16(thePointer[idx]))
			{
				valueT = (*((jvxInt16*)mxGetData(thePointer[idx])));
			}
			else if(mxIsInt8(thePointer[idx]))
			{
				valueT = (*((jvxInt8*)mxGetData(thePointer[idx])));
			}
			else if(mxIsUint64(thePointer[idx]))
			{
				valueT = (*((jvxUInt64*)mxGetData(thePointer[idx])));
			}
			else if(mxIsUint32(thePointer[idx]))
			{
				valueT = (*((jvxUInt32*)mxGetData(thePointer[idx])));
			}
			else if(mxIsUint16(thePointer[idx]))
			{
				valueT = (*((jvxUInt16*)mxGetData(thePointer[idx])));
			}
			else if(mxIsUint8(thePointer[idx]))
			{
				valueT = (*((jvxUInt8*)mxGetData(thePointer[idx])));
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}

			value = (T)valueT;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return(res);
}
	



static jvxErrorType mexArgument2Bool(bool& value, const mxArray** thePointer, jvxSize idx,jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;

	if(idx < numEntries)
	{
		if(thePointer[idx])
		{
			if(mxIsLogical(thePointer[idx]))
			{
				value = *((bool*)mxGetData(thePointer[idx]));
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return(res);
}

static jvxErrorType mexArgument2String(std::string& value, const mxArray** thePointer, jvxSize idx,jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;

	value = "";
		
	if(idx < numEntries)
	{
		if(thePointer[idx])
		{
			if(mxIsChar(thePointer[idx]))
			{
				value = jvx_mex_2_cstring(thePointer[idx]);
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return(res);
}

template<typename T> 
static jvxErrorType mexArgument2Type(T& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries, T defVal, jvxSize idxLim, jvxTextHelpers* strPtr)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string cpName = "unknown";
	jvxInt32 valI = -1;
	value = defVal;

	res = mexArgument2Index<jvxInt32>(valI, thePointer, idx, numEntries);
	if (res == JVX_NO_ERROR)
	{
		if ((valI >= 0) && (valI < JVX_COMPONENT_ALL_LIMIT))
		{
			value = (T)valI;
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = mexArgument2String(cpName, thePointer, idx, numEntries);
		if (res == JVX_NO_ERROR)
		{
			jvxSize idx_entry = 0;
			while (idx_entry < JVX_COMPONENT_ALL_LIMIT)
			{
				if (
					(strPtr[idx_entry].friendly == cpName) ||
					(strPtr[idx_entry].full == cpName))
				{
					break;
				}
				idx_entry++;
			}
			if (idx_entry == JVX_COMPONENT_ALL_LIMIT)
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
			else
			{
				value = (T)idx_entry;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	return(res);
}

static jvxErrorType mexArgument2ComponentIdentification(jvxComponentIdentification& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string cpName = "unknown";
	jvxInt32 valI = -1;
	std::string nm;

	value.reset();

	res = mexArgument2String(nm, thePointer, idx, numEntries);
	if (res == JVX_NO_ERROR)
	{
		res = jvxComponentIdentification_decode(value, nm);
	}
	else
	{
		res = mexArgument2Type<jvxComponentType>(value.tp, thePointer, idx, numEntries, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
	}
	return(res);
}

static jvxErrorType mexArgument2String(std::vector<std::string>& valueList, jvxSize expectedNumberEntries,const mxArray** thePointer, jvxSize idx,jvxSize numEntries)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	const mxArray* arr = NULL;
	valueList.clear();
	if(idx < numEntries)
	{
		if(thePointer[idx])
		{
			if(mxIsCell(thePointer[idx]))
			{
				jvxSize numEntries = mxGetNumberOfElements(thePointer[idx]);
				if(expectedNumberEntries == numEntries)
				{
					for(i = 0; i < numEntries; i ++)
					{
						arr = mxGetCell(thePointer[idx], JVX_SIZE_2_MAT_IDX(i));
						if(arr)
						{
							if(mxIsChar(arr))
							{
								valueList.push_back(jvx_mex_2_cstring(arr));
							}
							else
							{
								res = JVX_ERROR_INVALID_ARGUMENT;
							}
						}
						else
						{
							res = JVX_ERROR_INVALID_ARGUMENT;
						}
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return(res);
}

static jvxErrorType mexArgument2StringList(std::vector<std::string>& valueList, const mxArray** thePointer, jvxSize idx,jvxSize numEntries)
{
	jvxSize i;
	std::string oneToken;
	jvxErrorType res = JVX_NO_ERROR;
	const mxArray* arr = NULL;
	valueList.clear();
	if(idx < numEntries)
	{
		if(thePointer[idx])
		{
			if(mxIsCell(thePointer[idx]))
			{
				jvxSize numEntries = mxGetNumberOfElements(thePointer[idx]);
				for(i = 0; i < numEntries; i ++)
				{
					arr = mxGetCell(thePointer[idx], JVX_SIZE_2_MAT_IDX(i));
					if(arr)
					{
						if(mxIsChar(arr))
						{
							valueList.push_back(jvx_mex_2_cstring(arr));
						}
						else
						{
							res = JVX_ERROR_INVALID_ARGUMENT;
						}
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
				}
			}
			else if(mxIsChar(thePointer[idx]))
			{
				// There may also be just one string
				oneToken = jvx_mex_2_cstring(thePointer[idx]);
				valueList.push_back(oneToken);
			}
			else if(mxIsEmpty(thePointer[idx]))
			{
				// No entry may also happen
			}
			else 
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return(res);
}

static jvxErrorType mexArgument2Data(jvxData& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;
	value = 0.0;

	if (idx < numEntries)
	{
		if (thePointer[idx])
		{
			if (mxIsDouble(thePointer[idx]))
			{
				value = (jvxData)(*((double*)mxGetData(thePointer[idx])));
			}
			else if (mxIsSingle(thePointer[idx]))
			{
				value = (jvxData)(*((float*)mxGetData(thePointer[idx])));
			}
			else if (mxIsInt64(thePointer[idx]))
			{
				value = (jvxData)(*((jvxInt64*)mxGetData(thePointer[idx])));
			}
			else if (mxIsInt32(thePointer[idx]))
			{
				value = (jvxData)(*((jvxInt32*)mxGetData(thePointer[idx])));
			}
			else if (mxIsInt16(thePointer[idx]))
			{
				value = (jvxData)(*((jvxInt16*)mxGetData(thePointer[idx])));
			}
			else if (mxIsInt8(thePointer[idx]))
			{
				value = (jvxData)(*((jvxInt8*)mxGetData(thePointer[idx])));
			}
			else if (mxIsUint64(thePointer[idx]))
			{
				value = (jvxData)(*((jvxUInt64*)mxGetData(thePointer[idx])));
			}
			else if (mxIsUint32(thePointer[idx]))
			{
				value = (jvxData)(*((jvxUInt32*)mxGetData(thePointer[idx])));
			}
			else if (mxIsUint16(thePointer[idx]))
			{
				value = (jvxData)(*((jvxUInt16*)mxGetData(thePointer[idx])));
			}
			else if (mxIsUint8(thePointer[idx]))
			{
				value = (jvxData)(*((jvxUInt8*)mxGetData(thePointer[idx])));
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return(res);
}

// ==============================================================================
// ==============================================================================

class CjvxCToMatlabConverter
{
public:
	CjvxCToMatlabConverter(){};

	//! Translate data in C to MEX type
	static void mexReturnFieldInt32(mxArray*& plhs, std::vector<jvxInt32>& lstStr);

    
    //! Translate data in C to MEX type
	static void mexReturnInt8(mxArray*& plhs, const jvxInt8& value);

    //! Translate data in C to MEX type
	static void mexReturnInt16(mxArray*& plhs, const jvxInt16& value);

	static void mexReturnUInt16(mxArray*& plhs, const jvxUInt16& value);

    //! Translate data in C to MEX type
	static void mexReturnInt32(mxArray*& plhs, const jvxInt32& value);

	static void mexReturnUInt32(mxArray*& plhs, const jvxUInt32& value);

    //! Return data in MEX format
	static void mexReturnInt64(mxArray*& plhs, const jvxInt64& value);

	static void mexReturnUInt64(mxArray*& plhs, const jvxUInt64& value);

    
    //! Translate data in C to MEX type
	static void mexReturnInt8List(mxArray*& plhs, const jvxInt8* value, jvxSize numberValues);

    //! Translate data in C to MEX type
	static void mexReturnInt16List(mxArray*& plhs, const jvxInt16* value, jvxSize numberValues);

    //! Translate data in C to MEX type
	static void mexReturnInt32List(mxArray*& plhs, const jvxInt32* value, jvxSize numberValues);

    //! Translate data in C to MEX type
	static void mexReturnInt64List(mxArray*& plhs, const jvxInt64* value, jvxSize numberValues);


    
    //! Translate data in C to MEX type
	static void mexReturnData(mxArray*& plhs, const jvxData& value);

	//! Translate data in C to MEX type
	static void mexReturnDataList(mxArray*& plhs, const jvxData* value, jvxSize numberValues);

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
	static void mexReturnFloatList(mxArray*& plhs, const float* ptrField, jvxSize numElements);
#else
	static void mexReturnDoubleList(mxArray*& plhs, const double* ptrField, jvxSize numElements);
#endif

    
    
    
    //! Translate data in C to MEX type
	static void mexReturnString(mxArray*& plhs, const std::string& str);

	//! Translate data in C to MEX type
	static void mexReturnStringCell(mxArray*& plhs, const std::vector<std::string>& str);

	//! Translate data in C to MEX type
	static void mexReturnBool(mxArray*& plhs, jvxBool trueFalse);

	static void mexReturnErrorCode(mxArray*& plhs, jvxErrorType tp);

	//! Return negative result for wrong usage
	static void mexReturnAnswerNegativeResult(mxArray*&plhs, const std::string& expr, jvxErrorType errCode);


	static void mexReturnBitField(mxArray*& plhs, const jvxBitField& value);

	static void mexReturnCBitField(mxArray*& plhs, const jvxCBitField& value);
	
    
	static void mexFillEmpty(mxArray** plhs, jvxSize nlhs, jvxSize offset);

	static void mexReturnSelectionListStruct(mxArray*& plhs, const jvxSelectionList& theSelList);

	static void mexReturnValueInRangeStruct(mxArray*& plhs, const jvxValueInRange& valRange);


	static bool mexReturnNumericMatrix(mxArray*& plhs, void** content, jvxDataFormat form, int dimY, int dimX);

	static bool mexReturnStructSection(mxArray*& plhs, jvxConfigData* theSection, IjvxConfigProcessor* proc);

	static bool mexReturnDataFieldCell(mxArray*& plhs, std::vector<jvxData*> lstDbls, std::vector<jvxSize> lengths);

	static bool mexReturnGenericNumeric(mxArray*& retObject, const jvxHandle** fieldInput, jvxInt32 dimY, jvxInt32 dimX, jvxDataFormat processingFormat);

	static jvxErrorType convertMexToC(jvxHandle** fieldOutput, jvxInt32 dimY, jvxInt32 dimX,
		jvxDataFormat processingFormat, const jvxExternalDataType* ptrF, const char* nameVar, bool convertFloat,
		std::string& theErrordescr);

	//==============================================================================
	// Memory management
	//==============================================================================

	//! Browse through the list of allocated fields and erase the passed pointer
	static void browseThroughListAndDeallocate(std::list<jvxByte*>& lstPointers, jvxByte* ptrSearch);

	//! Combine two lists of garbage collections
	static void combinePointerLists(std::list<jvxByte*>& lstFirst, std::list<jvxByte*>& lstSecond);

	static void deallocateGarbageLists(std::list<jvxByte*>& lstGarbage);
};
 
#endif
