#ifndef __AYF_SDK_LOCAL_HEADERS_H__
#define __AYF_SDK_LOCAL_HEADERS_H__

#define STRINGIZE_(a) #a
#define STRINGIZE(a) STRINGIZE_(a)

#define JVX_COMPONENT_ACCESS_CALLING_CONVENTION 
#include "jvx_sdk.h"
#include "jvx.h"
#include "flutter_native_host_config.h"
#include STRINGIZE(AYF_PROJECT.h)

#endif