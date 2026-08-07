#include "CjvxPythonToCConverter.h"
#include "localPybindIncludes.h"

namespace py = pybind11;

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
CjvxPythonToCConverter::convert_py_buf_c_buf_1_x_N(jvxHandle* data_setprops, jvxDataFormat format, jvxSize N, const pybind11::object& prhs)
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
	bool* dat_logical = NULL;

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

	if (py::isinstance<py::array>(prhs))
	{
		py::array arr = prhs.cast<py::array>();
		if (arr.dtype().is(py::dtype::of<double>()))
		{
			dat_dbl = (double*)arr.mutable_data();
		}
		else if (arr.dtype().is(py::dtype::of<float>()))
		{
			dat_flt = (float*)arr.mutable_data();
		}
		else if (arr.dtype().is(py::dtype::of<jvxInt64>()))
		{
			dat_int64 = (jvxInt64*)arr.mutable_data();
		}
		else if (arr.dtype().is(py::dtype::of<jvxInt32>()))
		{
			dat_int32 = (jvxInt32*)arr.mutable_data();
		}
		else if (arr.dtype().is(py::dtype::of<jvxInt16>()))
		{
			dat_int16 = (jvxInt16*)arr.mutable_data();
		}
		else if (arr.dtype().is(py::dtype::of<jvxInt8>()))
		{
			dat_int8 = (jvxInt8*)arr.mutable_data();
		}
		else if (arr.dtype().is(py::dtype::of<jvxUInt64>()))
		{
			dat_uint64 = (jvxUInt64*)arr.mutable_data();
		}
		else if (arr.dtype().is(py::dtype::of<jvxUInt32>()))
		{
			dat_uint32 = (jvxUInt32*)arr.mutable_data();
		}
		else if (arr.dtype().is(py::dtype::of<jvxUInt16>()))
		{
			dat_uint16 = (jvxUInt16*)arr.mutable_data();
		}
		else if (arr.dtype().is(py::dtype::of<jvxUInt8>()))
		{
			dat_uint8 = (jvxUInt8*)arr.mutable_data();
		}
		else if (arr.dtype().is(py::dtype::of<bool>()))
		{
			dat_logical = (bool*)arr.mutable_data();
		}
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
		if (dat_dbl) { JVX_CONVERSION_LOOP(failedTransfer, dat_dbl, jvx_data, N, double, (jvxData)); }
		else if (dat_flt) { JVX_CONVERSION_LOOP(failedTransfer, dat_flt, jvx_data, N, jvxData, ); }
		else if (dat_int64) { JVX_CONVERSION_LOOP(failedTransfer, dat_int64, jvx_data, N, jvxData, ); }
		else if (dat_int32) { JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_data, N, jvxData, ); }
		else if (dat_int16) { JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_data, N, jvxData, ); }
		else if (dat_int8) { JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_data, N, jvxData, ); }
		else if (dat_uint64) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint64, jvx_data, N, jvxData, ); }
		else if (dat_uint32) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_data, N, jvxData, ); }
		else if (dat_logical) { JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_data, N, jvxData, ); }
		else if (dat_uint16) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_data, N, jvxData, ); }
		else if (dat_uint8) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_data, N, jvxData, ); }
	}
	else if (jvx_sz)
	{
		if (dat_dbl) { JVX_CONVERSION_LOOP(failedTransfer, dat_dbl, jvx_sz, N, jvxData, JVX_DATA2SIZE); }
		else if (dat_flt) { JVX_CONVERSION_LOOP(failedTransfer, dat_flt, jvx_sz, N, jvxData, JVX_DATA2SIZE); }
		else if (dat_int64) { JVX_CONVERSION_LOOP(failedTransfer, dat_int64, jvx_sz, N, jvxData, JVX_DATA2SIZE); }
		else if (dat_int32) { JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_sz, N, jvxData, JVX_DATA2SIZE); }
		else if (dat_int16) { JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_sz, N, jvxData, JVX_DATA2SIZE); }
		else if (dat_int8) { JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_sz, N, jvxData, JVX_DATA2SIZE); }
		else if (dat_uint64) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint64, jvx_sz, N, jvxData, JVX_DATA2SIZE); }
		else if (dat_uint32) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_sz, N, jvxData, JVX_DATA2SIZE); }
		else if (dat_logical) { JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_sz, N, jvxSize, ); }
		else if (dat_uint16) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_sz, N, jvxData, JVX_DATA2SIZE); }
		else if (dat_uint8) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_sz, N, jvxData, JVX_DATA2SIZE); }
	}
	else if (jvx_int64)
	{
		if (dat_dbl) { JVX_CONVERSION_LOOP(failedTransfer, dat_dbl, jvx_int64, N, jvxData, JVX_DATA2INT64); }
		else if (dat_flt) { JVX_CONVERSION_LOOP(failedTransfer, dat_flt, jvx_int64, N, jvxData, JVX_DATA2INT64); }
		else if (dat_int64) { JVX_CONVERSION_LOOP(failedTransfer, dat_int64, jvx_int64, N, jvxInt64, ); }
		else if (dat_int32) { JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_int64, N, jvxData, JVX_DATA2INT64); }
		else if (dat_int16) { JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_int64, N, jvxData, JVX_DATA2INT64); }
		else if (dat_int8) { JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_int64, N, jvxData, JVX_DATA2INT64); }
		else if (dat_uint64) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint64, jvx_int64, N, jvxData, JVX_DATA2INT64); }
		else if (dat_uint32) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_int64, N, jvxData, JVX_DATA2INT64); }
		else if (dat_uint16) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_int64, N, jvxData, JVX_DATA2INT64); }
		else if (dat_logical) { JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_int64, N, jvxInt64, ); }
		else if (dat_uint8) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_int64, N, jvxData, JVX_DATA2INT64); }
	}
	else if (jvx_int32)
	{
		if (dat_dbl) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_int32, N, jvxData, JVX_DATA2INT32, 0x7FFFFFFF, -0x7FFFFFFF); }
		else if (dat_flt) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_int32, N, jvxData, JVX_DATA2INT32, 0x7FFFFFFF, -0x7FFFFFFF); }
		else if (dat_int64) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_int32, N, jvxData, JVX_DATA2INT32, 0x7FFFFFFF, -0x7FFFFFFF); }
		else if (dat_int32) { JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_int32, N, jvxInt32, ); }
		else if (dat_int16) { JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_int32, N, jvxData, JVX_DATA2INT32); }
		else if (dat_int8) { JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_int32, N, jvxData, JVX_DATA2INT32); }
		else if (dat_uint64) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_int32, N, jvxData, JVX_DATA2INT32, 0x7FFFFFFF, -0x7FFFFFFF); }
		else if (dat_uint32) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_int32, N, jvxData, JVX_DATA2INT32); }
		else if (dat_uint16) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_int32, N, jvxData, JVX_DATA2INT32); }
		else if (dat_logical) { JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_int32, N, jvxInt32, ); }
		else if (dat_uint8) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_int32, N, jvxData, JVX_DATA2INT32); }
	}
	else if (jvx_int16)
	{
		if (dat_dbl) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF); }
		else if (dat_flt) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF); }
		else if (dat_int64) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF); }
		else if (dat_int32) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF); }
		else if (dat_int16) { JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_int16, N, jvxInt16, ); }
		else if (dat_int8) { JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_int16, N, jvxData, JVX_DATA2INT16); }
		else if (dat_uint64) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF); }
		else if (dat_uint32) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint32, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF); }
		else if (dat_uint16) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint16, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF); }
		else if (dat_logical) { JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_int16, N, jvxInt16, ); }
		else if (dat_uint8) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_int16, N, jvxData, JVX_DATA2INT16); }
	}
	else if (jvx_int8)
	{
		if (dat_dbl) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F); }
		else if (dat_flt) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F); }
		else if (dat_int64) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F); }
		else if (dat_int32) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F); }
		else if (dat_int16) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int16, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F); }
		else if (dat_int8) { JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_int8, N, jvxInt8, ); }
		else if (dat_uint64) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F); }
		else if (dat_uint32) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint32, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F); }
		else if (dat_logical) { JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_int8, N, jvxInt8, ); }
		else if (dat_uint16) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint16, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F); }
		else if (dat_uint8) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint8, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F); }
	}
	else if (jvx_uint64)
	{
		if (dat_dbl) { JVX_CONVERSION_LOOP(failedTransfer, dat_dbl, jvx_uint64, N, jvxData, JVX_DATA2UINT64); }
		else if (dat_flt) { JVX_CONVERSION_LOOP(failedTransfer, dat_flt, jvx_uint64, N, jvxData, JVX_DATA2UINT64); }
		else if (dat_int64) { JVX_CONVERSION_LOOP(failedTransfer, dat_int64, jvx_uint64, N, jvxData, JVX_DATA2UINT64); }
		else if (dat_int32) { JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_uint64, N, jvxData, JVX_DATA2UINT64); }
		else if (dat_int16) { JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_uint64, N, jvxData, JVX_DATA2UINT64); }
		else if (dat_int8) { JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_uint64, N, jvxData, JVX_DATA2UINT64); }
		else if (dat_uint64) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint64, jvx_uint64, N, jvxUInt64, ); }
		else if (dat_uint32) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_uint64, N, jvxData, JVX_DATA2UINT64); }
		else if (dat_uint16) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_uint64, N, jvxData, JVX_DATA2UINT64); }
		else if (dat_logical) { JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_uint64, N, jvxUInt64, ); }
		else if (dat_uint8) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint64, N, jvxData, JVX_DATA2UINT64); }
	}
	else if (jvx_uint32)
	{
		if (dat_dbl) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0); }
		else if (dat_flt) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0); }
		else if (dat_int64) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0); }
		else if (dat_int32) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0); }
		else if (dat_int16) { JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_uint32, N, jvxData, JVX_DATA2UINT32); }
		else if (dat_int8) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int8, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0); }
		else if (dat_uint64) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0); }
		else if (dat_uint32) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_uint32, N, jvxUInt32, ); }
		else if (dat_uint16) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_uint32, N, jvxData, JVX_DATA2UINT32); }
		else if (dat_logical) { JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_uint32, N, jvxUInt32, ); }
		else if (dat_uint8) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint32, N, jvxData, JVX_DATA2UINT32); }
	}
	else if (jvx_uint16)
	{
		if (dat_dbl) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0); }
		else if (dat_flt) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0); }
		else if (dat_int64) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0); }
		else if (dat_int32) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0); }
		else if (dat_int16) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int16, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0); }
		else if (dat_int8) { JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_uint16, N, jvxData, JVX_DATA2UINT16); }
		else if (dat_uint64) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0); }
		else if (dat_uint32) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint32, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0); }
		else if (dat_logical) { JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_uint16, N, jvxUInt16, ); }
		else if (dat_uint16) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_uint16, N, jvxUInt16, ); }
		else if (dat_uint8) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint16, N, jvxData, JVX_DATA2UINT16); }
	}
	else if (jvx_uint8)
	{
		if (dat_dbl) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0); }
		else if (dat_flt) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0); }
		else if (dat_int64) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0); }
		else if (dat_int32) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0); }
		else if (dat_int16) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int16, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0); }
		else if (dat_int8) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int8, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0); }
		else if (dat_uint64) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0); }
		else if (dat_uint32) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint32, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0); }
		else if (dat_logical) { JVX_CONVERSION_LOOP(failedTransfer, dat_logical, jvx_uint8, N, jvxUInt8, ); }
		else if (dat_uint16) { JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint16, jvx_uint8, N, jvxData, JVX_DATA2INT8, 0xFF, 0); }
		else if (dat_uint8) { JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint8, N, jvxUInt8, ); }
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPythonToCConverter::pyArgument2Bool(bool& value, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (idx < numEntries)
	{
		if (!thePointer[idx].is_none())
		{
			if (py::isinstance<py::bool_>(thePointer[idx]))
			{
				value = thePointer[idx].cast<bool>();
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
CjvxPythonToCConverter::pyArgument2String(std::string& value, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;

	value = "";

	if (idx < numEntries)
	{
		if (!thePointer[idx].is_none())
		{
			if (py::isinstance<py::str>(thePointer[idx]))
			{
				value = CjvxPythonToCConverter::jvx_py_2_cstring(thePointer[idx]);
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
CjvxPythonToCConverter::pyArgument2ComponentIdentification(jvxComponentIdentification& value, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string nm;

	value.reset();

	res = pyArgument2String(nm, thePointer, idx, numEntries);
	if (res == JVX_NO_ERROR)
	{
		res = jvxComponentIdentification_decode(value, nm);
	}
	else
	{
		res = pyArgument2Type<jvxComponentType>(value.tp, thePointer, idx, numEntries, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
	}
	return(res);
}

jvxErrorType
CjvxPythonToCConverter::pyArgument2String(std::vector<std::string>& valueList, jvxSize expectedNumberEntries, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;
	valueList.clear();
	if (idx < numEntries)
	{
		if (!thePointer[idx].is_none())
		{
			if (py::isinstance<py::list>(thePointer[idx]) || py::isinstance<py::tuple>(thePointer[idx]))
			{
				py::sequence seq = thePointer[idx].cast<py::sequence>();
				jvxSize numEntriesLocal = (jvxSize)seq.size();
				if (expectedNumberEntries == numEntriesLocal)
				{
					for (jvxSize i = 0; i < numEntriesLocal; i++)
					{
						py::object item = seq[i];
						if (py::isinstance<py::str>(item))
						{
							valueList.push_back(CjvxPythonToCConverter::jvx_py_2_cstring(item));
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
CjvxPythonToCConverter::pyArgument2StringList(std::vector<std::string>& valueList, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries)
{
	std::string oneToken;
	jvxErrorType res = JVX_NO_ERROR;
	valueList.clear();
	if (idx < numEntries)
	{
		if (!thePointer[idx].is_none())
		{
			if (py::isinstance<py::list>(thePointer[idx]) || py::isinstance<py::tuple>(thePointer[idx]))
			{
				py::sequence seq = thePointer[idx].cast<py::sequence>();
				for (jvxSize i = 0; i < (jvxSize)seq.size(); i++)
				{
					py::object item = seq[i];
					if (py::isinstance<py::str>(item))
					{
						valueList.push_back(CjvxPythonToCConverter::jvx_py_2_cstring(item));
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
				}
			}
			else if (py::isinstance<py::str>(thePointer[idx]))
			{
				// There may also be just one string
				oneToken = CjvxPythonToCConverter::jvx_py_2_cstring(thePointer[idx]);
				valueList.push_back(oneToken);
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		// None -> no entry, mirrors mxIsEmpty(...) case in the original
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return(res);
}

jvxErrorType
CjvxPythonToCConverter::pyArgument2Data(jvxData& value, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;
	value = 0.0;

	if (idx < numEntries)
	{
		if (!thePointer[idx].is_none())
		{
			try
			{
				value = (jvxData)thePointer[idx].cast<double>();
			}
			catch (const py::cast_error&)
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
CjvxPythonToCConverter::jvx_py_2_cstring(const pybind11::object& phs)
{
	return phs.cast<std::string>();
}

intptr_t
CjvxPythonToCConverter::jvx_py_2_intptr(const pybind11::object& phs)
{
	intptr_t retVal = 0;
	try
	{
		retVal = (intptr_t)phs.cast<uintptr_t>();
	}
	catch (const py::cast_error&)
	{
		retVal = 0;
	}
	return retVal;
}

std::vector<jvxValue>
CjvxPythonToCConverter::jvx_py_2_numeric(const pybind11::object& phs, jvxSize lineNo)
{
	std::vector<jvxValue> lstOnReturn;

	if (!py::isinstance<py::array>(phs))
	{
		return lstOnReturn;
	}

	py::array arr = phs.cast<py::array>();
	jvxSize M = (arr.ndim() == 2) ? (jvxSize)arr.shape(0) : 1;
	jvxSize N = (arr.ndim() == 2) ? (jvxSize)arr.shape(1) : (jvxSize)arr.size();
	jvxSize i = 0;
	lstOnReturn.resize(N);

	if (arr.dtype().is(py::dtype::of<double>()))
	{
		if (arr.ndim() == 2)
		{
			auto buf2 = arr.cast<py::array_t<double>>().unchecked<2>();
			for (i = 0; i < N; i++) lstOnReturn[i].assign(buf2(lineNo, i));
		}
		else
		{
			auto buf1 = arr.cast<py::array_t<double>>().unchecked<1>();
			for (i = 0; i < N; i++) lstOnReturn[i].assign(buf1(i));
		}
	}
	else if (arr.dtype().is(py::dtype::of<float>()))
	{
		if (arr.ndim() == 2) { auto b = arr.cast<py::array_t<float>>().unchecked<2>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(lineNo, i)); }
		else { auto b = arr.cast<py::array_t<float>>().unchecked<1>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(i)); }
	}
	else if (arr.dtype().is(py::dtype::of<jvxInt64>()))
	{
		if (arr.ndim() == 2) { auto b = arr.cast<py::array_t<jvxInt64>>().unchecked<2>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(lineNo, i)); }
		else { auto b = arr.cast<py::array_t<jvxInt64>>().unchecked<1>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(i)); }
	}
	else if (arr.dtype().is(py::dtype::of<jvxInt32>()))
	{
		if (arr.ndim() == 2) { auto b = arr.cast<py::array_t<jvxInt32>>().unchecked<2>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(lineNo, i)); }
		else { auto b = arr.cast<py::array_t<jvxInt32>>().unchecked<1>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(i)); }
	}
	else if (arr.dtype().is(py::dtype::of<jvxInt16>()))
	{
		if (arr.ndim() == 2) { auto b = arr.cast<py::array_t<jvxInt16>>().unchecked<2>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(lineNo, i)); }
		else { auto b = arr.cast<py::array_t<jvxInt16>>().unchecked<1>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(i)); }
	}
	else if (arr.dtype().is(py::dtype::of<jvxInt8>()))
	{
		if (arr.ndim() == 2) { auto b = arr.cast<py::array_t<jvxInt8>>().unchecked<2>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(lineNo, i)); }
		else { auto b = arr.cast<py::array_t<jvxInt8>>().unchecked<1>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(i)); }
	}
	else if (arr.dtype().is(py::dtype::of<jvxUInt64>()))
	{
		if (arr.ndim() == 2) { auto b = arr.cast<py::array_t<jvxUInt64>>().unchecked<2>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(lineNo, i)); }
		else { auto b = arr.cast<py::array_t<jvxUInt64>>().unchecked<1>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(i)); }
	}
	else if (arr.dtype().is(py::dtype::of<jvxUInt32>()))
	{
		if (arr.ndim() == 2) { auto b = arr.cast<py::array_t<jvxUInt32>>().unchecked<2>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(lineNo, i)); }
		else { auto b = arr.cast<py::array_t<jvxUInt32>>().unchecked<1>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(i)); }
	}
	else if (arr.dtype().is(py::dtype::of<jvxUInt16>()))
	{
		if (arr.ndim() == 2) { auto b = arr.cast<py::array_t<jvxUInt16>>().unchecked<2>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(lineNo, i)); }
		else { auto b = arr.cast<py::array_t<jvxUInt16>>().unchecked<1>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(i)); }
	}
	else if (arr.dtype().is(py::dtype::of<jvxUInt8>()))
	{
		if (arr.ndim() == 2) { auto b = arr.cast<py::array_t<jvxUInt8>>().unchecked<2>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(lineNo, i)); }
		else { auto b = arr.cast<py::array_t<jvxUInt8>>().unchecked<1>(); for (i = 0; i < N; i++) lstOnReturn[i].assign(b(i)); }
	}

	return lstOnReturn;
}

pybind11::object
CjvxPythonToCConverter::jvx_py_lookup_dictfield_core(const pybind11::object& strEntry, const std::string& key, jvxSize curLevel, int levelMax, jvxBool& moreLevels)
{
	if (!py::isinstance<py::dict>(strEntry))
	{
		return pybind11::none();
	}
	py::dict d = strEntry.cast<py::dict>();

	if (d.contains(key.c_str()))
	{
		return d[key.c_str()];
	}

	moreLevels = false;
	for (auto item : d)
	{
		py::object localFld = py::reinterpret_borrow<py::object>(item.second);
		if (py::isinstance<py::dict>(localFld))
		{
			jvxBool moreLevelsLoc = true;
			if (curLevel < (jvxSize)levelMax)
			{
				pybind11::object retVal = jvx_py_lookup_dictfield_core(localFld, key, curLevel + 1, levelMax, moreLevelsLoc);
				if (!retVal.is_none())
				{
					return retVal;
				}
			}
			if (moreLevelsLoc)
			{
				moreLevels = true;
			}
		}
	}
	return pybind11::none();
}

pybind11::object
CjvxPythonToCConverter::jvx_py_lookup_dictfield(const pybind11::object& strEntry, const std::string& key, jvxSize levelMax)
{
	pybind11::object retVal = pybind11::none();
	jvxBool moreLevels = false;
	jvxSize levelMaxCur = 0;
	do
	{
		retVal = jvx_py_lookup_dictfield_core(strEntry, key, 0, (int)levelMax, moreLevels);
		if (!retVal.is_none())
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

pybind11::object
CjvxPythonToCConverter::jvx_py_read_single_reference(const pybind11::object& fld, const std::string& expr)
{
	jvxSize i;
	std::vector<std::string> tokens;
	if (jvx::helper::parseStringListIntoTokens(expr, tokens, '/') == JVX_NO_ERROR)
	{
		pybind11::object curPtr = fld;
		for (i = 0; i < tokens.size(); i++)
		{
			jvxBool moreLevel = false;
			curPtr = jvx_py_lookup_dictfield_core(curPtr, tokens[i], 0, 0, moreLevel);
			if (curPtr.is_none())
			{
				return pybind11::none();
			}
			if (i < tokens.size() - 1)
			{
				if (!py::isinstance<py::dict>(curPtr))
				{
					return pybind11::none();
				}
			}
			else
			{
				return curPtr;
			}
		}
	}
	return pybind11::none();
}
