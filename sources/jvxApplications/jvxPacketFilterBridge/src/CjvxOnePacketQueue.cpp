#include "CjvxOnePacketQueue.h"

CjvxOnePacketQueue::CjvxOnePacketQueue()
{
	JVX_INITIALIZE_MUTEX(safeAccess);
	theQueuePackets = NULL;
	numberQueueSpaces = 0;
	_state = JVX_STATE_NONE;

	myThreadHandler.callback_thread_startup = st_callback_thread_startup;
	myThreadHandler.callback_thread_stopped = st_callback_thread_stopped;
	myThreadHandler.callback_thread_timer_expired = st_callback_thread_timer_expired;
	myThreadHandler.callback_thread_wokeup = st_callback_thread_wokeup;

	myName = "IDONOTKNOW";
}

CjvxOnePacketQueue::~CjvxOnePacketQueue()
{
	if(_state == JVX_STATE_INIT)
	{
		terminate();
	}
	JVX_TERMINATE_MUTEX(safeAccess);
}

jvxErrorType 
CjvxOnePacketQueue::initialize(jvxSize numberEntriesMax, jvxSize maxLengthPacket,
		jvxSize defTimeout_msec, IjvxOnePacketQueue_send* outRegular,
		IjvxOnePacketQueue_report* processPacketRef, const std::string& name)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccess);
	if(_state == JVX_STATE_NONE)
	{
		myName = name;
		regularOutput = outRegular;
		processingLink = processPacketRef;
		defaultTimeout_ms = defTimeout_msec;
		theQueuePackets = NULL;
		JVX_SAFE_NEW_FLD(theQueuePackets, jvxOnePacketInQueue, numberEntriesMax);
		for(i = 0; i < numberEntriesMax; i++)
		{
			JVX_SAFE_NEW_FLD(theQueuePackets[i].onePacket, jvxByte, maxLengthPacket);
			theQueuePackets[i].currentlyUsedBytes = JVX_SIZE_UNSELECTED;
			theQueuePackets[i].tStampIn = JVX_SIZE_UNSELECTED;
			theQueuePackets[i].tStampOut = JVX_SIZE_UNSELECTED;
		}
		numberQueueSpaces = numberEntriesMax;
		numberBytesPerEntry = maxLengthPacket;
		numberEntriesCurrent = 0;
		numberLostEntries = 0;

		JVX_SAFE_NEW_FLD(bufCopy, jvxByte, maxLengthPacket);
		szBufCopy = 0;

		resL = jvx_thread_initialize(&theOutputThread, &myThreadHandler, reinterpret_cast<jvxHandle*>(this),
			true);
		assert(resL == JVX_NO_ERROR);

		resL = jvx_thread_start(theOutputThread, defaultTimeout_ms);
		assert(resL == JVX_NO_ERROR);

		resL = jvx_thread_set_priority(theOutputThread, JVX_THREAD_PRIORITY_REALTIME);
		assert(resL == JVX_NO_ERROR);
		
		_state = JVX_STATE_INIT;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(safeAccess); 
	return(res);
}

jvxErrorType 
CjvxOnePacketQueue::terminate()
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	JVX_LOCK_MUTEX(safeAccess);
	if(_state == JVX_STATE_INIT)
	{
		resL = jvx_thread_trigger_stop(theOutputThread);
		assert(resL == JVX_NO_ERROR);

		resL = jvx_thread_wait_stop(theOutputThread, 1000);
		assert(resL == JVX_NO_ERROR);

		_state = JVX_STATE_NONE;

		JVX_SAFE_DELETE_FLD(bufCopy, jvxByte);
		szBufCopy = 0;

		for(i = 0; i < numberQueueSpaces; i++)
		{
			JVX_SAFE_DELETE_FLD(theQueuePackets[i].onePacket, jvxByte);
			theQueuePackets[i].currentlyUsedBytes = JVX_SIZE_UNSELECTED;
			theQueuePackets[i].tStampIn = JVX_SIZE_UNSELECTED;
			theQueuePackets[i].tStampOut = JVX_SIZE_UNSELECTED;
			theQueuePackets[i].ptr_tag_add = NULL;
			theQueuePackets[i].id_tag_add = JVX_SIZE_UNSELECTED;
		}
		numberQueueSpaces = 0;
		numberBytesPerEntry = 0;

		JVX_SAFE_DELETE_FLD(theQueuePackets, jvxOnePacketInQueue);
		theQueuePackets = NULL;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(safeAccess);
	return(res);

}
	
