#ifndef __CJVXWEBCONTROLNODE_H__
#define __CJVXWEBCONTROLNODE_H__

#include "jvx.h"
#include "jvxAppCtrNodes/CjvxAppCtrNode.h"

//#include "common/CjvxPropertiesP.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"
#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"

#include "common/CjvxWebServerHost.h"
#include "pcg_exports_device.h"

#include "CjvxWebControlNodeWsHandle.h"


class CjvxWebControlNode: public CjvxApplicationControllerNode,
	public IjvxProperties, public CjvxProperties,
	public IjvxWebServerHost_hooks,
	public IjvxSchedule,
	public CjvxWebServerHost,
	public genAppCtrWeb_device
	// public IjvxConfiguration
{
	struct oneThreadWorker
	{
		JVX_NOTIFY_HANDLE waitHdl;
		jvxErrorType resL = JVX_NO_ERROR;
		std::string command;
		std::string response;
		jvxState stat = JVX_STATE_INIT;
		jvxSize endCnt = 0;
	};

	IjvxCommandInterpreter* cmdInterpreter = nullptr;
	IjvxHost* hostRef = nullptr;
	refComp<IjvxWebServer> webServ;
	jvxState statWSInit = JVX_STATE_NONE;	 
	jvxBool silentMode = false;

	jvxBool acceptNewEvents = false;
	JVX_RW_MUTEX_HANDLE safeAccessWebConnection;
	std::map<JVX_THREAD_ID, oneThreadWorker*> workerHdls;
	std::map<jvxHandle*, CjvxWebControlNodeWsHandle*> wsConnections;

public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class JVX_INTERFACE member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxWebControlNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	
	JVX_CALLINGCONVENTION ~CjvxWebControlNode();

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect() override;

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"
#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

	// ====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION schedule_main_loop(jvxSize rescheduleId, jvxHandle* user_data) override;

	// ====================================================================================

	void right_before_start();

	virtual jvxErrorType synchronizeWebServerCoEvents(jvxHandle* context_server, jvxHandle* context_conn,
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
		jvxBool strictConstConnection, const char* uriprefix) override;

	virtual jvxErrorType synchronizeWebServerWsEvents(jvxHandle* context_server, jvxHandle* context_conn,
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
		jvxBool strictConstConnection, const char* uriprefix, int header, char* payload, size_t szFld) override;

	virtual jvxErrorType hook_safeConfigFile() override;
	virtual jvxErrorType hook_startSequencer() override;
	virtual jvxErrorType hook_stopSequencer() override;

	virtual jvxErrorType hook_operationComplete() override;

	// If support for web sockets...
/*	virtual jvxErrorType hook_wsConnect(jvxHandle* context_server, jvxHandle* context_conn){return(JVX_ERROR_UNSUPPORTED);};
	virtual jvxErrorType hook_wsReady(jvxHandle* context_server, jvxHandle* context_conn){return(JVX_ERROR_UNSUPPORTED);};
	virtual jvxErrorType hook_wsData(jvxHandle* context_server, jvxHandle* context_conn, int header, char *payload, size_t szFld){return(JVX_ERROR_UNSUPPORTED);};
	virtual jvxErrorType hook_wsClose(jvxHandle* context_server, jvxHandle* context_conn){return(JVX_ERROR_UNSUPPORTED);};
	*/
	virtual jvxErrorType hook_ws_started() override;
	virtual jvxErrorType hook_ws_stopped() override;

#define JVX_INTERFACE_SUPPORT_BASE_CLASS CjvxApplicationControllerNode
#define JVX_INTERFACE_SUPPORT_PROPERTIES
// #define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_SCHEDULE
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
// #undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_SCHEDULE

};

#endif
