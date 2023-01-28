#ifndef __IJVXPACKETFILTER_H__
#define __IJVXPACKETFILTER_H__

JVX_INTERFACE IjvxOnePacketQueue_send
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxOnePacketQueue_send() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION send_one_packet(jvxByte* fld, jvxSize fldSize) = 0;
};

JVX_INTERFACE IjvxOnePacketQueue_queue
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxOnePacketQueue_queue() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION add_one_packet(jvxInt64 deltaTOut, jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_tag_add, jvxSize id_tag_add) = 0;
};

JVX_INTERFACE IjvxOnePacketQueue_report
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxOnePacketQueue_report() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION filter_packet(jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_tag_add, jvxSize id_ptr_tag, jvxBool* packet_remove) = 0;
};

JVX_INTERFACE IjvxPacketFilterRule: public IjvxObject, public IjvxHiddenInterface
{
public:
	virtual ~IjvxPacketFilterRule(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hRef) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION preprare_rule(jvxSize context_id, 
		IjvxOnePacketQueue_queue*regularQueue, IjvxOnePacketQueue_send* regularSend,
		IjvxOnePacketQueue_queue* irregularQueue, IjvxOnePacketQueue_send* irregularSend) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_rule(jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_tag_add, jvxSize id_tag_add, jvxSize context_id, jvxBool* packet_remove) = 0;
		
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_rule(jvxSize context_id) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() = 0;
		
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;
};

#endif