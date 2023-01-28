#include "jvx_system.h"

jvxCBool JVX_FILE_EXISTS(const char* fpath)
{
	DWORD dwAttrib = GetFileAttributes(fpath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

/*
jvxCBool JVX_FOLDER_EXISTS(const char* fpath)
{
	DWORD dwAttrib = GetFileAttributes(fpath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
*/

jvxCBool JVX_FILE_REMOVE(const char* fName)
{
	if (JVX_FILE_EXISTS(fName))
	{
		DeleteFile(fName);
		return c_true;
	}
	return c_false;
}

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

jvxCBool JVX_DIRECTORY_EXISTS(const char* absolutePath)
{
	if (_access_s(absolutePath, 0) == 0)
	{
		struct _stat status;
		_stat(absolutePath, &status);
		return (status.st_mode & S_IFDIR) != 0;
	}

	return false;
}
