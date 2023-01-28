#ifndef __JVX_PLATFORM_H___WINDOWS__
#define __JVX_PLATFORM_H___WINDOWS__

#include <windows.h>
#include <string>
#include <cmath>
#include <ctime>
#include <direct.h>
#include <vector>
#include <cassert>
#include <Setupapi.h>
#include <tlhelp32.h>
#include <intrin.h>

#include "jvx_system.h"

#include "../jvx_platform_common.h"

//==============================================================
// Datatype definitions
//==============================================================

#define JVX_OS_WINDOWS

// Some more specific datatypes (not just processing)
#define JVX_TIME_USEC long
#define JVX_TIME_SEC long

#if _MSC_VER == 1900
#define JVX_PRINTF_CAST_INT64 "%lld"
#define JVX_PRINTF_CAST_SIZE "%zd"
#else
#define JVX_PRINTF_CAST_INT64 "%lld"
#define JVX_PRINTF_CAST_SIZE "%lld"
#endif

#define STATIC_INLINE static inline 

#define JVX_SYSTEM_DESCRIPTOR_PLATFORM JVX_SYSTEM_WINDOWS

#define JVX_ISINF(value) (value == std::numeric_limits<jvxData>::infinity())

#define JVX_ISNEGINF(value) (value == -std::numeric_limits<jvxData>::infinity())

// Define the interface type, in Mac, interface keyword is unknown and reused by someone
#define JVX_INTERFACE interface

#define JVX_CONVERT_BITFIELD_STRING_FUNC _strtoui64

//===============================================
// Misc stuff
//===============================================
#define JVX_CHDIR(a) _chdir(a)

#define JVX_SEPARATOR_DIR "\\"
#define JVX_SEPARATOR_DIR_THE_OTHER "/"

#define JVX_SEPARATOR_DIR_CHAR '\\'
#define JVX_SEPARATOR_DIR_CHAR_THE_OTHER '/'

#define JVX_COMPONENT_DIR "rtpComponents"

static std::string JVX_DISPLAY_LAST_ERROR_LOAD(std::string fName)
{
	DWORD errID = GetLastError();//errno

	char str[1024];
#if _MSC_VER >= 1600
	sprintf_s(str, "%d", errID);
#else
	sprintf(str, "%d", errID);
#endif

	std::string errMess = (std::string)"Unknown, error id=" + str;
	DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM;
	LPSTR buf = NULL;
	/*
	 DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM;
	 char str[
	 */
	if(FormatMessage(dwFlags, NULL, errID, 0/*GetSystemDefaultLangID()*/, (LPTSTR)&buf, 1024, NULL) != 0)
	{
		errMess = "File: " + fName + ", opening failed: " + buf;
		LocalFree(buf);
	}
	else
	{
		errID = GetLastError();

#if _MSC_VER >= 1600
	sprintf_s(str, "%d", errID);
#else
	sprintf(str, "%d", errID);
#endif

	errMess = "File: " + fName + ", opening failed, error id = " + str;
	}
	return(errMess);
}

#define JVX_RS232_HANDLE HANDLE

#define JVX_INIT_LOCALE(a)

//===============================================
// Everything for access to dynamic link libraries
//===============================================
//
#define JVX_DLL_EXPORT __declspec(dllexport)

#define JVX_HMODULE HMODULE

#define JVX_INVALID_HANDLE_VALUE INVALID_HANDLE_VALUE

#define JVX_LOADLIBRARYA(a) LoadLibraryA(a)

#define JVX_LOADLIBRARYA_PATH(a,b) LoadLibraryA(((std::string)b+ JVX_SEPARATOR_DIR + a).c_str())

#define JVX_LOADLIBRARY(a) LoadLibrary(a)

static JVX_HMODULE JVX_LOADLIBRARY_PATHVEC(std::string fName, std::vector<std::string> PathVec)
{
	for (size_t i = 0; i < PathVec.size(); i++) {
		if( PathVec[i] != "" )
		{
			JVX_HMODULE dll_loaded = LoadLibraryA((std::string(PathVec[i]+ JVX_SEPARATOR_DIR +fName).c_str()));
			if ( dll_loaded ) return( dll_loaded );
		}
	}
	return( LoadLibraryA(fName.c_str()) );
}

