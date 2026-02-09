#include "jvx_platform.h"
#include "jvx.h"

#include <pwd.h>

JVX_CREATE_PROCESS_RESULT JVX_CREATE_PROCESS_WITH_ARGS(JVX_CREATE_PROCESS_HANDLE& procHandle, const std::string& cmd, const std::list<std::string>& args) 
{
	jvxSize cnt = 0;
	char** argV = nullptr;
	JVX_SAFE_ALLOCATE_FIELD(argV, char*, args.size()+2);
	
	argV[cnt] = (char*)cmd.c_str();
	cnt++;
	for(auto elm: args)
	{
	  argV[cnt] = (char*)elm.c_str();
		cnt++;
	}
	argV[cnt] = nullptr;

#if defined(JVX_OS_ANDROID)	

	#if JVX_ANDROID_API_VERSION >= 28
		return posix_spawn(&procHandle, (char*)cmd.c_str(), NULL, NULL, argV, NULL);
	#else

		// https://stackoverflow.com/questions/63459129/i-am-getting-compilation-error-for-posix-spawnp-function-on-android-ndk
		assert(0);
		return JVX_CREATE_PROCESS_FAILED;
	#endif
	
#else
	return posix_spawn(&procHandle, (char*)cmd.c_str(), NULL, NULL, argV, NULL);
#endif
}

std::string JVX_GET_CURRENT_MODULE_PATH(void* ptrIdentify)
{
	/*
	https://man7.org/linux/man-pages/man3/dladdr.3.html 
	*/
	std::string retVal;
	Dl_info info;
    if (dladdr(ptrIdentify, &info)) 
	{
		// Loaded from DLL
        retVal = info.dli_fname;
    }
	else
	{
		// I am an executable
		char arg1[20] = { 0 };
		char path[JVX_MAXSTRING + 1] = {0};
		sprintf( arg1, "/proc/%d/exe", getpid() );
		ssize_t length = readlink( arg1, path, JVX_MAXSTRING );
		retVal = std::string(path, length);
	}
	return retVal;
}

// https://bytes.com/topic/c/answers/719349-getting-desktop-home-folder-linux
std::string 
JVX_GET_USER_DESKTOP_PATH()
{
	std::string oFolder_user = "UNSET"; 
	passwd *pw = getpwuid(getuid()); 
	if (pw) 
	{
		oFolder_user = pw->pw_dir;
	}
	return oFolder_user;
}

std::string JVX_FILENAME_FROM_FILE(FILE* hFile) 
{
  int fd;
  char fd_path[4096];
  char filename[4096];
  std::string retVal;
  ssize_t n;

  fd = fileno(hFile);
  sprintf(fd_path, "/proc/self/fd/%d", fd);
  n = readlink(fd_path, filename, sizeof(filename) - 1);
  if (n < 0)
      return retVal;
  
  filename[n] = '\0';
  retVal = filename;
  return retVal;
}

/* This functions reads a presentation of time which may be related to PTP or NTP synchronization 
 * and returns a unique timestamp given as a float value of seconds during the curent day.
 * E.g., at time 0:01, this timestamp should be 60.00000.
 */
jvxData 
JVX_TIMESTAMP_DATA_SECS_FROM_DATETIME(jvxBool verboseOn)
{
	return 0;
}