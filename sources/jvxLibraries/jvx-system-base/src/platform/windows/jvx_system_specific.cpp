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
	JVX_CREATE_PROCESS_RESULT res = CreateProcess(NULL, (char*)cmdWithArgs.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &procHandle.info, &procHandle.processInfo);
	if (res == JVX_CREATE_PROCESS_SUCCESS)
	{	
		std::cout << "External process successfully started with command line <" << cmdWithArgs << ">" << std::endl;
	}
	else
	{
		std::cout << "Faiuled to start external process with command line <" << cmdWithArgs << ">" << std::endl;
	}
	return res;
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

#include <io.h>
#include <psapi.h>
#include <tchar.h>
#include <strsafe.h>

std::string JVX_FILENAME_FROM_FILE(FILE* hFile) 
{
	std::string retVal;


	HANDLE ff = (HANDLE)_get_osfhandle(_fileno(hFile));
	BOOL bSuccess = FALSE;
	TCHAR pszFilename[MAX_PATH + 1];
	HANDLE hFileMap;

	// Get the file size.
	DWORD dwFileSizeHi = 0;
	DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

	  if( dwFileSizeLo == 0 && dwFileSizeHi == 0 )
  {
    
     return retVal;
  }

  // Create a file mapping object.
  hFileMap = CreateFileMapping(ff,
                    NULL, 
                    PAGE_READONLY,
                    0, 
                    1,
                    NULL);

  if (hFileMap) 
  {
    // Create a file mapping to get the file name.
    void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

    if (pMem) 
    {
      if (GetMappedFileName (GetCurrentProcess(), 
                             pMem, 
                             pszFilename,
                             MAX_PATH)) 
      {

        // Translate path with device name to drive letters.
        TCHAR szTemp[MAX_PATH];
        szTemp[0] = '\0';

        if (GetLogicalDriveStrings(MAX_PATH -1, szTemp))
        {
          TCHAR szName[MAX_PATH];
          TCHAR szDrive[3] = TEXT(" :");
          BOOL bFound = FALSE;
          TCHAR* p = szTemp;

          do 
          {
            // Copy the drive letter to the template string
            *szDrive = *p;

            // Look up each device name
            if (QueryDosDevice(szDrive, szName, MAX_PATH))
            {
              size_t uNameLen = _tcslen(szName);

              if (uNameLen < MAX_PATH) 
              {
                bFound = _tcsnicmp(pszFilename, szName, uNameLen) == 0
                         && *(pszFilename + uNameLen) == _T('\\');

                if (bFound) 
                {
                  // Reconstruct pszFilename using szTempFile
                  // Replace device path with DOS path
                  TCHAR szTempFile[MAX_PATH];
                  StringCchPrintf(szTempFile,
                            MAX_PATH,
                            TEXT("%s%s"),
                            szDrive,
                            pszFilename+uNameLen);
                  StringCchCopyN(pszFilename, MAX_PATH+1, szTempFile, _tcslen(szTempFile));
                }
              }
            }

            // Go to the next NULL character.
            while (*p++);
          } while (!bFound && *p); // end of string
        }
      }
      bSuccess = TRUE;
      UnmapViewOfFile(pMem);
    } 

    CloseHandle(hFileMap);
  }
  retVal = pszFilename;
  // return(bSuccess);
  return retVal;
}

/* This functions reads a presentation of time which may be related to PTP or NTP synchronization 
 * and returns a unique timestamp given as a float value of seconds during the curent day.
 * E.g., at time 0:01, this timestamp should be 60.00000.
 */
