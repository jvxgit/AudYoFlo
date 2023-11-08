/****************************************************************************
 *																			*
 ****************************************************************************
 * File:	jvx_message_queue.cpp											*
 * Purpose: 
 ****************************************************************************
 ****************************************************************************/

#include <cstring>
#include <iostream>
#include "jvx_message_queue.h"

#define TIMEOUT_END_MS JVX_INFINITE

static jvxErrorType jvx_message_queue_clear_queue(jvxHandle* hdl);

// ======================================================================================

typedef struct
{
	jvxSize contextId;
	struct
	{
		jvxHandle* fld;
		jvxSize fldSize;
		jvxTick tstamp_enter;
	} data;
	jvxHandle* priv_data_message;
} oneMessageInQueue;

// ======================================================================================

typedef struct
{
	jvxSize numElementsInQueueMax;
	oneMessageInQueue* messages;
	jvxSize timeoutMax_ms;
	JVX_MUTEX_HANDLE stateSync;

	bool queueIsReady;
	struct
	{
		JVX_THREAD_ID threadId;
		JVX_THREAD_HANDLE threadHdl;
		JVX_MUTEX_HANDLE threadSync;
		
		bool threadContinue;

		JVX_NOTIFY_HANDLE reportNewData;
		JVX_NOTIFY_HANDLE reportThreadStarted;

		jvxSize idWriteMessageQueue;
		jvxSize numElementsInQueue;
		jvxTimeStampData tStampRef;
		jvxBool trigger_clear_queue;
		jvxBool currentMessageInUse;
	} runtime;

	jvxHandle* queuePrivateData;
	jvxMessageQueueObjectStrategyType type;
	messageQueueHandler* report_callback;
	jvxSize maxSizeContainerMessage;

	

	jvxState state;
} oneMessageQueue;

