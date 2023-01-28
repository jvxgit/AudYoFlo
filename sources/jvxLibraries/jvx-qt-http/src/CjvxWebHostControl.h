#ifndef __CJVXWEBHOSTCONTROL_H__
#define __CJVXWEBHOSTCONTROL_H__

#include "jvx.h"
#include "jvx-helpers.h"

#include "QtCore/QUrl"
#include "QtNetwork/QNetworkRequest"
#include <QtNetwork/QNetworkReply>
#include <QtWebSockets/QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

#include "CjvxJson.h"

#define JVX_COBJECT_OS_BUF_SIZE 256


// CHECK THIS:
// http://qt.shoutwiki.com/wiki/How_to_wait_synchronously_for_a_Signal_in_Qt

JVX_INTERFACE CjvxWebHostControl_hooks
{
public:
	virtual ~CjvxWebHostControl_hooks() {};
	virtual void webcontrol_print_text(const std::string& txt) = 0;
	virtual void webcontrol_print_text_return(const std::string& txt) = 0;
	virtual void webcontrol_update_window() = 0;
	virtual void webcontrol_report_error() = 0;
};

class CjvxWebHostControl: public QObject, public IjvxHttpApi
{
protected:
	typedef struct
	{
		std::string uri;
		jvxSize uId;
		jvxState stat;
		QWebSocket* wso;
	} oneWebSocket;

	std::string target;
	int port;
	QNetworkAccessManager* mgr;
	std::string get_request;
	std::string post_request;
	std::string put_request;
	std::string post_content;
	std::string put_content;
	std::string wsocket_request;
	CjvxWebHostControl_hooks* myHooks;
	typedef struct
	{
		QNetworkReply* reply_otg;
		jvxTick tstamp;
		jvxBool timeoutApplication;
	} oneRequestInQueue;

	std::map<QNetworkReply*, oneRequestInQueue> requestsInProcessing;
	jvxTimeStampData myTimeStamp;
	QTimer* myTimer;
	jvxData timeout_sec;
	jvxSize uniqueId_ws;
	std::map<jvxSize, oneWebSocket> theWsockets;
	jvxBool stopWorkDueToStop;
	IjvxFactoryHost* theHost;

#ifdef JVX_OBJECTS_WITH_TEXTLOG
	JVX_DEFINE_RT_ST_INSTANCES
#endif

private:
	void convertQJsonArrayToJvxJsonArray(CjvxJsonArray& jelmarr, const QJsonArray& qjsonarr);
	void convertQJsonToJvxJson(CjvxJsonElementList& jelmlst, const QJsonObject& qjsonobj);

	Q_OBJECT
public:
	CjvxWebHostControl(QObject* parent);
	~CjvxWebHostControl();
	jvxErrorType initialize(CjvxWebHostControl_hooks* hooks, IjvxFactoryHost* theHost);
	jvxErrorType terminate();
	jvxErrorType activate();
	jvxErrorType deactivate();
	jvxErrorType set_uri_target(const std::string& new_target);
	std::string get_uri_target();
	jvxErrorType set_uri_port(int new_port);
	int get_uri_port();
	std::string get_uri_request();
	std::string get_uri_wsocket();
	std::string get_request_get();
	std::string get_request_post();
	std::string get_request_put();
	std::string get_content_post();
	std::string get_content_put();
	std::string get_request_wsocket();
	jvxErrorType the_socket_state(jvxSize idWs, QAbstractSocket::SocketState* theSockState);

	jvxData get_timeout_sec();
	void set_timeout_sec(jvxData timeout);

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = 

	void set_request_get(const std::string& in);
	void set_request_post(const std::string& in);
	void set_request_put(const std::string& in);
	void set_content_post(const std::string& in);
	void set_content_put(const std::string& in);
	void set_request_wsocket(const std::string& in);

	// ==========================================================================

	jvxErrorType trigger_send_post_request(jvxSize GunId = JVX_SIZE_UNSELECTED, 
		jvxSize IGunId = JVX_SIZE_UNSELECTED, 
		IjvxHttpApi_httpreport* rep = NULL,
		jvxHandle* priv = NULL);

	jvxErrorType trigger_send_put_request(jvxSize GunId = JVX_SIZE_UNSELECTED, 
		jvxSize IGunId = JVX_SIZE_UNSELECTED, 
		IjvxHttpApi_httpreport* rep = NULL, 
		jvxHandle* priv = NULL);

	jvxErrorType trigger_send_get_request(jvxSize GunId = JVX_SIZE_UNSELECTED, 
		jvxSize IGunId = JVX_SIZE_UNSELECTED, 
		IjvxHttpApi_httpreport* rep = NULL, 
		jvxHandle* priv = NULL);

	jvxErrorType open_websocket(jvxSize* uId, IjvxHttpApi_wsreport* rep, jvxHandle* priv);
	jvxErrorType close_websocket(jvxSize uId);

	

	// ==========================================================================
	virtual jvxErrorType request_transfer_get(const std::string& uri, jvxSize GuniqueId, jvxSize IGuniqueId, IjvxHttpApi_httpreport* rep, jvxHandle* priv) override;
	virtual jvxErrorType request_transfer_put(const std::string& uri, const std::string& content, jvxSize GuniqueId, jvxSize IGuniqueId, IjvxHttpApi_httpreport* rep, jvxHandle* priv) override;
	virtual jvxErrorType request_transfer_post(const std::string& uri, const std::string& content, jvxSize GuniqueId, jvxSize IGuniqueId, IjvxHttpApi_httpreport* rep, jvxHandle* priv) override;
	
	virtual jvxErrorType request_connect_websocket(const std::string& uri, jvxSize* uId, IjvxHttpApi_wsreport* rep, jvxHandle* priv) override;
	virtual jvxErrorType request_disconnect_websocket(jvxSize uId) override;
	virtual jvxErrorType request_transfer_websocket(jvxSize uId, jvxWebServerWebSocketFrameType opcode, const char* hdr) override;
	virtual jvxErrorType request_kill_all()override;

	virtual jvxErrorType blockCheckPending(jvxBool* isPending) override; // Todo: how get out right here..
	virtual jvxErrorType unblockCheckPending() override; // Todo: how get out right here..

public slots:

	void message_complete(QNetworkReply* reply);
	void onWsConnect();
	void onWsDisconnect();
	void onWsError(QAbstractSocket::SocketError error);
	void onWsStateChange(QAbstractSocket::SocketState st);
	void binaryMessageReceived(const QByteArray& );
	void timerExpired();
};

#endif