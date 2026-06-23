#ifndef __AYF_COMPONENT_LIB_TYPES_H__
#define __AYF_COMPONENT_LIB_TYPES_H__

#include "../../jvx_system_defines.h"

#include "jvx_system_config.h"
#include "jvx_system_types.h"
#include "jvx_system_helpers.h"
#include "jvx_system_error_types.h"
#include "jvx_system_dataformats.h"
// #include "jvx_system_time.h"
//#include "jvx_system_threads.h"
//#include "jvx_system_mutex.h"
//#include "jvx_system_compute.h"
//#include "jvx_system_misc.h"

// #include "../../jvx_system_includes.h"

// typedef jvxErrorType;

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
	AYF_VOID_PVOID_ID_TERMINATE,
	AYF_VOID_PVOID_ID_TRANSFER_OCON, // <- jvxLinkDataDescriptor_con_params*
	AYF_VOID_PVOID_ID_TEST_ICON, // <- jvxLinkDataDescriptor_con_params*
	AYF_VOID_PVOID_ID_PROC_PARAMS // <- ayfInitParamStruct*
} ayfVoidPvoidDefinition;

typedef jvxErrorType (*void_pvoid_callback)(ayfVoidPvoidDefinition voidDef, jvxHandle* priv, jvxHandle* purp);

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
	int numInChans;
	int numOutChans;
	int bSize;
	int sRate;
	jvxDataFormat format;
	ayfBufferInterleaveType ilTp;
	int passthroughMode;
	jvxErrorType lastError;
};

#define AYF_INIT_PARAM_STRUCT_RESET(paramStr) \
	paramStr.numInChans = -1; \
	paramStr.numOutChans = -1; \
	paramStr.bSize = -1; \
	paramStr.sRate = -1; \
	paramStr.format = JVX_DATAFORMAT_NONE; \
	paramStr.ilTp = AYF_BUFFERS_INVALID;

#endif
