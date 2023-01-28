#include "CjvxCircularBuffer.h"

#define LOOKBACK_DEBUG 1000
#define MIN(a,b) (a<b?a:b)

/*
 *
 *///=============================================================
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::CjvxCircularBuffer()
#else
CjvxCircularBuffer_extSynch::CjvxCircularBuffer_extSynch()
#endif
{
	buffer = NULL;
	form = JVX_DATAFORMAT_NONE;
	szElement = 0;
	bSize = 0;
	idxRead = 0;
	availableElements = 0;
	isReady = false;
	outinDelay = 0;
	// synchInOut;
	//synchInIn;
	//synchOutOut;
	

#ifdef DEBUG_BUFFER_HEIGHT
	backtrace = new debugStruct[LOOKBACK_DEBUG];
	memset(backtrace, 0, sizeof(debugStruct)*LOOKBACK_DEBUG);
	debugCnt = 0;
#endif
	
	
}

/*
 *
 *///=============================================================
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::~CjvxCircularBuffer()
#else
CjvxCircularBuffer_extSynch::~CjvxCircularBuffer_extSynch()
#endif
{
#ifdef DEBUG_BUFFER_HEIGHT
	delete[](backtrace);
#endif
}

/*
 *
 *///=============================================================
jvxErrorType 
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::initialize(jvxInt32 szBuffer, jvxDataFormat format)
#else
CjvxCircularBuffer_extSynch::initialize(jvxInt32 szBuffer, jvxDataFormat format)
#endif
{
	form = format;
	switch(form)
	{
		case JVX_DATAFORMAT_DATA:
			szElement = sizeof(jvxData);
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			szElement = sizeof(jvxInt32);
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			szElement = sizeof(jvxInt16);
			break;
		case JVX_DATAFORMAT_8BIT:
			szElement = sizeof(char);
			break;
		return(JVX_ERROR_INVALID_SETTING);
	}
	
	bSize = szBuffer;
	
	buffer = new char[bSize*szElement];
	memset(buffer, 0, bSize*szElement);
	
	idxRead = 0;
	
	availableElements = 0;
	
#ifdef SYNCHRONIZATION_INTERN
		JVX_INITIALIZE_MUTEX(synchInOut);
#endif
#ifdef SYNCH_ININOUTOUT
		JVX_INITIALIZE_MUTEX(synchInIn);
		JVX_INITIALIZE_MUTEX(synchOutOut);
#endif	

	
	isReady = true;
	return(JVX_NO_ERROR);
}

/*
 *
 *///=============================================================
jvxErrorType 
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::start(jvxInt32 outputinputDelay)
#else
CjvxCircularBuffer_extSynch::start(jvxInt32 outputinputDelay)
#endif
{
	if(isReady)
	{
		memset(buffer, 0, bSize*szElement);
		outinDelay = outputinputDelay;
		availableElements = outinDelay;
		return(JVX_NO_ERROR);	
	}
	return(JVX_ERROR_WRONG_STATE); 
}

/*
 *
 *///=============================================================
jvxErrorType 
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::stop()
#else
CjvxCircularBuffer_extSynch::stop()
#endif
{
	return(JVX_NO_ERROR); ;
}

/*
 *
 *///=============================================================
jvxErrorType 
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::terminate()
#else
CjvxCircularBuffer_extSynch::terminate()
#endif
{
	if(buffer)
	{
		delete[](buffer);
		buffer = NULL;
	}

	form = JVX_DATAFORMAT_NONE;
	szElement = 0;
	bSize = 0;
	idxRead = 0;
	availableElements = 0;
	isReady = false;
	outinDelay = 0;
	
#ifdef SYNCHRONIZATION_INTERN
	JVX_TERMINATE_MUTEX(synchInOut);
#endif
#ifdef SYNCH_ININOUTOUT
	JVX_TERMINATE_MUTEX(synchInIn);
	JVX_TERMINATE_MUTEX(synchOutOut);
#endif	
	return(JVX_NO_ERROR); ;
}

/*
 *
 *///=============================================================