static JVX_HMODULE JVX_LOADLIBRARY_PATH(std::string fName, std::string dir)
{
	if (dir != "")
	{
		return LoadLibraryA(((std::string)dir + JVX_SEPARATOR_DIR + fName).c_str());
	} else {
		return( LoadLibraryA(fName.c_str()) );
	}
}
//#define JVX_LOADLIBRARY_PATH(a,b) LoadLibraryA(((std::string)b+ JVX_SEPARATOR_DIR + a).c_str())

#define JVX_GETPROCADDRESS(a,b) GetProcAddress(a, b)

#define JVX_UNLOADLIBRARY(a) FreeLibrary(a)

//===============================================
// Everything for network stuff
//===============================================

// We use a specific socket definition here since pcap redefines socket as a uint
#define JVX_SOCKET UINT_PTR
// SOCKET

#define socklen_t int
#define JVX_CLOSE_SOCKET closesocket
#define JVX_SOCKADDR SOCKADDR
#define JVX_SOCKADDR_IN SOCKADDR_IN
#define JVX_INVALID_SOCKET_HANDLE  (SOCKET)(~0)

// Some defines for the new poll function
#define JVX_POLL_SOCKET_STRUCT WSAPOLLFD /* struct pollfd */
#define JVX_POLL_SOCKET WSAPoll /* poll */
#define JVX_POLL_SOCKET_RDNORM POLLRDNORM  /* POLLIN */
#define JVX_POLL_SOCKET_ERR POLLERR
#define JVX_POLL_SOCKET_HUP POLLHUP
#define JVX_POLL_SOCKET_NVAL POLLNVAL
#define JVX_SOCKET_POLL_INIT_DEFAULT (POLLIN)
#define JVX_SHUTDOWN_SOCKET shutdown

#define JVX_SOCKET_SD_RECEIVE SD_RECEIVE
#define JVX_SOCKET_SD_SEND SD_SEND
#define JVX_SOCKET_SD_BOTH SD_BOTH

#define JVX_SOCKET_REVENT SHORT

#define JVX_SOCKET_NUM_BYTES u_long
#define JVX_IOCTRL_SOCKET_FIONREAD(sock, ptrRead) ioctlsocket(sock, FIONREAD, ptrRead)

//===============================================
// Everything for multi threading
//===============================================

#define JVX_MUTEX_HANDLE CRITICAL_SECTION

#define JVX_LOCK_MUTEX(a) EnterCriticalSection(&a)

#define JVX_TRY_LOCK_MUTEX(returnVal,mutex) returnVal = TryEnterCriticalSection(&mutex)

#define JVX_TRY_LOCK_MUTEX_RESULT_TYPE BOOL

#define JVX_TRY_LOCK_MUTEX_SUCCESS 1

//#define JVX_TRY_LOCK_MUTEX_NO_SUCCESS 0 <- this does not exist in Linux, use !SUCESS!!

#define JVX_UNLOCK_MUTEX(a) LeaveCriticalSection(&a)

#define JVX_INITIALIZE_MUTEX(a) InitializeCriticalSection(&a)

#define JVX_TERMINATE_MUTEX(a) DeleteCriticalSection(&a)

#define JVX_NOTIFY_HANDLE HANDLE

#define JVX_INITIALIZE_NOTIFICATION(a) a = CreateEvent(NULL, false, false, NULL)

#define JVX_TERMINATE_NOTIFICATION(a) CloseHandle(a)

// ==============================================================
#ifdef WINDOWS7_NEWER
// ==============================================================

#define JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE BOOL

#define JVX_TRY_LOCK_RW_MUTEX_SUCCESS TRUE

#define JVX_RW_MUTEX_HANDLE SRWLOCK

#define JVX_INITIALIZE_RW_MUTEX(a) InitializeSRWLock(&a)

#define JVX_TERMINATE_RW_MUTEX(a)
//TerminateSRWLock(&a)

#define JVX_LOCK_RW_MUTEX_EXCLUSIVE(a) AcquireSRWLockExclusive(&a)

#define JVX_LOCK_RW_MUTEX_SHARED(a) AcquireSRWLockShared(&a)

