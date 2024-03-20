#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QSplashScreen>

//#define DEFAULT_NAME_CONFIG_FILE ".session.rtproc"
//#define POSTFIX_DONT_LOAD_CONFIG "--no-config"

#include "uMainWindow.h"
#include "uMainWindow_widgets.h"
#include <iostream>
// Includes for main host application

#include "jvxQtHostHookEntries.h"
#include "jvx-qt-helpers.h"

JVX_DECLARE_QT_METATYPES

void
uMainWindow::setTabPositionRight(int idTab)
{
	int i;
	jvxSize idFound = JVX_SIZE_UNSELECTED;

	QList<QTabBar *> tabList = findChildren<QTabBar *>();
	for(i = 0; i < tabList.size(); i++)
	{
		if(!tabList.isEmpty())
		{
			QTabBar *tabBar = tabList.at(i);
			QString txt = tabBar->tabText(i);

			// tabBar->currentIndex()
			if((txt == "Sequence Editor") || (txt == "Realtime Viewer"))
			{
				idFound = i;
			}
		}
	}
	if(JVX_CHECK_SIZE_SELECTED(idFound))
	{
		QTabBar *tabBar = tabList.at(JVX_SIZE_INT(idFound));
		tabBar->setCurrentIndex(idTab);
	}
}

#if 0
void
uMainWindow::resetJvxReferences()
{
	involvedComponents.theHost.hobject = NULL;
	involvedComponents.theHost.hFHost = NULL;
	involvedComponents.theTools.hTools = NULL;

}
#endif

