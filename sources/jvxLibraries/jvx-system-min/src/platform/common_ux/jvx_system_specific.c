#include "jvx_system.h"

jvxCBool JVX_FILE_EXISTS(const char* fpath)
{
	struct stat buffer;   
  	if(stat (fpath, &buffer) == 0)
		return c_true;
	return c_false;
}

jvxCBool JVX_FILE_REMOVE(const char* fName)
{
	int err = unlink (fName);
	if(err == 0)
	{
		return c_true;
	}
	return c_false;
}

/*
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
*/

jvxCBool JVX_DIRECTORY_EXISTS(const char* absolutePath)
{
	if (access(absolutePath, 0) == 0)
	{
		struct stat status;
		stat(absolutePath, &status);
		return (status.st_mode & S_IFDIR) != 0;
	}

	return false;
}
