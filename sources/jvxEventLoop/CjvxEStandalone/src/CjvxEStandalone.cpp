#include "CjvxEStandalone.h"

//#define EVENT_LOOP_OUTPUT
#define JVX_MIN_TIMEOUT_DELTA 1

JVX_THREAD_ENTRY_FUNCTION(theThreadEntry, param)
{
	JVX_THREAD_RETURN_TYPE res = 0;
	CjvxEStandalone* this_pointer = (CjvxEStandalone*)param;
	if (this_pointer)
	{
		this_pointer->loop_thread();
	}
	return res;
}

CjvxEStandalone::CjvxEStandalone(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_EVENTLOOP);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxEventLoopObject*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	JVX_INITIALIZE_MUTEX(safeAccessQueues);
	JVX_INITIALIZE_RW_MUTEX(safeAccessThreadId);
	unique_id = 1; // Definitely larger than 0!
}

CjvxEStandalone::~CjvxEStandalone()
{
	JVX_TERMINATE_RW_MUTEX(safeAccessThreadId); 
	JVX_TERMINATE_MUTEX(safeAccessQueues);
}

jvxErrorType 
CjvxEStandalone::initialize(
	IjvxHiddenInterface* theHost,
	jvxSize maxNum_lowprioqueue,
	jvxSize maxNum_highprioqueue,
	jvxSize maxNum_timerqueue,
	jvxSize maxNumberWait_msec)
{
	jvxErrorType res = JVX_NO_ERROR;

	res = CjvxObject::_initialize(theHost);
	if (res != JVX_NO_ERROR)
		return res;
	
	maxTimerWait = maxNumberWait_msec; // 1sec
	res = CjvxObject::_select();
	if (res != JVX_NO_ERROR)
		return res;
	res = CjvxObject::_activate();
	return res;
}

jvxErrorType
CjvxEStandalone::start()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxObject::_prepare();
	if (res != JVX_NO_ERROR)
		return res;
	res = CjvxObject::_start();
	if (res != JVX_NO_ERROR)
		return res;

	// Allocate processing queues
	queue_lowPriority.clear();
	queue_highPriority.clear();
	queue_Timer.clear();

	// Reset the timer
	JVX_GET_TICKCOUNT_US_SETREF(&myTimestamp);
	processElement = false;

	// Start the thread
	inThread.contRunning = true;
	inThread.noMoreIncoming = false;
	JVX_INITIALIZE_NOTIFICATION(inThread.notify);
	JVX_INITIALIZE_NOTIFICATION(inThread.Startup);

	JVX_WAIT_FOR_NOTIFICATION_I(inThread.Startup);
	JVX_CREATE_THREAD(inThread.theThreadHdl, theThreadEntry, reinterpret_cast<jvxHandle*>(this), inThread.theThreadId);
	JVX_WAIT_FOR_NOTIFICATION_II_INF(inThread.Startup);

	return res;
}

// =======================================================================

