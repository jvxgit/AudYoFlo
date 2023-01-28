/**
 * =========================================================================
 *
 *
 * =========================================================================
 *
 * File: main.cpp
 *
 * =========================================================================
 *
 * Purpose:
 *
 * =========================================================================
 */

#include "uMainWindow.h"
#include <iostream>
#include <QDir>
#include <QTime>
#include <QtWidgets/QSplashScreen>
#include "jvx-qt-helpers.h"

 //#define DEFAULT_NAME_CONFIG_FILE ".session.rtproc"
 //#define POSTFIX_DONT_LOAD_CONFIG "--no-config"

void delay(int msec)
{
	QTime dieTime = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


#ifdef COMPILE_FOR_CONSOLE
int main(int argc, char* argv[])
{
#ifdef JVX_OS_MACOSX
	jvxSize i;

	QDir dir(argv[0]); // e.g. appdir/Contents/MacOS/appname
	assert(dir.cdUp());
	assert(dir.cdUp());
	QString path = dir.absolutePath();
	std::vector<std::string> lstArgs;
	for (i = 0; i < argc; i++)
	{
		lstArgs.push_back(argv[i]);
	}

	std::string strpath = path.toLatin1().constData();
	strpath += "/runtime/jvxComponents";
	lstArgs.push_back("--jvxdir");
	lstArgs.push_back(strpath);

	strpath = path.toLatin1().constData();
	strpath += "/PlugIns";
	lstArgs.push_back("-platformpluginpath");
	lstArgs.push_back(strpath);

	strpath = path.toLatin1().constData();
	strpath += "/runtime/config.rtproc";
	lstArgs.push_back("--config");
	lstArgs.push_back(strpath);

	char** newArgv = new char*[lstArgs.size()];
	for (i = 0; i < lstArgs.size(); i++)
	{
		std::cout << "Argv[" << i << "]: " << lstArgs[i] << std::endl;
		newArgv[i] = (char*)lstArgs[i].c_str();
	}
	int newArgc = lstArgs.size();
	/*
	QDir dir(argv[0]); // e.g. appdir/Contents/MacOS/appname
	assert(dir.cdUp());
	assert(dir.cdUp());
	assert(dir.cd("Resources")); // e.g. appdir/Contents/PlugIns
	QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));
	printf("after change, libraryPaths=(%s)\n", QCoreApplication::libraryPaths().join(",").toUtf8().data());
	*/
	//QApplication app(argc, argv);
	QApplication app(newArgc, newArgv);
	path = path + "../../Resources";
	QCoreApplication::setLibraryPaths(QStringList(path));

#else
	QApplication app(argc, argv);
#endif
#else
int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
)
{
	QApplication app(__argc, __argv);
#endif
	jvxSize ymin, xmin;
	QSize szPix;
	/*
	std::string configFilename = DEFAULT_NAME_CONFIG_FILE;

	if(argc > 1)
		configFilename = argv[1];

	if(configFilename == POSTFIX_DONT_LOAD_CONFIG)
		configFilename = "";
*/
// Line required for Linux systems (whatever this is)
	JVX_INIT_LOCALE("C");
	uMainWindow sWindow;
	QSplashScreen* spl = NULL;
	
#ifndef JVX_SPLASH_SUPPRESSED
	spl = jvx_start_splash_qt();
#endif


	// If a fatal error, occurs, this is the right place to leave
#ifdef COMPILE_FOR_CONSOLE

#ifdef JVX_OS_MACOSX

	if (sWindow.initialize(&app, &newArgv[1], newArgc - 1) != JVX_NO_ERROR)
#else
	if (sWindow.initialize(&app, &argv[0], argc) != JVX_NO_ERROR)
#endif

#else
	if (sWindow.initialize(&app, &__argv[0], __argc) != JVX_NO_ERROR)
#endif

	{
		exit(0);
	}

	sWindow.show();
	sWindow.setWindowIcon(QIcon(":/images/images/icon.png"));

	/* Thought this would add a taskbar icon but it does not
#ifdef _WIN32
	HWND hwnd = (HWND)sWindow.winId();
	ShowWindow(hwnd, SW_HIDE);
	SetWindowLongPtr(hwnd, GWL_EXSTYLE,
				GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_APPWINDOW);
	ShowWindow(hwnd, SW_SHOW);
#endif
	*/
	
#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		jvx_stop_splash_qt(spl, &sWindow);
	}
#endif

	app.exec();

	return(0);
}
