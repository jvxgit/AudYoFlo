#ifndef _JVX_SYSTEM_MUTEX_H__
#define _JVX_SYSTEM_MUTEX_H__

#define JVX_MUTEX_HANDLE pthread_mutex_t

#define JVX_LOCK_MUTEX(a) pthread_mutex_lock(&a)

#define JVX_TRY_LOCK_MUTEX(a,b) a = pthread_mutex_trylock(&b)

#define JVX_TRY_LOCK_MUTEX_RESULT_TYPE int

#define JVX_TRY_LOCK_MUTEX_SUCCESS 0

#define JVX_TRY_LOCK_MUTEX_NO_SUCCESS -1

#define JVX_TRY_LOCK_MUTEX_TEST_SUCCESS(var) var == JVX_TRY_LOCK_MUTEX_SUCCESS
#define JVX_TRY_LOCK_MUTEX_TEST_NO_SUCCESS(var) var != JVX_TRY_LOCK_MUTEX_SUCCES

#define JVX_UNLOCK_MUTEX(a) pthread_mutex_unlock(&a)

#define JVX_INITIALIZE_MUTEX(a) { pthread_mutexattr_t obj; pthread_mutexattr_init(&obj); pthread_mutexattr_settype(&obj, PTHREAD_MUTEX_RECURSIVE); pthread_mutex_init(&a, &obj); pthread_mutexattr_destroy(&obj);}

#define JVX_TERMINATE_MUTEX(a) pthread_mutex_destroy(&a)


#define JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE int

#define JVX_TRY_LOCK_RW_MUTEX_SUCCESS 0

#define JVX_RW_MUTEX_HANDLE pthread_rwlock_t

#define JVX_INITIALIZE_RW_MUTEX(a) {pthread_rwlockattr_t obj; pthread_rwlockattr_init(&obj); /*pthread_rwlockattr_settype(&obj, PTHREAD_RWLOCK_RECURSIVE); */pthread_rwlock_init(&a, &obj);pthread_rwlockattr_destroy(&obj);}

#define JVX_TERMINATE_RW_MUTEX(a) pthread_rwlock_destroy(&a)
//TerminateSRWLock(&a)

#define JVX_LOCK_RW_MUTEX_EXCLUSIVE(a) pthread_rwlock_wrlock(&a)

#define JVX_LOCK_RW_MUTEX_SHARED(a) pthread_rwlock_rdlock(&a)

#define JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(a) pthread_rwlock_unlock(&a)

#define JVX_UNLOCK_RW_MUTEX_SHARED(a) pthread_rwlock_unlock(&a)

#define JVX_TRY_LOCK_RW_MUTEX_EXCLUSIVE(a,b) a = pthread_rwlock_trywrlock(&b)

#define JVX_TRY_LOCK_RW_MUTEX_SHARED(a,b) a = pthread_rwlock_tryrdlock(&b)

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

#define JVX_INTERLOCKED_COMPARE_EXCHANGE_16(dest, exch, comp) __sync_val_compare_and_swap(dest, comp, exch)
// type __sync_val_compare_and_swap (type *ptr, type oldval type newval, ...)

#define JVX_INTERLOCKED_EXCHANGE_16(target, value) __sync_lock_test_and_set(target, value)
// type __sync_lock_test_and_set (type *ptr, type value, ...)

#define JVX_INTERLOCKED_EXCHANGE_ADD_64(target, value) __sync_fetch_and_add(target, value) 
// __sync_fetch_and_add 
// ==============================================================

#endif