jvxErrorType 
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::setValueDelay(jvxInt32 new_outputinputDelay)
#else
CjvxCircularBuffer_extSynch::setValueDelay(jvxInt32 new_outputinputDelay)
#endif
{
	outinDelay = new_outputinputDelay;
	return(JVX_NO_ERROR); ;
}

/*
 *
 *///=============================================================
jvxErrorType 
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::write(const char* fldIn, jvxInt32 numSamples, jvxInt32 modFHeight)
#else
CjvxCircularBuffer_extSynch::write(const char* fldIn, jvxInt32 numSamples, jvxInt32 modFHeight)
#endif
{
#ifdef SYNCH_ININOUTOUT
	JVX_LOCK_MUTEX(synchInIn);
#endif	
	
	if(numSamples + modFHeight + availableElements > bSize)
	{
#ifdef SYNCH_ININOUTOUT
		JVX_UNLOCK_MUTEX(synchInIn);
#endif	

		// Input overflow
		return(JVX_ERROR_BUFFER_OVERFLOW);
	}
	
	// Compute index to write from index to read
#ifdef SYNCHRONIZATION_INTERN
	JVX_LOCK_MUTEX(synchInOut);
#endif	
	int idxWrite = idxRead + availableElements;
#ifdef SYNCHRONIZATION_INTERN
	JVX_UNLOCK_MUTEX(synchInOut);
#endif	
	
	if(idxWrite >= bSize)
		idxWrite -= bSize;
	
	int l1 = MIN(numSamples, (bSize-idxWrite));
	int l2 = numSamples-l1;
	
	char* ptrWrite = buffer + (szElement*idxWrite);
	
	if(l1 > 0)
		memcpy(ptrWrite, fldIn, l1*szElement);
	
	// Increment input pointer
	fldIn += szElement*l1;
	
	// Restart at the beginning (wrap around)
	if(l2 > 0)
		memcpy(buffer, fldIn, l2*szElement);
	
#ifdef SYNCHRONIZATION_INTERN
	JVX_LOCK_MUTEX(synchInOut);
#endif	
	availableElements += numSamples + modFHeight;
#ifdef SYNCHRONIZATION_INTERN
	JVX_UNLOCK_MUTEX(synchInOut);
#endif	

#ifdef SYNCH_ININOUTOUT
	JVX_UNLOCK_MUTEX(synchInIn);
#endif	
	return(JVX_NO_ERROR); ;
}

/*
 *
 *///=============================================================
jvxErrorType 
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::write_sim(jvxInt32 numSamples)
#else
CjvxCircularBuffer_extSynch::write_sim(jvxInt32 numSamples)
#endif
{
#ifdef SYNCH_ININOUTOUT
	JVX_LOCK_MUTEX(synchInIn);
#endif	
	
	if(numSamples + availableElements > bSize)
	{
#ifdef SYNCH_ININOUTOUT
		JVX_UNLOCK_MUTEX(synchInIn);
#endif	
		
		// Input overflow
		return(JVX_ERROR_BUFFER_OVERFLOW);
	}
	
	// Compute index to write from index to read
#ifdef SYNCHRONIZATION_INTERN
	JVX_LOCK_MUTEX(synchInOut);
#endif	
	int idxWrite = idxRead + availableElements;
#ifdef SYNCHRONIZATION_INTERN
	JVX_UNLOCK_MUTEX(synchInOut);
#endif	
	
	if(idxWrite >= bSize)
		idxWrite -= bSize;
		
#ifdef SYNCH_ININOUTOUT
	JVX_LOCK_MUTEX(synchInIn);
#endif	
	availableElements += numSamples;
#ifdef SYNCH_ININOUTOUT
	JVX_UNLOCK_MUTEX(synchInIn);
#endif	
	
#ifdef SYNCH_ININOUTOUT
	JVX_UNLOCK_MUTEX(synchInIn);
#endif	
	return(JVX_NO_ERROR); ;
}

/*
 *
 *///=============================================================
