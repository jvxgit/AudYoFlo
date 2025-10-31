#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSplashScreen>

#include "uMainWindow.h"
#include <iostream>

#include "jvxQtFactoryHostHookupEntries.h"
#include "interfaces/all-hosts/jvxHostHookupEntries.h"
#include "jvx-qt-helpers.h"

#include "jvxTconfigProcessor.h"
#include "jvxTGlobalLock.h"
#include "jvxTSystemTextLog.h"
#include "jvxTSocket.h"
#include "jvxTDataLogger.h"

/*
extern "C"
{
	extern jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, jvxApiString* adescr, jvxComponentType tp, jvxSize id);
}
*/

uMainWindow::uMainWindow() 
{
	theHttpControl = NULL;
	heartbeatState = 0;
	bootupComplete = false;
	
	subWidgets.main.theWidget = NULL;
	myHttpWidget_class = NULL;
	myHttpWidget_wid = NULL;
	myHttpWidget_dialog = NULL;
 	myHttpWidget_cfg = NULL;
	theHttpControl = NULL;
	myHttpWidget_trig_id = JVX_SIZE_UNSELECTED;
	myHttpWidget_autoc_id = JVX_SIZE_UNSELECTED;
	myHttpWidget_autoo_id = JVX_SIZE_UNSELECTED;

	theHostFeatures.allowOverlayOnly_config = false;
	theHostFeatures.cb_loadfilter = NULL;
	theHostFeatures.cb_loadfilter_priv = NULL;

	Q_INIT_RESOURCE(mainApplication);
	viewer_started = false;

	httpRef = NULL;

	runTimer = NULL;

	uIdGroup = 1;

	theConnState = JVX_CONNECTION_STATE_INIT;

	webSockRuntime.stateWebSock = JVX_WEBSOCK_STATE_NONE;
	webSockRuntime.idWebSock = JVX_SIZE_UNSELECTED;

	theWebCfg.ws.numTicksUpdateWebSocket = 10;
	theWebCfg.ws.ticksUpdateWebSocket_msec = 1000;
	theWebCfg.ws.reportData = NULL;
	theWebCfg.ws.useSocket = static_cast<jvxWebSocket_properties_install*>(this);
	theWebCfg.ws.verbose_out = false;

	theWebCfg.flow.autoStartSequencer = false;
	// theWebCfg.flow.startStopHdl = static_cast<jvxHttp_sequencer_startstop*>(this);

	version_info = "unknown";
	connection_info = "-- not connected --";
	pendigSystemUpdates = 0;
	
	auto_connect = c_false;
	auto_observe = c_false;

	initial_setup_request = false;
	high_prio_transfer.uId = 1;
	reportedConnect = false;

	JVX_INITIALIZE_MUTEX(webSockRuntime.mt.safeAccess);
}

uMainWindow::~uMainWindow(void)
{
	JVX_TERMINATE_MUTEX(webSockRuntime.mt.safeAccess);
}

void
uMainWindow::fatalStop(const char* str1, const char* str2)
{
	QMessageBox msgBox(QMessageBox::Critical, str1, str2);
	msgBox.exec();
	exit(-1);
}

void
uMainWindow::report_widget_closed(QWidget* theClosedWidget)
{
	updateWindow((jvxCBitField)1 << JVX_UPDATE_WINDOW_SHIFT);
}

void
uMainWindow::report_widget_specific(jvxSize id, jvxHandle* spec)
{
	switch (id)
	{
	case JVX_QTWIDGET_SPECIFIC_REPORT_UDPATE_WINDOW:
		updateWindow((jvxCBitField)1 << JVX_UPDATE_WINDOW_SHIFT);
		break;
	}
}

/*
void 
uMainWindow::report_widget_closed(QWidget* theClosedWidget)
{
	if (theClosedWidget == myHttpWidget_wid)
	{
		//actionOpen_Audio_Configuration->setChecked(false);
	}
}

void 
uMainWindow::report_widget_specific(jvxSize id, jvxHandle* spec)
{
	switch (id)
	{
	case JVX_QTWIDGET_SPECIFIC_REPORT_UDPATE_WINDOW:
		this->updateWindow();
		break;
	}
}
*/

jvxErrorType
uMainWindow::initSystem(QApplication* hdlApp, char* clStr[], int clNum)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errMess;
	jvxComponentIdentification cpType;
	jvxBool multObj;
	QWidget* theNewWidget = NULL;
	QSplashScreen* spl = NULL;


	jvxCBool cout_mess = c_false;
	jvxCBool verbose_dll = c_false;
	jvxCBool textLog_activate = c_false;
	jvxApiString theStr;

	IjvxConfigurationExtender* cfgExt = NULL;

	commLine.assign_args((const char**)&clStr[1], clNum - 1, clStr[0]);

