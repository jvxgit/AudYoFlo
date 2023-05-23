#ifdef JVX_RS232_ACTIVE

	#ifdef JVX_OS_WINDOWS
		#include "jvxTRs232Win32.h"
	#endif

	#ifdef JVX_OS_LINUX
		#include "jvxTRs232Glnx.h"
	#endif
	
#endif