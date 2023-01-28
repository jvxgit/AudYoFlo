#ifndef __JVXQTWEBSERVERQWIDGET_H__
#define __JVXQTWEBSERVERQWIDGET_H__

#include "jvx.h"

#include <QtWidgets/QWidget>
#include "jvxQtSaCustomBase.h"

class jvxQtSaWebServerWidget: public QWidget, public IjvxWebServerCo_report, public IjvxWebServerWs_report, public jvxQtSaCustomBase
{
private:
	
	IjvxWebServer* theLinkServer;

	Q_OBJECT
	
public:
	
	jvxQtSaWebServerWidget(QWidget* parent);
	
	~jvxQtSaWebServerWidget();
	
	jvxErrorType initialize(IjvxWebServer* theLink);

	void setupUi();
	
	jvxErrorType terminate();

	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_co_event(
		jvxHandle* context_server, jvxHandle* context_conn,
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId, jvxBool strictConstConnection,
		const char* uri_prefix) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_server_ws_event(
		jvxHandle* context_server, jvxHandle* context_conn,
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId, jvxBool strictConstConnection,
		const char* uri_prefix, int header, char *payload, size_t szFld) override;

signals:
	void emit_request_co_handler(jvxHandle* , jvxHandle* ,
		jvxWebServerHandlerPurpose , jvxSize, jvxBool strictConstConnection,
		const char* uri_prefix);

	void emit_request_ws_handler(jvxHandle*, jvxHandle*,
		jvxWebServerHandlerPurpose, jvxSize, jvxBool strictConstConnection,
		const char* uri_prefix, int header, char *payload, size_t szFld);
public slots:

	virtual void slot_request_co_handler(jvxHandle* context_server, jvxHandle* context_conn,
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId, jvxBool strictConstConnection, 
		const char*	uri_prefix) = 0;
	
	virtual void slot_request_ws_handler(jvxHandle* context_server, jvxHandle* context_conn,
		jvxWebServerHandlerPurpose purp, jvxSize uniqueId, jvxBool strictConstConnection,
		const char*	uri_prefix, int header, char *payload, size_t szFld) = 0;
};

#endif

