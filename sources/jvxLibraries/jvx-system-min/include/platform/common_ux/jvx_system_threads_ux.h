#ifndef _JVX_SYSTEM_THREADS_H__
#define _JVX_SYSTEM_THREADS_H__

#define JVX_THREAD_MIN(a, b) (a<b ? a:b)
#define JVX_THREAD_POLICY_RT SCHED_FIFO
#define JVX_THREAD_PRIO_RT_PREEMPT_MAX 49

#define JVX_THREAD_ID pthread_t

#define JVX_THREAD_HANDLE pthread_t
#define JVX_NULLTHREAD ((pthread_t)0)
#define JVX_THREAD_EXIT_CODE void*
#define JVX_THREAD_EXIT_NORMAL NULL
#define JVX_THREAD_EXIT_ERROR (void*)-1
#define JVX_THREAD_PRIORITY int
#define JVX_THREAD_PRIORITY_REALTIME -19
#define JVX_THREAD_PRIORITY_NORMAL 0
JVX_STATIC_INLINE void JVX_SET_THREAD_PRIORITY(pthread_t a, int b)
{
#if defined(JVX_OS_EMSCRIPTEN)
	assert(0);
#else
	int policy;
	int prio;
	int suc;
	struct sched_param parameters;

	struct utsname u;
    	int crit1, crit2 = 0;
	FILE *fd;

    	uname(&u);
	crit1 = (strcasestr (u.version, "PREEMPT RT") != NULL);

    	if ((fd = fopen("/sys/kernel/realtime","r")) != NULL) 
	{
	        int flag;
	        crit2 = ((fscanf(fd, "%d", &flag) == 1) && (flag == 1));
	        fclose(fd);
	}


	suc = pthread_getschedparam(a, &policy, &parameters);
	if(suc < 0)
	{
	    printf("xxx>> Failed to get thread priority parameters\n");
	}
	else
	{
		if (b == JVX_THREAD_PRIORITY_REALTIME)
		{
			policy = JVX_THREAD_POLICY_RT;
			prio = sched_get_priority_max(policy);
	        	if(crit1 && crit2)
			{
			    printf( "xxx> Running on a PREEMPT_RT kernel!\n");
				prio = JVX_THREAD_MIN(JVX_THREAD_PRIO_RT_PREEMPT_MAX, prio);
			}
			printf("xxx>>> Set scheduler type and priority to %i and prio to %i.\n", policy, prio);
			parameters.sched_priority = prio;
		}

		suc = pthread_setschedparam(a, policy, &parameters);
		if(suc < 0)
		{
		    printf("xxx>>> Failed to set thread priority parameters.\n");
		}
	}
#endif
}

JVX_STATIC_INLINE JVX_THREAD_PRIORITY JVX_GET_THREAD_PRIORITY(pthread_t a)
{
	int policy;
	struct sched_param parameters;
	pthread_getschedparam(a, &policy, &parameters);

    return parameters.sched_priority;
}

#define JVX_RETURN_THREAD(a) return((void*)((long long)a))
#define JVX_THREAD_ID_INVALID 0
#define JVX_GET_CURRENT_THREAD_ID pthread_self
#define JVX_GET_CURRENT_THREAD_HANDLE pthread_self
#define JVX_THREAD_RETURN_TYPE void*
#define JVX_THREAD_ENTRY_FUNCTION(a,b) static JVX_THREAD_RETURN_TYPE a(void* b)
#define JVX_THREAD_ID_UINT32(tid) (jvxUInt32)((uintptr_t)tid)

#ifdef JVX_RT_THREAD_CREATION
static void JVX_CREATE_THREAD(pthread_t& thread, void *(*start_routine) (void *), void *arg, pthread_t& threadid)
{
   	pthread_attr_t attr;
   
   	/* init to default values */
   	if (pthread_attr_init(&attr))
   	{
		assert(0);
	}
   	/* Set the requested stacksize for this thread */
   	if (pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN + JVX_MY_STACK_SIZE))
   	{
		assert(0);
	}
   	/* And finally start the actual thread */
   	pthread_create(&thread, &attr, start_routine, arg);
	threadid = thread;
}
#else
#define JVX_CREATE_THREAD(a, b, c, d) pthread_create(&a,NULL,b, c); d = a;
#endif

