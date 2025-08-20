#include "jvx_system.h"

jvxCBool JVX_FILE_EXISTS(const char* fpath)
{
	assert(0);
	return c_false;
}

jvxCBool JVX_FILE_REMOVE(const char* fName)
{
	assert(0);
	return c_false;
}

jvxCBool JVX_GETCURRENTDIRECTORY(char* destBuf, jvxSize szBytes)
{
	assert(0);
	return c_false;
}

jvxCBool JVX_DIRECTORY_EXISTS(const char* dirname)
{
	assert(0);
	return c_false;
}

// ===========================================================================================

float strtof(const char *nptr, char **endptr)
{
    return (float)strtod(nptr, endptr);
}

int fpclassify(float x) 
{
    if (isnan(x)) return FP_NAN;
    if (isinf(x)) return FP_INFINITE;
    if (x == 0.0f) return FP_ZERO;
    if (fabsf(x) < FLT_MIN) return FP_SUBNORMAL;
    return FP_NORMAL;
}

