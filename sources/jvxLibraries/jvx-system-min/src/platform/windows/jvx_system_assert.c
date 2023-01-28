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
		// Determine stack
		FILE* fPtr = NULL;
		JVX_FOPEN(fPtr, "jvx_assert_log_crash.txt", "w");
		if (fPtr)
		{
			printf("Debug assertion tracked, refer to file <jvx_assert_log_crash.txt>");
			if (errtext)
			{
				fprintf(fPtr, "Output before crash: %s<%i>", errtext, errid);
			}
			else
			{
				fprintf(fPtr, "Silent crash: <%i>", errid);
			}
			fclose(fPtr);
		}
		if (errtext)
		{
			printf("Output before crash: %s<%i>", errtext, errid);
		}
		else
		{
			printf("Silent crash: <%i>", errid);
		}
		assert(0);
	}
}

#ifdef JVX_OS_WINDOWS
void JVX_ASSERT(jvxCBool cond)
{
	if (!cond)
	{
		__debugbreak();
	}
	assert(cond);
}
#endif

#endif
#endif

