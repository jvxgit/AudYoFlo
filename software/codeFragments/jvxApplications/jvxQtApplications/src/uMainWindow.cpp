#include "uMainWindow.h"
#include <iostream>
#include <QtWidgets/QSplashScreen>

uMainWindow::uMainWindow() 
{
}

uMainWindow::~uMainWindow(void)
{
}

jvxErrorType
uMainWindow::initSystem(QApplication* hdlApp, char* clStr[], int clNum)
{
	QSplashScreen * spl = NULL;

#ifndef JVX_SPLASH_SUPPRESSED
	spl = jvx_start_splash_qt();
#endif
	
	IjvxConfigurationExtender* cfgExt = NULL;

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Starting audio host ...", Qt::AlignHCenter, QColor(0, 176, 240));
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

	// Status variable for bootup sequence
	bootupComplete = false;

	// signals Connect for different purposes
	connect(this, SIGNAL(emit_boot_delayed()), this, SLOT(boot_delayed()), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_close_app()), this, SLOT(close_app()), Qt::QueuedConnection);

	// All project specific connections in UI
	/*
	// connect_specific();
	*/
	
	// Start audio dialog window - even if no content can be shown yet
	
	// Bootup complete
	bootupComplete = true;

	// Check command line arguments
	CjvxCommandline::reset_command_line();
	CjvxCommandline::parse_command_line(clStr, clNum);

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Initializing application ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	// ===========================================================
	// Initialize the host
	// ===========================================================
	/*
	// initializeHost();
	*/

	// Trigger boot Delayed callback (to leave constructor)
	emit emit_boot_delayed();

#ifndef JVX_SPLASH_SUPPRESSED
	jvx_stop_splash(spl, this);
#endif

	return(JVX_NO_ERROR);
}

void
uMainWindow::closeEvent(QCloseEvent* event)
{
	
    // If this app is not yet ready to stop, we need to come back later. We have no chance
    // to wait within the QT loop as callbacks which must be stopped need to wait for QT thread
    if(0)
	{
		emit emit_close_app();
		event->ignore();
		return;
    }
    event->accept();
}

void
uMainWindow::close_app()
{
	this->close();
}

void
uMainWindow::boot_delayed()
{
}

void
uMainWindow::save()
{
}

void 
uMainWindow::open_preferences()
{
}

void 
uMainWindow::save_as()
{
}

void 
uMainWindow::open()
{
}