jvxErrorType
CjvxEStandalone::reserve_message_id(jvxSize* message_id)
{
	JVX_LOCK_MUTEX(safeAccessQueues);
	if (message_id)
		*message_id = unique_id++;
	JVX_UNLOCK_MUTEX(safeAccessQueues);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxEStandalone::events_process()
{
	if (inThread.theThreadId == JVX_GET_CURRENT_THREAD_ID())
	{
		loop_core(true);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxEStandalone::event_schedule(TjvxEventLoopElement* newElm, jvxErrorType* res_scheduled_event, jvxHandle* blockPrivate)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool waitForComplete = false;
	//jvxBool respond = false;
	;
	jvxTick tStampNow = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTimestamp);
	TjvxEventLoopElementSync myBlockingStruct;

	std::list<IjvxEventLoop_backend*>::iterator elmbe;
	std::list<IjvxEventLoop_frontend*>::iterator elmfe;

	/*
	(IjvxEventLoop_frontend* origin, jvxHandle* priv_fe,
		IjvxEventLoop_backend* target, jvxHandle* priv_be,
		jvxSize* message_id, jvxHandle* param, jvxSize paramType,
		jvxSize event_type, jvxOneEventClass event_class,
		jvxOneEventPriority event_priority,
		jvxSize delta_time_run,
		jvxCBool autoDeleteOnProcess,
		)
	newElm.eventClass = event_class;
	newElm.eventType = event_type;
	newElm.eventPriority = event_priority;
	newElm.origin_fe = origin;
	newElm.priv_fe = priv_fe;
	newElm.target_be = target;
	newElm.priv_be = priv_be;
	newElm.param = param;
	newElm.paramType = paramType;
	newElm.oneHdlBlock = NULL;
	newElm.rescheduleEvent = false;
	newElm.delta_t = delta_time_run;
	*/
	if (newElm == NULL)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}
	
	newElm->rescheduleEvent = false;
	newElm->thread_id = JVX_GET_CURRENT_THREAD_ID();
	newElm->oneHdlBlock = NULL;

	newElm->timestamp_run_us = tStampNow;
	if (JVX_CHECK_SIZE_SELECTED(newElm->delta_t))
	{
		newElm->timestamp_run_us += newElm->delta_t * 1000; // msec to usec
	}

	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		resL = JVX_NO_ERROR;

		// Check frontend list
		elmfe = std::find(lstBlockedFrontends.begin(), lstBlockedFrontends.end(), newElm->origin_fe);
		if (elmfe != lstBlockedFrontends.end())
		{
			// Deny adding a request if frontend is in blacklist
			return JVX_ERROR_ABORT;
		}

		// Check backend list
		elmbe = std::find(lstBlockedBackends.begin(), lstBlockedBackends.end(), newElm->target_be);
		if (elmbe != lstBlockedBackends.end())
		{
			// Deny adding a request if frontend is in blacklist
			return JVX_ERROR_ABORT;
		}

		if (newElm->eventClass == JVX_EVENTLOOP_REQUEST_IMMEDIATE)
		{
			if (JVX_GET_CURRENT_THREAD_ID() == inThread.theThreadId)
			{
				resL = this->processEvent(newElm);
				if ((newElm->target_be == NULL) &&newElm->origin_fe)
				{
					resL = newElm->origin_fe->report_process_event(newElm);
				}
				return resL;
			}
			else
			{
			  std::cout << "Immediate schedule failed since it is initiated from invalid thread: The thread has id = " <<
			    JVX_GET_CURRENT_THREAD_ID() << " whereas the allowed thread is id = " << inThread.theThreadId << "." << std::endl;
				return JVX_ERROR_INVALID_SETTING;
			}
		}
		if (newElm->autoDeleteOnProcess)
		{
			resL = copyMessageContent(&newElm->param, newElm->paramType, newElm->sz_param);
			if (resL != JVX_NO_ERROR)
			{
				std::cout << "Function call to <copyMessageContent> failed, error code = " << jvxErrorType_descr(resL) << std::endl;
				res = JVX_ERROR_INVALID_SETTING;
				return res;
			}
		}

		if (newElm->eventPriority == JVX_EVENTLOOP_PRIORITY_TIMER)
		{
			if (newElm->eventClass == JVX_EVENTLOOP_REQUEST_CALL_BLOCKING)
			{
				res = JVX_ERROR_UNSUPPORTED;
				resL = JVX_NO_ERROR;
				if (newElm->autoDeleteOnProcess)
				{
					resL = deleteMessageContent(&newElm->param, newElm->paramType, newElm->sz_param);
					if (resL != JVX_NO_ERROR)
					{
						std::cout << __FUNCTION__ << ": Error on <deleteMessageContent>" << std::endl;
					}
				}
				return res;
			}
		}
		switch (newElm->eventClass)
		{
		case JVX_EVENTLOOP_REQUEST_TRIGGER:
		case JVX_EVENTLOOP_REQUEST_TRIGGER_RESPONSE:
			//respond = true;
			break;
		case JVX_EVENTLOOP_REQUEST_CALL_BLOCKING:	
			waitForComplete = true;
			break;
		default:
			assert(0);
		}
		
		JVX_LOCK_MUTEX(safeAccessQueues);

		if (inThread.noMoreIncoming)
		{
			// Someone has initiated shutodown
			JVX_UNLOCK_MUTEX(safeAccessQueues);
			res = JVX_ERROR_END_OF_FILE;
		}
		else
		{
			/*
			newElm->message_id = 0;
			if (message_id)
			{
				if (*message_id != 0)
				{
					newElm->message_id = *message_id;
				}
			}
			*/

			if (
				(newElm->message_id == 0) ||
				JVX_CHECK_SIZE_UNSELECTED(newElm->message_id))
			{
				newElm->message_id = unique_id++;
			}
#if 0
			else
			{
				assert(0);
			}
#endif

			// Before we activate processing, set the message id to find the associated blocking thread
			if (waitForComplete)
			{
				JVX_INITIALIZE_NOTIFICATION(myBlockingStruct.notification);
				myBlockingStruct.result_block = JVX_ERROR_INTERNAL;
				myBlockingStruct.priv = blockPrivate;
				myBlockingStruct.mId = newElm->message_id;

				// We need to lock this mutex here to avoid that the signal comes before the wait!!
				JVX_WAIT_FOR_NOTIFICATION_I(myBlockingStruct.notification);

				newElm->oneHdlBlock = &myBlockingStruct;
			}

			switch (newElm->eventPriority)
			{
			case JVX_EVENTLOOP_PRIORITY_HIGH:
				queue_highPriority.push_back(*newElm);
				break;
			case JVX_EVENTLOOP_PRIORITY_NORMAL:
				queue_lowPriority.push_back(*newElm);
				break;
			case JVX_EVENTLOOP_PRIORITY_TIMER:
#ifdef EVENT_LOOP_OUTPUT
				std::cout << "Scheduling new event at " << newElm->timestamp_run_us << std::endl;
#endif
				std::list<TjvxEventLoopElement>::iterator elm = queue_Timer.begin();
				if (elm != queue_Timer.end())
				{
					//elm++; // Shift from "begin" to next following
					while ((elm != queue_Timer.end()) && (elm->timestamp_run_us < newElm->timestamp_run_us))
					{
						elm++;
					}
					if (elm == queue_Timer.end())
					{
						queue_Timer.push_back(*newElm);
					}
					else
					{
						// It is: insert before
						queue_Timer.insert(elm, *newElm);
					}
				}
				else
				{
					queue_Timer.push_back(*newElm);
				}
				//
				break;
			}

			JVX_UNLOCK_MUTEX(safeAccessQueues);

			// We detect that we have "retriggred the event loop from withint the event loop.
			// In this case, so extra notification must be triggered

			if (inThread.theThreadId != newElm->thread_id)
			{
				// Inform thread that new data is there
				JVX_SET_NOTIFICATION(inThread.notify);
			}

			if (waitForComplete)
			{
				JVX_WAIT_RESULT resW = JVX_WAIT_SUCCESS;
#ifdef EVENT_LOOP_OUTPUT
				std::cout << __FUNCTION__ << " entering wait event " << &myBlockingStruct.notification << std::endl;
#endif
				
				if (JVX_CHECK_SIZE_UNSELECTED(newElm->delta_t))
				{
					resW = JVX_WAIT_FOR_NOTIFICATION_II_INF(myBlockingStruct.notification);
				}
				else
				{
					resW = JVX_WAIT_FOR_NOTIFICATION_II_MS(myBlockingStruct.notification, (JVX_WAIT_TIMEOUT)(newElm->delta_t));
				}

				if (resW != JVX_WAIT_SUCCESS)
				{
					newElm->oneHdlBlock->result_block = JVX_ERROR_TIMEOUT;
					std::cout << __FUNCTION__ << ": Block event_schedule failed due to timeout." << std::endl;
					assert(0);
				}
				if (res_scheduled_event)
				{
					*res_scheduled_event = newElm->oneHdlBlock->result_block;
				}

				JVX_TERMINATE_NOTIFICATION(myBlockingStruct.notification);
				newElm->oneHdlBlock = NULL;
			}
		}


		if (res != JVX_NO_ERROR)
		{
			if (newElm->autoDeleteOnProcess)
			{
				resL = deleteMessageContent(&newElm->param, newElm->paramType, newElm->sz_param);
				if (resL != JVX_NO_ERROR)
				{
					std::cout << __FUNCTION__ << ": Error on <deleteMessageContent>" << std::endl;
				}
			}
		}

		return res;
	}

	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxEStandalone::event_clear(jvxSize event_id,
	IjvxEventLoop_frontend* fe_filter, IjvxEventLoop_backend* be_filter)
{
	jvxBool foundOne = false;

	JVX_THREAD_ID idLoc = JVX_GET_CURRENT_THREAD_ID();

	// IMPORTANT: THIS FUNCTION IS ONLY ALLOWED FROM WITHIN MAIN LOOP
	if (idLoc == inThread.theThreadId)
	{
	//if (_common_set_min.theState == JVX_STATE_PROCESSING)
	//{

		// Everything ok
		if (JVX_CHECK_SIZE_SELECTED(event_id) || fe_filter || be_filter)
		{
			removeAllEvents(queue_highPriority, event_id, fe_filter, be_filter);
			removeAllEvents(queue_lowPriority, event_id, fe_filter, be_filter);
			removeAllEvents(queue_Timer, event_id, fe_filter, be_filter);
		}
		

		return JVX_NO_ERROR;
	}
	else
	{
		std::cout << "Warning: trying to run <event_clear> from thread other than the main loop!" << std::endl;
	}
	return JVX_ERROR_INVALID_SETTING;
}

