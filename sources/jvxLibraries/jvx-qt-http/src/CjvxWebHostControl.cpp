#include "CjvxWebHostControl.h"	

#define QT_PROP_TAG_GROUP_UID "REF_G_UID"
#define QT_PROP_TAG_INGROUP_UID "REF_IG_UID"
#define QT_PROP_TAG_REQUEST_TYPE "REF_REQUEST_TYPE"
#define QT_PROP_TAG_HDL "REF_HDL"
#define QT_PROP_TAG_PRIV "REF_PRIV"
#define QT_PROP_TAG_WS_ID "REF_WS_ID"

Q_DECLARE_METATYPE(IjvxHttpApi_httpreport*);
Q_DECLARE_METATYPE(IjvxHttpApi_wsreport*);

CjvxWebHostControl::CjvxWebHostControl(QObject* parent): QObject(parent) JVX_INIT_RT_ST_INSTANCES_A
{
	target = "localhost";
	port = 8000;
	timeout_sec = 10;
	mgr = new QNetworkAccessManager(this);
	uniqueId_ws = 1;
	//qRegisterMetaType<MyClass>("MyClass");
	stopWorkDueToStop = false;
	JVX_GET_TICKCOUNT_US_SETREF(&myTimeStamp);

	theHost = NULL;
};

CjvxWebHostControl::~CjvxWebHostControl()
{
};

jvxErrorType
CjvxWebHostControl::initialize(CjvxWebHostControl_hooks* hooks, IjvxFactoryHost* theSysHost)
{
	myTimer = new QTimer(this);

	connect(mgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(message_complete(QNetworkReply*)));	
	connect(myTimer, SIGNAL(timeout()), this, SLOT(timerExpired()));

	myTimer->start(1000);

	myHooks = hooks;
	myHooks->webcontrol_update_window();

	stopWorkDueToStop = false;

	theHost = theSysHost;

	return JVX_NO_ERROR;
};

jvxErrorType
CjvxWebHostControl::activate()
{
#ifdef JVX_OBJECTS_WITH_TEXTLOG

	// Reset the logfile module
	jvx_init_text_log(jvxrtst_bkp);
	jvxrtst_bkp.set_module_name("CjvxWebHostControl");
	theHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&jvxrtst_bkp.theToolsHost));
	if (jvxrtst_bkp.theToolsHost)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(jvxrtst_bkp.jvxlst_buf, char, JVX_COBJECT_OS_BUF_SIZE);
		jvxrtst_bkp.jvxlst_buf_sz = JVX_COBJECT_OS_BUF_SIZE;
		jvx_request_text_log(jvxrtst_bkp);
	}

