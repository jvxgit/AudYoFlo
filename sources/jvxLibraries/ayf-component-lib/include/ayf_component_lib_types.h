#ifndef __AYF_COMPONENT_LIB_TYPES_H__
#define __AYF_COMPONENT_LIB_TYPES_H__

#include "jvx_system_types.h"

typedef enum
{
	AYF_VOID_PVOID_ID_STARTED,
	AYF_VOID_PVOID_ID_REPORT_NO_BINDING
}ayfVoidPvoidDefinition;

typedef void (*void_pvoid_callback)(jvxSize id, jvxHandle* priv, jvxHandle* purp);

#endif
