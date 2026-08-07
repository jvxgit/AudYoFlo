#ifndef __JVXCTOPYTHONCONVERTER_H__
#define __JVXCTOPYTHONCONVERTER_H__

#include "jvx.h"
#include "localPybindIncludes.h"

#define FLD_NAME_ERROR_DESCRIPTION_STRING "DESCRIPTION_STRING"
#define FLD_NAME_ERROR_ID_INT32 "ERRORID_INT32"

#define FLD_NAME_PROPERTY_SELLIST_OPTIONS "OPTIONS"
#define FLD_NAME_PROPERTY_SELLIST_SELECTION_BITFIELD "SELECTION_BITFIELD"
#define FLD_NAME_PROPERTY_SELLIST_EXCULSIVE_BITFIELD "EXCLUSIVE_BITFIELD"

#define FLD_NAME_PROPERTY_VALUE_IN_RANGE_MIN_DBL "VALUE_IN_RANGE_MIN_DBL"
#define FLD_NAME_PROPERTY_VALUE_IN_RANGE_MAX_DBL "VALUE_IN_RANGE_MAX_DBL"
#define FLD_NAME_PROPERTY_VALUE_IN_RANGE_VALUE_DBL "VALUE_IN_RANGE_VALUE_DBL"

// ==============================================================================
// Python analog of CjvxCToMatlabConverter: converts C/jvx values into
// pybind11 objects (return-by-value instead of the mxArray*& out-parameter
// style used by the Matlab/Octave variant, since pybind11::object is already
// a reference-counted RAII handle).
// ==============================================================================

class CjvxCToPythonConverter
{
public:
	CjvxCToPythonConverter() {};

	//! Translate a list of int32 values into a python object
	static pybind11::object pyReturnFieldInt32(std::vector<jvxInt32>& lstStr);

	static pybind11::object pyReturnInt8(const jvxInt8& value);
	static pybind11::object pyReturnInt16(const jvxInt16& value);
	static pybind11::object pyReturnUInt16(const jvxUInt16& value);
	static pybind11::object pyReturnInt32(const jvxInt32& value);
	static pybind11::object pyReturnUInt32(const jvxUInt32& value);
	static pybind11::object pyReturnInt64(const jvxInt64& value);
	static pybind11::object pyReturnUInt64(const jvxUInt64& value);

	static pybind11::object pyReturnIntPtr(const void* valueptr);

	static pybind11::object pyReturnInt8List(const jvxInt8* value, jvxSize numberValues);
	static pybind11::object pyReturnInt16List(const jvxInt16* value, jvxSize numberValues);
	static pybind11::object pyReturnInt32List(const jvxInt32* value, jvxSize numberValues);
	static pybind11::object pyReturnInt64List(const jvxInt64* value, jvxSize numberValues);

	static pybind11::object pyReturnData(const jvxData& value);
	static pybind11::object pyReturnDataList(const jvxData* value, jvxSize numberValues);

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
	static pybind11::object pyReturnFloatList(const float* ptrField, jvxSize numElements);
#else
	static pybind11::object pyReturnDoubleList(const double* ptrField, jvxSize numElements);
#endif

	static pybind11::object pyReturnString(const std::string& str);
	static pybind11::object pyReturnStringList(const std::vector<std::string>& str);
	static pybind11::object pyReturnBool(jvxBool trueFalse);
	static pybind11::object pyReturnErrorCode(jvxErrorType tp);

	//! Return negative result for wrong usage
	static pybind11::object pyReturnAnswerNegativeResult(const std::string& expr, jvxErrorType errCode);

	static pybind11::object pyReturnBitField(const jvxBitField& value);
	static pybind11::object pyReturnCBitField(const jvxCBitField& value);

	static pybind11::object pyReturnSelectionListStruct(const jvxSelectionList& theSelList);
	static pybind11::object pyReturnValueInRangeStruct(const jvxValueInRange& valRange);

	static pybind11::object pyReturnConfigSection(jvxConfigData* theSection, IjvxConfigProcessor* proc);

	static pybind11::object pyReturnDataFieldCell(std::vector<jvxData*> lstDbls, std::vector<jvxSize> lengths);

	static pybind11::object pyReturnGenericNumeric(const jvxHandle** fieldInput, jvxInt32 dimY, jvxInt32 dimX, jvxDataFormat processingFormat, jvxBool isCplx = false);

	static jvxErrorType convertPyToC(jvxHandle** fieldOutput, jvxInt32 dimY, jvxInt32 dimX,
		jvxDataFormat processingFormat, const pybind11::object& pyArr, const char* nameVar, bool convertFloat,
		std::string& theErrordescr);
};

#endif
