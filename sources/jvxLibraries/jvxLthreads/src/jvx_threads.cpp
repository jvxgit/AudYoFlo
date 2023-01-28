#include "jvx_threads.h"

#define DEFAULT_TIMEOUT_LEAVE 5000

typedef enum
{
	JVX_THREAD_LOCK_VAR_NON_SIGNALLED,
	JVX_THREAD_LOCK_VAR_SIGNALLED,
	JVX_THREAD_LOCK_VAR_INPROCESSING
} jvxLockVarType;

typedef struct
{
	struct
	{
		JVX_THREAD_ID threadId;
		JVX_THREAD_HANDLE threadHdl;
		JVX_NOTIFY_HANDLE threadEvent;
		JVX_NOTIFY_HANDLE threadStarted;
		jvxBool threadContinue;
		jvxInt64 tStamp_init_us;
		jvxInt16 lockvar;
	} runtime;

	jvx_thread_handler* refCallback;
	jvxHandle* privData;
	jvxTimeStampData tStampRef;
	jvxState state;
	jvxBool reportTimestamp;
	jvxSize timeout_ms;
	jvxBool noblocking;
	jvxBool asyncProcessing;
} jvx_threads_private;

// =========================================================================
JVX_THREAD_ENTRY_FUNCTION(jvx_thread_callback, this_hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxTick tStamp;
	jvxSize deltaTWait = JVX_SIZE_UNSELECTED;

	if (this_hdl)
	{
		jvx_threads_private* hdl = (jvx_threads_private*)this_hdl;

		if (!hdl->noblocking)
		{
			// Lock mutex to ensure clean thread startup
			JVX_WAIT_FOR_NOTIFICATION_I(hdl->runtime.threadEvent);
		}

		// Inform driving thread that thread is up and it is safe to continue now
		JVX_SET_NOTIFICATION(hdl->runtime.threadStarted);

		//		std::cout << "Startup" << std::endl;

		if (hdl->refCallback->callback_thread_startup)
		{
			tStamp = -1;
			if (hdl->reportTimestamp)
			{
				tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&hdl->tStampRef);
				tStamp -= hdl->runtime.tStamp_init_us;
			}
			hdl->refCallback->callback_thread_startup(hdl->privData, tStamp);
		}

		while (hdl->runtime.threadContinue)
		{
			if (hdl->noblocking)
			{
				jvxInt16 cmp = JVX_THREAD_LOCK_VAR_SIGNALLED;
				jvxInt16 exch = JVX_THREAD_LOCK_VAR_INPROCESSING;
				if (JVX_INTERLOCKED_COMPARE_EXCHANGE_16(&hdl->runtime.lockvar, exch, cmp) == JVX_THREAD_LOCK_VAR_SIGNALLED)
				{
					if (hdl->runtime.threadContinue)
					{
						if (hdl->asyncProcessing)
						{
							// If in async mode, immediately reset the signalled flag
							cmp = JVX_THREAD_LOCK_VAR_INPROCESSING;
							exch = JVX_THREAD_LOCK_VAR_NON_SIGNALLED;
							if (JVX_INTERLOCKED_COMPARE_EXCHANGE_16(&hdl->runtime.lockvar, exch, cmp) == JVX_THREAD_LOCK_VAR_INPROCESSING)
							{
								do
								{
									if (hdl->refCallback->callback_thread_wokeup)
									{
										tStamp = -1;
										if (hdl->reportTimestamp)
										{
											tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&hdl->tStampRef);
											tStamp -= hdl->runtime.tStamp_init_us;
										}
										res = hdl->refCallback->callback_thread_wokeup(hdl->privData, tStamp, &deltaTWait);
									}
									else
									{
										res = JVX_NO_ERROR;
									}
								} 
								while ((res == JVX_ERROR_THREAD_ONCE_MORE) && hdl->runtime.threadContinue);
							}
							else
							{
								// This case should never happen
								assert(0);
							}
						}
						else
						{
							do
							{
								if (hdl->refCallback->callback_thread_wokeup)
								{
									tStamp = -1;
									if (hdl->reportTimestamp)
									{
										tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&hdl->tStampRef);
										tStamp -= hdl->runtime.tStamp_init_us;
									}
									res = hdl->refCallback->callback_thread_wokeup(hdl->privData, tStamp, &deltaTWait);
								}
								else
								{
									res = JVX_NO_ERROR;
								}
							} 
							while ((res == JVX_ERROR_THREAD_ONCE_MORE) && hdl->runtime.threadContinue);

							cmp = JVX_THREAD_LOCK_VAR_INPROCESSING;
							exch = JVX_THREAD_LOCK_VAR_NON_SIGNALLED;
							JVX_INTERLOCKED_COMPARE_EXCHANGE_16(&hdl->runtime.lockvar, exch, cmp);
						}
					}
				}
			}//if(hdl->noblocking)
			else
			{
				JVX_WAIT_RESULT resW = JVX_WAIT_SUCCESS;
				bool stopmenow = false;
				bool retriggered = false;
				//				std::cout << "EW" << std::endl;
				if(JVX_CHECK_SIZE_UNSELECTED(deltaTWait))
				{ 
					deltaTWait = hdl->timeout_ms;
				}
				else
				{
					deltaTWait = JVX_MIN(deltaTWait, hdl->timeout_ms);
				}

				//std::cout << "Wait for " << deltaTWait << "msecs" << std::endl;

				if (deltaTWait == 0)
				{
					resW = JVX_WAIT_SUCCESS;
					JVX_WAIT_FOR_NOTIFICATION_II_CLEAR(hdl->runtime.threadEvent);
				}
				else
				{
					if (JVX_CHECK_SIZE_UNSELECTED(deltaTWait))
					{
						resW = JVX_WAIT_FOR_NOTIFICATION_II_INF(hdl->runtime.threadEvent);
					}
					else
					{
						resW = JVX_WAIT_FOR_NOTIFICATION_II_MS(hdl->runtime.threadEvent, deltaTWait);
					}
				}
				deltaTWait = JVX_SIZE_UNSELECTED;

				// Loop as long as the signal was retriggered
				do
				{
					//  				std::cout << "I" << std::endl;

					if (hdl->runtime.threadContinue) // <- only execute this function if we have not yet ended it
					{
						// 				std::cout << "II" << std::endl;

						if (resW == JVX_WAIT_SUCCESS)
						{
							//	        		    std::cout << "Signalled" << std::endl;

							jvxInt16 exch = JVX_THREAD_LOCK_VAR_INPROCESSING;
							JVX_INTERLOCKED_EXCHANGE_16(&hdl->runtime.lockvar, exch);

							if (hdl->asyncProcessing)
							{
								// Reset the "in processing flag" before start
								exch = JVX_THREAD_LOCK_VAR_NON_SIGNALLED;
								JVX_INTERLOCKED_EXCHANGE_16(&hdl->runtime.lockvar, exch);

								do
								{
									if (hdl->refCallback->callback_thread_wokeup)
									{
										tStamp = -1;
										if (hdl->reportTimestamp)
										{
											tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&hdl->tStampRef);
											tStamp -= hdl->runtime.tStamp_init_us;
										}
										res = hdl->refCallback->callback_thread_wokeup(hdl->privData, tStamp, &deltaTWait);
									}
									else
									{
										res = JVX_NO_ERROR;
									}
								} 
								while ((res == JVX_ERROR_THREAD_ONCE_MORE) && hdl->runtime.threadContinue);
							}
							else
							{
								do
								{
									if (hdl->refCallback->callback_thread_wokeup)
									{
										tStamp = -1;
										if (hdl->reportTimestamp)
										{
											tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&hdl->tStampRef);
											tStamp -= hdl->runtime.tStamp_init_us;
										}
										res = hdl->refCallback->callback_thread_wokeup(hdl->privData, tStamp, &deltaTWait);
									}
									else
									{
										res = JVX_NO_ERROR;
									}
								} 
								while ((res == JVX_ERROR_THREAD_ONCE_MORE) && hdl->runtime.threadContinue);

								exch = JVX_THREAD_LOCK_VAR_NON_SIGNALLED;
								JVX_INTERLOCKED_EXCHANGE_16(&hdl->runtime.lockvar, exch);
							}
						}// if(resW == JVX_WAIT_SUCCESS)
						else
						{
							jvxInt16 exch = JVX_THREAD_LOCK_VAR_INPROCESSING;
							JVX_INTERLOCKED_EXCHANGE_16(&hdl->runtime.lockvar, exch);

							if (hdl->asyncProcessing)
							{
								exch = JVX_THREAD_LOCK_VAR_NON_SIGNALLED;
								JVX_INTERLOCKED_EXCHANGE_16(&hdl->runtime.lockvar, exch);

								do
								{
									if (hdl->refCallback->callback_thread_timer_expired)
									{
										tStamp = -1;
										if (hdl->reportTimestamp)
										{
											tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&hdl->tStampRef);
											tStamp -= hdl->runtime.tStamp_init_us;
										}
										res = hdl->refCallback->callback_thread_timer_expired(hdl->privData, tStamp, &deltaTWait);
									}
									else
									{
										res = JVX_NO_ERROR;
									}
								} 
								while ((res == JVX_ERROR_THREAD_ONCE_MORE) && hdl->runtime.threadContinue);

							}
							else
							{
								do
								{
									if (hdl->refCallback->callback_thread_timer_expired)
									{
										tStamp = -1;
										if (hdl->reportTimestamp)
										{
											tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&hdl->tStampRef);
											tStamp -= hdl->runtime.tStamp_init_us;
										}
										res = hdl->refCallback->callback_thread_timer_expired(hdl->privData, tStamp, &deltaTWait);
									}
									else
									{
										res = JVX_NO_ERROR;
									}
								}
								while ((res == JVX_ERROR_THREAD_ONCE_MORE) && hdl->runtime.threadContinue);

								exch = JVX_THREAD_LOCK_VAR_NON_SIGNALLED;
								JVX_INTERLOCKED_EXCHANGE_16(&hdl->runtime.lockvar, exch);
							}
						}// else [if(resW == JVX_WAIT_SUCCESS)]

						// If callback returned an error, processing stops
						if (res != JVX_NO_ERROR)
						{
							if (res == JVX_ERROR_THREAD_ONCE_MORE)
							{
								// If we end up here, the timeout was used for iterative processing but someone has
								// requested the tread to stop from outside. This is fine!							
							}
							else
							{
								std::cout << "!!!! Thread callback failed: error code: " << jvxErrorType_txt(res) << " - stopping thread immediately !!!!" << std::endl;
								stopmenow = true;
								retriggered = false;
							}
						}
						else
						{
							retriggered = JVX_WAIT_FOR_CHECK_RETRIGGER_NOTIFICATION_I(hdl->runtime.threadEvent);
							if (retriggered)
							{
								// Change this from a timeout event to a signalled event if necessary
								resW = JVX_WAIT_SUCCESS;
							}
						}
					}// if(hdl->runtime.threadContinue)
					else
					{
						retriggered = false;
					}
					// std::cout << "III" << std::endl;

				} while (retriggered);// This is a shortcut to not fall into the wait if signal was retriggered in the mean time - linux only		    

				// std::cout << "IV" << std::endl;
				if (stopmenow)
				{
					// std::cout << "V" << std::endl;
					break;
				}
			}// else [if(hdl->noblocking)]
		}// while(hdl->runtime.threadContinue)

		if (hdl->refCallback->callback_thread_stopped)
		{
			//std::cout << "Stop" << std::endl;

			tStamp = -1;
			if (hdl->reportTimestamp)
			{
				tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&hdl->tStampRef);
				tStamp -= hdl->runtime.tStamp_init_us;
			}
			hdl->refCallback->callback_thread_stopped(hdl->privData, tStamp);
		}
	}

	// std::cout << "Leaving Thread" << std::endl;
	return(0);
}

