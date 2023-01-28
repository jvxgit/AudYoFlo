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

#ifdef JVX_MAIN_APPLICATION_CATCH_EVENTS
#include "CjvxApplication.h"
//
// Info on tslib:
// https://stackoverflow.com/questions/28868963/qt-5-4-linux-touchscreen-input-with-tslib-on-raspberry-pi-failing-with-linuxfb-q
//
// http://e2e.ti.com/support/processors/f/791/t/544310
#endif

 //#define DEFAULT_NAME_CONFIG_FILE ".session.rtproc"
 //#define POSTFIX_DONT_LOAD_CONFIG "--no-config"

extern "C"
{
#define FUNC_CORE_PROT_DECLARE jvx_init_before_start
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_init_before_start_local
#define FUNC_CORE_PROT_ARGS
#define FUNC_CORE_PROT_RETURNVAL void

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_init_before_start=jvx_init_before_start_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_init_before_start=_jvx_init_before_start_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		// Default implementation does just nothing
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL

}
  
/*
 * I do not know what this function was intended for - seems that it is no longer in use...
extern
void delay(int msec)
{
	QTime dieTime= QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
*/

void
jvx_deriveRunArgsQt(std::vector<std::string>& lst, const std::string& argv0)
{
	jvxBool retVal = false;
	jvxApiString astr;
	QDir dir(argv0.c_str());
	
	// Remove the app name
	dir.cdUp();

	// Get the bin path
	QString path = dir.absolutePath();
	
	// Get the working directory
	std::string workdir = path.toLatin1().data();

	// Plugins folder
#ifdef JVX_OS_MACOSX
	dir.cdUp();
	retVal = dir.cd("PlugIns");
#else
	retVal = dir.cd("plugins");
#endif

	path = dir.absolutePath();
	std::string pluginpath = path.toLatin1().data(); 
	dir.cdUp();

	std::cout << "Setting environment variable QT_PLUGIN_PATH=" << pluginpath << std::endl;
	JVX_SETENVIRONMENTVARIABLE("QT_PLUGIN_PATH", pluginpath.c_str(), 1);

#ifdef JVX_OS_MACOSX

	// This seems to be a bug in Mac OS X: -pluginplatformpath seems to not work
	// properly: the platform plugin can not be found if NOT setting the environment variable
	// I can well deal with unsing the environment variable instead
	// setenv("QT_PLUGIN_PATH", pluginpath.c_str(), 1);

	dir.cdUp();

#else

	/*
	lst.push_back("-platformpluginpath");
	astr.assign(pluginpath);
	retVal = jvx_init_request_cmd_args(JVX_COMMAND_LINE_ARG_PLUGIN_PATH, 0, &astr);
	if (retVal)
	{
		pluginpath = astr.std_str();
	}
	lst.push_back(pluginpath);
	*/
#endif
}

#if 0
	jvxBool retVal;
	jvxSize i;
	jvxSize cnt;
	jvxApiString astr;
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

	astr.assign(strpath);
	retVal = jvx_init_request_cmd_args(JVX_COMMAND_LINE_ARG_PLUGIN_PATH, 0, &astr);
	if (retVal)
	{
		lstArgs.push_back(astr.std_str());
	}
	else
	{
		lstArgs.push_back(strpath);
	}

	// ========================================================================

	strpath = path.toLatin1().constData();
	lstArgs.push_back("--config");

	astr.assign(strpath);
	retVal = jvx_init_request_cmd_args(JVX_COMMAND_LINE_ARG_CONFIG_FILE, 0, &astr);
	if (retVal)
	{
		lstArgs.push_back(astr.std_str());
	}
	else
	{
		strpath += "/runtime/config.rtproc";
		lstArgs.push_back(strpath);
	}

	cnt = 0;
	while (1)
	{
		retVal = jvx_init_request_cmd_args(JVX_COMMAND_LINE_ARG_CONFIG_ADD, cnt, &astr);
		if (retVal)
		{
			lstArgs.push_back(astr.std_str());
			cnt++;
		}
		else
		{
			break;
		}
	}
	// ======================================================================

	char** newArgv = new char* [lstArgs.size()];
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

#endif

#ifdef COMPILE_FOR_CONSOLE
int main(int argc, char* argv[])
{
#else
int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
)
{
	int argc = __argc;
	char** argv = __argv;

#endif

	jvx_init_before_start();

	std::vector<std::string> lstArgs;
	char** newArgv = NULL;
	int newArgc = 0;
	jvxBool generatedArgs = false;

	if (argc == 1)
	{
		// If there are no arguments, we need to create our own
		jvxSize i;
		lstArgs.push_back(argv[0]);
		jvx_deriveRunArgsQt(lstArgs, argv[0]);
		jvx_deriveRunArgsJvx(lstArgs, argv[0]);
	}
	else
	{
		jvxSize i;
		for(i = 1; i < argc; i++)
		{
			lstArgs.push_back(argv[i]);
		}
	}

	jvx_appArgs(newArgv, newArgc, lstArgs);
	generatedArgs = true;

#ifdef JVX_MAIN_APPLICATION_CATCH_EVENTS
	CjvxApplication app(newArgc, newArgv);
#else
	QApplication app(newArgc, newArgv);
#endif

	JVX_INIT_LOCALE("C");
	uMainWindow sWindow;

	if(sWindow.initSystem(&app, newArgv, newArgc) != JVX_NO_ERROR)
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

	app.exec();

	if (generatedArgs)
	{
		delete[](newArgv);
		newArgc = 0;
	}
	return 0;
}
