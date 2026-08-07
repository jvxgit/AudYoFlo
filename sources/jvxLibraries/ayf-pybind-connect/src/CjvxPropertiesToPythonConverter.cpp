// Need to include jvx.h here since the order of inclusion matters!
#include "jvx.h"

#include "localPybindIncludes.h"
#include "CjvxPropertiesToPythonConverter.h"
#include "CjvxCToPythonConverter.h"
#include "CjvxPythonToCConverter.h"

namespace py = pybind11;

#define ERROR_MESSAGE_REPORT(fct) ((std::string)"Internal error when using CjvxPropertiesToPythonConverter, member function " + fct + (std::string)", reason: ")

//! Allocates a 1D numpy array matching the given jvxDataFormat and returns both
//! the owning python object and a raw pointer to its (still empty) buffer, so
//! that IjvxProperties::get_property can write directly into it - analog of
//! mxCreateNumericArray(...) + mxGetData(...) in the Matlab variant.
static pybind11::object
jvx_py_allocate_numeric_array(jvxDataFormat format, jvxSize numElms, jvxHandle** dataOut)
{
	switch (format)
	{
	case JVX_DATAFORMAT_8BIT:
	{
		py::array_t<jvxInt8> arr(numElms);
		*dataOut = (jvxHandle*)arr.mutable_data();
		return arr;
	}
	case JVX_DATAFORMAT_16BIT_LE:
	{
		py::array_t<jvxInt16> arr(numElms);
		*dataOut = (jvxHandle*)arr.mutable_data();
		return arr;
	}
	case JVX_DATAFORMAT_32BIT_LE:
	{
		py::array_t<jvxInt32> arr(numElms);
		*dataOut = (jvxHandle*)arr.mutable_data();
		return arr;
	}
	case JVX_DATAFORMAT_64BIT_LE:
	{
		py::array_t<jvxInt64> arr(numElms);
		*dataOut = (jvxHandle*)arr.mutable_data();
		return arr;
	}
	case JVX_DATAFORMAT_U8BIT:
	{
		py::array_t<jvxUInt8> arr(numElms);
		*dataOut = (jvxHandle*)arr.mutable_data();
		return arr;
	}
	case JVX_DATAFORMAT_U16BIT_LE:
	{
		py::array_t<jvxUInt16> arr(numElms);
		*dataOut = (jvxHandle*)arr.mutable_data();
		return arr;
	}
	case JVX_DATAFORMAT_U32BIT_LE:
	{
		py::array_t<jvxUInt32> arr(numElms);
		*dataOut = (jvxHandle*)arr.mutable_data();
		return arr;
	}
	case JVX_DATAFORMAT_U64BIT_LE:
	{
		py::array_t<jvxUInt64> arr(numElms);
		*dataOut = (jvxHandle*)arr.mutable_data();
		return arr;
	}
	case JVX_DATAFORMAT_DATA:
	{
		py::array_t<jvxData> arr(numElms);
		*dataOut = (jvxHandle*)arr.mutable_data();
		return arr;
	}
	default:
		assert(0);
	}
	*dataOut = nullptr;
	return pybind11::none();
}

pybind11::object
CjvxPropertiesToPythonConverter::pyReturnStructProperties(jvx_propertyReferenceTriple& theTriple)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	jvxCallManagerProperties callGate;
	res = theTriple.theProps->number_properties(callGate, &num);

	if (num == 0)
	{
		return pybind11::none();
	}

	py::dict topDict;
	for (i = 0; i < num; i++)
	{
		jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus theDescr;
		jvx::propertyAddress::CjvxPropertyAddressLinear ident(i);
		res = theTriple.theProps->description_property(callGate, theDescr, ident);

		pybind11::object oneProp = this->pyReturnStructOneProperty(theDescr.category, theDescr.allowedStateMask, theDescr.allowedThreadingMask,
			theDescr.format, theDescr.num, theDescr.accessType, theDescr.decTp, theDescr.globalIdx,
			theDescr.ctxt, theDescr.name.std_str(),
			theDescr.description.c_str(), theDescr.descriptor.std_str(),
			theDescr.isValid, theDescr.accessFlags);

		topDict[(PREFIX_ONE_PROPERTY + jvx_size2String(i)).c_str()] = oneProp;
	}
	return topDict;
}

pybind11::object
CjvxPropertiesToPythonConverter::pyReturnStructOneProperty(jvxPropertyCategoryType catProperty, jvxUInt64 allowStateMask, jvxUInt64 allowThreadingMask,
	jvxDataFormat format, jvxSize numElms, jvxPropertyAccessType accessType, jvxPropertyDecoderHintType decHtTp, jvxSize hdlIdx,
	jvxPropertyContext context, const std::string& name, const std::string& description, const std::string& descriptor,
	jvxBool isValid, jvxFlagTag accessFlags)
{
	py::dict d;
	d[FLD_NAME_PROPERTY_NAME_STRING] = converter->pyReturnString(name);
	d[FLD_NAME_PROPERTY_DESCRIPTION_STRING] = converter->pyReturnString(description);
	d[FLD_NAME_PROPERTY_CATEGORY_INT32] = converter->pyReturnInt32((jvxInt32)catProperty);
	d[FLD_NAME_PROPERTY_UNIQUE_ID_INT32] = converter->pyReturnInt32((jvxInt32)hdlIdx);
	d[FLD_NAME_PROPERTY_FORMAT_INT32] = converter->pyReturnInt32((jvxInt32)format);
	d[FLD_NAME_PROPERTY_NUMBER_ELEMENTS_INT32] = converter->pyReturnInt32((jvxInt32)numElms);
	d[FLD_NAME_PROPERTY_DECODER_HINT_TYPE_INT32] = converter->pyReturnInt32((jvxInt32)decHtTp);
	d[FLD_NAME_PROPERTY_ALLOW_STATE_MASK_INT64] = converter->pyReturnInt64((jvxInt64)allowStateMask);
	d[FLD_NAME_PROPERTY_ALLOW_THREADING_MASK_INT64] = converter->pyReturnInt64((jvxInt64)allowThreadingMask);
	d[FLD_NAME_PROPERTY_ACCESS_TYPE_INT32] = converter->pyReturnInt64((jvxInt32)accessType);
	d[FLD_NAME_PROPERTY_DESCRIPTOR_STRING] = converter->pyReturnString(descriptor);
	d[FLD_NAME_PROPERTY_CONTEXT_INT32] = converter->pyReturnInt32((jvxInt32)context);
	d[FLD_NAME_PROPERTY_IS_VALID_BOOL] = converter->pyReturnBool(isValid);
	d[FLD_NAME_PROPERTY_ACCESS_FLAGS_INT32] = converter->pyReturnInt32((jvxInt32)accessFlags);
	return d;
}

