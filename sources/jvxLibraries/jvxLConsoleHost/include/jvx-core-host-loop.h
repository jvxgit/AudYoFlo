#ifndef __JVX_CORE_HOST_LOOP_H__
#define __JVX_CORE_HOST_LOOP_H__

#include "jvx.h"

void jvx_core_host_loop(int argc, char* argv[], IjvxEventLoop_frontend_ctrl** fldAddFes = nullptr, jvxSize numAddFes = 0);
jvxErrorType jvx_core_host_loop_stop();
jvxErrorType jvx_core_host_loop_trigger_invite(IjvxExternalModuleFactory* trig_this, jvxBool blockedRun);
jvxErrorType jvx_core_host_loop_trigger_uninvite(IjvxExternalModuleFactory* trig_this, jvxBool blockedRun);
#endif
