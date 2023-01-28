#ifndef _CJVXONEPACKETQUEUE_H__
#define _CJVXONEPACKETQUEUE_H__

#include "jvx.h"
#include "jvx_threads.h"
#include "jvx_connect_client_st.h"

#define JVX_ALPHA_SMOOTH_MAX 0.99

class CjvxOnePacketQueue
{
private:

	typedef struct
	{
		jvxByte* onePacket;
		jvxUInt64 tStampIn;
		jvxUInt64 tStampOut;
		jvxSize currentlyUsedBytes;
		jvxSize id_tag_add;
		jvxHandle* ptr_tag_add;
	} jvxOnePacketInQueue;
	
	jvxOnePacketInQueue* theQueuePackets;
	jvxSize numberQueueSpaces;
	jvxSize numberBytesPerEntry;
	jvxSize numberEntriesCurrent;
	jvxData numberEntriesMax;
	jvxSize numberLostEntries;
	
	JVX_MUTEX_HANDLE safeAccess;
	jvxState _state;
	jvxHandle* theOutputThread;

	jvx_thread_handler myThreadHandler;
	jvxSize defaultTimeout_ms;

	IjvxOnePacketQueue_send* regularOutput;
	IjvxOnePacketQueue_report* processingLink;

	std::string myName;

	jvxByte* bufCopy;
	jvxSize szBufCopy;
	jvxSize idBufCopy;
	jvxHandle* ptrBufCopy;

public:
	
	CjvxOnePacketQueue();
	~CjvxOnePacketQueue();
	jvxErrorType initialize(jvxSize numberEntriesMax, jvxSize maxLengthPacket, 
		jvxSize defTimeout_msec, IjvxOnePacketQueue_send* outOtherLink, 
		IjvxOnePacketQueue_report* processPacketRef, const std::string& name);

	jvxErrorType terminate();
	
	jvxErrorType add_one_packet(jvxInt64 deltaTOut, jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_tag_add, jvxSize id_tag_add);
	jvxErrorType checkQueue(jvxInt64 tStamp, jvxSize* deltaT_msec);

	static jvxErrorType st_callback_thread_startup(jvxHandle* privateData_thread, jvxInt64 timestamp_us);
	static jvxErrorType st_callback_thread_timer_expired(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms);
	static jvxErrorType st_callback_thread_wokeup(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms);
	static jvxErrorType st_callback_thread_stopped(jvxHandle* privateData_thread, jvxInt64 timestamp_us);

	jvxErrorType ic_callback_thread_startup(jvxInt64 timestamp_us);
	jvxErrorType ic_callback_thread_timer_expired(jvxInt64 timestamp_us, jvxSize* delta_ms);
	jvxErrorType ic_callback_thread_wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms);
	jvxErrorType ic_callback_thread_stopped(jvxInt64 timestamp_us);

	void data_admin(jvxSize& num_lost, jvxSize& numEntries);

};

#endif
	
		
		