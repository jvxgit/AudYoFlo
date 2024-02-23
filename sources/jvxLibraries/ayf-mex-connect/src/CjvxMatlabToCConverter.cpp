#include "CjvxMatlabToCConverter.h"
#include "localMexIncludes.h"

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
CjvxMatlabToCConverter::convert_mat_buf_c_buf_1_x_N(jvxHandle* data_setprops, jvxDataFormat format, jvxSize N, const mxArray* prhs)
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
	mxLogical* dat_logical = NULL;

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
	else if (mxIsLogical(prhs))
	{		
		dat_logical = (mxLogical*)mxGetData(prhs);
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
		else if (dat_logical)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_data, N, jvxData, );
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
		else if (dat_logical)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_sz, N, jvxSize, );
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
		else if (dat_logical)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_int64, N, jvxInt64, );
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
		else if (dat_logical)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_int32, N, jvxInt32, );
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
		else if (dat_logical)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_int16, N, jvxInt16, );
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
		else if (dat_logical)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_int8, N, jvxInt8, );
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
		else if (dat_logical)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_uint64, N, jvxUInt64, );
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
		else if (dat_logical)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_uint32, N, jvxUInt32, );
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint32, N, jvxData, JVX_DATA2UINT32);
		}
	}
	else if (jvx_uint16)
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
		else if (dat_logical)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_uint16, N, jvxUInt16, );
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
		else if (dat_logical)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_uint8, N, jvxUInt8, );
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
CjvxMatlabToCConverter::mexArgument2Bool(bool& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (idx < numEntries)
	{
		if (thePointer[idx])
		{
			if (mxIsLogical(thePointer[idx]))
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

jvxErrorType 
CjvxMatlabToCConverter::mexArgument2String(std::string& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;

	value = "";

	if (idx < numEntries)
	{
		if (thePointer[idx])
		{
			if (mxIsChar(thePointer[idx]))
			{
				value = CjvxMatlabToCConverter::jvx_mex_2_cstring(thePointer[idx]);
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

jvxErrorType 
CjvxMatlabToCConverter::mexArgument2ComponentIdentification(jvxComponentIdentification& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
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

jvxErrorType 
CjvxMatlabToCConverter::mexArgument2String(std::vector<std::string>& valueList, jvxSize expectedNumberEntries, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	const mxArray* arr = NULL;
	valueList.clear();
	if (idx < numEntries)
	{
		if (thePointer[idx])
		{
			if (mxIsCell(thePointer[idx]))
			{
				jvxSize numEntries = mxGetNumberOfElements(thePointer[idx]);
				if (expectedNumberEntries == numEntries)
				{
					for (i = 0; i < numEntries; i++)
					{
						arr = mxGetCell(thePointer[idx], JVX_SIZE_2_MAT_IDX(i));
						if (arr)
						{
							if (mxIsChar(arr))
							{
								valueList.push_back(CjvxMatlabToCConverter::jvx_mex_2_cstring(arr));
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

jvxErrorType 
CjvxMatlabToCConverter::mexArgument2StringList(std::vector<std::string>& valueList, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxSize i;
	std::string oneToken;
	jvxErrorType res = JVX_NO_ERROR;
	const mxArray* arr = NULL;
	valueList.clear();
	if (idx < numEntries)
	{
		if (thePointer[idx])
		{
			if (mxIsCell(thePointer[idx]))
			{
				jvxSize numEntries = mxGetNumberOfElements(thePointer[idx]);
				for (i = 0; i < numEntries; i++)
				{
					arr = mxGetCell(thePointer[idx], JVX_SIZE_2_MAT_IDX(i));
					if (arr)
					{
						if (mxIsChar(arr))
						{
							valueList.push_back(CjvxMatlabToCConverter::jvx_mex_2_cstring(arr));
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
			else if (mxIsChar(thePointer[idx]))
			{
				// There may also be just one string
				oneToken = CjvxMatlabToCConverter::jvx_mex_2_cstring(thePointer[idx]);
				valueList.push_back(oneToken);
			}
			else if (mxIsEmpty(thePointer[idx]))
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

jvxErrorType 
CjvxMatlabToCConverter::mexArgument2Data(jvxData& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
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

std::string 
CjvxMatlabToCConverter::jvx_mex_2_cstring(const mxArray* phs)
{
	jvxSize bufLen = mxGetM(phs)*mxGetN(phs)*sizeof(char)+1;
	char* buf = new char[bufLen];
	mxGetString(phs, buf, JVX_SIZE_INT(bufLen));
	std::string str = buf;
	delete[](buf);
	return(str);
}

std::vector<jvxValue> 
CjvxMatlabToCConverter::jvx_mex_2_numeric(const mxArray* phs, jvxSize lineNo)
{
	std::vector<jvxValue> lstOnReturn;
	if (mxIsNumeric(phs))
	{
		jvxSize M = mxGetM(phs);
		jvxSize N = mxGetN(phs);
		jvxSize i = 0;
		lstOnReturn.resize(N);

		if (mxIsDouble(phs))
		{
			double* ptr = (double*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsSingle(phs))
		{
			float* ptr = (float*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsInt64(phs))
		{
			jvxInt64* ptr = (jvxInt64*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsInt32(phs))
		{
			jvxInt32* ptr = (jvxInt32*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsInt16(phs))
		{
			jvxInt16* ptr = (jvxInt16*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsUint64(phs))
		{
			jvxUInt64* ptr = (jvxUInt64*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsUint32(phs))
		{
			jvxUInt32* ptr = (jvxUInt32*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsUint16(phs))
		{
			jvxUInt16* ptr = (jvxUInt16*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsInt8(phs))
		{
			jvxInt8* ptr = (jvxInt8*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsUint8(phs))
		{
			jvxUInt8* ptr = (jvxUInt8*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else
		{
		}
	}
	return lstOnReturn;
}

const mxArray* 
CjvxMatlabToCConverter::jvx_mex_lookup_strfield_core(const mxArray* strEntry, const std::string& key, jvxSize curLevel, int levelMax, jvxBool& moreLevels)
{
	jvxSize i;
	const mxArray* retVal = nullptr;
	jvxBool foundThisLevel = false;
	int num = mxGetNumberOfFields(strEntry);
	for (i = 0; i < num; i++)
	{
		const char* nm = mxGetFieldNameByNumber(strEntry, i);
		if ((std::string)nm == key)
		{
			retVal = mxGetField(strEntry, 0, key.c_str());
			foundThisLevel = true;
			break;
		}
	}

	if (!foundThisLevel)
	{
		moreLevels = false;
		for (i = 0; i < num; i++)
		{
			const mxArray* localFld = mxGetFieldByNumber(strEntry, 0, i);
			if (localFld)
			{
				if (mxIsStruct(localFld))
				{
					jvxBool moreLevelsLoc = true;
					if (curLevel < levelMax)
					{
						retVal = jvx_mex_lookup_strfield_core(localFld, key, curLevel + 1, levelMax, moreLevelsLoc);
						if (retVal != nullptr)
						{
							break;
						}
					}
					if (moreLevelsLoc)
					{
						moreLevels = true;
					}
				}
			}
		}
	}
	return retVal;
}

const mxArray* 
CjvxMatlabToCConverter::jvx_mex_lookup_strfield(const mxArray* strEntry, const std::string& key, jvxSize levelMax)
{
	const mxArray* retVal = nullptr;
	jvxBool moreLevels = false;
	jvxSize levelMaxCur = 0;
	do
	{
		retVal = jvx_mex_lookup_strfield_core(strEntry, key, 0, levelMax, moreLevels);
		if (retVal != nullptr)
		{
			break;
		}
		if (!moreLevels)
		{
			break;
		}
		levelMaxCur++;

	} while (levelMaxCur < levelMax);
	return retVal;
}

const mxArray* 
CjvxMatlabToCConverter::jvx_mex_read_single_reference(const mxArray* fld, const std::string& expr)
{
	jvxSize i;
	std::vector<std::string> tokens;
	if (jvx::helper::parseStringListIntoTokens(expr, tokens, '/') == JVX_NO_ERROR)
	{
		const mxArray* curPtr = fld;
		for (i = 0; i < tokens.size(); i++)
		{
			jvxBool moreLevel = false;
			curPtr = jvx_mex_lookup_strfield_core(curPtr, tokens[i], 0, 0, moreLevel);
			if (curPtr == nullptr)
			{
				return nullptr;
			}
			if (i < tokens.size() - 1)
			{
				if (!mxIsStruct(curPtr))
				{
					return nullptr;
				}
			}
			else
			{
				return curPtr;
			}
		}
	}
	return nullptr;
}