#if defined( JVX_OS_LINUX) 

static int jvx_pthread_join(pthread_t thread, void** value_ptr, jvxSize timeoutmsec)
{
	struct timespec tt;
	//timeval tp;

	// Here, we need to use CLOCK_REALTIME since pthread_timedjoin_np also uses CLOCK_REALTIME.
	// I did not find a way to habe pthread_timedjoin_np wait for ANOTHER clock source
	//int res = clock_gettime(JVX_GLOBAL_CLOCK_SOURCE/*CLOCK_MONOTONIC*//*CLOCK_PROCESS_CPUTIME_ID*/, &tt);
	int res = clock_gettime(CLOCK_REALTIME/*CLOCK_MONOTONIC*//*CLOCK_PROCESS_CPUTIME_ID*/, &tt);
 
	jvxInt64 sec = (jvxInt64)tt.tv_sec;
	jvxInt64 nsec = (jvxInt64)tt.tv_nsec + (jvxInt64)1000000 * (jvxInt64)timeoutmsec;

	sec += nsec/1000000000;
	nsec = nsec%1000000000;

	/* Convert from timeval to timespec */
	tt.tv_sec  = sec;
	tt.tv_nsec = nsec;
	
	res = pthread_timedjoin_np(thread, value_ptr, &tt);
	
	return(res);
}
#else
	
#if defined( JVX_OS_MACOSX) || defined(JVX_OS_IOS) || defined(JVX_OS_EMSCRIPTEN) || defined(JVX_OS_ANDROID)


// Following taken from here:
// https://stackoverflow.com/questions/11551188/alternative-to-pthread-timedjoin-np

struct args {
    int joined;
    pthread_t td;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    void **res;
};

JVX_STATIC_INLINE void *waiter(void *ap)
{
  struct args *args = (struct args *)ap;
    pthread_join(args->td, args->res);
    pthread_mutex_lock(&args->mtx);
    args->joined = 1;
    pthread_mutex_unlock(&args->mtx);
    pthread_cond_signal(&args->cond);
    return 0;
}

JVX_STATIC_INLINE int pthread_timedjoin_np(pthread_t td, void **res, struct timespec *ts)
{
    pthread_t tmp;
    int ret;
    struct args args = { .td = td, .res = res };

    pthread_mutex_init(&args.mtx, 0);
    pthread_cond_init(&args.cond, 0);
    pthread_mutex_lock(&args.mtx);

    ret = pthread_create(&tmp, 0, waiter, &args);
    assert(ret == 0);

    do ret = pthread_cond_timedwait(&args.cond, &args.mtx, ts);
    while (!args.joined && ret != ETIMEDOUT);

    pthread_mutex_unlock(&args.mtx);

#if defined(JVX_OS_ANDROID)
	
	// Could not find a good replacement for this function in Android,
	// https://stackoverflow.com/questions/4610086/pthread-cancel-alternatives-in-android-ndk
	
	assert(0);
#else
    pthread_cancel(tmp);
#endif

    pthread_join(tmp, 0);

    pthread_cond_destroy(&args.cond);
    pthread_mutex_destroy(&args.mtx);

    return args.joined ? 0 : ret;
}

JVX_STATIC_INLINE int jvx_pthread_join(pthread_t thread, void** value_ptr, jvxSize timeoutmsec)
{
	struct timespec tt;
	struct timeval tp;
#if defined(JVX_OS_EMSCRIPTEN)
	assert(0);
	int res = clock_gettime(CLOCK_REALTIME/*CLOCK_MONOTONIC*//*CLOCK_PROCESS_CPUTIME_ID*/, &tt);
#else

	int res = gettimeofday(&tp, NULL);
#endif

	/* Convert from timeval to timespec */
	tt.tv_sec = tp.tv_sec;
	tt.tv_nsec = tp.tv_usec * 1000;
	tt.tv_sec += timeoutmsec;

	res = pthread_timedjoin_np(thread, value_ptr, &tt);
	return(res);
}
#else
	