JVX_THREAD_ENTRY_FUNCTION(jvx_message_queue_thread_callback, this_queue)
{
	jvxTick tStamp;

	if(this_queue)
	{
		oneMessageQueue* theQueue = (oneMessageQueue* )this_queue;
		
		JVX_SET_NOTIFICATION(theQueue->runtime.reportThreadStarted);
		JVX_WAIT_FOR_NOTIFICATION_I(theQueue->runtime.reportNewData);

		if(theQueue->report_callback->callback_message_queue_started)
		{
			tStamp = -1;
			tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&theQueue->runtime.tStampRef);
			theQueue->report_callback->callback_message_queue_started(theQueue->queuePrivateData, tStamp);
		}
		while(theQueue->runtime.threadContinue)
		{
			JVX_WAIT_RESULT resW = JVX_WAIT_SUCCESS;
			jvxErrorType resCb = JVX_NO_ERROR;
			bool stopmenow = false;
			bool retriggered = false;

			if (JVX_CHECK_SIZE_UNSELECTED(theQueue->timeoutMax_ms))
			{
				resW = JVX_WAIT_FOR_NOTIFICATION_II_INF(theQueue->runtime.reportNewData);
			}
			else
			{
				resW = JVX_WAIT_FOR_NOTIFICATION_II_MS(theQueue->runtime.reportNewData, theQueue->timeoutMax_ms);
			}

			// Loop as long as the signal was retriggered
			do
			{
				if(resW == JVX_WAIT_SUCCESS)
				{
					while (theQueue->runtime.numElementsInQueue > 0)
					{
						jvxBool leaveLoop = false;
						jvxByte* fldDelete = NULL;

						// It is not reuired to put this into a critical section here: 
						// If this thread runs, there should not be any condition that queue is not ready

						// This critical section is only to keep queue in order
						JVX_LOCK_MUTEX(theQueue->runtime.threadSync);
						theQueue->runtime.currentMessageInUse = true;

						jvxSize idxMessageFromQueue =
							(theQueue->runtime.idWriteMessageQueue + theQueue->numElementsInQueueMax - theQueue->runtime.numElementsInQueue)
							% theQueue->numElementsInQueueMax;

						// HK, corrected 15042021:
						// We address the object in queue but we keep it in queue 
						// We need however to release the lock since the following callback may 
						// add another element to the queue
						JVX_UNLOCK_MUTEX(theQueue->runtime.threadSync);

						oneMessageInQueue* theMess = &theQueue->messages[idxMessageFromQueue];
						if (theQueue->report_callback->callback_message_queue_message_in_queue_ready)
						{
							tStamp = -1;
							tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&theQueue->runtime.tStampRef);
							resCb = theQueue->report_callback->callback_message_queue_message_in_queue_ready(theMess->contextId, theMess->data.fld, theMess->data.fldSize,
								theQueue->queuePrivateData, theMess->priv_data_message, tStamp);
						}

						if ((resCb == JVX_NO_ERROR) || (resCb == JVX_ERROR_ABORT))
						{
							// HK, corrected 15042021:
							// If we are here we can remove message since it was successfully served
							JVX_LOCK_MUTEX(theQueue->runtime.threadSync);
							theQueue->runtime.numElementsInQueue--;
							JVX_UNLOCK_MUTEX(theQueue->runtime.threadSync);

							// Here, we are sure that the message will disappear from queue
							if (theQueue->type == JVX_MESSAGE_QUEUE_DYNAMIC_OBJECTS)
							{
								fldDelete = (char*)theMess->data.fld;
								theMess->data.fld = NULL;
								theMess->data.fldSize = 0;
							}
					
							// HK, corrected 15042021:
							// It might be requested to clear the queue
							JVX_LOCK_MUTEX(theQueue->runtime.threadSync);
							if (theQueue->runtime.trigger_clear_queue)
							{
								// Delete must be done here. Should not be a speed problem since we just remove elements anyway
								if (theQueue->type == JVX_MESSAGE_QUEUE_DYNAMIC_OBJECTS)
								{
									jvxSize i;
									for (i = 0; i < theQueue->numElementsInQueueMax; i++)
									{
										if (theQueue->messages[i].data.fld)
										{
										    JVX_DSP_SAFE_DELETE_FIELD_TYPE(theQueue->messages[i].data.fld, jvxByte);
										}
									}

									// Unselect buffer since it was deleted already
									fldDelete = NULL;
								}
								theQueue->runtime.numElementsInQueue = 0;
								theQueue->runtime.trigger_clear_queue = false;
							}
							JVX_UNLOCK_MUTEX(theQueue->runtime.threadSync);
						}
						else
						{
							leaveLoop = true;
						}

						// Here we are really sure that no one is working on this message
						JVX_LOCK_MUTEX(theQueue->runtime.threadSync);
						theQueue->runtime.currentMessageInUse = false;
						JVX_UNLOCK_MUTEX(theQueue->runtime.threadSync);

						if (fldDelete)
						{
						    JVX_DSP_SAFE_DELETE_FIELD_TYPE(fldDelete, jvxByte);
						}						

						if (leaveLoop)
						{
							break;
						}
					}
				}
				else
				{
					while (theQueue->runtime.numElementsInQueue > 0)
					{
						jvxBool leaveLoop = false;
						char* fldDelete = NULL;

						// This critical section is only to keep queue in order
						JVX_LOCK_MUTEX(theQueue->runtime.threadSync);
						theQueue->runtime.currentMessageInUse = true; 
						jvxSize idxMessageFromQueue =
							(theQueue->runtime.idWriteMessageQueue + theQueue->numElementsInQueueMax - theQueue->runtime.numElementsInQueue)
							% theQueue->numElementsInQueueMax;

						// HK, corrected 15042021:
						// We address the object in queue but we keep it in queue 
						// We need however to release the lock since the following callback may 
						// add another element to the queue
						JVX_UNLOCK_MUTEX(theQueue->runtime.threadSync);

						oneMessageInQueue* theMess = &theQueue->messages[idxMessageFromQueue];
						if (theQueue->report_callback->callback_message_queue_message_in_queue_timeout)
						{
							tStamp = -1;
							tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&theQueue->runtime.tStampRef);
							resCb = theQueue->report_callback->callback_message_queue_message_in_queue_timeout(theMess->contextId, theMess->data.fld, theMess->data.fldSize,
								theQueue->queuePrivateData, theMess->priv_data_message, tStamp);
						}
						
						if ((resCb == JVX_NO_ERROR) || (resCb == JVX_ERROR_ABORT))
						{
							// If we are here the message may be removed
							JVX_LOCK_MUTEX(theQueue->runtime.threadSync);
							theQueue->runtime.numElementsInQueue--;
							JVX_UNLOCK_MUTEX(theQueue->runtime.threadSync);

							if (theQueue->type == JVX_MESSAGE_QUEUE_DYNAMIC_OBJECTS)
							{
								fldDelete = (char*)theMess->data.fld;
								theMess->data.fld = NULL;
								theMess->data.fldSize = 0;
							}

							// This critical section is only to keep queue in order
							JVX_LOCK_MUTEX(theQueue->runtime.threadSync);
							if (theQueue->runtime.trigger_clear_queue)
							{
								if (theQueue->type == JVX_MESSAGE_QUEUE_DYNAMIC_OBJECTS)
								{
									jvxSize i;
									for (i = 0; i < theQueue->numElementsInQueueMax; i++)
									{
										if (theQueue->messages[i].data.fld)
										{
										    JVX_DSP_SAFE_DELETE_FIELD_TYPE(theQueue->messages[i].data.fld, jvxByte);
										}
									}

									// Unselect buffer since it was deleted already
									fldDelete = NULL;
								}
								theQueue->runtime.numElementsInQueue = 0;
								theQueue->runtime.trigger_clear_queue = false;
							}			
							JVX_UNLOCK_MUTEX(theQueue->runtime.threadSync);
						}
						else
						{							
							leaveLoop = true;
						}

						JVX_LOCK_MUTEX(theQueue->runtime.threadSync);
						theQueue->runtime.currentMessageInUse = false;
						JVX_UNLOCK_MUTEX(theQueue->runtime.threadSync);

						if (fldDelete)
						{
						    JVX_DSP_SAFE_DELETE_FIELD_TYPE(fldDelete, jvxByte);
						}

						if (leaveLoop)
						{
							break;
						}
					}
				}
				if(theQueue->runtime.threadContinue)
				{
					retriggered = JVX_WAIT_FOR_CHECK_RETRIGGER_NOTIFICATION_I(theQueue->runtime.reportNewData);
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
		}

		if(theQueue->report_callback->callback_message_queue_stopped)
		{
			tStamp = -1;
			tStamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&theQueue->runtime.tStampRef);
			theQueue->report_callback->callback_message_queue_stopped(theQueue->queuePrivateData, tStamp);
		}
	}
	return(0);
}