#ifndef JVX_SPLASH_SUPPRESSED
	spl = jvx_start_splash_qt(theHostFeatures.config_ui.scalefac_splash,
		theHostFeatures.config_ui.pixmap_splash,
		theHostFeatures.config_ui.ftsize_splash,
		theHostFeatures.config_ui.italic_splash);

	if(spl)
	{
		spl->showMessage("Starting web host ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	// Setup the UI elements
	setupUi(this);

	layout()->setContentsMargins(0,0,0,0);

	JVX_REGISTER_QT_METATYPES

	// Pass by reference to subclass
	/*
	// init_specific(this);
	*/
	theControl.init(static_cast<IjvxMainWindowControl_connect*>(this), menuApplication, menuParameters, this);

	// Status variable for bootup sequence
	bootupComplete = false;

	// signals Connect for different purposes
	connect(this, SIGNAL(emit_postMessage(QString, QColor)), this, SLOT(postMessage_inThread(QString, QColor)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_boot_delayed()), this, SLOT(boot_delayed()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_close_app()), this, SLOT(close_app()), Qt::QueuedConnection);
	connect(this, SIGNAL(remit_http_connect()), this, SLOT(http_connect()), Qt::QueuedConnection);

	connect(this, SIGNAL(emit_setup_websocket()), this, SLOT(slot_setup_websocket()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_state_update(QString)), this, SLOT(slot_state_update(QString)), Qt::QueuedConnection);
	// All project specific connections in UI
	/*
	// connect_specific();
	*/
	
	// Start audio dialog window - even if no content can be shown yet
#ifndef JVX_SPLASH_SUPPRESSED	
	if(spl)
	{
		spl->showMessage("Configuring web host ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	res = this->boot_configure(&errMess, static_cast<IjvxCommandLine*>(&commLine), static_cast<IjvxReport*>(this), NULL, NULL, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Initializing web host ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	res = boot_initialize(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Activating web host ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	res = boot_activate(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Preparing web host ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	res = boot_prepare(&errMess, spl);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Starting web host ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	res = boot_start(&errMess, spl);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

	updateWindow((jvxCBitField)1 << JVX_UPDATE_WINDOW_SYSTEM_STARTUP_SHIFT);
	/*
	splash.showMessage("Post boot processing...", Qt::AlignHCenter, QColor(0, 176, 240));
	qApp->processEvents();
	postbootup_specific();
	*/

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Creating windows...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

#ifndef JVX_SPLASH_SUPPRESSED
	jvx_stop_splash_qt(spl, this);
#endif

	// Trigger boot Delayed callback (to leave constructor)
	emit emit_boot_delayed();

	runTimer = new QTimer(this);
	connect(runTimer, SIGNAL(timeout()), this, SLOT(periodic_timeout()));

	label_heartbeat->setPixmap(QPixmap(QString::fromUtf8(":/images/images/not_alive.png")));

	return(JVX_NO_ERROR);
}

void
uMainWindow::closeEvent(QCloseEvent* event)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if (viewer_started)
	{
		trigger_viewer_stop();
	}
	if (webSockRuntime.mt.registeredProperties.size())
	{
		emit emit_close_app();
		event->ignore();
		return;
	}
	if (
		(webSockRuntime.stateWebSock == JVX_WEBSOCK_STATE_NONE) ||
		(webSockRuntime.stateWebSock == JVX_WEBSOCK_STATE_CONNECTING_PENDING))
	{
		// There was no valid connection
	}
	else
	{
		// A web socket has been established which need to be closed at first
		if (webSockRuntime.stateWebSock != JVX_WEBSOCK_STATE_WAIT_FOR_CLOSE)
		{
			webSockRuntime.stateWebSock = JVX_WEBSOCK_STATE_WAIT_FOR_CLOSE;
			httpRef->request_disconnect_websocket(webSockRuntime.theWSId);
		}
		JVX_SLEEP_MS(100);
		emit emit_close_app();
		event->ignore();
		return;
	}

    // If this app is not yet ready to stop, we need to come back later. We have no chance
    // to wait within the QT loop as callbacks which must be stopped need to wait for QT thread
	jvxBool isPending = false;
	if (theHttpControl)
	{
		// Check if any http request is still pending 
		// If so, delay the shutdown until the http requests have been stopped.
		// We put the close on hold by submitting it to the event queue 
		theHttpControl->block_check_pending(&isPending);
	}
	if(isPending)
	{
		// If http requests are still pending, come back later
		emit emit_close_app();
		event->ignore();
		return;
    }
	
	if (!((theConnState == JVX_CONNECTION_STATE_INIT) ||
		(theConnState == JVX_CONNECTION_STATE_STOPPED)))
	{
		// We need to disconnect first!!
		http_disconnect_finalize();
		emit emit_close_app();
		event->ignore();
		return;
	}

	if (this->subWidgets.main.theWidget)
	{
		// Try to shutdown the mainCentral widget. That widget may refuse to shutdown
		res = this->subWidgets.main.theWidget->inform_close_triggered();
		if (res == JVX_ERROR_NOT_READY)
		{
			// If the sequencer is not yet ready, we need to come back later. We have no chance
			// to wait within the QT loop as callbacks which must be stopped need to wait for QT thread
			emit emit_close_app();
			event->ignore();
			return;
		}

		delete(runTimer);

		subWidgets.main.theWidget->inform_about_to_shutdown();

		jvxApiString errMess;
		res = shutdown_stop(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

		res = shutdown_postprocess(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

		res = shutdown_deactivate(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

		res = shutdown_terminate(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}
		commLine.unassign_args();
		theControl.terminate();
	}


    event->accept();
}

void
uMainWindow::close_app()
{
	this->close();
}

#if 0
void
uMainWindow::save()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxFlagTag flagtag = 0;

	if (theHostFeatures.allowOverlayOnly_config)
	{
		if (!this->_command_line_parameters.configFilename_ovlay.empty())
		{
			flagtag = jvxFlagTag(1 << JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_SHIFT);
			res = configureToFile(this->_command_line_parameters.configFilename_ovlay, flagtag);
		}
	}
	else
	{
		if (!this->_command_line_parameters.configFilename.empty())
		{
			res = configureToFile(this->_command_line_parameters.configFilename, flagtag);
		}
	}
}
#endif

void 
uMainWindow::open_preferences()
{
	myHttpWidget_dialog->show();
}

#if 0
void 
uMainWindow::save_as()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxFlagTag flagtag = 0;
	QString str = QFileDialog::getSaveFileName(this, tr("Save to Config File"), tr(".\\"), tr("(*.jvx)"));
	if (!str.isEmpty())
	{
		std::string fname = str.toLatin1().constData();

		if (theHostFeatures.allowOverlayOnly_config)
		{
			this->_command_line_parameters.configFilename_ovlay = fname;
			if (!this->_command_line_parameters.configFilename_ovlay.empty())
			{
				flagtag = jvxFlagTag(1 << JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_SHIFT);
				res = configureToFile(this->_command_line_parameters.configFilename_ovlay, flagtag);
			}
		}
		else
		{
			this->_command_line_parameters.configFilename = fname;
			if (!this->_command_line_parameters.configFilename.empty())
			{
				res = configureToFile(this->_command_line_parameters.configFilename, flagtag);
			}
		}
	}
}
#endif

void 
uMainWindow::open()
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxState theStat = JVX_STATE_NONE;
	QString str = QFileDialog::getOpenFileName(this, tr("Open Config File"), tr(".\\"), tr("(*.jvx)"));
	jvxBool requestHandled = false;
	if (!str.isEmpty())
	{
		openConfigFile_core(str.toLatin1().constData(), theHostFeatures.allowOverlayOnly_config);
	}
}

jvxErrorType
uMainWindow::report_simple_text_message(const char* txt, jvxReportPriority prio)
{
	if (prio > JVX_REPORT_PRIORITY_NONE)
	{
		std::cout << "--> " << jvxReportPriority_txt(prio) << " -- " << txt << std::endl;
	}
	else
	{
		std::cout << "--> " << txt << std::endl;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
	jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize offs, jvxSize num, const jvxComponentIdentification& tpTo,
	jvxPropertyCallPurpose callpurpose )
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
uMainWindow::report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
	jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
	jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
uMainWindow::report_command_request(
	jvxCBitField request, 
	jvxHandle* caseSpecificData, 
	jvxSize szSpecificData)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
uMainWindow::request_command(const CjvxReportCommandRequest& request)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
uMainWindow::report_os_specific(jvxSize commandId, void* context) 
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
uMainWindow::interface_sub_report(IjvxSubReport** subReport)
{
	return JVX_ERROR_UNSUPPORTED;
}

void
uMainWindow::http_connect()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string initCommand;

	if (theConnState == JVX_CONNECTION_STATE_STOPPED)
	{
		// Ignore
		return;
	}
	if (theConnState == JVX_CONNECTION_STATE_INIT)
	{
		httpRef->unblockCheckPending();
		theConnState = JVX_CONNECTION_STATE_INIT_VERSION;
		webSockRuntime.stateWebSock = JVX_WEBSOCK_STATE_CONNECTING_PENDING;
		
		jvxSyncRemoteHttp::initialize(
			subWidgets.main.theWidget, httpRef,
			this->involvedComponents.theHost.hFHost);
		
		// Send out the initial status check
		initCommand = "/jvx/host/system/version";
		httpRef->request_transfer_get(initCommand, uIdGroup, JVX_SIZE_UNSELECTED, static_cast<IjvxHttpApi_httpreport*>(this), NULL);	
		requestedGets[uIdGroup] = initCommand;
		uIdGroup++;		

		version_info = "unknown";
		connection_info = "-- connection started --";
	}
	else 
	{
		jvxErrorType res = JVX_NO_ERROR;

		if (viewer_started)
		{
			trigger_viewer_stop();
		}
		if (webSockRuntime.mt.registeredProperties.size())
		{
			emit remit_http_connect();
			return;
		}

		if (webSockRuntime.stateWebSock == JVX_WEBSOCK_STATE_CONNECTING_PENDING)
		{
			// If we end here, the websocket could not be connected which happens if 
			// other side is not responding.
			webSockRuntime.stateWebSock = JVX_WEBSOCK_STATE_NONE;
		}

		if (webSockRuntime.stateWebSock != JVX_WEBSOCK_STATE_NONE)
		{
			if (webSockRuntime.stateWebSock != JVX_WEBSOCK_STATE_WAIT_FOR_CLOSE)
			{
				webSockRuntime.stateWebSock = JVX_WEBSOCK_STATE_WAIT_FOR_CLOSE;
				httpRef->request_disconnect_websocket(webSockRuntime.theWSId);
			}
			JVX_SLEEP_MS(100);
			emit remit_http_connect();
			return;
		}

		jvxBool isPending = false;
		httpRef->blockCheckPending(&isPending);
		if(isPending)
		{
			emit remit_http_connect();
			return;
		}

		http_disconnect_finalize();
		
		version_info = "unknown";
		connection_info = "-- not connected --";
	}

	this->updateWindow((jvxCBitField)1 << JVX_UPDATE_WINDOW_SYSTEM_STATE_TRANSITION_SHIFT);
}

void
uMainWindow::http_disconnect_finalize()
{
	std::map< jvxComponentIdentification, jvxSyncRemoteHttp::onePartnerComponent> receivedList;
	sync_remote_disconnect();

	jvxSyncRemoteHttp::terminate();

	// if (theConnState == JVX_CONNECTION_STATE_OPERATE)
	//if(theConnState != JVX_CONNECTION_STATE_INIT)
	if(reportedConnect)
	{
		subWidgets.main.theWidget->inform_specific(
			JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_HTTP_DISCONNECTED, &theWebCfg,
			JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_TYPE_HTTP_CONNECT_WEB);
	}
	reportedConnect = false;
	theConnState = JVX_CONNECTION_STATE_STOPPED;
}

void
uMainWindow::updateWindow(jvxCBitField prio)
{
	jvxBool isReady = (webSockRuntime.stateWebSock == JVX_WEBSOCK_STATE_READY);
		
	isReady = isReady && subWidgets.main.theWidget->is_ready();
	if (viewer_started)
	{
		pushButton_startstop->setIcon(QPixmap(QString::fromUtf8(":/images/images/icon_ws_on.png")));
	}
	else
	{
		isReady = isReady && (webSockRuntime.mt.registeredProperties.size() == 0);
		pushButton_startstop->setIcon(QPixmap(QString::fromUtf8(":/images/images/icon_ws_off.png")));
	}
	if (isReady)
	{
		pushButton_startstop->setEnabled(true);
	}
	else
	{
		pushButton_startstop->setEnabled(false);
	}

	switch (seqStat)
	{
	case JVX_SEQUENCER_STATUS_IN_OPERATION:
		pushButton_seqstartstop->setIcon(QPixmap(QString::fromUtf8(":/images/images/icon_stop.png")));
		pushButton_seqbreak->setIcon(QPixmap(QString::fromUtf8(":/images/images/icon_break_on.png")));
		pushButton_seqbreak->setEnabled(true);
		break;
	case JVX_SEQUENCER_STATUS_NONE:
		pushButton_seqstartstop->setIcon(QPixmap(QString::fromUtf8(":/images/images/icon_start.png")));
		pushButton_seqbreak->setIcon(QPixmap(QString::fromUtf8(":/images/images/icon_break_off.png")));
		pushButton_seqbreak->setEnabled(false);
		break;
	}
	if (isReady && 
		((seqStat == JVX_SEQUENCER_STATUS_IN_OPERATION) || (seqStat == JVX_SEQUENCER_STATUS_NONE)))
	{
		pushButton_seqstartstop->setEnabled(true);
	}
	else
	{
		pushButton_seqstartstop->setEnabled(false);
		pushButton_seqbreak->setEnabled(false);
	}

	// ===================================================================
	// ===================================================================
	theControl.update_window();

	subWidgets.main.theWidget->inform_update_window(prio);

	label_version->setText(connection_info.c_str());
	label_version->setToolTip(version_info.c_str());
}

void
uMainWindow::postMessage_outThread(const char* txt, QColor col)
{
	QString txtq = txt;
	emit emit_postMessage(txtq, col);
}

void
uMainWindow::postMessage_inThread(QString txt, QColor col)
{
	//this->subWidgets.messages.theWidget->postMessage_inThread(txt, col);
	std::cout << txt.toLatin1().data() << std::endl;

}

jvxErrorType 
uMainWindow::report_error()
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxHttpApi* httpRef = NULL;
	jvxState stat = JVX_STATE_NONE;

	theConnState = JVX_CONNECTION_STATE_ERROR;
	// std::cout << __FUNCTION__ << std::endl;
	emit remit_http_connect();

	/*
	if (theHttpControl)
	{
		theHttpControl->state(&stat);

		if (stat == JVX_STATE_INIT)
		{
			theHttpControl = NULL;
			if (started)
			{
				this->trigger_stop();
			}
			subWidgets.main.theWidget->inform_inactive(tp, static_cast<IjvxAccessProperties*>(theHttpControl));
			theHttpControl_loc->terminate(&httpRef, NULL);
			res = myHttpWidget_class->return_http_api(httpRef);
			delete theHttpControl_loc;
			theHttpControl = NULL;
		}

		this->updateWindow();
	}*/
	return JVX_NO_ERROR;
}

void
uMainWindow::trigger_ws_startstop()
{
	if (viewer_started)
	{
		// Stop
		trigger_viewer_stop();
	}
	else
	{
		trigger_viewer_start();
	}
}

void
uMainWindow::trigger_seq_startstop()
{
	switch (seqStat)
	{
	case JVX_SEQUENCER_STATUS_NONE:
		trigger_seq_start();
		break;
		//case JVX_SEQUENCER_STATUS_IN_OPERATION:
	default:
		trigger_seq_stop();
		break;
	}
}

void
uMainWindow::trigger_viewer_start()
{
	jvxSize tout_msec = 1000;
	viewer_started = true;

	subWidgets.main.theWidget->inform_viewer_about_to_start(&tout_msec);
	/*
	subWidgets.main.theWidget->inform_specific(JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_HTTP_OBSERVATION_ABOUT_TO_START, NULL, 
		JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_TYPE_NONE);
		*/
	runTimer->start(tout_msec);
	if (theWebCfg.flow.autoStartSequencer)
	{
		trigger_seq_start();
	}

	heartbeatState = 0;

	updateWindow((jvxCBitField)1 << JVX_UPDATE_WINDOW_PROPERTY_VIEWER_STARTUP_SHIFT);
}

void 
uMainWindow::trigger_viewer_stop()
{
	jvxBool noPropertyRegistered = false;
	if (theConnState == JVX_CONNECTION_STATE_ERROR)
	{
		seqStat = JVX_SEQUENCER_STATUS_NONE;
		webSockRuntime.stateWebSock = JVX_WEBSOCK_STATE_NONE;
		
		// Need to secure this field due to high priority messages in extra thread
		JVX_LOCK_MUTEX(webSockRuntime.mt.safeAccess);
		webSockRuntime.mt.registeredProperties.clear();
		JVX_UNLOCK_MUTEX(webSockRuntime.mt.safeAccess);

	}
	else
	{
		if (theWebCfg.flow.autoStartSequencer)
		{
			trigger_seq_stop();
		}
	}

	/*
	runTimer->stop();
	label_heartbeat->setPixmap(QPixmap(QString::fromUtf8(":/images/images/not_alive.png")));
	*/
	stop_timer_viewer();

	if (webSockRuntime.mt.registeredProperties.size() == 0)
	{
		noPropertyRegistered = true;
	}
	subWidgets.main.theWidget->inform_viewer_about_to_stop();
	/*
	subWidgets.main.theWidget->inform_specific(JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_HTTP_OBSERVATION_ABOUT_TO_STOP, NULL, JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_TYPE_NONE);
	*/
	if (noPropertyRegistered)
	{
		// If no property was registered, otherwise where the props are reported as unregistered
		viewer_started = false;
		subWidgets.main.theWidget->inform_viewer_stopped();
	}
	
	updateWindow((jvxCBitField)1 << JVX_UPDATE_WINDOW_PROPERTY_VIEWER_SHUTDOWN_SHIFT);
}

jvxErrorType
uMainWindow::trigger_seq_start()
{
	std::string initCommand = "/jvx/host/sequencer/start";
	subWidgets.main.theWidget->inform_sequencer_about_to_start();
	httpRef->request_transfer_post(initCommand, "", uIdGroup, JVX_SIZE_UNSELECTED, static_cast<IjvxHttpApi_httpreport*>(this), NULL);
	requestedPosts[uIdGroup] = initCommand;
	uIdGroup++;
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::trigger_seq_stop()
{
	std::string initCommand = "/jvx/host/sequencer/stop";
	subWidgets.main.theWidget->inform_sequencer_about_to_stop();
	httpRef->request_transfer_post(initCommand, "", uIdGroup, JVX_SIZE_UNSELECTED, static_cast<IjvxHttpApi_httpreport*>(this), NULL);
	requestedPosts[uIdGroup] = initCommand;
	uIdGroup++;
	return JVX_NO_ERROR;
}

void
uMainWindow::trigger_seq_break()
{
	std::string initCommand = "/jvx/host/sequencer/stops";
	subWidgets.main.theWidget->inform_sequencer_about_to_stop();
	httpRef->request_transfer_post(initCommand, "", uIdGroup, JVX_SIZE_UNSELECTED, static_cast<IjvxHttpApi_httpreport*>(this), NULL);
	requestedPosts[uIdGroup] = initCommand;
	uIdGroup++;
}

// =======================================================================+

jvxErrorType
uMainWindow::forward_trigger_operation(
	jvxMainWindowController_trigger_operation_ids id_operation,
	jvxHandle* theData)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	jvxMainWindowController_specificSubIds_hdr* hdr_spec =
		(jvxMainWindowController_specificSubIds_hdr*)theData;

	switch (id_operation)
	{
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_UPDATE_WINDOW:
		updateWindow((jvxCBitField)1 << JVX_UPDATE_WINDOW_SHIFT);
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_VIEWER_START:
		if (!viewer_started)
		{
			this->trigger_viewer_start();
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_VIEWER_STOP:
		if (viewer_started)
		{
			this->trigger_viewer_stop();
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_START:
		if (seqStat == JVX_SEQUENCER_STATUS_NONE)
		{
			this->trigger_seq_start();
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_STOP:
		if (seqStat != JVX_SEQUENCER_STATUS_NONE)
		{
			this->trigger_seq_stop();
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_STOPS:
		this->trigger_seq_break();
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_HIDE_EXPERT_UI:
		/*
		subWidgets.sequences.theWidgetD->hide();
		subWidgets.realtimeViewer.props.theWidgetD->hide();
		subWidgets.realtimeViewer.plots.theWidgetD->hide();
		subWidgets.messages.theWidgetD->hide();
		menuConfiguration->menuAction()->setVisible(false);
		menuView->menuAction()->setVisible(false);
		menuRuntime_Parameters->menuAction()->setVisible(false);
		menuPreferences->menuAction()->setVisible(false);
		menuSettings->menuAction()->setVisible(false);
		*/
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SHOW_EXPERT_UI:
		/*
		subWidgets.sequences.theWidgetD->show();
		subWidgets.realtimeViewer.props.theWidgetD->show();
		subWidgets.realtimeViewer.plots.theWidgetD->show();
		subWidgets.messages.theWidgetD->show();
		menuConfiguration->menuAction()->setVisible(true);
		menuView->menuAction()->setVisible(true);
		menuRuntime_Parameters->menuAction()->setVisible(true);
		menuPreferences->menuAction()->setVisible(true);
		menuSettings->menuAction()->setVisible(true);
		*/
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SET_WINDOW_TITLE:
		this->setWindowTitle(reinterpret_cast<const char*>(theData));
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_ADJUST_SIZE:
		this->adjustSize();
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SAFE_CONFIG:
		this->saveConfigFile();
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_OPEN_CONFIG_FULL:
		this->openConfigFile_core((const char*)theData, false);
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_OPEN_CONFIG_OVLAY:
		this->openConfigFile_core((const char*)theData, true);
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SPECIFIC:
		switch (hdr_spec->id_subid)
		{
		case JVX_WEB_HOST_TRIGGER_SUB_ID_HIDE_ALIVE_IMAGE:
			label_heartbeat->hide();
			res = JVX_NO_ERROR;
			break;
		default:
			res = JVX_ERROR_UNSUPPORTED;
			break;
		}
		break;

	default:
		break;
	}

	return(res);
}

void
uMainWindow::periodic_timeout()
{
	subWidgets.main.theWidget->inform_update_window_periodic();

	heartbeatState = (heartbeatState + 1) % JVX_NUM_HEARTBEAT_MAX;
	update_heartbeat();
}

void uMainWindow::boot_delayed()
{
	jvxBool wantsToAdjust = false;
	subWidgets.main.theWidget->inform_bootup_complete(&wantsToAdjust);
	if (wantsToAdjust)
	{
		this->adjustSize();
	}
	else
	{
		this->showMaximized();
	}

	if (auto_connect)
	{
		http_connect();
	}
}

// Entries for higher level functions
jvxErrorType
uMainWindow::boot_initialize_specific(jvxApiString* errloc)
{
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxApiString descriptionComponent;
	jvxSize i;
	oneAddedStaticComponent comp;
	QWidget* theNewWidget = NULL;

	theEventLoop.initialize(involvedComponents.theHost.hFHost, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, 1000);
	theEventLoop.start();
	involvedComponents.theHost.hFHost->add_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxEventLoop*>(&theEventLoop)), JVX_INTERFACE_EVENTLOOP);

	// ====================================================================
	// Start main central widget
	// We need a valid host reference with a valid report reference when setting up the main widget	
	subWidgets.main.theWidget = NULL;
	mainWindow_UiExtension_hostfactory_init(&subWidgets.main.theWidget, static_cast<QWidget*>(this));
	assert(subWidgets.main.theWidget);
	subWidgets.main.theWidget->init_submodule(involvedComponents.theHost.hFHost);
	subWidgets.main.theWidget->return_widget(&theNewWidget);
	assert(theNewWidget);

	// Set firwst and afterwards request some additional features from host
	theHostFeatures.mainWidgetInScrollArea = true;

	jvx_configure_factoryhost_features(&theHostFeatures);

	pushButton_seqstartstop->setMinimumSize(theHostFeatures.config_ui.iconw_small, theHostFeatures.config_ui.iconh_small);
	pushButton_seqstartstop->setMaximumSize(theHostFeatures.config_ui.iconw_small, theHostFeatures.config_ui.iconh_small);
	pushButton_seqstartstop->setIconSize(QSize(theHostFeatures.config_ui.iconw_small-2, theHostFeatures.config_ui.iconh_small-2));

	pushButton_seqbreak->setMinimumSize(theHostFeatures.config_ui.iconw_small, theHostFeatures.config_ui.iconh_small);
	pushButton_seqbreak->setMaximumSize(theHostFeatures.config_ui.iconw_small, theHostFeatures.config_ui.iconh_small);
	pushButton_seqbreak->setIconSize(QSize(theHostFeatures.config_ui.iconw_small - 2, theHostFeatures.config_ui.iconh_small - 2));

	pushButton_startstop->setMinimumSize(theHostFeatures.config_ui.iconw_small, theHostFeatures.config_ui.iconh_small);
	pushButton_startstop->setMaximumSize(theHostFeatures.config_ui.iconw_small, theHostFeatures.config_ui.iconh_small);
	pushButton_startstop->setIconSize(QSize(theHostFeatures.config_ui.iconw_small - 2, theHostFeatures.config_ui.iconh_small - 2));

	if (theHostFeatures.mainWidgetInScrollArea)
	{
		scrollArea->setWidget(theNewWidget);
		scrollArea->setWidgetResizable(true);
		frame_cwidget->hide();
	}
	else
	{
		QLayout* ll = frame_cwidget->layout();
		if (ll == NULL)
		{
			ll = new QGridLayout();
			frame_cwidget->setLayout(ll);
		}
		ll->addWidget(theNewWidget);
		scrollArea->hide();
	}
	//this->setCentralWidget(theNewWidget);

	LOAD_ONE_MODULE_LIB_FULL(jvxTSystemTextLog_init,
		jvxTSystemTextLog_terminate, "Text Log",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTconfigProcessor_init,
		jvxTconfigProcessor_terminate, "Config Parser",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTGlobalLock_init,
		jvxTGlobalLock_terminate, "Global Lock",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTSocket_init,
		jvxTSocket_terminate, "Socket",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTDataLogger_init,
		jvxTDataLogger_terminate, "Data Logger",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);

	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_HOST);
	//involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_PACKETFILTER_RULE, false);

	// Allow to attach local static libs
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		jvxSize cnt = 0;
		while (1)
		{
			comp.reset();
			resL = jvx_access_link_objects(&comp.funcInit, &comp.funcTerm, &descriptionComponent, (jvxComponentType)i, cnt);
			if (resL == JVX_NO_ERROR)
			{
				comp.theStaticObject = NULL;
				resL = comp.funcInit(&comp.theStaticObject, &comp.theStaticGlobal, NULL);
				assert(resL == JVX_NO_ERROR);
				assert(comp.theStaticObject);
				resL = involvedComponents.theHost.hFHost->add_external_component(comp.theStaticObject, 
					comp.theStaticGlobal,
					descriptionComponent.c_str(), true, 
					comp.funcInit, comp.funcTerm);
				if (resL == JVX_NO_ERROR)
				{
					involvedComponents.addedStaticObjects.push_back(comp);
				}
				else
				{
					comp.funcTerm(comp.theStaticObject);
				}
			}
			else
			{
				break;
			}
			cnt++;
		}
	}

	if (theHostFeatures.cb_loadfilter)
	{
		involvedComponents.theHost.hFHost->set_component_load_filter_function(theHostFeatures.cb_loadfilter, theHostFeatures.cb_loadfilter_priv);
	}

	// Start secondrary widget
	init_jvxQtWidgetFrameDialog(&myHttpWidget_frame, 0);
	myHttpWidget_frame->init(static_cast<IjvxQtSpecificWidget_report*>(this));
	myHttpWidget_frame->getMyQDialog(&myHttpWidget_dialog, 0);
	init_jvxQtWebControl(&myHttpWidget_class, myHttpWidget_dialog);
	myHttpWidget_class->init(involvedComponents.theHost.hFHost, 0, NULL, static_cast<IjvxQtSpecificWidget_report*>(this));
	myHttpWidget_class->set_report(static_cast<IjvxQtWebControlWidget_report*>(this));
	myHttpWidget_class->getMyQWidget(&myHttpWidget_wid);
	myHttpWidget_frame->attachWidget(myHttpWidget_wid);

	// Get the http API reference
	res = myHttpWidget_class->request_http_api(&httpRef);

	// Add HTTP interface to host to be taken by other components
	involvedComponents.theHost.hFHost->add_external_interface(reinterpret_cast<jvxHandle*>(httpRef), JVX_INTERFACE_HTTP_API);

	theControl.register_action_widget("Http Connect Control", myHttpWidget_class, myHttpWidget_frame);
	theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_ONOFF,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Http Connect Trigger",
		&myHttpWidget_trig_id,
		false, 
		JVX_REGISTER_FUNCTION_MENU_SEPARATOR_IF_NOT_FIRST);
	theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_ONOFF,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Http Auto Connect",
		&myHttpWidget_autoc_id,
		false, JVX_REGISTER_FUNCTION_MENU_NO_SEPARATOR);
	theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_ONOFF,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Http Auto Observe",
		&myHttpWidget_autoo_id,
		false, 
		JVX_REGISTER_FUNCTION_MENU_NO_SEPARATOR);

	if (subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->init_extend_specific_widgets(static_cast<IjvxMainWindowControl*>(&theControl), static_cast<IjvxQtSpecificWidget_report*>(this));
	}

	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::boot_specify_slots_specific() 
{
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::boot_prepare_specific(jvxApiString* errloc)
{
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::boot_start_specific(jvxApiString* errloc)
{
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::boot_activate_specific(jvxApiString* errloc)
{
	jvxErrorType res = JVX_NO_ERROR;

	/* Activate all attached widgets */
	theControl.activate();

	IjvxConfigurationExtender* cfgExtender = NULL;
	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle**>(&cfgExtender));
	if ((res == JVX_NO_ERROR) && cfgExtender)
	{
		res = myHttpWidget_class->request_configuration_ext_report(&myHttpWidget_cfg);
		if (res == JVX_NO_ERROR)
		{
			assert(myHttpWidget_cfg);
			res = cfgExtender->register_extension(myHttpWidget_cfg, JVX_WEBCONTROL);
		}
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle*>(cfgExtender));
	}

	return JVX_NO_ERROR;
}


jvxErrorType
uMainWindow::shutdown_terminate_specific(jvxApiString* errloc)
{
	jvxSize i;

	if (subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->terminate_extend_specific_widgets();
	}
	theControl.unregister_action_widget("Http Connect", myHttpWidget_class, myHttpWidget_frame);

	// Add HTTP interface to host to be taken by other components
	involvedComponents.theHost.hFHost->remove_external_interface(reinterpret_cast<jvxHandle*>(httpRef), JVX_INTERFACE_HTTP_API);
	myHttpWidget_class->return_http_api(httpRef);
	httpRef = NULL;

	myHttpWidget_frame->detachWidget(myHttpWidget_wid);
	myHttpWidget_wid = NULL;
	myHttpWidget_class->set_report(NULL);
	myHttpWidget_class->terminate();
	terminate_jvxQtWebControl(myHttpWidget_class);
	myHttpWidget_class = NULL;
	
	myHttpWidget_dialog = NULL;
	//myHttpWidget_frame->terminate();
	terminate_jvxQtWidgetFrameDialog(myHttpWidget_frame);
	myHttpWidget_frame = NULL;

	for (i = 0; i < involvedComponents.addedStaticObjects.size(); i++)
	{
		UNLOAD_ONE_MODULE_LIB_FULL(involvedComponents.addedStaticObjects[i], involvedComponents.theHost.hFHost);
	}
	involvedComponents.addedStaticObjects.clear();

	this->involvedComponents.theHost.hFHost->remove_component_load_blacklist(JVX_COMPONENT_HOST);

	// ====================================================================
	// Stop main central widget
	subWidgets.main.theWidget->terminate_submodule();
	mainWindow_UiExtension_hostfactory_terminate(subWidgets.main.theWidget);
	subWidgets.main.theWidget = NULL;

	theEventLoop.stop();
	theEventLoop.terminate();
	involvedComponents.theHost.hFHost->remove_external_interface(
		reinterpret_cast<jvxHandle*>(static_cast<IjvxEventLoop*>(&theEventLoop)), JVX_INTERFACE_EVENTLOOP);

	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::shutdown_deactivate_specific(jvxApiString* errloc)
{
	// Better to be done in a better way in the future!!
	if (theHttpControl)
	{
		subWidgets.main.theWidget->inform_inactive(tp, static_cast<IjvxAccessProperties*>(theHttpControl));
		theHttpControl->terminate(&httpRef, NULL);
		delete theHttpControl;
		theHttpControl = NULL;
	}

	IjvxConfigurationExtender* cfgExtender = NULL;
	jvxErrorType res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle**>(&cfgExtender));
	if ((res == JVX_NO_ERROR) && cfgExtender)
	{
		res = myHttpWidget_class->request_configuration_ext_report(&myHttpWidget_cfg);
		if (res == JVX_NO_ERROR)
		{
			assert(myHttpWidget_cfg);
			res = cfgExtender->unregister_extension(JVX_WEBCONTROL);
		}
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle*>(cfgExtender));
	}

	theControl.deactivate();	

	return JVX_NO_ERROR;
}