#error Ended up here for an unknown OS type

#endif
#endif

#define JVX_WAIT_FOR_THREAD_TERMINATE_INF(a) pthread_join(a, NULL)
#define JVX_WAIT_FOR_THREAD_TERMINATE_MS(a,b) jvx_pthread_join(a, NULL, b)

#if defined(JVX_OS_ANDROID)

	// Could not find a good replacement for this function in Android,
	// https://stackoverflow.com/questions/4610086/pthread-cancel-alternatives-in-android-ndk
	#define JVX_TERMINATE_THREAD(arg1, arg2) assert(0)
#else

	#define JVX_TERMINATE_THREAD(arg1, arg2) pthread_cancel(arg1)
	
#endif

#define JVX_COMPARE_THREADS(a,b) (pthread_equal(a,b) != 0)

typedef struct
{
    pthread_cond_t cond;
    pthread_mutex_t mutex;
#ifdef JVX_DEBUG_THREADS
    JVX_THREAD_ID tId;
#endif
    int condSet;
} JVX_NOTIFY_HANDLE;

#define JVX_NOTIFY_HANDLE_DECLARE(notification) JVX_NOTIFY_HANDLE notification

#define JVX_INITIALIZE_NOTIFICATION(hdl)  jvx_initialize_notification_l(&hdl)

#if defined(JVX_OS_LINUX) || defined(JVX_OS_EMSCRIPTEN) || defined(JVX_OS_ANDROID)

/* ============= LINUX ================*/
JVX_STATIC_INLINE int jvx_initialize_notification_l(JVX_NOTIFY_HANDLE* hdl)
{
	pthread_mutexattr_t obj;
	pthread_mutexattr_init(&obj);
	pthread_mutexattr_settype(&obj, PTHREAD_MUTEX_RECURSIVE);
	hdl->condSet = 0;
	int res1 = pthread_mutex_init(&hdl->mutex, &obj);
	pthread_mutexattr_destroy(&obj);

	pthread_condattr_t cattr;
	pthread_condattr_init(&cattr);
	pthread_condattr_setclock(&cattr, JVX_GLOBAL_CLOCK_SOURCE);
	
	int res2 = pthread_cond_init(&hdl->cond, &cattr);
	pthread_condattr_destroy(&cattr);
	
	return(res1 && res2);
}
#else
	
#if defined (JVX_OS_MACOSX) || defined(JVX_OS_IOS)

/* ===== MACOSX ===============*/
JVX_STATIC_INLINE int jvx_initialize_notification_l(JVX_NOTIFY_HANDLE* hdl)
{
	pthread_mutexattr_t obj;
	pthread_mutexattr_init(&obj);
	pthread_mutexattr_settype(&obj, PTHREAD_MUTEX_RECURSIVE);
	hdl->condSet = 0;
	int res1 = pthread_mutex_init(&hdl->mutex, &obj);
	pthread_mutexattr_destroy(&obj);
	int res2 = pthread_cond_init(&hdl->cond, NULL);
	return(res1 && res2);
}

#else
	
#error Ended up here for an unknown OS type

#endif
#endif

#define JVX_TERMINATE_NOTIFICATION(hdl) jvx_terminate_notification_l(&hdl)
JVX_STATIC_INLINE int jvx_terminate_notification_l(JVX_NOTIFY_HANDLE* hdl)
{
	int res1 = pthread_cond_destroy(&hdl->cond);
	int res2 = pthread_mutex_destroy(&hdl->mutex);
	return(res1 && res2);
}