#endif
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebHostControl::blockCheckPending(jvxBool* isPending)
{
	if (isPending)
		*isPending = false;

	stopWorkDueToStop = true; // Prevent new messages

	if (requestsInProcessing.size())
	{
		if (isPending)
			*isPending = true;
	}

	request_kill_all();

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxWebHostControl::unblockCheckPending()
{
	stopWorkDueToStop = false;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebHostControl::deactivate()
{
#ifdef JVX_OBJECTS_WITH_TEXTLOG

	if (jvxrtst_bkp.theToolsHost)
	{
		jvx_return_text_log(jvxrtst_bkp);
		JVX_DSP_SAFE_DELETE_FIELD(jvxrtst_bkp.jvxlst_buf);

		jvxrtst_bkp.jvxlst_buf = NULL;
		jvxrtst_bkp.jvxlst_buf_sz = 0;
		jvxrtst_bkp.dbgModule = false;

		theHost->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(jvxrtst_bkp.jvxlst_buf));	
	}
	jvxrtst_bkp.jvxlst_buf = NULL;
	jvx_terminate_text_log(jvxrtst_bkp);
	return JVX_NO_ERROR;
}

#endif
jvxErrorType 
CjvxWebHostControl::terminate()
{
	myHooks = NULL;
	disconnect(mgr, SIGNAL(finished(QNetworkReply*)));


	theHost = NULL;
	return JVX_NO_ERROR;
};

// ===============================================================

jvxErrorType 
CjvxWebHostControl::set_uri_target(const std::string& new_target)
{
	target = new_target;
	return JVX_NO_ERROR;
};

std::string 
CjvxWebHostControl::get_uri_target()
{
	return(target);
};

jvxErrorType 
CjvxWebHostControl::set_uri_port(int new_port)
{
	port = new_port;
	return JVX_NO_ERROR;
};

int 
CjvxWebHostControl::get_uri_port()
{
	return port;
};

std::string 
CjvxWebHostControl::get_uri_request()
{
	std::string new_prefix = "http://" + target + ":" + jvx_int2String(port);
	return new_prefix;
};

std::string 
CjvxWebHostControl::get_uri_wsocket()
{
	std::string new_prefix = "ws://" + target + ":" + jvx_int2String(port);;
	return new_prefix;
};

std::string 
CjvxWebHostControl::get_request_get()
{
	return get_request;
};

std::string 
CjvxWebHostControl::get_request_post()
{
	return post_request;
};

std::string 
CjvxWebHostControl::get_request_put()
{
	return put_request;
};

std::string 
CjvxWebHostControl::get_content_post()
{
	return post_content;
};

std::string 
CjvxWebHostControl::get_content_put()
{
	return put_content;
};

std::string 
CjvxWebHostControl::get_request_wsocket()
{
	return wsocket_request;
};

jvxErrorType
CjvxWebHostControl::the_socket_state(jvxSize idWs, QAbstractSocket::SocketState* theSockState)
{

	*theSockState = QAbstractSocket::UnconnectedState;
	std::map<jvxSize, oneWebSocket>::iterator elm = theWsockets.find(idWs);

	if (elm != theWsockets.end())
	{
		*theSockState = elm->second.wso->state();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
};

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = 

void 
CjvxWebHostControl::set_request_get(const std::string& in)
{
	get_request = in;
};

void 
CjvxWebHostControl::set_request_post(const std::string& in)
{
	post_request = in;
};

void 
CjvxWebHostControl::set_request_put(const std::string& in)
{
	put_request = in;
};

void 
CjvxWebHostControl::set_content_post(const std::string& in)
{
	post_content = in;
};

void 
CjvxWebHostControl::set_content_put(const std::string& in)
{
	put_content = in;
};

void 
CjvxWebHostControl::set_request_wsocket(const std::string& in)
{
	wsocket_request = in;
};

// ==========================================================================

jvxErrorType 
CjvxWebHostControl::trigger_send_put_request(jvxSize GunId, jvxSize IGunId, IjvxHttpApi_httpreport* rep, jvxHandle* priv)
{
	std::string uri = this->get_uri_request();
	QString uriQStr;
	QUrl myQUri;
	jvxCBitField reqId = 0;

	if (!stopWorkDueToStop)
	{
		uri = jvx_makePathExpr(uri, put_request, false, false);
		myQUri.setUrl(QString(uri.c_str()));

		uriQStr = myQUri.toEncoded();
		myQUri.setUrl(uriQStr);

		uri = uriQStr.toLatin1().data();
		myHooks->webcontrol_print_text("Sending HTTP PUT request " + uri + "--" + put_content);

		QNetworkRequest req_put(QUrl(QString(uri.c_str())));

		req_put.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
		QByteArray theData = put_content.c_str();

		//"http://137.226.198.65:8000/jvx/host/components/return?component=audio technology")));
		QNetworkReply *reply = mgr->put(req_put, theData);

		QVariant var = QVariant::fromValue(rep);
		reply->setProperty(QT_PROP_TAG_HDL, var);
		var = QVariant::fromValue(priv);
		reply->setProperty(QT_PROP_TAG_PRIV, var);
		var = QVariant::fromValue(GunId);
		reply->setProperty(QT_PROP_TAG_GROUP_UID, var);
		var = QVariant::fromValue(IGunId);
		reply->setProperty(QT_PROP_TAG_INGROUP_UID, var);
		jvx_bitZSet(reqId, JVX_WEB_SERVER_URI_PUT_HANDLER_SHIFT);
		var = QVariant::fromValue(reqId);
		reply->setProperty(QT_PROP_TAG_REQUEST_TYPE, var);

#ifdef JVX_OBJECTS_WITH_TEXTLOG
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvxrtst << "Sending HTTP POST request <" << uri << +"--" << put_content << ">,G-ID = " << GunId << ", IG-ID = " << IGunId << std::endl;
		}
#endif
		oneRequestInQueue newElm;
		newElm.reply_otg = reply;
		newElm.tstamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTimeStamp);
		newElm.timeoutApplication = false;

		requestsInProcessing[reply] = newElm;

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_END_OF_FILE;
};

jvxErrorType 
CjvxWebHostControl::trigger_send_get_request(jvxSize GunId, jvxSize IGunId, IjvxHttpApi_httpreport* rep, jvxHandle* priv)
{
	std::string uri = this->get_uri_request();
	QString uriQStr;
	QUrl myQUri;
	jvxCBitField reqId = 0;

	if (!stopWorkDueToStop)
	{
		uri = jvx_makePathExpr(uri, get_request, false, false);
		myQUri.setUrl(QString(uri.c_str()));

		/* Note here: the parameters should all be percent encoded before being url encoded to 
		 * cope with & signs in the text strings
		 */
		uriQStr = myQUri.toEncoded();
		myQUri.setUrl(uriQStr);

		uri = uriQStr.toLatin1().data();
		myHooks->webcontrol_print_text("Sending HTTP GET request " + uri);

		QNetworkRequest req_get(myQUri);
		//"http://137.226.198.65:8000/jvx/host/components/return?component=audio technology")));

		QNetworkReply *reply = mgr->get(req_get);

		QVariant var = QVariant::fromValue(rep);
		reply->setProperty(QT_PROP_TAG_HDL, var);
		var = QVariant::fromValue(priv);
		reply->setProperty(QT_PROP_TAG_PRIV, var);
		var = QVariant::fromValue(GunId);
		reply->setProperty(QT_PROP_TAG_GROUP_UID, var);
		var = QVariant::fromValue(IGunId);
		reply->setProperty(QT_PROP_TAG_INGROUP_UID, var);
		jvx_bitZSet(reqId, JVX_WEB_SERVER_URI_GET_HANDLER_SHIFT);
		var = QVariant::fromValue(reqId);
		reply->setProperty(QT_PROP_TAG_REQUEST_TYPE, var);

#ifdef JVX_OBJECTS_WITH_TEXTLOG
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvxrtst << "Sending HTTP GET request <" << uri << ">,G-ID = " << GunId << ", IG-ID = " << IGunId << std::endl;
		}
#endif
		oneRequestInQueue newElm;
		newElm.reply_otg = reply;
		newElm.tstamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTimeStamp);
		newElm.timeoutApplication = false;

		requestsInProcessing[reply] = newElm;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_END_OF_FILE;
};