//=============================================================================
jvxErrorType
jvx_thread_initialize(jvxHandle** hdlOnReturn, jvx_thread_handler* report_callback, jvxHandle* privData, jvxBool report_timestamp, jvxBool nonblocking, jvxBool asyncProcessing)
{
	if (hdlOnReturn)
	{
		jvx_threads_private* hdl = new jvx_threads_private;

		hdl->privData = privData;
		hdl->refCallback = report_callback;

		hdl->state = JVX_STATE_INIT;
		hdl->noblocking = nonblocking;
		hdl->asyncProcessing = asyncProcessing;

		hdl->runtime.threadContinue = false;
		hdl->runtime.threadHdl = JVX_NULLTHREAD;
		hdl->runtime.threadId = 0;

		JVX_INITIALIZE_NOTIFICATION(hdl->runtime.threadEvent);
		JVX_INITIALIZE_NOTIFICATION(hdl->runtime.threadStarted);

		hdl->runtime.lockvar = JVX_THREAD_LOCK_VAR_NON_SIGNALLED;

		//hdl->timeout_ms = timeout_ms;
		hdl->reportTimestamp = report_timestamp;

		*hdlOnReturn = hdl;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_thread_terminate(jvxHandle* hdlEnter)
{
	if (hdlEnter)
	{
		jvx_threads_private* hdl = (jvx_threads_private*)hdlEnter;

		if (hdl->state == JVX_STATE_PROCESSING)
		{
			jvx_thread_stop(hdl);
		}

		delete(hdl);
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_thread_start(jvxHandle* hdlEnter, jvxSize timeout_ms)
{
	if (hdlEnter)
	{
		jvx_threads_private* hdl = (jvx_threads_private*)hdlEnter;

		if (hdl->state == JVX_STATE_INIT)
		{
			JVX_GET_TICKCOUNT_US_SETREF(&hdl->tStampRef);
			hdl->runtime.threadContinue = true;
			hdl->timeout_ms = timeout_ms;

			JVX_WAIT_FOR_NOTIFICATION_I(hdl->runtime.threadStarted);

			JVX_CREATE_THREAD(hdl->runtime.threadHdl, jvx_thread_callback, hdl, hdl->runtime.threadId);

			// Wait for thread to be in position
			JVX_WAIT_FOR_NOTIFICATION_II_INF(hdl->runtime.threadStarted);

			hdl->state = JVX_STATE_PROCESSING;
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_WRONG_STATE);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType jvx_thread_id(jvxHandle* hdlEnter, JVX_THREAD_ID* t_id)
{
	if (hdlEnter)
	{
		jvx_threads_private* hdl = (jvx_threads_private*)hdlEnter;

		if (t_id)
			*t_id = hdl->runtime.threadId;

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType jvx_threads_get_time(jvxHandle* hdlEnter, jvxTick* myTime)
{
	if (hdlEnter)
	{
		jvx_threads_private* hdl = (jvx_threads_private*)hdlEnter;

		if (hdl->state == JVX_STATE_PROCESSING)
		{
			if (myTime)
			{
				*myTime = JVX_GET_TICKCOUNT_US_GET_CURRENT(&hdl->tStampRef);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_WRONG_STATE);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_thread_reconfigure(jvxHandle* hdlEnter, jvxSize timeout_ms)
{
	if (hdlEnter)
	{
		jvx_threads_private* hdl = (jvx_threads_private*)hdlEnter;

		hdl->timeout_ms = timeout_ms;
		if (hdl->state == JVX_STATE_PROCESSING)
		{
			jvx_thread_wakeup(hdl);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_thread_test_complete(jvxHandle* hdlEnter, jvxBool* ready_for_next_trigger)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (hdlEnter)
	{
		jvx_threads_private* hdl = (jvx_threads_private*)hdlEnter;

		if (hdl->state == JVX_STATE_PROCESSING)
		{
			if (ready_for_next_trigger)
			{
				*ready_for_next_trigger = (hdl->runtime.lockvar == JVX_THREAD_LOCK_VAR_NON_SIGNALLED);
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		return(res);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_thread_wakeup(jvxHandle* hdlEnter, jvxBool trigger_even_if_busy)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (hdlEnter)
	{
		jvx_threads_private* hdl = (jvx_threads_private*)hdlEnter;

		if (hdl->state == JVX_STATE_PROCESSING)
		{
			if (hdl->noblocking)
			{
				jvxInt16 cmp = JVX_THREAD_LOCK_VAR_NON_SIGNALLED;
				jvxInt16 exch = JVX_THREAD_LOCK_VAR_SIGNALLED;
				jvxInt16 retVal = JVX_THREAD_LOCK_VAR_INPROCESSING;
				retVal = JVX_INTERLOCKED_COMPARE_EXCHANGE_16(&hdl->runtime.lockvar, JVX_THREAD_LOCK_VAR_SIGNALLED, cmp);
				if (retVal != JVX_THREAD_LOCK_VAR_NON_SIGNALLED)
				{
					res = JVX_ERROR_COMPONENT_BUSY;
				}
			}
			else
			{
				jvxInt16 cmp = JVX_THREAD_LOCK_VAR_NON_SIGNALLED;
				jvxInt16 exch = JVX_THREAD_LOCK_VAR_SIGNALLED;
				jvxInt16 retVal = JVX_THREAD_LOCK_VAR_INPROCESSING;
				retVal = JVX_INTERLOCKED_COMPARE_EXCHANGE_16(&hdl->runtime.lockvar, JVX_THREAD_LOCK_VAR_SIGNALLED, cmp);
				if (retVal != JVX_THREAD_LOCK_VAR_NON_SIGNALLED)
				{
					res = JVX_ERROR_COMPONENT_BUSY;
					if (trigger_even_if_busy)
					{
						JVX_SET_NOTIFICATION(hdl->runtime.threadEvent);
					}
				}
				else
				{
					JVX_SET_NOTIFICATION(hdl->runtime.threadEvent);
				}
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		return(res);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_thread_set_priority(jvxHandle* hdlEnter, JVX_THREAD_PRIORITY prio)
{
	if (hdlEnter)
	{
		jvx_threads_private* hdl = (jvx_threads_private*)hdlEnter;

		if (hdl->state == JVX_STATE_PROCESSING)
		{
			JVX_SET_THREAD_PRIORITY(hdl->runtime.threadHdl, prio);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType jvx_thread_stop(jvxHandle* hdlEnter, jvxSize timeout_msec)
{
	if (hdlEnter)
	{
		jvx_threads_private* hdl = (jvx_threads_private*)hdlEnter;

		if (hdl->state == JVX_STATE_PROCESSING)
		{
			hdl->runtime.threadContinue = false;

			JVX_SET_NOTIFICATION(hdl->runtime.threadEvent);
			JVX_WAIT_RESULT res = JVX_WAIT_SUCCESS;
			if (JVX_CHECK_SIZE_UNSELECTED(timeout_msec))
			{
				res = JVX_WAIT_FOR_THREAD_TERMINATE_INF(hdl->runtime.threadHdl);
			}
			else
			{
				res = JVX_WAIT_FOR_THREAD_TERMINATE_MS(hdl->runtime.threadHdl, timeout_msec);
			}
			if (res != JVX_WAIT_SUCCESS)
			{
				JVX_TERMINATE_THREAD(hdl->runtime.threadHdl, 0);
			}
			hdl->state = JVX_STATE_INIT;
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_WRONG_STATE);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType jvx_thread_trigger_stop(jvxHandle* hdlEnter)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (hdlEnter)
	{
		jvx_threads_private* hdl = (jvx_threads_private*)hdlEnter;
		if (hdl->state == JVX_STATE_PROCESSING)
		{
			hdl->runtime.threadContinue = false;
			JVX_SET_NOTIFICATION(hdl->runtime.threadEvent);
			hdl->state = JVX_STATE_PREPARED;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		return res;
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType jvx_thread_wait_stop(jvxHandle* hdlEnter, jvxSize timeout_ms)
{
	if (hdlEnter)
	{
		jvx_threads_private* hdl = (jvx_threads_private*)hdlEnter;

		if (hdl->state == JVX_STATE_PREPARED)
		{
			JVX_WAIT_RESULT res = JVX_WAIT_SUCCESS;
			if (JVX_CHECK_SIZE_UNSELECTED(timeout_ms))
			{
				res = JVX_WAIT_FOR_THREAD_TERMINATE_INF(hdl->runtime.threadHdl);
			}
			else
			{
				res = JVX_WAIT_FOR_THREAD_TERMINATE_MS(hdl->runtime.threadHdl, timeout_ms);
			}
			if (res != JVX_WAIT_SUCCESS)
			{
			    std::cout << "thread did not terminate cleanly, force it to stop!" << std::endl;
				JVX_TERMINATE_THREAD(hdl->runtime.threadHdl, 0);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_WRONG_STATE);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