#define JVX_WAIT_FOR_NOTIFICATION_I(hdl) jvx_wait_for_notification_i_l(&hdl)
JVX_STATIC_INLINE void jvx_wait_for_notification_i_l(JVX_NOTIFY_HANDLE* hdl)
{
    // This function will not be able to detect any retrigger event in Linux
    // since it does not check for it
    pthread_mutex_lock(&hdl->mutex);
    //assert(hdl.condSet == 0);
#ifdef JVX_DEBUG_THREADS
    hdl->tId = JVX_GET_CURRENT_THREAD_ID();
#endif

}

#define JVX_WAIT_FOR_CHECK_RETRIGGER_NOTIFICATION_I( hdl) jvx_wait_for_check_retrigger_notification_i_l(&hdl)
JVX_STATIC_INLINE int jvx_wait_for_check_retrigger_notification_i_l(JVX_NOTIFY_HANDLE* hdl)
{
    // We lock mutex here and check condition. If condition is 0,
    // there was no incoming event inbetween. We return 0 but KEEP the
    // mutex locked. A JVX_WAIT_FOR_NOTIFICATION_II.. must follow to release the
    // mutex!!!
    pthread_mutex_lock(&hdl->mutex);
    if(hdl->condSet == 0)
    {
#ifdef JVX_DEBUG_THREADS
	hdl->tId = JVX_GET_CURRENT_THREAD_ID();
#endif
	return 0;
    }

    // If condifition was met, unlock mutex and use return value to jump back to processing
    // After this function, JVX_WAIT_FOR_NOTIFICATION_II.. MUST not be set as next since mutex is not held
    // We need to reset condition however, otherwise, it will never be cleared again!
    hdl->condSet = 0;
    pthread_mutex_unlock(&hdl->mutex);
    return 1;
}
	
	
#define JVX_CHECK_RETRIGGER_NOTIFICATION( hdl) jvx_check_retrigger_notification_i_l(&hdl)
JVX_STATIC_INLINE int jvx_check_retrigger_notification_i_l(JVX_NOTIFY_HANDLE* hdl)
{
	int retVal = 0;
    // We lock mutex here and check condition. If condition is 0,
    // there was no incoming event inbetween. We return 0 but KEEP the
    // mutex locked. A JVX_WAIT_FOR_NOTIFICATION_II.. must follow to release the
    // mutex!!!
    pthread_mutex_lock(&hdl->mutex);
    if(hdl->condSet == 0)
    {
#ifdef JVX_DEBUG_THREADS
		hdl->tId = JVX_GET_CURRENT_THREAD_ID();
#endif
		retVal = 0;
    }
	else
	{
		retVal = 1;
		
		// If condifition was met, unlock mutex and use return value to jump back to processing
		// After this function, JVX_WAIT_FOR_NOTIFICATION_II.. MUST not be set as next since mutex is not held
		// We need to reset condition however, otherwise, it will never be cleared again!
		hdl->condSet = 0;
	}
    pthread_mutex_unlock(&hdl->mutex);
    return retVal;
}
	
	
#define JVX_WAIT_FOR_NOTIFICATION_II_INF(hdl) jvx_wait_for_notification_ii_inf_l(&hdl)
JVX_STATIC_INLINE int jvx_wait_for_notification_ii_inf_l(JVX_NOTIFY_HANDLE* hdl)
{
    int res = 0;

#ifdef JVX_DEBUG_THREADS
	JVX_THREAD_ID tId = JVX_GET_CURRENT_THREAD_ID();
	assert(tId == hdl->tId);
#endif

    // This while loop is due to some spurius wakeup which may occur accoridng to here:
    // https://en.wikipedia.org/wiki/Spurious_wakeup
    while(!hdl->condSet)
    {
	res = pthread_cond_wait(&hdl->cond, &hdl->mutex);
    }
    hdl->condSet = 0;
    pthread_mutex_unlock(&hdl->mutex);
    return(res);
}