jvxErrorType
CjvxPropertiesToPythonConverter::pyReturnPropertyNumerical(pybind11::object& plhs, jvxSize hdlIdx, jvxPropertyCategoryType catTp, jvxDataFormat format,
	jvxSize numElms, jvxPropertyDecoderHintType decHtTp, jvx_propertyReferenceTriple& theTriple, jvxSize offset)
{
	jvxErrorType res = JVX_NO_ERROR;
	pybind11::object arr = pybind11::none();
	jvxHandle* data = nullptr;
	jvxCallManagerProperties callGate;
	jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE;
	callGate.on_get.prop_access_type = &accessType;

	if (numElms > 0)
	{
		arr = jvx_py_allocate_numeric_array(format, numElms, &data);
		res = theTriple.theProps->get_property(callGate, jPRG(data, numElms, format),
			jPAGID(hdlIdx, catTp),
			jPD(true, offset));
		if (res != JVX_NO_ERROR)
		{
			arr = pybind11::none();
		}
	}

	if (res == JVX_NO_ERROR)
	{
		py::dict d;
		d[FLD_NAME_PROPERTY_SUBFIELD] = arr;
		d[FLD_NAME_PROPERTY_ACCESS_TYPE_INT32] = converter->pyReturnInt32(accessType);
		d[FLD_NAME_PROPERTY_DECODER_HINT_INT32] = converter->pyReturnInt32(decHtTp);
		d[FLD_NAME_PROPERTY_ACCESS_PROTOCOL_INT32] = converter->pyReturnInt32(callGate.access_protocol);
		plhs = d;
	}
	else
	{
		plhs = pybind11::none();
	}
	return res;
}

jvxErrorType
CjvxPropertiesToPythonConverter::pyReturnPropertyNumericalSize(pybind11::object& plhs, jvxSize hdlIdx, jvxPropertyCategoryType catTp, jvxDataFormat format, jvxSize numElms,
	jvxPropertyDecoderHintType decHtTp, jvx_propertyReferenceTriple& theTriple, jvxSize offset)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	pybind11::object arr = pybind11::none();
	jvxSize* data = nullptr;
	jvxCallManagerProperties callGate;
	jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE;
	callGate.on_get.prop_access_type = &accessType;

	if (numElms > 0)
	{
		assert(format == JVX_DATAFORMAT_SIZE);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data, jvxSize, numElms);
		res = theTriple.theProps->get_property(
			callGate, jPRG(data, numElms, format),
			jPAGID(hdlIdx, catTp),
			jPD(true, offset));

		if (res == JVX_NO_ERROR)
		{
			py::array_t<jvxInt32> outArr(numElms);
			auto buf = outArr.mutable_unchecked<1>();
			for (i = 0; i < numElms; i++)
			{
				buf(i) = JVX_SIZE_INT32(data[i]);
			}
			arr = outArr;
		}
		JVX_DSP_SAFE_DELETE_FIELD(data);
	}

	if (res == JVX_NO_ERROR)
	{
		py::dict d;
		d[FLD_NAME_PROPERTY_SUBFIELD] = arr;
		d[FLD_NAME_PROPERTY_ACCESS_TYPE_INT32] = converter->pyReturnInt32(accessType);
		d[FLD_NAME_PROPERTY_DECODER_HINT_INT32] = converter->pyReturnInt32(decHtTp);
		d[FLD_NAME_PROPERTY_ACCESS_PROTOCOL_INT32] = converter->pyReturnInt32(callGate.access_protocol);
		plhs = d;
	}
	else
	{
		plhs = pybind11::none();
	}
	return res;
}

jvxErrorType
CjvxPropertiesToPythonConverter::pyReturnPropertyNumerical(pybind11::object& plhs, jvxDataFormat format, jvxSize numElms, const char* descr, jvx_propertyReferenceTriple& theTriple, jvxSize offset)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxHandle* data = nullptr;

	if (numElms > 0)
	{
		plhs = jvx_py_allocate_numeric_array(format, numElms, &data);
		res = jvx_get_property(theTriple.theProps, data, offset, numElms, format, true, descr, callGate);
		if (res != JVX_NO_ERROR)
		{
			plhs = pybind11::none();
		}
	}
	else
	{
		plhs = pybind11::none();
	}
	return res;
}