jvxErrorType
CjvxWebHostControl::trigger_send_post_request(jvxSize GunId, jvxSize IGunId, IjvxHttpApi_httpreport* rep, jvxHandle* priv)
{
	std::string uri = this->get_uri_request();
	QString uriQStr;
	QUrl myQUri;
	jvxCBitField reqId = 0;

	if (!stopWorkDueToStop)
	{
		uri = jvx_makePathExpr(uri, post_request, false, false);
		myQUri.setUrl(QString(uri.c_str()));

		uriQStr = myQUri.toEncoded();
		myQUri.setUrl(uriQStr);

		uri = uriQStr.toLatin1().data();
		myHooks->webcontrol_print_text("Sending HTTP POST request " + uri + "--" + put_content);

		QNetworkRequest req_put(QUrl(QString(uri.c_str())));

		req_put.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
		QByteArray theData = post_content.c_str();

		//"http://137.226.198.65:8000/jvx/host/components/return?component=audio technology")));
		QNetworkReply *reply = mgr->post(req_put, theData);

		QVariant var = QVariant::fromValue(rep);
		reply->setProperty(QT_PROP_TAG_HDL, var);
		var = QVariant::fromValue(priv);
		reply->setProperty(QT_PROP_TAG_PRIV, var);
		var = QVariant::fromValue(GunId);
		reply->setProperty(QT_PROP_TAG_GROUP_UID, var);
		var = QVariant::fromValue(IGunId);
		reply->setProperty(QT_PROP_TAG_INGROUP_UID, var);
		jvx_bitZSet(reqId, JVX_WEB_SERVER_URI_POST_HANDLER_SHIFT);
		var = QVariant::fromValue(reqId);
		reply->setProperty(QT_PROP_TAG_REQUEST_TYPE, var);

#ifdef JVX_OBJECTS_WITH_TEXTLOG
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvxrtst << "Sending HTTP POST request <" << uri << +"--" << put_content << ">,G-ID = " << GunId << ", IG-ID = " << IGunId << std::endl;
		}
#endif
		oneRequestInQueue newElm;
		newElm.reply_otg = reply;
		newElm.tstamp = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTimeStamp);
		newElm.timeoutApplication = false;

		requestsInProcessing[reply] = newElm;

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_END_OF_FILE;
};

