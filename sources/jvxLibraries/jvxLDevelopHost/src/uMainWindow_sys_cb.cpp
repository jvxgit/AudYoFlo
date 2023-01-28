#include "uMainWindow.h"
#include "uMainWindow_widgets.h"
#include "jvxQtHostHookEntries.h"
#include "jvxHost_config.h"

#ifndef JVX_SPLASH_SUPPRESSED
#include <QtWidgets/QSplashScreen>
#endif

// =======================================================================================
// =======================================================================================

// Weak fuction to do some kind of pre configuration
extern "C"
{
#define FUNC_CORE_PROT_DECLARE jvx_preconfigure_host
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_preconfigure_host_local
#define FUNC_CORE_PROT_ARGS IjvxFactoryHost* fhost, jvxBool onBoot
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_preconfigure_host=jvx_preconfigure_host_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_preconfigure_host=_jvx_preconfigure_host_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		if (onBoot)
		{
			std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
			// Default implementation does just nothing
		}
		return JVX_NO_ERROR;
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL

}

// =======================================================================================
// =======================================================================================

jvxErrorType
uMainWindow::boot_specify_slots_specific()
{
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::boot_initialize_specific(jvxApiString* errloc)
{
	QVBoxLayout* myVLayout = NULL;
	QGridLayout* myGLayout = NULL;
	QSize sz, sz2;
	jvxSize i, j;
	jvxErrorType res = JVX_NO_ERROR;
	QWidget* theNewWidget = NULL;
	
	theEventLoop.initialize(involvedComponents.theHost.hFHost, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, 1000);
	theEventLoop.start();
	involvedComponents.theHost.hFHost->add_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxEventLoop*>(&theEventLoop)), JVX_INTERFACE_EVENTLOOP);

	// setup_dock_widgets();

	// We need a valid host reference with a valid report reference when setting up the main widget
	subWidgets.main.theWidget = NULL;
	mainWindow_UiExtension_host_init(&subWidgets.main.theWidget, static_cast<QWidget*>(this));
	assert(subWidgets.main.theWidget);

	subWidgets.main.theWidget->init_submodule(involvedHost.hHost);
	subWidgets.main.theWidget->return_widget(&theNewWidget);
	//assert(theNewWidget);
	//setCentralWidget(theNewWidget);
	theNewWidget->show();

	res = boot_negotiate_specific();
	if (res != JVX_NO_ERROR)
		return res;

	// ========================================================================================
	// Some additional pre-configurations
	res = jvx_preconfigure_host(this->involvedComponents.theHost.hFHost, true);
	assert(res == JVX_NO_ERROR);
	// ========================================================================================

	QLayout* myLay = NULL;

	switch (theHostFeatures.config_ui.tweakUi)
	{
	case JVX_QT_MAINWIDGET_SCROLL:
		frame_add_me_scroll->setWidget(theNewWidget);
		frame_add_me_noscroll->hide();
		stackedWidget->setCurrentIndex(1);
		break;
	case JVX_QT_MAINWIDGET_EXPAND_FULLSIZE:
		myLay = frame_add_me_noscroll->layout();
		if (myLay)
			delete myLay;

		myVLayout = new QVBoxLayout();
		myVLayout->addWidget(theNewWidget);
		frame_add_me_noscroll->setLayout(myVLayout);
		frame_add_me_scroll->hide();
		myVLayout->setContentsMargins(0, 0, 0, 0);
		stackedWidget->setCurrentIndex(0);
		break;
	case JVX_QT_MAINWIDGET_NO_EXPAND_CENTER:
		myLay = frame_add_me_noscroll->layout();
		if (myLay)
			delete myLay;

		myGLayout = new QGridLayout();
		myGLayout->addWidget(theNewWidget, 0, 0, (Qt::AlignHCenter | Qt::AlignVCenter));
		frame_add_me_noscroll->setLayout(myGLayout);
		frame_add_me_scroll->hide();
		myGLayout->setContentsMargins(0, 0, 0, 0);
		stackedWidget->setCurrentIndex(0);
		break;
	}

	sz.setWidth(0);
	sz.setHeight(0);

	if (theHostFeatures.config_ui.minWidthWindow >= 0)
	{
		sz.setWidth(theHostFeatures.config_ui.minWidthWindow);
	}
	if (theHostFeatures.config_ui.minHeightWindow >= 0)
	{
		sz.setHeight(theHostFeatures.config_ui.minHeightWindow);
	}
	this->setMinimumSize(sz);

	sz.setWidth(QWIDGETSIZE_MAX);
	sz.setHeight(QWIDGETSIZE_MAX);
	if (theHostFeatures.config_ui.maxWidthWindow >= 0)
	{
		sz.setWidth(theHostFeatures.config_ui.maxWidthWindow);
	}
	if (theHostFeatures.config_ui.maxHeightWindow >= 0)
	{
		sz.setHeight(theHostFeatures.config_ui.maxHeightWindow);
	}
	this->setMaximumSize(sz);

	// ===============================================================================
	setup_dock_widgets();

	if (theHostFeatures.showExpertUi_seq)
	{
		subWidgets.sequencer.theWidgetD->show();
	}
	else
	{
		subWidgets.sequencer.theWidgetD->hide();
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

	IjvxPropertyPool* thePool = NULL;
	menuProperty_Pool->clear();
	res = this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTY_POOL, 
		reinterpret_cast<jvxHandle**>(&thePool));
	if ((res == JVX_NO_ERROR) && thePool)
	{
		jvxSize numProps;
		jvxSize idProp;
		jvxApiString astr;
		jvxSize numassoc;
		jvxComponentIdentification tp;
		thePool->number_system_properties(&numProps);
		for (i = 0; i < numProps; i++)
		{
			thePool->system_property_id(i, &idProp, &astr);
			std::string nm = "Id=" + jvx_size2String(idProp) + " - <" + astr.std_str() + ">";

			QMenu* nMenu = new QMenu(this);
			nMenu->setObjectName(nm.c_str());
			nMenu->setTitle(nm.c_str());
			
			thePool->number_entries_pool_property(idProp, &numassoc);
			for (j = 0; j < numassoc; j++)
			{
				jvxBool isMaster = false;
				thePool->entry_pool_property(idProp, j, &tp, &isMaster);
				nm = jvxComponentIdentification_txt(tp);
				QAction* act = new QAction(this);
				act->setText(nm.c_str());
				if (isMaster)
				{
					QFont ft = act->font();
					ft.setBold(true);
					act->setFont(ft);
				}
				nMenu->addAction(act);
			}
			menuProperty_Pool->addAction(nMenu->menuAction());
		}
		this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTY_POOL,
			reinterpret_cast<jvxHandle*>(thePool));
	}

	return res;
}