jvxErrorType
uMainWindow::shutdown_postprocess_specific(jvxApiString* errloc)
{
	return JVX_NO_ERROR;
}


jvxErrorType
uMainWindow::shutdown_stop_specific(jvxApiString* errloc)
{
	return JVX_NO_ERROR;
}

void
uMainWindow::report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context)
{
	report_simple_text_message(txt, prio);
}

void
uMainWindow::inform_config_read_start(const char* nmfile, jvxHandle* context)
{
}

void
uMainWindow::inform_config_read_stop(jvxErrorType resRead, const char* nmfile, jvxHandle* context)
{
}

void
uMainWindow::get_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)
{
	jvxValue val;
	val.assign(auto_connect);
	ext->set_configuration_entry(JVX_WEBCONTROL_AUTO_CONNECT, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, JVX_SIZE_UNSELECTED);

	val.assign(auto_observe);
	ext->set_configuration_entry(JVX_WEBCONTROL_AUTO_OBSERVE, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, JVX_SIZE_UNSELECTED);
}

void
uMainWindow::get_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data)
{
}

void
uMainWindow::put_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data, const char* fName)
{
}

void
uMainWindow::put_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxValue val;
	jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
	res = ext->get_configuration_entry(JVX_WEBCONTROL_AUTO_CONNECT, &val, &tp, JVX_SIZE_UNSELECTED);
	if (res == JVX_NO_ERROR)
	{
		val.toContent(&auto_connect);
	}

	tp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
	res = ext->get_configuration_entry(JVX_WEBCONTROL_AUTO_OBSERVE, &val, &tp, JVX_SIZE_UNSELECTED);
	if (res == JVX_NO_ERROR)
	{
		val.toContent(&auto_observe);
	}
}