#define JVX_WAIT_FOR_NOTIFICATION_II_CLEAR(hdl) jvx_wait_for_notification_ii_clear_l(&hdl)
JVX_STATIC_INLINE int jvx_wait_for_notification_ii_clear_l(JVX_NOTIFY_HANDLE* hdl)
{
    // This function just unlocks the lock as acquired in part I of the function if we do NOT use WAIT part II
    // This function was required in the case of a 0-timeout thread behavior (only loop and call wokeup)
    int res = 0;
    hdl->condSet = 0;
    pthread_mutex_unlock(&hdl->mutex);
    return(res);
}

#define JVX_WAIT_FOR_NOTIFICATION_II_MS(hdl, timeoutmsec) jvx_wait_for_notification_ii_ms_l(&hdl, timeoutmsec)

#if defined (JVX_OS_LINUX) || defined(JVX_OS_EMSCRIPTEN) || defined(JVX_OS_ANDROID)

JVX_STATIC_INLINE int jvx_wait_for_notification_ii_ms_l(JVX_NOTIFY_HANDLE* hdl, jvxSize timeoutmsec)
{
	struct timespec tt;
	// struct timeval tp;

	int res = clock_gettime(JVX_GLOBAL_CLOCK_SOURCE/*CLOCK_MONOTONIC*//*CLOCK_PROCESS_CPUTIME_ID*/, &tt);

	jvxInt64 sec = (jvxInt64)tt.tv_sec;
	jvxInt64 nsec = (jvxInt64)tt.tv_nsec + (jvxInt64)1000000 * (jvxInt64)timeoutmsec;

#ifdef JVX_DEBUG_THREADS
	JVX_THREAD_ID tId = JVX_GET_CURRENT_THREAD_ID();
	assert(tId == hdl->tId);
#endif

	sec += nsec/1000000000;
	nsec = nsec%1000000000;

	/* Convert from timeval to timespec */
	tt.tv_sec  = sec;
	tt.tv_nsec = nsec;

	assert(res == 0);
	res = 0;
	
	// This while loop is due to some spurius wakeup which may occur accoridng to here:
	// https://en.wikipedia.org/wiki/Spurious_wakeup
	while(!hdl->condSet)
	{
        //printf("Hier %i", timeoutmsec);
	    res = pthread_cond_timedwait(&hdl->cond, &hdl->mutex, &tt);
	    if(res == ETIMEDOUT)
	    {
	    	hdl->condSet = 1;
	    }
	}
	hdl->condSet = 0;
	pthread_mutex_unlock(&hdl->mutex);
	return(res);
}

#define JVX_NOTIFY_HANDLE_2WAIT_DECLARE(hdl) int hdl
#define JVX_INVALID_HANDLE_VALUE_2WAIT 0

#if defined(JVX_OS_EMSCRIPTEN)

// Not supported in emscripten - do not know what to do yet
#define JVX_INITIALIZE_NOTIFICATION_2WAIT(hdlStop) assert(0);
#define JVX_SET_NOTIFICATION_2WAIT(hdlStop) assert(0);

#else
	
#define JVX_INITIALIZE_NOTIFICATION_2WAIT(hdlStop) hdlStop = eventfd(0, EFD_NONBLOCK);
#define JVX_SET_NOTIFICATION_2WAIT(hdlStop) eventfd_write(hdlStop, 1);

#endif

#define JVX_TERMINATE_NOTIFICATION_2WAIT(hdlStop) close(hdlStop);

#define JVX_EVENT_WAIT_2WAIT_RESULT_TYPE int
#define JVX_EVENT_WAIT_2WAIT_RESULT_SUCCESS 0
#define JVX_EVENT_WAIT_2WAIT(epfd, monitored, nummonitored, triggered, numtriggered, timeout_msec) \
	epoll_wait(epfd, triggered, numtriggered, timeout_msec)


#else
	
#if defined( JVX_OS_MACOSX) || defined(JVX_OS_IOS)

