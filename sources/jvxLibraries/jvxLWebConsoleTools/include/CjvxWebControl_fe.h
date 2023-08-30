#ifndef __CJVXWEBCONTROL_FE_H__
#define __CJVXWEBCONTROL_FE_H__

#include "jvx.h"
#include "interfaces/console/IjvxEventLoop_frontend_ctrl.h"
#include "common/CjvxWebServerHost.h"
#include "typedefs/console/TjvxFrontendBackend.h"
#include "allHostsStatic_common.h"
#include "CjvxWebControl_fe_types.h"
#include "CjvxBinaryWebSockets.h"
#include "CjvxTextWebSockets.h"

#include <map>

// #define JVX_VERBOSE_FLOW_CONTROL


// ===================================================================
// ============================================================================

class CjvxWebControl_fe: public IjvxEventLoop_frontend_ctrl,
	public CjvxWebServerHost, public IjvxWebServerHost_hooks,
	public IjvxEventLoop_backend, public IjvxReport, public IjvxConfigurationExtender_report,
	public IjvxReportSystemForward	
	//public IjvxEventLoop_threadcleanup

{
	friend class CjvxBinaryWebSockets;
	friend class CjvxTextWebSockets;

private:

	CjvxBinaryWebSockets binWs;
	CjvxTextWebSockets txtWs;

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
	virtual jvxErrorType JVX_CALLINGCONVENTION query_property(jvxFrontendSupportQueryType tp, jvxHandle* load)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_sync(jvxFrontendTriggerType tp, jvxHandle* load)override;
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
	
	void web_socket_disconnect(jvxHandle* hdl);
	void process_incoming_binary(char* payload, size_t szFld);
};

#endif