void
CjvxEStandalone::removeAllEvents(std::list<TjvxEventLoopElement>& queue, jvxSize event_id,
	IjvxEventLoop_frontend* fe_filter, IjvxEventLoop_backend* be_filter)
{
	while (1)
	{
		std::list<TjvxEventLoopElement>::iterator elm = queue.begin();
		for (; elm != queue.end(); elm++)
		{
			if (
				(JVX_CHECK_SIZE_SELECTED(elm->message_id) && (elm->message_id == event_id)) ||
				((fe_filter != NULL) && (elm->origin_fe == fe_filter)) ||
				((be_filter != NULL) && (elm->target_be == be_filter)))
			{
				break;
			}
		}
		if (elm == queue.end())
		{
			break;
		}
		else
		{
			switch (elm->eventClass)
			{
			case JVX_EVENTLOOP_REQUEST_TRIGGER: // Modified by HK
			case JVX_EVENTLOOP_REQUEST_TRIGGER_RESPONSE:
				if (elm->origin_fe)
				{
					TjvxEventLoopElement qel = *elm;
					elm->origin_fe->report_cancel_event(&qel);
				}
				break;
			case JVX_EVENTLOOP_REQUEST_CALL_BLOCKING:
				elm->oneHdlBlock->result_block = JVX_ERROR_ABORT;
				JVX_SET_NOTIFICATION(elm->oneHdlBlock->notification);// Unlock waiting process
				break;
			}
			queue.erase(elm);
		}
	}

}