jvxErrorType 
CjvxWebHostControl::open_websocket(jvxSize* uId, IjvxHttpApi_wsreport* rep, jvxHandle* priv)
{
	std::string uriL = get_uri_wsocket();
	if(uId)
		*uId = JVX_SIZE_UNSELECTED;
	std::map<jvxSize, oneWebSocket>::iterator elm = theWsockets.begin();
	for (; elm != theWsockets.end(); elm++)
	{
		if (elm->second.uri == uriL)
		{
			break;
		}
	}

	if (elm == theWsockets.end())
	{
		oneWebSocket newElm;
		newElm.stat = JVX_STATE_NONE;
		newElm.uId = uniqueId_ws++;
		newElm.uri = jvx_makePathExpr(uriL, this->wsocket_request, false, false);

		newElm.wso = new QWebSocket(("jvx-qt-websocket_" + jvx_size2String(newElm.uId)).c_str(), QWebSocketProtocol::VersionLatest, this);
		connect(newElm.wso, SIGNAL(connected()), this, SLOT(onWsConnect()));
		connect(newElm.wso, SIGNAL(disconnected()), this, SLOT(onWsDisconnect()));
		connect(newElm.wso, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onWsError(QAbstractSocket::SocketError)));
		connect(newElm.wso, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onWsStateChange(QAbstractSocket::SocketState)));
		//connect(wso, SIGNAL(binaryFrameReceived(const QByteArray &, bool)), this, SLOT(binaryFrameReceived(const QByteArray &, bool)));
		connect(newElm.wso, SIGNAL(binaryMessageReceived(const QByteArray&)), this, SLOT(binaryMessageReceived(const QByteArray&)));

		QVariant var = QVariant::fromValue(rep);
		newElm.wso->setProperty(QT_PROP_TAG_HDL, var);
		var = QVariant::fromValue(priv);
		newElm.wso->setProperty(QT_PROP_TAG_PRIV, var);
		var = QVariant::fromValue(newElm.uId);
		newElm.wso->setProperty(QT_PROP_TAG_WS_ID, var);

		theWsockets[newElm.uId] = newElm;
		if (uId)
			*uId = newElm.uId;

		myHooks->webcontrol_print_text("Trying to connect web socket to " + newElm.uri);

		newElm.wso->open(QUrl(newElm.uri.c_str()));


		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}

jvxErrorType
CjvxWebHostControl::close_websocket(jvxSize idWs)
{
	std::map<jvxSize, oneWebSocket>::iterator elm = theWsockets.find(idWs);

	if (elm != theWsockets.end())
	{
		myHooks->webcontrol_print_text("Shutting down web socket targeting uri " + elm->second.uri);
		
		// The close my appear again - therefore, we need to clear it from list first and close it afterwards.
		QWebSocket* wso_deleteme = elm->second.wso;

		disconnect(elm->second.wso, SIGNAL(connected()));
		disconnect(elm->second.wso, SIGNAL(disconnected()));
		disconnect(elm->second.wso, SIGNAL(error(QAbstractSocket::SocketError)));
		disconnect(elm->second.wso, SIGNAL(stateChanged(QAbstractSocket::SocketState)));
		disconnect(elm->second.wso, SIGNAL(binaryMessageReceived(const QByteArray &)));

		theWsockets.erase(elm);

		wso_deleteme->close();
		wso_deleteme->deleteLater();
		//delete(wso_deleteme); <- this, we should not do. It seems that QT will delete this object lateron

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxWebHostControl::request_transfer_websocket(jvxSize uId, jvxWebServerWebSocketFrameType opcode, const char* hdr)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	std::map<jvxSize, oneWebSocket>::iterator elm = theWsockets.find(uId);
	jvxProtocolHeader* phdr = (jvxProtocolHeader*)hdr; 
	QByteArray byArray;
	qint64 numSent = 0;
	if (elm != theWsockets.end())
	{
		switch (opcode)
		{
		case JVX_WEBSOCKET_OPCODE_BINARY:
			
			byArray.setRawData(hdr, phdr->paketsize);
			numSent = elm->second.wso->sendBinaryMessage(byArray);
			if (numSent == phdr->paketsize)
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}
			break;
		default:
			res = JVX_ERROR_INVALID_SETTING;
		}
	}
	return res;
}

