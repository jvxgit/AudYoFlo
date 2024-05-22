#ifndef _JVX_SYSTEM_MUTEX_H__
#define _JVX_SYSTEM_MUTEX_H__

//===============================================
// Everything for multi threading
//===============================================

#define JVX_MUTEX_HANDLE CRITICAL_SECTION

#define JVX_LOCK_MUTEX(a) EnterCriticalSection(&a)

#define JVX_TRY_LOCK_MUTEX(returnVal,mutex) returnVal = TryEnterCriticalSection(&mutex)

#define JVX_TRY_LOCK_MUTEX_RESULT_TYPE BOOL

#define JVX_TRY_LOCK_MUTEX_SUCCESS 1
#define JVX_TRY_LOCK_MUTEX_NO_SUCCESS 0

#define JVX_TRY_LOCK_MUTEX_TEST_SUCCESS(var) var == JVX_TRY_LOCK_MUTEX_SUCCESS
#define JVX_TRY_LOCK_MUTEX_TEST_NO_SUCCESS(var) var != JVX_TRY_LOCK_MUTEX_SUCCESS


#define JVX_UNLOCK_MUTEX(a) LeaveCriticalSection(&a)

#define JVX_INITIALIZE_MUTEX(a) InitializeCriticalSection(&a)

#define JVX_TERMINATE_MUTEX(a) DeleteCriticalSection(&a)


// ==============================================================
#ifndef OLDER_THAN_WINDOWS7
// ==============================================================

#define JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE BOOL

#define JVX_TRY_LOCK_RW_MUTEX_INIT_SUCCESS TRUE
#define JVX_TRY_LOCK_RW_MUTEX_INIT_NO_SUCCESS FALSE

#define JVX_TRY_LOCK_RW_MUTEX_CHECK_SUCCESS(val) (val == JVX_TRY_LOCK_RW_MUTEX_INIT_SUCCESS)

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

#define JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE jvxCBool

#define JVX_TRY_LOCK_RW_MUTEX_INIT_SUCCESS true
#define JVX_TRY_LOCK_RW_MUTEX_INIT_NO_SUCCESS false

#define JVX_TRY_LOCK_RW_MUTEX_CHECK_SUCCESS(val) (val == JVX_TRY_LOCK_RW_MUTEX_INIT_SUCCESS)

typedef struct
{
    CRITICAL_SECTION readerCountLock;
    CRITICAL_SECTION writerLock;
    HANDLE noReaders;
    int readerCount;
} JVX_RW_MUTEX_HANDLE;

#define JVX_INITIALIZE_RW_MUTEX(a) jvx_initialize_rw_mutex_l(&a)

#define JVX_TERMINATE_RW_MUTEX(a) jvx_terminate_rw_mutex_l(&a)

#define JVX_LOCK_RW_MUTEX_EXCLUSIVE(a) jvx_lock_rw_mutex_exclusive_l(&a)

#define JVX_LOCK_RW_MUTEX_SHARED(a) jvx_lock_rw_mutex_shared_l(&a)

#define JVX_UNLOCK_RW_MUTEX_EXCLUSIVE( a) jvx_unlock_rw_mutex_exclusive_l(&a)

#define JVX_UNLOCK_RW_MUTEX_SHARED(a) jvx_unlock_rw_mutex_shared_l(&a)

#define JVX_TRY_LOCK_RW_MUTEX_EXCLUSIVE( res,  hdl) res = jvx_try_lock_rw_mutex_exclusive_l(&hdl)


#define JVX_TRY_LOCK_RW_MUTEX_SHARED(a, b) a = jvx_try_lock_rw_mutex_shared_l(&b)


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

#define JVX_INTERLOCKED_COMPARE_EXCHANGE_16(dest, exch, comp) _InterlockedCompareExchange16(dest, exch, comp)
//LONG __cdecl InterlockedCompareExchange(
//  _Inout_ LONG volatile *Destination,
//  _In_    LONG          Exchange,
//  _In_    LONG          Comparand
//);

#define JVX_INTERLOCKED_EXCHANGE_16(target, value) _InterlockedExchange16(target, value)
//LONG __cdecl InterlockedExchange(
//  _Inout_ LONG volatile *Target,
//  _In_    LONG          Value
//);

#define JVX_INTERLOCKED_EXCHANGE_ADD_64(target, value) InterlockedExchangeAdd64(target, value) 
// ==============================================================

JVX_SYSTEM_LIB_BEGIN

void jvx_initialize_rw_mutex_l(JVX_RW_MUTEX_HANDLE* a);
void jvx_terminate_rw_mutex_l(JVX_RW_MUTEX_HANDLE* a);
JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE jvx_try_lock_rw_mutex_shared_l(JVX_RW_MUTEX_HANDLE* b);
void jvx_lock_rw_mutex_exclusive_l(JVX_RW_MUTEX_HANDLE* a);
void jvx_lock_rw_mutex_shared_l(JVX_RW_MUTEX_HANDLE* a);
void jvx_unlock_rw_mutex_exclusive_l(JVX_RW_MUTEX_HANDLE* a);
void jvx_unlock_rw_mutex_shared_l(JVX_RW_MUTEX_HANDLE* a);
JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE jvx_try_lock_rw_mutex_exclusive_l(JVX_RW_MUTEX_HANDLE* b);

JVX_SYSTEM_LIB_END

#endif
