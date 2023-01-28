#include "jvx.h"

extern "C"
{
	#define FUNC_CORE_PROT_DECLARE jvx_init_request_cmd_args
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_init_request_cmd_args_local
#define FUNC_CORE_PROT_ARGS jvxCommandLineArgSpecifyType tp, jvxSize id, jvxApiString* astr
#define FUNC_CORE_PROT_RETURNVAL jvxBool

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_init_request_cmd_args=jvx_init_request_cmd_args_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_init_request_cmd_args=_jvx_init_request_cmd_args_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;

		// Default implementation does just nothing
		return false;
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL
}

void
jvx_appArgs(char**& newArgv, int& newArgc, std::vector<std::string>& lstArgs)
{
	jvxSize i;
	newArgv = new char* [lstArgs.size()];
	for (i = 0; i < lstArgs.size(); i++)
	{
		std::cout << "Argv[" << i << "]: " << lstArgs[i] << std::endl;
		newArgv[i] = (char*)lstArgs[i].c_str();
	}
	newArgc = lstArgs.size();
}

void
jvx_deriveRunArgsJvx(std::vector<std::string>& lst, const std::string& argv0, bool binaryInSubfolder)
{
	jvxBool retVal = false;
	jvxApiString astr;
	std::string exec = jvx_extractDirectoryFromFilePath(argv0);
	std::string app = jvx_extractFileFromFilePath(argv0);

	app = jvx_fileBaseName(app);
	std::string workingdir = exec;

	if (binaryInSubfolder)
	{
		// If the executable is located in bin folder (typical cmake installation) runtime folder is up one folder
		workingdir = jvx_changeDirectoryUp(exec);
	}

#ifdef JVX_OS_MACOSX
	workingdir = jvx_changeDirectoryPath(workingdir, "runtime");
#endif

	lst.push_back("--workdir");
	lst.push_back(workingdir);
	lst.push_back("--systemdir");
	lst.push_back(workingdir);

	std::string jvxdir = jvx_changeDirectoryPath(workingdir, "jvxComponents");
	lst.push_back("--jvxdir");
	lst.push_back(jvxdir);

	lst.push_back("--configdir");
	lst.push_back(workingdir);

#ifdef JVX_OS_WINDOWS
	std::string userLocalpath = JVX_GET_USER_APPDATA_PATH() + JVX_SEPARATOR_DIR + app;
#else
	std::string userLocalpath = JVX_GET_USER_DESKTOP_PATH();
#endif
	lst.push_back("--userdir");
	lst.push_back(userLocalpath);
	lst.push_back("--configdir");
	lst.push_back(userLocalpath);
	if (JVX_DIRECTORY_EXISTS(userLocalpath.c_str()) == c_false)
	{
		JVX_CREATE_DIRECTORY(userLocalpath.c_str());
	}

	std::string cfg_file = app; // app_basename.toLatin1().data();
	cfg_file += ".jvx";
	astr.assign(cfg_file);
	lst.push_back("--config");
	retVal = jvx_init_request_cmd_args(JVX_COMMAND_LINE_ARG_CONFIG_FILE, 0, &astr);
	if (retVal)
	{
		cfg_file = astr.std_str();
	}
	lst.push_back(cfg_file);

	// All additional configs
	std::vector<std::string> lstTokens;
	jvxSize i;

	jvx_parseCommandLineOneToken(JVX_COMMAND_LINE_TOKENS_BUILDIN, lstTokens, ' ');
	for (i = 0; i < lstTokens.size(); i++)
	{
		lst.push_back(lstTokens[i]);
	}

	i = 0;
	while (1)
	{
		astr.assign(JVX_COMMAND_LINE_TOKENS_BUILDIN);
		retVal = jvx_init_request_cmd_args(JVX_COMMAND_LINE_ARG_CONFIG_ADD, i, &astr);
		if (retVal)
		{
			lst.push_back(astr.std_str());
			i++;
		}
		else
		{
			break;
		}
	}
}

#if 0

// This taken from the QT main app - should work identical to the other version
void
jvx_deriveRunArgsJvx(std::vector<std::string>& lst, const std::string& argv0)
{
	jvxBool retVal = false;
	jvxApiString astr;
	QDir dir(argv0.c_str());

	dir.cdUp();
	QString app = dir.relativeFilePath(argv0.c_str());

	// Remove the app name
	QFileInfo fInfo;
	fInfo.setFile(app);
	QString app_basename = fInfo.baseName();

	// From bin to working dir
	dir.cdUp();

#ifdef JVX_OS_MACOSX
	retVal = dir.cd("runtime");
#endif

	QString path = dir.absolutePath();
	
	std::string workingdir = path.toLatin1().data();

	lst.push_back("--workdir");
	lst.push_back(workingdir);
	lst.push_back("--systemdir");
	lst.push_back(workingdir);

	dir.cd("jvxComponents");
	path = dir.absolutePath();
	std::string jvxdir = path.toLatin1().data();
	lst.push_back("--jvxdir");
	lst.push_back(jvxdir);

	lst.push_back("--configdir");
	lst.push_back(workingdir);

	std::string userdesktoppath = JVX_GET_USER_DESKTOP_PATH();
	lst.push_back("--userdir");
	lst.push_back(userdesktoppath);
	lst.push_back("--configdir");
	lst.push_back(userdesktoppath);

	std::string cfg_file = app_basename.toLatin1().data();
	cfg_file += ".jvx";
	astr.assign(cfg_file);
	lst.push_back("--config");
	retVal = jvx_init_request_cmd_args(JVX_COMMAND_LINE_ARG_CONFIG_FILE, 0, &astr);
	if (retVal)
	{
		cfg_file = astr.std_str();
	}
	lst.push_back(cfg_file);

	// All additional configs
	std::vector<std::string> lstTokens;
	jvxSize i;

	jvx_parseCommandLineOneToken(JVX_COMMAND_LINE_TOKENS_BUILDIN, lstTokens, ' ');
	for (i = 0; i < lstTokens.size(); i++)
	{
		lst.push_back(lstTokens[i]);
	}

	i = 0;
	while (1)
	{
		astr.assign(JVX_COMMAND_LINE_TOKENS_BUILDIN);
		retVal = jvx_init_request_cmd_args(JVX_COMMAND_LINE_ARG_CONFIG_ADD, i, &astr);
		if (retVal)
		{
			lst.push_back(astr.std_str());
			i++;
		}
		else
		{
			break;
		}
	}
}

#endif