void
CjvxWebHostControl::message_complete(QNetworkReply* reply)
{
	QVariant varPtr;
	QVariant varPriv;
	QVariant varGUId;
	QVariant varIGUId;
	QVariant varReqTp;
	IjvxHttpApi_httpreport* rep = NULL;
	jvxHandle* priv = NULL;
	jvxSize GunId = JVX_SIZE_UNSELECTED;
	jvxSize IGunId = JVX_SIZE_UNSELECTED;
	jvxCBitField reqId = 0;
	jvxBool reportError = false;
	std::string errStr;
	std::map<QNetworkReply*, oneRequestInQueue>::iterator elm = requestsInProcessing.find(reply);

	if (reply->error() == QNetworkReply::NoError)
	{
		std::string response;
		std::vector<std::string> errs;
		CjvxJsonElementList jelmlst;
		QString strReply = (QString)reply->readAll();

		// Convert the percent encoded stuff into a non-percent encoded one
		strReply = QUrl::fromPercentEncoding(strReply.toLatin1());

		response = strReply.toLatin1().data();

		varPtr = reply->property(QT_PROP_TAG_HDL);
		varPriv = reply->property(QT_PROP_TAG_PRIV);
		varGUId = reply->property(QT_PROP_TAG_GROUP_UID);
		varIGUId = reply->property(QT_PROP_TAG_INGROUP_UID);
		varReqTp = reply->property(QT_PROP_TAG_REQUEST_TYPE);

		if (varPtr.isValid() && varPriv.isValid() && varGUId.isValid() && varIGUId.isValid())
		{
			rep = varPtr.value<IjvxHttpApi_httpreport*>();
			priv = varPriv.value<jvxHandle*>();
			GunId = varGUId.value<jvxSize>();
			IGunId = varIGUId.value<jvxSize>();
		}

		if (varReqTp.isValid())
		{
			reqId = varReqTp.value<jvxCBitField>();
		}

#ifdef JVX_OBJECTS_WITH_TEXTLOG
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvxrtst << "Receivig incoming message: <" << response << ">, G-ID = " << GunId << ", IG-ID = " << IGunId << std::endl;
		}
#endif
		if (rep)
		{
			rep->complete_transfer_http(GunId, IGunId, response.c_str(), priv, reqId);
		}

		/*
		QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
		QJsonObject jsonObj = jsonResponse.object();
		convertQJsonToJvxJson(jelmlst, jsonObj);
		*/
		jvxErrorType resL = CjvxJsonElementList::stringToRepresentation(response, jelmlst, errs);
		if (resL != JVX_NO_ERROR)
		{
			std::cout << "Error while parsing returned json string, input:" << std::endl;
			std::cout << response << "." << std::endl;
			assert(errs.size() == 1);
			std::cout << errs[0] << std::endl;
			errs.clear();
		}
		response.clear();
		jelmlst.markAllowsOneLineOutput();
		jelmlst.printString(response, JVX_JSON_PRINT_TXTCONSOLE, 0, "\t", "\t", "\n", false);
		myHooks->webcontrol_print_text_return(response);
		/*
		QJsonObject::iterator elm = jsonObj.begin();
		for (; elm != jsonObj.end(); elm++)
		{
			CjvxJsonElement jelm;
			if (elm->isArray())
			{
				CjvxJsonArray jelmarr;
				CjvxJsonArrayElement jelmarrelm;
				QJsonArray qArr = elm->toArray();
				QJsonObject::iterator elm =
			}
			qDebug() << elm.key() << "=" << elm.value();
		}
		*/

		reply->deleteLater();
	}
	else
	{
		reportError = true;
		if (elm->second.timeoutApplication)
		{
			errStr = "Failure: ApplicationTimeout";
			myHooks->webcontrol_print_text(errStr);
		}
		else
		{
			errStr = (std::string)"Failure" + reply->errorString().toLatin1().data();
			myHooks->webcontrol_print_text(errStr);
		}
		varPtr = reply->property(QT_PROP_TAG_HDL);
		varPriv = reply->property(QT_PROP_TAG_PRIV);
		varGUId = reply->property(QT_PROP_TAG_GROUP_UID);
		varIGUId = reply->property(QT_PROP_TAG_INGROUP_UID);
		varReqTp = reply->property(QT_PROP_TAG_REQUEST_TYPE);

		if(varPtr.isValid() && varPriv.isValid() && varGUId.isValid() && varIGUId.isValid())
		{
			rep = varPtr.value<IjvxHttpApi_httpreport*>();
			priv = varPriv.value<jvxHandle*>();
			GunId = varGUId.value<jvxSize>();
			IGunId = varIGUId.value<jvxSize>();
		}
	
		if (varReqTp.isValid())
		{
			reqId = varReqTp.value<jvxCBitField>();
		}

#ifdef JVX_OBJECTS_WITH_TEXTLOG
		jvxrtst << "Reporting transfer error: <" << errStr << ">, G-ID = " << GunId << ", IG-ID = " << IGunId << std::endl;
#endif
		if (rep)
		{
			rep->failed_transfer_http(GunId, IGunId, JVX_ERROR_INTERNAL, priv, reqId);
		}
		reply->deleteLater();
	}

	requestsInProcessing.erase(elm);
	if (reportError)
	{
		myHooks->webcontrol_report_error();
	}
};

