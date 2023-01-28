#include "jvx-helpers.h"

#ifdef __cplusplus

jvxErrorType JVX_OUTPUT_LINE_ASSIGN_SINGLE_VALUE_CONVERT(std::string& out, const std::string& varName, jvxSize idxEnum, const std::string& cvt)
{
	out = varName;
	out += " = ";
	out += "(";
	out += cvt;
	out += ")";
	out += jvx_size2String(idxEnum);
	out += ";";
	return JVX_NO_ERROR;
}

jvxErrorType JVX_OUTPUT_LINE_ASSIGN_SINGLE_VALUE_IDX(std::string& out, const std::string& varName, jvxHandle* fld, jvxDataFormat form, jvxSize numDigits, jvxSize idx)
{
	jvxData* ptrDat = (jvxData*)fld;
	jvxInt32* ptrI32 = (jvxInt32*)fld;
	jvxInt16* ptrI16 = (jvxInt16*)fld;
	jvxUInt16* ptrUI16 = (jvxUInt16*)fld;
	jvxInt8* ptrI8 = (jvxInt8*)fld;
	jvxInt64* ptrI64 = (jvxInt64*)fld;
	jvxSize* ptrSz = (jvxSize*)fld;

	switch (form)
	{
	case JVX_DATAFORMAT_DATA:
	case JVX_DATAFORMAT_32BIT_LE:
	case JVX_DATAFORMAT_64BIT_LE:
	case JVX_DATAFORMAT_16BIT_LE:
	case JVX_DATAFORMAT_U16BIT_LE:
	case JVX_DATAFORMAT_8BIT:
	case JVX_DATAFORMAT_SIZE:
		break;
	default:
		return JVX_ERROR_UNSUPPORTED;
	}

	out = varName;
	out += "[";
	out += jvx_size2String(idx);
	out += "]";
	out += " = ";
	switch (form)
	{
	case JVX_DATAFORMAT_DATA:
		out += jvx_data2String_highPrecision(ptrDat[idx], numDigits);
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		out += jvx_int2String(ptrI32[idx]);
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		out += jvx_int642String(ptrI64[idx]);
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		out += jvx_int2String(ptrI16[idx]);
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		out += jvx_uint2String(ptrUI16[idx]);
		break;
	case JVX_DATAFORMAT_8BIT:
		out += jvx_int2String(ptrI8[idx]);
		break;
	case JVX_DATAFORMAT_SIZE:
		out += jvx_size2String(ptrSz[idx]);
		break;
	}
	out += ";";
	return JVX_NO_ERROR;
}

jvxErrorType JVX_OUTPUT_LINE_ASSIGN_SINGLE_VALUE(std::string& out, const std::string& varName, jvxHandle* fld, jvxDataFormat form, jvxSize numDigits)
{
	jvxData* ptrDat = (jvxData*)fld;
	jvxInt32* ptrI32 = (jvxInt32*)fld;
	jvxInt16* ptrI16 = (jvxInt16*)fld;
	jvxUInt16* ptrUI16 = (jvxUInt16*)fld;
	jvxInt8* ptrI8 = (jvxInt8*)fld;
	jvxInt64* ptrI64 = (jvxInt64*)fld;
	jvxSize* ptrSz = (jvxSize*)fld;

	switch (form)
	{
	case JVX_DATAFORMAT_DATA:
	case JVX_DATAFORMAT_32BIT_LE:
	case JVX_DATAFORMAT_64BIT_LE:
	case JVX_DATAFORMAT_16BIT_LE:
	case JVX_DATAFORMAT_U16BIT_LE:
	case JVX_DATAFORMAT_8BIT:
	case JVX_DATAFORMAT_SIZE:
		break;
	default:
		return JVX_ERROR_UNSUPPORTED;
	}

	out = varName;
	out += " = ";
	switch (form)
	{
	case JVX_DATAFORMAT_DATA:
		out += jvx_data2String_highPrecision(ptrDat[0], numDigits);
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		out += jvx_int2String(ptrI32[0]);
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		out += jvx_int642String(ptrI64[0]);
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		out += jvx_int2String(ptrI16[0]);
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		out += jvx_uint2String(ptrUI16[0]);
		break;
	case JVX_DATAFORMAT_8BIT:
		out += jvx_int2String(ptrI8[0]);
		break;
	case JVX_DATAFORMAT_SIZE:
		out += jvx_size2String(ptrSz[0]);
		break;
	}
	out += ";";
	return JVX_NO_ERROR;
}

jvxErrorType JVX_OUTPUT_LINE_ASSIGN_FIELD(std::string& out, const std::string& varName, jvxHandle* fld, jvxDataFormat form, jvxSize dimY, jvxSize numTabs, jvxSize numDigits)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData* ptrDat = (jvxData*)fld;
	jvxInt32* ptrI32 = (jvxInt32*)fld;
	jvxInt16* ptrI16 = (jvxInt16*)fld;
	jvxInt8* ptrI8 = (jvxInt8*)fld;
	jvxInt64* ptrI64 = (jvxInt64*)fld;
	jvxSize* ptrSz = (jvxSize*)fld;

	switch (form)
	{
	case JVX_DATAFORMAT_DATA:
	case JVX_DATAFORMAT_32BIT_LE:
	case JVX_DATAFORMAT_64BIT_LE:
	case JVX_DATAFORMAT_16BIT_LE:
	case JVX_DATAFORMAT_8BIT:
	case JVX_DATAFORMAT_SIZE:
		break;
	default:
		return JVX_ERROR_UNSUPPORTED;
	}

	for (i = 0; i < dimY; i++)
	{
		out += varName;
		out += "[";
		out += jvx_size2String(i);
		out += "] = ";
		switch (form)
		{
		case JVX_DATAFORMAT_DATA:
			out += jvx_data2String_highPrecision(ptrDat[i], numDigits);
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			out += jvx_int2String( ptrI32[i]);
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			out += jvx_int642String(ptrI64[i]);
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			out += jvx_int2String(ptrI16[i]);
			break;
		case JVX_DATAFORMAT_8BIT:
			out += jvx_int2String(ptrI8[i]);
			break;
		case JVX_DATAFORMAT_SIZE:
			out += jvx_size2String(ptrSz[i]);
			break;
		}
		out += ";";
	}
	return JVX_NO_ERROR;
}

jvxErrorType JVX_OUTPUT_LINE_ASSIGN_ENUM(std::string& out, const std::string& varName, jvxSize idxOption, jvxTextHelpers* helpers, jvxSize numHelpersMax)
{
	if (idxOption < numHelpersMax)
	{
		out += varName;
		out += " = ";
		out += helpers[idxOption].full;
		out += ";";
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}
#endif
