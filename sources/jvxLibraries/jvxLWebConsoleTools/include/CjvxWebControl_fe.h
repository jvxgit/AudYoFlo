#ifndef __CJVXWEBCONTROL_FE_H__
#define __CJVXWEBCONTROL_FE_H__

#include "jvx.h"
#include "interfaces/console/IjvxEventLoop_frontend_ctrl.h"
#include "common/CjvxWebServerHost.h"
#include "typedefs/console/TjvxFrontendBackend.h"
#include "jvx-net-helpers.h"
#include "allHostsStatic_common.h"

#include <map>

// #define JVX_VERBOSE_FLOW_CONTROL

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

class CjvxWebControl_fe: public IjvxEventLoop_frontend_ctrl,
	public CjvxWebServerHost, public IjvxWebServerHost_hooks,
	public IjvxEventLoop_backend, public IjvxReport, public IjvxConfigurationExtender_report,
	public HjvxPropertyStreamUdpSocket_report, public IjvxReportSystemForward
	//public IjvxEventLoop_threadcleanup

{
private:

	typedef enum
	{
		JVX_PROPERTY_WEBSOCKET_REGULAR,
		JVX_PROPERTY_WEBSOCKET_CIRCBUFFER
	} onePropertyWebSocketDataType;

	typedef enum
	{
		JVX_PROPERTY_FLOW_READY,
		JVX_PROPERTY_FLOW_WAIT_FOR_RESPONSE
	} jvxOnePropertyWebSocketTransferState;
	
	struct
	{
		std::map<jvxInt32, HjvxPropertyStreamUdpSocket*> theUdpSockets;
	} high_prio_transfer;

	typedef struct
	{
		jvxByte* buf;
		jvxSize selector;
	} oneAllocatedField;

	IjvxEventLoopObject* evLop;
	jvxOnePriBackendAndState linkedPriBackend;
	std::list<jvxOneBackendAndState> linkedSecBackends;

	typedef struct
	{
		jvxErrorType res_mthread;
		std::string ret_mthread;
	} oneThreadReturnType;

	jvxSize wsUniqueId;
	std::map<jvxSize, onePropertyWebSocketDefinition> lstUpdateProperties;

	std::vector<oneAllocatedField> flds;
	JVX_MUTEX_HANDLE safeAccessMemList;
	jvxSize timerCount;
	jvxState theState;
	IjvxHost* myHostRef;
	IjvxToolsHost* myToolsHost;

	struct
	{
		IjvxObject* obj;
		IjvxWebServer* hdl;
		jvxInt16 closeProcedureState;
	} myWebServer;

	typedef struct
	{
		jvxHandle* context_conn;
		jvxHandle* context_server;
	} jvxWebContext;

	struct
	{
		jvxWebContext theCtxt;
		jvxSize timeout_msec;
		jvxSize ping_cnt_trigger;
		jvxSize ping_cnt_close;
		jvxSize ping_cnt_close_failed;
		jvxSize wsMessId;
		std::string uri;
		std::string query;
		std::string local_uri;
		std::string url_origin;
		std::string user;
		jvxSize current_ping_count;

	} webSocketPeriodic;

	jvxTimeStampData tStamp;

	struct
	{
		jvxCBool silent_mode;
		IjvxConfigurationExtender* hostRefConfigExtender;
		jvxSize numThreads;
		jvxSize listeningPort;
		std::string docRoot;
	} config;

	std::vector<oneAddedStaticComponent> addedStaticObjects;

	JVX_DEFINE_RT_ST_INSTANCES

public:

	CjvxWebControl_fe();
	~CjvxWebControl_fe();

	// Interface IjvxEventLoop_frontend_ctrl
	// void set_reference(IjvxEventLoopObject* eventLoop, IjvxEventLoop_backend* theBackend);
	virtual jvxErrorType JVX_CALLINGCONVENTION add_reference_event_loop(IjvxEventLoopObject* eventLoop) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_reference_event_loop(IjvxEventLoopObject* eventLoop)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_pri_reference_event_backend(IjvxEventLoop_backend_ctrl* theBackend)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_sec_reference_event_backend(IjvxEventLoop_backend* theBackend)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_sec_reference_event_backend(IjvxEventLoop_backend* theBackend)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION returns_from_start(jvxBool* doesReturn)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start(int argc, char* argv[])override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION wants_restart(jvxBool *)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_register_fe_commandline(IjvxCommandLine* comLine)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_readout_fe_commandline(IjvxCommandLine* comLine)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_register_be_commandline(IjvxCommandLine* comLine)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_readout_be_commandline(IjvxCommandLine* comLine)override;

	// Interface IjvxEventLoop_frontend
	virtual jvxErrorType JVX_CALLINGCONVENTION report_process_event(TjvxEventLoopElement* theQueueElement)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_cancel_event(TjvxEventLoopElement* theQueueElement)override;

	//void start_timer(jvxSize time_diff_msec);
	//void wait_for_input();

	virtual jvxErrorType JVX_CALLINGCONVENTION report_assign_output(TjvxEventLoopElement* theQueueElement, jvxErrorType sucOperation, jvxHandle* priv)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_special_event(TjvxEventLoopElement* theQueueElement, jvxHandle* priv) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_want_to_shutdown_ext(jvxBool restart) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_property(jvxFrontendSupportRequestType tp, jvxHandle* load)override;
	// =========================================================================

	jvxErrorType request_if_command_forward(IjvxReportSystemForward** fwdCalls) override;
	void request_command_in_main_thread(CjvxReportCommandRequest* request, jvxBool removeAfterHandle = true) override;
	// =========================================================================

	virtual jvxErrorType synchronizeWebServerCoEvents(jvxHandle* context_server, jvxHandle* context_conn, jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
		jvxBool strictConstConnection, const char* uriprefix)override;

	virtual jvxErrorType synchronizeWebServerWsEvents(jvxHandle* context_server, jvxHandle* context_conn, jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
		jvxBool strictConstConnection, const char* uriprefix, int header, char *payload, size_t szFld)override;

	virtual jvxErrorType hook_safeConfigFile()override;
	virtual jvxErrorType hook_startSequencer()override;
	virtual jvxErrorType hook_stopSequencer()override;

	virtual jvxErrorType hook_operationComplete()override;

	// If support for web sockets...
	/*	virtual jvxErrorType hook_wsConnect(jvxHandle* context_server, jvxHandle* context_conn){return(JVX_ERROR_UNSUPPORTED);};
	virtual jvxErrorType hook_wsReady(jvxHandle* context_server, jvxHandle* context_conn){return(JVX_ERROR_UNSUPPORTED);};
	virtual jvxErrorType hook_wsData(jvxHandle* context_server, jvxHandle* context_conn, int header, char *payload, size_t szFld){return(JVX_ERROR_UNSUPPORTED);};
	virtual jvxErrorType hook_wsClose(jvxHandle* context_server, jvxHandle* context_conn){return(JVX_ERROR_UNSUPPORTED);};
	*/
	virtual jvxErrorType hook_ws_started()override;
	virtual jvxErrorType hook_ws_stopped()override;

	// ===================================================================

	jvxErrorType cleanup(
		jvxSize message_id,
		jvxHandle* param, jvxSize paramType,
		jvxSize event_type, 
		jvxOneEventClass event_class, 
		jvxOneEventPriority event_priority,
		jvxCBool autoDeleteOnProcess);

	virtual jvxErrorType JVX_CALLINGCONVENTION process_event(
		TjvxEventLoopElement* elm
		/*jvxSize message_id,
		IjvxEventLoop_frontend* origin, jvxHandle* priv_fe,
		jvxHandle* param, jvxSize paramType,
		jvxSize event_type, jvxOneEventClass event_class,
		jvxOneEventPriority event_priority,
		jvxCBool autoDeleteOnProcess,
		jvxHandle* privBlock, jvxBool* reschedule*/
	) override;

	// ===================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION report_simple_text_message(const char* txt, jvxReportPriority prio) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, 
		jvxSize offsetStart, jvxSize numElements, const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_command_request(
		jvxCBitField request, 
		jvxHandle* caseSpecificData = NULL,
		jvxSize szSpecificData = 0) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION interface_sub_report(IjvxSubReport** subReport) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_os_specific(jvxSize commandId, void* context)override;

	// ===================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;
	 
	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename, jvxInt32 lineno)override;

	// ===================================================================
	// ===================================================================

	virtual void right_before_start() override;