void 
CjvxWebHostControl::onWsConnect()
{
	QObject* sendit = QObject::sender();
	QVariant varPtr = sendit->property(QT_PROP_TAG_HDL);
	QVariant varId = sendit->property(QT_PROP_TAG_WS_ID);
	QVariant varPriv = sendit->property(QT_PROP_TAG_PRIV);
	IjvxHttpApi_wsreport* ref = NULL;
	jvxHandle* priv = NULL;
	jvxSize id = JVX_SIZE_UNSELECTED;

	if (varPtr.isValid() && varPriv.isValid() && varId.isValid() )
	{
		ref = varPtr.value<IjvxHttpApi_wsreport*>();
		priv = varPriv.value<jvxHandle*>();
		id = varId.value<jvxSize>();
	}

	if (ref)
	{
		ref->websocket_connected(id, priv);
	}
	else
	{
		myHooks->webcontrol_print_text("WS Connected");
	}
};

void 
CjvxWebHostControl::onWsDisconnect()
{
	QObject* sendit = QObject::sender();
	QVariant varPtr = sendit->property(QT_PROP_TAG_HDL);
	QVariant varId = sendit->property(QT_PROP_TAG_WS_ID);
	QVariant varPriv = sendit->property(QT_PROP_TAG_PRIV);
	IjvxHttpApi_wsreport* ref = NULL;
	jvxHandle* priv = NULL;
		jvxSize id = JVX_SIZE_UNSELECTED;


	if (varPtr.isValid() && varPriv.isValid() && varId.isValid())
	{
		ref = varPtr.value<IjvxHttpApi_wsreport*>();
		priv = varPriv.value<jvxHandle*>();
		id = varId.value<jvxSize>();
	}

	if (ref)
	{
		ref->websocket_disconnected(id, priv);
	}
	else
	{
		myHooks->webcontrol_print_text("WS Disconnected");
	}
};

void 
CjvxWebHostControl::onWsError(QAbstractSocket::SocketError error)
{
	QObject* sendit = QObject::sender();
	QVariant varPtr = sendit->property(QT_PROP_TAG_HDL);
	QVariant varId = sendit->property(QT_PROP_TAG_WS_ID);
	QVariant varPriv = sendit->property(QT_PROP_TAG_PRIV);
	IjvxHttpApi_wsreport* ref = NULL;
	jvxHandle* priv = NULL;
	jvxSize id = JVX_SIZE_UNSELECTED;


	if (varPtr.isValid() && varPriv.isValid() && varId.isValid())
	{
		ref = varPtr.value<IjvxHttpApi_wsreport*>();
		priv = varPriv.value<jvxHandle*>();
		id = varId.value<jvxSize>();
	}

	std::string txt;
	switch (error)
	{
	case QAbstractSocket::ConnectionRefusedError:
		txt = "Error on WS";
		txt += "QAbstractSocket::ConnectionRefusedError";
		break;
	case QAbstractSocket::RemoteHostClosedError:
		txt = "Error on WS";
		txt += "QAbstractSocket::RemoteHostClosedError";
		break;
	case QAbstractSocket::HostNotFoundError:
		txt = "Error on WS";
		txt += "QAbstractSocket::HostNotFoundError";
		break;
	case QAbstractSocket::SocketAccessError:
		txt = "Error on WS";
		txt += "QAbstractSocket::SocketAccessError";
		break;
	case QAbstractSocket::SocketResourceError:
		txt = "Error on WS";
		txt += "QAbstractSocket::SocketResourceError";
		break;
	case QAbstractSocket::SocketTimeoutError:
		txt = "Error on WS";
		txt += "QAbstractSocket::SocketTimeoutError";
		break;
	case QAbstractSocket::DatagramTooLargeError:
		txt = "Error on WS";
		txt += "QAbstractSocket::DatagramTooLargeError";
		break;
	case QAbstractSocket::NetworkError:
		txt = "Error on WS";
		txt += "QAbstractSocket::NetworkError";
		break;
	case QAbstractSocket::AddressInUseError:
		txt = "Error on WS";
		txt += "QAbstractSocket::AddressInUseError";
		break;
	case QAbstractSocket::SocketAddressNotAvailableError:
		txt = "Error on WS";
		txt += "QAbstractSocket::SocketAddressNotAvailableError";
		break;
	case QAbstractSocket::UnsupportedSocketOperationError:
		txt = "Error on WS";
		txt += "QAbstractSocket::UnsupportedSocketOperationError";
		break;
	case QAbstractSocket::UnfinishedSocketOperationError:
		txt = "Error on WS";
		txt += "QAbstractSocket::UnfinishedSocketOperationError";
		break;
	case QAbstractSocket::ProxyAuthenticationRequiredError:
		txt = "Error on WS";
		txt += "QAbstractSocket::ProxyAuthenticationRequiredError";
		break;
	case QAbstractSocket::SslHandshakeFailedError:
		txt = "Error on WS";
		txt += "QAbstractSocket::SslHandshakeFailedError";
		break;
	case QAbstractSocket::ProxyConnectionRefusedError:
		txt = "Error on WS";
		txt += "QAbstractSocket::ProxyConnectionRefusedError";
		break;
	case QAbstractSocket::ProxyConnectionClosedError:
		txt = "Error on WS";
		txt += "QAbstractSocket::ProxyConnectionClosedError";
		break;
	case QAbstractSocket::ProxyConnectionTimeoutError:
		txt = "Error on WS";
		txt += "QAbstractSocket::ProxyConnectionTimeoutError";
		break;
	case QAbstractSocket::ProxyNotFoundError:
		txt = "Error on WS";
		txt += "QAbstractSocket::ProxyNotFoundError";
		break;
	case QAbstractSocket::ProxyProtocolError:
		txt = "Error on WS";
		txt += "QAbstractSocket::ProxyProtocolError";
		break;
	case QAbstractSocket::OperationError:
		txt = "Error on WS";
		txt += "QAbstractSocket::OperationError";
		break;
	case QAbstractSocket::SslInternalError:
		txt = "Error on WS";
		txt += "QAbstractSocket::SslInternalError";
		break;
	case QAbstractSocket::SslInvalidUserDataError:
		txt = "Error on WS";
		txt += "QAbstractSocket::SslInvalidUserDataError";
		break;
	case QAbstractSocket::TemporaryError:
		txt = "Error on WS";
		txt += "QAbstractSocket::TemporaryError";
		break;
	}

	if (ref)
	{
		ref->websocket_error(id, priv, txt.c_str());
	}
	else
	{


		myHooks->webcontrol_print_text(txt);
		myHooks->webcontrol_update_window();
	}
};

