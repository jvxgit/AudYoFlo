#ifndef __AYF_COMPONENT_LIB_TYPES_H__
#define __AYF_COMPONENT_LIB_TYPES_H__

#include "jvx_system_types.h"

typedef enum 
{
	AYF_BUFFERS_INTERLEAVED,
	AYF_BUFFERS_NONINTERLEAVED,
	AYF_BUFFERS_INVALID
} ayfBufferInterleaveType;

typedef enum 
{
	AYF_VOID_PVOID_ID_STARTED,
	AYF_VOID_PVOID_ID_REPORT_NO_BINDING,
	AYF_VOID_PVOID_ID_BEFORE_START,
	AYF_VOID_PVOID_ID_BEFORE_STOP,
	AYF_VOID_PVOID_ID_STOPPED,
	AYF_VOID_PVOID_ID_TERMINATE
} ayfVoidPvoidDefinition;

typedef void (*void_pvoid_callback)(ayfVoidPvoidDefinition voidDef, jvxHandle* priv, jvxHandle* purp);

struct ayfInitConnectStruct
{
	jvxSize ayfIdentsPtr[2];
	const char* fNameIniPtr;
	const char* fNameDllProxr;
	void_pvoid_callback fptr_multi_purpose;
	void* priv;
	jvxCBool doNotLoadProxy;
};

struct ayfInitParamStruct
{
	int numInChans = -1;
	int numOutChans = -1;
	int bSize = -1;
	int sRate = -1;
	ayfBufferInterleaveType ilTp = ayfBufferInterleaveType::AYF_BUFFERS_INVALID;
	int passthroughMode = -1;
};

#endif