#define JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(a) ReleaseSRWLockExclusive(&a)

#define JVX_UNLOCK_RW_MUTEX_SHARED(a) ReleaseSRWLockShared(&a)

#define JVX_TRY_LOCK_RW_MUTEX_EXCLUSIVE(a,b) a = TryAcquireSRWLockExclusive(&b)

#define JVX_TRY_LOCK_RW_MUTEX_SHARED(a,b) a = TryAcquireSRWLockShared(&b)

// SleepConditionVariableSRW 	Sleeps on the specified condition variable and releases the specified lock as an atomic operation.

/*
AcquireSRWLockExclusive 	Acquires an SRW lock in exclusive mode.
AcquireSRWLockShared 	Acquires an SRW lock in shared mode.
InitializeSRWLock 	Initialize an SRW lock.
ReleaseSRWLockExclusive 	Releases an SRW lock that was opened in exclusive mode.
ReleaseSRWLockShared 	Releases an SRW lock that was opened in shared mode.
SleepConditionVariableSRW 	Sleeps on the specified condition variable and releases the specified lock as an atomic operation.
TryAcquireSRWLockExclusive 	Attempts to acquire a slim reader/writer (SRW) lock in exclusive mode. If the call is successful, the calling thread takes ownership of the lock.
TryAcquireSRWLockShared
*/

#else

// Emulate RW locks, e.g., on Windows XP
// Code fragments taken from here: http://stackoverflow.com/questions/1008726/win32-read-write-lock-using-only-critical-sections

#define JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE bool

#define JVX_TRY_LOCK_RW_MUTEX_SUCCESS true

typedef struct
{
    CRITICAL_SECTION readerCountLock;
    CRITICAL_SECTION writerLock;
    HANDLE noReaders;
    int readerCount;
} JVX_RW_MUTEX_HANDLE;

static void JVX_INITIALIZE_RW_MUTEX(JVX_RW_MUTEX_HANDLE& a)
{
    InitializeCriticalSection(&a.readerCountLock);
    InitializeCriticalSection(&a.writerLock);

    /*
     * We use a manual-reset event as poor man condition variable that
     * can only do broadcast.  Actually, only one thread will be waiting
     * on this at any time, because the wait is done while holding the
     * writerLock.
     */
    a.noReaders = CreateEvent (NULL, TRUE, TRUE, NULL);
	a.readerCount = 0;
}


static void JVX_TERMINATE_RW_MUTEX(JVX_RW_MUTEX_HANDLE& a)
{
    DeleteCriticalSection(&a.readerCountLock);
    DeleteCriticalSection(&a.writerLock);

    CloseHandle(a.noReaders);
	a.readerCount = 0;
}

static void JVX_LOCK_RW_MUTEX_EXCLUSIVE(JVX_RW_MUTEX_HANDLE& a)
{
	EnterCriticalSection(&a.writerLock);
    if (a.readerCount > 0)
	{
        WaitForSingleObject(a.noReaders, INFINITE);
    }

    /* writerLock remains locked.  */
}

static void JVX_LOCK_RW_MUTEX_SHARED(JVX_RW_MUTEX_HANDLE& a)
{
	/*
     * We need to lock the writerLock too, otherwise a writer could
     * do the whole of rwlock_wrlock after the readerCount changed
     * from 0 to 1, but before the event was reset.
     */
    EnterCriticalSection(&a.writerLock);
    EnterCriticalSection(&a.readerCountLock);
    if (++a.readerCount == 1)
	{
        ResetEvent(a.noReaders);
    }
    LeaveCriticalSection(&a.readerCountLock);
	LeaveCriticalSection(&a.writerLock);
}

static void JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(JVX_RW_MUTEX_HANDLE& a)
{
	LeaveCriticalSection(&a.writerLock);
}


static void JVX_UNLOCK_RW_MUTEX_SHARED(JVX_RW_MUTEX_HANDLE& a)
{
	EnterCriticalSection(&a.readerCountLock);
    assert (a.readerCount > 0);
    if (--a.readerCount == 0)
	{
        SetEvent(a.noReaders);
    }
    LeaveCriticalSection(&a.readerCountLock);
}

