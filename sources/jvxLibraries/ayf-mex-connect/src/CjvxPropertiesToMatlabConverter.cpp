// Need to include jvx.h here since the order of inclusion matters!
#include "jvx.h"

#if _MATLAB_MEXVERSION < 500
#if (_MSC_VER >= 1600)
#include <yvals.h>
#define __STDC_UTF_16__
#endif
#endif
#include "mex.h"

#include <iostream>

#if _MATLAB_MEXVERSION >= 100
#define SZ_MAT_TYPE mwSize
#else
#define SZ_MAT_TYPE int
#endif

#include "CjvxPropertiesToMatlabConverter.h"
#include "CjvxCToMatlabConverter.h"

#define ERROR_MESSAGE_REPORT(fct) ((std::string)"Internal error when using CjvxPropertiesToMatlabConverter, member function " + fct + (std::string)", reason: ")

void
CjvxPropertiesToMatlabConverter::mexReturnStructProperties(mxArray*& plhs, jvx_propertyReferenceTriple& theTriple)
{
	jvxSize i;
	const char** fld = NULL;
	std::string* fldStr = NULL;
	mxArray* arr = NULL;

	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	jvxSize idxProp = 0;
	jvxCallManagerProperties callGate;
	res = theTriple.theProps->number_properties(callGate, &num);

	if(num > 0)
	{
		SZ_MAT_TYPE ndim = 2;
		SZ_MAT_TYPE dims[2] = {1, 1};
		JVX_SAFE_NEW_FLD(fld, const char*, num);
		JVX_SAFE_NEW_FLD(fldStr, std::string, num);

		for(i = 0; i < num; i ++)
		{
			fldStr[i] = PREFIX_ONE_PROPERTY + jvx_size2String(i);
			fld[i] = fldStr[i].c_str();
		}

		plhs = mxCreateStructArray(ndim, dims, (int)num, fld);

		JVX_SAFE_DELETE_FLD(fld, const char*);
		JVX_SAFE_DELETE_FLD(fldStr, std::string);

		for(i = 0; i < num; i ++)
		{
			jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus theDescr;
			jvx::propertyAddress::CjvxPropertyAddressLinear ident(i);
			arr = NULL;
			res = theTriple.theProps->description_property(callGate, theDescr, ident);
			//if(JVX_PROPERTY_FLAG_FULL_ALLOW, callGate.access_protocol)
			//{

				this->mexReturnStructOneProperty(arr, theDescr.category, theDescr.allowedStateMask, theDescr.allowedThreadingMask, 
					theDescr.format, theDescr.num, theDescr.accessType, theDescr.decTp, theDescr.globalIdx,
					theDescr.ctxt, theDescr.name.std_str(), 
					theDescr.description.c_str(), theDescr.descriptor.std_str(), 
					theDescr.isValid, theDescr.accessFlags);
			//}
			//else
			//{
			//	mexFillEmpty(&arr, 1, 0);
			//}

			mxSetFieldByNumber(plhs, 0, (int)i, arr);
		}
	}
	else
	{
		converter->mexFillEmpty(&plhs, 1, 0);
	}
}

