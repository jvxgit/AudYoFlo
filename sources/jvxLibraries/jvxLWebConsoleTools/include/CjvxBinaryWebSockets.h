#ifndef __CJVXBINARYWEBSOCKETS_H__
#define __CJVXBINARYWEBSOCKETS_H__

#include "CjvxWebControl_fe_types.h"
#include "jvx-net-helpers.h"

class CjvxWebControl_fe;

// ===================================================================
class CjvxBinaryWebSockets: public HjvxPropertyStreamUdpSocket_report
{
private:
	CjvxWebControl_fe* hostRef = nullptr;

public:

	struct
	{
		std::map<jvxInt32, HjvxPropertyStreamUdpSocket*> theUdpSockets;
	} high_prio_transfer;

	jvxTimeStampData tStamp;

	jvxSize wsUniqueId = 1;
	std::map<jvxSize, onePropertyWebSocketDefinition> lstUpdateProperties;

	// This is the binary socket of which there is always only one
	oneWsConnectionPeriodicDeploy webSocketPeriodic;

	CjvxBinaryWebSockets();

	void force_stop_properties(jvxHandle* hdl);
	void register_binary_socket_main_loop(jvxWebContext* ctxt);
	jvxErrorType unregister_binary_socket_main_loop(jvxHandle* refCtxt);

	void initialize(CjvxWebControl_fe* hostrefArg)
	{
		webSocketPeriodic.theCtxt.context_conn = NULL;
		webSocketPeriodic.theCtxt.context_server = NULL;
		webSocketPeriodic.timeout_msec = 100;
		webSocketPeriodic.wsMessId = JVX_SIZE_UNSELECTED;
		hostRef = hostrefArg;
	}

	jvxErrorType try_activate_property(onePropertyWebSocketDefinition& defOneProperty);
	jvxErrorType deactivate_property(onePropertyWebSocketDefinition& defOneProperty);

	jvxPropertyTransferType check_transfer_property(onePropertyWebSocketDefinition& defOneProperty, jvxPropertyStreamCondUpdate theReason, const jvxComponentIdentification& idCp = jvxComponentIdentification(JVX_COMPONENT_UNKNOWN), jvxPropertyCategoryType cat = JVX_PROPERTY_CATEGORY_UNKNOWN, jvxSize propId = JVX_SIZE_UNSELECTED);
	jvxErrorType transfer_activated_property(onePropertyWebSocketDefinition& defOneProperty, jvxSize& numBytesTransferred);

	jvxErrorType allocateLinearBuffer_propstream(onePropertyWebSocketDefinition& defOneProperty);
	jvxErrorType deallocateLinearBuffer_propstream(onePropertyWebSocketDefinition& defOneProperty);

	jvxErrorType allocateMultichannelCircBuf_propstream(onePropertyWebSocketDefinition& defOneProperty);
	jvxErrorType deallocateMultichannelCircBuf_propstream(onePropertyWebSocketDefinition& defOneProperty);

	void step_update_properties_websocket(jvxPropertyStreamCondUpdate theReason, jvxBool* has_disconnected,
		const jvxComponentIdentification& idCp = jvxComponentIdentification(JVX_COMPONENT_UNKNOWN), jvxPropertyCategoryType cat = JVX_PROPERTY_CATEGORY_UNKNOWN, jvxSize propId = JVX_SIZE_UNSELECTED);

	void add_property_observer_list(jvxHandle* param, jvxSize paramType);
	void rem_property_observer_list(jvxHandle* param, jvxSize paramType);
	void process_flow_event(jvxHandle* param, jvxSize paramType);
	void timeout_reconfigure(jvxHandle* param, jvxSize paramType);

	jvxErrorType disconnect_udp_port(const onePropertyWebSocketDefinition& prop_elm);
	virtual jvxErrorType report_incoming_packet(jvxByte* fld, jvxSize sz, HjvxPropertyStreamUdpSocket* resp_socket)override;
	virtual jvxErrorType report_connection_error(const char* errorText)override;


};

#endif