jvxErrorType 
CjvxOnePacketQueue::add_one_packet(jvxInt64 deltaT, jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_tag_add, jvxSize id_tag_add)
{
	jvxSize i;
	jvxSize idxAdd = JVX_SIZE_UNSELECTED;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool informThread = false;

	jvxInt64 tStampIn = 0;

	JVX_LOCK_MUTEX(safeAccess);
	if (_state == JVX_STATE_INIT)
	{
		jvx_threads_get_time(theOutputThread, &tStampIn);

		for (i = 0; i < numberQueueSpaces; i++)
		{
			if (theQueuePackets[i].currentlyUsedBytes == JVX_SIZE_UNSELECTED)
			{
				memcpy(theQueuePackets[i].onePacket, fld, numBytesUsed);
				theQueuePackets[i].currentlyUsedBytes = numBytesUsed;
				theQueuePackets[i].tStampIn = tStampIn;
				theQueuePackets[i].tStampOut = tStampIn + deltaT;
				theQueuePackets[i].id_tag_add = id_tag_add;
				theQueuePackets[i].ptr_tag_add = ptr_tag_add;
				informThread = true;
				numberEntriesCurrent++;
				if (numberEntriesCurrent > numberEntriesMax)
					numberEntriesMax = (jvxData)numberEntriesCurrent;
				break;
			}
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(safeAccess);

	// Notify thread that new packages have arrived
	if (informThread)
	{
		jvx_thread_wakeup(theOutputThread);
	}
	else
	{
		numberLostEntries++;
	}

	return(res);
}

jvxErrorType
CjvxOnePacketQueue::checkQueue(jvxInt64 tStamp, jvxSize* deltaT_msec)
{
	jvxSize i;
	jvxBool requestSend = false;
	jvxBool packetRemoved = false;
	while (1)
	{
		JVX_LOCK_MUTEX(safeAccess);
		if (_state == JVX_STATE_INIT)
		{
			requestSend = false;

			for (i = 0; i < numberQueueSpaces; i++)
			{
				if (JVX_CHECK_SIZE_SELECTED(theQueuePackets[i].currentlyUsedBytes))
				{
					if (theQueuePackets[i].tStampOut <= tStamp)
					{
						// Copy a package and set flag to "send"
						szBufCopy = theQueuePackets[i].currentlyUsedBytes;
						memcpy(bufCopy, theQueuePackets[i].onePacket, szBufCopy);
						idBufCopy = theQueuePackets[i].id_tag_add;
						ptrBufCopy = theQueuePackets[i].ptr_tag_add;

						// Free Queue space
						theQueuePackets[i].currentlyUsedBytes = JVX_SIZE_UNSELECTED;
						theQueuePackets[i].tStampIn = JVX_SIZE_UNSELECTED;
						theQueuePackets[i].tStampOut = JVX_SIZE_UNSELECTED;
						theQueuePackets[i].id_tag_add = JVX_SIZE_UNSELECTED;
						theQueuePackets[i].ptr_tag_add = NULL;
						numberEntriesCurrent--;
						numberEntriesMax *= JVX_ALPHA_SMOOTH_MAX;
						requestSend = true;
						break;
					}
				}
			}
		}
		JVX_UNLOCK_MUTEX(safeAccess);

		if (requestSend)
		{
			if (processingLink)
			{
				jvxErrorType res = processingLink->filter_packet(bufCopy, szBufCopy, ptrBufCopy, idBufCopy, &packetRemoved);
				if (res != JVX_NO_ERROR)
				{
					std::cout << "Error filtering packet." << std::endl;
				}
			}
			if (!packetRemoved)
			{
				// Do send the package
				regularOutput->send_one_packet(bufCopy, szBufCopy);
			}
		}
		else
		{
			// Come back later
			break;
		}
	}
	return JVX_NO_ERROR;
}

// =============================================================================
// Timer threads
// =============================================================================

jvxErrorType 
CjvxOnePacketQueue::st_callback_thread_startup(jvxHandle* privateData_thread, jvxInt64 timestamp_us)
{
	CjvxOnePacketQueue* this_pointer = (CjvxOnePacketQueue*)privateData_thread;
	assert(this_pointer);
	return(this_pointer->ic_callback_thread_startup(timestamp_us));
}

jvxErrorType 
CjvxOnePacketQueue::st_callback_thread_timer_expired(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	CjvxOnePacketQueue* this_pointer = (CjvxOnePacketQueue*)privateData_thread;
	assert(this_pointer);
	return(this_pointer->ic_callback_thread_timer_expired(timestamp_us, delta_ms));
}

jvxErrorType 
CjvxOnePacketQueue::st_callback_thread_wokeup(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	CjvxOnePacketQueue* this_pointer = (CjvxOnePacketQueue*)privateData_thread;
	assert(this_pointer);
	return(this_pointer->ic_callback_thread_wokeup(timestamp_us, delta_ms));
}

jvxErrorType 
CjvxOnePacketQueue::st_callback_thread_stopped(jvxHandle* privateData_thread, jvxInt64 timestamp_us)
{
	CjvxOnePacketQueue* this_pointer = (CjvxOnePacketQueue*)privateData_thread;
	assert(this_pointer);
	return(this_pointer->ic_callback_thread_stopped(timestamp_us));
}

jvxErrorType CjvxOnePacketQueue::ic_callback_thread_startup(jvxInt64 timestamp_us)
{
	std::cout << "Startup thread " << myName << std::endl;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxOnePacketQueue::ic_callback_thread_timer_expired(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	this->checkQueue(timestamp_us, delta_ms);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxOnePacketQueue::ic_callback_thread_wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	this->checkQueue(timestamp_us, delta_ms);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxOnePacketQueue::ic_callback_thread_stopped(jvxInt64 timestamp_us)
{
	std::cout << "Stop thread " << myName << std::endl;
	return JVX_NO_ERROR;
}

void 
CjvxOnePacketQueue::data_admin(jvxSize& num_lost, jvxSize& numEntries)
{
	num_lost = numberLostEntries;
	numEntries = JVX_DATA2SIZE( numberEntriesMax);
}