void
CjvxPropertiesToMatlabConverter::mexReturnStructOneProperty(mxArray*& plhs, jvxPropertyCategoryType catProperty, jvxUInt64 allowStateMask, jvxUInt64 allowThreadingMask,
	jvxDataFormat format, jvxSize numElms, jvxPropertyAccessType accessType, jvxPropertyDecoderHintType decHtTp, jvxSize hdlIdx,
	jvxPropertyContext context, const std::string& name, const std::string& description, const std::string& descriptor,
	jvxBool isValid, jvxFlagTag accessFlags)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	mxArray* arr = NULL;

	std::string fldName0 = FLD_NAME_PROPERTY_NAME_STRING;
	std::string fldName1 = FLD_NAME_PROPERTY_DESCRIPTION_STRING;
	std::string fldName2 = FLD_NAME_PROPERTY_CATEGORY_INT32;
	std::string fldName3 = FLD_NAME_PROPERTY_UNIQUE_ID_INT32;
	std::string fldName4 = FLD_NAME_PROPERTY_FORMAT_INT32;
	std::string fldName5 = FLD_NAME_PROPERTY_NUMBER_ELEMENTS_INT32;
	std::string fldName6 = FLD_NAME_PROPERTY_DECODER_HINT_TYPE_INT32;
	std::string fldName7 = FLD_NAME_PROPERTY_ALLOW_STATE_MASK_INT64;
	std::string fldName8 = FLD_NAME_PROPERTY_ALLOW_THREADING_MASK_INT64;
	std::string fldName9 = FLD_NAME_PROPERTY_ACCESS_TYPE_INT32;
	std::string fldName10 = FLD_NAME_PROPERTY_DESCRIPTOR_STRING;
	std::string fldName11 = FLD_NAME_PROPERTY_CONTEXT_INT32;
	std::string fldName12 = FLD_NAME_PROPERTY_IS_VALID_BOOL;
	std::string fldName13 = FLD_NAME_PROPERTY_ACCESS_FLAGS_INT32;

	const char* fld[14];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();
	fld[3] = fldName3.c_str();
	fld[4] = fldName4.c_str();
	fld[5] = fldName5.c_str();
	fld[6] = fldName6.c_str();
	fld[7] = fldName7.c_str();
	fld[8] = fldName8.c_str();
	fld[9] = fldName9.c_str();
	fld[10] = fldName10.c_str();
	fld[11] = fldName11.c_str();
	fld[12] = fldName12.c_str();
	fld[13] = fldName13.c_str();

	plhs = mxCreateStructArray(ndim, dims, 14, fld);

	arr = NULL;
	converter->mexReturnString(arr, name);
	mxSetFieldByNumber(plhs, 0, 0, arr);

	arr = NULL;
	converter->mexReturnString(arr, description);
	mxSetFieldByNumber(plhs, 0, 1, arr);

	arr = NULL;
	converter->mexReturnInt32(arr, (jvxInt32)catProperty);
	mxSetFieldByNumber(plhs, 0, 2, arr);

	arr = NULL;
	converter->mexReturnInt32(arr, (jvxInt32)hdlIdx);
	mxSetFieldByNumber(plhs, 0, 3, arr);

	arr = NULL;
	converter->mexReturnInt32(arr, (jvxInt32)format);
	mxSetFieldByNumber(plhs, 0, 4, arr);

	arr = NULL;
	converter->mexReturnInt32(arr, (jvxInt32)numElms);
	mxSetFieldByNumber(plhs, 0, 5, arr);

	arr = NULL;
	converter->mexReturnInt32(arr, (jvxInt32)decHtTp);
	mxSetFieldByNumber(plhs, 0, 6, arr);

	arr = NULL;
	converter->mexReturnInt64(arr, (jvxInt64)allowStateMask);
	mxSetFieldByNumber(plhs, 0, 7, arr);

	arr = NULL;
	converter->mexReturnInt64(arr, (jvxInt64)allowThreadingMask);
	mxSetFieldByNumber(plhs, 0, 8, arr);

	arr = NULL;
	converter->mexReturnInt64(arr, (jvxInt32)accessType);
	mxSetFieldByNumber(plhs, 0, 9, arr);

	arr = NULL;
	converter->mexReturnString(arr, descriptor);
	mxSetFieldByNumber(plhs, 0, 10, arr);

	arr = NULL;
	converter->mexReturnInt32(arr, (jvxInt32)context);
	mxSetFieldByNumber(plhs, 0, 11, arr);

	arr = NULL;
	converter->mexReturnBool(arr, isValid);
	mxSetFieldByNumber(plhs, 0, 12, arr);

	arr = NULL;
	converter->mexReturnInt32(arr, (jvxInt32)accessFlags);
	mxSetFieldByNumber(plhs, 0, 13, arr);
}

jvxErrorType
CjvxPropertiesToMatlabConverter::mexReturnPropertyNumerical(mxArray*& plhs, jvxSize hdlIdx, jvxPropertyCategoryType catTp, jvxDataFormat format,
	jvxSize numElms, jvxPropertyDecoderHintType decHtTp, jvx_propertyReferenceTriple& theTriple, jvxSize offset)
{
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	mxArray* arr = NULL;
	jvxHandle* data = NULL;
	mxClassID classIdAlloc;
	bool isValid = false;
	jvxCallManagerProperties callGate;
	jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE;
	callGate.on_get.prop_access_type = &accessType;

	std::string fldName0 = FLD_NAME_PROPERTY_SUBFIELD;
	std::string fldName1 = FLD_NAME_PROPERTY_ACCESS_TYPE_INT32;
	std::string fldName2 = FLD_NAME_PROPERTY_DECODER_HINT_INT32;
	std::string fldName3 = FLD_NAME_PROPERTY_ACCESS_PROTOCOL_INT32;

	const char* fld[4];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();
	fld[3] = fldName3.c_str();


	plhs = mxCreateStructArray(ndim, dims, 4, fld);

	arr = NULL;
	if (numElms > 0)
	{
		switch (format)
		{
		case JVX_DATAFORMAT_8BIT:
			classIdAlloc = mxINT8_CLASS;
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			classIdAlloc = mxINT16_CLASS;
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			classIdAlloc = mxINT32_CLASS;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			classIdAlloc = mxINT64_CLASS;
			break;
		case JVX_DATAFORMAT_U8BIT:
			classIdAlloc = mxUINT8_CLASS;
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			classIdAlloc = mxUINT16_CLASS;
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			classIdAlloc = mxINT32_CLASS;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			classIdAlloc = mxINT64_CLASS;
			break;
		case JVX_DATAFORMAT_DATA:
			classIdAlloc = JVX_DATA_MEX_CONVERT;
			break;
		default:
			assert(0);
		}

		dims[1] = numElms;
		arr = mxCreateNumericArray(ndim, dims, classIdAlloc, mxREAL);
		data = mxGetData(arr);
		res = theTriple.theProps->get_property(callGate, jPRG(data, numElms, format),
			jPAGID(hdlIdx, catTp),
			jPD(true, offset));
		if (res != JVX_NO_ERROR)
		{
			mxDestroyArray(arr);
			arr = NULL;
		}
	}
	else
	{
		converter->mexFillEmpty(&arr, 1, 0);
	}

	if (res == JVX_NO_ERROR)
	{
		mxSetFieldByNumber(plhs, 0, 0, arr);

		arr = NULL;
		converter->mexReturnInt32(arr, accessType);
		mxSetFieldByNumber(plhs, 0, 1, arr);

		arr = NULL;
		converter->mexReturnInt32(arr, decHtTp);
		mxSetFieldByNumber(plhs, 0, 2, arr);

		arr = NULL;
		converter->mexReturnInt32(arr, callGate.access_protocol);
		mxSetFieldByNumber(plhs, 0, 3, arr);
	}
	else
	{
		mxDestroyArray(plhs);
		plhs = NULL;
	}
	return res;
}

