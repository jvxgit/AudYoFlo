#include "CjvxQtWebControlWidget.h"

#define JVX_WEBCONTROL_URI_TARGET "JVX_WEBCONTROL_URI_TARGET"
#define JVX_WEBCONTROL_URI_PORT "JVX_WEBCONTROL_URI_PORT"
#define JVX_WEBCONTROL_URI_TIMEOUT "JVX_WEBCONTROL_URI_TIMEOUT"

// ======================================================================
JVX_QT_WIDGET_INIT_DEFINE(IjvxQtWebControlWidget, jvxQtWebControl, CjvxQtWebControlWidget)
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtWebControlWidget, jvxQtWebControl)
// ======================================================================

CjvxQtWebControlWidget::CjvxQtWebControlWidget(QWidget* parent): QWidget(parent)
{
	myControl = NULL;
	bwdRef = NULL;
	bwdMainUi = NULL;
	theHost = NULL;
	myWsId = JVX_SIZE_UNSELECTED;
}

CjvxQtWebControlWidget::~CjvxQtWebControlWidget()
{
}

void 
CjvxQtWebControlWidget::hideEvent(QHideEvent *)
{
	if (bwdMainUi)
		bwdMainUi->report_widget_closed(static_cast<QWidget*>(this));
}

void
CjvxQtWebControlWidget::init(IjvxFactoryHost* theHost, jvxCBitField mode, jvxHandle* specPtr, IjvxQtSpecificWidget_report* bwd)
{
	setupUi(this);
	bwdMainUi = bwd;
	theHost = theHost;

	myControl = new CjvxWebHostControl(this);
	myControl->initialize(static_cast<CjvxWebHostControl_hooks*>(this), theHost);

	if (theHost)
	{
		theHost->add_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxHttpApi*>(myControl)), JVX_INTERFACE_HTTP_API);
	}

	update_window();
}

void 
CjvxQtWebControlWidget::getMyQWidget(QWidget** retWidget, jvxSize id)
{
	switch (id)
	{
	case 0:
		if (retWidget)
		{
			*retWidget = static_cast<QWidget*>(this);
		}
		break;
	default:
		if (retWidget)
		{
			*retWidget = NULL;
		}
	}
}

void 
CjvxQtWebControlWidget::activate()
{
	if(myControl)
		myControl->activate();
}

void
CjvxQtWebControlWidget::deactivate()
{
	if (myControl)
		myControl->deactivate();
}


void
CjvxQtWebControlWidget::processing_started()
{

}

void
CjvxQtWebControlWidget::processing_stopped()
{
}

void
CjvxQtWebControlWidget::update_window_periodic()
{
}

void
CjvxQtWebControlWidget::terminate()
{
	if (theHost)
	{
		theHost->remove_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxHttpApi*>(myControl)), JVX_INTERFACE_HTTP_API);
	}
	myControl->terminate();
	delete(myControl);
	myControl = NULL;
	bwdRef = NULL;
	bwdMainUi = NULL;
	myWsId = JVX_SIZE_UNSELECTED;
}


void
CjvxQtWebControlWidget::trigger_get()
{
	edit_get_request();
	myControl->trigger_send_get_request();
}

void
CjvxQtWebControlWidget::trigger_put()
{
	edit_put_request();
	edit_put_content();
	myControl->trigger_send_put_request();
}

void
CjvxQtWebControlWidget::trigger_post()
{
	edit_post_request();
	edit_post_content();
	myControl->trigger_send_post_request();
}

void
CjvxQtWebControlWidget::set_address()
{
	QString strQA = lineEdit_target->text();
	if (myControl)
	{
		myControl->set_uri_target(strQA.toLatin1().data());
	}
	else
	{
		std::cout << "Failed to set http address since control handle is NULL." << std::endl;
	}
	update_window();
}

void
CjvxQtWebControlWidget::set_port()
{
	QString strQP = lineEdit_port->text();
	if (myControl)
	{
		myControl->set_uri_port(strQP.toInt());
	}
	else
	{
		std::cout << "Failed to set http port since control handle is NULL." << std::endl;
	}
	update_window();
}

void
CjvxQtWebControlWidget::trigger_openws()
{
	edit_wsocket_request();
	QAbstractSocket::SocketState stat = QAbstractSocket::UnconnectedState;
	jvxErrorType res = myControl->the_socket_state(myWsId, &stat);
	if(stat == QAbstractSocket::UnconnectedState)
	{
		myControl->open_websocket(&myWsId, static_cast<IjvxHttpApi_wsreport*>(this), NULL);
	}
	else
	{
		myControl->close_websocket(myWsId);
	}
}

