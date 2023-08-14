#ifndef __IJVXTHREADS_H__
#define __IJVXTHREADS_H__

JVX_INTERFACE IjvxThreads_report
{
public:
	virtual ~IjvxThreads_report(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION startup(jvxInt64 timestamp_us) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION expired(jvxInt64 timestamp_us, jvxSize* delta_ms) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION stopped(jvxInt64 timestamp_us) = 0;

};

JVX_INTERFACE IjvxThreads: public IjvxObject
{
public:
	virtual ~IjvxThreads(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxThreads_report* bwdReport) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION start(jvxSize timeout = JVX_SIZE_UNSELECTED, jvxBool push_prio = false, jvxBool rep_tstamp = false, jvxBool nonblock = false, jvxBool async = false) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION thread_id(JVX_THREAD_ID* t_id) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop(jvxSize timeout_stop_msecs = 5000)  = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_wakeup(jvxBool triggere_even_if_busy = true) = 0;
};

#endif