jvxErrorType 
CjvxEStandalone::force_clear_end_event_loop()
{
	JVX_THREAD_ID idLoc = JVX_GET_CURRENT_THREAD_ID();

	// IMPORTANT: THIS FUNCTION IS ONLY ALLOWED FROM WITHIN MAIN LOOP
	if (idLoc == inThread.theThreadId)
	{
		inThread.noMoreIncoming = true;

		// Only in this: remove all pending elements in queue
		while (queue_highPriority.size())
		{
			std::list<TjvxEventLoopElement>::iterator elm = queue_highPriority.begin();
			removeAllEvents(queue_highPriority, elm->message_id, NULL, NULL);
		}
		while (queue_lowPriority.size())
		{
			std::list<TjvxEventLoopElement>::iterator elm = queue_lowPriority.begin();
			removeAllEvents(queue_lowPriority, elm->message_id, NULL, NULL);
		}
		while (queue_Timer.size())
		{
			std::list<TjvxEventLoopElement>::iterator elm = queue_Timer.begin();
			removeAllEvents(queue_Timer, elm->message_id, NULL, NULL);
		}

		inThread.contRunning = false;
		return JVX_NO_ERROR;
	}
	else
	{
		std::cout << "Warning: trying to run <force_clear_end_event_loop> from thread other than the main loop!" << std::endl;
	}
	return JVX_ERROR_INVALID_SETTING;
}
jvxErrorType 
CjvxEStandalone::stop()
{
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		// Everything ok
		// Set thread loop variable to false
		// Set stop notification
		inThread.contRunning = false;

		JVX_SET_NOTIFICATION(inThread.notify);

		if (JVX_WAIT_FOR_THREAD_TERMINATE_MS(inThread.theThreadHdl, 5000) != JVX_WAIT_SUCCESS)
		{
#ifdef EVENT_LOOP_OUTPUT
			std::cout << "Active stop of event loop thread.." << std::endl;
#endif
			JVX_TERMINATE_THREAD(inThread.theThreadHdl, -1);
		}
		CjvxObject::_stop();
		CjvxObject::_postprocess();

		// Clear the blacklists
		lstBlockedFrontends.clear();
		lstBlockedBackends.clear();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxEStandalone::wait_stop(jvxSize timeout_msec)
{
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		// The thread should be stopped by event emission
		if (JVX_WAIT_FOR_THREAD_TERMINATE_MS(inThread.theThreadHdl, timeout_msec) != JVX_WAIT_SUCCESS)
		{
			assert(0);
#ifdef EVENT_LOOP_OUTPUT
			std::cout << "Active stop of event loop thread.." << std::endl;
#endif
			JVX_TERMINATE_THREAD(inThread.theThreadHdl, -1);
		}
		CjvxObject::_stop();
		CjvxObject::_postprocess();

		// Clear the blacklists
		lstBlockedFrontends.clear();
		lstBlockedBackends.clear();

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}


jvxErrorType
CjvxEStandalone::terminate()
{
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		stop();
	}
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = CjvxObject::_deactivate();
		res = CjvxObject::_unselect();
		res = CjvxObject::_terminate();
		_common_set_min.theHostRef = NULL;

		maxTimerWait = JVX_SIZE_UNSELECTED; // 1sec
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}


