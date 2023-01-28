#ifndef _JVX_SYSTEM_TYPES_H__
#define _JVX_SYSTEM_TYPES_H__

typedef size_t jvxSize;

#ifdef JVX_OS_LINUX

/* 
 * On Mac OS X, the size_t is a specific variable type and
 * is not typedefed to another type.
 */
 
//! Size datatype to be used within RTProc
#if defined(JVX_SYS_LINUX_GNU_32BIT)
#define JVX_SIZE_IS_UINT32
#else
#ifndef __LP64__
#define JVX_SIZE_IS_UINT32
#else
#define JVX_SIZE_IS_UINT64

#endif // #ifndef __LP64__
#endif // #if defined(JVX_SYS_LINUX_GNU_32BIT)
#endif // #ifdef JVX_OS_LINUX

typedef ssize_t jvxSSize;

//! 8 Bit = 1 Byte datatype
typedef char jvxByte;

//! 8 Bit = 1 Byte datatype
typedef unsigned char jvxUByte;

//! Conventional 32 bit data with sign
typedef int32_t jvxInt32;

//! Conventional 32 bit data without sign
typedef uint32_t jvxUInt32;

//! Signed 16 bit datatype
typedef int16_t jvxInt16;

//! Signed 16 bit datatype
typedef uint16_t jvxSize16;

//! Unsigned 16 bit datatype
typedef uint16_t jvxUInt16;

//! Signed 64 bit datatype
typedef int64_t jvxInt64;

//! Unsigned 64 bit datatype
typedef uint64_t jvxUInt64;

//! Signed 8 bit datatype
typedef int8_t jvxInt8;

//! Unsigned 8 bit datatype
typedef uint8_t jvxUInt8;

//! Double precision floatdata format (64 bit float)
//typedef double jvxDouble;

//! Single precision floatdata format (64 bit float)
//typedef float jvxFloat;

typedef float float32;

typedef double float64;

//! Bool variable
#ifdef __cplusplus
typedef bool jvxBool;
typedef jvxUInt16 jvxCBool;
#define JVX_CBOOL_IS_UINT16
#define c_true 1
#define c_false 0
#define JVX_DATAFORMAT_BOOL JVX_DATAFORMAT_16BIT_LE 
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
typedef uint64_t jvxCBitField;
#define JVX_CBITFIELD_IS_UINT64
#define JVX_NUMBITS_CBITFIELD 64

#endif
