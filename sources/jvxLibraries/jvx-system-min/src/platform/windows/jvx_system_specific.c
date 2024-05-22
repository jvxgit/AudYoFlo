#include "jvx_system.h"

jvxCBool JVX_FILE_EXISTS(const char* fpath)
{
	DWORD dwAttrib = GetFileAttributes(fpath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

/*
jvxCBool JVX_FOLDER_EXISTS(const char* fpath)
{
	DWORD dwAttrib = GetFileAttributes(fpath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
*/

jvxCBool JVX_FILE_REMOVE(const char* fName)
{
	if (JVX_FILE_EXISTS(fName))
	{
		DeleteFile(fName);
		return c_true;
	}
	return c_false;
}

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

jvxCBool JVX_DIRECTORY_EXISTS(const char* absolutePath)
{
	if (_access_s(absolutePath, 0) == 0)
	{
		struct _stat status;
		_stat(absolutePath, &status);
		return (status.st_mode & S_IFDIR) != 0;
	}

	return false;
}

#ifdef OLDER_THAN_WINDOWS7
void
jvx_initialize_rw_mutex_l(JVX_RW_MUTEX_HANDLE* a)
{
	InitializeCriticalSection(&(a->readerCountLock));
	InitializeCriticalSection(&(a->writerLock));

	/*
	 * We use a manual-reset event as poor man condition variable that
	 * can only do broadcast.  Actually, only one thread will be waiting
	 * on this at any time, because the wait is done while holding the
	 * writerLock.
	 */
	a->noReaders = CreateEvent(NULL, TRUE, TRUE, NULL);
	a->readerCount = 0;
}

void
jvx_terminate_rw_mutex_l(JVX_RW_MUTEX_HANDLE* a)
{
	DeleteCriticalSection(&(a->readerCountLock));
	DeleteCriticalSection(&(a->writerLock));

	CloseHandle(a->noReaders);
	a->readerCount = 0;
}

JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE
jvx_try_lock_rw_mutex_shared_l(JVX_RW_MUTEX_HANDLE* b)
{
	JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE res = c_false;
	if (TryEnterCriticalSection(&(b->writerLock)))
	{
		if (TryEnterCriticalSection(&(b->readerCountLock)))
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

void
jvx_lock_rw_mutex_exclusive_l(JVX_RW_MUTEX_HANDLE* a)
{
	EnterCriticalSection(&(a->writerLock));
	if (a->readerCount > 0)
	{
		WaitForSingleObject(a->noReaders, INFINITE);
	}

	/* writerLock remains locked.  */
}

void
jvx_lock_rw_mutex_shared_l(JVX_RW_MUTEX_HANDLE* a)
{
	/*
	 * We need to lock the writerLock too, otherwise a writer could
	 * do the whole of rwlock_wrlock after the readerCount changed
	 * from 0 to 1, but before the event was reset.
	 */
	EnterCriticalSection(&a->writerLock);
	EnterCriticalSection(&a->readerCountLock);
	if (++(a->readerCount) == 1)
	{
		ResetEvent(a->noReaders);
	}
	LeaveCriticalSection(&a->readerCountLock);
	LeaveCriticalSection(&a->writerLock);
}

void
jvx_unlock_rw_mutex_exclusive_l(JVX_RW_MUTEX_HANDLE* a)
{
	LeaveCriticalSection(&(a->writerLock));
}

void
jvx_unlock_rw_mutex_shared_l(JVX_RW_MUTEX_HANDLE* a)
{
	EnterCriticalSection(&(a->readerCountLock));
	assert(a->readerCount > 0);
	if (--(a->readerCount) == 0)
	{
		SetEvent(a->noReaders);
	}
	LeaveCriticalSection((&a->readerCountLock));
}

JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE
jvx_try_lock_rw_mutex_exclusive_l(JVX_RW_MUTEX_HANDLE* b)
{
	JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE res = c_false;
	if (TryEnterCriticalSection(&(b->writerLock)) == TRUE)
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
#endif