#if 0
typedef struct
{
	jvxSize oper;
	jvxInt64 load;
} oneEntryLog;

#define LOGENTRIESMAX 1000
oneEntryLog logEntries[LOGENTRIESMAX] = { 0 };
static int cnt = 0;
#endif

JVX_THREAD_RETURN_TYPE
CjvxEStandalone::loop_thread()
{

	TjvxEventLoopElement theQueueElement;


	//inThread.myThreadId = JVX_GET_CURRENT_THREAD_ID(); <- Thread id is taken on startup

	JVX_WAIT_FOR_NOTIFICATION_I(inThread.notify);
	JVX_SET_NOTIFICATION(inThread.Startup);
	
	this->loop_core(false);

	// ==============================================================
	// LEAVE CODE
	// ==============================================================

	jvxBool keepThisRunning = true;

	// Report on all timer events with a response that they are cancelled.
	// All other events should have gone before
	while (keepThisRunning)
	{
		JVX_LOCK_MUTEX(safeAccessQueues);
		if (queue_Timer.size())
		{
			theQueueElement = queue_Timer.front();
			queue_Timer.pop_front();
			if (theQueueElement.origin_fe)
			{
				theQueueElement.origin_fe->report_cancel_event(&theQueueElement);
			}
		}
		else
		{
			keepThisRunning = false;
		}
		JVX_UNLOCK_MUTEX(safeAccessQueues);
	}

	assert(queue_Timer.size() == 0);
	assert(queue_highPriority.size() == 0);
	assert(queue_lowPriority.size() == 0);

	return 0;
}