void 
CjvxWebHostControl::onWsStateChange(QAbstractSocket::SocketState st)
{
	std::string txt;
	switch (st)
	{
	case QAbstractSocket::UnconnectedState:
		txt = "State change on WS: ";
		txt += "QAbstractSocket::UnconnectedState" ;
		break;
	case QAbstractSocket::HostLookupState:
		txt = "State change on WS: ";
		txt += "QAbstractSocket::HostLookupState";
		break;
	case QAbstractSocket::ConnectingState:
		txt = "State change on WS: ";
		txt += "QAbstractSocket::ConnectingState";
		break;
	case QAbstractSocket::ConnectedState:
		txt = "State change on WS: ";
		txt += "QAbstractSocket::ConnectedState" ;
		break;
	case QAbstractSocket::BoundState:
		txt = "State change on WS: ";
		txt += "QAbstractSocket::BoundState" ;
		break;
	case QAbstractSocket::ListeningState:
		txt = "State change on WS: ";
		txt += "QAbstractSocket::ListeningState";
		break;
	case QAbstractSocket::ClosingState:
		txt = "State change on WS: ";
		txt += "QAbstractSocket::ClosingState";
		break;
	}

	myHooks->webcontrol_print_text(txt);
	myHooks->webcontrol_update_window();
};

void 
CjvxWebHostControl::binaryMessageReceived(const QByteArray & messPayload)
{
	const char* theData = messPayload.data();
	jvxSize numBytes = messPayload.size();

	QObject* sendit = QObject::sender();
	QVariant varPtr = sendit->property(QT_PROP_TAG_HDL);
	QVariant varId = sendit->property(QT_PROP_TAG_WS_ID);
	QVariant varPriv = sendit->property(QT_PROP_TAG_PRIV);
	IjvxHttpApi_wsreport* ref = NULL;
	jvxHandle* priv = NULL;
		jvxSize id = JVX_SIZE_UNSELECTED;


	if (varPtr.isValid() && varPriv.isValid() && varId.isValid())
	{
		ref = varPtr.value<IjvxHttpApi_wsreport*>();
		priv = varPriv.value<jvxHandle*>();
		id = varId.value<jvxSize>();
	}

	if (ref)
	{
		ref->websocket_data_binary(id, (jvxByte*)theData, numBytes, priv);
	}
	else
	{
		// What to with the received stuff
		std::cout << "Received WS binary message" << std::endl;
	}
}

