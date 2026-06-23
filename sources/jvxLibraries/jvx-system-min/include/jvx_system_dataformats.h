#ifndef __JVX_DATAFORMATS_H__
#define __JVX_DATAFORMATS_H__

#include <string.h>
#include <assert.h>
#include <stdio.h>

#define JVX_DATA_EPS_DOUBLE DBL_EPSILON
#define JVX_DATA_EPS_FLOAT FLT_EPSILON

#define JVX_FLOAT_DATAFORMAT_DOUBLE_SHIFT 0
#define JVX_FLOAT_DATAFORMAT_SINGLE_SHIFT 1

#ifdef JVX_DATA_FORMAT_DOUBLE

typedef double jvxData;
#define JVX_DBL_2_DATA(a) (a)
#define JVX_DATA_MAX_POS DBL_MAX
#define JVX_DATA_MAX_NEG -DBL_MAX
#define JVX_DATA_EPS DBL_EPSILON
#define JVX_DATA_MIN DBL_MIN
#define JVX_DATA_MIN_UNDERFLOW_PREVENT DBL_EPSILON
#define JVX_DATA_DESCRIPTOR "double"
#define JVX_DATA_DESCRIPTOR_THE_OTHER "single"
#define toData toDouble
#define JVX_DATA_MEX_CONVERT mxDOUBLE_CLASS
#define mxIsData mxIsDouble
#define mxDATA_CLASS mxDOUBLE_CLASS
#define sf_writef_data sf_writef_double
#define sf_readf_data sf_readf_double
#define JVX_FLOAT_DATAFORMAT_ID 1
#define JVX_FLOAT_DATAFORMAT_ID_OTHER 0
#define JVX_MAX_NUM_DIGITS_INT_IN_FLOAT 53
#define JVX_FABS fabs
#define JVX_LOG10 log10
#define JVX_C_STR2DATA strtod
#define JVX_FLOAT_DATAFORMAT_SHIFT JVX_FLOAT_DATAFORMAT_DOUBLE_SHIFT
#define JVX_DATA_DATAFORMAT_ENUM JVX_DATA_TYPE_SPEC_DOUBLE

#else // JVX_DATA_FORMAT_DOUBLE

#ifdef JVX_DATA_FORMAT_FLOAT

typedef float jvxData;
#define JVX_DBL_2_DATA(a) (float)(a)
#define JVX_DATA_MAX_POS FLT_MAX
#define JVX_DATA_MAX_NEG -FLT_MAX
#define JVX_DATA_EPS FLT_EPSILON
#define JVX_DATA_MIN FLT_MIN
#define JVX_DATA_MIN_UNDERFLOW_PREVENT FLT_EPSILON
#define JVX_DATA_DESCRIPTOR "single"
#define JVX_DATA_DESCRIPTOR_THE_OTHER "double"
#define toData toFloat
#define JVX_DATA_MEX_CONVERT mxSINGLE_CLASS
#define mxIsData mxIsSingle
#define mxDATA_CLASS mxSINGLE_CLASS
#define sf_writef_data sf_writef_float
#define sf_readf_data sf_readf_float
#define JVX_FLOAT_DATAFORMAT_ID 0
#define JVX_FLOAT_DATAFORMAT_ID_OTHER 1
#define JVX_MAX_NUM_DIGITS_INT_IN_FLOAT 23
#define JVX_FABS fabsf
#define JVX_LOG10 log10f
#define JVX_C_STR2DATA strtof
#define JVX_FLOAT_DATAFORMAT_SHIFT JVX_FLOAT_DATAFORMAT_SINGLE_SHIFT
#define JVX_DATA_DATAFORMAT_ENUM JVX_DATA_TYPE_SPEC_FLOAT

#else // JVX_DATA_FORMAT_FLOAT

#error("unkown arithmetic datatype")

#endif // JVX_DSP_DATA_FORMAT_FLOAT
#endif // JVX_DSP_DATA_FORMAT_DOUBLE

#if defined(JVX_SYS_EMSCRIPTEN_CLANG_32BIT) || defined(JVX_SYS_ANDROID_CLANG_32BIT)
#define JVX_SIZE_OVERFLOW 0x7FFFFFFF
#else
#define JVX_SIZE_OVERFLOW 0x7FFFFFFFFFFFFFFF
#endif

typedef jvxUInt64 jvxCBitField64;
typedef jvxUInt32 jvxCBitField32;
typedef jvxUInt16 jvxCBitField16;
typedef jvxSize jvxUniqueId;
#define JVX_UNIQUE_ID_UNSPECIFIED JVX_SIZE_UNSELECTED

typedef enum
{
        JVX_DATAFORMAT_NONE = 0,                                        // Undefined (invalid)
        JVX_DATAFORMAT_DATA,                                        // double or float datatype
        JVX_DATAFORMAT_16BIT_LE,                                    // 16 Bit signed
        JVX_DATAFORMAT_32BIT_LE,                                    // 32 Bit signed
        JVX_DATAFORMAT_64BIT_LE,                                    // 64 Bit signed
        JVX_DATAFORMAT_8BIT,                                                // 8 Bit
		JVX_DATAFORMAT_SIZE,

        JVX_DATAFORMAT_STRING,                                              // String type (character buffers)
        JVX_DATAFORMAT_STRING_LIST,                                         // String type (character buffers)
        JVX_DATAFORMAT_SELECTION_LIST,
        JVX_DATAFORMAT_VALUE_IN_RANGE,
		
		JVX_DATAFORMAT_U16BIT_LE,                                   // 16 Bit unsigned
		JVX_DATAFORMAT_U32BIT_LE,                                   // 32 Bit unsigned
		JVX_DATAFORMAT_U64BIT_LE,                                   // 64 Bit unsigned
		JVX_DATAFORMAT_U8BIT,										// 8 Bit unsigned
		
		JVX_DATAFORMAT_FLOAT,										// explicit float for external access
		JVX_DATAFORMAT_DOUBLE,										// explicit double for external access
		JVX_DATAFORMAT_BYTE, // Void: Gateway for other datatypes

		/*
		* The external buffer datatype has a valid value type to identify
		* if the associated dataformat is correct.
		* The other "installable" formats are of a dedicated type
		JVX_DATAFORMAT_EXTERNAL_BUFFER,
		*/
		JVX_DATAFORMAT_HANDLE, // Void: Gateway for other datatypes
		JVX_DATAFORMAT_POINTER, // Void: Gateway for other datatypes
		JVX_DATAFORMAT_CALLBACK, // Pointer for callbacks
		JVX_DATAFORMAT_INTERFACE,
		JVX_DATAFORMAT_LIMIT
} jvxDataFormat; 

#endif