void
CjvxEStandalone::loop_core(jvxBool processEventFlag)
{
	jvxTick tStamp;
	jvxTick timeout_wait = maxTimerWait;
	jvxTick timeout_wait_i = maxTimerWait;
	jvxBool sendResponse = false;
	jvxBool blockResponse = false;
	jvxBool shutdown = false;
	jvxBool processElementStack = false;
	TjvxEventLoopElement theQueueElement;
	jvxErrorType resLocal = JVX_NO_ERROR;

	while (inThread.contRunning)
	{
		JVX_WAIT_RESULT resW = JVX_WAIT_SUCCESS;
		bool retriggered = false;

		if (processEventFlag)
		{
			processElementStack = processElement;
			processElement = false;
			// Do nothing, do not wait for anything!
			resW = JVX_WAIT_SUCCESS;
		}
		else
		{
#ifdef EVENT_LOOP_OUTPUT
			std::cout << "Wait for " << timeout_wait << "msec" << std::endl;
#endif
			/*
			logEntries[cnt].oper = 0;
			logEntries[cnt].load = timeout_wait;
			cnt = (cnt + 1) % LOGENTRIESMAX;
			*/
			if (JVX_CHECK_SIZE_UNSELECTED(timeout_wait))
			{
				resW = JVX_WAIT_FOR_NOTIFICATION_II_INF(inThread.notify);
			}
			else
			{
				resW = JVX_WAIT_FOR_NOTIFICATION_II_MS(inThread.notify, (JVX_WAIT_TIMEOUT)timeout_wait);
			}
#ifdef EVENT_LOOP_TIMEOUT_VERBOSE
			std::cout << "Awakened event loop, timeout was <" << timeout_wait << "> msecs." << std::endl;
#endif
		}
		//do
		do
		{
			if (resW == JVX_WAIT_SUCCESS)
			{
#ifdef EVENT_LOOP_OUTPUT
				std::cout << "Immediate return" << std::endl;
#endif
				// Someone triggered the queue
			}
			else
			{
#ifdef EVENT_LOOP_OUTPUT
				std::cout << "Timeout return" << std::endl;
#endif
				// Timeout - but this does not make any difference
			}

			while (1)
			{
				tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTimestamp);
#ifdef EVENT_LOOP_OUTPUT
				std::cout << "now I:" << tStamp << std::endl;
#endif
#if 0
				if (cnt % 100 == 0)
					std::cout << "Cnt=" << cnt << std::endl;
				logEntries[cnt].oper = 1;
				logEntries[cnt].load = tStamp;
				cnt = (cnt + 1) % LOGENTRIESMAX;
#endif
				JVX_LOCK_MUTEX(safeAccessQueues);
				if (queue_highPriority.size())
				{
					theQueueElement = queue_highPriority.front();
					queue_highPriority.pop_front();
					processElement = true;
				}
				if (!processElement)
				{
					if (queue_lowPriority.size())
					{
						theQueueElement = queue_lowPriority.front();
						queue_lowPriority.pop_front();
						processElement = true;
					}
				}
				if (!processElement)
				{
					if (queue_Timer.size())
					{
						theQueueElement = queue_Timer.front();
						if (theQueueElement.timestamp_run_us <= (tStamp + JVX_MIN_TIMEOUT_DELTA * 1000))
						{
#if 0
							logEntries[cnt].oper = 10;
							logEntries[cnt].load = tStamp;
							cnt = (cnt + 1) % LOGENTRIESMAX;
#endif
							queue_Timer.pop_front();
							processElement = true;
						}
#if 0
						else
						{
							logEntries[cnt].oper = 11;
							logEntries[cnt].load = theQueueElement.timestamp_run_us;
							cnt = (cnt + 1) % LOGENTRIESMAX;
						}
#endif
					}
				}

				JVX_UNLOCK_MUTEX(safeAccessQueues);

				resLocal = JVX_NO_ERROR;
				sendResponse = false;
				blockResponse = false;
				shutdown = false;

				if (processElement)
				{
					if (!inThread.noMoreIncoming)
					{
						resLocal = this->processEvent(&theQueueElement);
					}
					else
					{
						resLocal = JVX_ERROR_ABORT;
					}

					if (
							(theQueueElement.target_be == NULL) &&
							(theQueueElement.eventType == JVX_EVENTLOOP_EVENT_DEACTIVATE))
					{
						shutdown = true;
					}
					JVX_LOCK_MUTEX(safeAccessQueues);
					processElement = false;

					if (theQueueElement.rescheduleEvent)
					{
						switch (theQueueElement.eventPriority)
						{
						case JVX_EVENTLOOP_PRIORITY_NORMAL:
							queue_lowPriority.push_back(theQueueElement);
							break;
						case JVX_EVENTLOOP_PRIORITY_HIGH:
							queue_highPriority.push_back(theQueueElement);
							break;
						case JVX_EVENTLOOP_PRIORITY_TIMER:

							// Reschedule timer event with new timestamp
							theQueueElement.timestamp_run_us += theQueueElement.delta_t * 1000; // msec to usec
							queue_Timer.push_back(theQueueElement);
							break;
						}
					}
					JVX_UNLOCK_MUTEX(safeAccessQueues);

					switch (theQueueElement.eventClass)
					{
					case JVX_EVENTLOOP_REQUEST_TRIGGER_RESPONSE:

						sendResponse = true;
						break;
					case JVX_EVENTLOOP_REQUEST_CALL_BLOCKING:
						blockResponse = true;
						break;
					default:
						break;
					}
				}
				else
				{
					break;
				}

				if (sendResponse)
				{
					if (theQueueElement.origin_fe)
					{
						if (inThread.noMoreIncoming)
						{
							theQueueElement.origin_fe->report_cancel_event(&theQueueElement);
						}
						else
						{
							theQueueElement.origin_fe->report_process_event(&theQueueElement);
						}
					}
				}

				if (blockResponse)
				{
					theQueueElement.oneHdlBlock->result_block = resLocal;
					JVX_SET_NOTIFICATION(theQueueElement.oneHdlBlock->notification);
#ifdef EVENT_LOOP_OUTPUT
					std::cout << __FUNCTION__ << " notification set " << &theQueueElement.oneHdlBlock->notification << std::endl;
#endif
				}

				if (shutdown)
				{
					inThread.contRunning = false;
					JVX_LOCK_MUTEX(safeAccessQueues);
					inThread.noMoreIncoming = true;
					JVX_UNLOCK_MUTEX(safeAccessQueues);
				}

				if (theQueueElement.autoDeleteOnProcess)
				{
					jvxErrorType resL = deleteMessageContent(&theQueueElement.param, theQueueElement.paramType, theQueueElement.sz_param);
					if (resL != JVX_NO_ERROR)
					{
						std::cout << __FUNCTION__ << ": Error on <deleteMessageContent>" << std::endl;
					}
				}
			}

			// Re-schedule timer
			tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTimestamp);
#ifdef EVENT_LOOP_OUTPUT
			std::cout << "now II:" << tStamp << std::endl;
#endif
#if 0
			logEntries[cnt].oper = 2;
			logEntries[cnt].load = tStamp / 1000;
			cnt = (cnt + 1) % LOGENTRIESMAX;
#endif
			if (inThread.contRunning)
			{
				if (processEventFlag)
				{
					retriggered = JVX_CHECK_RETRIGGER_NOTIFICATION(inThread.notify);
				}
				else
				{
					retriggered = JVX_WAIT_FOR_CHECK_RETRIGGER_NOTIFICATION_I(inThread.notify);
				}
				if (retriggered)
				{
					// Change this from a timeout event to a signalled event if necessary
					resW = JVX_WAIT_SUCCESS;
				}
			}
			else
			{
				retriggered = false;
			}			
		} while (retriggered);// This is a shortcut to not fall into the wait if signal was retriggered in the mean time - linux only		    

		if (processEventFlag)
		{
			// Nothing else to do, leave the loop
			break;
		}
		else
		{
			// Prepare the timeout for re-entry
			timeout_wait = maxTimerWait;
			//tStamp = JVX_GET_TICKCOUNT_US_GET(&myTimestamp);
			JVX_LOCK_MUTEX(safeAccessQueues);

#ifdef EVENT_LOOP_TIMEOUT_VERBOSE
			printf("-> tStamp = " JVX_PRINTF_CAST_TICK_HEX "\n", tStamp);
#endif

			if (queue_Timer.size())
			{
				timeout_wait_i = (queue_Timer.begin())->timestamp_run_us - tStamp;	// <- in usec
				timeout_wait_i = JVX_MAX((jvxTick)0, timeout_wait_i);
				timeout_wait = (timeout_wait_i + (jvxTick)500) / (jvxTick)1000; // <- in msec but round not trunc!
				timeout_wait = JVX_MAX(timeout_wait, (jvxTick)JVX_MIN_TIMEOUT_DELTA); // <- wait at least a little bit to avoid "non-blocking" wait
			}
			JVX_UNLOCK_MUTEX(safeAccessQueues);
			timeout_wait = JVX_MIN(timeout_wait, maxTimerWait);
#if 0
			logEntries[cnt].oper = 3;
			logEntries[cnt].load = timeout_wait;
			cnt = (cnt + 1) % LOGENTRIESMAX;
#endif	
		}
	}// while (inThread.contRunning)
	if (processEventFlag)
	{
		processElement = processElementStack;
	}
}

