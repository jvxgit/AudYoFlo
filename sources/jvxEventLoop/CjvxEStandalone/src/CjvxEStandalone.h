#ifndef __CJVXEVENTLOOP_H__
#define __CJVXEVENTLOOP_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include <map>

class CjvxEStandalone : public IjvxEventLoopObject, public CjvxObject
{

private:

	std::list<TjvxEventLoopElement> queue_lowPriority;
	std::list<TjvxEventLoopElement> queue_highPriority;
	std::list<TjvxEventLoopElement> queue_Timer;

	std::map<JVX_THREAD_ID, TjvxEventLoopElementSync> notifications;
	jvxSize unique_id;
	jvxBool processElement;
	jvxSize maxTimerWait;
	jvxTimeStampData myTimestamp;
	JVX_MUTEX_HANDLE safeAccessQueues;
	JVX_RW_MUTEX_HANDLE safeAccessThreadId;
	struct
	{
		jvxBool contRunning;
		JVX_NOTIFY_HANDLE notify;
		JVX_NOTIFY_HANDLE Startup;
		JVX_THREAD_HANDLE theThreadHdl;
		JVX_THREAD_ID theThreadId;
		jvxBool noMoreIncoming;
	} inThread;
	std::list<IjvxEventLoop_frontend*> lstBlockedFrontends;
	std::list<IjvxEventLoop_backend*> lstBlockedBackends;

public:
	
	JVX_CALLINGCONVENTION CjvxEStandalone(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxEStandalone();

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(
		IjvxHiddenInterface* theHost,
		jvxSize maxNum_lowprioqueue,
		jvxSize maxNum_highprioqueue,
		jvxSize maxNum_timerqueue,
		jvxSize maxNumberWait_msec)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION reserve_message_id(
		jvxSize* message_id) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION event_schedule(TjvxEventLoopElement* newElm, jvxErrorType* res_scheduled_event, jvxHandle* blockPrivate) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION events_process()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION event_clear(jvxSize event_id,
		IjvxEventLoop_frontend* fe_filter, IjvxEventLoop_backend* be_filter) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION frontend_block(IjvxEventLoop_frontend* fe_filter)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION frontend_unblock(IjvxEventLoop_frontend* fe_filter)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION backend_block(IjvxEventLoop_backend* fe_filter)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION backend_unblock(IjvxEventLoop_backend* fe_filter)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION force_clear_end_event_loop() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION wait_stop(jvxSize timeout_msec) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;

#include "codeFragments/simplify/jvxObjects_simplify.h"


	// ================================================================
	JVX_THREAD_RETURN_TYPE loop_thread();
	void loop_core(jvxBool processEvents);
	jvxErrorType processEvent(TjvxEventLoopElement* theQueueElement);

	jvxErrorType copyMessageContent(jvxHandle** ptr, jvxSize paramType, jvxSize szElm);
	jvxErrorType deleteMessageContent(jvxHandle** ptr, jvxSize paramType, jvxSize szElm);

	void removeAllEvents(std::list<TjvxEventLoopElement>& queue, jvxSize event_id,
		IjvxEventLoop_frontend* fe_filter, IjvxEventLoop_backend* be_filter);
};

#endif