jvxErrorType
CjvxPropertiesToMatlabConverter::mexReturnPropertyNumericalSize(mxArray*& plhs, jvxSize hdlIdx, jvxPropertyCategoryType catTp, jvxDataFormat format, jvxSize numElms,
	jvxPropertyDecoderHintType decHtTp, jvx_propertyReferenceTriple& theTriple, jvxSize offset)
{
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	jvxSize i;
	mxArray* arr = NULL;
	jvxSize* data = NULL;
	jvxInt32* dataMex = NULL;
	jvxCallManagerProperties callGate;
	bool isValid = false;
	jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE;
	callGate.on_get.prop_access_type = &accessType;

	std::string fldName0 = FLD_NAME_PROPERTY_SUBFIELD;
	std::string fldName1 = FLD_NAME_PROPERTY_ACCESS_TYPE_INT32;
	std::string fldName2 = FLD_NAME_PROPERTY_DECODER_HINT_INT32;
	std::string fldName3 = FLD_NAME_PROPERTY_ACCESS_PROTOCOL_INT32;

	const char* fld[4];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();
	fld[3] = fldName3.c_str();

	plhs = mxCreateStructArray(ndim, dims, 4, fld);

	arr = NULL;
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
			dims[1] = numElms;
			arr = mxCreateNumericArray(ndim, dims, mxINT32_CLASS, mxREAL);
			dataMex = (jvxInt32*)mxGetData(arr);
			for (i = 0; i < numElms; i++)
			{
				dataMex[i] = JVX_SIZE_INT32(data[i]);
			}
		}
		JVX_DSP_SAFE_DELETE_FIELD(data);
	}
	else
	{
		converter->mexFillEmpty(&arr, 1, 0);
	}

	if (res == JVX_NO_ERROR)
	{
		mxSetFieldByNumber(plhs, 0, 0, arr);

		arr = NULL;
		converter->mexReturnInt32(arr, accessType);
		mxSetFieldByNumber(plhs, 0, 1, arr);

		arr = NULL;
		converter->mexReturnInt32(arr, decHtTp);
		mxSetFieldByNumber(plhs, 0, 2, arr);

		arr = NULL;
		converter->mexReturnInt32(arr, callGate.access_protocol);
		mxSetFieldByNumber(plhs, 0, 3, arr);
	}
	else
	{
		mxDestroyArray(plhs);
		plhs = NULL;
	}
	return res;
}

jvxErrorType
CjvxPropertiesToMatlabConverter::mexReturnPropertyNumerical(mxArray*& plhs, jvxDataFormat format, jvxSize numElms, const char* descr, jvx_propertyReferenceTriple& theTriple, jvxSize offset)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;

	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	jvxHandle* data = NULL;
	mxClassID classIdAlloc;
	if (numElms > 0)
	{
		switch (format)
		{
		case JVX_DATAFORMAT_8BIT:
			classIdAlloc = mxINT8_CLASS;
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			classIdAlloc = mxINT16_CLASS;
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			classIdAlloc = mxINT32_CLASS;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			classIdAlloc = mxINT64_CLASS;
			break;
		case JVX_DATAFORMAT_U8BIT:
			classIdAlloc = mxUINT8_CLASS;
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			classIdAlloc = mxUINT16_CLASS;
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			classIdAlloc = mxUINT32_CLASS;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			classIdAlloc = mxUINT64_CLASS;
			break;
		case JVX_DATAFORMAT_DATA:
			classIdAlloc = JVX_DATA_MEX_CONVERT;
			break;
		default:
			assert(0);
		}

		dims[1] = numElms;
		plhs = mxCreateNumericArray(ndim, dims, classIdAlloc, mxREAL);
		data = mxGetData(plhs);
		res = jvx_get_property(theTriple.theProps, data, offset, numElms, format, true, descr, callGate);
		if (res != JVX_NO_ERROR)
		{
			mxDestroyArray(plhs);
			plhs = NULL;
		}
	}
	else
	{
		converter->mexFillEmpty(&plhs, 1, 0);
	}
	return res;
}