static void JVX_TRY_LOCK_RW_MUTEX_EXCLUSIVE(JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE& a, JVX_RW_MUTEX_HANDLE& b)
{
	if(TryEnterCriticalSection(&b.writerLock) == TRUE)
	{
		if (b.readerCount == 0)
		{
			a = true;
		}
		else
		{
			LeaveCriticalSection(&b.writerLock);
			a = false;
		}
    }
	else
	{
		// Lock has not been acquired
		a = false;
	}
}

static void JVX_TRY_LOCK_RW_MUTEX_SHARED(JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE& a, JVX_RW_MUTEX_HANDLE& b)
{
	if(TryEnterCriticalSection(&b.writerLock))
	{
	    if(TryEnterCriticalSection(&b.readerCountLock))
		{
			if (++b.readerCount == 1)
			{
				ResetEvent(b.noReaders);
			}
			LeaveCriticalSection(&b.readerCountLock);
			LeaveCriticalSection(&b.writerLock);
			a = true;
		}
		else
		{
			LeaveCriticalSection(&b.writerLock);
			a = false;
		}
	}
	else
	{
		a = false;
	}
}

#endif

// ==============================================================
// Some Atomic Operations to - maybe - replace critical sections
// From MSDN: LONG __cdecl InterlockedCompareExchange(  _Inout_  LONG volatile *Destination, _In_     LONG Exchange, _In_     LONG Comparand);
// The function returns the initial value of the Destination parameter.
// ==============================================================

#define JVX_ATOMIC_MUTEX_HANDLE_TYPE volatile LONG

#define JVX_ATOMIC_MUTEX_HANDLE_CREATE(obj) obj = 0

#define JVX_ATOMIC_MUTEX_HANDLE_TRYENTER(obj) InterlockedCompareExchange((LONG*)&obj, 1, 0)

#define JVX_ATOMIC_MUTEX_HANDLE_ENTERSUCCESS 0

#define JVX_ATOMIC_MUTEX_HANDLE_TRYLEAVE(obj) InterlockedCompareExchange((LONG*)&obj, 0, 1)

#define JVX_ATOMIC_MUTEX_HANDLE_LEAVESUCCESS 1

#define JVX_ATOMIC_MUTEX_HANDLE_DELETE(obj)

// ==============================================================
#define JVX_INTERLOCKED_16_VAR unsigned long

#define JVX_INTERLOCKED_COMPARE_EXCHANGE_16(dest, exch, comp) _InterlockedCompareExchange(dest, exch, comp)
//LONG __cdecl InterlockedCompareExchange(
//  _Inout_ LONG volatile *Destination,
//  _In_    LONG          Exchange,
//  _In_    LONG          Comparand
//);

#define JVX_INTERLOCKED_EXCHANGE_16(target, value) _InterlockedExchange(target, value)
//LONG __cdecl InterlockedExchange(
//  _Inout_ LONG volatile *Target,
//  _In_    LONG          Value
//);

// ==============================================================

#define JVX_THREAD_HANDLE HANDLE

#define JVX_NULLTHREAD NULL

#define JVX_THREAD_ID DWORD

#define JVX_THREAD_EXIT_CODE DWORD

#define JVX_STILL_ACTIVE STILL_ACTIVE

#define JVX_RETURN_THREAD(a) return(a)

#define JVX_GET_CURRENT_THREAD_ID GetCurrentThreadId

#define JVX_GET_CURRENT_THREAD_HANDLE GetCurrentThread

#define JVX_THREAD_RETURN_TYPE DWORD

#define JVX_COMPARE_THREADS(a,b) (a == b)

#define JVX_THREAD_ENTRY_FUNCTION(fName,param) static JVX_THREAD_RETURN_TYPE WINAPI fName(LPVOID param)

#define JVX_CREATE_THREAD(hdl, entry, param, id) hdl = CreateThread(NULL, 0, entry, param, 0, &id);

#define JVX_WAIT_FOR_NOTIFICATION_MS(a, b) WaitForSingleObject(a, b)

#define JVX_WAIT_FOR_NOTIFICATION_S(a, b) WaitForSingleObject(a, b*1000)

