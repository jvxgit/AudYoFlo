#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSplashScreen>
#include "jvx-qt-helpers.h"

#include "uMainWindow.h"
#include <iostream>


// Includes for main host application
#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
#include "jvxHAppHost_nd.h"
#else
#include "jvxHAppHost.h"
#endif

#include "jvxQtAudioHostHookupEntries.h"

JVX_DECLARE_QT_METATYPES

// ##################################################################################
// ##################################################################################
#ifndef JVX_QT_SPLASH_TEXT_COLOR 
#define JVX_QT_SPLASH_TEXT_COLOR QColor(0, 176, 240)
#endif

/*
 * Initialize the overall system
 *///===============================================================================
jvxErrorType
uMainWindow::initSystem(QApplication* hdlApp, char* clStr[], int clNum)
{
	jvxConfigSectionTypes myCfgTp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	jvxApiString errMess;
	jvxErrorType res = JVX_NO_ERROR;
	QSplashScreen* spl = NULL;
	
#ifndef JVX_SPLASH_SUPPRESSED
	spl = jvx_start_splash_qt(theHostFeatures.config_ui.scalefac_splash, theHostFeatures.config_ui.pixmap_splash, 
		theHostFeatures.config_ui.ftsize_splash, theHostFeatures.config_ui.italic_splash);
#endif

	IjvxConfigurationExtender* cfgExt = NULL;

	std::string exec = jvx_extractFileFromFilePath(clStr[0]);
	commLine.assign_args((const char**)&clStr[1], clNum-1, exec.c_str());

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		spl->showMessage("Starting audio host ...", Qt::AlignHCenter, JVX_QT_SPLASH_TEXT_COLOR);
	}
	qApp->processEvents();
#endif

	// Setup the UI elements
	setupUi(this);

	layout()->setContentsMargins(0,0,0,0);

	JVX_REGISTER_QT_METATYPES

	this->setWindowTitle(JVX_DATA_STRING "--" JVX_VERSION_STRING);
	
	// Pass by reference to subclass
	init_specific(this);
	theControl.init(static_cast<IjvxMainWindowControl_connect*>(this), menuConfiguration, menuApplication, this);

	// signals Connect for different purposes
	connect(this, SIGNAL(emit_postMessage(QString, jvxReportPriority)), this, SLOT(postMessage_inThread(QString, jvxReportPriority)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_bootDelayed()), this, SLOT(bootDelayed()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_report_internals_have_changed_inThread(jvxComponentIdentification, IjvxObject*, jvxPropertyCategoryType, jvxSize, bool, jvxComponentIdentification, jvxPropertyCallPurpose)), this,
		SLOT(report_internals_have_changed_inThread(jvxComponentIdentification, IjvxObject*, jvxPropertyCategoryType, jvxSize, bool, jvxComponentIdentification, jvxPropertyCallPurpose)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_report_command_request(jvxCBitField, jvxHandle* , jvxSize)), this, 
		SLOT(report_command_request_inThread(jvxCBitField, jvxHandle* , jvxSize)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_request_command(CjvxReportCommandRequest*)), this,
		SLOT(request_command_inThread(CjvxReportCommandRequest*)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_close_app()), this, SLOT(close_app()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_autoStart()), this, SLOT(buttonPushed_startstop()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_reopen_config()), this, SLOT(reopen_config()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_autoStop()), this, SLOT(run_autostop()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_saveConfigFile()), this, SLOT(saveConfigFile()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_openConfigFile()), this, SLOT(openConfigFile()), Qt::QueuedConnection);

	connect(this, SIGNAL(emit_triggerTestChain()), this, SLOT(trigger_test_chain_inThread()), Qt::QueuedConnection);
	
	// All project specific connections in UI
	connect_specific();

	/*
	 * ==================================================================================================
     * ==================================================================================================
     * ==================================================================================================
     * ==================================================================================================
     * ==================================================================================================
    */

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		spl->showMessage("Configuring audio host ...", Qt::AlignHCenter, JVX_QT_SPLASH_TEXT_COLOR);
	}
	qApp->processEvents();
#endif
	res = this->boot_configure(&errMess, static_cast<IjvxCommandLine*>(&commLine), static_cast<IjvxReport*>(this), 
		static_cast<IjvxReportOnConfig*>(this), static_cast<IjvxReportStateSwitch*>(this), NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		spl->showMessage("Initializing audio host ...", Qt::AlignHCenter, JVX_QT_SPLASH_TEXT_COLOR);
	}
	qApp->processEvents();	
