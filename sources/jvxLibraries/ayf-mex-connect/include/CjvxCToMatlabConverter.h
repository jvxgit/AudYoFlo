#ifndef __JVXCTOMATLABCONVERTER_H__
#define __JVXCTOMATLABCONVERTER_H__

#include "jvx.h"

#define FLD_NAME_ERROR_DESCRIPTION_STRING "DESCRIPTION_STRING"
#define FLD_NAME_ERROR_ID_INT32 "ERRORID_INT32"

#define FLD_NAME_PROPERTY_SELLIST_OPTIONS "OPTIONS"
#define FLD_NAME_PROPERTY_SELLIST_SELECTION_BITFIELD "SELECTION_BITFIELD"
#define FLD_NAME_PROPERTY_SELLIST_EXCULSIVE_BITFIELD "EXCLUSIVE_BITFIELD"

#define FLD_NAME_PROPERTY_VALUE_IN_RANGE_MIN_DBL "VALUE_IN_RANGE_MIN_DBL"
#define FLD_NAME_PROPERTY_VALUE_IN_RANGE_MAX_DBL "VALUE_IN_RANGE_MAX_DBL"
#define FLD_NAME_PROPERTY_VALUE_IN_RANGE_VALUE_DBL "VALUE_IN_RANGE_VALUE_DBL"

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

	static bool mexReturnGenericNumeric(mxArray*& retObject, const jvxHandle** fieldInput, jvxInt32 dimY, jvxInt32 dimX, jvxDataFormat processingFormat, jvxBool isCplx = false);

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
