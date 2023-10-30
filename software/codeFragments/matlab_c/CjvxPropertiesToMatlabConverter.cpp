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
#include "codeFragments/matlab_c/CjvxCToMatlabConverter.h"

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



jvxErrorType
CjvxPropertiesToMatlabConverter::mexGetPropertyCore(mxArray*& arr,
	jvx_propertyReferenceTriple& theTriple, const std::string& descString,
	jvxSize offset, std::string& errMessOnReturn)
{
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("CjvxPropertiesToMatlabConverter::mexGetPropertyCore");
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
				errMessOnReturn = internalErrorMessage + "Component request returned error message <" + jvxErrorType_descr(res) + ">.";
			}
			break;
		case JVX_DATAFORMAT_SIZE:

			res = mexReturnPropertyNumericalSize(arr, descr.format, descr.num, descString.c_str(), theTriple, offset);
			if (res != JVX_NO_ERROR)
			{
				errMessOnReturn = internalErrorMessage + "Component request returned error message <" + jvxErrorType_descr(res) + ">.";
			}
			break;
		case JVX_DATAFORMAT_SELECTION_LIST:
		case JVX_DATAFORMAT_STRING:
		case JVX_DATAFORMAT_STRING_LIST:
		case JVX_DATAFORMAT_VALUE_IN_RANGE:

			res = this->mexReturnPropertyOthers(arr, descr.format, descr.num, descString.c_str(), theTriple, offset);			
			if (res != JVX_NO_ERROR)
			{
				errMessOnReturn = internalErrorMessage + "Component request returned error message <" + jvxErrorType_descr(res) + ">.";
			}
			break;
		default:
			errMessOnReturn = "Request for unsupported property type.";
			res = JVX_ERROR_UNSUPPORTED;
		}
	}
	else
	{
		errMessOnReturn = internalErrorMessage + "Did not find property with specified descriptor tag.";
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return res;
}