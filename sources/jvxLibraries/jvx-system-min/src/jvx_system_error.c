#include "jvx_system.h"

const char* jvxErrorType_txt(jvxSize id)
{
	if(id < (int)JVX_ERROR_LIMIT)
	{
		return(jvxErrorType_str[id].friendly);
	}
	return("Unknown error type");
}

const char* jvxErrorType_descr(jvxSize id)
{
	if(id < (int)JVX_ERROR_LIMIT)
	{
		return(jvxErrorType_str[id].full);
	}
	return("Unknown error type");
}

jvxErrorType jvxErrorType_decode(const char* txt)
{
	jvxSize i;
	for (i = 0; i < (int)JVX_ERROR_LIMIT; i++)
	{
		if (strcmp(jvxErrorType_str[i].friendly, txt) == 0)
		{
			return (jvxErrorType)i;
		}
		if (strcmp(jvxErrorType_str[i].full,txt) == 0)
		{
			return (jvxErrorType)i;
		}
	}
	return JVX_ERROR_LIMIT;
}
