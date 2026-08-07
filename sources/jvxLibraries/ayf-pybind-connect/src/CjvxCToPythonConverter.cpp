// Need to include jvx.h here since the order of inclusion matters!
#include "jvx.h"

#include "localPybindIncludes.h"
#include "CjvxCToPythonConverter.h"
#include "CjvxPythonToCConverter.h"

#include <complex>

//======================================================================
//======================================================================
// PY-RETURNS PY-RETURNS PY-RETURNS PY-RETURNS PY-RETURNS PY-RETURNS
//======================================================================
//======================================================================

//==================================================================================================
// Analog of CjvxCToMatlabConverter.cpp, mapping onto pybind11 objects instead of mxArray.
//==================================================================================================

/**
 * Function to return a list of int32 values as a python object.
 *///===============================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnFieldInt32(std::vector<jvxInt32>& lstStr)
{
	if (lstStr.size() == 0)
	{
		return pybind11::none();
	}
	py::array_t<jvxInt32> arr(lstStr.size());
	auto buf = arr.mutable_unchecked<1>();
	for (size_t i = 0; i < lstStr.size(); i++)
	{
		buf(i) = lstStr[i];
	}
	return arr;
}

/**
 * Return a single scalar in python format.
 *///=======================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnInt32(const jvxInt32& value)
{
	return py::int_(value);
}

pybind11::object
CjvxCToPythonConverter::pyReturnIntPtr(const void* valueptr)
{
	return py::int_((uintptr_t)valueptr);
}

pybind11::object
CjvxCToPythonConverter::pyReturnUInt32(const jvxUInt32& value)
{
	return py::int_(value);
}

pybind11::object
CjvxCToPythonConverter::pyReturnInt8(const jvxInt8& value)
{
	return py::int_(value);
}

/**
 * Return a list of int8 values as a python object.
 *///=======================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnInt8List(const jvxInt8* valueList, jvxSize numberVals)
{
	if ((valueList == NULL) || (numberVals == 0))
	{
		return pybind11::none();
	}
	py::array_t<jvxInt8> arr(numberVals);
	std::memcpy(arr.mutable_data(), valueList, sizeof(jvxInt8) * numberVals);
	return arr;
}

pybind11::object
CjvxCToPythonConverter::pyReturnInt32List(const jvxInt32* valueList, jvxSize numberVals)
{
	if ((valueList == NULL) || (numberVals == 0))
	{
		return pybind11::none();
	}
	py::array_t<jvxInt32> arr(numberVals);
	std::memcpy(arr.mutable_data(), valueList, sizeof(jvxInt32) * numberVals);
	return arr;
}

/**
 * Return a jvxData scalar as a python object.
 *///=======================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnData(const jvxData& value)
{
	return py::float_(value);
}

/**
 * Return a jvxData list as a python object.
 *///=======================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnDataList(const jvxData* valueList, jvxSize numberVals)
{
	if ((valueList == NULL) || (numberVals == 0))
	{
		return pybind11::none();
	}
	py::array_t<jvxData> arr(numberVals);
	std::memcpy(arr.mutable_data(), valueList, sizeof(jvxData) * numberVals);
	return arr;
}

/**
 * Return a list of int16 values as a python object.
 *///===========================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnInt16(const jvxInt16& value)
{
	return py::int_(value);
}

pybind11::object
CjvxCToPythonConverter::pyReturnUInt16(const jvxUInt16& value)
{
	return py::int_(value);
}

pybind11::object
CjvxCToPythonConverter::pyReturnInt16List(const jvxInt16* valueList, jvxSize numberVals)
{
	if ((valueList == NULL) || (numberVals == 0))
	{
		return pybind11::none();
	}
	py::array_t<jvxInt16> arr(numberVals);
	std::memcpy(arr.mutable_data(), valueList, sizeof(jvxInt16) * numberVals);
	return arr;
}

/**
 * Function to return a single string as a python object.
 *///==============================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnString(const std::string& str)
{
	return py::str(str);
}

/**
 * Function to return a list of strings as a python object.
 *///=======================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnStringList(const std::vector<std::string>& lstStr)
{
	py::list lst;
	for (size_t i = 0; i < lstStr.size(); i++)
	{
		lst.append(py::str(lstStr[i]));
	}
	return lst;
}