jvxErrorType
CjvxPropertiesToPythonConverter::pyReturnPropertyNumericalSize(pybind11::object& plhs, jvxDataFormat format, jvxSize numElms, const char* descr, jvx_propertyReferenceTriple& theTriple, jvxSize offset)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxSize* data = nullptr;
	if (numElms > 0)
	{
		assert(format == JVX_DATAFORMAT_SIZE);

		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data, jvxSize, numElms);

		res = jvx_get_property(theTriple.theProps, data, offset, numElms, format, true, descr, callGate);

		if (res == JVX_NO_ERROR)
		{
			py::array_t<jvxInt32> outArr(numElms);
			auto buf = outArr.mutable_unchecked<1>();
			for (i = 0; i < numElms; i++)
			{
				buf(i) = JVX_SIZE_INT32(data[i]);
			}
			plhs = outArr;
		}
		JVX_DSP_SAFE_DELETE_FIELD(data);
	}
	else
	{
		plhs = pybind11::none();
	}
	return res;
}

jvxErrorType
CjvxPropertiesToPythonConverter::pyReturnPropertyOthers(pybind11::object& plhs, jvxSize hdlIdx, jvxPropertyCategoryType catTp, jvxDataFormat format, jvxSize numElms,
	jvxPropertyDecoderHintType decHtTp, jvx_propertyReferenceTriple& theTriple, jvxSize offset)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	pybind11::object arr = pybind11::none();
	jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE;

	jvxSelectionList theSelList;
	jvxApiString fldStr;
	jvxApiStringList fldStrLst;
	jvxValueInRange vInRange;

	std::string txt;
	std::vector<std::string> lstTxt;
	jvxCallManagerProperties callGate;
	callGate.on_get.prop_access_type = &accessType;

	if (numElms > 0)
	{
		switch (format)
		{
		case JVX_DATAFORMAT_SELECTION_LIST:
			theSelList.bitFieldExclusive = 0;
			theSelList.bitFieldSelected() = 0;

			res = theTriple.theProps->get_property(callGate,
				jPRG(&theSelList, 1, format),
				jPAGID(hdlIdx, catTp),
				jPD(false, offset));
			if (res == JVX_NO_ERROR)
			{
				arr = converter->pyReturnSelectionListStruct(theSelList);
			}
			break;
		case JVX_DATAFORMAT_STRING:
			res = theTriple.theProps->get_property(callGate,
				jPRG(&fldStr, 1, format),
				jPAGID(hdlIdx, catTp),
				jPD(false, offset));
			if (res == JVX_NO_ERROR)
			{
				txt = fldStr.std_str();
				arr = converter->pyReturnString(txt);
			}
			break;
		case JVX_DATAFORMAT_STRING_LIST:
			res = theTriple.theProps->get_property(callGate,
				jPRG(&fldStrLst, 1, format),
				jPAGID(hdlIdx, catTp),
				jPD(false, offset));
			if (res == JVX_NO_ERROR)
			{
				for (i = 0; i < fldStrLst.ll(); i++)
				{
					lstTxt.push_back(fldStrLst.std_str_at(i));
				}
				arr = converter->pyReturnStringList(lstTxt);
			}
			break;
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
			res = theTriple.theProps->get_property(callGate,
				jPRG(&vInRange, 1, format),
				jPAGID(hdlIdx, catTp),
				jPD(false, offset));
			if (res == JVX_NO_ERROR)
			{
				arr = converter->pyReturnValueInRangeStruct(vInRange);
			}
			break;
		default:
			assert(0);
		}
	}

	if (res == JVX_NO_ERROR)
	{
		py::dict d;
		d[FLD_NAME_PROPERTY_SUBFIELD] = arr;
		d[FLD_NAME_PROPERTY_ACCESS_TYPE_INT32] = converter->pyReturnInt32(accessType);
		d[FLD_NAME_PROPERTY_DECODER_HINT_INT32] = converter->pyReturnInt32(decHtTp);
		d[FLD_NAME_PROPERTY_ACCESS_PROTOCOL_INT32] = converter->pyReturnInt32(callGate.access_protocol);
		plhs = d;
	}
	else
	{
		plhs = pybind11::none();
	}
	return res;
}

jvxErrorType
CjvxPropertiesToPythonConverter::pyReturnPropertyOthers(pybind11::object& plhs, jvxDataFormat format, jvxSize numElms, const char* descr, jvx_propertyReferenceTriple& theTriple, jvxSize offset)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList theSelList;
	jvxApiString fldStr;
	jvxApiStringList fldStrLst;
	jvxValueInRange vInRange;

	std::string txt;
	std::vector<std::string> lstTxt;
	jvxCallManagerProperties callGate;

	plhs = pybind11::none();

	if (numElms > 0)
	{
		switch (format)
		{
		case JVX_DATAFORMAT_SELECTION_LIST:
			theSelList.bitFieldExclusive = 0;
			theSelList.bitFieldSelected() = 0;

			res = jvx_get_property(theTriple.theProps, &theSelList, offset, 1, format, false, descr, callGate);
			if (res == JVX_NO_ERROR)
			{
				plhs = converter->pyReturnSelectionListStruct(theSelList);
			}
			break;
		case JVX_DATAFORMAT_STRING:
			res = jvx_get_property(theTriple.theProps, &fldStr, offset, 1, format, false, descr, callGate);
			if (res == JVX_NO_ERROR)
			{
				txt = fldStr.std_str();
				plhs = converter->pyReturnString(txt);
			}
			break;
		case JVX_DATAFORMAT_STRING_LIST:
			res = jvx_get_property(theTriple.theProps, &fldStrLst, offset, 1, format, false, descr, callGate);
			if (res == JVX_NO_ERROR)
			{
				for (i = 0; i < fldStrLst.ll(); i++)
				{
					lstTxt.push_back(fldStrLst.std_str_at(i));
				}
				plhs = converter->pyReturnStringList(lstTxt);
			}
			break;
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
			res = jvx_get_property(theTriple.theProps, &vInRange, offset, 1, format, false, descr, callGate);
			if (res == JVX_NO_ERROR)
			{
				plhs = converter->pyReturnValueInRangeStruct(vInRange);
			}
			break;
		default:
			assert(0);
		}
	}
	return res;
}