#endif

	/*
	QSize szz = this->minimumSize();
	std::cout << "1) Min: " << szz.width() << ":" << szz.height() << std::endl;
	*/

	res = boot_initialize(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		spl->showMessage("Activating audio host ...", Qt::AlignHCenter, JVX_QT_SPLASH_TEXT_COLOR);
	}
	qApp->processEvents();
#endif

	/*
	szz = this->minimumSize();
	std::cout << "2) Min: " << szz.width() << ":" << szz.height() << std::endl;
	*/

	res = boot_activate(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		spl->showMessage("Preparing audio host ...", Qt::AlignHCenter, JVX_QT_SPLASH_TEXT_COLOR);
		qApp->processEvents();
	}
#endif
	res = boot_prepare(&errMess, spl);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

	jvxValue val;
	theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_INT_VALUE,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Timeout Period Sequencer [ms]",
		&id_period_seq_ms,
		false, JVX_REGISTER_FUNCTION_MENU_SEPARATOR_IF_NOT_FIRST);

	theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_INT_VALUE,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Timeout Period Main Central [ms]",
		&id_rtview_period_maincentral_ms,
		false, JVX_REGISTER_FUNCTION_MENU_NO_SEPARATOR);

	val.assign(genQtAudioHost::host_params.autostart.value);
	theControl.register_functional_callback(
		JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_ONOFF,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Auto Start",
		&id_autostart_id,
		false, JVX_REGISTER_FUNCTION_MENU_SEPARATOR_IF_NOT_FIRST,
		&val);

	val.assign(genQtAudioHost::host_params.autostop.value);
	theControl.register_functional_callback(
		JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_ONOFF,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Auto Stop",
		&id_autostop_id,
		false, JVX_REGISTER_FUNCTION_MENU_NO_SEPARATOR,
		&val);
	
	/*
	szz = this->minimumSize();
	std::cout << "3) Min: " << szz.width() << ":" << szz.height() << std::endl;
	*/

	// All phases passed, staring right away UI elements */

	// Update ui
	updateWindow((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);

	// Trigger boot Delayed callback (to leave constructor)
	emit emit_bootDelayed();

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		jvx_stop_splash_qt(spl, this);
	}
#endif
	return(JVX_NO_ERROR);

// ##################################################################################
}


// ##################################################################################

/*
 * Callback to do some operations after bootup.
 *///======================================================================================
void
uMainWindow::bootDelayed()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errMess;

	/*
	horizontalSpacer_left->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
	horizontalSpacer_right->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
	verticalSpacer_top->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
	verticalSpacer_bottom->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
	*/

	/*
	QSize szz = this->minimumSize();
	std::cout << "4) Min: " << szz.width() << ":" << szz.height() << std::endl;
	*/
	boot_start(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}
}

// =============================================================================================