/**
 * Return bool as a python object.
 *///========================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnBool(jvxBool trueFalse)
{
	return py::bool_(trueFalse != 0);
}

pybind11::object
CjvxCToPythonConverter::pyReturnErrorCode(jvxErrorType tp)
{
	return CjvxCToPythonConverter::pyReturnInt32((jvxInt32)tp);
}

/**
 * Function to return the indication for an error, in python format.
 *///==============================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnAnswerNegativeResult(const std::string& expr, jvxErrorType errCode)
{
	py::dict d;
	d[FLD_NAME_ERROR_DESCRIPTION_STRING] = CjvxCToPythonConverter::pyReturnString(expr);
	d[FLD_NAME_ERROR_ID_INT32] = CjvxCToPythonConverter::pyReturnInt32((jvxInt32)errCode);
	return d;
}

/**
 * Return a long (int64) as a python object.
 *///=======================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnInt64(const jvxInt64& value)
{
	return py::int_(value);
}

pybind11::object
CjvxCToPythonConverter::pyReturnUInt64(const jvxUInt64& value)
{
	return py::int_(value);
}

pybind11::object
CjvxCToPythonConverter::pyReturnInt64List(const jvxInt64* valueList, jvxSize numberVals)
{
	if ((valueList == NULL) || (numberVals == 0))
	{
		return pybind11::none();
	}
	py::array_t<jvxInt64> arr(numberVals);
	std::memcpy(arr.mutable_data(), valueList, sizeof(jvxInt64) * numberVals);
	return arr;
}

/**
 * Return a bitfield as a python object (translates to a string, same as the Matlab variant).
 *///=======================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnBitField(const jvxBitField& value)
{
	jvxBitField cpFld = value;
	return CjvxCToPythonConverter::pyReturnString(jvx_bitField2String(cpFld));
}

pybind11::object
CjvxCToPythonConverter::pyReturnCBitField(const jvxCBitField& value)
{
	jvxBitField cpFld;
	cpFld.setValueC(value);
	return CjvxCToPythonConverter::pyReturnString(jvx_bitField2String(cpFld));
}

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
/**
 * Return a jvxData (double) list as a python object, converted from a float buffer.
 *///=======================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnFloatList(const float* ptrField, jvxSize numElements)
{
	if ((ptrField == NULL) || (numElements == 0))
	{
		return pybind11::none();
	}
	py::array_t<jvxData> arr(numElements);
	auto buf = arr.mutable_unchecked<1>();
	for (jvxSize i = 0; i < numElements; i++)
	{
		buf(i) = (jvxData)ptrField[i];
	}
	return arr;
}
#else
/**
 * Return a jvxData (float) list as a python object, converted from a double buffer.
 *///=======================================================================
pybind11::object
CjvxCToPythonConverter::pyReturnDoubleList(const double* ptrField, jvxSize numElements)
{
	if ((ptrField == NULL) || (numElements == 0))
	{
		return pybind11::none();
	}
	py::array_t<jvxData> arr(numElements);
	auto buf = arr.mutable_unchecked<1>();
	for (jvxSize i = 0; i < numElements; i++)
	{
		buf(i) = (jvxData)ptrField[i];
	}
	return arr;
}
#endif

pybind11::object
CjvxCToPythonConverter::pyReturnSelectionListStruct(const jvxSelectionList& theSelList)
{
	jvxSize i;
	std::vector<std::string> lstTxt;

	for (i = 0; i < theSelList.strList.ll(); i++)
	{
		lstTxt.push_back(theSelList.strList.std_str_at(i));
	}

	py::dict d;
	d[FLD_NAME_PROPERTY_SELLIST_OPTIONS] = CjvxCToPythonConverter::pyReturnStringList(lstTxt);
	d[FLD_NAME_PROPERTY_SELLIST_SELECTION_BITFIELD] = CjvxCToPythonConverter::pyReturnBitField(theSelList.bitFieldSelected());
	d[FLD_NAME_PROPERTY_SELLIST_EXCULSIVE_BITFIELD] = CjvxCToPythonConverter::pyReturnBitField(theSelList.bitFieldExclusive);
	return d;
}

