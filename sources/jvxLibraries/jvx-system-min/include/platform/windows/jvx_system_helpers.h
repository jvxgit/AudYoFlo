#ifndef _JVX_SYSTEM_HELPERS_H__
#define _JVX_SYSTEM_HELPERS_H__

#define JVX_SIZET_SIZE(var) var 
#define JVX_SIZE_SIZET(var) var 

#define JVX_RESTRICT /*__restrict*/
#define STATIC_INLINE static inline 
#if defined(__GNUC__) || defined(__clang__)
#define JVX_FORCE_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define JVX_FORCE_INLINE __forceinline
#else
#define JVX_FORCE_INLINE
#endif

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
#define JVX_PRINTF_CAST_SIZE "%Iu"
#define JVX_PRINTF_CAST_DATA "%f"

#define JVX_PRINTF_CAST_TICK JVX_PRINTF_CAST_INT64 // <- so defined in jvx_system_time.h
#define JVX_PRINTF_CAST_TICK_HEX JVX_PRINTF_CAST_INT64_HEX // <- so defined in jvx_system_time.h
/* from https://stackoverflow.com/questions/2125845/platform-independent-size-t-format-specifiers-in-c/22114959#22114959 */

/*
#define JVX_MAKE_DIRECTORY_RETVAL int
#define JVX_MAKE_DIRECTORY_POS 0
#define JVX_MAKE_DIRECTORY(DIRNAME, ACCESS) _mkdir(DIRNAME)
#define JVX_MAKE_DIRECTORY_DEFAULT_ACCESS 0777
*/

// Fopen must be an fopen_s in Windows!
#define JVX_FOPEN(fHdl, fName, howtoopen) fopen_s(&fHdl, fName, howtoopen)
#define JVX_FOPEN_UNBUFFERED(fHdl, fName, howtoopen) \
	fopen_s(&fHdl, fName, howtoopen); \
	if(fHdl) {setvbuf(fHdl, NULL, _IONBF, 0);}

#define JVX_FCLOSE(fHdl) fclose(fHdl)

#define JVX_STATIC_INLINE static inline

JVX_SYSTEM_LIB_BEGIN

jvxCBool JVX_FILE_EXISTS(const char* fpath);
// jvxCBool JVX_FOLDER_EXISTS(const char* fpath);
jvxCBool JVX_FILE_REMOVE(const char* fName);
jvxCBool JVX_DIRECTORY_EXISTS(const char* absolutePath);

JVX_SYSTEM_LIB_END

/*
https://stackoverflow.com/questions/4387288/convert-stdwstring-to-const-char-in-c
*/
//wcstombs(path, (const wchar_t*)wpath, MAX_PATH);

#define JVX_CREATE_DIRECTORY_RESULT BOOL 
#define JVX_CREATE_DIRECTORY_SUCCESS TRUE
#define JVX_CREATE_DIRECTORY_FAILED FALSE
#define JVX_CREATE_DIRECTORY_DEFAULT_FLAGS 0777
#define JVX_CREATE_DIRECTORY(path) CreateDirectoryA(path, NULL)
#define JVX_CREATE_DIRECTORY_MODE(path, flags) CreateDirectoryA(path, NULL)


#ifndef __cplusplus
// It seems that defining this macro causes many problems in C++
#define inline __inline


#endif

#ifndef __func__
#define __func__ __FUNCTION__
#endif

#if __cplusplus >= 201703L
#define std_isnan std::isnan
#else

#ifndef std_isnan
#define std_isnan _isnan
#endif

#endif

#endif