void
uMainWindow::setup_widgets()
{
	// Start audio dialog window - even if no content can be shown yet
	subWidgets.theAudioDialog = new configureAudio(this, tpAll);
	subWidgets.theAudioArgs = new additionalArgsWidget(this, tpAll);

	// Now that host is there, initialize the specific sub widgets
	//subWidgets.theConnectionsFrame = new jvx_widget_in_frame_dialog(this);
	init_jvxQtWidgetFrameDialog(&subWidgets.theConnectionsFrame, 0);
	subWidgets.theConnectionsFrame->init(static_cast<IjvxQtSpecificWidget_report*>(this));
	subWidgets.theConnectionsFrame->getMyQDialog(&subWidgets.theConnectionsFrameDialog, 0);
	init_jvxQtConnectionWidget(&subWidgets.theConnectionsWidget, subWidgets.theConnectionsFrameDialog);
	subWidgets.theConnectionsWidget->init(involvedHost.hHost, 0, NULL, static_cast<IjvxQtSpecificWidget_report*>(this));
	subWidgets.theConnectionsQWidget = NULL;
	subWidgets.theConnectionsWidget->getMyQWidget(&subWidgets.theConnectionsQWidget, 0);
	subWidgets.theConnectionsFrame->attachWidget(subWidgets.theConnectionsQWidget);
	//subWidgets.theConnectionsFrame->show(); //<- show this later!!

	// Now that host is there, initialize the specific sub widgets
	//subWidgets.theConnectionsFrame = new jvx_widget_in_frame_dialog(this);
	init_jvxQtWidgetFrameDialog(&subWidgets.theSequencerFrame, 0);
	subWidgets.theSequencerFrame->init(static_cast<IjvxQtSpecificWidget_report*>(this));
	subWidgets.theSequencerFrame->getMyQDialog(&subWidgets.theSequencerFrameDialog, 0);
	init_jvxQtSequencerWidget(&subWidgets.theSequencerWidget, subWidgets.theSequencerFrameDialog);
	subWidgets.theSequencerWidget->init(involvedHost.hHost, 0, NULL, static_cast<IjvxQtSpecificWidget_report*>(this));
	subWidgets.theSequencerQWidget = NULL;
	subWidgets.theSequencerWidget->getMyQWidget(&subWidgets.theSequencerQWidget, 0);
	subWidgets.theSequencerFrame->attachWidget(subWidgets.theSequencerQWidget);

	// ===============================================================================================

	init_jvxQtMenuComponentsBridge(&subWidgets.bridgeMenuComponents, this);
	subWidgets.bridgeMenuComponents->createMenues(&theHostFeatures,
		involvedHost.hHost,
		involvedComponents.theTools.hTools,
		menuComponents, tpAll, 
		&systemParams.skipStateSelected,
		this);

	// ===============================================================================================

	theControl.register_action_widget(JVX_DESCRIPTION_CONNECTION_WIDGET, subWidgets.theConnectionsWidget, subWidgets.theConnectionsFrame,
		JVX_REGISTER_FUNCTION_MENU_SEPARATOR_IF_NOT_FIRST);
	theControl.register_action_widget(JVX_DESCRIPTION_SEQUENCER_WIDGET, subWidgets.theSequencerWidget, subWidgets.theSequencerFrame,
		JVX_REGISTER_FUNCTION_MENU_NO_SEPARATOR);

	if (subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->init_extend_specific_widgets(static_cast<IjvxMainWindowControl*>(&theControl), static_cast<IjvxQtSpecificWidget_report*>(this));
	}
}

void
uMainWindow::finalize_widgets()
{
	subWidgets.theAudioDialog->init();
	subWidgets.theAudioDialog->hide();
	/*
	QRect rr = subWidgets.theAudioDialog->geometry();
	rr.setWidth(0);
	rr.setHeight(0);
	subWidgets.theAudioDialog->setGeometry(rr);
	*/

	subWidgets.theAudioArgs->init();
	subWidgets.theAudioArgs->hide();

	theControl.activate();
}

void
uMainWindow::postbootup_widgets()
{
	subWidgets.theSequencerWidget->set_control_ref(static_cast<IjvxMainWindowControl*>(&theControl), static_cast<IjvxQtSequencerWidget_report*>(this));
	subWidgets.theSequencerWidget->update_window_rebuild();

	if (theHostFeatures.viewerUpdateAlways)
	{
		subWidgets.theAudioDialog->pre_allow_timer();
	}
}

// ##################################################################################
// ##################################################################################

jvxErrorType 
uMainWindow::get_variable_edit(jvxHandle* privData, jvxValue& val, jvxSize id)
{
	if (id == id_period_seq_ms)
	{
		val.assign(theHostFeatures.timeout_period_seq_ms);
	}
	if (id == id_rtview_period_maincentral_ms)
	{
		val.assign(theHostFeatures.timeout_viewer_maincentral_ms);
	}
	if (id == id_autostart_id)
	{
		val.assign(genQtAudioHost::host_params.autostart.value);
	}
	if (id == id_autostop_id)
	{
		val.assign(genQtAudioHost::host_params.autostop.value);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::set_variable_edit(jvxHandle* privData, jvxValue& val, jvxSize id)
{
	jvxBool runUpdate = false;
	if (id == id_period_seq_ms)
	{
		val.toContent(&theHostFeatures.timeout_period_seq_ms);
	}
	if (id == id_rtview_period_maincentral_ms)
	{
		val.toContent(&theHostFeatures.timeout_viewer_maincentral_ms);
	}
	if (id == id_autostart_id)
	{
		runUpdate = true;
		val.toContent(&genQtAudioHost::host_params.autostart.value);
	}
	if (id == id_autostop_id)
	{
		runUpdate = true;
		val.toContent(&genQtAudioHost::host_params.autostop.value);
	}

	if (runUpdate)
	{
		this->updateWindow();
	}

	return JVX_NO_ERROR;
}
