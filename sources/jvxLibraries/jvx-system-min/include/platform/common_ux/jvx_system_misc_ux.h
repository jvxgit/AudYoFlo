#ifndef _JVX_SYSTEM_MISC_UX_H__
#define _JVX_SYSTEM_MISC_UX_H__

#include <stdlib.h> 

JVX_STATIC_INLINE int JVX_GETENVIRONMENTVARIABLE(char* var,char* bufRet,int ll)
{
	const char* varP = getenv(var);
	memset(bufRet, 0, sizeof(char)*ll);
	if(varP)
	{
		size_t sz = strlen(varP);
		memcpy(bufRet, varP, sizeof(char)*sz);
		return(1);
	}
	return(0);
}

#define JVX_SETENV_OVERWRITE 1
#define JVX_SETENV_NO_OVERWRITE 0

#define JVX_SETENVIRONMENTVARIABLE(var, value, overwrite) setenv(var,  value, overwrite)

#endif
