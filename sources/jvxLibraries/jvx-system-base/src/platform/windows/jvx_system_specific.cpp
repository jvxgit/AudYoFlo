#include "jvx_platform.h"
#include "jvx.h"

JVX_CREATE_PROCESS_RESULT JVX_CREATE_PROCESS_WITH_ARGS(JVX_CREATE_PROCESS_HANDLE& procHandle, const std::string& cmdLine, const std::list<std::string>& args) 
{
	std::string cmdWithArgs = cmdLine;
	for (auto elm : args)
	{
		cmdWithArgs += " ";
		cmdWithArgs += elm;
	}

	// return CreateProcess(NULL, (char*)cmdWithArgs.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &procHandle.info, &procHandle.processInfo);

	// If I run the process with inherit = true, sockets can not be closed properly. It seems that this issue was also
	// reason for this post:
	// https://stackoverflow.com/questions/58813349/createprocess-occupies-socket-port-only-one-usage-of-each-socket-address-permit
	// I did not expect it to come from this side..
	return CreateProcess(NULL, (char*)cmdWithArgs.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &procHandle.info, &procHandle.processInfo);
}

std::string JVX_GET_CURRENT_MODULE_PATH(void* ptrIdentify)
{
	HMODULE mod;

	/*
	https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandleexa?redirectedfrom=MSDN
	*/
	GetModuleHandleExA(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCTSTR)ptrIdentify,
		&mod);

	std::string retVal;
	char bufc[MAX_PATH + 1] = { 0 };

	GetModuleFileNameA(mod, bufc,MAX_PATH);
	retVal = bufc;
	return retVal;
}

std::string JVX_GET_USER_DESKTOP_PATH()
{
	std::string retVal;
	/*
	https://stackoverflow.com/questions/17933917/get-the-users-desktop-folder-using-windows-api
	*/
	char path[MAX_PATH] = { 0 };
	SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, path);
	retVal = path;
	return retVal;
}

std::string JVX_GET_USER_APPDATA_PATH()
{
	std::string retVal;
	/*
	https://stackoverflow.com/questions/17933917/get-the-users-desktop-folder-using-windows-api
	*/
	char path[MAX_PATH] = { 0 }; 
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path);
	retVal = path;
	return retVal;
}
	
std::string JVX_DISPLAY_LAST_ERROR_LOAD(std::string fName)
{
	DWORD errID = GetLastError();//errno

	char str[1024];
#if _MSC_VER >= 1600
	sprintf_s(str, "%ld", errID);
#else
	sprintf(str, "%ld", errID);
#endif

	std::string errMess = (std::string)"Unknown, error id=" + str;
	DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
	LPSTR buf = NULL;
	/*
	 DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM;
	 char str[
	 */
	if (FormatMessage(dwFlags, NULL, errID, 0/*GetSystemDefaultLangID()*/, (LPTSTR)&buf, 1024, NULL) != 0)
	{
		errMess = "File: " + fName + ", opening failed: " + buf;
		LocalFree(buf);
	}
	else
	{
		errID = GetLastError();

#if _MSC_VER >= 1600
		sprintf_s(str, "%ld", errID);
#else
		sprintf(str, "%ld", errID);
#endif

		errMess = "File: " + fName + ", opening failed, error id = " + str;
	}
	return(errMess);
}

JVX_HMODULE JVX_LOADLIBRARY_PATHVEC(std::string fName, std::vector<std::string> PathVec)
{
	for (size_t i = 0; i < PathVec.size(); i++) {
		if (PathVec[i] != "")
		{
			JVX_HMODULE dll_loaded = LoadLibraryA((std::string(PathVec[i] + JVX_SEPARATOR_DIR + fName).c_str()));
			if (dll_loaded) return(dll_loaded);
		}
	}
	return(LoadLibraryA(fName.c_str()));
}

JVX_HMODULE JVX_LOADLIBRARY_PATH(std::string fName, std::string dir)
{
	if (dir != "")
	{
		return LoadLibraryA(((std::string)dir + JVX_SEPARATOR_DIR + fName).c_str());
	}
	else {
		return(LoadLibraryA(fName.c_str()));
	}
}

std::string JVX_TIME()
{
	char ret[256];
	memset(ret, 0, sizeof(char) * 256);
	_strtime_s(ret, 256);
	std::string strDate = ret;

	return(strDate);
}

std::string JVX_DATE()
{
	char ret[256];
	memset(ret, 0, sizeof(char) * 256);
	_strdate_s(ret, 256);
	std::string strDate = ret;

	return(strDate);
}

/*
std::string JVX_GETCURRENTDIRECTORY()
{
	std::string retVal = __FUNCTION__;
	retVal += ": ERROR on convert, buffer may be too short";
	char destBuf[JVX_MAXSTRING + 1] = { 0 };
	char* bufRet = _getcwd(destBuf, JVX_MAXSTRING); /*GetCurrentDirectory(b, a)* /
	if (bufRet)
	{
		retVal = bufRet;
	}
	return retVal;
}
*/