jvxData 
JVX_TIMESTAMP_DATA_SECS_FROM_DATETIME(jvxBool verboseOn)
{
	FILETIME ft;
	GetSystemTimePreciseAsFileTime(&ft);

	// Link regarding PTP clock in Windows 11:
	// https://www.msxfaq.de/netzwerk/grundlagen/ptp_precision_time_protocol.htm

	// Application PTPSync which seems to work rather well:
	// https://github.com/GridProtectionAlliance/PTPSync?tab=readme-ov-file

	// Command: "w32tm / query / status / verbose" -> Ausgabe Status Systemclock

	// Commands, um an einen PTP Server zu binden: 
	// "w32tm /config /manualpeerlist:"<PTP- oder NTP-Server>" /syncfromflags:manual /update
	// "w32tm / resync"
	//

	// Commands to activate PTP in registry:
	// reg add HKLM\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\PtpClient /v Enabled /t REG_DWORD /d 1 /f
	// reg add HKLM\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\PtpClient /v InputProvider /t REG_DWORD /d 1 /f
	// reg add HKLM\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\PtpClient /v PtpMasters /t REG_SZ /d "192.168.178.200" /f
	// reg add HKLM\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\PtpClient /v DllName /t REG_SZ /d "%systemroot%\system32\ptpprov.dll" /f
	// reg add HKLM\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\PtpClient /v AnnouncePollInterval /t REG_DWORD /d 4000 /f
	// reg add HKLM\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\PtpClient /v DelayPollInterval /t REG_DWORD /d 16000 /f
	// 
	// reg add HKLM\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\NtpClient /v Enabled /t REG_DWORD /d 0 /f
	// reg add HKLM\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\VMICTimeProvider /t REG_DWORD /v Enabled /d 0 /f
	// 
	// net stop w32time
	// w32tm /unregister
	// w32tm / register
	// net start w32time

	// Regedit: HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\NtpClient Enabled auf "0"

			// Commands, um Systemfeedback einzuholen:
	// w32tm /query /status /verbose
	// w32tm /query /configuration

	// Nach perplexity scheint Ptpclient trotz aller Versprechungen auf meinem System nicht verfügbar zu sein.
	// "Die native PTP-Unterstützung ist auf Ihrem System aktuell nicht nutzbar."

	// Prüfen der Optionen mittels Netzwerkkarte:
	// GetInterfaceSupportedTimestampCapabilities 
	// GetInterfaceActiveTimestampCapabilities 

	// ===================================== SUMMARY ==================================
	// As of now, I have learned today: 
	// 1) PTP onboard sync does not work on Windows 11 in a reliable way. Most of the docs explaining how it is done
	//    seem to repair to older versions of Windows, e.g. Windows 10 etc. The proble is that all PtpClient entries setup
	//    on a PC will confuse the w32tim service on start such that the service does not run. By unregistering and registering the service,
	//    the w32time service can be started. hiwever, the PtpClient entries in the registry will be removed in this case.
	// 2) We can inst PTPSync instead. To do so, we add a configuration with a reference to the existing PTP server,
	//    e.g., ConnectionString = FileName=ptpd.exe; Arguments={-b {E5B70792-17E9-4122-89D0-0AB1B8C5B4D3} -V -g -u 192.168.178.200}; ForceKillOnDispose=True
	//    This will output info about received information on the console debug output.
	// 3) If we run w32time in parallel to PTPSync, the synchronization may be broken since NTP and PTP synchronization are active 
	//	  at the same time.
	// 4) We can deactivate w32time service by running <net stop w32time> and restart it later by running <net start w32time>
	// 5) We can run the MS services app to find out the status of synchronization or <w32tm /query /status>

	// FILETIME in 100-Nanosekunden-Intervallen seit 1601-01-01
	ULARGE_INTEGER uli;
	uli.LowPart = ft.dwLowDateTime;
	uli.HighPart = ft.dwHighDateTime;
	
	if (verboseOn)
	{
		std::cout << "Timestamp raw data: " << uli.QuadPart << std::endl;
	}

	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&ft, &sysTime);
	jvxSize wYear = sysTime.wYear;
	jvxSize wMonth = sysTime.wMonth;
	jvxSize wDay = sysTime.wDay;
	jvxSize wHour = sysTime.wHour;
	jvxSize wMin = sysTime.wMinute;
	jvxSize wSec = sysTime.wSecond;
	jvxSize wMSecs = sysTime.wMilliseconds;

	FILETIME ft_round;
	SystemTimeToFileTime(&sysTime, &ft_round);

	ULARGE_INTEGER uli_round;
	uli_round.LowPart = ft_round.dwLowDateTime;
	uli_round.HighPart = ft_round.dwHighDateTime;
	jvxSize nano100 = uli.QuadPart - uli_round.QuadPart;
	jvxData wMicroSecs = ((jvxData)nano100) * 0.1;

	if (verboseOn)
	{
		std::cout << __FUNCTION__ << "Derived current moment in time: " <<
			"Year: " << wYear << "; " <<
			"Month: " << wMonth << "; " <<
			"Day: " << wDay << "; " <<
			"Hour: " << wHour << "; " <<
			"Minute: " << wMin << "; " <<
			"Seconds: " << wSec << "; " <<
			"Millisecs: " << wMSecs << "; " <<
			"Micros: " << wMicroSecs << ". " <<
			std::endl;
	}

	jvxData millSecsToday = wHour;
	millSecsToday *= 60;
	millSecsToday += wMin;
	millSecsToday *= 60;
	millSecsToday += wSec;
	millSecsToday *= 1000;
	millSecsToday += wMSecs;

	jvxData secsToday = millSecsToday * 0.001;
	secsToday += wMicroSecs * 0.000001;

	return secsToday;
}