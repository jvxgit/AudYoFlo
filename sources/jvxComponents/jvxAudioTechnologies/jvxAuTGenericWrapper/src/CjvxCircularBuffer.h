#ifndef __CJVXCIRCULARBUFFER_H__
#define __CJVXCIRCULARBUFFER_H__

#include "jvx.h"

#define DEBUG_BUFFER_HEIGHT
#define SYNCHRONIZATION_INTERN

#ifdef SYNCHRONIZATION_INTERN
JVX_INTERFACE CjvxCircularBuffer
#else
JVX_INTERFACE CjvxCircularBuffer_extSynch
#endif
{
public:
#ifdef SYNCHRONIZATION_INTERN
	CjvxCircularBuffer();
	virtual ~CjvxCircularBuffer();
#else
	CjvxCircularBuffer_extSynch();
	~CjvxCircularBuffer_extSynch();
#endif	
	virtual jvxErrorType initialize(jvxInt32 szBuffer, jvxDataFormat format);
	virtual jvxErrorType start(jvxInt32 outputinputDelay);
	virtual jvxErrorType stop();
	virtual jvxErrorType terminate();
	virtual jvxErrorType setValueDelay(jvxInt32 new_outputinputDelay);
	virtual jvxErrorType  write(const char* fldIn, jvxInt32 numSamples, jvxInt32 modFHeight);
	virtual jvxErrorType read(char* fldIn, jvxInt32 numSamples);
	virtual jvxErrorType  write_sim(jvxInt32 numSamples);
	virtual jvxErrorType read_sim(jvxInt32 numSamples);
	virtual jvxErrorType spaceInBuffer(jvxInt32* numSamples);
	virtual jvxErrorType elementsInBuffer(jvxInt32* numSamples);
	virtual jvxErrorType reset();
	virtual jvxErrorType fillHeight(jvxInt32*);
	
private:

#ifdef DEBUG_BUFFER_HEIGHT
	typedef enum
	{
		OPERATION_READ,
		OPERATION_WRITE
	} operation;		

	typedef enum
	{
		OPERATION_SUCCESS,
		OPERATION_FAILED
	} success;
	
	typedef struct
	{
		int bHeightBeforeOp;
		int bHeightAfterOp;
		operation op;
		success suc;
		int numValues;
	} debugStruct;
		
	debugStruct* backtrace;
	
	int debugCnt;
#endif
	
	char* buffer;
	jvxDataFormat form;
	size_t szElement;
	int bSize;
	int idxRead;
	int availableElements;
	bool isReady;
	int outinDelay;

#ifdef SYNCHRONIZATION_INTERN	
	JVX_MUTEX_HANDLE synchInOut;
#endif

#ifdef SYNCH_ININOUTOUT
	JVX_MUTEX_HANDLE synchInIn;
	JVX_MUTEX_HANDLE synchOutOut;
#endif
};

#endif

