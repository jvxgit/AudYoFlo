#ifndef _CJVXPROPERTIESTOMATLABCONVERTER_H__
#define _CJVXPROPERTIESTOMATLABCONVERTER_H__

#include "jvx.h"
#include "codeFragments/matlab_c/HjvxMex2CConverter.h"

#define PREFIX_ONE_PROPERTY "PROPERTY"
#define FLD_NAME_PROPERTY_NAME_STRING "NAME_STRING"
#define FLD_NAME_PROPERTY_DESCRIPTION_STRING "DESCRIPTION_STRING"
#define FLD_NAME_PROPERTY_CATEGORY_INT32 "CATEGORY_INT32"
#define FLD_NAME_PROPERTY_UNIQUE_ID_INT32 "UNIQUE_ID"
#define FLD_NAME_PROPERTY_FORMAT_INT32 "FORMAT_INT32"
#define FLD_NAME_PROPERTY_NUMBER_ELEMENTS_INT32 "NUMBER_ELEMENTS_INT32"
#define FLD_NAME_PROPERTY_DECODER_HINT_TYPE_INT32 "DECODER_HINT_TYPE_INT32"
#define FLD_NAME_PROPERTY_ALLOW_STATE_MASK_INT64 "ALLOW_STATE_MASK_INT64"
#define FLD_NAME_PROPERTY_ALLOW_THREADING_MASK_INT64 "ALLOW_THREADING_MASK_INT64"
#define FLD_NAME_PROPERTY_DESCRIPTOR_STRING "DESCRIPTOR_STRING"
#define FLD_NAME_PROPERTY_CONTEXT_INT32 "CONTEXT_INT32"

#define FLD_NAME_PROPERTY_SUBFIELD "SUBFIELD"
#define FLD_NAME_PROPERTY_IS_VALID_BOOL "IS_VALID_BOOL"
#define FLD_NAME_PROPERTY_ACCESS_TYPE_INT32 "ACCESS_TYPE_INT32"
#define FLD_NAME_PROPERTY_DECODER_HINT_INT32 "DECODER_HINT_TYPE_INT32"
#define FLD_NAME_PROPERTY_ACCESS_PROTOCOL_INT32 "ACCESS_PROTOCOL_INT32"
#define FLD_NAME_PROPERTY_ACCESS_FLAGS_INT32 "ACCESS_FLAGS_INT32"

class CjvxCToMatlabConverter;

class CjvxPropertiesToMatlabConverter
{
protected:
	CjvxCToMatlabConverter* converter = nullptr;

public:

	void mexReturnStructProperties(mxArray*& plhs, jvx_propertyReferenceTriple& theTriple);
	void mexReturnStructOneProperty(mxArray*& plhs, jvxPropertyCategoryType catProperty, jvxUInt64 allowStateMask, jvxUInt64 allowThreadingMask,
		jvxDataFormat format, jvxSize numElms, jvxPropertyAccessType accessType, jvxPropertyDecoderHintType decHtTp, jvxSize hdlIdx,
		jvxPropertyContext context, const std::string& name, const std::string& description, const std::string& descriptor,
		jvxBool isValid, jvxFlagTag accessFlags);

	jvxErrorType mexGetPropertyCore(mxArray*& arr, jvx_propertyReferenceTriple& theTriple, const std::string& descString, jvxSize offset, std::string& errMessOnReturn);

	jvxErrorType mexReturnPropertyNumerical(mxArray*& plhs, jvxSize hdlIdx, jvxPropertyCategoryType catTp, jvxDataFormat format, jvxSize numElms,
		jvxPropertyDecoderHintType decHtTp, jvx_propertyReferenceTriple& theTriple, jvxSize offset);
	jvxErrorType mexReturnPropertyNumericalSize(mxArray*& plhs, jvxSize hdlIdx, jvxPropertyCategoryType catTp, jvxDataFormat format, jvxSize numElms, jvxPropertyDecoderHintType decHtTp, jvx_propertyReferenceTriple& theTriple, jvxSize offset);
	jvxErrorType mexReturnPropertyNumerical(mxArray*& plhs, jvxDataFormat format, jvxSize numElms, const char* descr, jvx_propertyReferenceTriple& theTriple, jvxSize offset);
	jvxErrorType mexReturnPropertyNumericalSize(mxArray*& plhs, jvxDataFormat format, jvxSize numElms, const char* descr, jvx_propertyReferenceTriple& theTriple, jvxSize offset);

	jvxErrorType mexReturnPropertyOthers(mxArray*& plhs, jvxSize hdlIdx, jvxPropertyCategoryType catTp, jvxDataFormat format, jvxSize numElms, jvxPropertyDecoderHintType decHtTp, jvx_propertyReferenceTriple& theTriple, jvxSize offset);
	jvxErrorType mexReturnPropertyOthers(mxArray*& plhs, jvxDataFormat format, jvxSize numElms, const char* descr, jvx_propertyReferenceTriple& theTriple, jvxSize offset);


};

#endif
