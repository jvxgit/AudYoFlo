#ifndef __AYF_COMPONENT_LIB_TYPES_H__
#define __AYF_COMPONENT_LIB_TYPES_H__

#include "jvx_system_types.h"

typedef enum 
{
	AYF_BUFFERS_INTERLEAVED,
	AYF_BUFFERS_NONINTERLEAVED
} ayfBufferInterleaveType;

typedef enum 
{
	AYF_VOID_PVOID_ID_STARTED,
	AYF_VOID_PVOID_ID_REPORT_NO_BINDING
} ayfVoidPvoidDefinition;

typedef void (*void_pvoid_callback)(ayfVoidPvoidDefinition voidDef, jvxHandle* priv, jvxHandle* purp);

struct ayfInitConnectStruct
{
	jvxSize ayfIdentsPtr[2];
	const char* fNameIniPtr;
	const char* fNameDllProxr;
	void_pvoid_callback fptr;
	void* priv;
};

struct ayfInitParamStruct
{
	int numInChans;
	int numOutChans;
	int bSize;
	int sRate;
	ayfBufferInterleaveType ilTp;
	int passthroughMode;
};

#endif