private:

	jvxErrorType try_activate_property(onePropertyWebSocketDefinition& defOneProperty);
	jvxErrorType deactivate_property(onePropertyWebSocketDefinition& defOneProperty);
	jvxPropertyTransferType check_transfer_property(onePropertyWebSocketDefinition& defOneProperty, jvxPropertyStreamCondUpdate theReason, const jvxComponentIdentification& idCp = jvxComponentIdentification(JVX_COMPONENT_UNKNOWN), jvxPropertyCategoryType cat = JVX_PROPERTY_CATEGORY_UNKNOWN, jvxSize propId = JVX_SIZE_UNSELECTED);
	jvxErrorType transfer_activated_property(onePropertyWebSocketDefinition& defOneProperty, jvxSize & numBytesTransferred);
	
	jvxErrorType allocateLinearBuffer_propstream(onePropertyWebSocketDefinition& defOneProperty);
	jvxErrorType deallocateLinearBuffer_propstream(onePropertyWebSocketDefinition& defOneProperty);

	jvxErrorType allocateMultichannelCircBuf_propstream(onePropertyWebSocketDefinition& defOneProperty);
	jvxErrorType deallocateMultichannelCircBuf_propstream(onePropertyWebSocketDefinition& defOneProperty);

	void step_update_properties_websocket(jvxPropertyStreamCondUpdate theReason, jvxBool* has_disconnected,
		const jvxComponentIdentification& idCp = jvxComponentIdentification(JVX_COMPONENT_UNKNOWN), jvxPropertyCategoryType cat = JVX_PROPERTY_CATEGORY_UNKNOWN, jvxSize propId = JVX_SIZE_UNSELECTED);

	jvxErrorType disconnect_udp_port(const onePropertyWebSocketDefinition& prop_elm);
	virtual jvxErrorType report_incoming_packet(jvxByte* fld, jvxSize sz, HjvxPropertyStreamUdpSocket* resp_socket)override;
	virtual jvxErrorType report_connection_error(const char* errorText)override;
	
	void web_socket_disconnect();
};

#endif