pybind11::object
CjvxCToPythonConverter::pyReturnValueInRangeStruct(const jvxValueInRange& valRange)
{
	py::dict d;
	d[FLD_NAME_PROPERTY_VALUE_IN_RANGE_MIN_DBL] = CjvxCToPythonConverter::pyReturnData(valRange.minVal);
	d[FLD_NAME_PROPERTY_VALUE_IN_RANGE_MAX_DBL] = CjvxCToPythonConverter::pyReturnData(valRange.maxVal);
	d[FLD_NAME_PROPERTY_VALUE_IN_RANGE_VALUE_DBL] = CjvxCToPythonConverter::pyReturnData(valRange.val());
	return d;
}

pybind11::object
CjvxCToPythonConverter::pyReturnConfigSection(jvxConfigData* theSection, IjvxConfigProcessor* proc)
{
	signed i, j, k;
	jvxSize numSections = 0;

	std::string txt;
	jvxData dataDbl;
	jvxValue val;

	jvxSize numStrings = 0, numLists = 0;
	std::vector<std::string> vecStrings;

	jvxApiValueList lstVals;
	jvxConfigData* theSubsection = NULL;

	std::vector<jvxData*> lstDblsCopy;
	std::vector<jvxSize> lengths;

	jvxBitField bfield;
	jvxSize numEntriesList;
	jvxSize sz;
	jvxBool returnCell;

	proc->getNumberEntriesCurrentSection(theSection, &numSections);
	if (!numSections)
	{
		return pybind11::none();
	}

	py::dict topDict;

	for (i = 0; i < numSections; i++)
	{
		jvxApiString fldStrRTP;
		py::dict entryDict;

		proc->getReferenceEntryCurrentSection_id(theSection, &theSubsection, i);

		proc->getNameCurrentEntry(theSubsection, &fldStrRTP);
		txt = fldStrRTP.std_str();
		entryDict["DESCRIPTOR"] = CjvxCToPythonConverter::pyReturnString(txt);

		jvxConfigSectionTypes tpSection = JVX_CONFIG_SECTION_TYPE_SECTION;
		proc->getTypeCurrentEntry(theSubsection, &tpSection);
		entryDict["TYPE"] = CjvxCToPythonConverter::pyReturnInt32(tpSection);

		pybind11::object content = pybind11::none();

		switch (tpSection)
		{
		case JVX_CONFIG_SECTION_TYPE_SECTION:
			content = CjvxCToPythonConverter::pyReturnConfigSection(theSubsection, proc);
			break;
		case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTHEXSTRING:
			proc->getAssignmentBitField(theSubsection, &bfield);
			content = CjvxCToPythonConverter::pyReturnBitField(bfield);
			break;
		case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
			proc->getAssignmentString(theSubsection, &fldStrRTP);
			txt = fldStrRTP.std_str();
			content = CjvxCToPythonConverter::pyReturnString(txt);
			break;
		case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
			dataDbl = 0.0;
			proc->getAssignmentValue(theSubsection, &val);
			val.toContent(&dataDbl);
			content = CjvxCToPythonConverter::pyReturnData(dataDbl);
			break;
		case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
			numStrings = 0;
			vecStrings.clear();
			proc->getNumberStrings(theSubsection, &numStrings);
			for (j = 0; j < numStrings; j++)
			{
				txt = "error";
				proc->getString_id(theSubsection, &fldStrRTP, j);
				txt = fldStrRTP.std_str();
				vecStrings.push_back(txt);
			}
			content = CjvxCToPythonConverter::pyReturnStringList(vecStrings);
			break;
		case JVX_CONFIG_SECTION_TYPE_VALUELIST:
			numLists = 0;
			proc->getNumberValueLists(theSubsection, &numLists);
			returnCell = true;
			if (numLists > 0)
			{
				returnCell = false;
				proc->getValueListSize_id(theSubsection, &numEntriesList, 0);

				for (j = 0; j < numLists; j++)
				{
					proc->getValueListSize_id(theSubsection, &sz, 0);
					if (sz != numEntriesList)
					{
						returnCell = true;
						break;
					}
				}
			}
			if (returnCell)
			{
				for (j = 0; j < numLists; j++)
				{
					proc->getValueList_id(theSubsection, &lstVals, j);
					if (lstVals.ll())
					{
						jvxData* copyLst = new jvxData[lstVals.ll()];
						for (k = 0; k < lstVals.ll(); k++)
						{
							lstVals.elm_at(k).toContent(&copyLst[k]);
						}
						lstDblsCopy.push_back(copyLst);
						lengths.push_back(lstVals.ll());
					}
					else
					{
						lstDblsCopy.push_back(NULL);
						lengths.push_back(0);
					}
				}

				content = CjvxCToPythonConverter::pyReturnDataFieldCell(lstDblsCopy, lengths);

				for (j = 0; j < (signed)lstDblsCopy.size(); j++)
				{
					delete[](lstDblsCopy[j]);
				}
				lstDblsCopy.clear();
				lengths.clear();
			}
			else
			{
				jvxData** fldBuf = NULL;
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fldBuf, jvxData*, numLists);
				for (j = 0; j < numLists; j++)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fldBuf[j], jvxData, numEntriesList);
					proc->getValueList_id(theSubsection, &lstVals, j);
					assert(lstVals.ll() == numEntriesList);
					for (k = 0; k < numEntriesList; k++)
					{
						lstVals.elm_at(k).toContent(&fldBuf[j][k]);
					}
				}
				content = CjvxCToPythonConverter::pyReturnGenericNumeric((const jvxHandle**)fldBuf, (jvxInt32)numLists, (jvxInt32)numEntriesList, JVX_DATAFORMAT_DATA);
				for (j = 0; j < numLists; j++)
				{
					JVX_DSP_SAFE_DELETE_FIELD(fldBuf[j]);
				}
				JVX_DSP_SAFE_DELETE_FIELD(fldBuf);
			}
			break;
		}
		entryDict["CONTENT"] = content;

		topDict[("entry" + jvx_int2String(i)).c_str()] = entryDict;
	}
	return topDict;
}

