#ifndef _CJVXWEBCONTROL_FE_TYPES_H__
#define _CJVXWEBCONTROL_FE_TYPES_H__

#include "jvx.h"

class HjvxPropertyStreamUdpSocket;

typedef enum
{
	JVX_PROPERTY_WEBSOCKET_STATE_NONE,
	JVX_PROPERTY_WEBSOCKET_STATE_IN_TRANSFER
}jvxStreamPropertyTransferState;

class onePropertyWebSocketDefinition
{
public:
	std::string propertyName;
	jvxComponentIdentification cpId;
	jvxDataFormat format_spec;
	jvxSize numElms_spec;
	jvxPropertyDecoderHintType htTp;
	jvxSize offset;
	jvxSize uniqueId;
	jvxUInt32 param0;
	jvxUInt32 param1;
	jvxPropertyTransferPriority prio;
	jvxState state_active;
	jvxPropertyStreamCondUpdate cond_update;
	jvxSize param_cond_update;
	jvxUInt32 tStamp;
	jvxStreamPropertyTransferState state_transfer;
	jvxSize streamPropertyInTransferState;
	//jvxSize streamPropertyInTransferStateMax;
	jvxBool requiresFlowControl;
	jvxSize num_emit_min;
	jvxSize num_emit_max;

	struct
	{
		jvxInt32 port;
		std::string target;
		
	} high_prio;

	struct
	{
		jvx_propertyReferenceTriple theTriple;
		jvxPropertyDescriptor descr;
		jvxByte* allocatedRawBuffer;
		jvxSize szRawBuffer;
		jvxHandle* referencedPayloadBuffer;
		jvxState transferState;
		jvxSize cntTicks;

		jvxExternalBuffer* specbuffer;
		jvxSize specbuffer_sz;
		jvxBool specbuffer_valid;

		jvxBool serious_fail;
		jvxData last_send_msec;
		jvxUInt32 latest_tstamp;

		struct
		{
			HjvxPropertyStreamUdpSocket* udpSocket;
		} prio_high;

		// jvxOnePropertyWebSocketTransferState messageTransferState;
	} runtime;

	onePropertyWebSocketDefinition()
	{
		//std::string propertyName;
		cpId = JVX_COMPONENT_UNKNOWN;
		format_spec = JVX_DATAFORMAT_NONE;
		numElms_spec = 0;
		htTp = JVX_PROPERTY_DECODER_NONE;
		offset = 0;
		uniqueId = JVX_SIZE_UNSELECTED;
		param0 = 0;
		param1 = 0;
		prio = JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO;
		state_active = JVX_STATE_NONE;
		cond_update = JVX_PROP_STREAM_UPDATE_TIMEOUT;
		param_cond_update = JVX_SIZE_UNSELECTED;
		tStamp = 0;
		state_transfer = JVX_PROPERTY_WEBSOCKET_STATE_NONE;
		streamPropertyInTransferState = 0;
		//jvxSize streamPropertyInTransferStateMax;
		requiresFlowControl = false;
		num_emit_min = 0;
		num_emit_max = JVX_SIZE_UNSELECTED;
		high_prio.port = 0;
		
		// high_prio.target;

		jvx_initPropertyReferenceTriple(&runtime.theTriple);
		jvx_initPropertyDescription(runtime.descr);
		runtime.allocatedRawBuffer = NULL;
		runtime.szRawBuffer = 0;
		runtime.referencedPayloadBuffer = NULL;
		runtime.transferState = JVX_STATE_NONE;
		runtime.cntTicks = 0;
		runtime.specbuffer = NULL;
		runtime.specbuffer_sz = 0;
		runtime.specbuffer_valid = false;
		runtime.serious_fail = false;
		runtime.last_send_msec = 0;
		runtime.latest_tstamp = 0;
		runtime.prio_high.udpSocket = NULL;

	};

	~onePropertyWebSocketDefinition()
	{
	

	};
};

// ===================================================================
class jvxWebContext
{
public:
	jvxHandle* context_conn = nullptr;
	jvxHandle* context_server = nullptr;
	std::string uri;
	void reset()
	{
		context_conn = nullptr;
		context_server = nullptr;
		uri.clear();
	}
};

class jvxWsConnection
{
public:
	jvxWebContext theCtxt;
	std::string query;
	std::string local_uri;
	std::string url_origin;
	std::string user;
	void reset()
	{
		theCtxt.reset();
		query.clear();
		local_uri.clear();
		url_origin.clear();
		user.clear();
	}
};

class oneWsConnectionPeriodicDeploy : public jvxWsConnection
{
public:
	jvxSize timeout_msec = 100;
	jvxSize ping_cnt_trigger = JVX_SIZE_UNSELECTED;
	jvxSize ping_cnt_close = JVX_SIZE_UNSELECTED;
	jvxSize ping_cnt_close_failed = JVX_SIZE_UNSELECTED;
	jvxSize wsMessId = JVX_SIZE_UNSELECTED;
	jvxSize current_ping_count = JVX_SIZE_UNSELECTED;

	void reset()
	{
		jvxWsConnection::reset();
		timeout_msec = 100;
		ping_cnt_trigger = JVX_SIZE_UNSELECTED;
		ping_cnt_close = JVX_SIZE_UNSELECTED;
		ping_cnt_close_failed = JVX_SIZE_UNSELECTED;
		wsMessId = JVX_SIZE_UNSELECTED;
		current_ping_count = JVX_SIZE_UNSELECTED;
	}
};

#endif
