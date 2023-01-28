#include "jvx_platform.h"
#include "jvx.h"

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

