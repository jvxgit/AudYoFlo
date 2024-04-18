#ifndef _JVX_SYSTEM_MISC_H__
#define _JVX_SYSTEM_MISC_H__

// These macros need to be part of the C environment

#define JVX_GETENVIRONMENTVARIABLE(a,b,c) GetEnvironmentVariableA(a, b, c)
#define JVX_SETENVIRONMENTVARIABLE(a, b, overwrite_not_in_mswin) SetEnvironmentVariableA(a, b)

#endif