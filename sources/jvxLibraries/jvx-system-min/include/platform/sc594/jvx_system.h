#ifndef __JVX_BASE_DSP_TYPES_ADI_SHARC_H__
#define __JVX_BASE_DSP_TYPES_ADI_SHARC_H__

#include <adi_types.h>
#include <inttypes.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
// #include <sys/stat.h>

// ==========================================================
// On SHARC, the native format of all registers is 32 bits.
// Therefore, int16_t, int8_t and others are not defined!
typedef signed char     int8_t;
typedef unsigned char   uint8_t;
typedef short           int16_t;
typedef unsigned short  uint16_t;
// ==========================================================

//! Size datatype to be used within RTProc
typedef size_t jvxSize;

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

//! Bool variable
#ifdef __cplusplus
typedef bool jvxBool;
typedef jvxUInt16 jvxCBool;
#define c_true 1
#define c_false 0
#else
typedef jvxUInt16 jvxCBool;
#define true 1
#define false 0
#define c_true 1
#define c_false 0
#endif

#define JVX_MAKE_DIRECTORY_RETVAL int
#define JVX_MAKE_DIRECTORY_POS 0
#define JVX_MAKE_DIRECTORY(DIRNAME, ACCESS_RIGHTS) mkdir(DIRNAME, ACCESS_RIGHTS)

#define JVX_SIZET_SIZE(var) var 
#define JVX_SIZE_SIZET(var) var 

#define JVX_RESTRICT __restrict

#define JVX_FOPEN(fHdl, fName, howtoopen) fopen(&fHdl, fName, howtoopen)
#define JVX_FCLOSE(fHdl) fclose(fHdl)

//! Variable type for every unspecific data types.
typedef void jvxHandle;

//! Unsigned 64 bit datatype
typedef uint64_t jvxCBitField;

#include "../../jvx_system_common.h"
#include "jvx_system_helpers.h"
#include "../../jvx_system_includes.h"
// #include "../../jvx_system_dataformats.h"

#endif
