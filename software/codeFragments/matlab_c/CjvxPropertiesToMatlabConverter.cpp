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