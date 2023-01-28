#ifndef __JVX_THREADS_H__
#define __JVX_THREADS_H__

#include "jvx.h"

typedef struct
{
	jvxErrorType (*callback_thread_startup)(jvxHandle* privateData_thread, jvxInt64 timestamp_us);
	jvxErrorType (*callback_thread_timer_expired)(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms);
	jvxErrorType (*callback_thread_wokeup)(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms);
	jvxErrorType (*callback_thread_stopped)(jvxHandle* privateData_thread, jvxInt64 timestamp_us);
} jvx_thread_handler;

jvxErrorType jvx_thread_initialize(jvxHandle** hdlOnReturn, jvx_thread_handler* report_callback, jvxHandle* privData, jvxBool report_timestamp, jvxBool noblocking = false, jvxBool asyncProcessing = false);

jvxErrorType jvx_thread_start(jvxHandle* hdlEnter, jvxSize timeout_ms);

jvxErrorType jvx_thread_id(jvxHandle* hdlEnter, JVX_THREAD_ID* t_id);

jvxErrorType jvx_threads_get_time(jvxHandle* hdlEnter, jvxInt64* myTime);

// Functions to be executed while thread is active
jvxErrorType  jvx_thread_set_priority(jvxHandle* hdlEnter, JVX_THREAD_PRIORITY prio);
jvxErrorType jvx_thread_reconfigure(jvxHandle* hdlEnter, jvxSize timeout_ms);
jvxErrorType jvx_thread_wakeup(jvxHandle* hdlEnter, jvxBool trigger_even_if_busy = true);

jvxErrorType jvx_thread_test_complete(jvxHandle* hdlEnter, jvxBool* ready_for_next_trigger);

jvxErrorType jvx_thread_stop(jvxHandle* hdlEnter, jvxSize timeout_msec = JVX_INFINITE_MS);

jvxErrorType jvx_thread_trigger_stop(jvxHandle* hdlEnter);
jvxErrorType jvx_thread_wait_stop(jvxHandle* hdlEnter, jvxSize timeout_ms);

jvxErrorType jvx_thread_terminate(jvxHandle* hdlLeave);

// =================================================================================
#define JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_FUNC(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData_thread, jvxInt64 timestamp_us); \
	jvxErrorType ic_callback_ ## fname(jvxInt64 timestamp_us);

#define JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData_thread, jvxInt64 timestamp_us) \
	{ \
		cname* this_pointer = (cname*)privateData_thread; \
		return(this_pointer->ic_callback_ ## fname(timestamp_us)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvxInt64 timestamp_us) 

#define JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_FUNC_IP(fname) \
	static jvxErrorType callback_ ## fname(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms); \
	jvxErrorType ic_callback_ ## fname(jvxInt64 timestamp_us, jvxSize* delta_ms);

#define JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_IP(cname, fname) \
	jvxErrorType \
	cname::callback_ ## fname(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms) \
	{ \
		cname* this_pointer = (cname*)privateData_thread; \
		return(this_pointer->ic_callback_ ## fname(timestamp_us, delta_ms)); \
	} \
	jvxErrorType \
	cname::ic_callback_ ## fname(jvxInt64 timestamp_us, jvxSize* delta_ms) 

#define JVX_THREADS_FORWARD_C_CALLBACK_ASSIGN(var, cname, fname_started, fname_stopped, fname_expired, fname_wokeup) \
	var.callback_thread_startup = cname::callback_ ## fname_started; \
	var.callback_thread_stopped = cname::callback_ ## fname_stopped; \
	var.callback_thread_timer_expired = cname::callback_ ## fname_expired; \
	var.callback_thread_wokeup = cname::callback_ ## fname_wokeup; 

#define JVX_THREADS_FORWARD_C_CALLBACK_ASSIGN_D_IC(var) \
	var.callback_thread_startup = callback_ ## started; \
	var.callback_thread_stopped = callback_ ## stopped; \
	var.callback_thread_timer_expired = callback_ ## expired; \
	var.callback_thread_wokeup = callback_ ## wokeup; 

#define JVX_THREADS_FORWARD_C_CALLBACK_RESET(var) \
	var.callback_thread_startup = NULL; \
	var.callback_thread_stopped = NULL; \
	var.callback_thread_timer_expired = NULL; \
	var.callback_thread_wokeup = NULL; 

#define JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_ALL \
	JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_FUNC(started) \
	JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_FUNC_IP(expired) \
	JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_FUNC(stopped) \
	JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_FUNC_IP(wokeup) \


#endif