jvxErrorType
CjvxPropertiesToMatlabConverter::mexReturnPropertyNumericalSize(mxArray*& plhs, jvxDataFormat format, jvxSize numElms, const char* descr, jvx_propertyReferenceTriple& theTriple, jvxSize offset)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	jvxInt32* dataMex = NULL;
	jvxSize* data = NULL;
	if (numElms > 0)
	{
		assert(format == JVX_DATAFORMAT_SIZE);

		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data, jvxSize, numElms);

		res = jvx_get_property(theTriple.theProps, data, offset, numElms, format, true, descr, callGate);

		if (res == JVX_NO_ERROR)
		{
			dims[1] = numElms;
			plhs = mxCreateNumericArray(ndim, dims, mxINT32_CLASS, mxREAL);
			dataMex = (jvxInt32*)mxGetData(plhs);

			for (i = 0; i < numElms; i++)
			{
				dataMex[i] = JVX_SIZE_INT32(data[i]);
			}
		}
		JVX_DSP_SAFE_DELETE_FIELD(data);
	}
	else
	{
		converter->mexFillEmpty(&plhs, 1, 0);
	}
	return res;
}

jvxErrorType
CjvxPropertiesToMatlabConverter::mexReturnPropertyOthers(mxArray*& plhs, jvxSize hdlIdx, jvxPropertyCategoryType catTp, jvxDataFormat format, jvxSize numElms,
	jvxPropertyDecoderHintType decHtTp, jvx_propertyReferenceTriple& theTriple, jvxSize offset)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	mxArray* arr = NULL;
	jvxHandle* data = NULL;
	//mxClassID classIdAlloc;
	bool isValid = false;
	jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE;

	jvxSelectionList theSelList;
	jvxApiString fldStr;
	jvxApiStringList fldStrLst;
	jvxValueInRange vInRange;

	std::string txt;
	std::vector<std::string> lstTxt;
	jvxCallManagerProperties callGate;
	callGate.on_get.prop_access_type = &accessType;

	std::string fldName0 = FLD_NAME_PROPERTY_SUBFIELD;
	std::string fldName1 = FLD_NAME_PROPERTY_ACCESS_TYPE_INT32;
	std::string fldName2 = FLD_NAME_PROPERTY_DECODER_HINT_INT32;
	std::string fldName3 = FLD_NAME_PROPERTY_ACCESS_PROTOCOL_INT32;

	const char* fld[4];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();
	fld[3] = fldName3.c_str();

	plhs = mxCreateStructArray(ndim, dims, 4, fld);

	arr = NULL;
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
				converter->mexReturnSelectionListStruct(arr, theSelList);
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
				converter->mexReturnString(arr, txt);
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

				converter->mexReturnStringCell(arr, lstTxt);
			}

			break;
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
			res = theTriple.theProps->get_property(callGate,
				jPRG(&vInRange, 1, format),
				jPAGID(hdlIdx, catTp),
				jPD(false, offset));
			if (res == JVX_NO_ERROR)
			{
				converter->mexReturnValueInRangeStruct(arr, vInRange);
			}

			break;
		default:
			assert(0);
		}
	}
	else
	{
		converter->mexFillEmpty(&arr, 1, 0);
	}

	if (res == JVX_NO_ERROR)
	{
		mxSetFieldByNumber(plhs, 0, 0, arr);

		arr = NULL;
		converter->mexReturnInt32(arr, accessType);
		mxSetFieldByNumber(plhs, 0, 1, arr);

		arr = NULL;
		converter->mexReturnInt32(arr, decHtTp);
		mxSetFieldByNumber(plhs, 0, 2, arr);

		arr = NULL;
		converter->mexReturnInt32(arr, callGate.access_protocol);
		mxSetFieldByNumber(plhs, 0, 3, arr);
	}
	else
	{
		// Remove previously allocated memory
		mxDestroyArray(plhs);
		plhs = NULL;
	}
	return res;
}

jvxErrorType
CjvxPropertiesToMatlabConverter::mexReturnPropertyOthers(mxArray*& plhs, jvxDataFormat format, jvxSize numElms, const char* descr, jvx_propertyReferenceTriple& theTriple, jvxSize offset)
{
	int i;
	jvxHandle* data = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList theSelList;
	jvxApiString fldStr;
	jvxApiStringList fldStrLst;
	jvxValueInRange vInRange;

	std::string txt;
	std::vector<std::string> lstTxt;
	jvxCallManagerProperties callGate;
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
				converter->mexReturnSelectionListStruct(plhs, theSelList);
			}

			break;
		case JVX_DATAFORMAT_STRING:

			res = jvx_get_property(theTriple.theProps, &fldStr, offset, 1, format, false, descr, callGate);
			if (res == JVX_NO_ERROR)
			{
				txt = fldStr.std_str();
				converter->mexReturnString(plhs, txt);
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

				converter->mexReturnStringCell(plhs, lstTxt);
			}

			break;
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
			res = jvx_get_property(theTriple.theProps, &vInRange, offset, 1, format, false, descr, callGate);
			if (res == JVX_NO_ERROR)
			{
				converter->mexReturnValueInRangeStruct(plhs, vInRange);
			}

			break;
		default:
			assert(0);
		}
	}
	else
	{
		converter->mexFillEmpty(&plhs, 1, 0);
	}
	return res;
}

