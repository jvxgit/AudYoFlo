#include "common/CjvxReportMessageQueue.h"

CjvxReportMessageQueue::CjvxReportMessageQueue()
	{
		runtime.state = JVX_STATE_NONE;
		JVX_INITIALIZE_MUTEX(runtime.safeAccessQueue);
	}

CjvxReportMessageQueue::~CjvxReportMessageQueue()
	{
		terminate_queue();
		JVX_TERMINATE_MUTEX(runtime.safeAccessQueue);
	}

	jvxErrorType 
		CjvxReportMessageQueue::initialize_queue(CjvxMessageQueue_process_object cb_process, jvxHandle* pv_process)
	{
		jvxErrorType res = JVX_NO_ERROR;
		JVX_LOCK_MUTEX(runtime.safeAccessQueue);
		if(runtime.state == JVX_STATE_NONE)
		{
			runtime.cb_process = cb_process;
			runtime.pv_process = pv_process;
			runtime.state = JVX_STATE_INIT;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		JVX_UNLOCK_MUTEX(runtime.safeAccessQueue);
		return res;
	}

	jvxErrorType 
		CjvxReportMessageQueue::start_queue()
	{
		jvxErrorType res = JVX_NO_ERROR;
		JVX_LOCK_MUTEX(runtime.safeAccessQueue);
		if(runtime.state == JVX_STATE_INIT)
		{
			runtime.state = JVX_STATE_PROCESSING;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		JVX_UNLOCK_MUTEX(runtime.safeAccessQueue);
		return res;
	}

	jvxErrorType 
		CjvxReportMessageQueue::stop_queue()
	{
		jvxErrorType res = JVX_NO_ERROR;
		JVX_LOCK_MUTEX(runtime.safeAccessQueue);
		if(runtime.state == JVX_STATE_PROCESSING)
		{
			while(runtime.theQueue.size())
			{
				oneMessageInQueue theMess = runtime.theQueue.front();
				runtime.theQueue.pop_front();
				if(theMess.remove_cb)
				{
					theMess.remove_cb(theMess.mess_content_remove_me_after_post, theMess.sz_mess_content, theMess.elementType, theMess.remove_ctxt);
				}
			}
			runtime.theQueue.clear();
			runtime.state = JVX_STATE_INIT;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		JVX_UNLOCK_MUTEX(runtime.safeAccessQueue);
		return res;
	}

	jvxErrorType 
		CjvxReportMessageQueue::terminate_queue()
	{
		jvxErrorType res = JVX_NO_ERROR;

		stop_queue();

		JVX_LOCK_MUTEX(runtime.safeAccessQueue);
		if(runtime.state == JVX_STATE_INIT)
		{
			runtime.state = JVX_STATE_NONE;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		JVX_UNLOCK_MUTEX(runtime.safeAccessQueue);
		return res;
	}

	jvxErrorType 
		CjvxReportMessageQueue::trigger_process_element_remove_queue()
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Quick return if nothing to be done
		if(runtime.theQueue.empty())
		{
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}

		JVX_LOCK_MUTEX(runtime.safeAccessQueue);
		if(runtime.state == JVX_STATE_PROCESSING)
		{
			oneMessageInQueue newElm;
			newElm = runtime.theQueue.front();
			runtime.theQueue.pop_front();
			if(runtime.cb_process)
			{
				runtime.cb_process(newElm.mess_content_remove_me_after_post, newElm.sz_mess_content, newElm.elementType, runtime.pv_process);
			}
			if(newElm.remove_cb)
			{
				newElm.remove_cb(newElm.mess_content_remove_me_after_post, newElm.sz_mess_content, newElm.elementType, newElm.remove_ctxt);			
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		JVX_UNLOCK_MUTEX(runtime.safeAccessQueue);
		return res;
	};

	jvxErrorType 
		CjvxReportMessageQueue::add_element_queue(jvxCBitField elementType, jvxHandle* mess_content_remove_me_after_post, jvxSize sz_mess, CjvxMessageQueue_remove_object remove_cb, jvxHandle* ctxt)
	{
		jvxErrorType res = JVX_NO_ERROR;
		JVX_LOCK_MUTEX(runtime.safeAccessQueue);
		if(runtime.state == JVX_STATE_PROCESSING)
		{
			oneMessageInQueue newElm;
			newElm.elementType = elementType;
			newElm.mess_content_remove_me_after_post = mess_content_remove_me_after_post;
			newElm.sz_mess_content = sz_mess;
			newElm.remove_cb = remove_cb;
			newElm.remove_ctxt = ctxt;
			
			runtime.theQueue.push_back(newElm);
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		JVX_UNLOCK_MUTEX(runtime.safeAccessQueue);
		return res;
	}