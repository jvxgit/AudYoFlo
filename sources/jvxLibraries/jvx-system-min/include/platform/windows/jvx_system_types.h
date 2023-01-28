#ifndef _JVX_SYSTEM_TYPES_H__
#define _JVX_SYSTEM_TYPES_H__

//! Size datatype to be used within RTProc
typedef  size_t jvxSize;
#ifdef _WIN64
#define JVX_SIZE_IS_UINT64
#else
#define JVX_SIZE_IS_UINT32
#endif

//! 8 Bit = 1 Byte datatype
typedef char jvxByte;

//! 8 Bit = 1 Byte datatype
typedef unsigned char jvxUByte;

//! Conventional 32 bit data with sign
typedef signed __int32 jvxInt32;

//! Conventional 32 bit data without sign
typedef unsigned __int32 jvxUInt32;

//! Signed 16 bit datatype
typedef signed __int16 jvxInt16;

//! Signed 16 bit datatype
typedef unsigned __int16 jvxSize16;

//! Unsigned 16 bit datatype
typedef unsigned __int16 jvxUInt16;

//! Signed 64 bit datatype
typedef signed __int64 jvxInt64;

//! Unsigned 64 bit datatype
typedef unsigned __int64 jvxUInt64;

//! Signed 8 bit datatype
typedef signed __int8 jvxInt8;

//! Unsigned 8 bit datatype
typedef unsigned __int8 jvxUInt8;

#ifdef _WIN64
typedef jvxInt64 jvxSSize;
#else
typedef jvxInt32 jvxSSize;
#endif

typedef float float32;

typedef double float64;

//! Double precision floatdata format (64 bit float)
//typedef double jvxDouble;

//! Single precision floatdata format (64 bit float)
//typedef float jvxFloat;

//! Bool variable
#ifdef __cplusplus
typedef bool jvxBool;
typedef jvxUInt16 jvxCBool;
#define JVX_CBOOL_IS_UINT16
#define c_true 1
#define c_false 0
#define JVX_DATAFORMAT_BOOL JVX_DATAFORMAT_U16BIT_LE 
#else
typedef jvxUInt16 jvxCBool;
#define JVX_CBOOL_IS_UINT16
#define true 1
#define false 0
#define c_true 1
#define c_false 0
#endif

//! Variable type for every unspecific data types.
typedef void jvxHandle;

//! Unsigned 64 bit datatype
typedef jvxUInt64 jvxCBitField;
#define JVX_CBITFIELD_IS_UINT64
#define JVX_NUMBITS_CBITFIELD 64

#endif