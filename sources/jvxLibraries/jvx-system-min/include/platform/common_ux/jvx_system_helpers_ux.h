#ifndef _JVX_SYSTEM_HELPERS_H__
#define _JVX_SYSTEM_HELPERS_H__

/*
#define JVX_MAKE_DIRECTORY_RETVAL int
#define JVX_MAKE_DIRECTORY_POS 0
#define JVX_MAKE_DIRECTORY(DIRNAME, ACCESS_RIGHTS) mkdir(DIRNAME, ACCESS_RIGHTS)
*/
#define JVX_SIZET_SIZE(var) var 
#define JVX_SIZE_SIZET(var) var 

#define JVX_RESTRICT /*__restrict*/
#define JVX_STATIC_INLINE static inline

#define JVX_PRINTF_S printf_s
#define JVX_SPRINTF_S sprintf_s

#define JVX_PRINTF_CAST_INT64 "%" PRId64
#define JVX_PRINTF_CAST_INT64_HEX "0x%016x" PRId64
#define JVX_PRINTF_CAST_UINT64 "%" PRIu64
#define JVX_PRINTF_CAST_INT32 "%" PRId32
#define JVX_PRINTF_CAST_UINT32 "%" PRIu32
#define JVX_PRINTF_CAST_UINT32_HEX "0x%08x" PRIu32
#define JVX_PRINTF_CAST_INT16 "%" PRId16
#define JVX_PRINTF_CAST_UINT16 "%" PRIu16
#define JVX_PRINTF_CAST_SIZE "%zu"
#define JVX_PRINTF_CAST_DATA "%f"

#define JVX_PRINTF_CAST_TICK JVX_PRINTF_CAST_INT64 // <- so defined in jvx_system_time.h
#define JVX_PRINTF_CAST_TICK_HEX JVX_PRINTF_CAST_INT64_HEX // <- so defined in jvx_system_time.h

#define JVX_FOPEN(fHdl, fName, howtoopen) fHdl = fopen(fName, howtoopen)
#define JVX_FOPEN_UNBUFFERED(fHdl, fName, howtoopen) \
	fHdl = fopen(fName, howtoopen); \
	if(fHdl) { setvbuf(fHdl, NULL, _IONBF, 0); }
#define JVX_FCLOSE(fHdl) fclose(fHdl)

JVX_SYSTEM_LIB_BEGIN

jvxCBool JVX_FILE_EXISTS(const char* fpath);
jvxCBool JVX_FILE_REMOVE(const char* fName);
jvxCBool JVX_DIRECTORY_EXISTS(const char* absolutePath);

JVX_SYSTEM_LIB_END

#define JVX_CREATE_DIRECTORY_RESULT int 
#define JVX_CREATE_DIRECTORY_SUCCESS 0
#define JVX_CREATE_DIRECTORY_FAILED -1
#define JVX_CREATE_DIRECTORY_DEFAULT_FLAGS 0777
#define JVX_CREATE_DIRECTORY(path) mkdir(path, 0777)
#define JVX_CREATE_DIRECTORY_MODE(path, mode) mkdir(path, mode)

// #if __cplusplus >= 201703L
#define std_isnan std::isnan
// #else

// #ifndef std_isnan
// #define std_isnan isnan
//#endif

// #endif

#endif