pybind11::object
CjvxCToPythonConverter::pyReturnDataFieldCell(std::vector<jvxData*> lstDbls, std::vector<jvxSize> lengths)
{
	if (lstDbls.size() == 0)
	{
		return pybind11::none();
	}

	py::list lst;
	for (size_t i = 0; i < lstDbls.size(); i++)
	{
		lst.append(CjvxCToPythonConverter::pyReturnDataList(lstDbls[i], JVX_SIZE_INT(lengths[i])));
	}
	return lst;
}

//! Generic numeric matrix conversion (dimY x dimX), analog of mexReturnGenericNumeric.
pybind11::object
CjvxCToPythonConverter::pyReturnGenericNumeric(const jvxHandle** fieldInput, jvxInt32 dimY, jvxInt32 dimX, jvxDataFormat processingFormat, jvxBool isCplx)
{
	int i, ii;

	switch (processingFormat)
	{
	case JVX_DATAFORMAT_DATA:
		if (isCplx)
		{
			py::array_t<std::complex<jvxData>> arr({ (py::ssize_t)dimY, (py::ssize_t)dimX });
			auto buf = arr.mutable_unchecked<2>();
			for (i = 0; i < dimX; i++)
			{
				for (ii = 0; ii < dimY; ii++)
				{
					jvxDataCplx v = ((jvxDataCplx*)fieldInput[ii])[i];
					buf(ii, i) = std::complex<jvxData>(v.re, v.im);
				}
			}
			return arr;
		}
		else
		{
			py::array_t<jvxData> arr({ (py::ssize_t)dimY, (py::ssize_t)dimX });
			auto buf = arr.mutable_unchecked<2>();
			for (i = 0; i < dimX; i++)
			{
				for (ii = 0; ii < dimY; ii++)
				{
					buf(ii, i) = ((jvxData*)fieldInput[ii])[i];
				}
			}
			return arr;
		}
	case JVX_DATAFORMAT_16BIT_LE:
	{
		py::array_t<jvxInt16> arr({ (py::ssize_t)dimY, (py::ssize_t)dimX });
		auto buf = arr.mutable_unchecked<2>();
		for (i = 0; i < dimX; i++)
			for (ii = 0; ii < dimY; ii++)
				buf(ii, i) = ((jvxInt16*)fieldInput[ii])[i];
		return arr;
	}
	case JVX_DATAFORMAT_32BIT_LE:
	{
		py::array_t<jvxInt32> arr({ (py::ssize_t)dimY, (py::ssize_t)dimX });
		auto buf = arr.mutable_unchecked<2>();
		for (i = 0; i < dimX; i++)
			for (ii = 0; ii < dimY; ii++)
				buf(ii, i) = ((jvxInt32*)fieldInput[ii])[i];
		return arr;
	}
	case JVX_DATAFORMAT_64BIT_LE:
	{
		py::array_t<jvxInt64> arr({ (py::ssize_t)dimY, (py::ssize_t)dimX });
		auto buf = arr.mutable_unchecked<2>();
		for (i = 0; i < dimX; i++)
			for (ii = 0; ii < dimY; ii++)
				buf(ii, i) = ((jvxInt64*)fieldInput[ii])[i];
		return arr;
	}
	case JVX_DATAFORMAT_8BIT:
	{
		py::array_t<jvxInt8> arr({ (py::ssize_t)dimY, (py::ssize_t)dimX });
		auto buf = arr.mutable_unchecked<2>();
		for (i = 0; i < dimX; i++)
			for (ii = 0; ii < dimY; ii++)
				buf(ii, i) = ((jvxInt8*)fieldInput[ii])[i];
		return arr;
	}
	case JVX_DATAFORMAT_SIZE:
	{
		py::array_t<jvxData> arr({ (py::ssize_t)dimY, (py::ssize_t)dimX });
		auto buf = arr.mutable_unchecked<2>();
		for (i = 0; i < dimX; i++)
			for (ii = 0; ii < dimY; ii++)
				buf(ii, i) = (jvxData)((jvxSize*)fieldInput[ii])[i];
		return arr;
	}
	default:
		assert(0);
	}
	return pybind11::none();
}