#define JVX_WAIT_FOR_NOTIFICATION_I(hdl)

#define JVX_WAIT_FOR_NOTIFICATION_II_MS(a, b) JVX_WAIT_FOR_NOTIFICATION_MS(a,b)

#define JVX_WAIT_FOR_NOTIFICATION_II_S(a, b) JVX_WAIT_FOR_NOTIFICATION_S(a,b)

#define JVX_WAIT_SUCCESS WAIT_OBJECT_0

#define JVX_WAIT_RESULT DWORD

#define JVX_TERMINATE_THREAD(thrdHandle, exitCode) TerminateThread(thrdHandle, exitCode);

#define JVX_SET_NOTIFICATION(a) SetEvent(a)

#define JVX_RESET_NOTIFICATION(a) ResetEvent(a)

#define JVX_THREAD_PRIORITY DWORD

#define JVX_SET_THREAD_PRIORITY(a,b) SetThreadPriority(a,b)

#define JVX_GET_THREAD_PRIORITY(a) GetThreadPriority(a)

#define JVX_THREAD_PRIORITY_REALTIME THREAD_PRIORITY_TIME_CRITICAL

#define JVX_THREAD_BACKGROUND_BEGIN(tHdl) SetThreadPriority(tHdl, THREAD_MODE_BACKGROUND_BEGIN)

#define JVX_THREAD_BACKGROUND_END(tHdl) SetThreadPriority(tHdl, THREAD_MODE_BACKGROUND_END)

#define JVX_THREAD_PRIORITY_NORMAL THREAD_PRIORITY_NORMAL

#define JVX_WAIT_FOR_THREAD_TERMINATE_MS JVX_WAIT_FOR_NOTIFICATION_MS

#define JVX_INFINITE INFINITE

//#define JVX_GET_TICKCOUNT64 GetTickCount64

//#define JVX_GET_TICKCOUNT32 GetTickCount

typedef struct
{
	LARGE_INTEGER referenceTime;
} jvxTimeStampData;

STATIC_INLINE void JVX_GET_TICKCOUNT_US_SETREF(jvxTimeStampData& refData)
{
  QueryPerformanceCounter(&refData.referenceTime);
}

STATIC_INLINE jvxInt64 JVX_GET_TICKCOUNT_US_GET(jvxTimeStampData& refData)
{
	LARGE_INTEGER freq, time;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&time);
	double freqDbl = (double)freq.QuadPart;
	double cntDbl = (double)(time.QuadPart - refData.referenceTime.QuadPart);
	double time_us = (cntDbl/freqDbl)*1000000.0;
	return((jvxInt64)time_us);
}

STATIC_INLINE jvxInt64 JVX_GET_TICKCOUNT_US_ABS()
{
	LARGE_INTEGER freq, time;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&time);
	double freqDbl = (double)freq.QuadPart;
	double cntDbl = (double)(time.QuadPart);
	double time_us = (cntDbl/freqDbl)*1000000.0;
	return((jvxInt64)time_us);
}


#define JVX_SLEEP_MS(a) Sleep((DWORD)a)

#define JVX_SLEEP_S(a) Sleep(a*1000)

//===============================================
// Everything for directory/file access
//===============================================

#define JVX_GETCURRENTDIRECTORY(a, b) _getcwd(a, b)/*GetCurrentDirectory(b, a)*/

#define JVX_HANDLE_DIR HANDLE

#define JVX_DIR_FIND_DATAA WIN32_FIND_DATAA
#define JVX_FINDFIRSTFILEA(a,b1,b2,c) FindFirstFileA( ((std::string)b1 + JVX_SEPARATOR_DIR +"*" +b2).c_str(), &c )
#define JVX_FINDNEXTFILEA(a,b,c) FindNextFileA( a, &b )

#define JVX_DIR_FIND_DATA WIN32_FIND_DATA

#define INIT_SEARCH_DIR_DEFAULT {0}
#define JVX_FINDFIRSTFILE_WC(a,b1,b2,c) FindFirstFile( ((std::string)b1 + JVX_SEPARATOR_DIR +"*" +b2).c_str(), &c )
#define JVX_FINDFIRSTFILE_S(path, file, handle) FindFirstFile((path + std::string(JVX_SEPARATOR_DIR) + file).c_str(), &handle )
#define JVX_FINDNEXTFILE(a,b,c) FindNextFile( a, &b )