// ===================================================================================

jvxErrorType
CjvxPropertiesToMatlabConverter::copyDataToComponentNumerical(const mxArray* prhs, jvx_propertyReferenceTriple& theTriple, jvxPropertyCategoryType cat, jvxDataFormat format,
	jvxSize numElms, jvxSize uniqueId, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = NULL;
	jvxCallManagerProperties callGate;
	numTypeConvert inputConvert = { 0 };

	switch (format)
	{
	case JVX_DATAFORMAT_DATA:
		if (mxIsData(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		if (mxIsInt64(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		if (mxIsInt32(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		if (mxIsInt16(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		if (mxIsInt8(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_U64BIT_LE:
		if (mxIsUint64(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_U32BIT_LE:
		if (mxIsUint32(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		if (mxIsUint16(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_U8BIT:
		if (mxIsUint8(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	}

	if (!data)
	{
		/*
		jvxData singleDat = 0;
		jvxInt8 singleInt8 = 0;
		jvxInt16 singleInt16 = 0;
		jvxInt32 singleInt32 = 0;
		jvxInt64 singleInt64 = 0;
		jvxInt8 singleInt8 = 0;
		jvxUInt16 singleUInt16 = 0;
		jvxUInt32 singleUInt32 = 0;
		jvxUInt64 singleUInt64 = 0;
		*/
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
CjvxPropertiesToMatlabConverter::copyDataToComponentNumericalSize(const mxArray* prhs, jvx_propertyReferenceTriple& theTriple, jvxPropertyCategoryType cat,
	jvxDataFormat format, jvxSize numElms, jvxSize uniqueId, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = NULL;
	jvxSize* dataSz = NULL;
	jvxCallManagerProperties callGate;
	assert(format == JVX_DATAFORMAT_SIZE);
	if (mxIsInt32(prhs))
	{
		if (mxGetM(prhs) == 1)
		{
			if (mxGetN(prhs) == numElms)
			{
				data = mxGetData(prhs);
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(dataSz, jvxSize, numElms);
				for (i = 0; i < numElms; i++)
				{
					dataSz[i] = JVX_INT_SIZE(((jvxInt32*)data)[i]);
				}
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
CjvxPropertiesToMatlabConverter::copyDataToComponentNumerical(const mxArray* prhs, jvx_propertyReferenceTriple& theTriple, jvxDataFormat format, jvxSize numElms,
	const char* descr, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */

	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = NULL;
	jvxCallManagerProperties callGate;
	jvxData* data_dat = NULL;
	jvxSize* data_sz = NULL;
	jvxInt64* data_int64 = NULL;
	jvxInt32* data_int32 = NULL;
	jvxInt16* data_int16 = NULL;
	jvxInt8* data_int8 = NULL;
	jvxUInt64* data_uint64 = NULL;
	jvxUInt32* data_uint32 = NULL;
	jvxUInt16* data_uint16 = NULL;
	jvxUInt8* data_uint8 = NULL;
	jvxBool formatFits = false;
	jvxHandle* data_setprop = NULL;

	if (mxGetM(prhs) == 1)
	{
		if (mxGetN(prhs) == numElms)
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

		convert_mat_buf_c_buf_1_x_N(data_setprop, format, numElms, prhs);

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
		case JVX_DATAFORMAT_DATA:
			JVX_DSP_SAFE_DELETE_FIELD(data_dat);
			break;
		case JVX_DATAFORMAT_SIZE:
			JVX_DSP_SAFE_DELETE_FIELD(data_sz);
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			JVX_DSP_SAFE_DELETE_FIELD(data_int64);
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			JVX_DSP_SAFE_DELETE_FIELD(data_int32);
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			JVX_DSP_SAFE_DELETE_FIELD(data_int16);
			break;
		case JVX_DATAFORMAT_8BIT:
			JVX_DSP_SAFE_DELETE_FIELD(data_int8);
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			JVX_DSP_SAFE_DELETE_FIELD(data_uint64);
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			JVX_DSP_SAFE_DELETE_FIELD(data_uint32);
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			JVX_DSP_SAFE_DELETE_FIELD(data_uint16);
			break;
		case JVX_DATAFORMAT_U8BIT:
			JVX_DSP_SAFE_DELETE_FIELD(data_uint8);
			break;
		}
	}
	return(res);
}

#define JVX_CONVERSION_LOOP(failedTransfer, dat_src, jvx_dest, N, TPConvert, FConvert ) \
	if (dat_src) \
	{ \
		jvxSize i; \
		failedTransfer = false; \
		for (i = 0; i < N; i++) \
		{ \
			TPConvert tmp = (TPConvert)dat_src[i]; \
			jvx_dest[i] = FConvert(tmp); \
		} \
	}

#define JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_src, jvx_dest, N, TPConvert, FConvert, LMax, LMin ) \
	if (dat_src) \
	{ \
		jvxSize i; \
		failedTransfer = false; \
		for (i = 0; i < N; i++) \
		{ \
			TPConvert tmp = (TPConvert)dat_src[i]; \
			tmp = JVX_MIN(tmp, LMax); \
			tmp = JVX_MAX(tmp, LMin); \
			jvx_dest[i] = FConvert(tmp); \
		} \
	}

jvxErrorType
CjvxPropertiesToMatlabConverter::convert_mat_buf_c_buf_1_x_N(jvxHandle* data_setprops, jvxDataFormat format, jvxSize N, const mxArray* prhs)
{
	double* dat_dbl = NULL;
	float* dat_flt = NULL;

	jvxInt64* dat_int64 = NULL;
	jvxInt32* dat_int32 = NULL;
	jvxInt16* dat_int16 = NULL;
	jvxInt8* dat_int8 = NULL;
	jvxUInt64* dat_uint64 = NULL;
	jvxUInt32* dat_uint32 = NULL;
	jvxUInt16* dat_uint16 = NULL;
	jvxUInt8* dat_uint8 = NULL;

	jvxData* jvx_data = NULL;
	jvxInt64* jvx_int64 = NULL;
	jvxInt32* jvx_int32 = NULL;
	jvxInt16* jvx_int16 = NULL;
	jvxInt8* jvx_int8 = NULL;
	jvxUInt64* jvx_uint64 = NULL;
	jvxUInt32* jvx_uint32 = NULL;
	jvxUInt16* jvx_uint16 = NULL;
	jvxUInt8* jvx_uint8 = NULL;
	jvxSize* jvx_sz = NULL;

	jvxSize i;

	if (mxIsDouble(prhs))
	{
		dat_dbl = (double*)mxGetData(prhs);
	}
	else if (mxIsSingle(prhs))
	{
		dat_flt = (float*)mxGetData(prhs);
	}
	else if (mxIsInt64(prhs))
	{
		dat_int64 = (jvxInt64*)mxGetData(prhs);
	}
	else if (mxIsInt32(prhs))
	{
		dat_int32 = (jvxInt32*)mxGetData(prhs);
	}
	else if (mxIsInt16(prhs))
	{
		dat_int16 = (jvxInt16*)mxGetData(prhs);
	}
	else if (mxIsInt8(prhs))
	{
		dat_int8 = (jvxInt8*)mxGetData(prhs);
	}
	else if (mxIsUint64(prhs))
	{
		dat_uint64 = (jvxUInt64*)mxGetData(prhs);
	}
	else if (mxIsUint32(prhs))
	{
		dat_uint32 = (jvxUInt32*)mxGetData(prhs);
	}
	else if (mxIsUint16(prhs))
	{
		dat_uint16 = (jvxUInt16*)mxGetData(prhs);
	}
	else if (mxIsUint8(prhs))
	{
		dat_uint8 = (jvxUInt8*)mxGetData(prhs);
	}

	// Now, convert
	switch (format)
	{
	case JVX_DATAFORMAT_DATA:
		jvx_data = (jvxData*)data_setprops;
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		jvx_int64 = (jvxInt64*)data_setprops;
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		jvx_int32 = (jvxInt32*)data_setprops;
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		jvx_int16 = (jvxInt16*)data_setprops;
		break;
	case JVX_DATAFORMAT_8BIT:
		jvx_int8 = (jvxInt8*)data_setprops;
		break;
	case JVX_DATAFORMAT_U64BIT_LE:
		jvx_uint64 = (jvxUInt64*)data_setprops;
		break;
	case JVX_DATAFORMAT_U32BIT_LE:
		jvx_uint32 = (jvxUInt32*)data_setprops;
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		jvx_uint16 = (jvxUInt16*)data_setprops;
		break;
	case JVX_DATAFORMAT_U8BIT:
		jvx_uint8 = (jvxUInt8*)data_setprops;
		break;
	case JVX_DATAFORMAT_SIZE:
		jvx_sz = (jvxSize*)data_setprops;
		break;
	}

	jvxBool failedTransfer = true;

	if (jvx_data)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_dbl, jvx_data, N, double, (jvxData));
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_flt, jvx_data, N, jvxData, );
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int64, jvx_data, N, jvxData, );
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_data, N, jvxData, );
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_data, N, jvxData, );
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_data, N, jvxData, );
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint64, jvx_data, N, jvxData, );
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_data, N, jvxData, );
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_data, N, jvxData, );
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_data, N, jvxData, );
		}
	}
	else if (jvx_sz)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_dbl, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_flt, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int64, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint64, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
	}
	else if (jvx_int64)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_dbl, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_flt, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int64, jvx_int64, N, jvxInt64, );
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint64, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
	}
	else if (jvx_int32)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_int32, N, jvxData, JVX_DATA2INT32, 0x7FFFFFFF, -0x7FFFFFFF);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_int32, N, jvxData, JVX_DATA2INT32, 0x7FFFFFFF, -0x7FFFFFFF);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_int32, N, jvxData, JVX_DATA2INT32, 0x7FFFFFFF, -0x7FFFFFFF);
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_int32, N, jvxInt32, );
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_int32, N, jvxData, JVX_DATA2INT32);
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_int32, N, jvxData, JVX_DATA2INT32);
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_int32, N, jvxData, JVX_DATA2INT32, 0x7FFFFFFF, -0x7FFFFFFF);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_int32, N, jvxData, JVX_DATA2INT32);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_int32, N, jvxData, JVX_DATA2INT32);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_int32, N, jvxData, JVX_DATA2INT32);
		}
	}
	else if (jvx_int16)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_int16, N, jvxInt16, );
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_int16, N, jvxData, JVX_DATA2INT16);
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint32, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint16, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_int16, N, jvxData, JVX_DATA2INT16);
		}
	}
	else if (jvx_int8)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int16, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_int8, N, jvxInt8, );
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint32, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint16, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint8, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
	}

	else if (jvx_uint64)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_dbl, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_flt, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int64, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint64, jvx_uint64, N, jvxUInt64, );
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
		}
	}
	else if (jvx_uint32)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0);
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0);
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_uint32, N, jvxData, JVX_DATA2UINT32);
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int8, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0);
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_uint32, N, jvxUInt32, );
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_uint32, N, jvxData, JVX_DATA2UINT32);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint32, N, jvxData, JVX_DATA2UINT32);
		}
	}
	else if (jvx_int16)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int16, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_uint16, N, jvxData, JVX_DATA2UINT16);
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint32, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_uint16, N, jvxUInt16, );
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint16, N, jvxData, JVX_DATA2UINT16);
		}
	}
	else if (jvx_uint8)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int16, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int8, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint32, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint16, jvx_uint8, N, jvxData, JVX_DATA2INT8, 0xFF, 0);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint8, N, jvxUInt8, );
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPropertiesToMatlabConverter::copyDataToComponentNumericalSize(const mxArray* prhs, jvx_propertyReferenceTriple& theTriple, jvxDataFormat format,
	jvxSize numElms, const char* descr, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = NULL;
	jvxSize* dataSz = NULL;
	jvxCallManagerProperties callGate;
	assert(format == JVX_DATAFORMAT_SIZE);
	if (mxIsInt32(prhs))
	{
		if (mxGetM(prhs) == 1)
		{
			if (mxGetN(prhs) == numElms)
			{
				data = mxGetData(prhs);
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(dataSz, jvxSize, numElms);
				for (i = 0; i < numElms; i++)
				{
					dataSz[i] = JVX_INT_SIZE(((jvxInt32*)data)[i]);
				}
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
CjvxPropertiesToMatlabConverter::copyDataToComponentOthers(const mxArray** prhs, int nrhs, jvx_propertyReferenceTriple& theTriple,
	jvxPropertyCategoryType cat, jvxDataFormat format, jvxSize numElms, jvxSize uniqueId, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = NULL;
	jvxCallManagerProperties callGate;
	jvxApiString fldStr;
	std::string token;
	std::vector<std::string> tokenLst;
	jvxSize numEntries = 0;
	const mxArray* arr = NULL;
	bool errorDetected = false;
	jvxApiStringList fldStrLst;
	jvxSelectionList selList;
	jvxInt64 valI64 = 0;
	jvxData valD;
	jvxValueInRange valR;

	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(uniqueId, cat);
	jvx::propertyDetail::CjvxTranferDetail trans(true);

	switch (format)
	{
	case JVX_DATAFORMAT_STRING:
		if (mxIsChar(prhs[0]))
		{
			token = jvx_mex_2_cstring(prhs[0]);
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
		res = mexArgument2String(tokenLst, numElms, prhs, 0, 1);
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

		res = mexArgument2String(token, prhs, 0, nrhs);
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
				res = mexArgument2String(token, prhs, 1, nrhs);
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
					res = mexArgument2StringList(tokenLst, prhs, 2, nrhs);
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
		res = mexArgument2Index<jvxData>(valD, prhs, 0, nrhs);
		if (res == JVX_NO_ERROR)
		{
			valR.val() = valD;
			res = mexArgument2Index<jvxData>(valD, prhs, 1, nrhs);
			if (res == JVX_NO_ERROR)
			{
				valR.minVal = valD;
				res = mexArgument2Index<jvxData>(valD, prhs, 2, nrhs);
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
CjvxPropertiesToMatlabConverter::copyDataToComponentOthers(const mxArray** prhs, int nrhs, jvx_propertyReferenceTriple& theTriple, jvxDataFormat format, jvxSize numElms,
	const char* descr, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = NULL;
	jvxCallManagerProperties callGate;
	jvxBitField btfld;
	jvxApiString fldStr;
	std::string token;
	std::vector<std::string> tokenLst;
	jvxSize numEntries = 0;
	const mxArray* arr = NULL;
	bool errorDetected = false;
	jvxApiStringList fldStrLst;
	jvxSelectionList selList;
	jvxInt64 valI64 = 0;
	jvxData valD;
	jvxValueInRange valR;

	switch (format)
	{
	case JVX_DATAFORMAT_STRING:
		if (mxIsChar(prhs[0]))
		{
			token = jvx_mex_2_cstring(prhs[0]);
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
		res = mexArgument2String(tokenLst, numElms, prhs, 0, 1);
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
		res = mexArgument2String(token, prhs, 0, nrhs);
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
			res = mexArgument2Index<jvxInt64>(valI64, prhs, 0, nrhs);
			btfld = (jvxBitField)valI64;
		}
		selList.bitFieldSelected() = btfld;
		if (res == JVX_NO_ERROR)
		{
			res = mexArgument2String(token, prhs, 1, nrhs);
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
				res = mexArgument2Index<jvxInt64>(valI64, prhs, 1, nrhs);
				btfld = (jvxBitField)valI64;
			}
			selList.bitFieldExclusive = btfld;

			if (res == JVX_NO_ERROR)
			{
				res = mexArgument2StringList(tokenLst, prhs, 2, nrhs);
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
		res = mexArgument2Index<jvxData>(valD, prhs, 0, nrhs);
		if (res == JVX_NO_ERROR)
		{
			valR.val() = valD;
			res = mexArgument2Index<jvxData>(valD, prhs, 1, nrhs);
			if (res == JVX_NO_ERROR)
			{
				valR.minVal = valD;
				res = mexArgument2Index<jvxData>(valD, prhs, 2, nrhs);
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
	return(res);
}

jvxErrorType
CjvxPropertiesToMatlabConverter::convertSingleNumericalUnion(jvxDataFormat format, numTypeConvert& inputConvert, const mxArray* prhs)
{
	jvxErrorType res = JVX_ERROR_INVALID_FORMAT;
	void* data = mxGetData(prhs);
	if ((mxGetM(prhs) == 1) && (mxGetN(prhs) == 1))
	{
		res = JVX_NO_ERROR;
		switch (format)
		{
		case JVX_DATAFORMAT_DATA:
			if (mxIsData(prhs)) inputConvert.singleDat = (jvxData) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleDat = (jvxData) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleDat = (jvxData) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleDat = (jvxData) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleDat = (jvxData) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleDat = (jvxData) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleDat = (jvxData) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleDat = (jvxData) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleDat = (jvxData) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;

		case JVX_DATAFORMAT_8BIT:
			if (mxIsData(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			if (mxIsData(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			if (mxIsData(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			if (mxIsData(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;

		case JVX_DATAFORMAT_U8BIT:
			if (mxIsData(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			if (mxIsData(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			if (mxIsData(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			if (mxIsData(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;

		default:
			return JVX_ERROR_INVALID_ARGUMENT;

		}
	}
	return res;
}






jvxErrorType
CjvxPropertiesToMatlabConverter::mexGetPropertyCore(mxArray*& arr,
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

			res = mexReturnPropertyNumerical(arr, descr.format, descr.num, descString.c_str(), theTriple, offset);
			if (res != JVX_NO_ERROR)
			{
				errMessOnReturn = __FUNCTION__;
				errMessOnReturn += "Error: Component request returned error message <"; 
				errMessOnReturn += jvxErrorType_descr(res);
				errMessOnReturn += ">.";
			}
			break;
		case JVX_DATAFORMAT_SIZE:

			res = mexReturnPropertyNumericalSize(arr, descr.format, descr.num, descString.c_str(), theTriple, offset);
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

			res = this->mexReturnPropertyOthers(arr, descr.format, descr.num, descString.c_str(), theTriple, offset);			
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
CjvxPropertiesToMatlabConverter::mexSetPropertyCore(mxArray*& arrOut, 
	const mxArray* prhs[], int nrhs_off, int nrhs,
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
		(mexArgument2String(valS, prhs, nrhs_off + 1, nrhs) == JVX_NO_ERROR) &&
		(mexArgument2Index<jvxInt32>(valI, prhs, nrhs_off + 2, nrhs) == JVX_NO_ERROR))
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
				converter->mexReturnBool(arrOut, true);
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
			res = copyDataToComponentOthers(&prhs[nrhs_off], nrhs - nrhs_off, theTriple, descr.format, descr.num, descString.c_str(), offset, &callGate.access_protocol);
			if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, descString, theTriple.theProps))
			{
				converter->mexReturnBool(arrOut, true);
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
