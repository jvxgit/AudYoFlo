#include "jvxLibHost.h"

jvxErrorType
jvxLibHost::periodic_trigger()
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (synchronize_thread_enter())
	{
		if (init_done)
		{
			
		}
		synchronize_thread_leave();
	}
	return res;
}