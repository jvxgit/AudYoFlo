#ifndef __JVX_PLATFORM_H___SC594__
#define __JVX_PLATFORM_H___SC594__

/*
typedef jvxUInt64 jvxTimeStampData;

static inline void JVX_GET_TICKCOUNT_CLK_SETREF(jvxTimeStampData* refData)
{
	*refData = _itoll(TSCH, TSCL);
}

static inline jvxInt64 JVX_GET_TICKCOUNT_CLK_GET(jvxTimeStampData* refData)
{
	jvxUInt64 tsc_now = _itoll(TSCH, TSCL);

    // detect (single) wraparound
    if (*refData > tsc_now)
	{
        return (jvxUInt64)ULLONG_MAX - *refData + tsc_now;
    } 
	else 
	{
        return tsc_now - *refData;
    }
}

// To be used in combination with private (void) function pointers
static inline jvxInt64 JVX_GET_TICKCOUNT_CLK_GET2(jvxTimeStampData* refData, jvxInt64* output)
{
    *output = JVX_GET_TICKCOUNT_CLK_GET(refData);
}
*/

#include <string>
#include <vector>
#include <list>
#include <map>
#include <limits>

//#include <type_traits>
namespace std {
template<typename T, typename U>
struct is_same {
  static constexpr bool value = false;
};

template<typename T>
struct is_same<T, T> {
  static constexpr bool value = true;
};
};

#define JVX_THREAD_ID int
#define JVX_THREAD_ID_INVALID 0
#define JVX_NULLTHREAD 0
#define JVX_INTERFACE struct

#define JVX_CALLINGCONVENTION

// ==============================================================
// MUTEX
// ==============================================================

#define JVX_MUTEX_HANDLE jvxHandle*
#define JVX_LOCK_MUTEX(a) assert(false)
#define JVX_TRY_LOCK_MUTEX(returnVal,mutex) assert(false)
#define JVX_TRY_LOCK_MUTEX_RESULT_TYPE jvxCBool
#define JVX_TRY_LOCK_MUTEX_SUCCESS c_true
#define JVX_TRY_LOCK_MUTEX_NO_SUCCESS c_false
#define JVX_TRY_LOCK_MUTEX_TEST_SUCCESS(var) var == JVX_TRY_LOCK_MUTEX_SUCCESS
#define JVX_TRY_LOCK_MUTEX_TEST_NO_SUCCESS(var) var != JVX_TRY_LOCK_MUTEX_SUCCESS
#define JVX_UNLOCK_MUTEX(a) assert(false)
#define JVX_INITIALIZE_MUTEX(a) a = NULL
#define JVX_TERMINATE_MUTEX(a) assert(false)

// ==============================================================
// RW MUTEX
// ==============================================================
#define JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE jvxCBool
#define JVX_TRY_LOCK_RW_MUTEX_INIT_SUCCESS c_true
#define JVX_TRY_LOCK_RW_MUTEX_INIT_NO_SUCCESS c_false
#define JVX_TRY_LOCK_RW_MUTEX_CHECK_SUCCESS(val) (val == JVX_TRY_LOCK_RW_MUTEX_INIT_SUCCESS)
#define JVX_RW_MUTEX_HANDLE jvxHandle*
#define JVX_INITIALIZE_RW_MUTEX(a) a = NULL
#define JVX_TERMINATE_RW_MUTEX(a) assert(false)
#define JVX_LOCK_RW_MUTEX_EXCLUSIVE(a) assert(false)
#define JVX_LOCK_RW_MUTEX_SHARED(a) assert(false)
#define JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(a) assert(false)
#define JVX_UNLOCK_RW_MUTEX_SHARED(a) assert(false)
#define JVX_TRY_LOCK_RW_MUTEX_EXCLUSIVE(a,b) assert(false)
#define JVX_TRY_LOCK_RW_MUTEX_SHARED(a,b) assert(false)

// ==============================================================
// ==============================================================
#define JVX_NOTIFY_HANDLE int
#define JVX_NOTIFY_HANDLE_DECLARE(var) int var

#define JVX_INFINITE_MS JVX_SIZE_UNSELECTED

#define JVX_ISINF(value) (value == std::numeric_limits<jvxData>::infinity())
#define JVX_ISNEGINF(value) (value == -std::numeric_limits<jvxData>::infinity())

std::string JVX_DATE();
std::string JVX_TIME();
int JVX_GETCURRENTDIRECTORY(char* destBuf, size_t szBytes);
int JVX_DIRECTORY_EXISTS(const char* dirname);

#define JVX_SEPARATOR_DIR "/"
#define JVX_SEPARATOR_DIR_CHAR '/'
#define JVX_SEPARATOR_DIR_CHAR_THE_OTHER '\\'

// ==================================================================
// Threading
// ==================================================================

#define JVX_THREAD_ID int
static int JVX_GET_CURRENT_THREAD_ID() {return -1;}
typedef int jvxTick;

typedef void* JVX_HMODULE;

#endif