// ======================================================================================

jvxErrorType 
jvx_message_queue_initialize(jvxHandle** hdlOnReturn, jvxMessageQueueObjectStrategyType type, messageQueueHandler* report_callback, jvxHandle* privData, 
		jvxSize numMessagesInQueueMax, jvxSize timeoutMax_ms, jvxSize maxSizeContainerMessage)
{
	unsigned int i;
	if(hdlOnReturn)
	{
		oneMessageQueue* theQueue = new oneMessageQueue;

		theQueue->queuePrivateData = privData;
		theQueue->report_callback = report_callback;
		theQueue->maxSizeContainerMessage = maxSizeContainerMessage;
		theQueue->type = type;
		theQueue->numElementsInQueueMax = numMessagesInQueueMax;
		theQueue->timeoutMax_ms = timeoutMax_ms;
		JVX_INITIALIZE_MUTEX(theQueue->runtime.threadSync);
		JVX_INITIALIZE_MUTEX(theQueue->stateSync);
		theQueue->queueIsReady = false;

		if (theQueue->numElementsInQueueMax)
		{
			theQueue->messages = new oneMessageInQueue[theQueue->numElementsInQueueMax];
			for (i = 0; i < theQueue->numElementsInQueueMax; i++)
			{
				memset(&theQueue->messages[i], 0, sizeof(oneMessageInQueue));
				if (theQueue->type == JVX_MESSAGE_QUEUE_STATIC_OBJECTS)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(theQueue->messages[i].data.fld, jvxByte, theQueue->maxSizeContainerMessage);
					theQueue->messages[i].data.fldSize = theQueue->maxSizeContainerMessage;
					memset(theQueue->messages[i].data.fld, 0, theQueue->maxSizeContainerMessage);
				}
			}
		}
		theQueue->state = JVX_STATE_ACTIVE;
		*hdlOnReturn = theQueue;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}


