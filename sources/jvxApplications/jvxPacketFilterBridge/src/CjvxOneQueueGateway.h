#ifndef __JVXONEQUEUEGATEWAY_H__
#define __JVXONEQUEUEGATEWAY_H__

#include "jvx.h"
#include "CjvxOnePacketQueue.h"

class uMainWindow;

class CjvxOneQueueGateway : public IjvxOnePacketQueue_send, public IjvxOnePacketQueue_queue, public IjvxOnePacketQueue_report
{

public:

	typedef enum
	{
		JVX_PACKETFILTER_NONE,
		JVX_PACKETFILTER_LEFT_TO_RIGHT,
		JVX_PACKETFILTER_RIGHT_TO_LEFT
	} packetQueueType;

private:
	packetQueueType association;
	CjvxOnePacketQueue theQueue;
	uMainWindow* myRef;

public:

	CjvxOneQueueGateway();
	~CjvxOneQueueGateway();

	jvxErrorType initialize(uMainWindow* ref, packetQueueType tp, jvxSize numberEntriesMax, 
		jvxSize maxLengthPacket, jvxSize defTimeout_msec, IjvxOnePacketQueue_send* irregularLnk, const std::string& nmQueue);

	jvxErrorType terminate();

	virtual jvxErrorType JVX_CALLINGCONVENTION add_one_packet(jvxInt64 deltaT, jvxByte* fld, jvxSize fldSize, jvxHandle* ptr_tag_add, jvxSize id_tag_add) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION send_one_packet(jvxByte* fld, jvxSize fldSize) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION filter_packet(jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_tag_add, jvxSize id_tag_add, jvxBool* packet_remove) override;

	void data_admin(jvxSize& num_lost, jvxSize& numEntries);
};

#endif