void
CjvxWebHostControl::convertQJsonArrayToJvxJsonArray(CjvxJsonArray& jelmarr, const QJsonArray& qjsonarr)
{
	QJsonArray::const_iterator elm = qjsonarr.begin();
	for (; elm != qjsonarr.end(); elm++)
	{
		CjvxJsonArrayElement jelmarrl;
		if (elm->isString())
		{
			jelmarrl.makeString(elm->toString().toLatin1().data());
		}
		else if (elm->isObject())
		{
			CjvxJsonElementList jelmlst;
			convertQJsonToJvxJson(jelmlst, elm->toObject());
			jelmarrl.makeSection(jelmlst);
		}
		else
		{
			std::cout << "Unexpected Json Array Object" << std::endl;
		}
		jelmarr.addConsumeElement(jelmarrl);
	}
}

void
CjvxWebHostControl::convertQJsonToJvxJson(CjvxJsonElementList& jelmlst, const QJsonObject& qjsonobj)
{
	QJsonObject::const_iterator elm = qjsonobj.begin();
	for (; elm != qjsonobj.end(); elm++)
	{
		CjvxJsonElement jelm;
		if (elm->isArray())
		{
			CjvxJsonArray jelmarr;
			QJsonArray qjsonarr = elm->toArray();
			convertQJsonArrayToJvxJsonArray(jelmarr, qjsonarr);
			jelm.makeArray(elm.key().toLatin1().data(), jelmarr);
		}
		else if (elm->isString())
		{
			jelm.makeAssignmentString(elm.key().toLatin1().data(), elm->toString().toLatin1().data());
		}
		else if (elm->isDouble())
		{
			jelm.makeAssignmentData(elm.key().toLatin1().data(), elm->toDouble(), 5);
		}
		else
		{
			assert(elm->isObject());

			CjvxJsonElementList njelmlst;
			convertQJsonToJvxJson(njelmlst, elm->toObject());
			jelm.makeSection(elm.key().toLatin1().data(), njelmlst);
		}

		jelmlst.addConsumeElement(jelm);
	}
}

jvxErrorType
CjvxWebHostControl::request_connect_websocket(const std::string& uri, jvxSize* id, IjvxHttpApi_wsreport* rep, jvxHandle* priv)
{
	jvxErrorType res = JVX_NO_ERROR;
	set_request_wsocket(uri);
	res = open_websocket(id, rep, priv);
	return res;
}

jvxErrorType
CjvxWebHostControl::request_disconnect_websocket(jvxSize uId)
{
	close_websocket(uId);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebHostControl::request_transfer_get(const std::string& uri, jvxSize GuniqueId, jvxSize IGuniqueId, IjvxHttpApi_httpreport* rep, jvxHandle* priv)
{
	set_request_get(uri);
	trigger_send_get_request(GuniqueId, IGuniqueId, rep, priv);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebHostControl::request_transfer_put(const std::string& uri, const std::string& content, jvxSize GuniqueId, jvxSize IGuniqueId, IjvxHttpApi_httpreport* rep, jvxHandle* priv)
{
	set_request_put(uri);
	set_content_put(content);
	trigger_send_put_request(GuniqueId, IGuniqueId, rep, priv);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebHostControl::request_transfer_post(const std::string& uri, const std::string& content, jvxSize GuniqueId, jvxSize IGuniqueId, IjvxHttpApi_httpreport* rep, jvxHandle* priv)
{
	set_request_post(uri);
	set_content_post(content);
	trigger_send_post_request(GuniqueId, IGuniqueId, rep, priv);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxWebHostControl::request_kill_all()
{
	std::map<QNetworkReply*, oneRequestInQueue>::iterator elm;
	while(1)
	{
		elm = requestsInProcessing.begin();
		if (elm == requestsInProcessing.end())
		{
			break;
		}

		// This will remove the rely from map (via error report)
		elm->second.reply_otg->abort();
	}
	// NOT NEEDED: requestsInProcessing.clear();
	return JVX_NO_ERROR;
}

void 
CjvxWebHostControl::timerExpired()
{
	jvxTick ts = JVX_GET_TICKCOUNT_US_GET_CURRENT(&myTimeStamp);
	jvxBool contTest = true;
	std::map<QNetworkReply*, oneRequestInQueue>::iterator elm;
	while (contTest)
	{
		contTest = false;
		elm = requestsInProcessing.begin();
		for (; elm != requestsInProcessing.end(); elm++)
		{
			if (ts - elm->second.tstamp > (timeout_sec * 1000000))
			{
				elm->second.timeoutApplication = true;
				elm->second.reply_otg->abort();
				contTest = true;
				break;
			}
		}
	}
}

jvxData 
CjvxWebHostControl::get_timeout_sec()
{
	return timeout_sec;
}

void 
CjvxWebHostControl::set_timeout_sec(jvxData timeout)
{
	timeout_sec = timeout;
}