jvxErrorType
uMainWindow::initSystem(QApplication* hdlApp, char* clStr[], int clNum)
{
	jvxApiString errMess;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	JVX_REGISTER_QT_METATYPES
	QSplashScreen* spl = NULL;
	this->setWindowTitle(JVX_DATA_STRING "--" JVX_VERSION_STRING);

	std::string exec = jvx_extractFileFromFilePath(clStr[0]);
	commLine.assign_args((const char**)&clStr[1], clNum - 1, exec.c_str());

#ifndef JVX_SPLASH_SUPPRESSED
	spl = jvx_start_splash_qt(theHostFeatures.config_ui.scalefac_splash,
		theHostFeatures.config_ui.pixmap_splash,
		theHostFeatures.config_ui.ftsize_splash,
		theHostFeatures.config_ui.italic_splash);
	if(spl)
	{
		spl->showMessage("Starting develop host...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	setupUi(this);

	theControl.init(static_cast<IjvxMainWindowControl_connect*>(this), menuRuntime_Parameters, menuApplication, this);

	subWidgets.bootupComplete = false;

	showMinimized();


	// Connect for different purposes
	connect(this, SIGNAL(emit_postMessage(QString, QColor)), this, SLOT(postMessage_inThread(QString, QColor)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_bootDelayed()), this, SLOT(bootDelayed()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_report_internals_have_changed_inThread(jvxComponentIdentification, IjvxObject*, jvxPropertyCategoryType, jvxSize, bool, jvxComponentIdentification, jvxPropertyCallPurpose)), this,
		SLOT(report_internals_have_changed_inThread(jvxComponentIdentification, IjvxObject*, jvxPropertyCategoryType, jvxSize, bool, jvxComponentIdentification, jvxPropertyCallPurpose)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_report_command_request(jvxCBitField , jvxHandle*, jvxSize )), 
		this, SLOT(report_command_request_inThread(jvxCBitField , jvxHandle* , jvxSize)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_request_command(CjvxReportCommandRequest*)), this,
		SLOT(request_command_inThread(CjvxReportCommandRequest*)), Qt::QueuedConnection); 
	connect(this, SIGNAL(emit_close_app()), this, SLOT(close_app()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_reportSequencerError(QString, QString)), this,
		SLOT(reportSequencerError_inThread(QString, QString)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_reopen_config(QString, bool)), this, SLOT(reopen_config(QString, bool)), Qt::QueuedConnection);

	connect_specific();

	/*
		 * ==================================================================================================
		 * ==================================================================================================
		 * ==================================================================================================
		 * ==================================================================================================
		 * ==================================================================================================
		*/

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Configuring develop host ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	res = this->boot_configure(&errMess, static_cast<IjvxCommandLine*>(&commLine), static_cast<IjvxReport*>(this), 
		static_cast<IjvxReportOnConfig*>(this), static_cast<IjvxReportStateSwitch*>(this), NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Initializing Host ...", Qt::AlignHCenter, QColor(0, 176, 240));
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
		spl->showMessage("Activating Host ...", Qt::AlignHCenter, QColor(0, 176, 240));
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
		spl->showMessage("Preparing Host ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif
	
	res = boot_prepare(&errMess, spl);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

	// Update first menu extensions
	setup_ext_menu_system();

	// Update ui
	updateWindow();

	//showMinimized();
	//showFullScreen();
	//showMaximized();
	emit emit_bootDelayed();

#ifndef JVX_SPLASH_SUPPRESSED
	jvx_stop_splash_qt(spl, this);
#endif

	return(JVX_NO_ERROR);
}

void
uMainWindow::bootDelayed()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errMess;
	boot_start(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}
}

#if 0
void
uMainWindow::load_host()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentIdentification cpType;
	bool multObj = false;

	resetJvxReferences();

	res = jvxHJvx_init(&involvedComponents.theHost.hobject);
	if ((res != JVX_NO_ERROR) || (involvedComponents.theHost.hobject == NULL))
	{
		fatalStop("Fatal Error", "Could not load main host component!");
	}
	involvedComponents.theHost.hobject->request_specialization((jvxHandle**)&involvedComponents.theHost.hFHost, &cpType, &multObj);
	assert(involvedComponents.theHost.hFHost);
	assert(cpType == JVX_COMPONENT_HOST);
}
#endif

void
uMainWindow::setup_dock_widgets()
{
	subWidgets.messages.theWidgetD = new QDockWidget_ext("Messages", this, &subWidgets.messages.added, JVX_QT_POSITION_MESSAGE_DOCK_WIDGET);
	subWidgets.messages.theWidgetD->setAllowedAreas(JVX_QT_POSITION_MESSAGE_DOCK_WIDGET);
	subWidgets.messages.theWidget = new messagesOutput(subWidgets.messages.theWidgetD);
	subWidgets.messages.theWidget->init();
	subWidgets.messages.theWidgetD->setWidget(subWidgets.messages.theWidget);
	subWidgets.messages.added = true;
	addDockWidget(JVX_QT_POSITION_MESSAGE_DOCK_WIDGET, subWidgets.messages.theWidgetD);

	subWidgets.sequencer.theWidgetD = new QDockWidget_ext("Sequencer", this, &subWidgets.sequencer.added, JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET);
	subWidgets.sequencer.theWidgetD->setAllowedAreas(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET);
	init_jvxQtSequencerWidget(&subWidgets.sequencer.theWidget, this);
	subWidgets.sequencer.theWidget->init(involvedHost.hHost, 0, NULL, static_cast<IjvxQtSpecificWidget_report*>(this));// this
	subWidgets.sequencer.theQWidget = NULL;
	subWidgets.sequencer.theWidget->getMyQWidget(&subWidgets.sequencer.theQWidget, 0);
	subWidgets.sequencer.theWidgetD->setWidget(subWidgets.sequencer.theQWidget);
	subWidgets.sequencer.added = true;
	addDockWidget(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET, subWidgets.sequencer.theWidgetD);

	subWidgets.realtimeViewer.props.theWidgetD = new QDockWidget_ext("RT Viewer Properties", this, &subWidgets.realtimeViewer.props.added, JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET);
	subWidgets.realtimeViewer.props.theWidgetD->setAllowedAreas(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET);
	subWidgets.realtimeViewer.props.theWidget = new CrealtimeViewer(subWidgets.realtimeViewer.props.theWidgetD, JVX_REALTIME_VIEWER_VIEW_PROPERTIES, tpAll);
	subWidgets.realtimeViewer.props.theWidget->init();
	subWidgets.realtimeViewer.props.theWidgetD->setWidget(subWidgets.realtimeViewer.props.theWidget);
	subWidgets.realtimeViewer.props.added = true;
	addDockWidget(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET, subWidgets.realtimeViewer.props.theWidgetD);

	subWidgets.realtimeViewer.plots.theWidgetD = new QDockWidget_ext("RT Viewer Plots", this, &subWidgets.realtimeViewer.plots.added, JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET);
	subWidgets.realtimeViewer.plots.theWidgetD->setAllowedAreas(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET);
	subWidgets.realtimeViewer.plots.theWidget = new CrealtimeViewer(subWidgets.realtimeViewer.plots.theWidgetD, JVX_REALTIME_VIEWER_VIEW_PLOTS, tpAll);
	subWidgets.realtimeViewer.plots.theWidget->init();
	subWidgets.realtimeViewer.plots.theWidgetD->setWidget(subWidgets.realtimeViewer.plots.theWidget);
	subWidgets.realtimeViewer.plots.added = true;
	addDockWidget(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET, subWidgets.realtimeViewer.plots.theWidgetD);

	subWidgets.connections.theWidgetD = new QDockWidget_ext("Object Connections", this, &subWidgets.connections.added, JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET);
	subWidgets.connections.theWidgetD->setAllowedAreas(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET);
	init_jvxQtConnectionWidget(&subWidgets.connections.theWidget, this);
	subWidgets.connections.theWidget->init(involvedHost.hHost, 0, NULL, static_cast<IjvxQtSpecificWidget_report*>(this));// this
	subWidgets.connections.theQWidget = NULL;
	subWidgets.connections.theWidget->getMyQWidget(&subWidgets.connections.theQWidget, 0);
	subWidgets.connections.theWidgetD->setWidget(subWidgets.connections.theQWidget);
	subWidgets.connections.added = true;
	addDockWidget(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET, subWidgets.connections.theWidgetD);

	// Order of the right tab dock widgets
	this->arrangeTabsRightDock();

	// ===============================================================================================
	// Allocate object to handle menues
	init_jvxQtMenuComponentsBridge(&subWidgets.bridgeMenuComponents, this);

	subWidgets.bridgeMenuComponents->createMenues(&theHostFeatures,
		involvedHost.hHost,
		involvedComponents.theTools.hTools,
		menuConfiguration, tpAll, 
		&systemParams.skipStateSelected,
		this);
	// ===========================================================================

	if (subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->init_extend_specific_widgets(static_cast<IjvxMainWindowControl*>(&theControl), static_cast<IjvxQtSpecificWidget_report*>(this));
	}

	// 2 start

	if (theHostFeatures.showExpertUi_seq)
	{
		subWidgets.sequencer.theWidgetD->show();
		subWidgets.connections.theWidgetD->show();
	}
	else
	{
		subWidgets.sequencer.theWidgetD->hide();
		subWidgets.connections.theWidgetD->hide();
	}
	if (theHostFeatures.showExpertUi_props)
	{
		subWidgets.realtimeViewer.props.theWidgetD->show();
	}
	else
	{
		subWidgets.realtimeViewer.props.theWidgetD->hide();
	}
	if (theHostFeatures.showExpertUi_plots)
	{
		subWidgets.realtimeViewer.plots.theWidgetD->show();
	}
	else
	{
		subWidgets.realtimeViewer.plots.theWidgetD->hide();
	}
	if (theHostFeatures.showExpertUi_messages)
	{
		subWidgets.messages.theWidgetD->show();
	}
	else
	{
		subWidgets.messages.theWidgetD->hide();
	}
	if (theHostFeatures.showExpertUi_menuelements)
	{
		menuConfiguration->menuAction()->setVisible(true);
		menuView->menuAction()->setVisible(true);
		menuRuntime_Parameters->menuAction()->setVisible(true);
		menuSettings->menuAction()->setVisible(true);
	}
	else
	{
		menuConfiguration->menuAction()->setVisible(false);
		menuView->menuAction()->setVisible(false);
		menuRuntime_Parameters->menuAction()->setVisible(false);
		menuSettings->menuAction()->setVisible(false);
	}

	// 2 stop

	// Release dock widgets to be handles as all parts are complete
	subWidgets.bootupComplete = true;
}

void
uMainWindow::finalize_dock_widgets()
{
	// Pass host reference to sequencer ui
	subWidgets.sequencer.theWidget->activate();
	subWidgets.realtimeViewer.props.theWidget->setHostRef(involvedHost.hHost, static_cast<IjvxReport*>(this));
	subWidgets.realtimeViewer.plots.theWidget->setHostRef(involvedHost.hHost, static_cast<IjvxReport*>(this));
	subWidgets.connections.theWidget->activate();

	theControl.activate();
}

void
uMainWindow::postbootup_dock_widgets()
{
	// Update sequencer/editor
	subWidgets.sequencer.theWidget->update_window_rebuild();
	subWidgets.sequencer.theWidget->set_control_ref(static_cast<IjvxMainWindowControl*>(&theControl), static_cast<IjvxQtSequencerWidget_report*>(this));
}