void
CjvxQtWebControlWidget::webcontrol_report_error()
{
	if (bwdRef)
	{
		bwdRef->report_error();
	}
}

void
CjvxQtWebControlWidget::update_window(jvxCBitField prio)
{
	webcontrol_update_window();
}

void
CjvxQtWebControlWidget::webcontrol_update_window()
{
	std::string str;
	int port;

	str = myControl->get_uri_request();
	label_httpprefix->setText(str.c_str());

	str = myControl->get_uri_wsocket();
	label_websocketprefix->setText(str.c_str());

	str = myControl->get_uri_target();
	lineEdit_target->setText(str.c_str());

	port = myControl->get_uri_port();
	lineEdit_port->setText(jvx_int2String(port).c_str());

	str = myControl->get_request_get();
	lineEdit_get->setText(str.c_str());

	str = myControl->get_request_post();
	lineEdit_post->setText(str.c_str());

	str = myControl->get_request_put();
	lineEdit_put->setText(str.c_str());

	str = myControl->get_request_wsocket();
	lineEdit_websocket->setText(str.c_str());

	str = myControl->get_content_put();
	lineEdit_putc->setText(str.c_str());

	str = myControl->get_content_post();
	lineEdit_postc->setText(str.c_str());

	QAbstractSocket::SocketState st = QAbstractSocket::UnconnectedState;
	myControl->the_socket_state(myWsId, &st);

	switch (st)
	{
	case QAbstractSocket::UnconnectedState:
		pushButton_ws->setText("Connect");
		break;
	default:
		pushButton_ws->setText("Disconnect");
		break;
	}

	jvxData tout = myControl->get_timeout_sec();
	lineEdit_tout_sec->setText(jvx_data2String(tout, 2).c_str());
}

void
CjvxQtWebControlWidget::edit_get_request()
{
	QString strQ = lineEdit_get->text();
	myControl->set_request_get(strQ.toLatin1().data());
}

void
CjvxQtWebControlWidget::edit_put_request()
{
	QString strQ = lineEdit_put->text();
	myControl->set_request_put(strQ.toLatin1().data());
}

void
CjvxQtWebControlWidget::edit_post_request()
{
	QString strQ = lineEdit_post->text();
	myControl->set_request_post(strQ.toLatin1().data());
}

void
CjvxQtWebControlWidget::edit_wsocket_request()
{
	QString strQ = lineEdit_websocket->text();
	myControl->set_request_wsocket(strQ.toLatin1().data());
}

void
CjvxQtWebControlWidget::edit_put_content()
{
	QString strQ = lineEdit_putc->text();
	myControl->set_content_put(strQ.toLatin1().data());
}

void
CjvxQtWebControlWidget::edit_post_content()
{
	QString strQ = lineEdit_postc->text();
	myControl->set_content_post(strQ.toLatin1().data());
}

void
CjvxQtWebControlWidget::webcontrol_print_text(const std::string& txt)
{
	if (checkBox_track->isChecked())
	{
		listWidget_print->addItem(txt.c_str());
		listWidget_print->setCurrentRow(listWidget_print->count() - 1);
	}
}

void
CjvxQtWebControlWidget::webcontrol_print_text_return(const std::string& txt)
{
	std::string txtL;
	std::string txtRem = txt;
	size_t pos = std::string::npos;
	size_t scnt = 0;

	if (checkBox_track->isChecked())
	{
		while (txtRem.size())
		{
			pos = txtRem.find('\n');
			if (pos != std::string::npos)
			{
				txtL = txtRem.substr(0, pos);
				if (txtRem.size() > pos + 1)
				{
					txtRem = txtRem.substr(pos + 1, std::string::npos);
				} else
				{
					txtRem.clear();
				}
				if (txtL.size())
				{
					listWidget_print->addItem(txtL.c_str());
				}
			} else
			{
				break;
			}
		}
		if (txtRem.size())
		{
			listWidget_print->addItem(txtRem.c_str());
		}
		listWidget_print->setCurrentRow(listWidget_print->count() - 1);
	}
}

// ====================================================================================

