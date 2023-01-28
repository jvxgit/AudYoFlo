#include "jvx.h"

#include <pwd.h>
#include <mach-o/dyld.h>

std::string JVX_GET_CURRENT_MODULE_PATH(void* ptrIdentify)
{
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
		char path[JVX_MAXSTRING + 1] = {0};
		uint32_t size = JVX_MAXSTRING;
		if (_NSGetExecutablePath(path, &size) == 0)
		{
			retVal = path;
		}
		else
		{
			retVal = "<path var is too short!>";
		}
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