// ===================================================================================

jvxErrorType
CjvxPropertiesToPythonConverter::copyDataToComponentNumerical(const pybind11::object& prhs, jvx_propertyReferenceTriple& theTriple, jvxPropertyCategoryType cat, jvxDataFormat format,
	jvxSize numElms, jvxSize uniqueId, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = nullptr;
	jvxCallManagerProperties callGate;
	numTypeConvert inputConvert = { 0 };

	if (py::isinstance<py::array>(prhs))
	{
		py::array arr = prhs.cast<py::array>();
		if ((jvxSize)arr.size() == numElms)
		{
			bool dtypeOk = false;
			switch (format)
			{
			case JVX_DATAFORMAT_DATA: dtypeOk = arr.dtype().is(py::dtype::of<jvxData>()); break;
			case JVX_DATAFORMAT_64BIT_LE: dtypeOk = arr.dtype().is(py::dtype::of<jvxInt64>()); break;
			case JVX_DATAFORMAT_32BIT_LE: dtypeOk = arr.dtype().is(py::dtype::of<jvxInt32>()); break;
			case JVX_DATAFORMAT_16BIT_LE: dtypeOk = arr.dtype().is(py::dtype::of<jvxInt16>()); break;
			case JVX_DATAFORMAT_8BIT: dtypeOk = arr.dtype().is(py::dtype::of<jvxInt8>()); break;
			case JVX_DATAFORMAT_U64BIT_LE: dtypeOk = arr.dtype().is(py::dtype::of<jvxUInt64>()); break;
			case JVX_DATAFORMAT_U32BIT_LE: dtypeOk = arr.dtype().is(py::dtype::of<jvxUInt32>()); break;
			case JVX_DATAFORMAT_U16BIT_LE: dtypeOk = arr.dtype().is(py::dtype::of<jvxUInt16>()); break;
			case JVX_DATAFORMAT_U8BIT: dtypeOk = arr.dtype().is(py::dtype::of<jvxUInt8>()); break;
			default: break;
			}
			if (dtypeOk)
			{
				data = (jvxHandle*)arr.mutable_data();
			}
		}
	}

	if (!data)
	{
		if (numElms == 1)
		{
			if (convertSingleNumericalUnion(format, inputConvert, prhs) == JVX_NO_ERROR)
			{
				data = &inputConvert;
			}
		}
	}

	if (data)
	{
		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(uniqueId, cat);
		jvx::propertyDetail::CjvxTranferDetail trans(true);

		res = theTriple.theProps->set_property(callGate,
			jPRG(data, numElms, format), ident, trans);
		if (accProt)
			*accProt = callGate.access_protocol;
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType
CjvxPropertiesToPythonConverter::copyDataToComponentNumericalSize(const pybind11::object& prhs, jvx_propertyReferenceTriple& theTriple, jvxPropertyCategoryType cat,
	jvxDataFormat format, jvxSize numElms, jvxSize uniqueId, jvxSize offset, jvxAccessProtocol* accProt)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize* dataSz = nullptr;
	jvxCallManagerProperties callGate;
	assert(format == JVX_DATAFORMAT_SIZE);

	if (py::isinstance<py::array>(prhs))
	{
		py::array arr = prhs.cast<py::array>();
		if (((jvxSize)arr.size() == numElms) && arr.dtype().is(py::dtype::of<jvxInt32>()))
		{
			auto buf = arr.cast<py::array_t<jvxInt32>>().unchecked<1>();
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(dataSz, jvxSize, numElms);
			for (i = 0; i < numElms; i++)
			{
				dataSz[i] = JVX_INT_SIZE(buf(i));
			}
		}
	}

	if (dataSz)
	{
		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(uniqueId, cat);
		jvx::propertyDetail::CjvxTranferDetail trans(true);

		res = theTriple.theProps->set_property(callGate,
			jPRG(dataSz, numElms, format), ident, trans);
		if (accProt)
			*accProt = callGate.access_protocol;
		JVX_DSP_SAFE_DELETE_FIELD(dataSz);
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType
CjvxPropertiesToPythonConverter::copyDataToComponentNumerical(const pybind11::object& prhs, jvx_propertyReferenceTriple& theTriple, jvxDataFormat format, jvxSize numElms,
	const char* descr, jvxSize offset, jvxAccessProtocol* accProt)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxData* data_dat = nullptr;
	jvxSize* data_sz = nullptr;
	jvxInt64* data_int64 = nullptr;
	jvxInt32* data_int32 = nullptr;
	jvxInt16* data_int16 = nullptr;
	jvxInt8* data_int8 = nullptr;
	jvxUInt64* data_uint64 = nullptr;
	jvxUInt32* data_uint32 = nullptr;
	jvxUInt16* data_uint16 = nullptr;
	jvxUInt8* data_uint8 = nullptr;
	jvxBool formatFits = false;
	jvxHandle* data_setprop = nullptr;

	if (py::isinstance<py::array>(prhs))
	{
		py::array arr = prhs.cast<py::array>();
		if ((jvxSize)arr.size() == numElms)
		{
			formatFits = true;
		}
	}

	if (formatFits)
	{
		switch (format)
		{
		case JVX_DATAFORMAT_DATA:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_dat, jvxData, numElms);
			data_setprop = data_dat;
			break;
		case JVX_DATAFORMAT_SIZE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_sz, jvxSize, numElms);
			data_setprop = data_sz;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_int64, jvxInt64, numElms);
			data_setprop = data_int64;
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_int32, jvxInt32, numElms);
			data_setprop = data_int32;
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_int16, jvxInt16, numElms);
			data_setprop = data_int16;
			break;
		case JVX_DATAFORMAT_8BIT:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_int8, jvxInt8, numElms);
			data_setprop = data_int8;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_uint64, jvxUInt64, numElms);
			data_setprop = data_uint64;
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_uint32, jvxUInt32, numElms);
			data_setprop = data_uint32;
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_uint16, jvxUInt16, numElms);
			data_setprop = data_uint16;
			break;
		case JVX_DATAFORMAT_U8BIT:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_uint8, jvxUInt8, numElms);
			data_setprop = data_uint8;
			break;
		}

		CjvxPythonToCConverter::convert_py_buf_c_buf_1_x_N(data_setprop, format, numElms, prhs);

		if (data_setprop)
		{
			res = jvx_set_property(theTriple.theProps, data_setprop, offset, numElms, format, true, descr, callGate);
			if (accProt)
				*accProt = callGate.access_protocol;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}

		switch (format)
		{
		case JVX_DATAFORMAT_DATA: JVX_DSP_SAFE_DELETE_FIELD(data_dat); break;
		case JVX_DATAFORMAT_SIZE: JVX_DSP_SAFE_DELETE_FIELD(data_sz); break;
		case JVX_DATAFORMAT_64BIT_LE: JVX_DSP_SAFE_DELETE_FIELD(data_int64); break;
		case JVX_DATAFORMAT_32BIT_LE: JVX_DSP_SAFE_DELETE_FIELD(data_int32); break;
		case JVX_DATAFORMAT_16BIT_LE: JVX_DSP_SAFE_DELETE_FIELD(data_int16); break;
		case JVX_DATAFORMAT_8BIT: JVX_DSP_SAFE_DELETE_FIELD(data_int8); break;
		case JVX_DATAFORMAT_U64BIT_LE: JVX_DSP_SAFE_DELETE_FIELD(data_uint64); break;
		case JVX_DATAFORMAT_U32BIT_LE: JVX_DSP_SAFE_DELETE_FIELD(data_uint32); break;
		case JVX_DATAFORMAT_U16BIT_LE: JVX_DSP_SAFE_DELETE_FIELD(data_uint16); break;
		case JVX_DATAFORMAT_U8BIT: JVX_DSP_SAFE_DELETE_FIELD(data_uint8); break;
		}
	}
	return(res);
}

