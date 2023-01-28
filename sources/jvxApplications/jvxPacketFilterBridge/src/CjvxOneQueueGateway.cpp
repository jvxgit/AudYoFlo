#include "CjvxOneQueueGateway.h"
#include "uMainWindow.h"

CjvxOneQueueGateway::CjvxOneQueueGateway()
{
	association = JVX_PACKETFILTER_LEFT_TO_RIGHT;
}

CjvxOneQueueGateway::~CjvxOneQueueGateway()
{
}

jvxErrorType
CjvxOneQueueGateway::initialize(uMainWindow* ref, packetQueueType tp, jvxSize numberEntriesMax, 
	jvxSize maxLengthPacket, jvxSize defTimeout_msec, IjvxOnePacketQueue_send* irregularLnk, 
	const std::string& nmQueue)
{
	association = tp;
	myRef = ref;
	return theQueue.initialize(numberEntriesMax, maxLengthPacket, defTimeout_msec, this, this, nmQueue);
}

jvxErrorType 
CjvxOneQueueGateway::terminate()
{
	association = JVX_PACKETFILTER_NONE;
	myRef = NULL;
	return theQueue.terminate();
}

jvxErrorType 
CjvxOneQueueGateway::add_one_packet(jvxInt64 deltaT, jvxByte* fld, jvxSize fldSize, jvxHandle* ptr_tag_add, jvxSize id_tag_add)
{
	return theQueue.add_one_packet(deltaT, fld, fldSize, ptr_tag_add, id_tag_add);
}

jvxErrorType 
CjvxOneQueueGateway::send_one_packet(jvxByte* fld, jvxSize fldSize)
{
	switch (association)
	{
	case JVX_PACKETFILTER_LEFT_TO_RIGHT:
		return(myRef->send_to_right(fld, fldSize));
		break;
	case JVX_PACKETFILTER_RIGHT_TO_LEFT:
		return(myRef->send_to_left(fld, fldSize));
		break;
	default:
		assert(0);
	}

	return(JVX_ERROR_INTERNAL);
}

jvxErrorType
CjvxOneQueueGateway::filter_packet(jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_tag_add, jvxSize id_tag_add, jvxBool* packet_remove)
{
	switch (association)
	{
	case JVX_PACKETFILTER_LEFT_TO_RIGHT:
		return(myRef->filter_packet_left_to_right(fld, numBytesUsed, ptr_tag_add, id_tag_add, packet_remove));
		break;
	case JVX_PACKETFILTER_RIGHT_TO_LEFT:
		return(myRef->filter_packet_right_to_left(fld, numBytesUsed, ptr_tag_add, id_tag_add, packet_remove));
		break;
	default:
		assert(0);
	}

	return(JVX_ERROR_INTERNAL);
}

void 
CjvxOneQueueGateway::data_admin(jvxSize& num_lost, jvxSize& numEntries)
{
	theQueue.data_admin(num_lost, numEntries);
}