jvxErrorType 
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::read(char* fldOut, jvxInt32 numSamples)
#else
CjvxCircularBuffer_extSynch::read(char* fldOut, jvxInt32 numSamples)
#endif
{
#ifdef SYNCH_ININOUTOUT
	JVX_LOCK_MUTEX(synchOutOut);
#endif	
	if(numSamples > availableElements)
	{
		// Output underflow
#ifdef SYNCH_ININOUTOUT
		JVX_UNLOCK_MUTEX(synchOutOut);
#endif	
		return(JVX_ERROR_BUFFER_UNDERRUN);
	}
	
	int l1 = MIN(numSamples, (bSize-idxRead));
	int l2 = numSamples-l1;
	
	char* ptrRead = buffer + (szElement*idxRead);
	
	if(l1 > 0)
		memcpy(fldOut, ptrRead, l1*szElement);
	
	// Increment output pointer
	fldOut += l1*szElement;
	
	if(l2 > 0)
	{
		memcpy(fldOut, buffer, l2*szElement);
	}
	
#ifdef SYNCHRONIZATION_INTERN
	JVX_LOCK_MUTEX(synchInOut);
#endif	
	availableElements -= numSamples;

#ifdef SYNCHRONIZATION_INTERN
	JVX_UNLOCK_MUTEX(synchInOut);
#endif		
	// increment read counter
	idxRead += numSamples;
	if(idxRead >= bSize)
		idxRead -= bSize;
		
#ifdef SYNCH_ININOUTOUT
	JVX_LOCK_MUTEX(synchOutOut);
#endif	
	return(JVX_NO_ERROR); ;
}

/*
 *
 *///=============================================================
jvxErrorType 
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::read_sim(jvxInt32 numSamples)
#else
CjvxCircularBuffer_extSynch::read_sim(jvxInt32 numSamples)
#endif
{
#ifdef SYNCH_ININOUTOUT
	JVX_LOCK_MUTEX(synchOutOut);
#endif	
	if(numSamples > availableElements)
	{
		// Output underflow
#ifdef SYNCH_ININOUTOUT
		JVX_UNLOCK_MUTEX(synchOutOut);
#endif	
		return(JVX_ERROR_BUFFER_UNDERRUN);
	}
	
#ifdef SYNCHRONIZATION_INTERN
	JVX_LOCK_MUTEX(synchInOut);
#endif	
	availableElements -= numSamples;
	
	// increment read counter
	idxRead += numSamples;
	if(idxRead >= bSize)
		idxRead -= bSize;
	
#ifdef SYNCHRONIZATION_INTERN
	JVX_UNLOCK_MUTEX(synchInOut);
#endif	
	
#ifdef SYNCH_ININOUTOUT
	JVX_LOCK_MUTEX(synchOutOut);
#endif	
	return(JVX_NO_ERROR); ;
}

/*
 *
 *///=============================================================
jvxErrorType 
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::spaceInBuffer(jvxInt32* numSamples)
#else
CjvxCircularBuffer_extSynch::spaceInBuffer(jvxInt32* numSamples)
#endif
{
	if(numSamples)
	{
		*numSamples = bSize-availableElements;
		return(JVX_NO_ERROR); ;
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

/*
 *
 *///=============================================================
jvxErrorType 
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::elementsInBuffer(jvxInt32* numSamples)
#else
CjvxCircularBuffer_extSynch::elementsInBuffer(jvxInt32* numSamples)
#endif
{
	if(numSamples)
	{
		*numSamples = availableElements;
		return(JVX_NO_ERROR); ;
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

/*
 *
 *///=============================================================
jvxErrorType
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::reset()
#else
CjvxCircularBuffer_extSynch::reset()
#endif
{
	if(bSize)
	{
		memset(buffer, 0, szElement*bSize);
	}
	return(JVX_NO_ERROR); ;
}

/*
 *
 *///=============================================================
jvxErrorType
#ifdef SYNCHRONIZATION_INTERN
CjvxCircularBuffer::fillHeight(jvxInt32* retObj)
#else
CjvxCircularBuffer_extSynch::fillHeight(jvxInt32* retObj)
#endif
{
	if(retObj)
	{
		*retObj = availableElements;
		return(JVX_NO_ERROR); ;
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

