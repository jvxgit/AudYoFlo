#include "jvx_platform.h"
#include "jvx.h"

#include <pwd.h>

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