jvxErrorType
CjvxCToPythonConverter::convertPyToC(jvxHandle** fieldOutput, jvxInt32 dimY, jvxInt32 dimX,
	jvxDataFormat processingFormat, const pybind11::object& pyArr, const char* nameVar, bool convertFloat,
	std::string& theErrordescr)
{
	theErrordescr = "";

	if (!py::isinstance<py::array>(pyArr))
	{
		theErrordescr = ((std::string)"Argument " + nameVar + " is not a numpy array.");
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	py::array arr = pyArr.cast<py::array>();
	if (arr.ndim() != 2)
	{
		theErrordescr = ((std::string)"Argument " + nameVar + " must be a 2-dimensional array.");
		return JVX_ERROR_INVALID_ARGUMENT;
	}
	if ((arr.shape(0) != dimY) || (arr.shape(1) != dimX))
	{
		theErrordescr = ((std::string)"Format of matrix " + nameVar +
			" does not fit to expected field, expected (" + jvx_int2String(dimY) + "," + jvx_int2String(dimX) +
			"), found (" + jvx_int2String((int)arr.shape(0)) + "," + jvx_int2String((int)arr.shape(1)) + ")");
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	int i, ii;

	switch (processingFormat)
	{
	case JVX_DATAFORMAT_DATA:
	{
		if (py::isinstance<py::array_t<jvxData>>(arr) && (arr.dtype().is(py::dtype::of<jvxData>())))
		{
			auto buf = arr.cast<py::array_t<jvxData>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxData*)fieldOutput[ii])[i] = buf(ii, i);
		}
		else if (convertFloat)
		{
#ifdef JVX_DSP_DATA_FORMAT_FLOAT
			if (arr.dtype().is(py::dtype::of<double>()))
			{
				auto buf = arr.cast<py::array_t<double>>().unchecked<2>();
				for (i = 0; i < dimX; i++)
					for (ii = 0; ii < dimY; ii++)
						((jvxData*)fieldOutput[ii])[i] = (jvxData)buf(ii, i);
			}
#else
			if (arr.dtype().is(py::dtype::of<float>()))
			{
				auto buf = arr.cast<py::array_t<float>>().unchecked<2>();
				for (i = 0; i < dimX; i++)
					for (ii = 0; ii < dimY; ii++)
						((jvxData*)fieldOutput[ii])[i] = (jvxData)buf(ii, i);
			}
#endif
			else
			{
				theErrordescr = ((std::string)"Type of matrix " +
					nameVar + " does not match the expected float type, expected float type is " + JVX_DATA_DESCRIPTOR_THE_OTHER);
				return JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			theErrordescr = ((std::string)"Type of matrix " +
				nameVar + " does not match the expected type, expected type is " + JVX_DATA_DESCRIPTOR);
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	}
	case JVX_DATAFORMAT_32BIT_LE:
		if (!arr.dtype().is(py::dtype::of<jvxInt32>()))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar + " does not match the expected type, expected type is int32");
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		{
			auto buf = arr.cast<py::array_t<jvxInt32>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxInt32*)fieldOutput[ii])[i] = buf(ii, i);
		}
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		if (!arr.dtype().is(py::dtype::of<jvxInt64>()))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar + " does not match the expected type, expected type is int64");
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		{
			auto buf = arr.cast<py::array_t<jvxInt64>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxInt64*)fieldOutput[ii])[i] = buf(ii, i);
		}
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		if (!arr.dtype().is(py::dtype::of<jvxInt16>()))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar + " does not match the expected type, expected type is int16");
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		{
			auto buf = arr.cast<py::array_t<jvxInt16>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxInt16*)fieldOutput[ii])[i] = buf(ii, i);
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		if (!arr.dtype().is(py::dtype::of<jvxInt8>()))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar + " does not match the expected type, expected type is int8");
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		{
			auto buf = arr.cast<py::array_t<jvxInt8>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxInt8*)fieldOutput[ii])[i] = buf(ii, i);
		}
		break;
	case JVX_DATAFORMAT_U32BIT_LE:
		if (!arr.dtype().is(py::dtype::of<jvxUInt32>()))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar + " does not match the expected type, expected type is uint32");
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		{
			auto buf = arr.cast<py::array_t<jvxUInt32>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxUInt32*)fieldOutput[ii])[i] = buf(ii, i);
		}
		break;
	case JVX_DATAFORMAT_U64BIT_LE:
		if (!arr.dtype().is(py::dtype::of<jvxUInt64>()))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar + " does not match the expected type, expected type is uint64");
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		{
			auto buf = arr.cast<py::array_t<jvxUInt64>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxUInt64*)fieldOutput[ii])[i] = buf(ii, i);
		}
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		if (!arr.dtype().is(py::dtype::of<jvxUInt16>()))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar + " does not match the expected type, expected type is uint16");
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		{
			auto buf = arr.cast<py::array_t<jvxUInt16>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxUInt16*)fieldOutput[ii])[i] = buf(ii, i);
		}
		break;
	case JVX_DATAFORMAT_U8BIT:
		if (!arr.dtype().is(py::dtype::of<jvxUInt8>()))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar + " does not match the expected type, expected type is uint8");
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		{
			auto buf = arr.cast<py::array_t<jvxUInt8>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxUInt8*)fieldOutput[ii])[i] = buf(ii, i);
		}
		break;
	case JVX_DATAFORMAT_SIZE:
	{
		bool handled = true;
		if (arr.dtype().is(py::dtype::of<double>()))
		{
			auto buf = arr.cast<py::array_t<double>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxSize*)fieldOutput[ii])[i] = JVX_DATA2SIZE(buf(ii, i));
		}
		else if (arr.dtype().is(py::dtype::of<float>()))
		{
			auto buf = arr.cast<py::array_t<float>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxSize*)fieldOutput[ii])[i] = JVX_DATA2SIZE(buf(ii, i));
		}
		else if (arr.dtype().is(py::dtype::of<jvxInt8>()))
		{
			auto buf = arr.cast<py::array_t<jvxInt8>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxSize*)fieldOutput[ii])[i] = buf(ii, i);
		}
		else if (arr.dtype().is(py::dtype::of<jvxInt16>()))
		{
			auto buf = arr.cast<py::array_t<jvxInt16>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxSize*)fieldOutput[ii])[i] = buf(ii, i);
		}
		else if (arr.dtype().is(py::dtype::of<jvxInt32>()))
		{
			auto buf = arr.cast<py::array_t<jvxInt32>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxSize*)fieldOutput[ii])[i] = buf(ii, i);
		}
		else if (arr.dtype().is(py::dtype::of<jvxInt64>()))
		{
			auto buf = arr.cast<py::array_t<jvxInt64>>().unchecked<2>();
			for (i = 0; i < dimX; i++)
				for (ii = 0; ii < dimY; ii++)
					((jvxSize*)fieldOutput[ii])[i] = buf(ii, i);
		}
		else
		{
			handled = false;
		}

		if (!handled)
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar +
				" does not match the expected type, it may be of type int8, int16, int32, int64, single or double.");
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	}
	default:
		assert(0);
	}
	return JVX_NO_ERROR;
}
