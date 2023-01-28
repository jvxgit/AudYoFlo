#include "jvxQtSaWebServerWidget.h"

Q_DECLARE_METATYPE(jvxHandle*)
Q_DECLARE_METATYPE(jvxWebServerHandlerPurpose)
Q_DECLARE_METATYPE(jvxSize)

jvxQtSaWebServerWidget::jvxQtSaWebServerWidget(QWidget* parent): QWidget(parent)
{
	qRegisterMetaType<jvxHandle*>("jvxHandle*");
	qRegisterMetaType<jvxWebServerHandlerPurpose>("jvxWebServerHandlerPurpose");
	qRegisterMetaType<jvxSize>("jvxSize");

	this->theLinkServer = NULL;
}
	
jvxQtSaWebServerWidget::~jvxQtSaWebServerWidget()
{
}
	
jvxErrorType 
jvxQtSaWebServerWidget::initialize(IjvxWebServer* theLink)
{
	if(this->theLinkServer == NULL)
	{
		this->theLinkServer = theLink;
		this->theLinkServer->initialize();

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

void 
jvxQtSaWebServerWidget::setupUi()
{
	// Very strict coupling
	connect(this, 
		SIGNAL(emit_request_co_handler(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool, const char*)), 
		this, 
		SLOT(slot_request_co_handler(jvxHandle* , jvxHandle* ,jvxWebServerHandlerPurpose , jvxSize, jvxBool, const char*)), 
		Qt::BlockingQueuedConnection);

	connect(this,
		SIGNAL(emit_request_ws_handler(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool, const char*, int , char *, size_t)),
		this,
		SLOT(slot_request_ws_handler(jvxHandle*, jvxHandle*, jvxWebServerHandlerPurpose, jvxSize, jvxBool, const char*, int , char *, size_t)),
		Qt::BlockingQueuedConnection);
	
}
	
	
jvxErrorType 
jvxQtSaWebServerWidget::terminate()
{
	if(this->theLinkServer)
	{
		jvxErrorType res = this->theLinkServer->terminate();
		if(res == JVX_NO_ERROR)
		{
			this->theLinkServer = NULL;
		}	
		return(res);
	}
	return(JVX_ERROR_WRONG_STATE);
	
}

jvxErrorType 
jvxQtSaWebServerWidget::report_server_co_event(
	jvxHandle* context_server, jvxHandle* context_conn,
	jvxWebServerHandlerPurpose purp, jvxSize uniqueId, jvxBool strictConstConnection,
	const char* uri_prefix)
{
	// Map request to the QT thread
	emit emit_request_co_handler(context_server, context_conn, purp, uniqueId, strictConstConnection, uri_prefix);

	// Return success in any case
	return(JVX_NO_ERROR);
}

jvxErrorType
jvxQtSaWebServerWidget::report_server_ws_event(
	jvxHandle* context_server, jvxHandle* context_conn,
	jvxWebServerHandlerPurpose purp, jvxSize uniqueId, jvxBool strictConstConnection,
	const char* uri_prefix, int header, char *payload, size_t sz)
{
	// Map request to the QT thread
	emit emit_request_ws_handler(context_server, context_conn, purp, uniqueId, strictConstConnection, uri_prefix,
		header, payload, sz);

	// Return success in any case
	return(JVX_NO_ERROR);
}
/*
void 
jvxQtSaWebServerWidget::slot_request_handler(jvxHandle* context_server, jvxHandle* context_conn,
					jvxWebServerHandlerPurpose purp, jvxSize uniqueId)
{
	// This function MUST be overwritten by derived class
	assert(0);
}
*/