jvxErrorType 
CjvxEStandalone::processEvent(TjvxEventLoopElement* theQueueElement)
{
	jvxErrorType res = JVX_NO_ERROR;
	assert(theQueueElement);

	if (theQueueElement->target_be)
	{
		res = theQueueElement->target_be->process_event(theQueueElement);
				/*
				theQueueElement.message_id,
				theQueueElement.origin_fe, theQueueElement.priv_fe,
				theQueueElement.param, theQueueElement.paramType, theQueueElement.eventType,
				theQueueElement.eventClass, theQueueElement.eventPriority, theQueueElement.autoDeleteOnProcess,
				theQueueElement.oneHdlBlock->priv, &theQueueElement.rescheduleEvent);
				*/
	}
	else
	{
		/*if (theQueueElement->origin_fe)
		{
			res = theQueueElement->origin_fe->report_process_event(theQueueElement);
		}*/
	}

	return res;
}

jvxErrorType
CjvxEStandalone::copyMessageContent(jvxHandle** ptr, jvxSize paramType,jvxSize szElm)
{
	return jvx_eventloop_copy_message( ptr, paramType, szElm);
}

jvxErrorType
CjvxEStandalone::deleteMessageContent(jvxHandle** ptr, jvxSize paramType, jvxSize szElm)
{
	return jvx_eventloop_delete_message(ptr, paramType, szElm);
}

