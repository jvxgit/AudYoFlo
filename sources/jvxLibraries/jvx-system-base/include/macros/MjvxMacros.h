#ifndef _MJVX_MACROS_H__
#define _MJVX_MACROS_H__

//! Assign return value to a pointer - if the pointer is non-null
#define JVX_PTR_SAFE_ASSIGN(retVal, assignme) if (retVal) *retVal = assignme

//! Check status res and run operation if res is JVX_NO_ERROR. Operation agains sets res
#define JVX_RUN_ON_NO_ERROR(res, operation) if(res == JVX_NO_ERROR)	res = operation;

#endif
