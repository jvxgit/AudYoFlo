#include "jvx_eventloop_object.h"

Q_DECLARE_METATYPE(TjvxEventLoopElement*);

CjvxQtEventLoop::CjvxQtEventLoop()
{
	mainEventThread = 0;
	unique_id = 1;
	_state = JVX_STATE_NONE;
	JVX_INITIALIZE_MUTEX(safeAccessQueues);
	qRegisterMetaType<TjvxEventLoopElement>("TjvxEventLoopElement");
}

CjvxQtEventLoop::~CjvxQtEventLoop()
{
	JVX_TERMINATE_MUTEX(safeAccessQueues);
}

jvxErrorType 
CjvxQtEventLoop::initialize(
	IjvxHiddenInterface* theHost,
	jvxSize maxNum_lowprioqueue,
	jvxSize maxNum_highprioqueue,
	jvxSize maxNum_timerqueue,
	jvxSize maxNumberWait_msec)
{
	if (_state == JVX_STATE_NONE)
	{
		_state = JVX_STATE_INIT;
		connect(this, SIGNAL(emit_call_blocked(TjvxEventLoopElement*)), this, SLOT(slot_call_blocked(TjvxEventLoopElement*)), 
			Qt::BlockingQueuedConnection);
		connect(this, SIGNAL(emit_call_triggered(TjvxEventLoopElement*)), this, SLOT(slot_call_triggered(TjvxEventLoopElement*)),
			Qt::QueuedConnection);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxQtEventLoop::start()
{
	if (_state == JVX_STATE_INIT)
	{
		mainEventThread = JVX_GET_CURRENT_THREAD_ID();
		_state = JVX_STATE_PROCESSING;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxQtEventLoop::reserve_message_id(jvxSize* message_id)
{
	JVX_LOCK_MUTEX(safeAccessQueues);
	if (message_id)
	{
		*message_id = unique_id;
		unique_id++;
	}
	JVX_UNLOCK_MUTEX(safeAccessQueues);
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxQtEventLoop::event_schedule(TjvxEventLoopElement* newElm, jvxErrorType* res_scheduled_event,
	jvxHandle* blockPrivate)
{
	jvxErrorType resL = JVX_NO_ERROR;
	if (_state == JVX_STATE_PROCESSING)
	{
		if (
			(newElm->eventPriority == JVX_EVENTLOOP_PRIORITY_NORMAL) &&
			((newElm->eventClass == JVX_EVENTLOOP_REQUEST_CALL_BLOCKING)
				||(newElm->eventClass == JVX_EVENTLOOP_REQUEST_TRIGGER)
				||(newElm->eventClass == JVX_EVENTLOOP_REQUEST_TRIGGER_RESPONSE)))
		{
			newElm->rescheduleEvent = false;
			newElm->thread_id = JVX_GET_CURRENT_THREAD_ID();

			if (newElm->autoDeleteOnProcess)
			{
				resL = jvx_eventloop_copy_message(&newElm->param, newElm->paramType, newElm->sz_param);
				assert(resL == JVX_NO_ERROR);
			}

			if (JVX_CHECK_SIZE_UNSELECTED(newElm->message_id))
			{
				reserve_message_id(&newElm->message_id);
			}
			if (newElm->eventClass == JVX_EVENTLOOP_REQUEST_CALL_BLOCKING)
			{
				TjvxEventLoopElementSync myBlockingStruct;
				myBlockingStruct.result_block = JVX_ERROR_INTERNAL;
				myBlockingStruct.priv = blockPrivate;
				myBlockingStruct.mId = newElm->message_id;

				newElm->oneHdlBlock = &myBlockingStruct;

				emit emit_call_blocked(newElm);

				if (res_scheduled_event)
				{
					*res_scheduled_event = myBlockingStruct.result_block;
				}
				newElm->oneHdlBlock = NULL;
				if (newElm->autoDeleteOnProcess)
				{
					resL = jvx_eventloop_delete_message(&newElm->param, newElm->paramType, newElm->sz_param);
					assert(resL == JVX_NO_ERROR);
				}
			}
			else
			{
				TjvxEventLoopElement* newElmC = new TjvxEventLoopElement;
				*newElmC = *newElm;
				emit emit_call_triggered(newElmC);
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_UNSUPPORTED;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxQtEventLoop::event_clear(jvxSize event_id,
	IjvxEventLoop_frontend* fe_filter, IjvxEventLoop_backend* be_filter)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxQtEventLoop::events_process() 
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxQtEventLoop::frontend_block(IjvxEventLoop_frontend* fe_filter)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxQtEventLoop::frontend_unblock(IjvxEventLoop_frontend* fe_filter)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxQtEventLoop::backend_block(IjvxEventLoop_backend* fe_filter)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxQtEventLoop::backend_unblock(IjvxEventLoop_backend* fe_filter)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxQtEventLoop::force_clear_end_event_loop()
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxQtEventLoop::stop()
{
	if (_state == JVX_STATE_PROCESSING)
	{
		_state = JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxQtEventLoop::wait_stop(jvxSize timeout_msec)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxQtEventLoop::terminate()
{
	if (_state == JVX_STATE_INIT)
	{
		_state = JVX_STATE_NONE;
		disconnect(this, SIGNAL(emit_call_blocked(TjvxEventLoopElement*)));
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

void 
CjvxQtEventLoop::slot_call_blocked(TjvxEventLoopElement* qelm)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (qelm->target_be)
	{
		res = qelm->target_be->process_event(qelm);
	}
	qelm->oneHdlBlock->result_block = res;
}

void
CjvxQtEventLoop::slot_call_triggered(TjvxEventLoopElement* qelm)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (qelm->target_be)
	{
		res = qelm->target_be->process_event(qelm);
	}
	if (qelm->eventClass == JVX_EVENTLOOP_REQUEST_TRIGGER_RESPONSE)
	{
		if (qelm->origin_fe)
		{
			qelm->origin_fe->report_process_event(qelm);
		}
	}
	if (qelm->autoDeleteOnProcess)
	{
		res = jvx_eventloop_delete_message(&qelm->param, qelm->paramType, qelm->sz_param);
		assert(res == JVX_NO_ERROR);
	}
	delete qelm;
	
}