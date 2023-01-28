#ifndef _MAINCENTRAL_HOST_WW_LOG_WEB_H__
#define _MAINCENTRAL_HOST_WW_LOG_WEB_H__

#include "mainCentral_host_ww_log_web.h"

mainCentral_host_ww_log_web::mainCentral_host_ww_log_web(QWidget* parent) : mainCentral_host_ww_log(parent)
{
	theWebserver.hdl = NULL;
	theWebserver.obj = NULL;
	theWebserver.myTimer = NULL;
	theWebserver.closeProcedureState = 0;
}

void
mainCentral_host_ww_log_web::init_submodule(IjvxHost* theHost)
{
	jvxSize i, j;

	mainCentral_host_ww_log::init_submodule(theHost);

	// void emit_report_co_event_request(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool, const char*);
	connect(this, SIGNAL(emit_report_co_event_request(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool, const char*)),
		this, SLOT(slot_report_co_event_request(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool, const char*)),
		Qt::BlockingQueuedConnection);
	// void emit_report_ws_event_request(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool, const char*, int, char*, size_t);
	connect(this, SIGNAL(emit_report_ws_event_request(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool, const char*, int, char*, size_t)),
		this, SLOT(slot_report_ws_event_request(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool, const char*, int, char*, size_t)),
		Qt::BlockingQueuedConnection);
}

void
mainCentral_host_ww_log_web::inform_bootup_complete(jvxBool* wantsToAdjust)
{
	jvxErrorType resL = JVX_NO_ERROR;
	mainCentral_host_ww_log::inform_bootup_complete(wantsToAdjust);

	if (theToolsHost)
	{
		resL = theToolsHost->reference_tool(JVX_COMPONENT_WEBSERVER, &theWebserver.obj, 0, NULL);
		if ((resL == JVX_NO_ERROR) && theWebserver.obj)
		{
			theWebserver.obj->request_specialization(reinterpret_cast<jvxHandle**>(&theWebserver.hdl), NULL, NULL);
		}
	}

	// Start web server
	resL = start_webserver(theWebserver.hdl, theHostRef, static_cast<IjvxWebServerHost_hooks*>(this));
	assert(resL == JVX_NO_ERROR);
}

void
mainCentral_host_ww_log_web::inform_about_to_shutdown()
{
	mainCentral_host_ww_log::inform_about_to_shutdown();

	jvxErrorType resL = JVX_NO_ERROR;
	// Stop web server
	resL = stop_webserver();
	assert(resL == JVX_NO_ERROR);

	if (theToolsHost)
	{
		if (theWebserver.obj)
		{
			resL = theToolsHost->return_reference_tool(JVX_COMPONENT_WEBSERVER, theWebserver.obj);
			theWebserver.obj = NULL;
			theWebserver.hdl = NULL;
		}
	}
}

jvxErrorType
mainCentral_host_ww_log_web::synchronizeWebServerCoEvents(jvxHandle* context_server,
	jvxHandle* context_conn, jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
	jvxBool strictConstConnection, const char* uriprefix)
{
	theWebserver.res_mthread = JVX_ERROR_UNSUPPORTED;
	//if(!theWebserver.closeProcedureStarted)
	//{
	emit emit_report_co_event_request(context_server, context_conn, purp, uniqueId, strictConstConnection, uriprefix);
	//}
	return(theWebserver.res_mthread);
}

jvxErrorType
mainCentral_host_ww_log_web::synchronizeWebServerWsEvents(jvxHandle* context_server,
	jvxHandle* context_conn, jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
	jvxBool strictConstConnection, const char* uriprefix,
	int header, char* payload, size_t szFld)
{
	theWebserver.res_mthread = JVX_ERROR_UNSUPPORTED;
	//if(!theWebserver.closeProcedureStarted)
	//{
	emit emit_report_ws_event_request(context_server, context_conn, purp, uniqueId,
		strictConstConnection, uriprefix, header, payload, szFld);
	//}
	return(theWebserver.res_mthread);
}

jvxErrorType
mainCentral_host_ww_log_web::hook_safeConfigFile()
{
	if (mwCtrlRef)
	{
		mwCtrlRef->trigger_operation(JVX_MAINWINDOWCONTROLLER_TRIGGER_SAFE_CONFIG, NULL);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mainCentral_host_ww_log_web::hook_startSequencer()
{
	if (mwCtrlRef)
	{
		mwCtrlRef->trigger_operation(JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_START, NULL);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mainCentral_host_ww_log_web::hook_stopSequencer()
{
	if (mwCtrlRef)
	{
		mwCtrlRef->trigger_operation(JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_STOP, NULL);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mainCentral_host_ww_log_web::hook_operationComplete()
{
	if (mwCtrlRef)
	{
		mwCtrlRef->trigger_operation(JVX_MAINWINDOWCONTROLLER_TRIGGER_UPDATE_WINDOW, NULL);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mainCentral_host_ww_log_web::hook_ws_started()
{
	theWebserver.myTimer = new QTimer(this);
	connect(theWebserver.myTimer, SIGNAL(timeout()), this, SLOT(slot_postPropertyStreams()));
	theWebserver.myTimer->start(200);
	return(JVX_NO_ERROR);
}

jvxErrorType
mainCentral_host_ww_log_web::hook_ws_stopped()
{
	theWebserver.myTimer->stop();
	disconnect(theWebserver.myTimer, SIGNAL(timeout()), this, SLOT(slot_postPropertyStreams()));
	delete(theWebserver.myTimer);
	theWebserver.myTimer = NULL;
	return(JVX_NO_ERROR);
}

void
mainCentral_host_ww_log_web::slot_postPropertyStreams()
{
	postPropertyStreams();
}

void
mainCentral_host_ww_log_web::slot_report_co_event_request(jvxHandle* context_server, jvxHandle* context_conn, jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
	jvxBool strictConstConnection, const char* uriprefix)
{
	theWebserver.res_mthread = JVX_ERROR_UNSUPPORTED;
	if (theWebserver.hdl)
	{
		theWebserver.res_mthread = report_event_request_core(
			context_server, context_conn, purp, uniqueId, strictConstConnection,
			uriprefix, 0, NULL, 0);
	}
}

void
mainCentral_host_ww_log_web::slot_report_ws_event_request(jvxHandle* context_server, jvxHandle* context_conn, jvxWebServerHandlerPurpose purp, jvxSize uniqueId,
	jvxBool strictConstConnection, const char* uriprefix, int header, char* payload, size_t szFld)
{
	theWebserver.res_mthread = JVX_ERROR_UNSUPPORTED;
	if (theWebserver.hdl)
	{
		theWebserver.res_mthread = report_event_request_core(
			context_server, context_conn, purp, uniqueId, strictConstConnection,
			uriprefix, header, payload, szFld);
	}
}
#endif