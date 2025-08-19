#ifdef _DEBUG

#ifndef __JVX_ASSERT_H__
#define __JVX_ASSERT_H__

#include <stdio.h>
#include <assert.h>
#include "jvx_system.h"

void JVX_ASSERT_X(const char* errtext, jvxInt32 errid, jvxCBool condition)
{
	if(!condition)
	{
		assert(0);
	}
}

void JVX_ASSERT(jvxCBool cond)
{
	//if (!cond)
	//{
	//	__debugbreak();
	//}
	assert(cond);
}

#endif
#endif


