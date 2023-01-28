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

#define JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE jvxCBool

#define JVX_TRY_LOCK_RW_MUTEX_SUCCESS true

typedef struct
{
    CRITICAL_SECTION readerCountLock;
    CRITICAL_SECTION writerLock;
    HANDLE noReaders;
    int readerCount;
} JVX_RW_MUTEX_HANDLE;

#define JVX_INITIALIZE_RW_MUTEX(a) jvx_initialize_rw_mutex_l(&a)
static void jvx_initialize_rw_mutex_l(JVX_RW_MUTEX_HANDLE* a)
{
    InitializeCriticalSection(&(a->readerCountLock));
    InitializeCriticalSection(&(a->writerLock));

    /*
     * We use a manual-reset event as poor man condition variable that
     * can only do broadcast.  Actually, only one thread will be waiting
     * on this at any time, because the wait is done while holding the
     * writerLock.
     */
    a->noReaders = CreateEvent (NULL, TRUE, TRUE, NULL);
	a->readerCount = 0;
}

#define JVX_TERMINATE_RW_MUTEX(a) jvx_terminate_rw_mutex_l(&a)
static void jvx_terminate_rw_mutex_l(JVX_RW_MUTEX_HANDLE* a)
{
    DeleteCriticalSection(&(a->readerCountLock));
    DeleteCriticalSection(&(a->writerLock));

    CloseHandle(a->noReaders);
	a->readerCount = 0;
}

#define JVX_LOCK_RW_MUTEX_EXCLUSIVE(a) jvx_lock_rw_mutex_exclusive_l(&a)
static void jvx_lock_rw_mutex_exclusive_l(JVX_RW_MUTEX_HANDLE* a)
{
	EnterCriticalSection(&(a->writerLock));
    if (a->readerCount > 0)
	{
        WaitForSingleObject(a->noReaders, INFINITE);
    }

    /* writerLock remains locked.  */
}

#define JVX_LOCK_RW_MUTEX_SHARED(a) jvx_lock_rw_mutex_shared_l(&a)
static void jvx_lock_rw_mutex_shared_l(JVX_RW_MUTEX_HANDLE* a)
{
	/*
     * We need to lock the writerLock too, otherwise a writer could
     * do the whole of rwlock_wrlock after the readerCount changed
     * from 0 to 1, but before the event was reset.
     */
    EnterCriticalSection(&a->writerLock);
    EnterCriticalSection(&a->readerCountLock);
    if (++ (a->readerCount) == 1)
	{
        ResetEvent(a->noReaders);
    }
    LeaveCriticalSection(&a->readerCountLock);
	LeaveCriticalSection(&a->writerLock);
}

#define JVX_UNLOCK_RW_MUTEX_EXCLUSIVE( a) jvx_unlock_rw_mutex_exclusive_l(&a)
static void jvx_unlock_rw_mutex_exclusive_l(JVX_RW_MUTEX_HANDLE* a)
{
	LeaveCriticalSection(&(a->writerLock));
}


#define JVX_UNLOCK_RW_MUTEX_SHARED(a) jvx_unlock_rw_mutex_shared_l(&a)
static void jvx_unlock_rw_mutex_shared_l(JVX_RW_MUTEX_HANDLE* a)
{
	EnterCriticalSection(&(a->readerCountLock));
    assert (a->readerCount > 0);
    if (--(a->readerCount) == 0)
	{
        SetEvent(a->noReaders);
    }
    LeaveCriticalSection((&a->readerCountLock));
}

#define JVX_TRY_LOCK_RW_MUTEX_EXCLUSIVE( res,  hdl) res = jvx_try_lock_rw_mutex_exclusive_l(&hdl)
static JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE jvx_try_lock_rw_mutex_exclusive_l(JVX_RW_MUTEX_HANDLE* b)
{
	JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE res = c_false;
	if(TryEnterCriticalSection(&(b->writerLock)) == TRUE)
	{
		if (b->readerCount == 0)
		{
			res = c_true;
		}
		else
		{
			LeaveCriticalSection(&(b->writerLock));
			res = c_false;
		}
    }
	else
	{
		// Lock has not been acquired
		res = c_false;
	}
	return res;
}

#define JVX_TRY_LOCK_RW_MUTEX_SHARED(a, b) a = jvx_try_lock_rw_mutex_shared_l(&b)
static JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE jvx_try_lock_rw_mutex_shared_l(JVX_RW_MUTEX_HANDLE* b)
{
	JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE res = c_false;
	if(TryEnterCriticalSection(&(b->writerLock)))
	{
	    if(TryEnterCriticalSection(&(b->readerCountLock)))
		{
			if (++(b->readerCount) == 1)
			{
				ResetEvent(b->noReaders);
			}
			LeaveCriticalSection(&(b->readerCountLock));
			LeaveCriticalSection(&(b->writerLock));
			res = true;
		}
		else
		{
			LeaveCriticalSection(&(b->writerLock));
			res = false;
		}
	}
	else
	{
		res = false;
	}
	return res;
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

#endif