#define JVX_GETFILENAMEDIR(a) a.cFileName

#define JVX_FINDCLOSE(a) FindClose( a)

//===============================================
// Everything for rs232 variables
//===============================================
#define JVX_HANDLE_PORT HANDLE

//===============================================
// Everything for environment variables
//===============================================

#define JVX_GETENVIRONMENTVARIABLE(a,b,c) GetEnvironmentVariableA(a, b, c)

#define JVX_SETENVIRONMENTVARIABLE(a, b) SetEnvironmentVariableA(a, b)

#define JVX_PASTE_ENV_VARIABLE_ATT(a, b, c) (std::string)a + JVX_SEPARATOR_DIR + c + ";" + b;

#define JVX_PASTE_ENV_VARIABLE_ADD_PATH_FRONT(a, b) (std::string)b + ";" + a;

#define JVX_PASTE_ENV_VARIABLE_NEW(a,b) (std::string)a + JVX_SEPARATOR_DIR + b;


//! Define the overall calling conventions for all C++ functions!
#define JVX_CALLINGCONVENTION /*__stdcall*/

//===============================================
// Some important names
//===============================================

#define JVX_DLL_EXTENSION ".dll"

#define JVX_MEX_EXTENSION ".mexw32"

#define JVX_DYN_LIB_PATH_VARIABLE "PATH"

#define JVX_LOAD_DLL_DIR ((std::string)JVX_SEPARATOR_DIR "loadDLLs")

static std::string JVX_TIME()
{
	char ret[256];
	memset(ret, 0, sizeof(char)*256);
	_strtime_s(ret, 256);
	std::string strDate = ret;
	return(strDate);
}

static std::string JVX_DATE()
{
	char ret[256];
	memset(ret, 0, sizeof(char)*256);
	_strdate_s(ret, 256);
	std::string strDate = ret;
	return(strDate);
}

// Expression for "INFINITE" in maths
#define JVX_MATH_INFINITE std::numeric_limits<jvxData>::infinity()

// Expression for "NOT A NUMBER" in maths
#define JVX_MATH_NAN std::numeric_limits<jvxData>::quiet_NaN()

#define JVX_FSEEK _fseeki64

#define JVX_FTELL _ftelli64

#define JVX_GUID GUID

//! Define the GUID for access to the COMPORT, taken from somewhere in the internet
#ifndef GUID_CLASS_COMPORT
DEFINE_GUID(GUID_CLASS_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4,\
			0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73)
#endif

#define JVX_PROCESS_HANDLE HANDLE

#define JVX_GET_CURRENT_PROCESS_HANDLE(hdl) hdl = GetCurrentProcess()

#define JVX_GET_PROCESS_AFFINITY_RESULT BOOL

#define JVX_GET_PROCESS_AFFINITY_SUCCESS TRUE

#define JVX_AFFINITY_MASK DWORD_PTR

#define JVX_GET_PROCESS_AFFINITY_MASK(res, hdl, procAff, sysAff) res = GetProcessAffinityMask(hdl, &procAff, &sysAff)

#define JVX_SET_PROCESS_AFFINITY_MASK(res, hdl, procAff) res = SetProcessAffinityMask(hdl, procAff)

#define JVX_GET_PROCESS_ID(id, hdl) id = GetProcessId(hdl)

#define JVX_PROCESS_ID DWORD

#define JVX_OPEN_PROCESS(hdl, id) hdl = OpenProcess( PROCESS_ALL_ACCESS, FALSE, id );

#define JVX_CLOSE_PROCESS(hdl) CloseHandle(hdl)

// Compile for MATLAB version MATLAB 2007 A plus
// 100: 2007A
// 200: 2008A
// 250: 2008b
// 300: 2009A
// 350: 2009B
// 400: 2010A
// 450: 2010B
// 500: 2011A
#define _MATLAB_MEXVERSION 500

#ifndef __func__
#define __func__ __FUNCTION__
#endif

//#ifndef isnan
//#define isnan(x) ((x) != (x))
//#endif

#endif