JVX_STATIC_INLINE int jvx_wait_for_notification_ii_ms_l(JVX_NOTIFY_HANDLE* hdl, jvxSize timeoutmsec)
{
	struct timespec tt;
	struct timeval tp;

	int res = gettimeofday(&tp, NULL);

	/* Convert from timeval to timespec */
	tt.tv_sec = tp.tv_sec;
	tt.tv_nsec = tp.tv_usec * 1000;
		
	jvxInt64 sec = (jvxInt64)tt.tv_sec;
	jvxInt64 nsec = (jvxInt64)tt.tv_nsec + (jvxInt64)1000000 * (jvxInt64)timeoutmsec;

#ifdef JVX_DEBUG_THREADS
	JVX_THREAD_ID tId = JVX_GET_CURRENT_THREAD_ID();
	assert(tId == hdl->tId);
#endif

	sec += nsec/1000000000;
	nsec = nsec%1000000000;

	/* Convert from timeval to timespec */
	tt.tv_sec  = sec;
	tt.tv_nsec = nsec;

	assert(res == 0);
	res = 0;
	
	// This while loop is due to some spurius wakeup which may occur accoridng to here:
	// https://en.wikipedia.org/wiki/Spurious_wakeup
	while(!hdl->condSet)
	{
        //printf("Hier %i", timeoutmsec);
	    res = pthread_cond_timedwait(&hdl->cond, &hdl->mutex, &tt);
	    if(res == ETIMEDOUT)
	    {
	    	hdl->condSet = 1;
	    }
	}
	hdl->condSet = 0;
	pthread_mutex_unlock(&hdl->mutex);
	return(res);
}

/*
 Let us try a pipe for end-signaling
 
 https://austingwalters.com/io-multiplexing/
 https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/pipe.2.html#//apple_ref/doc/man/2/pipe
 https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/write.2.html

 https://wiki.netbsd.org/tutorials/kqueue_tutorial/#index3h2
 */

#define JVX_NOTIFY_HANDLE_2WAIT_DECLARE(hdlStop) int hdlStop[2]
#define JVX_INVALID_HANDLE_VALUE_2WAIT {0}
#define JVX_INITIALIZE_NOTIFICATION_2WAIT(hdlStop) pipe(hdlStop)
#define JVX_SET_NOTIFICATION_2WAIT(hdlStop) {int wme = 1; write(hdlStop[1], &wme, sizeof(int));}
#define JVX_TERMINATE_NOTIFICATION_2WAIT(hdlStop) close(hdlStop[0]); close(hdlStop[1])
#define JVX_EVENT_WAIT_2WAIT(kqfd, monitored, nummonitored, triggered, numtriggered, timeout_msec) \
	kevent(kqfd, monitored, nummonitored, triggered, numtriggered, NULL)
#else
	
#error Ended up here for an unknown OS type

#endif
#endif

#define JVX_WAIT_FOR_NOTIFICATION_II_S(hdl, timeoutSec) jvx_wait_for_notification_ii_s_l(&hdl, timeoutSec)
JVX_STATIC_INLINE int jvx_wait_for_notification_ii_s_l(JVX_NOTIFY_HANDLE* hdl, jvxSize timeoutSec)
{
    return jvx_wait_for_notification_ii_ms_l(hdl, timeoutSec*1000);
}

#define JVX_WAIT_SUCCESS 0
#define JVX_WAIT_RESULT int
#define JVX_WAIT_TIMEOUT int
#define JVX_INFINITE_MS JVX_SIZE_UNSELECTED

JVX_STATIC_INLINE void JVX_SLEEP_MS(unsigned int ms)
{
    usleep(ms*1000);
}

JVX_STATIC_INLINE void JVX_SLEEP_S(unsigned int s)
{
    sleep(s);
}

#define JVX_SET_NOTIFICATION(hdl) jvx_set_notification_l(&hdl)
JVX_STATIC_INLINE int jvx_set_notification_l(JVX_NOTIFY_HANDLE* hdl)
{
	pthread_mutex_lock(&hdl->mutex);
	hdl->condSet = 1;
	pthread_cond_signal(&hdl->cond);
	pthread_mutex_unlock(&hdl->mutex);
	return(0);
}

#define JVX_RESET_NOTIFICATION(a)

#endif