jvxErrorType
CjvxEStandalone::frontend_block(IjvxEventLoop_frontend* fe_filter)
{
	if (JVX_GET_CURRENT_THREAD_ID() == this->inThread.theThreadId)
	{
		std::list<IjvxEventLoop_frontend*>::iterator elm = lstBlockedFrontends.begin();
		elm = std::find(lstBlockedFrontends.begin(), lstBlockedFrontends.end(), fe_filter);
		if (elm == lstBlockedFrontends.end())
		{
			lstBlockedFrontends.push_back(fe_filter);
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	else
	{
		std::cout << "Warning: trying to run <backend_block> from thread other than the main loop!" << std::endl;
	}
	return JVX_ERROR_INVALID_SETTING;
}

jvxErrorType 
CjvxEStandalone::frontend_unblock(IjvxEventLoop_frontend* fe_filter)
{
	if (JVX_GET_CURRENT_THREAD_ID() == this->inThread.theThreadId)
	{
		std::list<IjvxEventLoop_frontend*>::iterator elm = lstBlockedFrontends.begin();
		elm = std::find(lstBlockedFrontends.begin(), lstBlockedFrontends.end(), fe_filter);
		if (elm == lstBlockedFrontends.end())
		{
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		lstBlockedFrontends.erase(elm);
		return JVX_NO_ERROR;
	}
	else
	{
		std::cout << "Warning: trying to run <backend_block> from thread other than the main loop!" << std::endl;
	}
	return JVX_ERROR_INVALID_SETTING;
}

jvxErrorType
CjvxEStandalone::backend_block(IjvxEventLoop_backend* be_filter)
{
	if (JVX_GET_CURRENT_THREAD_ID() == this->inThread.theThreadId)
	{
		std::list<IjvxEventLoop_backend*>::iterator elm = lstBlockedBackends.begin();
		elm = std::find(lstBlockedBackends.begin(), lstBlockedBackends.end(), be_filter);
		if (elm == lstBlockedBackends.end())
		{
			lstBlockedBackends.push_back(be_filter);
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	else
	{
		std::cout << "Warning: trying to run <backend_block> from thread other than the main loop!" << std::endl;
	}
	return JVX_ERROR_INVALID_SETTING;
}

jvxErrorType
CjvxEStandalone::backend_unblock(IjvxEventLoop_backend* be_filter)
{
	if (JVX_GET_CURRENT_THREAD_ID() == this->inThread.theThreadId)
	{
		std::list<IjvxEventLoop_backend*>::iterator elm = lstBlockedBackends.begin();
		elm = std::find(lstBlockedBackends.begin(), lstBlockedBackends.end(), be_filter);
		if (elm == lstBlockedBackends.end())
		{
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		lstBlockedBackends.erase(elm);
		return JVX_NO_ERROR;
	}
	else
	{
		std::cout << "Warning: trying to run <backend_block> from thread other than the main loop!" << std::endl;
	}
	return JVX_ERROR_INVALID_SETTING;
}
