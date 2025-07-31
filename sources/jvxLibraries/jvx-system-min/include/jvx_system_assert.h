#ifndef __JVX_SYSTEM_ASSERT_H__
#define __JVX_SYSTEM_ASSERT_H__

JVX_SYSTEM_LIB_BEGIN

#ifdef JVX_OS_WINDOWS
#include <intrin.h>
#endif

#ifdef _DEBUG

void JVX_ASSERT_X(const char* errtext, jvxInt32 errid, jvxCBool condition);

#else
#define JVX_ASSERT_X(a, b, c) 
#endif

#ifdef _DEBUG

#ifdef JVX_OS_WINDOWS
void JVX_ASSERT(jvxCBool cond);
#else
#define JVX_ASSERT(COND) \
	assert(COND);
#endif

#else

#define JVX_ASSERT(COND) 

#endif

JVX_SYSTEM_LIB_END

#endif
