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

#ifndef JVX_SPLASH_SUPPRESSED
#include <QtWidgets/QSplashScreen>
#endif

//#define DEFAULT_NAME_CONFIG_FILE ".session.rtproc"
//#define POSTFIX_DONT_LOAD_CONFIG "--no-config"

#ifdef COMPILE_FOR_CONSOLE
int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
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
	/*
	std::string configFilename = DEFAULT_NAME_CONFIG_FILE;

	if(argc > 1)
		configFilename = argv[1];

	if(configFilename == POSTFIX_DONT_LOAD_CONFIG)
		configFilename = "";
*/
	// Line required for Linux systems (whatever this is)
	JVX_INIT_LOCALE("C");
	//Sleep(10000);

	//QPixmap pixmap("images/sdt.png");
	QPixmap pixmap("images/xxxSplash.png");
	QSplashScreen splash(pixmap);
	splash.show();

	uMainWindow sWindow;


	// If a fatal error, occurs, this is the right place to leave
#ifdef COMPILE_FOR_CONSOLE
	if(sWindow.initSystem(&app, &argv[1], argc-1) != JVX_NO_ERROR)
#else
	if(sWindow.initSystem(&app, &__argv[1], __argc-1) != JVX_NO_ERROR)
#endif
	{
		exit(0);
	}

	sWindow.show();

    splash.finish(&sWindow);

	app.exec();

	return(0);
}
