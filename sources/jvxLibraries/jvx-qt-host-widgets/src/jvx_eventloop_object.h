#ifndef __CJVXQTEVENTLOOP_H__
#define __CJVXQTEVENTLOOP_H__

#include "jvx.h"
#include <QtCore/QObject>

class CjvxQtEventLoop : public QObject, public IjvxEventLoop
{
private:
	Q_OBJECT

	jvxSize unique_id;
	jvxState _state;
	JVX_THREAD_ID mainEventThread;
	JVX_MUTEX_HANDLE safeAccessQueues;
public:
	CjvxQtEventLoop();
	~CjvxQtEventLoop();

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(
		IjvxHiddenInterface* theHost,
		jvxSize maxNum_lowprioqueue,
		jvxSize maxNum_highprioqueue,
		jvxSize maxNum_timerqueue,
		jvxSize maxNumberWait_msec) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION reserve_message_id(
		jvxSize* message_id) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION event_schedule(TjvxEventLoopElement* newElm, jvxErrorType* res_scheduled_event, 
		jvxHandle* blockPrivate) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION events_process() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION event_clear(jvxSize event_id,
		IjvxEventLoop_frontend* fe_filter, IjvxEventLoop_backend* be_filter) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION frontend_block(IjvxEventLoop_frontend* fe_filter) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION frontend_unblock(IjvxEventLoop_frontend* fe_filter) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION backend_block(IjvxEventLoop_backend* fe_filter) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION backend_unblock(IjvxEventLoop_backend* fe_filter) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION force_clear_end_event_loop() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION wait_stop(jvxSize timeout_msec) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() override;

signals:
	void emit_call_blocked(TjvxEventLoopElement*);
	void emit_call_triggered(TjvxEventLoopElement*);
public slots:
	void slot_call_blocked(TjvxEventLoopElement*);
	void slot_call_triggered(TjvxEventLoopElement*);

};

#endif