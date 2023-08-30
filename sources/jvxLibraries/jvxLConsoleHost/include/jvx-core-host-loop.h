#ifndef __JVX_CORE_HOST_LOOP_H__
#define __JVX_CORE_HOST_LOOP_H__

#include "jvx.h"

void jvx_core_host_loop(int argc, char* argv[]);
jvxErrorType jvx_core_host_loop_stop();
jvxErrorType jvx_core_host_loop_trigger_invite(IjvxExternalModuleFactory* trig_this);
jvxErrorType jvx_core_host_loop_trigger_uninvite(IjvxExternalModuleFactory* trig_this);
#endif
