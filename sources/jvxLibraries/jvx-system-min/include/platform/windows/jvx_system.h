#ifndef __JVX_SYSTEM_H___WINDOWS__
#define __JVX_SYSTEM_H___WINDOWS__

#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2bth.h>
#include <afunix.h>
#include <windows.h>
#include <math.h>
#include <direct.h> 
#include <inttypes.h>
#include <assert.h>

#define JVX_OS_WINDOWS

// ============================================================
// Include another system header to the base of everything
// ============================================================
#define INCF(F) INCF_(F)
#define INCF_(F) #F
#ifdef JVX_ADDITIONAL_SYSTEM_INCLUDE_HEADER
#include INCF(JVX_ADDITIONAL_SYSTEM_INCLUDE_HEADER)
#endif
// ============================================================

#include "../../jvx_system_defines.h"

#include "jvx_system_types.h"
#include "jvx_system_helpers.h"
#include "jvx_system_time.h"
#include "jvx_system_threads.h"
#include "jvx_system_mutex.h"
#include "jvx_system_compute.h"
#include "jvx_system_misc.h"

#include "../../jvx_system_includes.h"



/*
-> No longer used:
#define JVX_STILL_ACTIVE STILL_ACTIVE
#define JVX_THREAD_BACKGROUND_BEGIN(tHdl) SetThreadPriority(tHdl, THREAD_MODE_BACKGROUND_BEGIN)
#define JVX_THREAD_BACKGROUND_END(tHdl) SetThreadPriority(tHdl, THREAD_MODE_BACKGROUND_END)
*/


#endif
