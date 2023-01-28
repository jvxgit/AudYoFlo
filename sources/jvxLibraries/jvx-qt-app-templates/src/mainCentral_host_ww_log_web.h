#ifndef __MAINCENTRAL_HOST_WW_LOG_WEB_H__
#define __MAINCENTRAL_HOST_WW_LOG_WEB_H__

#include "mainCentral_host_ww_log.h"
#include "common/CjvxWebServerHost.h"
#include <QTimer>

class mainCentral_host_ww_log_web: public mainCentral_host_ww_log,
	public CjvxWebServerHost, public IjvxWebServerHost_hooks
{

private:
	struct
	{
		IjvxWebServer* hdl;
		IjvxObject* obj;
		jvxErrorType res_mthread;
		QTimer* myTimer;
		jvxInt16 closeProcedureState;
	} theWebserver;

	Q_OBJECT
	
public:

	mainCentral_host_ww_log_web(QWidget* parent);
	
	void init_submodule(IjvxHost* theHost)override;

	// ==========================================================

	void inform_bootup_complete(jvxBool* wantsToAdjust) override;
	void inform_about_to_shutdown() override;

	// ==========================================================

	jvxErrorType synchronizeWebServerCoEvents(jvxHandle* context_server,
		jvxHandle* context_conn, jvxWebServerHandlerPurpose purp, 
		jvxSize uniqueId,jvxBool strictConstConnection, 
		const char* uriprefix) override;

	jvxErrorType synchronizeWebServerWsEvents(jvxHandle* context_server, 
		jvxHandle* context_conn, jvxWebServerHandlerPurpose purp, 
		jvxSize uniqueId, jvxBool strictConstConnection, const char* uriprefix, 
		int header, char *payload, size_t szFld) override;

	jvxErrorType hook_safeConfigFile() override;
	jvxErrorType hook_startSequencer()override;
	jvxErrorType hook_stopSequencer()override;

	jvxErrorType hook_operationComplete()override;

	jvxErrorType hook_ws_started()override;
	jvxErrorType hook_ws_stopped()override;


signals:

	void emit_report_co_event_request(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool, const char*);
	void emit_report_ws_event_request(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool, const char*, int, char*, size_t);

public slots:

	void slot_report_co_event_request(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool strictConstConnection, const char* uriprefix);
	void slot_report_ws_event_request(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool strictConstConnection, const char* uriprefix, int header, char *payload, size_t szFld);
	void slot_postPropertyStreams();

};

#endif