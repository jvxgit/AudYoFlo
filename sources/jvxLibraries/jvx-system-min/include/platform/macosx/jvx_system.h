#ifndef __JVX_SYSTEM_H__MACOSX__
#define __JVX_SYSTEM_H__MACOSX__

#include <sys/types.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <sys/utsname.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/un.h>

/*
 * Discussion regarding time issues in Mac OS X:
 * https://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x
 */
// Define the operating system
#define JVX_OS_MACOSX

#include "../../jvx_system_defines.h"

#include "../common_ux/jvx_system_types_ux.h"
#include "../common_ux/jvx_system_helpers_ux.h"

// For all conditional waits, we need to specify the right clock source
#define JVX_GLOBAL_CLOCK_SOURCE CLOCK_MONOTONIC
// CLOCK_BOOTTIME
// CLOCK_MONOTONIC
// CLOCK_REALTIME

#include "../common_ux/jvx_system_time_ux.h"
#include "../common_ux/jvx_system_threads_ux.h"
#include "../common_ux/jvx_system_mutex_ux.h"
#include "../common_ux/jvx_system_compute_ux.h"

#include "../../jvx_system_includes.h"

#endif