jvxErrorType 
CjvxQtWebControlWidget::request_http_api(IjvxHttpApi** theHttpRef)
{
	if (theHttpRef)
		*theHttpRef = static_cast<IjvxHttpApi*>(myControl);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxQtWebControlWidget::return_http_api(IjvxHttpApi* theHttpRef)
{
	return JVX_NO_ERROR;
}

// ==============================================================================

jvxErrorType
CjvxQtWebControlWidget::request_configuration_ext_report(IjvxConfigurationExtender_report** theConfigExtenderReport)
{
	if (theConfigExtenderReport)
		*theConfigExtenderReport = static_cast<IjvxConfigurationExtender_report*>(this);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxQtWebControlWidget::return_configuration_ext_report(IjvxConfigurationExtender_report* theConfigExtenderReport)
{
	if (theConfigExtenderReport == static_cast<IjvxConfigurationExtender_report*>(this))
		return JVX_NO_ERROR;
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType 
CjvxQtWebControlWidget::set_report(IjvxQtWebControlWidget_report* ptr)
{
	bwdRef =  ptr;
	return JVX_NO_ERROR;
};

jvxErrorType
CjvxQtWebControlWidget::get_configuration_ext(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxConfigData* datTmp = NULL;
	std::string uri_target = myControl->get_uri_target();
	jvxInt32 uri_port = myControl->get_uri_port();
	jvxData tout = myControl->get_timeout_sec();

	jvxValue val(uri_port);

	processor->createAssignmentString(&datTmp, JVX_WEBCONTROL_URI_TARGET, uri_target.c_str());
	processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);

	processor->createAssignmentValue(&datTmp, JVX_WEBCONTROL_URI_PORT, val);
	processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);

	val.assign(tout);
	processor->createAssignmentValue(&datTmp, JVX_WEBCONTROL_URI_TIMEOUT, val);
	processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);

	return JVX_NO_ERROR;
};

jvxErrorType
CjvxQtWebControlWidget::put_configuration_ext(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename, jvxInt32 lineno)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* datTmp = NULL;
	std::string token = JVX_WEBCONTROL_URI_TARGET;
	jvxApiString str;
	jvxValue val;
	jvxInt32 ival;
	jvxData dval;
	res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, token.c_str());
	if (res == JVX_NO_ERROR)
	{
		res = processor->getAssignmentString(datTmp, &str);
		assert(res == JVX_NO_ERROR);
		myControl->set_uri_target(str.std_str());
	}
	else
	{
		std::cout << "Failed to read config entry <" << token << ">: " << jvxErrorType_txt(res) << std::endl;
	}


	token = JVX_WEBCONTROL_URI_PORT;	
	datTmp = NULL;
	res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, token.c_str());
	if (res == JVX_NO_ERROR)
	{
		res = processor->getAssignmentValue(datTmp, &val);
		assert(res == JVX_NO_ERROR);
		res = val.toContent(&ival);
		assert(res == JVX_NO_ERROR);
		myControl->set_uri_port(ival);
	}
	else
	{
		std::cout << "Failed to read config entry <" << token << ">: " << jvxErrorType_txt(res) << std::endl;
	}

	token = JVX_WEBCONTROL_URI_TIMEOUT;
	datTmp = NULL;
	res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, token.c_str());
	if (res == JVX_NO_ERROR)
	{
		res = processor->getAssignmentValue(datTmp, &val);
		assert(res == JVX_NO_ERROR);
		res = val.toContent(&dval);
		assert(res == JVX_NO_ERROR);
		myControl->set_timeout_sec(dval);
	}
	else
	{
		std::cout << "Failed to read config entry <" << token << ">: " << jvxErrorType_txt(res) << std::endl;
	}

	this->update_window();
	return JVX_NO_ERROR;
};

void 
CjvxQtWebControlWidget::set_timeout_sec()
{
	QString txt = lineEdit_tout_sec->text();
	jvxData tout = txt.toData();
	myControl->set_timeout_sec(tout);
}

jvxErrorType 
CjvxQtWebControlWidget::websocket_connected(jvxSize uId, jvxHandle* priv)
{
	webcontrol_print_text("WebSocket Connected");
	update_window();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxQtWebControlWidget::websocket_disconnected(jvxSize uId, jvxHandle* priv)
{
	webcontrol_print_text("WebSocket Disconnected");
	update_window();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxQtWebControlWidget::websocket_data_binary(jvxSize uId, jvxByte* payload, jvxSize szFld, jvxHandle* priv)
{
	webcontrol_print_text("WebSocket Data");
	update_window();
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxQtWebControlWidget::websocket_error(jvxSize uId, jvxHandle* priv, const char* reason)
{
	std::string txt = reason;

	std::cout << txt << std::endl;
	webcontrol_print_text(reason);
	update_window();
	return JVX_NO_ERROR;
}