jvxErrorType 
uMainWindow::boot_prepare_specific(jvxApiString* errloc) 
{
	postbootup_specific();

	postbootup_dock_widgets();

	return JVX_NO_ERROR;
}

void 
uMainWindow::report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName)
{
	if (callConf->configModeFlags & JVX_CONFIG_MODE_OVERLAY)
	{
		this->subWidgets.realtimeViewer.props.theWidget->updateAllWidgetsOnStateChange();
		this->subWidgets.realtimeViewer.plots.theWidget->updateAllWidgetsOnStateChange();
	}

	subWidgets.main.theWidget->inform_config_read_complete(fName);
}

jvxErrorType 
uMainWindow::boot_start_specific(jvxApiString* errloc)
{
#ifdef JVX_OS_WINDOWS
	// windows issue
// no idea why, but if I use only showMaximized the window does not show up maximized - QT bug?
	showMinimized();
#endif
	showMaximized();

	bootDelayed_specific();

	if (systemParams.auto_start)
	{
		control_startSequencer();
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::boot_activate_specific(jvxApiString* errloc)
{
	jvxErrorType res = JVX_NO_ERROR;
	// Need to activate tooltips for file menu
	menuFile->setToolTipsVisible(true);

	//subWidgets.sequences.theWidget->updateWindow_rebuild();
	finalize_dock_widgets();

	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::shutdown_terminate_specific(jvxApiString* errloc)
{
	release_dock_widgets();

	// ========================================================================================
	// Some additional pre-configurations
	jvx_preconfigure_host(this->involvedComponents.theHost.hFHost, false);
	// ========================================================================================

	// ========================================================================================
	shutdown_specific();
	// ========================================================================================

	// ====================================================================
	// Stop main central widget
	subWidgets.main.theWidget->terminate_submodule();
	mainWindow_UiExtension_host_terminate(subWidgets.main.theWidget);
	subWidgets.main.theWidget = NULL;

	involvedComponents.theHost.hFHost->remove_external_interface(
		reinterpret_cast<jvxHandle*>(static_cast<IjvxEventLoop*>(&theEventLoop)), JVX_INTERFACE_EVENTLOOP);

	theEventLoop.stop();
	theEventLoop.terminate();

	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::shutdown_deactivate_specific(jvxApiString* errloc)
{
	pre_release_dock_widgets();

	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::shutdown_postprocess_specific(jvxApiString* errloc)
{

	this->subWidgets.realtimeViewer.props.theWidget->newSelectionButton_stop();
	this->subWidgets.realtimeViewer.plots.theWidget->newSelectionButton_stop();

	// ====================================================================
	// Stop main central widget
	subWidgets.main.theWidget->inform_about_to_shutdown();

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
	this->postMessage_inThread(txt, prio);
}

void 
uMainWindow::inform_config_read_start(const char* nmfile, jvxHandle* context)
{
#ifdef JVX_SPLASH_SUPPRESSED
	std::cout << "Reading configuration <" << nmfile << ">..." << std::endl;
#else
	QSplashScreen* splash = (QSplashScreen*)context;
	splash->showMessage(("Reading configuration <" + (std::string)nmfile + ">...").c_str(), Qt::AlignHCenter, QColor(0, 176, 240));
#endif
	qApp->processEvents();
}

void 
uMainWindow::inform_config_read_stop(jvxErrorType resRead, const char* nmfile, jvxHandle* context)
{
	if (resRead != JVX_NO_ERROR)
		std::cout << "Reading config file " << nmfile << " failed, reason: " << jvxErrorType_descr(resRead) << std::endl;

}