jvxErrorType 
jvx_message_queue_terminate(jvxHandle* hdl)
{
	unsigned i;
	if(hdl)
	{
		oneMessageQueue* theQueue = (oneMessageQueue*)hdl;

		JVX_LOCK_MUTEX(theQueue->stateSync);
		if(theQueue->state == JVX_STATE_PROCESSING)
		{
			// Stop message queue if running
			jvx_message_queue_stop(hdl);
		}
		if(theQueue->state == JVX_STATE_SELECTED) // Special case if trigger_stop was previously called
		{
			jvx_message_queue_wait_for_stop(hdl);
		}

		// Delete all message
		if(theQueue->type == JVX_MESSAGE_QUEUE_STATIC_OBJECTS)
		{
			for(i = 0; i < theQueue->numElementsInQueueMax; i++)
			{
				if(theQueue->type == JVX_MESSAGE_QUEUE_STATIC_OBJECTS)
				{
				    JVX_DSP_SAFE_DELETE_FIELD_TYPE(theQueue->messages[i].data.fld, jvxByte);
					theQueue->messages[i].data.fld = NULL;
					theQueue->messages[i].data.fldSize = 0;
				}
			}
			theQueue->numElementsInQueueMax = 0;
		}	

		theQueue->state = JVX_STATE_INIT;
		delete[](theQueue->messages);

		JVX_UNLOCK_MUTEX(theQueue->stateSync);

		JVX_TERMINATE_MUTEX(theQueue->runtime.threadSync);
		JVX_TERMINATE_MUTEX(theQueue->stateSync);

		delete(theQueue);
		theQueue = NULL;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
jvx_message_queue_start(jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdl)
	{
		oneMessageQueue* theQueue = (oneMessageQueue*)hdl;

		JVX_LOCK_MUTEX(theQueue->stateSync);
		if(theQueue->state == JVX_STATE_ACTIVE)
		{

			JVX_GET_TICKCOUNT_US_SETREF(&theQueue->runtime.tStampRef);

			theQueue->runtime.idWriteMessageQueue = 0;
			theQueue->runtime.numElementsInQueue = 0;
			theQueue->runtime.threadContinue = true;
			theQueue->runtime.trigger_clear_queue = false;
			theQueue->runtime.currentMessageInUse = false;

			theQueue->queueIsReady = true;
			JVX_INITIALIZE_NOTIFICATION(theQueue->runtime.reportNewData);
			
			JVX_INITIALIZE_NOTIFICATION(theQueue->runtime.reportThreadStarted);
			JVX_WAIT_FOR_NOTIFICATION_I(theQueue->runtime.reportThreadStarted);
			
			JVX_CREATE_THREAD(theQueue->runtime.threadHdl, jvx_message_queue_thread_callback, theQueue, theQueue->runtime.threadId);

			JVX_WAIT_FOR_NOTIFICATION_II_INF(theQueue->runtime.reportThreadStarted);

			theQueue->state = JVX_STATE_PROCESSING;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		JVX_UNLOCK_MUTEX(theQueue->stateSync);
		
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}
	

jvxErrorType 
jvx_message_queue_stop(jvxHandle* hdl)
{
	if(hdl)
	{
		oneMessageQueue* theQueue = (oneMessageQueue*)hdl;
		
		JVX_LOCK_MUTEX(theQueue->stateSync);
		if(theQueue->state == JVX_STATE_PROCESSING)
		{
			theQueue->runtime.threadContinue = false;
			JVX_SET_NOTIFICATION(theQueue->runtime.reportNewData);
			theQueue->state = JVX_STATE_PREPARED;
			JVX_UNLOCK_MUTEX(theQueue->stateSync);

			JVX_WAIT_RESULT res =  JVX_WAIT_FOR_THREAD_TERMINATE_INF(theQueue->runtime.threadHdl);
			if(res != JVX_WAIT_SUCCESS)
			{
				JVX_TERMINATE_THREAD(theQueue->runtime.threadHdl, -1);
			}

			JVX_TERMINATE_NOTIFICATION(theQueue->runtime.reportThreadStarted);
			JVX_TERMINATE_NOTIFICATION(theQueue->runtime.reportNewData);
			
			// Clear all messages from queue
			jvx_message_queue_clear_queue(hdl);
			
			JVX_LOCK_MUTEX(theQueue->stateSync);
			theQueue->state = JVX_STATE_ACTIVE;
			JVX_UNLOCK_MUTEX(theQueue->stateSync);

			return(JVX_NO_ERROR);
		}
		else
		{
			JVX_UNLOCK_MUTEX(theQueue->stateSync);
			return(JVX_ERROR_WRONG_STATE);
		}
	}
	
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
jvx_message_queue_trigger_stop(jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdl)
	{
		oneMessageQueue* theQueue = (oneMessageQueue*)hdl;
		JVX_LOCK_MUTEX(theQueue->stateSync);
		if(theQueue->state == JVX_STATE_PROCESSING)
		{
			theQueue->runtime.threadContinue = false;
			JVX_SET_NOTIFICATION(theQueue->runtime.reportNewData);
			theQueue->state = JVX_STATE_PREPARED;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		JVX_UNLOCK_MUTEX(theQueue->stateSync);
	}
	
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_message_queue_get_timestamp_us(jvxHandle* hdl, jvxTick* timestamp_us)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (hdl)
	{
		oneMessageQueue* theQueue = (oneMessageQueue*)hdl;
		if (timestamp_us)
			*timestamp_us = JVX_GET_TICKCOUNT_US_GET_CURRENT(&theQueue->runtime.tStampRef);
	}
	return res;
}

jvxErrorType 
jvx_message_queue_wait_for_stop(jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(hdl)
	{
		oneMessageQueue* theQueue = (oneMessageQueue*)hdl;

		JVX_LOCK_MUTEX(theQueue->stateSync); 
		if(theQueue->state == JVX_STATE_PREPARED)
		{
			theQueue->state = JVX_STATE_SELECTED; // Temp state
			JVX_UNLOCK_MUTEX(theQueue->stateSync); 

			// To prevent deadlocks, do not put this into a critical section!
			JVX_WAIT_RESULT res =  JVX_WAIT_FOR_THREAD_TERMINATE_INF(theQueue->runtime.threadHdl);
			if(res != JVX_WAIT_SUCCESS)
			{
				JVX_TERMINATE_THREAD(theQueue->runtime.threadHdl, -1);
			}

			JVX_TERMINATE_NOTIFICATION(theQueue->runtime.reportNewData);
			
			// Clear all messages from queue
			jvx_message_queue_clear_queue(hdl);

			JVX_LOCK_MUTEX(theQueue->stateSync); 
			theQueue->state = JVX_STATE_ACTIVE;			
			JVX_UNLOCK_MUTEX(theQueue->stateSync); 
		}
		else
		{
			JVX_UNLOCK_MUTEX(theQueue->stateSync); 
			res = JVX_ERROR_WRONG_STATE;
		}
		return(res);
	}
	
	return(JVX_ERROR_INVALID_ARGUMENT);
}


jvxErrorType 
jvx_message_queue_add_message_to_queue(jvxHandle* hdl, jvxSize contextId, jvxHandle* data, jvxSize szFld, jvxHandle* privateData_message)
{
	jvxBool lock_set = false;
	jvxErrorType res = JVX_NO_ERROR;
	if(hdl)
	{
		oneMessageQueue* theQueue = (oneMessageQueue*)hdl;
		JVX_LOCK_MUTEX(theQueue->stateSync); 
		JVX_THREAD_ID threadCtxt = JVX_GET_CURRENT_THREAD_ID();

		if(theQueue->state == JVX_STATE_PROCESSING)
		{
			if(theQueue->type == JVX_MESSAGE_QUEUE_DYNAMIC_OBJECTS)
			{
				// Copy content
			    jvxByte* tmp = NULL;
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(tmp, jvxByte, szFld);
				memcpy(tmp, data, szFld);				

				
				if (theQueue->runtime.threadId != threadCtxt)
				{
					lock_set = true;
					JVX_LOCK_MUTEX(theQueue->runtime.threadSync);
				}
				if(theQueue->queueIsReady)
				{
					if(theQueue->numElementsInQueueMax > theQueue->runtime.numElementsInQueue)
					{
						// Yes, there is enough space
						jvxSize idxWrite = 0;

						idxWrite = theQueue->runtime.idWriteMessageQueue;
						theQueue->runtime.idWriteMessageQueue = (theQueue->runtime.idWriteMessageQueue + 1)% theQueue->numElementsInQueueMax;
						theQueue->runtime.numElementsInQueue++;

						oneMessageInQueue* mess = &theQueue->messages[idxWrite];
						mess->data.fldSize = szFld;
						mess->data.fld = tmp;
						mess->data.tstamp_enter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&theQueue->runtime.tStampRef);
						mess->contextId = contextId;
						mess->priv_data_message = privateData_message;
					}
					else
					{
						res = JVX_ERROR_MESSAGE_QUEUE_FULL;
					}
				}
				else
				{
					res = JVX_ERROR_NOT_READY;
				}

				if (lock_set)
				{
					JVX_UNLOCK_MUTEX(theQueue->runtime.threadSync);
					JVX_SET_NOTIFICATION(theQueue->runtime.reportNewData);
				}
			}
			else
			{
				if(szFld <= theQueue->maxSizeContainerMessage)
				{
					if (theQueue->runtime.threadId != threadCtxt)
					{
						lock_set = true;
						JVX_LOCK_MUTEX(theQueue->runtime.threadSync);
					}
					if(theQueue->queueIsReady)
					{
						if(theQueue->numElementsInQueueMax > theQueue->runtime.numElementsInQueue)
						{
							// Yes, there is enough space
							jvxSize idxWrite = 0;
							idxWrite = theQueue->runtime.idWriteMessageQueue;
							theQueue->runtime.idWriteMessageQueue = (theQueue->runtime.idWriteMessageQueue + 1)% theQueue->numElementsInQueueMax;
							theQueue->runtime.numElementsInQueue++;
							oneMessageInQueue* mess = &theQueue->messages[idxWrite];
							mess->data.tstamp_enter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&theQueue->runtime.tStampRef);
							memcpy(mess->data.fld, data, szFld);
							mess->data.fldSize = szFld;
							mess->contextId = contextId;
							mess->priv_data_message = privateData_message;
						}
						else
						{
							res = JVX_ERROR_MESSAGE_QUEUE_FULL;
						}
					}
					else
					{
						res = JVX_ERROR_NOT_READY;
					}
					if (lock_set)
					{
						JVX_UNLOCK_MUTEX(theQueue->runtime.threadSync);
						JVX_SET_NOTIFICATION(theQueue->runtime.reportNewData);
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		JVX_UNLOCK_MUTEX(theQueue->stateSync); 
		
	}
	return(res);
}

jvxErrorType 
jvx_message_queue_trigger_queue(jvxHandle* hdl)
{
	if(hdl)
	{
		oneMessageQueue* theQueue = (oneMessageQueue*)hdl;
		if(theQueue->state == JVX_STATE_PROCESSING)
		{
			JVX_SET_NOTIFICATION(theQueue->runtime.reportNewData);
		}
		return(JVX_ERROR_WRONG_STATE);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
jvx_message_queue_clear_queue(jvxHandle* hdl)
{
	unsigned i;
	if(hdl)
	{
		oneMessageQueue* theQueue = (oneMessageQueue*)hdl;
		if((theQueue->state >= JVX_STATE_PREPARED) ||(theQueue->state == JVX_STATE_SELECTED)) // <- special rule seloected in case of trigger_stop
		{
			if(theQueue->queueIsReady)
			{
				if(theQueue->type == JVX_MESSAGE_QUEUE_DYNAMIC_OBJECTS)
				{
					// Remove all entries from queue
					for(i = 0; i < theQueue->numElementsInQueueMax; i++)
					{
						if(theQueue->messages[i].data.fld)
						{
						    JVX_DSP_SAFE_DELETE_FIELD_TYPE(theQueue->messages[i].data.fld, jvxByte);
							theQueue->messages[i].data.fld = NULL;
							theQueue->messages[i].data.fldSize = 0;
						}
					}
				}
				theQueue->runtime.numElementsInQueue = 0;
				theQueue->runtime.idWriteMessageQueue = 0;
				theQueue->queueIsReady = false;
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_WRONG_STATE);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
jvx_message_queue_trigger_clear_messages_queue(jvxHandle* hdl)
{
	jvxBool lock_set = false;
	unsigned i;
	if (hdl)
	{
		oneMessageQueue* theQueue = (oneMessageQueue*)hdl;
		if (theQueue->state >= JVX_STATE_PREPARED) 
		{
			jvxErrorType res = JVX_NO_ERROR;

			JVX_THREAD_ID threadCtxt = JVX_GET_CURRENT_THREAD_ID();

			if (theQueue->runtime.threadId != threadCtxt)
			{
				lock_set = true;
				JVX_LOCK_MUTEX(theQueue->runtime.threadSync);
			}

			if (theQueue->queueIsReady)
			{
				if (!theQueue->runtime.currentMessageInUse)
				{
					if (theQueue->type == JVX_MESSAGE_QUEUE_DYNAMIC_OBJECTS)
					{
						for (i = 0; i < theQueue->numElementsInQueueMax; i++)
						{
							if (theQueue->messages[i].data.fld)
							{
							    JVX_DSP_SAFE_DELETE_FIELD_TYPE(theQueue->messages[i].data.fld, jvxByte);
							}
						}
					}
					theQueue->runtime.numElementsInQueue = 0;
				}
				else
				{
					theQueue->runtime.trigger_clear_queue = true;
					res = JVX_ERROR_POSTPONE;
				}
			}
			if (lock_set)
			{
				JVX_UNLOCK_MUTEX(theQueue->runtime.threadSync);			
			}			
			return(res);
		}
		return(JVX_ERROR_WRONG_STATE);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
jvx_message_queue_num_message_in_queue(jvxHandle* hdl, jvxSize* numMessages)
{
	if(hdl)
	{
		oneMessageQueue* theQueue = (oneMessageQueue*)hdl;
		if(theQueue->state == JVX_STATE_PROCESSING)
		{	
			if(numMessages)
			{
				*numMessages = theQueue->runtime.numElementsInQueue;
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_WRONG_STATE);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
