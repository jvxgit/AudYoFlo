#include "jvx.h"
#include "CjvxThreads.h"


/**
 * Constructor: Set all into uninitialized.
 *///===================================================
CjvxThreads::CjvxThreads(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_THREADS);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxThreads*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	JVX_THREADS_FORWARD_C_CALLBACK_ASSIGN_D_IC(thread.cbs);

	rep = NULL;
	thread.hdl = NULL;
}

CjvxThreads::~CjvxThreads()
{
	terminate();
}

	//! Initialize the module, set size of buffer and strategy
jvxErrorType 
CjvxThreads::initialize(IjvxThreads_report* bwdReport)
{
	rep = bwdReport;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxThreads::start(jvxSize timeout, jvxBool push_prio, jvxBool rep_tstamp, jvxBool nonblock, jvxBool async)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE; 
	res = jvx_thread_initialize(&thread.hdl, &thread.cbs, static_cast<jvxHandle*>(this), rep_tstamp, nonblock, async);
	assert(res == JVX_NO_ERROR);

	res = jvx_thread_start(thread.hdl, timeout);
	assert(res == JVX_NO_ERROR);

	if (push_prio)
	{
		res = jvx_thread_set_priority(thread.hdl, JVX_THREAD_PRIORITY_REALTIME);
		assert(res == JVX_NO_ERROR);
	}

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxThreads::thread_id(JVX_THREAD_ID* t_id)
{
	jvx_thread_id(thread.hdl, t_id);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxThreads::stop(jvxSize timeout_stop)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	
	if (thread.hdl)
	{
		res = jvx_thread_stop(thread.hdl, timeout_stop);
		assert(res == JVX_NO_ERROR);

		res = jvx_thread_terminate(thread.hdl);
		assert(res == JVX_NO_ERROR);

		thread.hdl = NULL;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxThreads::terminate()
{
	rep = NULL;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxThreads::trigger_wakeup(jvxBool trigger_even_if_busy)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (thread.hdl)
	{
		res = jvx_thread_wakeup(thread.hdl, trigger_even_if_busy);
	}
	return res;
}

// CALLBACKS CALLBACKS CALLBACKS CALLBACKS CALLBACKS CALLBACKS

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC(CjvxThreads, started)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (rep)
	{
		res = rep->startup(timestamp_us);
	}
	return res;
}

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC(CjvxThreads, stopped)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (rep)
	{
		res = rep->stopped(timestamp_us);
	}
	return res;
}

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_IP(CjvxThreads, wokeup)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (rep)
	{
		res = rep->wokeup(timestamp_us, delta_ms);
	}
	return res;
}

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_IP(CjvxThreads, expired)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (rep)
	{
		res = rep->expired(timestamp_us, delta_ms);
	}
	return res;

}