void uMainWindow::report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName)
{
}

jvxErrorType
uMainWindow::get_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id)
{
	if (id == myHttpWidget_trig_id)
	{
		jvxState stat = JVX_STATE_NONE;
		if (theHttpControl == NULL)
		{
			var.assign(false);
		}
		else
		{
			theHttpControl->state(&stat);

			if (stat == JVX_STATE_NONE)
			{
				var.assign(false);
			}
			else if (stat == JVX_STATE_INIT)
			{
				var.assign(true);
			}
		}
	}
	if (id == myHttpWidget_autoc_id)
	{
		var.assign(auto_connect == c_true);
	}
	if (id == myHttpWidget_autoo_id)
	{
		var.assign(auto_observe == c_true);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::set_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id)
{
	if (id == myHttpWidget_trig_id)
	{
		// If connections was autostopped before, set it to init again here!
		if (theConnState == JVX_CONNECTION_STATE_STOPPED)
		{
			theConnState = JVX_CONNECTION_STATE_INIT;
		}
		http_connect();
	}

	if (id == myHttpWidget_autoc_id)
	{
		// Toggle
		auto_connect = !auto_connect;
	}

	if (id == myHttpWidget_autoo_id)
	{
		// Toggle
		auto_observe = !auto_observe;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::complete_transfer_http(jvxSize uniqueIdGroup, jvxSize uniqueId_ingroup, const char* response, jvxHandle* priv, jvxCBitField reqId)
{
	jvxErrorType resL = JVX_NO_ERROR;
	std::vector<std::string> errs;

	if (jvx_bitTest(reqId, JVX_WEB_SERVER_URI_POST_HANDLER_SHIFT))
	{
		// Post requests arrive here
		std::map<jvxSize, std::string>::iterator elm = requestedPosts.find(uniqueIdGroup);
		if (elm != requestedPosts.end())
		{
			requestedPosts.erase(elm);
		}
	}
	else
	{
		// All others are GET requests
		if (theConnState == JVX_CONNECTION_STATE_INIT_VERSION)
		{
			std::map<jvxSize, std::string>::iterator elm = requestedGets.find(uniqueIdGroup);
			if (elm != requestedGets.end())
			{
				CjvxJsonElementList lst;
				CjvxJsonElementList::stringToRepresentation(response, lst, errs);

				connection_info = "-- connected --";
				version_to_tag(lst, version_info);

				requestedGets.erase(elm);

				theConnState = JVX_CONNECTION_STATE_OPERATE;

				// Report that module has been connected
				resL = subWidgets.main.theWidget->inform_specific(
					JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_HTTP_CONNECTED, &theWebCfg,
					JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_TYPE_HTTP_CONNECT_WEB);
				if (resL == JVX_NO_ERROR)
				{
					// Start web socket
					if (httpRef)
					{
						assert(webSockRuntime.stateWebSock == JVX_WEBSOCK_STATE_CONNECTING_PENDING);
						webSockRuntime.stateWebSock = JVX_WEBSOCK_STATE_CONNECTING;
						webSockRuntime.idWebSock = JVX_SIZE_UNSELECTED;

						resL = httpRef->request_connect_websocket("jvx/host/socket/", &webSockRuntime.theWSId,
							static_cast<IjvxHttpApi_wsreport*>(this), NULL);
						assert(resL == JVX_NO_ERROR);
					}
				}
				else
				{
					assert(0);
				}
				reportedConnect = true;
			}
			else
			{
				assert(0);
			}
			// Do what is needed
		}
		else if (theConnState == JVX_CONNECTION_STATE_UPDATE)
		{
			std::map<jvxSize, std::string>::iterator elm = requestedGets.find(uniqueIdGroup);
			if (elm != requestedGets.end())
			{
				if (pendigSystemUpdates > 0)
				{
					request_status();
					pendigSystemUpdates--;
				}
				else
				{
					// Get request mapped to subsequent thread to allow killall command
					QString respq = response;
					emit emit_state_update(respq);
				}
				requestedGets.erase(elm);
			}
			else
			{
				assert(0);
			}
		}
		else
		{
			std::cout << "Unexpected status update in state OPERATE " << std::endl;
			//assert(0);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::failed_transfer_http(jvxSize uniqueIdGroup, jvxSize uniqueId_ingroup, jvxErrorType errRes, jvxHandle* priv, jvxCBitField reqId)
{
	if (theConnState == JVX_CONNECTION_STATE_INIT_VERSION)
	{
		std::map<jvxSize, std::string>::iterator elm = requestedGets.find(uniqueIdGroup);
		if (elm != requestedGets.end())
		{
			requestedGets.erase(elm);
		}
		else
		{
			assert(0);
		}

		version_info = "unknown";
		connection_info = "-- not connected --";

		theConnState = JVX_CONNECTION_STATE_INIT;
		updateWindow((jvxCBitField)1 << JVX_UPDATE_WINDOW_ERROR_SHIFT);
	}
	else
	{
		// Do nothing. This may be due to a hanging connection
		// assert(0);
	}
	return JVX_NO_ERROR;
}

void
uMainWindow::slot_state_update(QString strq)
{
	std::vector<std::string> errs; 

	if (theConnState == JVX_CONNECTION_STATE_UPDATE)
	{
		jvxBool is_seq_state_transition = false;
		jvxBool is_system_state_transition = false;

		CjvxJsonElementList lst;
		CjvxJsonElementList::stringToRepresentation(strq.toLatin1().data(), lst, errs);

		// if(l)
		sync_remote(lst, is_system_state_transition, is_seq_state_transition);

		if (initial_setup_request)
		{
			// This ws the very first request of the status. Here is where the auto observe should be placed!
			if (auto_observe)
			{
				trigger_viewer_start();
			}
			initial_setup_request = false;
		}
		// std::cout << "Switching state to operate" << std::endl;
		theConnState = JVX_CONNECTION_STATE_OPERATE;
		
		jvxCBitField report_prio = 0;
		if (is_system_state_transition)
		{
			jvx_bitSet(report_prio, JVX_UPDATE_WINDOW_SYSTEM_STATE_REMOTE_TRANSISTION_SHIFT);
		}
		if (is_seq_state_transition)
		{
			jvx_bitSet(report_prio, JVX_UPDATE_WINDOW_SYSTEM_STATE_TRANSITION_SHIFT);
		}
		else
		{
			jvx_bitSet(report_prio, JVX_UPDATE_WINDOW_SEQ_STATE_REPORT_SHIFT);
		}
		this->updateWindow(report_prio);
	}
}

// =======================================================================
jvxErrorType 
uMainWindow::websocket_connected(jvxSize uId, jvxHandle* priv)
{
	assert(webSockRuntime.stateWebSock == JVX_WEBSOCK_STATE_CONNECTING);

	webSockRuntime.stateWebSock = JVX_WEBSOCK_STATE_CONNECTED;
	webSockRuntime.idWebSock = uId;

	// Prepare the setup for web sockets
	emit emit_setup_websocket();

	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::websocket_error(jvxSize uId, jvxHandle* priv, const char* descrErr) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::websocket_disconnected(jvxSize uId, jvxHandle* priv) 
{
	JVX_LOCK_MUTEX(webSockRuntime.mt.safeAccess);
	std::map<jvxComponentIdentification, propertiesOneComponent>::iterator elm = webSockRuntime.mt.registeredProperties.begin();
	for (; elm != webSockRuntime.mt.registeredProperties.end(); elm++)
	{
		std::map<jvxSize, jvxOneRemotePropertySpecification>::iterator elmi = elm->second.registeredProperties.begin();
		for (; elmi != elm->second.registeredProperties.end(); elmi++)
		{
			if (theWebCfg.ws.verbose_out)
			{
				std::cout << "Unregistering property #" << elmi->first << ": " << elmi->second.nmProp << "--" <<
					jvxComponentIdentification_txt(elmi->second.cpTp) << std::endl;
			}

			if (elmi->second.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
			{
				this->try_detach_udp_socket(elmi->second.port, elmi->second.port_uid);
			}
		}
		elm->second.registeredProperties.clear();
		elm->second.propertiesToRegister.clear();
	}

	webSockRuntime.mt.registeredProperties.clear();
	JVX_UNLOCK_MUTEX(webSockRuntime.mt.safeAccess);

	if (viewer_started)
	{
		viewer_started = false;
		stop_timer_viewer();

		// If web socket fell and properties could not be removed, we need to report end of viewer here
		subWidgets.main.theWidget->inform_viewer_stopped();
	}

	if (webSockRuntime.stateWebSock == JVX_WEBSOCK_STATE_WAIT_FOR_CLOSE)
	{
		if (theWebCfg.ws.verbose_out)
		{
			std::cout << "Websocket down" << std::endl;
		}
		webSockRuntime.stateWebSock = JVX_WEBSOCK_STATE_NONE;
	}
	else
	{
		if (theWebCfg.ws.verbose_out)
		{
			std::cout << "Websocket closed" << std::endl;
		}
		// Stop the current connection session
		if (theConnState != JVX_CONNECTION_STATE_INIT)
		{
			this->http_connect();
		}
		// Other side has closed connection, disconnect
	}
	webSockRuntime.stateWebSock = JVX_WEBSOCK_STATE_NONE;
	initial_setup_request = false;
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::websocket_data_binary(jvxSize uId, jvxByte* payload, jvxSize szFld, jvxHandle* priv) 
{
	jvxErrorType resL = JVX_NO_ERROR;
	assert(szFld >= sizeof(jvxProtocolHeader));
	if (szFld >= sizeof(jvxProtocolHeader))
	{
		jvxProtocolHeader* hdr = (jvxProtocolHeader*)payload;
		if (hdr->fam_hdr.proto_family == JVX_PROTOCOL_TYPE_PROPERTY_STREAM)
		{
			// Accept message responses from connected application
			if (
				((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
				((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PS_CONFIGURE_PROPERTY_OBSERVATION))
			{
				// ======================================================
				// Configure timer on web socket thread accepted
				// ======================================================
				jvxPropertyConfigurePropertySend_response* rhdr = (jvxPropertyConfigurePropertySend_response*)payload;
				assert(szFld >= sizeof(jvxPropertyConfigurePropertySend_response));

				switch (webSockRuntime.stateWebSock)
				{
				case JVX_WEBSOCK_STATE_SETUP_PENDING:
					if (rhdr->errcode == JVX_NO_ERROR)
					{
						if (theWebCfg.ws.verbose_out)
						{
							std::cout << "Websocket up" << std::endl;
						}
					}
					else
					{
						if (theWebCfg.ws.verbose_out)
						{
							std::cout << "Websocket failed" << std::endl;
						}
						// Error registering properties
					}

					webSockRuntime.stateWebSock = JVX_WEBSOCK_STATE_READY;
					
					// Show that we are ready!!
					updateWindow((jvxCBitField)1 << JVX_UPDATE_WINDOW_PROPERTY_CONNECTED_SHIFT);

					// Here, we run the very first update of the system
					assert(theConnState == JVX_CONNECTION_STATE_OPERATE);
					if (theWebCfg.ws.verbose_out)
					{
						std::cout << "Switching state to update" << std::endl;
					}
					theConnState = JVX_CONNECTION_STATE_UPDATE;

					initial_setup_request = true;
					
					request_status();

					/*
					if (auto_observe)
					{
						trigger_viewer_start();
					}
					*/

					break;
				}
			}
			else if (
				((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
				((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PS_ADD_PROPERTY_TO_OBSERVE))
			{
				// ======================================================
				// Adding of new property via web socket accepted
				// ======================================================
				jvxPropertyPropertyObserveHeader_response* rhdr = (jvxPropertyPropertyObserveHeader_response*)payload;
				assert(szFld >= sizeof(jvxPropertyPropertyObserveHeader_response));
				assert(webSockRuntime.stateWebSock == JVX_WEBSOCK_STATE_READY);
				if (rhdr->errcode == JVX_NO_ERROR)
				{
					jvxComponentIdentification tpCp;
					tpCp.tp = (jvxComponentType)rhdr->component_type;
					tpCp.slotid = rhdr->component_slot;
					tpCp.slotsubid = rhdr->component_subslot;

					std::map<jvxComponentIdentification, propertiesOneComponent>::iterator elm = webSockRuntime.mt.registeredProperties.find(tpCp);
					if (elm != webSockRuntime.mt.registeredProperties.end())
					{
						std::map<jvxSize, jvxOneRemotePropertySpecification>::iterator elmi = elm->second.propertiesToRegister.begin();
						if (elmi != elm->second.propertiesToRegister.end())
						{
							// Move from the one list to the other
							jvxOneRemotePropertySpecification newProp = elmi->second;
							elm->second.registeredProperties[rhdr->stream_id] = newProp;

							if (theWebCfg.ws.reportData)
							{
								theWebCfg.ws.reportData->report_property_registered(elm->first, elmi->second.user_data);
							}
							
							// Property successfully registered
							if (theWebCfg.ws.verbose_out)
							{
								std::cout << "Property successfully registered at web control: stream id #" << rhdr->stream_id
									<< " mapped to prop user id #" << rhdr->user_specific << " -- " <<
									elmi->second.nmProp << "--" << jvxComponentIdentification_txt(elmi->second.cpTp) << std::endl;
							}
							elm->second.propertiesToRegister.erase(elmi);

						}
					}
					else
					{
						std::cout << __FUNCTION__ << ": Web socket disconnected while viewer was about to register properties." << std::endl;
					}
				}
				else
				{
					assert(0);
				}

				// Procede with next parameter
				trigger_setup_next_prop();
			}
			else if (
				((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE) &&
				((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PS_REMOVE_PROPERTY_TO_OBSERVE))
			{
				jvxPropertyPropertyObserveHeader_response* rhdr = (jvxPropertyPropertyObserveHeader_response*)payload;
				assert(szFld >= sizeof(jvxPropertyPropertyObserveHeader_response));
				if (rhdr->errcode == JVX_NO_ERROR)
				{
					jvxComponentIdentification tpCp;
					tpCp.tp = (jvxComponentType)rhdr->component_type;
					tpCp.slotid = rhdr->component_slot;
					tpCp.slotsubid = rhdr->component_subslot;

					std::map<jvxComponentIdentification, propertiesOneComponent>::iterator elm = webSockRuntime.tobereleasedProperties.find(tpCp);
					if (elm != webSockRuntime.tobereleasedProperties.end())
					{
						std::map<jvxSize, jvxOneRemotePropertySpecification>::iterator elmi = elm->second.registeredProperties.begin();
						for (; elmi != elm->second.registeredProperties.end(); elmi++)
						{
							if (theWebCfg.ws.verbose_out)
							{
								std::cout << "Property successfully unregistered at web control: stream id #" << rhdr->stream_id
									<< " mapped to prop user id #" << rhdr->user_specific << " -- " <<
									elmi->second.nmProp << "--" << jvxComponentIdentification_txt(elmi->second.cpTp) << std::endl;
							}
							
							if (elmi->second.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
							{
							    this->try_detach_udp_socket(elmi->second.port, elmi->second.port_uid);
							}
							
							if (theWebCfg.ws.reportData)
							{
								theWebCfg.ws.reportData->report_property_unregistered(elm->first, elmi->second.user_data);
							}
						}
						elm->second.registeredProperties.clear();
						webSockRuntime.tobereleasedProperties.erase(elm);
						if (webSockRuntime.tobereleasedProperties.size())
						{
							trigger_release_props(JVX_COMPONENT_ALL_LIMIT);
						}
						else
						{
							viewer_started = false;
							subWidgets.main.theWidget->inform_viewer_stopped();
							updateWindow((jvxCBitField)1 << JVX_UPDATE_WINDOW_PROPERTY_DISCONNECTED_SHIFT);
						}
					}
					else
					{
						assert(0);
					}
				}
				else
				{
					assert(0);
				}
			}
			else if (
			((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER) &&
			((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PS_SEND_INFORM_SYSTEM_CHANGE))
			{
				if (theWebCfg.ws.verbose_out)
				{
					std::cout << "System Update request" << std::endl;
				}
				if (theConnState == JVX_CONNECTION_STATE_OPERATE)
				{
					// This triggers a refresh of the syste status
					if (theWebCfg.ws.verbose_out)
					{
						std::cout << "Switching state to update" << std::endl;
					}
					theConnState = JVX_CONNECTION_STATE_UPDATE;
					request_status();
				}
				else
				{
					pendigSystemUpdates++;
				}
			}
			else if (
			((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER) &&
			((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PS_SEND_INFORM_SEQUENCER_EVENT))
			{
				jvxSequencerEventHeader* hdrSeq = (jvxSequencerEventHeader*)payload;
				assert(szFld >= sizeof(jvxSequencerEventHeader));
				if (
					(hdrSeq->seq_elm_tp == JVX_SEQUENCER_TYPE_CALLBACK) &&
					(hdrSeq->seq_event_mask == JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP))
				{
					subWidgets.main.theWidget->inform_sequencer_callback(hdrSeq->seq_step_fid);
				}

				if (
					(hdrSeq->seq_elm_tp == JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER ) &&
					(hdrSeq->seq_event_mask == JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP))
				{
					subWidgets.main.theWidget->inform_specific(JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_REPORT_SEQUENCER_UPDATE_WINDOW, 
						NULL, JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_NONE);
				}

				if (
					(hdrSeq->seq_event_mask == JVX_SEQUENCER_EVENT_REPORT_OPERATION_STATE) ||
					(hdrSeq->seq_event_mask == JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP) ||
					(hdrSeq->seq_event_mask == JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR))
				{
					jvxSize stateSeq = hdrSeq->seq_oper_state;
					subWidgets.main.theWidget->inform_specific(JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_REPORT_SEQUENCER_OPERATION_STATE, 
						&stateSeq, JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_TYPE_SIZE);
				}

				if (hdrSeq->seq_event_mask == JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR)
				{
					std::string errMsg = hdrSeq->seq_step_description;
					QMessageBox *reportBox = new QMessageBox(this);
					reportBox->setText("Error reported by remote sequencer:");
					reportBox->setInformativeText(errMsg.c_str());
					reportBox->show();
				}
				if (hdrSeq->seq_event_mask == JVX_SEQUENCER_EVENT_PROCESS_TERMINATED) 
				{
					jvxSize stateSeq = JVX_SIZE_UNSELECTED;
					subWidgets.main.theWidget->inform_specific(JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_REPORT_SEQUENCER_OPERATION_STATE, &stateSeq, JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_TYPE_SIZE);
					
					// Report this to application
					subWidgets.main.theWidget->inform_sequencer_stopped();
				}

				if (theWebCfg.ws.verbose_out)
				{
					std::cout << " -- Sequencer event [" << JVX_TIME() << "]" << std::flush;
					std::cout << " -- " << jvxSequencerEventType_txt(hdrSeq->seq_event_mask) << std::flush;
					std::cout << " -- " << hdrSeq->seq_step_description << std::endl;
				}
			
				if (theConnState == JVX_CONNECTION_STATE_OPERATE)
				{
					// This triggers a refresh of the syste status
					if (theWebCfg.ws.verbose_out)
					{
						std::cout << "Switching state to update" << std::endl;
					}
					theConnState = JVX_CONNECTION_STATE_UPDATE;
					request_status();
				}
				else
				{
					pendigSystemUpdates++;
				}
			}
			else if (
				((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER) &&
				((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PS_SEND_LINEARFIELD))
			{
				jvxSize i;
				jvxPropertyStreamHeader* shdr = (jvxPropertyStreamHeader*)payload;
				assert(szFld >= sizeof(jvxPropertyStreamHeader));

				std::map<jvxComponentIdentification, propertiesOneComponent>::iterator elm = webSockRuntime.mt.registeredProperties.begin();
				for (; elm != webSockRuntime.mt.registeredProperties.end(); elm++)
				{
					std::map<jvxSize, jvxOneRemotePropertySpecification>::iterator elmi = elm->second.registeredProperties.find(shdr->raw_header.stream_id);
					if (elmi != elm->second.registeredProperties.end())
					{
						if (theWebCfg.ws.reportData)
						{
							theWebCfg.ws.reportData->report_property_binary_data(elm->first, elmi->second.user_data, payload,
								JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO);
							
							

							// In case of flow control, send a confirmation about reception
							if (elmi->second.requires_flow_control)
							{
								if (shdr->requires_flow_response)
								{
									jvxRawStreamHeader_response prot_header;
									memset(&prot_header, 0, sizeof(jvxRawStreamHeader_response));

									szFld = sizeof(jvxRawStreamHeader_response);
									prot_header.raw_header.loc_header.paketsize = JVX_SIZE_UINT32(szFld);
									prot_header.raw_header.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
									prot_header.raw_header.loc_header.purpose = (JVX_PS_SEND_LINEARFIELD | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE);

									prot_header.raw_header.stream_id = shdr->raw_header.stream_id;
									prot_header.raw_header.packet_tstamp = shdr->raw_header.packet_tstamp;

									resL = httpRef->request_transfer_websocket(webSockRuntime.idWebSock, JVX_WEBSOCKET_OPCODE_BINARY, (const char*)&prot_header);
								}
								else
								{
									std::cout << __FUNCTION__ << ": Not sending a response, more data expected." << std::endl;
								}
							}
							break;
						}
					}
				}
			}
			else if (
				((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER) &&
				((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PS_SEND_INFORM_PROPERTY_CHANGE))
			{
				jvxComponentIdentification cpTp;
				jvxPropertyChangedHeader* shdr = (jvxPropertyChangedHeader*)payload;
				assert(szFld >= sizeof(jvxPropertyChangedHeader));

				cpTp.tp = (jvxComponentType)shdr->component_type;
				cpTp.slotid = shdr->component_slot;
				cpTp.slotsubid = shdr->component_subslot;
				subWidgets.main.theWidget->inform_internals_have_changed(cpTp, NULL, (jvxPropertyCategoryType)shdr->cat, 
					shdr->prop_id, false, JVX_COMPONENT_UNKNOWN, JVX_PROPERTY_CALL_PURPOSE_TAKE_OVER);
			}
			else
			{
				assert(0);
			}
		}
		else
		{
			assert(0);
		}
	}
	else
	{
		assert(0);
	}

	return JVX_NO_ERROR;
}

void
uMainWindow::slot_setup_websocket()
{
	jvxErrorType res = JVX_NO_ERROR;
	// Trigger websocket setup
	assert(webSockRuntime.stateWebSock == JVX_WEBSOCK_STATE_CONNECTED);
	webSockRuntime.stateWebSock = JVX_WEBSOCK_STATE_SETUP_PENDING;

	res = trigger_config_websocket(theWebCfg.ws.ticksUpdateWebSocket_msec, 
		theWebCfg.ws.numTicksUpdateWebSocket); // Period for websocket check: 1 sec, number of slots for ping: 10 (every 10 seconds)

}

jvxErrorType
uMainWindow::trigger_config_websocket(jvxSize ticks_base_cnt_msec, jvxSize ping_cnt)
{
	jvxSize szFld = 0;
	jvxPropertyConfigurePropertySend prot_header;
	//jvxByte* charFld;
	jvxErrorType res = JVX_NO_ERROR;

	memset(&prot_header, 0, sizeof(prot_header));
	prot_header.tick_msec = JVX_SIZE_INT32(ticks_base_cnt_msec);
	prot_header.ping_count = JVX_SIZE_INT16(ping_cnt);
	prot_header.user_specific = 0;
	prot_header.loc_header.paketsize = sizeof(prot_header);
	prot_header.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
	prot_header.loc_header.purpose = (JVX_PS_CONFIGURE_PROPERTY_OBSERVATION | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST);

	res = httpRef->request_transfer_websocket(webSockRuntime.idWebSock, JVX_WEBSOCKET_OPCODE_BINARY, (const char*)&prot_header);
	return res;
}

jvxErrorType 
uMainWindow::register_properties(jvxComponentIdentification tp, jvxOneRemotePropertySpecification* props, jvxSize numProps)
{
	jvxSize i;
	for (i = 0; i < numProps; i++)
	{
		propertiesOneComponent newProperties;
		propertiesOneComponent* newProperties_ptr = &newProperties;
		jvxBool addToMap = true;

		if (
			(tp.tp == JVX_COMPONENT_UNKNOWN) ||
			(tp == props[i].cpTp))
		{
			auto elm = webSockRuntime.mt.registeredProperties.find(props[i].cpTp);
			if (elm != webSockRuntime.mt.registeredProperties.end())
			{
				addToMap = false;
				newProperties_ptr = &elm->second;
			}

			/*
			auto elmp = newProperties_ptr->propertiesToRegister.begin();
			jvxBool idRegisteredBefore;
			for (; elmp != newProperties_ptr->propertiesToRegister.end(); elmp++)
			{
				elmp->second.
			}
			*/
			newProperties_ptr->propertiesToRegister[newProperties_ptr->count] = props[i];
			newProperties_ptr->propertiesToRegister[newProperties_ptr->count].id_user = i;
			newProperties_ptr->propertiesToRegister[newProperties_ptr->count].registeredOnOtherSide = false;
			newProperties_ptr->count++;

			if (addToMap)
			{
				JVX_LOCK_MUTEX(webSockRuntime.mt.safeAccess);
				webSockRuntime.mt.registeredProperties[props[i].cpTp] = newProperties;
				JVX_UNLOCK_MUTEX(webSockRuntime.mt.safeAccess);
			}
		}
	}

	trigger_setup_next_prop();

	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::unregister_properties(jvxComponentIdentification tp)
{
	return trigger_release_props(tp);
}

jvxErrorType
uMainWindow::trigger_setup_next_prop()
{
	jvxSize szFld = 0;
	jvxPropertyPropertyObserveHeader* prot_header = NULL;
	jvxByte* charFld;
	jvxErrorType res = JVX_NO_ERROR;
	assert(webSockRuntime.stateWebSock == JVX_WEBSOCK_STATE_READY);

	std::map<jvxComponentIdentification, propertiesOneComponent>::iterator elm = webSockRuntime.mt.registeredProperties.begin();
	for (; elm != webSockRuntime.mt.registeredProperties.end(); elm++)
	{
		std::map<jvxSize, jvxOneRemotePropertySpecification>::iterator elmi = elm->second.propertiesToRegister.begin();
		if (elmi != elm->second.propertiesToRegister.end())
		{
			szFld = sizeof(jvxPropertyPropertyObserveHeader) + elmi->second.nmProp.size();

			prot_header = (jvxPropertyPropertyObserveHeader*) new jvxByte[szFld];
			memset(prot_header, 0, szFld);
			prot_header->loc_header.paketsize = JVX_SIZE_UINT32(szFld);
			prot_header->loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
			prot_header->loc_header.purpose = (JVX_PS_ADD_PROPERTY_TO_OBSERVE | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST);

			prot_header->component_slot = JVX_SIZE_UINT16(elmi->second.cpTp.slotid);
			prot_header->component_subslot = JVX_SIZE_UINT16(elmi->second.cpTp.slotsubid);
			prot_header->component_type = elmi->second.cpTp.tp;

			prot_header->property_format = elmi->second.format;
			prot_header->property_num_elements = JVX_SIZE_UINT32(elmi->second.numElms);
			prot_header->property_offset = JVX_SIZE_UINT16(elmi->second.offset);
			prot_header->property_dec_hint_tp = elmi->second.htTp;

			prot_header->param0 = JVX_SIZE_INT32(elmi->second.param0);
			prot_header->param1 = JVX_SIZE_INT32(elmi->second.param1);
			prot_header->state_active = JVX_SIZE_INT32(elmi->second.state_in_which_active);

			prot_header->user_specific = JVX_SIZE_UINT32(elmi->second.id_user);

			prot_header->cond_update = JVX_SIZE_INT16((jvxSize)elmi->second.cond_update);
			prot_header->param_cond_update = JVX_SIZE_INT16((jvxSize)elmi->second.param_cond_update);

			prot_header->cnt_report_disconnect = JVX_SIZE_INT16((jvxSize)elmi->second.cnt_report_disconnect);
			prot_header->requires_flow_control = jvxUInt16(elmi->second.requires_flow_control);

			prot_header->priority = jvxUInt16(elmi->second.prio);
			prot_header->port = jvxUInt16(elmi->second.port);

			prot_header->num_emit_min = JVX_SIZE_UINT32(elmi->second.min_num_emit);
			prot_header->num_emit_max = JVX_SIZE_UINT32(elmi->second.max_num_emit);

			if (elmi->second.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
			{
				elmi->second.port_uid = high_prio_transfer.uId++;
				res = try_attach_udp_socket(elmi->second.port, elmi->second.port_uid);
				assert(res == JVX_NO_ERROR);
			}
			charFld = (jvxByte*)prot_header + sizeof(jvxPropertyPropertyObserveHeader);
			memcpy(charFld, elmi->second.nmProp.c_str(), elmi->second.nmProp.size());

			res = httpRef->request_transfer_websocket(webSockRuntime.idWebSock, JVX_WEBSOCKET_OPCODE_BINARY, (const char*)prot_header);

			// Next, delete the allocated field again:
			delete[]((jvxByte*)prot_header);

			break;
		}
		else
		{
			// If we are here, all properties have been registered!
			subWidgets.main.theWidget->inform_viewer_started();
		}
	}
	return res;
}

jvxErrorType
uMainWindow::trigger_release_props(jvxComponentIdentification tp)
{
	jvxSize szFld = 0;
	jvxPropertyPropertyObserveHeader prot_header;
	jvxByte* charFld;
	jvxErrorType res = JVX_NO_ERROR;

	if (tp.tp != JVX_COMPONENT_ALL_LIMIT)
	{
		if (tp.tp == JVX_COMPONENT_UNKNOWN)
		{
			webSockRuntime.tobereleasedProperties = webSockRuntime.mt.registeredProperties;
			webSockRuntime.mt.registeredProperties.clear();
		}
		else
		{
			std::map<jvxComponentIdentification, propertiesOneComponent>::iterator elm = webSockRuntime.mt.registeredProperties.begin();
			if (elm != webSockRuntime.mt.registeredProperties.end())
			{
				webSockRuntime.tobereleasedProperties[elm->first] = elm->second;
				JVX_LOCK_MUTEX(webSockRuntime.mt.safeAccess);
				webSockRuntime.mt.registeredProperties.erase(elm);
				JVX_UNLOCK_MUTEX(webSockRuntime.mt.safeAccess);
			}
			else
			{
				return JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
	}

	// Moved registered properties to another list to be removed
	std::map<jvxComponentIdentification, propertiesOneComponent>::iterator elm = webSockRuntime.tobereleasedProperties.begin();
	if (elm != webSockRuntime.tobereleasedProperties.end())
	{
		memset(&prot_header, 0, sizeof(jvxPropertyPropertyObserveHeader));

		szFld = sizeof(jvxPropertyPropertyObserveHeader);
		prot_header.loc_header.paketsize = JVX_SIZE_UINT32(szFld);
		prot_header.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
		prot_header.loc_header.purpose = (JVX_PS_REMOVE_PROPERTY_TO_OBSERVE | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST);

		prot_header.component_slot = JVX_SIZE_UINT16(elm->first.slotid);
		prot_header.component_subslot = JVX_SIZE_UINT16(elm->first.slotsubid);
		prot_header.component_type = elm->first.tp;

		res = httpRef->request_transfer_websocket(webSockRuntime.idWebSock, JVX_WEBSOCKET_OPCODE_BINARY, (const char*)&prot_header);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

void
uMainWindow::request_status()
{
	// Directly re-emit the status update request  and avoid any intermediate stuff
	std::string initCommand = "/jvx/host/system/status?purpose=compact";
	httpRef->request_transfer_get(initCommand, uIdGroup, JVX_SIZE_UNSELECTED, static_cast<IjvxHttpApi_httpreport*>(this), NULL);
	requestedGets[uIdGroup] = initCommand;
	uIdGroup++;
}

void
uMainWindow::update_heartbeat()
{
	switch (heartbeatState)
	{
	case 0:
		label_heartbeat->setPixmap(QPixmap(QString::fromUtf8(":/images/images/alive_0.png")));
		break;
	case 1:
		label_heartbeat->setPixmap(QPixmap(QString::fromUtf8(":/images/images/alive_1.png")));
		break;
	}
}

jvxErrorType
uMainWindow::try_attach_udp_socket(jvxInt32 port, jvxSize uniqueId_property)
{
	// Create a UDP socket and open!
	auto elm = high_prio_transfer.theUdpSockets.find(port);
	if (elm == high_prio_transfer.theUdpSockets.end())
	{
		// Allocate new socket
		HjvxPropertyStreamUdpSocket* theNewUdpSocket = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(theNewUdpSocket, HjvxPropertyStreamUdpSocket);
		theNewUdpSocket->initialize(static_cast<HjvxPropertyStreamUdpSocket_report*>(this),
			involvedComponents.theTools.hTools, involvedComponents.theHost.hFHost, false);
		theNewUdpSocket->register_property(uniqueId_property, port);
		high_prio_transfer.theUdpSockets[port] = theNewUdpSocket;
	}
	else
	{
		(elm->second)->register_property(uniqueId_property, port);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::try_detach_udp_socket(jvxInt32 port, jvxSize uniqueId_property)
{
#ifdef JVX_WEBCONTROL_UDP_VERBOSE
    std::cout << __FUNCTION__ << ": Entering with id = " << uniqueId_property << "." << std::endl;
#endif

    auto elm = high_prio_transfer.theUdpSockets.find(port);
	if (elm != high_prio_transfer.theUdpSockets.end())
	{
		jvxInt32 port = 0;
		elm->second->unregister_property(uniqueId_property, port);
		if (port != 0)
		{
			HjvxPropertyStreamUdpSocket* to_remove = elm->second;
			high_prio_transfer.theUdpSockets.erase(elm);
			to_remove->terminate();
			JVX_DSP_SAFE_DELETE_OBJECT(to_remove);
		}
	}
	else
	{
		assert(0);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::report_incoming_packet(jvxByte* fld, jvxSize sz, HjvxPropertyStreamUdpSocket* resp_socket)
{
	jvxProtocolHeader* hdr = (jvxProtocolHeader*)fld;

	assert(sz >= sizeof(jvxProtocolHeader));

	if (hdr->fam_hdr.proto_family == JVX_PROTOCOL_TYPE_PROPERTY_STREAM)
	{
		if (
			((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK) == JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER) &&
			((hdr->purpose & JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK) == JVX_PS_SEND_LINEARFIELD))
		{
			jvxSize i;
			jvxPropertyStreamHeader* shdr = (jvxPropertyStreamHeader*)fld;
			assert(sz >= sizeof(jvxPropertyStreamHeader));

			jvxBool foundProperty = false;
			JVX_LOCK_MUTEX(webSockRuntime.mt.safeAccess);
			std::map<jvxComponentIdentification, propertiesOneComponent>::iterator elm = webSockRuntime.mt.registeredProperties.begin();
			for (; elm != webSockRuntime.mt.registeredProperties.end(); elm++)
			{
				std::map<jvxSize, jvxOneRemotePropertySpecification>::iterator elmi = elm->second.registeredProperties.find(shdr->raw_header.stream_id);
				if (elmi != elm->second.registeredProperties.end())
				{
					foundProperty = true;
					if (theWebCfg.ws.reportData)
					{
						theWebCfg.ws.reportData->report_property_binary_data(elm->first, elmi->second.user_data, fld, JVX_PROP_CONNECTION_TYPE_HIGH_PRIO);

						// In case of flow control, send a confirmation about reception
						if (elmi->second.requires_flow_control)
						{
							if (shdr->requires_flow_response)
							{
								jvxRawStreamHeader_response prot_header;
								memset(&prot_header, 0, sizeof(jvxRawStreamHeader_response));

								jvxSize szFld = sizeof(jvxRawStreamHeader_response);
								prot_header.raw_header.loc_header.paketsize = JVX_SIZE_UINT32(szFld);
								prot_header.raw_header.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
								prot_header.raw_header.loc_header.purpose = (JVX_PS_SEND_LINEARFIELD | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE);

								prot_header.raw_header.stream_id = shdr->raw_header.stream_id;
								prot_header.raw_header.packet_tstamp = shdr->raw_header.packet_tstamp;

								// The default behavior is to sed back to where we got the trigger from
								jvxErrorType resL = resp_socket->send_packet((const char*)&prot_header, szFld);
							}
						}
						break;
					}
				}
			}
			JVX_UNLOCK_MUTEX(webSockRuntime.mt.safeAccess);

			if (!foundProperty)
			{
				std::cout << "Incoming message reported for stream id <" << shdr->raw_header.stream_id << ">." << std::endl;
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::report_connection_error(const char* errorText)
{
	return JVX_NO_ERROR;
}

// =============================================================================
jvxErrorType
jvxSyncRemoteHttp::initialize(
	mainWindow_UiExtension_hostfactory* theMainWidgetL, 
	IjvxHttpApi* httpRefL,
	IjvxFactoryHost* hFHostRef)
{
	if (myState == JVX_STATE_NONE)
	{
		theMainWidget = theMainWidgetL;
		httpRef = httpRefL;
		myState = JVX_STATE_INIT;
		hFHost = hFHostRef;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxSyncRemoteHttp::terminate()
{
	if (myState == JVX_STATE_INIT)
	{
		
		sync_out();
		theMainWidget = NULL;
		httpRef = NULL;
		myState = JVX_STATE_NONE;
		hFHost = nullptr;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
jvxSyncRemoteHttp::sync_remote_disconnect()
{
	jvxBool system_state_switch = false;
	std::map< jvxComponentIdentification, onePartnerComponent> receivedList;
	std::map< jvxComponentIdentification, onePartnerComponent>::iterator elm = registeredPartners.begin();
	for (; elm != registeredPartners.end(); elm++)
	{
		elm->second.temp_flag = false;
		elm->second.toState = JVX_STATE_NONE;
	}
	return match_new_old_lists(receivedList, system_state_switch);
}

jvxErrorType 
jvxSyncRemoteHttp::sync_remote(CjvxJsonElementList& lst, 
	jvxBool& is_system_state_transition,
	jvxBool& is_state_transition_process)
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize i;

	jvxSize num;
	CjvxJsonElement* jsn_elm_sys = NULL;
	CjvxJsonElementList* jsn_elm_sysl = NULL;
	CjvxJsonElement* jsn_elm = NULL;
	CjvxJsonElementList* jsn_elm_lst = NULL;
	CjvxJsonElement* jsn_elm_lst_elm = NULL;
	CjvxJsonArray* jsn_arr = NULL;
	CjvxJsonArrayElement* jsn_arr_elm = NULL;
	jvxApiString fldStr;
	jvxSize stat = JVX_STATE_NONE;
	std::map< jvxComponentIdentification, onePartnerComponent> receivedList;

	std::map< jvxComponentIdentification, onePartnerComponent>::iterator elm = registeredPartners.begin();
	for (; elm != registeredPartners.end(); elm++)
	{
		elm->second.temp_flag = false;
		elm->second.toState = JVX_STATE_NONE;
	}

	jvxErrorType resR = JVX_NO_ERROR;
	jvxBool err = false;
	lst.reference_element(&jsn_elm, "return_code", CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
	if (jsn_elm)
	{
		resL = jsn_elm->reference_string(fldStr);
		if (resL == JVX_NO_ERROR)
		{
			resR = jvxErrorType_decode(fldStr.c_str());
		}
		else
		{
			err = true;
		}
	}
	else
	{
		err = true;
	}

	if (err)
	{
		std::cout << "Invalid return value from system." << std::endl;
	}

	if (resR != JVX_NO_ERROR)
	{
		std::cout << "Web responded error in call." << std::endl;
	}
	else
	{
		lst.reference_element(&jsn_elm_sys, "system", CjvxJsonElement::JVX_JSON_ASSIGNMENT_SECTION);
		if(jsn_elm_sys)
		{
			// selected part
			jsn_elm_sys->reference_section(&jsn_elm_sysl);
			if(jsn_elm_sysl)
			{
				jsn_elm_sysl->reference_element(&jsn_elm, "selected", CjvxJsonElement::JVX_JSON_ASSIGNMENT_ARRAY);
				
				if (jsn_elm)
				{
					jsn_elm->reference_array(&jsn_arr);
					if (jsn_arr)
					{
						jsn_arr->numElements(&num);
						for (i = 0; i < num; i++)
						{
							jvxComponentIdentification tp;
							jvxSize uId;
							std::string description;
							std::string descriptor;

							jsn_arr->reference_elementAt(i, &jsn_arr_elm);
							if (jsn_arr_elm)
							{
								jsn_arr_elm->reference_section(&jsn_elm_lst);
								if (jsn_elm_lst)
								{
									jsn_elm_lst->reference_element(&jsn_elm_lst_elm, "descriptor", CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
									if (jsn_elm_lst_elm)
									{
										resL = jsn_elm_lst_elm->reference_string(fldStr);
										if (resL == JVX_NO_ERROR)
										{
											descriptor = fldStr.std_str();
										}
										else
										{
											err = true;
										}
									} // if (jsn_elm_lst_elm)
									else
									{
										err = true;
									}

									jsn_elm_lst->reference_element(&jsn_elm_lst_elm, "description", CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
									if (jsn_elm_lst_elm)
									{
										resL = jsn_elm_lst_elm->reference_string(fldStr);
										if (resL == JVX_NO_ERROR)
										{
											description = fldStr.std_str();
										}
										else
										{
											err = true;
										}
									} // if (jsn_elm_lst_elm)
									else
									{
										err = true;
									}

									jsn_elm_lst->reference_element(&jsn_elm_lst_elm, "component_identification", CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
									if (jsn_elm_lst_elm)
									{
										resL = jsn_elm_lst_elm->reference_string(fldStr);
										if (resL == JVX_NO_ERROR)
										{
											resL = jvxComponentIdentification_decode(tp, fldStr.std_str());
											if (resL != JVX_NO_ERROR)
											{
												err = true;
											}
										}
										else
										{
											err = true;
										}
									} // if (jsn_elm_lst_elm)
									else
									{
										err = true;
									}

									jsn_elm_lst->reference_element(&jsn_elm_lst_elm, "state", CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
									if (jsn_elm_lst_elm)
									{
										resL = jsn_elm_lst_elm->reference_string(fldStr);
										if (resL == JVX_NO_ERROR)
										{
											stat = jvxState_decode(fldStr.std_str(), err);
										}
										else
										{
											err = true;
										}
									} // if (jsn_elm_lst_elm)
									else
									{
										err = true;
									}

									jsn_elm_lst->reference_element(&jsn_elm_lst_elm, "uid", CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
									if (jsn_elm_lst_elm)
									{
										resL = jsn_elm_lst_elm->reference_string(fldStr);
										if (resL == JVX_NO_ERROR)
										{
											uId = jvx_string2Size(fldStr.std_str(), err);
										}
										else
										{
											err = true;
										}
									} // if (jsn_elm_lst_elm)
									else
									{
										err = true;
									}

									if (err == false)
									{
										onePartnerComponent newComp;
										newComp.description = description;
										newComp.descriptor = descriptor;
										newComp.temp_flag = false;
										newComp.theHttpControl = NULL;
										newComp.tp = tp;
										newComp.uId = uId;
										newComp.fromState = JVX_STATE_NONE;
										newComp.toState = (jvxState)stat;
										receivedList[tp] = newComp;
									}
									else
									{
										break;
									}
								} // if (jsn_elm_lst)
								else
								{
									err = true;
								}
							} // if (jsn_arr_elm)
							else
							{
								err = true;
							}
						}// for (i = 0; i < num; i++)

					} // if (jsn_arr)
					else
					{
						err = true;
					}

					jsn_elm = NULL;
					jsn_elm_sysl->reference_element(&jsn_elm, "sequencer", CjvxJsonElement::JVX_JSON_ASSIGNMENT_SECTION);
					if (jsn_elm)
					{
						jsn_elm_lst = NULL;
						jsn_elm->reference_section(&jsn_elm_lst);
						if (jsn_elm_lst)
						{
							jsn_elm_lst_elm = NULL;
							jsn_elm_lst->reference_element(&jsn_elm_lst_elm, "seq_status", CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
							if (jsn_elm_lst_elm)
							{
								jsn_elm_lst_elm->reference_string(fldStr);
								jvxSequencerStatus seqStatNew = jvxSequencerStatus_decode(fldStr.c_str());	
								if (seqStat != seqStatNew)
								{
									is_state_transition_process = true;
								}
								seqStat = seqStatNew;
							}
							else
							{
								err = true;
							}
						}
						else
						{
							err = true;
						}
					}
					else
					{
						err = true;
					}
				} // if (jsn_elm)
				else
				{
					err = true;
				}
			} // if(jsn_elm_sysl)
			else
			{
				err = true;
			}
		} // if(jsn_elm_sys)
		else
		{
			err = true;
		}
	} // else: if (resR != JVX_NO_ERROR)

	return match_new_old_lists(receivedList, is_system_state_transition);
}

jvxErrorType
jvxSyncRemoteHttp::sync_out()
{
	jvxBool system_state_switch = false;
	std::map< jvxComponentIdentification, onePartnerComponent> emptyLst; 
	std::map< jvxComponentIdentification, onePartnerComponent>::iterator elm = registeredPartners.begin();
	for (; elm != registeredPartners.end(); elm++)
	{
		elm->second.temp_flag = false;
		elm->second.toState = JVX_STATE_NONE;
	}
	return match_new_old_lists(emptyLst, system_state_switch);
}

jvxErrorType 
jvxSyncRemoteHttp::version_to_tag(CjvxJsonElementList& lst, std::string &version_tag)
{
	jvxSize num;
	CjvxJsonElement* jsn_elm = NULL;
	CjvxJsonElement* jsn_elm_elm = NULL;
	CjvxJsonElementList* jsn_elm_lst = NULL;
	jvxApiString fldStr;
	jvxErrorType resL = JVX_NO_ERROR;

	jvxErrorType resR = JVX_NO_ERROR;
	jvxBool err = false;
	std::string version_tag_loc;

	version_tag = "IAMBROKEN!";

	lst.reference_element(&jsn_elm, "return_code", CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
	if (jsn_elm)
	{
		resL = jsn_elm->reference_string(fldStr);
		if (resL == JVX_NO_ERROR)
		{
			resR = jvxErrorType_decode(fldStr.c_str());
		}
		else
		{
			err = true;
		}
	}
	else
	{
		err = true;
	}

	if (err)
	{
		std::cout << "Invalid return value from system." << std::endl;
	}

	if (resR != JVX_NO_ERROR)
	{
		std::cout << "Web responded error in call." << std::endl;
	}
	else
	{
		jvxBool errDetected = false;
		lst.reference_element(&jsn_elm, "version", CjvxJsonElement::JVX_JSON_ASSIGNMENT_SECTION);
		if (jsn_elm)
		{
			jsn_elm->reference_section(&jsn_elm_lst);
			if (jsn_elm_lst)
			{
				jsn_elm_lst->reference_element(&jsn_elm_elm, "me_tag", CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
				if (jsn_elm_elm)
				{
					jsn_elm_elm->reference_string(fldStr);
					version_tag_loc += fldStr.std_str();
				}
				else
				{
					errDetected = true;
				}

				jsn_elm_lst->reference_element(&jsn_elm_elm, "git_tag", CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
				if (jsn_elm_elm)
				{
					jsn_elm_elm->reference_string(fldStr);
					version_tag_loc += " -- ";
					version_tag_loc += fldStr.std_str();
				}
				else
				{
					errDetected = true;
				}

				jsn_elm_lst->reference_element(&jsn_elm_elm, "date_tag", CjvxJsonElement::JVX_JSON_ASSIGNMENT_STRING);
				if (jsn_elm_elm)
				{
					jsn_elm_elm->reference_string(fldStr);
					version_tag_loc += " -- ";
					version_tag_loc += fldStr.std_str();
				}
				else
				{
					errDetected = true;
				}
			}
			else
			{
				errDetected = true;
			}

			version_tag = "X";
		}
	
		if (!errDetected)
		{
			version_tag = version_tag_loc;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvxSyncRemoteHttp::match_new_old_lists(
	std::map< jvxComponentIdentification, onePartnerComponent>& receivedList,
	jvxBool& is_system_state_transition)
{
	int ii;

	is_system_state_transition = false;

	// Match the two
	std::map< jvxComponentIdentification, onePartnerComponent> actList;
	std::map< jvxComponentIdentification, onePartnerComponent>::iterator elm = receivedList.begin();
	for (; elm != receivedList.end(); elm++)
	{
		jvxBool foundit = false;
		std::map< jvxComponentIdentification, onePartnerComponent>::iterator elmcp = registeredPartners.find(elm->second.tp);
		if (elmcp != registeredPartners.end())
		{
			if (
				(elmcp->second.description == elm->second.description) &&
				(elmcp->second.descriptor == elm->second.descriptor) &&
				(elmcp->second.descriptor == elm->second.descriptor))
			{
				elmcp->second.temp_flag = true;
				elmcp->second.toState = elm->second.toState;
				foundit = true;
			}
		}
		if (!foundit)
		{
			onePartnerComponent copyMe = elm->second;
			copyMe.removeMe = false;
			actList[elm->second.tp] = copyMe;
		}
	}

	std::map< jvxComponentIdentification, onePartnerComponent> registeredPartners_copy;

	// Switch state down logic --
	for (ii = JVXSTATE_NUM; ii > 0; ii--)
	{
		elm = registeredPartners.begin();
		for (; elm != registeredPartners.end(); elm++)
		{
			if (elm->second.fromState == (1 << (ii-1)))
			{
				if (elm->second.toState < elm->second.fromState)
				{
					is_system_state_transition = true;
					switch (elm->second.fromState)
					{
					case JVX_STATE_ACTIVE:
						if (elm->second.theHttpControl)
						{
							theMainWidget->inform_inactive(elm->second.tp,
								static_cast<IjvxAccessProperties*>(elm->second.theHttpControl));
						}
						break;
					default:
						break;
					}
					elm->second.fromState = (jvxState)(1 << (ii - 2));

					if (elm->second.fromState == JVX_STATE_INIT)
					{
						if (elm->second.theHttpControl)
						{
							elm->second.theHttpControl->terminate();
							delete(elm->second.theHttpControl);
						}
						elm->second.theHttpControl = NULL;
						elm->second.removeMe = true;
					}
				}
			}
		} // for (; elm != registeredPartners.end(); elm++)
	}


	// Switch state up
	for (ii = 2; ii < JVXSTATE_NUM; ii++)
	{
		elm = registeredPartners.begin();
		for (; elm != registeredPartners.end(); elm++)
		{
			if (elm->second.fromState == (1 << (ii - 1)))
			{
				if (elm->second.toState > elm->second.fromState)
				{
					is_system_state_transition = true;
					switch (elm->second.fromState)
					{
					case JVX_STATE_SELECTED:
						if (elm->second.theHttpControl)
						{
							theMainWidget->inform_active(elm->second.tp,
								static_cast<IjvxAccessProperties*>(elm->second.theHttpControl));
						}
						break;
					default:
						break;
					}
					elm->second.fromState = (jvxState)(1 << (ii));

					if (elm->second.fromState == JVX_STATE_INIT)
					{
						if (elm->second.theHttpControl)
						{
							elm->second.theHttpControl->terminate();
							delete(elm->second.theHttpControl);
						}
						elm->second.theHttpControl = NULL;
						elm->second.removeMe = true;
					}
				}
			}
		} // for (; elm != registeredPartners.end(); elm++)
	}


	// Use those instances which are still active
	elm = registeredPartners.begin();
	for (; elm != registeredPartners.end(); elm++)
	{
		if(!elm->second.removeMe)
		// if (elm->second.theHttpControl)
		{
			registeredPartners_copy[elm->second.tp] = elm->second;
		}
	}

	// Switch up state for all new components
	for (ii = 1; ii < JVXSTATE_NUM; ii++)
	{
		jvxState stat_step = (jvxState)(1 << (ii - 1));
		elm = actList.begin();
		for (; elm != actList.end(); elm++)
		{
			is_system_state_transition = true;

			if (elm->second.toState > elm->second.fromState)
			{
				if (elm->second.fromState == JVX_STATE_NONE)
				{
					elm->second.theHttpControl = new CjvxAccessProperties_Qhttp();					
					elm->second.theHttpControl->initialize(
						httpRef, elm->second.tp, elm->second.description.c_str(), 
						elm->second.descriptor.c_str(), elm->second.tag.c_str(),
						hFHost);
				}
				if (elm->second.fromState == stat_step)
				{
					if (
						(elm->second.fromState == JVX_STATE_SELECTED) &&
						(elm->second.toState >= JVX_STATE_ACTIVE))
					{
						if (elm->second.theHttpControl)
						{
							theMainWidget->inform_active(elm->second.tp,
								static_cast<IjvxAccessProperties*>(elm->second.theHttpControl));
						}
					}
				}
				elm->second.fromState = (jvxState)(1 << (ii));
			}
		} // for (; elm != registeredPartners.end(); elm++)
	}
	elm = actList.begin();
	for (; elm != actList.end(); elm++)
	{
		registeredPartners_copy[elm->second.tp] = elm->second;
	}

	// Replace old by new list 
	registeredPartners = registeredPartners_copy;
	return JVX_NO_ERROR;
}

void
uMainWindow::stop_timer_viewer()
{
	runTimer->stop();
	label_heartbeat->setPixmap(QPixmap(QString::fromUtf8(":/images/images/not_alive.png")));
}
