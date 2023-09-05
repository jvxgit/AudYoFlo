#ifdef JVX_OS_WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "jvx-core-host-loop.h"

int main(int argc, char* argv[])
{
	jvx_core_host_loop(argc, argv);

#ifdef JVX_OS_WINDOWS
	_CrtDumpMemoryLeaks();
#endif
	return(0);
}