jvxErrorType
CjvxPropertiesToPythonConverter::copyDataToComponentNumericalSize(const pybind11::object& prhs, jvx_propertyReferenceTriple& theTriple, jvxDataFormat format,
	jvxSize numElms, const char* descr, jvxSize offset, jvxAccessProtocol* accProt)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize* dataSz = nullptr;
	jvxCallManagerProperties callGate;
	assert(format == JVX_DATAFORMAT_SIZE);

	if (py::isinstance<py::array>(prhs))
	{
		py::array arr = prhs.cast<py::array>();
		if (((jvxSize)arr.size() == numElms) && arr.dtype().is(py::dtype::of<jvxInt32>()))
		{
			auto buf = arr.cast<py::array_t<jvxInt32>>().unchecked<1>();
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(dataSz, jvxSize, numElms);
			for (i = 0; i < numElms; i++)
			{
				dataSz[i] = JVX_INT_SIZE(buf(i));
			}
		}
	}

	if (dataSz)
	{
		res = jvx_set_property(theTriple.theProps, dataSz, offset, numElms, format, true, descr, callGate);
		if (accProt)
		{
			*accProt = callGate.access_protocol;
		}
		JVX_DSP_SAFE_DELETE_FIELD(dataSz);
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType
CjvxPropertiesToPythonConverter::copyDataToComponentOthers(const std::vector<pybind11::object>& prhs, int nrhs, jvx_propertyReferenceTriple& theTriple,
	jvxPropertyCategoryType cat, jvxDataFormat format, jvxSize numElms, jvxSize uniqueId, jvxSize offset, jvxAccessProtocol* accProt)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxApiString fldStr;
	std::string token;
	std::vector<std::string> tokenLst;
	jvxApiStringList fldStrLst;
	jvxSelectionList selList;
	jvxData valD;
	jvxValueInRange valR;

	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(uniqueId, cat);
	jvx::propertyDetail::CjvxTranferDetail trans(true);

	switch (format)
	{
	case JVX_DATAFORMAT_STRING:
		if (!prhs.empty() && py::isinstance<py::str>(prhs[0]))
		{
			token = CjvxPythonToCConverter::jvx_py_2_cstring(prhs[0]);
			fldStr.assign_const(token.c_str(), token.size());
			res = theTriple.theProps->set_property(callGate,
				jPRG(&fldStr, 1, format), ident, trans);
			if (accProt)
			{
				*accProt = callGate.access_protocol;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_DATAFORMAT_STRING_LIST:
		res = CjvxPythonToCConverter::pyArgument2String(tokenLst, numElms, prhs, 0, 1);
		if (res == JVX_NO_ERROR)
		{
			fldStrLst.assign(tokenLst);
			res = theTriple.theProps->set_property(callGate,
				jPRG(&fldStrLst, 1, format), ident, trans);
			if (accProt)
			{
				*accProt = callGate.access_protocol;
			}
		}
		break;
	case JVX_DATAFORMAT_SELECTION_LIST:
		res = CjvxPythonToCConverter::pyArgument2String(token, prhs, 0, nrhs);
		if (res == JVX_NO_ERROR)
		{
			jvxBool err = false;
			selList.bitFieldSelected() = jvx_string2BitField(token, err);
			if (err)
			{
				res = JVX_ERROR_PARSE_ERROR;
			}
			else
			{
				res = CjvxPythonToCConverter::pyArgument2String(token, prhs, 1, nrhs);
			}
			if (res == JVX_NO_ERROR)
			{
				err = false;
				selList.bitFieldExclusive = jvx_string2BitField(token, err);
				if (err)
				{
					res = JVX_ERROR_PARSE_ERROR;
				}
				else
				{
					res = CjvxPythonToCConverter::pyArgument2StringList(tokenLst, prhs, 2, nrhs);
				}
				if (res == JVX_NO_ERROR)
				{
					selList.strList.assign(tokenLst);
					trans.contentOnly = false;
					res = theTriple.theProps->set_property(callGate,
						jPRG(&selList, 1, format), ident, trans);
					if (accProt)
					{
						*accProt = callGate.access_protocol;
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else
			{
				res = theTriple.theProps->set_property(callGate,
					jPRG(&selList, 1, format), ident, trans);
				if (accProt)
				{
					*accProt = callGate.access_protocol;
				}
			}
		}
		break;
	case JVX_DATAFORMAT_VALUE_IN_RANGE:
		res = CjvxPythonToCConverter::pyArgument2Index<jvxData>(valD, prhs, 0, nrhs);
		if (res == JVX_NO_ERROR)
		{
			valR.val() = valD;
			res = CjvxPythonToCConverter::pyArgument2Index<jvxData>(valD, prhs, 1, nrhs);
			if (res == JVX_NO_ERROR)
			{
				valR.minVal = valD;
				res = CjvxPythonToCConverter::pyArgument2Index<jvxData>(valD, prhs, 2, nrhs);
				if (res == JVX_NO_ERROR)
				{
					valR.maxVal = valD;
					trans.contentOnly = false;
					res = theTriple.theProps->set_property(callGate,
						jPRG(&valR, 1, format),
						ident, trans);
					if (accProt)
					{
						*accProt = callGate.access_protocol;
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else
			{
				res = theTriple.theProps->set_property(callGate,
					jPRG(&valR, 1, format), ident, trans);
				if (accProt)
				{
					*accProt = callGate.access_protocol;
				}
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	}
	return(res);
}

jvxErrorType
CjvxPropertiesToPythonConverter::copyDataToComponentOthers(const std::vector<pybind11::object>& prhs, int nrhs, jvx_propertyReferenceTriple& theTriple, jvxDataFormat format, jvxSize numElms,
	const char* descr, jvxSize offset, jvxAccessProtocol* accProt)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxBitField btfld;
	jvxApiString fldStr;
	std::string token;
	std::vector<std::string> tokenLst;
	jvxApiStringList fldStrLst;
	jvxSelectionList selList;
	jvxInt64 valI64 = 0;
	jvxValueInRange valR;

	switch (format)
	{
	case JVX_DATAFORMAT_STRING:
		if (!prhs.empty() && py::isinstance<py::str>(prhs[0]))
		{
			token = CjvxPythonToCConverter::jvx_py_2_cstring(prhs[0]);
			fldStr.assign_const(token.c_str(), token.size());
			res = jvx_set_property(theTriple.theProps, &fldStr, offset, 1, format, true, descr, callGate);
			if (accProt)
			{
				*accProt = callGate.access_protocol;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_DATAFORMAT_STRING_LIST:
		res = CjvxPythonToCConverter::pyArgument2String(tokenLst, numElms, prhs, 0, 1);
		if (res == JVX_NO_ERROR)
		{
			fldStrLst.assign(tokenLst);
			res = jvx_set_property(theTriple.theProps, &fldStrLst, offset, 1, format, true, descr, callGate);
			if (accProt)
			{
				*accProt = callGate.access_protocol;
			}
		}
		break;
	case JVX_DATAFORMAT_SELECTION_LIST:
		res = CjvxPythonToCConverter::pyArgument2String(token, prhs, 0, nrhs);
		if (res == JVX_NO_ERROR)
		{
			jvxBool err = false;
			btfld = jvx_string2BitField(token, err);
			if (err)
			{
				res = JVX_ERROR_PARSE_ERROR;
			}
		}
		else
		{
			res = CjvxPythonToCConverter::pyArgument2Index<jvxInt64>(valI64, prhs, 0, nrhs);
			btfld = (jvxBitField)valI64;
		}
		if (res == JVX_NO_ERROR)
		{
			selList.bitFieldSelected() = btfld;

			res = CjvxPythonToCConverter::pyArgument2String(token, prhs, 1, nrhs);
			if (res == JVX_NO_ERROR)
			{
				jvxBool err = false;
				btfld = jvx_string2BitField(token, err);
				if (err)
				{
					res = JVX_ERROR_PARSE_ERROR;
				}
			}
			else
			{
				res = CjvxPythonToCConverter::pyArgument2Index<jvxInt64>(valI64, prhs, 1, nrhs);
				if (res == JVX_NO_ERROR)
				{
					btfld = (jvxBitField)valI64;
				}
			}

			if (res == JVX_NO_ERROR)
			{
				selList.bitFieldExclusive = btfld;
				res = CjvxPythonToCConverter::pyArgument2StringList(tokenLst, prhs, 2, nrhs);
				if (res == JVX_NO_ERROR)
				{
					selList.strList.assign(tokenLst);
					res = jvx_set_property(theTriple.theProps, &selList, offset, 1, format, false, descr, callGate);
					if (accProt)
					{
						*accProt = callGate.access_protocol;
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else
			{
				res = jvx_set_property(theTriple.theProps, &selList, offset, 1, format, true, descr, callGate);
				if (accProt)
				{
					*accProt = callGate.access_protocol;
				}
			}
		}
		break;
	case JVX_DATAFORMAT_VALUE_IN_RANGE:
	{
		jvxData valD;
		res = CjvxPythonToCConverter::pyArgument2Index<jvxData>(valD, prhs, 0, nrhs);
		if (res == JVX_NO_ERROR)
		{
			valR.val() = valD;
			res = CjvxPythonToCConverter::pyArgument2Index<jvxData>(valD, prhs, 1, nrhs);
			if (res == JVX_NO_ERROR)
			{
				valR.minVal = valD;
				res = CjvxPythonToCConverter::pyArgument2Index<jvxData>(valD, prhs, 2, nrhs);
				if (res == JVX_NO_ERROR)
				{
					valR.maxVal = valD;
					res = jvx_set_property(theTriple.theProps, &valR, offset, 1, format, false, descr, callGate);
					if (accProt)
					{
						*accProt = callGate.access_protocol;
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else
			{
				res = jvx_set_property(theTriple.theProps, &valR, offset, 1, format, true, descr, callGate);
				if (accProt)
				{
					*accProt = callGate.access_protocol;
				}
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	}
	}
	return(res);
}

jvxErrorType
CjvxPropertiesToPythonConverter::convertSingleNumericalUnion(jvxDataFormat format, numTypeConvert& inputConvert, const pybind11::object& prhs)
{
	jvxErrorType res = JVX_ERROR_INVALID_FORMAT;

	// Python ints are arbitrary precision, so an exact integer round-trip is
	// preferred over routing through a double (which would lose precision for
	// large 64-bit values) - mirrors the original's per-dtype mxIsInt*/mxIsUint*
	// branches, but driven by Python's own int/float distinction instead of a
	// fixed set of numpy classes.
	bool haveInt = py::isinstance<py::int_>(prhs);
	bool haveFloat = !haveInt && (py::isinstance<py::float_>(prhs) || py::isinstance<py::array>(prhs));

	if (!haveInt && !haveFloat)
	{
		return res;
	}

	try
	{
		jvxInt64 asInt = haveInt ? prhs.cast<jvxInt64>() : (jvxInt64)0;
		double asDbl = haveFloat ? prhs.cast<double>() : (double)asInt;

		res = JVX_NO_ERROR;
		switch (format)
		{
		case JVX_DATAFORMAT_DATA:
			inputConvert.singleDat = haveInt ? (jvxData)asInt : (jvxData)asDbl;
			break;
		case JVX_DATAFORMAT_8BIT:
			inputConvert.singleInt8 = haveInt ? (jvxInt8)asInt : (jvxInt8)asDbl;
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			inputConvert.singleInt16 = haveInt ? (jvxInt16)asInt : (jvxInt16)asDbl;
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			inputConvert.singleInt32 = haveInt ? (jvxInt32)asInt : (jvxInt32)asDbl;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			inputConvert.singleInt64 = haveInt ? (jvxInt64)asInt : (jvxInt64)asDbl;
			break;
		case JVX_DATAFORMAT_U8BIT:
			inputConvert.singleUInt8 = haveInt ? (jvxUInt8)asInt : (jvxUInt8)asDbl;
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			inputConvert.singleUInt16 = haveInt ? (jvxUInt16)asInt : (jvxUInt16)asDbl;
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			inputConvert.singleUInt32 = haveInt ? (jvxUInt32)asInt : (jvxUInt32)asDbl;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			inputConvert.singleUInt64 = haveInt ? (jvxUInt64)asInt : (jvxUInt64)asDbl;
			break;
		default:
			return JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	catch (const py::cast_error&)
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return res;
}

jvxErrorType
CjvxPropertiesToPythonConverter::pyGetPropertyCore(pybind11::object& arr,
	jvx_propertyReferenceTriple& theTriple, const std::string& descString,
	jvxSize offset, std::string& errMessOnReturn)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	errMessOnReturn.clear();
	jvx::propertyAddress::CjvxPropertyAddressDescriptor idProp(descString.c_str());
	jvx::propertyDescriptor::CjvxPropertyDescriptorCore descr;

	// Map given unique id to one specific property. We need the format and other infos before accessing the content
	res = theTriple.theProps->description_property(callGate, descr, idProp);

	if (res == JVX_NO_ERROR && (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK))
	{
		switch (descr.format)
		{
		case JVX_DATAFORMAT_8BIT:
		case JVX_DATAFORMAT_16BIT_LE:
		case JVX_DATAFORMAT_32BIT_LE:
		case JVX_DATAFORMAT_64BIT_LE:
		case JVX_DATAFORMAT_U8BIT:
		case JVX_DATAFORMAT_U16BIT_LE:
		case JVX_DATAFORMAT_U32BIT_LE:
		case JVX_DATAFORMAT_U64BIT_LE:
		case JVX_DATAFORMAT_DATA:

			res = pyReturnPropertyNumerical(arr, descr.format, descr.num, descString.c_str(), theTriple, offset);
			if (res != JVX_NO_ERROR)
			{
				errMessOnReturn = __FUNCTION__;
				errMessOnReturn += "Error: Component request returned error message <";
				errMessOnReturn += jvxErrorType_descr(res);
				errMessOnReturn += ">.";
			}
			break;
		case JVX_DATAFORMAT_SIZE:

			res = pyReturnPropertyNumericalSize(arr, descr.format, descr.num, descString.c_str(), theTriple, offset);
			if (res != JVX_NO_ERROR)
			{
				errMessOnReturn = __FUNCTION__;
				errMessOnReturn += "Component request returned error message <";
				errMessOnReturn += jvxErrorType_descr(res);
				errMessOnReturn += ">.";
			}
			break;
		case JVX_DATAFORMAT_SELECTION_LIST:
		case JVX_DATAFORMAT_STRING:
		case JVX_DATAFORMAT_STRING_LIST:
		case JVX_DATAFORMAT_VALUE_IN_RANGE:

			res = this->pyReturnPropertyOthers(arr, descr.format, descr.num, descString.c_str(), theTriple, offset);
			if (res != JVX_NO_ERROR)
			{
				errMessOnReturn = __FUNCTION__;
				errMessOnReturn += "Component request returned error message <";
				errMessOnReturn += jvxErrorType_descr(res);
				errMessOnReturn += ">.";
			}
			break;
		default:
			errMessOnReturn = "Request for unsupported property type.";
			res = JVX_ERROR_UNSUPPORTED;
		}
	}
	else
	{
		errMessOnReturn = __FUNCTION__;
		errMessOnReturn += "Did not find property with specified descriptor tag.";
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return res;
}

jvxErrorType
CjvxPropertiesToPythonConverter::pySetPropertyCore(pybind11::object& arrOut,
	const std::vector<pybind11::object>& prhs, int nrhs_off, int nrhs,
	jvx_propertyReferenceTriple& theTriple, const std::string& descString, std::string& errMessOnReturn)
{
	jvxSize offset = 0;
	jvxErrorType res = JVX_NO_ERROR;
	jvx::propertyDescriptor::CjvxPropertyDescriptorCore descr;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor idProp(nullptr);
	jvxCallManagerProperties callGate;

	// Map given unique id to one specific property. We need the format and other infos before accessing the content
	idProp.descriptor = descString.c_str();
	res = theTriple.theProps->description_property(callGate, descr, idProp);

	std::string valS;
	jvxInt32 valI;
	if (
		(CjvxPythonToCConverter::pyArgument2String(valS, prhs, nrhs_off + 1, nrhs) == JVX_NO_ERROR) &&
		(CjvxPythonToCConverter::pyArgument2Index<jvxInt32>(valI, prhs, nrhs_off + 2, nrhs) == JVX_NO_ERROR))
	{
		if (valS == "offset")
		{
			nrhs_off += 2;
			offset = (jvxSize)valI;
		}
	}

	if (res == JVX_NO_ERROR && (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK))
	{
		switch (descr.format)
		{
		case JVX_DATAFORMAT_8BIT:
		case JVX_DATAFORMAT_16BIT_LE:
		case JVX_DATAFORMAT_32BIT_LE:
		case JVX_DATAFORMAT_64BIT_LE:
		case JVX_DATAFORMAT_U8BIT:
		case JVX_DATAFORMAT_U16BIT_LE:
		case JVX_DATAFORMAT_U32BIT_LE:
		case JVX_DATAFORMAT_U64BIT_LE:
		case JVX_DATAFORMAT_DATA:
		case JVX_DATAFORMAT_SIZE:
			res = copyDataToComponentNumerical(prhs[nrhs_off], theTriple, descr.format, descr.num, descString.c_str(), offset, &callGate.access_protocol);
			if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, descString, theTriple.theProps))
			{
				arrOut = converter->pyReturnBool(true);
			}
			else
			{
				errMessOnReturn = __FUNCTION__;
				errMessOnReturn += "Failed to copy property data to property <";
				errMessOnReturn += descString;
				errMessOnReturn += ">.";
				if (res == JVX_NO_ERROR)
				{
					res = JVX_ERROR_INVALID_SETTING;
				}
			}
			break;
		case JVX_DATAFORMAT_SELECTION_LIST:
		case JVX_DATAFORMAT_STRING:
		case JVX_DATAFORMAT_STRING_LIST:
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
		{
			std::vector<pybind11::object> subArgs(prhs.begin() + nrhs_off, prhs.begin() + nrhs);
			res = copyDataToComponentOthers(subArgs, nrhs - nrhs_off, theTriple, descr.format, descr.num, descString.c_str(), offset, &callGate.access_protocol);
			if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, descString, theTriple.theProps))
			{
				arrOut = converter->pyReturnBool(true);
			}
			else
			{
				errMessOnReturn = __FUNCTION__;
				errMessOnReturn += "Failed to copy property data to property <";
				errMessOnReturn += descString;
				errMessOnReturn += ">.";
				if (res == JVX_NO_ERROR)
				{
					res = JVX_ERROR_INVALID_SETTING;
				}
			}
			break;
		}
		default:
			errMessOnReturn = __FUNCTION__;
			errMessOnReturn += "Request for unsupported property <";
			errMessOnReturn += descString;
			errMessOnReturn += "> which is of type <";
			errMessOnReturn += jvxDataFormat_txt(descr.format);
			errMessOnReturn += ">.";
		}
	}
	else
	{
		errMessOnReturn = __FUNCTION__;
		errMessOnReturn += "Request for property <";
		errMessOnReturn += descString;
		errMessOnReturn += "> which does not exist.";
	}
	return res;
}
