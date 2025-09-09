#ifndef _JVX_SYSTEM_HELPERS_H__
#define _JVX_SYSTEM_HELPERS_H__

// #define STATIC_INLINE static inline 
#define JVX_STATIC_INLINE static inline

#define JVX_PRINTF_CAST_INT64 "%" PRId64
#define JVX_PRINTF_CAST_INT64_HEX "0x%016x" PRId64
#define JVX_PRINTF_CAST_UINT64 "%" PRIu64
#define JVX_PRINTF_CAST_INT32 "%" PRId32
#define JVX_PRINTF_CAST_UINT32 "%" PRIu32
#define JVX_PRINTF_CAST_UINT32_HEX "0x%08x" PRIu32
#define JVX_PRINTF_CAST_INT16 "%" PRId16
#define JVX_PRINTF_CAST_UINT16 "%" PRIu16
#define JVX_PRINTF_CAST_SIZE "%" PRIuFAST32
#define JVX_PRINTF_CAST_DATA "%" PRIdFAST32

#define JVX_PRINTF_CAST_TICK JVX_PRINTF_CAST_INT64 // <- so defined in jvx_system_time.h
#define JVX_PRINTF_CAST_TICK_HEX JVX_PRINTF_CAST_INT64_HEX // <- so defined in jvx_system_time.h
/* from https://stackoverflow.com/questions/2125845/platform-independent-size-t-format-specifiers-in-c/22114959#22114959 */


#endif

